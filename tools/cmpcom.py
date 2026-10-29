"""Compare a rebuilt UNISOUND.COM with the original: size, first difference, differing ranges."""
import sys, os
a = open(sys.argv[1], 'rb').read(); b = open(os.environ.get('UNISOUND_COM', os.path.join(os.path.dirname(os.path.abspath(__file__)), '../orig/UNISOUND.COM')), 'rb').read()
print(f"size {len(a)} vs original {len(b)}")
if a == b: print("IDENTICAL"); sys.exit(0)
diffs = [i for i in range(min(len(a), len(b))) if a[i] != b[i]]
print(f"{len(diffs)} differing bytes in the common length")
rng = []; 
for i in diffs:
    if rng and i - rng[-1][1] <= 8: rng[-1][1] = i
    else: rng.append([i, i])
for s, e in rng[:15]: print(f"  {s+0x100:04x}-{e+0x100:04x}: {a[s:e+1][:16].hex()} vs {b[s:e+1][:16].hex()}")
if len(rng) > 15: print(f"  ... {len(rng)} ranges")
