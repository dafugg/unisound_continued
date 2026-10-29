import sys, re
sys.path.insert(0, __import__('os').path.dirname(__file__))
from omf import modules
lib = open(sys.argv[1], 'rb').read(); exe = open(sys.argv[2], 'rb').read()
found = 0; total = 0
for name, content in modules(lib):
    if '_TEXT' not in content: continue
    buf, msk = content['_TEXT']
    if len(buf) < 12: continue
    total += 1
    pat = b''.join(re.escape(bytes([c])) if m == 1 else b'.' for c, m in zip(buf, msk))
    hits = [h.start() for h in re.finditer(pat, exe, re.S)]
    if hits:
        found += 1
        print(f"{name:20s} {len(buf):5d}B  at {', '.join(hex(h+0x100) for h in hits[:3])}")
print(f"matched {found} of {total} library modules with code >=12 bytes", file=sys.stderr)
