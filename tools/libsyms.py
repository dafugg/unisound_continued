"""Locate CS.LIB modules' code and data in UNISOUND.COM -> syms.json {hexaddr: name}"""
import sys, os, re, json
D = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, D)
import omf
lib = open(os.environ.get("CSLIB", f"{D}/../toolchain/tc30/LIB/CS.LIB"), "rb").read(); exe = open(os.environ.get("UNISOUND_COM", f"{D}/../orig/UNISOUND.COM"), "rb").read()
CODE = (0x9185 - 0x100, 0xb364 - 0x100); DATA = (0xb364 - 0x100, len(exe))
syms = {}
def find(buf, msk, lo, hi):
    pat = b''.join(re.escape(bytes([c])) if m == 1 else b'.' for c, m in zip(buf, msk))
    return [h.start() + lo for h in re.finditer(pat, exe[lo:hi], re.S)]
for name, content in omf.modules(lib):
    pubs = omf.modules.pubs
    for seg, rng in (('_TEXT', CODE), ('_DATA', DATA)):
        if seg not in content: continue
        buf, msk = content[seg]
        if len(buf) < (12 if seg == '_TEXT' else 6) or sum(1 for m in msk if m == 1) < len(buf) * 0.6: continue
        hits = find(buf, msk, *rng)
        if len(hits) != 1: continue
        base = hits[0] + 0x100
        for nm, s, off in pubs:
            if s == seg: syms[f"{base+off:04x}"] = nm
json.dump(dict(sorted(syms.items())), open(f"{D}/../data/syms.json", "w"), indent=0)
print(len(syms), "symbols")
for k, v in sorted(syms.items()): print(k, v)
