"""Annotated disassembly of an author function in UNISOUND.COM.
Usage: fn.py ADDR            (hex, e.g. 3c21)   |   fn.py --list   (all functions)
Annotations: library symbol names, f_XXXX for author functions, g_XXXX for data,
string literal contents for immediates pointing at strings, switch tables as data."""
import sys, os, json, re
from capstone import Cs, CS_ARCH_X86, CS_MODE_16
D = os.path.dirname(os.path.abspath(__file__))
exe = open(os.environ.get("UNISOUND_COM", f"{D}/../orig/UNISOUND.COM"), "rb").read()
fm = json.load(open(f"{D}/../data/funcs.json")); funcs = {f["addr"]: f for f in fm["funcs"]}
syms = {int(k, 16): v for k, v in json.load(open(f"{D}/../data/syms.json")).items()}
DATA_LO, FILE_END = 0xb364, 0x100 + len(exe)
md = Cs(CS_ARCH_X86, CS_MODE_16)

def name(a):
    if a in syms: return syms[a]
    if a in funcs: return f"f_{a:04x}"
    return None

def cstr(a):
    if not (DATA_LO <= a < FILE_END): return None
    o = a - 0x100; e = exe.find(b'\0', o)
    if e < 0 or e - o > 300: return None
    s = exe[o:e]
    if len(s) == 0 or (a > DATA_LO and exe[o-1] != 0 and len(s) < 3): return None
    if all(32 <= c < 127 or c in (9, 10, 13) for c in s):
        return '"' + s.decode().replace('\\', '\\\\').replace('\n', '\\n').replace('\r', '\\r').replace('\t', '\\t').replace('"', '\\"') + '"'
    return None

def fix(i):
    ops = re.sub(r'0xffff([0-9a-f]{4})\b', r'0x\1', i.op_str)
    mn = {'cwde': 'cbw', 'cdq': 'cwd'}.get(i.mnemonic, i.mnemonic)
    return mn, ops

def annotate(i):
    notes = []
    for m in re.finditer(r'0x[0-9a-f]+', i.op_str):
        v = int(m.group(0), 16) & 0xffff
        if i.mnemonic in ('call', 'jmp') or i.mnemonic.startswith('j') or i.mnemonic.startswith('loop'):
            n = name(v)
            if n: notes.append(n)
            continue
        if v >= DATA_LO and v < 0xe000:
            mem = f'[{m.group(0)}]' in i.op_str or f'+ {m.group(0)}]' in i.op_str or f'- {m.group(0)}]' in i.op_str
            n = syms.get(v)
            s = None if mem else cstr(v)
            notes.append(s if s else (n or f"g_{v:04x}"))
    return ("   ; " + ", ".join(notes)) if notes else ""

def show(a):
    f = funcs[a]
    callers = ", ".join(f"{c:04x}" for c in f["callers"]) or "-"
    print(f"; f_{a:04x}  size {f['size']}  [{a:04x}..{f['end']:04x})  called from: {callers}")
    tabs = [tuple(t) for t in f["tables"]]
    p = a
    while p < f["end"]:
        t = next((t for t in tabs if t[0] == p), None)
        if t:
            ents = [exe[x-0x100] | exe[x-0xff] << 8 for x in range(t[0], t[1], 2)]
            print(f"{p:05x}  ; switch table: " + " ".join(f"{e:04x}" for e in ents)); p = t[1]; continue
        stop = min([t[0] for t in tabs if t[0] > p] + [f["end"]])
        moved = False
        for i in md.disasm(exe[p-0x100:stop-0x100], p):
            mn, ops = fix(i)
            print(f"{i.address:05x}  {i.bytes.hex():14s} {mn} {ops}{annotate(i)}")
            p = i.address + i.size; moved = True
        if p < stop and not moved or p < stop:
            print(f"{p:05x}  {exe[p-0x100]:02x}             db"); p += 1

if sys.argv[1] == '--list':
    for f in fm["funcs"]: print(f"f_{f['addr']:04x} size {f['size']:5d} callers {len(f['callers'])}")
else:
    show(int(sys.argv[1], 16))
