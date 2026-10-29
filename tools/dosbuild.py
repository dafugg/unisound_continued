"""Run a batch job in the DOS VM ($UNIVM): copy a source dir to C:\\WORK, boot, run C:\\WORK\\JOB.BAT
(as DOS sees it), power off, copy C:\\WORK back to <srcdir>/out/.
Usage: dosbuild.py SRCDIR [timeout_s]"""
import os, sys, shutil, subprocess, time
from pathlib import Path
if os.environ.get("MTOOLS_DIR"):
    os.environ["PATH"] = os.environ["MTOOLS_DIR"] + ":" + os.environ["PATH"]
if os.environ.get("BOX86MCP"):
    os.environ.setdefault("BOX86_BINARY", f"{os.environ['BOX86MCP']}/docker/86box.sh")
from box86mcp import server, disk
VM = Path(os.environ["UNIVM"])
src = Path(sys.argv[1]).resolve()
timeout = int(sys.argv[2]) if len(sys.argv) > 2 else 300
img = str(VM / "hdd.img")
part = f"{img}@@{disk.partition_offset(img)}"
env = dict(os.environ, MTOOLS_SKIP_CHECK="1", MTOOLS_NO_VFAT="1")
mt = lambda *a: subprocess.run(list(a), env=env, capture_output=True, text=True)
mt("mdeltree", "-i", part, "::WORK")
mt("mmd", "-i", part, "::WORK")
files = [str(p) for p in src.iterdir() if p.is_file()]
r = mt("mcopy", "-o", "-i", part, *files, "::WORK/")
if r.returncode: sys.exit(r.stderr)
server.vm_start(str(VM), held=True)
try:
    server.wait_for_text("C:\\>", 60000)
    server.dos_run("CD \\WORK", 5000)
    t0 = time.time()
    server.dos_run("JOB.BAT", timeout * 1000)
    screen = server.screen_text()
finally:
    server.vm_stop()
out = src / "out"
shutil.rmtree(out, ignore_errors=True); out.mkdir()
mt("mcopy", "-o", "-n", "-i", part, "::WORK/*", str(out) + "/")
print(screen)
