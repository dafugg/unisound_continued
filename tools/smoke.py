"""86Box smoke test (used by CI): run UNISOUND.COM on a few emulated sound cards with
tools/cardtest.py and check that each command's output contains the expected text.

Usage: smoke.py UNISOUND.COM OUTDIR MACHINE=BASEVM...
  MACHINE=BASEVM  a VM that boots to C:\\> on its own, such as one made by tools/freedos-vm.py, for
                  each machine name used in CASES (pnp: 486SP3C, pnp2: 486-VIP-IO2)
Run with the 86box-mcp server's Python (it needs box86mcp), with BOX86_BINARY set to an 86Box built
with -D AUTOMATION=ON; 86Box finds its ROMs as usual, or in $BOX86_ROMS. Exits 1 if a check fails.
"""
import json, os, subprocess, sys
from pathlib import Path

# (machine, card, [(command, [text its output must contain]), ...])
CASES = [
    ("pnp", "sb16_pnp", [
        ("UNISOUND", ["PnP card found: [CTL0025]", "Initialization done"]),
        ("UNISOUND /R", ["Reading card current settings", "ADD:220 OPL:388 MPU:330 IRQ:5"]),
        ("UNISOUND /VL0 /V5", ["SB Mixer [VOL:5 "]),                 # one-digit volumes (4928fd5)
        ("UNISOUND /DUMMY", ["Unknown option: /DUMMY"]),              # whole option names (e75475b)
        ("IF ERRORLEVEL 2 ECHO EL2", ["EL2"]),
        ("UNISOUND /VL100", ["Invalid value: /VL100 (0-99)"]),
        ("IF ERRORLEVEL 2 ECHO EL2", ["EL2"]),
    ]),
    # The second /NOPNP found nothing before the DSP flush fix (17868c1).
    ("pnp", "ess_es1688", [
        ("UNISOUND /NOPNP", ["Card detected: ES1688 at [220h]"]),
        ("UNISOUND /NOPNP", ["Card detected: ES1688 at [220h]"]),
    ]),
    # The 486SP3C's BIOS hangs on this card's PnP data (as the real board does), so it runs on the
    # 486-VIP-IO2, whose BIOS sets the card up first.
    ("pnp2", "azt2320", [
        ("UNISOUND", ["PnP card found: [AZT1008]", "Initialization done", "Card mode: [SBPro]"]),
        ("UNISOUND /R", ["ADD:220 WSS:534 OPL:388 IRQ:5 DMA:1/3 MPU:330/I9"]),
    ]),
]

com, out = sys.argv[1], Path(sys.argv[2])
bases = dict(a.split("=", 1) for a in sys.argv[3:])
os.environ.setdefault("BOX86MCP", "unused")  # cardtest.py only uses it to find 86Box's Docker wrapper
cardtest = Path(__file__).resolve().parent / "cardtest.py"
failed = 0
for machine, card, cases in CASES:
    cmds = ";".join(c for c, _ in cases)
    subprocess.run([sys.executable, "-I", str(cardtest), str(out), com,
                    f"{machine}={bases[machine]}", card, "-c", cmds], check=True)
    result = json.loads((out / machine / card / "result.json").read_text())
    got = {c["n"]: c for c in result["commands"]}
    for n, (cmd, wants) in enumerate(cases, 1):
        c = got.get(n)
        missing = [w for w in wants if c is None or w not in c["output"]]
        ok = c is not None and c["status"] == "ok" and not missing
        failed += not ok
        print(f"{'ok  ' if ok else 'FAIL'} {machine} {card}: {cmd}")
        if not ok:
            print("     status:", c["status"] if c else "not run", "| missing:", missing)
            if c:
                print("     " + c["output"].replace("\n", "\n     "))
print(f"{failed} check(s) failed" if failed else "all checks passed")
sys.exit(1 if failed else 0)
