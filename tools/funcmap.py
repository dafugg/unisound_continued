"""Build the function map of the author's code (0x369..0x9185) -> funcs.json.
Handles Borland switch jump tables (jmp cs:[bx+T]) placed in the code segment."""
import json, os, re
from capstone import Cs, CS_ARCH_X86, CS_MODE_16
D = os.path.dirname(os.path.abspath(__file__))
exe = open(os.environ.get("UNISOUND_COM", f"{D}/../orig/UNISOUND.COM"), "rb").read()
LO, HI = 0x369, 0x9185
md = Cs(CS_ARCH_X86, CS_MODE_16)
w = lambda a: exe[a-0x100] | exe[a-0xff] << 8

def linear(lo, hi, tables):
    """Disassemble [lo,hi) skipping known table ranges; returns instructions and newly found tables."""
    out, a, new = [], lo, {}
    while a < hi:
        t = next((t for t in tables if t[0] <= a < t[1]), None)
        if t: a = t[1]; continue
        stop = min([t[0] for t in tables if t[0] > a] + [hi])
        got = False
        for i in md.disasm(exe[a-0x100:stop-0x100], a):
            out.append(i); a = i.address + i.size; got = True
            m = re.search(r'cs:\[bx \+ (0x[0-9a-f]+)\]', i.op_str)
            if i.mnemonic == 'jmp' and m:
                new[int(m.group(1), 16)] = i.address
        if a < stop and not got or (a < stop):
            if a < stop: a += 1   # undecodable byte
    return out, new

tables = []
for _ in range(10):
    ins, new = linear(LO, HI, tables)
    added = False
    for T, jaddr in new.items():
        if any(t[0] == T for t in tables): continue
        # entries: words pointing back into code between the function start (<= jaddr) and T
        n = 0
        while T + 2*n < HI and LO <= w(T + 2*n) < T and w(T + 2*n) > jaddr - 0x2000: n += 1
        tables.append((T, T + 2*n)); added = True
    if not added: break
starts = set(); calls = {}
for j, i in enumerate(ins[:-1]):
    if i.bytes == b'\x55' and ins[j+1].bytes == b'\x8b\xec': starts.add(i.address)
for i in ins:
    if i.mnemonic == 'call' and i.op_str.startswith('0x'):
        calls.setdefault(int(i.op_str, 16) & 0xffff, []).append(i.address)
called_user = {t for t in calls if LO <= t < HI}
print("prologues", len(starts), "user call targets", len(called_user), "jump tables", len(tables))
print("call targets without prologue:", [hex(t) for t in sorted(called_user - starts)])
print("prologues never called directly:", [hex(t) for t in sorted(starts - called_user)])
fs = sorted(starts | called_user)
funcs = []
for k, a in enumerate(fs):
    e = fs[k+1] if k+1 < len(fs) else HI
    funcs.append({"addr": a, "end": e, "size": e - a,
                  "tables": [list(t) for t in tables if a <= t[0] < e],
                  "callers": sorted(calls.get(a, []))})
json.dump({"funcs": funcs, "tables": tables}, open(f"{D}/../data/funcs.json", "w"), indent=0)
print("functions", len(funcs))
print("largest:", [(hex(f['addr']), f['size']) for f in sorted(funcs, key=lambda f: -f["size"])[:8]])
