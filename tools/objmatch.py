"""Compare a compiled OBJ with UNISOUND.COM, function by function (fixup bytes masked).
Usage: objmatch.py FILE.OBJ START_ADDR [-v FUNC_ADDR]
The OBJ's _TEXT is taken to start at START_ADDR. The OBJ's public functions (in offset order) are
paired with the original functions from START_ADDR on, in order, so a function of the wrong size
doesn't hide the status of the ones after it. All functions must be non-static (static ones have no
public symbol). -v shows a side-by-side diff for one original function."""
import sys, os, json
D = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, D)
import omf
from capstone import Cs, CS_ARCH_X86, CS_MODE_16
exe = open(os.environ.get("UNISOUND_COM", f"{D}/../orig/UNISOUND.COM"), "rb").read()
funcs = json.load(open(f"{D}/../data/funcs.json"))["funcs"]
args = sys.argv[1:]; verbose = None
if '-v' in args:
    k = args.index('-v'); verbose = int(args[k+1], 16); del args[k:k+2]
obj = open(args[0], 'rb').read(); start = int(args[1], 16)
name, content = next(omf.modules(obj)); pubs = omf.modules.pubs
buf, msk = content['_TEXT']
pf = sorted((off, nm) for nm, seg, off in pubs if seg == '_TEXT')
spans = [(nm, off, (pf[k+1][0] if k+1 < len(pf) else len(buf))) for k, (off, nm) in enumerate(pf)]
orig = [f for f in funcs if f["addr"] >= start][:len(spans)]
md = Cs(CS_ARCH_X86, CS_MODE_16)
allok = True
for (nm, o0, o1), f in zip(spans, orig):
    a, size = f["addr"], f["size"]
    ob, om = buf[o0:o1], msk[o0:o1]
    ref = exe[a-0x100:a-0x100+size]
    bad = [i for i in range(min(len(ob), size)) if om[i] == 1 and ob[i] != ref[i]]
    ok = not bad and len(ob) == size
    allok &= ok
    pos = f"{a:04x}" if o0 == a - start else f"{a:04x} (obj offset {o0:#x} != {a-start:#x})"
    status = "MATCH" if ok else (f"{len(bad)} bytes differ, first at +{bad[0]:#x}" if bad else "same prefix") + \
        (f", size {len(ob)} vs {size}" if len(ob) != size else "")
    print(f"{nm:24s} f_{pos}: {status}")
    if verbose == a and not ok:
        x = list(md.disasm(bytes(ob), a)); y = list(md.disasm(ref, a))
        first = bad[0] if bad else min(len(ob), size)
        badset = set(bad)
        for i in range(max(len(x), len(y))):
            p, q = (x[i] if i < len(x) else None), (y[i] if i < len(y) else None)
            if (p or q).address - a + 16 < first: continue
            fl = '!!' if (p and q and (p.bytes != q.bytes) and any(j in badset for j in range(p.address-a, p.address-a+p.size))) or not p or not q or p.address != q.address else '  '
            ps = f"{p.address:05x} {p.mnemonic} {p.op_str}" if p else ""
            qs = f"{q.address:05x} {q.mnemonic} {q.op_str}" if q else ""
            print(f"{fl} {ps:44s} | {qs}")
if len(spans) > len(orig): print("extra functions in OBJ beyond the author code")
print("ALL MATCH" if allok else "NOT ALL MATCH")
