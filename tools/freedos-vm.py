"""Make a FreeDOS 1.4 VM for tools/build.sh (with --tc) and tools/cardtest.py: 86box-mcp's DX2-66,
by default on the 486SP3C (PnP BIOS), booting the FreeDOS LiteUSB disk image straight to C:\\> (its
installer is removed), with HIMEMX for extended memory.

Usage: freedos-vm.py FD14LITE.img VMDIR [--tc TCDIR] [--machine NAME --nvr FILE]
  VMDIR      must not exist yet
  --tc       a Turbo C++ 3.0 install (BIN/, LIB/, INCLUDE/), copied to C:\\TC
  --machine  another 86Box board, such as 486vipio2
  --nvr      the CMOS to start that board with (tools/nvr/), so its BIOS doesn't stop for F1;
             86box-mcp has one only for the 486SP3C
Run with the 86box-mcp server's Python (it needs box86mcp); mtools must be on PATH.
"""
import argparse, os, shutil, subprocess
from pathlib import Path
from box86mcp import disk, vm

FDCONFIG = """!FILES=40
!BUFFERS=20
!LASTDRIVE=Z
DOS=HIGH
DEVICE=\\FREEDOS\\BIN\\HIMEMX.EXE
SHELL=\\FREEDOS\\BIN\\COMMAND.COM \\FREEDOS\\BIN /E:1024 /P=\\FDAUTO.BAT
"""
FDAUTO = """@ECHO OFF
SET DOSDIR=\\FREEDOS
SET PATH=\\FREEDOS\\BIN
PROMPT $P$G
"""

a = argparse.ArgumentParser()
a.add_argument("img", type=Path)
a.add_argument("vmdir", type=Path)
a.add_argument("--tc", type=Path)
a.add_argument("--machine")
a.add_argument("--nvr", type=Path)
args = a.parse_args()
d = args.vmdir
d.mkdir()
hdd = d / "hdd.img"
shutil.copyfile(args.img, hdd)
part = f"{hdd}@@{disk.partition_offset(hdd)}"
env = dict(os.environ, MTOOLS_SKIP_CHECK="1", MTOOLS_NO_VFAT="1")
for name, text in [("FDCONFIG.SYS", FDCONFIG), ("FDAUTO.BAT", FDAUTO)]:
    (d / name).write_bytes(text.replace("\n", "\r\n").encode())
    subprocess.run(["mcopy", "-o", "-i", part, str(d / name), "::" + name], env=env, check=True)
    (d / name).unlink()
subprocess.run(["mdel", "-i", part, "::SETUP.BAT"], env=env, check=True)
if args.tc:
    subprocess.run(["mmd", "-i", part, "::TC"], env=env, check=True)
    subprocess.run(["mcopy", "-s", "-i", part, *[str(p) for p in args.tc.iterdir()], "::TC/"],
                   env=env, check=True)
spt, heads = 63, 16
cfg = vm.write_config(d, hdd.stat().st_size // (512 * spt * heads), heads, spt)
if args.machine:
    cfg.write_text(cfg.read_text().replace("machine = 486sp3c", f"machine = {args.machine}"))
print(vm.prime_nvr(d, args.nvr))
