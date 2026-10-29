"""pnpdecode.py RESULT_DIR...: turn TRACEPNP logs (port=value) into PnP register writes.
A write to the address port (279h, or the ESS 660h/800h) selects a register, a write to the
matching data port (A79h, 661h, 801h) writes it. Prints, per traced command, the writes with
the logical device (register 07h) they went to; card-level registers (< 30h) are marked."""
import json, re, sys
from pathlib import Path
DATA = {0xA79: 0x279, 0x661: 0x660, 0x801: 0x800}
def decode(text):
    sel = {}; ldn = None; out = []
    for p, v in re.findall(r"\b([0-9A-F]{4})=([0-9A-F]{2})\b", text):
        p, v = int(p, 16), int(v, 16)
        if p in DATA:
            r = sel.get(DATA[p])
            if r is None:
                continue
            if r == 7:
                ldn = v
            out.append((ldn, r, v))
        else:
            sel[p] = v
    return out
for d in sys.argv[1:]:
    for rj in sorted(Path(d).glob("*/*/result.json")):
        r = json.loads(rj.read_text())
        print(f"##### {rj.parent.relative_to(d)} {r.get('com','')}")
        for c in r["commands"]:
            o = c.get("output") or ""
            if "writes:" not in o:
                continue
            w = decode(o)
            keep = [x for x in w if x[1] in (0x28, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75) or x[1] == 0x30]
            print(f"> {c['cmd']}")
            for l in o.split("\n"):
                if "MPU:" in l or "exit code" in l:
                    print("   ", l.strip())
            print("    " + " ".join(("card" if reg < 0x30 else f"L{ldn}") + f":{reg:02X}={val:02X}" for ldn, reg, val in keep))
