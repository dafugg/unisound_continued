"""86Box smoke test (used by CI): run UNISOUND.COM on a few emulated sound cards with
tools/cardtest.py and check that each command's output contains the expected text.

Usage: smoke.py UNISOUND.COM BASEVM OUTDIR
  BASEVM  a VM that boots to C:\\> on its own, such as one made by tools/freedos-vm.py
Run with the 86box-mcp server's Python (it needs box86mcp), with BOX86_BINARY set to an 86Box built
with -D AUTOMATION=ON; 86Box finds its ROMs as usual, or in $BOX86_ROMS. Exits 1 if a check fails.
"""
import json, os, subprocess, sys
from pathlib import Path

# card: [(command, [text its output must contain]), ...]
CASES = {
    "sb16_pnp": [
        ("UNISOUND", ["PnP card found: [CTL0025]", "Initialization done"]),
        ("UNISOUND /R", ["Reading card current settings", "ADD:220 OPL:388 MPU:330 IRQ:5"]),
        ("UNISOUND /VL0 /V5", ["SB Mixer [VOL:5 "]),                 # one-digit volumes (4928fd5)
        ("UNISOUND /DUMMY", ["Unknown option: /DUMMY"]),              # whole option names (e75475b)
        ("IF ERRORLEVEL 2 ECHO EL2", ["EL2"]),
        ("UNISOUND /VL100", ["Invalid value: /VL100 (0-99)"]),
        ("IF ERRORLEVEL 2 ECHO EL2", ["EL2"]),
    ],
    # The second /NOPNP found nothing before the DSP flush fix (17868c1).
    "ess_es1688": [
        ("UNISOUND /NOPNP", ["Card detected: ES1688 at [220h]"]),
        ("UNISOUND /NOPNP", ["Card detected: ES1688 at [220h]"]),
    ],
}

com, base, out = sys.argv[1], sys.argv[2], Path(sys.argv[3])
os.environ.setdefault("BOX86MCP", "unused")  # cardtest.py only uses it to find 86Box's Docker wrapper
cardtest = Path(__file__).resolve().parent / "cardtest.py"
failed = 0
for card, cases in CASES.items():
    cmds = ";".join(c for c, _ in cases)
    subprocess.run([sys.executable, "-I", str(cardtest), str(out), com, f"pnp={base}", card, "-c", cmds],
                   check=True)
    result = json.loads((out / "pnp" / card / "result.json").read_text())
    got = {c["n"]: c for c in result["commands"]}
    for n, (cmd, wants) in enumerate(cases, 1):
        c = got.get(n)
        missing = [w for w in wants if c is None or w not in c["output"]]
        ok = c is not None and c["status"] == "ok" and not missing
        failed += not ok
        print(f"{'ok  ' if ok else 'FAIL'} {card}: {cmd}")
        if not ok:
            print("     status:", c["status"] if c else "not run", "| missing:", missing)
            if c:
                print("     " + c["output"].replace("\n", "\n     "))
print(f"{failed} check(s) failed" if failed else "all checks passed")
sys.exit(1 if failed else 0)
