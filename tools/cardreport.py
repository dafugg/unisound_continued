"""Summarize a cardtest.py results directory as one line per command per (platform, card).
Usage: cardreport.py RESULTS [--tsv]"""
import json, sys
from pathlib import Path

def key_lines(out: str) -> str:
    """The informative lines of one UNISOUND run (banner and rule dropped)."""
    if out is None:
        return "(no output file)"
    lines = [l.strip() for l in out.split("\n")]
    lines = [l for l in lines if l and not l.startswith("Universal ISA PnP") and not set(l) <= {"─"}]
    return " | ".join(lines)

root = Path(sys.argv[1])
for res in sorted(root.glob("*/*/result.json")):
    plat, card = res.parts[-3], res.parts[-2]
    r = json.loads(res.read_text())
    if r["status"] not in ("ok", "hang") or not r.get("commands"):
        print(f"{plat}\t{card}\t-\t{r['status']}\t{r.get('error', '')[:200]}")
        continue
    for c in r["commands"]:
        print(f"{plat}\t{card}\t{c['cmd']}\t{c['status']}\t{key_lines(c.get('output'))}")
