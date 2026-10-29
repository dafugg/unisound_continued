"""Run UNISOUND against 86Box sound cards.

For each (platform, card) pair: copy the platform's base VM (reflinked disk image, CMOS),
set the sound card in 86box.cfg, put UNISOUND.COM in C:\\T, boot, run each command with its
output redirected to a file, then copy the files back. A command that doesn't return to the
prompt in time is recorded as a hang, with the screen, and ends that run.

Usage: cardtest.py OUTDIR COM PLATFORM=BASEVM[,...] CARD[,...] [-j N] [-c CMD;CMD...] [-e VAR=VAL;...]
  PLATFORM=BASEVM  e.g. pnp=/path/vm-bc31   (base VMs must boot to C:\\> on their own)
  CARD             86Box internal sound card name, or "none"
  -c               commands to run (default: the baseline set below)
  -e               environment variables to SET before the commands
  -f               extra files to copy into C:\\T, comma-separated
Results: OUTDIR/<platform>/<card>/{result.json,<n>.TXT,screen-*.txt}
Run with the 86box-mcp server's Python (it needs box86mcp), with MTOOLS_DIR and BOX86MCP set.
"""
import json, os, re, shutil, subprocess, sys, time
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

BASELINE = ["UNISOUND /CL", "UNISOUND /R", "UNISOUND", "UNISOUND /R", "UNISOUND /NOPNP", "UNISOUND /CS"]
TIMEOUT_MS = 120000


def set_card(cfg: str, card: str) -> str:
    """Return cfg with the [Sound] section's sndcard set (or removed for "none")."""
    cfg = re.sub(r'\n\[Sound\]\n(?:[^\[\n][^\n]*\n|\n)*', '\n', cfg)
    if card != "none":
        cfg = cfg.rstrip('\n') + f"\n\n[Sound]\nsndcard = {card}\n"
    return cfg


def one(outdir: Path, com: Path, platform: str, base: Path, card: str, cmds, env, files=()) -> dict:
    """Run one (platform, card) pair in a child process; returns its result dict."""
    out = outdir / platform / card
    shutil.rmtree(out, ignore_errors=True)
    out.mkdir(parents=True)
    t0 = time.time()
    r = subprocess.run([sys.executable, "-I", __file__, "--child", str(out), str(com), str(base), card,
                        json.dumps(cmds), json.dumps(env), json.dumps(list(files))], capture_output=True, text=True,
                       timeout=60 * 30)
    try:
        res = json.loads((out / "result.json").read_text())
    except Exception:
        res = {"status": "harness-error", "stderr": r.stderr[-3000:]}
        (out / "result.json").write_text(json.dumps(res, indent=1))
    res["wall_s"] = round(time.time() - t0)
    print(f"{platform:6} {card:24} {res['status']:12} {res['wall_s']}s", flush=True)
    return res


def child(out: Path, com: Path, base: Path, card: str, cmds, env, files=()) -> None:
    if os.environ.get("MTOOLS_DIR"):
        os.environ["PATH"] = os.environ["MTOOLS_DIR"] + ":" + os.environ["PATH"]
    os.environ.setdefault("BOX86_BINARY", f"{os.environ['BOX86MCP']}/docker/86box.sh")
    from box86mcp import server, disk
    vm = out / "vm"
    vm.mkdir()
    subprocess.run(["cp", "--reflink=auto", str(base / "hdd.img"), str(vm / "hdd.img")], check=True)
    shutil.copytree(base / "nvr", vm / "nvr")
    (vm / "86box.cfg").write_text(set_card((base / "86box.cfg").read_text(), card))
    img = str(vm / "hdd.img")
    part = f"{img}@@{disk.partition_offset(img)}"
    mt_env = dict(os.environ, MTOOLS_SKIP_CHECK="1", MTOOLS_NO_VFAT="1")
    mt = lambda *a: subprocess.run(list(a), env=mt_env, capture_output=True, text=True)
    mt("mmd", "-i", part, "::T")
    mt("mcopy", "-o", "-i", part, str(com), "::T/UNISOUND.COM")
    for f in files:
        mt("mcopy", "-o", "-i", part, f, "::T/")
    res = {"card": card, "base": str(base), "commands": [], "status": "ok"}
    try:
        server.vm_start(str(vm), held=True)
    except Exception as e:
        res["status"] = "vm-start-failed"
        res["error"] = str(e)[-2000:]
        (out / "result.json").write_text(json.dumps(res, indent=1))
        return
    try:
        w = server.wait_for_text("C:\\>", 90000)
        if "TIMED OUT" in w:
            res["status"] = "no-boot"
            (out / "screen-boot.txt").write_text(server.screen_text())
            return
        server.dos_run("CD \\T", 10000)
        for k, v in env.items():
            server.dos_run(f"SET {k}={v}", 10000)
        for n, cmd in enumerate(cmds, 1):
            r = server.dos_run(f"{cmd} > {n}.TXT", TIMEOUT_MS)
            entry = {"n": n, "cmd": cmd}
            if "TIMED OUT" in r or "timed out" in r.lower():
                entry["status"] = "hang"
                res["status"] = "hang"
                (out / f"screen-{n}.txt").write_text(server.screen_text())
                res["commands"].append(entry)
                break
            entry["status"] = "ok"
            res["commands"].append(entry)
    except Exception as e:
        res["status"] = "harness-error"
        res["error"] = repr(e)[-2000:]
    finally:
        try:
            server.vm_stop()
        except Exception:
            pass
        for e in res["commands"]:
            mt("mcopy", "-o", "-n", "-i", part, f"::T/{e['n']}.TXT", str(out) + "/")
            f = out / f"{e['n']}.TXT"
            e["output"] = f.read_bytes().decode("cp437").replace("\r", "") if f.exists() else None
        (out / "result.json").write_text(json.dumps(res, indent=1))
        shutil.rmtree(vm, ignore_errors=True)


def main() -> None:
    a = sys.argv[1:]
    if a and a[0] == "--child":
        child(Path(a[1]), Path(a[2]), Path(a[3]), a[4], json.loads(a[5]), json.loads(a[6]), json.loads(a[7]))
        return
    jobs, cmds, env, files = 6, BASELINE, {}, []
    while "-j" in a:
        i = a.index("-j"); jobs = int(a[i + 1]); del a[i:i + 2]
    while "-c" in a:
        i = a.index("-c"); cmds = a[i + 1].split(";"); del a[i:i + 2]
    while "-e" in a:
        i = a.index("-e"); env = dict(x.split("=", 1) for x in a[i + 1].split(";")); del a[i:i + 2]
    while "-f" in a:
        i = a.index("-f"); files = [str(Path(x).resolve()) for x in a[i + 1].split(",")]; del a[i:i + 2]
    outdir, com = Path(a[0]).resolve(), Path(a[1]).resolve()
    plats = dict(x.split("=", 1) for x in a[2].split(","))
    cards = a[3].split(",")
    work = [(p, Path(b).resolve(), c) for p, b in plats.items() for c in cards]
    with ThreadPoolExecutor(jobs) as ex:
        list(ex.map(lambda w: one(outdir, com, w[0], w[1], w[2], cmds, env, files), work))


if __name__ == "__main__":
    main()
