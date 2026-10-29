"""Make a FreeDOS 1.4 VM for tools/build.sh (with TCDIR) and tools/cardtest.py: 86box-mcp's DX2-66
on the 486SP3C (PnP BIOS), booting the FreeDOS LiteUSB disk image straight to C:\\> (its installer
is removed), with HIMEMX for extended memory.

Usage: freedos-vm.py FD14LITE.img VMDIR [TCDIR]      (VMDIR must not exist yet)
  TCDIR  a Turbo C++ 3.0 install (BIN/, LIB/, INCLUDE/), copied to C:\\TC
Run with the 86box-mcp server's Python (it needs box86mcp); mtools must be on PATH.
"""
import os, shutil, subprocess, sys
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

img, d = Path(sys.argv[1]), Path(sys.argv[2])
tc = Path(sys.argv[3]) if len(sys.argv) > 3 else None
d.mkdir()
hdd = d / "hdd.img"
shutil.copyfile(img, hdd)
part = f"{hdd}@@{disk.partition_offset(hdd)}"
env = dict(os.environ, MTOOLS_SKIP_CHECK="1", MTOOLS_NO_VFAT="1")
for name, text in [("FDCONFIG.SYS", FDCONFIG), ("FDAUTO.BAT", FDAUTO)]:
    (d / name).write_bytes(text.replace("\n", "\r\n").encode())
    subprocess.run(["mcopy", "-o", "-i", part, str(d / name), "::" + name], env=env, check=True)
    (d / name).unlink()
subprocess.run(["mdel", "-i", part, "::SETUP.BAT"], env=env, check=True)
if tc:
    subprocess.run(["mmd", "-i", part, "::TC"], env=env, check=True)
    subprocess.run(["mcopy", "-s", "-i", part, *[str(p) for p in tc.iterdir()], "::TC/"], env=env, check=True)
spt, heads = 63, 16
vm.write_config(d, hdd.stat().st_size // (512 * spt * heads), heads, spt)
print(vm.prime_nvr(d))
