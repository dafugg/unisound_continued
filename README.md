# UNISOUND 0.82a decompilation

C source for **UNISOUND.COM v0.82a**, the "Universal ISA PnP Sound Card Driver for DOS" by
JazeFox, decompiled so that it rebuilds the released binary byte for byte:

    606f8faec686d56468bb53eecdf1e28e4d6cda7a8a2051c36b7bcec16c37b370  UNISOUND.COM

The byte-identical source is the tag `v0.82a` and the branch `0.82a`. `main` adds bug fixes on
top of it, so its build no longer has that hash. All of this applies only to version 0.82a;
other releases will differ.

## Fixes on main

These were found by reading the decompiled source. "Tested" means the bug was reproduced in
86Box and the fix confirmed there ([docs/TESTING.md](docs/TESTING.md)). None has been tested
on real hardware.

- [Ensoniq `/R` reads the IRQ and DMA registers as bytes](https://github.com/dafugg/unisound_decomp/commit/57b36ade0588d6924a1459c90c50140c2ee28e76), not as words that take in the next register.
- [`/R` reads the MPU port back from the register it was written to](https://github.com/dafugg/unisound_decomp/commit/66d1b101702368b970c4aa1d492ba08899a78bf7) (ES1868 Maxi Sound, RJA0, TBS20).
- [`ULTRASND` parsing stops at the last field](https://github.com/dafugg/unisound_decomp/commit/71ac68182e51dc2a0eab69891489eb0946b15ac1): a value with fewer than five fields gave IRQ 0. Tested.
- [`BLASTER` parsing stops at the last field](https://github.com/dafugg/unisound_decomp/commit/f0f3c4983d5d82fc0039911fb4811f09d57e40b9) instead of reading past the end of the string.
- [Option parsing no longer processes `argv[argc]`](https://github.com/dafugg/unisound_decomp/commit/ce32fd2472a11da985a03e8a4ad79028cd3fec46), the NULL after the last argument.
- [Card and device names are cut to the buffer size](https://github.com/dafugg/unisound_decomp/commit/ddb518b7edf0fe7c5d58d29f8161a60c0db523a5): a long ANSI name overflowed the buffer and `/CL` hung. Tested.
- [The SAM9407 firmware upload no longer sends a word past the end of the file](https://github.com/dafugg/unisound_decomp/commit/d89926b8799d1c1a254279b73d9ddbcd7d9fc1ca).
- [The AWE reverb send level (`/VR`) is no longer sign-extended](https://github.com/dafugg/unisound_decomp/commit/23c60457e6d81cbf492b87b21d842ccd58e9740f): `/VR99` set voice 31's PTRX to FFFFFFFF. Tested.
- [InterWave ROM bank names are NUL-terminated](https://github.com/dafugg/unisound_decomp/commit/c96f1e896e52fc641c6cacace324eaf012744625): a 16-character name ran into the next field. Tested.
- [The ES688/ES1688 probe flushes stale bytes from the DSP read buffer](https://github.com/dafugg/unisound_decomp/commit/44bf539d3d8c3c3ae78c3ae4d47f0d8035d5a694), so `/NOPNP` finds the card after earlier commands. Found and tested in 86Box only.

## Building

You need **Turbo C++ 3.0** (TCC.EXE of February 1992, with its own C0T.OBJ and CS.LIB). Borland
C++ 3.0 and 3.1 produce slightly different code. In DOS, with `C:\TC\BIN` on the PATH and
`C:\TC\BIN\TURBOC.CFG` containing `-IC:\TC\INCLUDE -LC:\TC\LIB`:

    copy src\UNISOUND.C .
    BUILD.BAT

`BUILD.BAT` runs:

    TCC -c -mt -O -d UNISOUND.C
    TLINK /c /t C:\TC\LIB\C0T UNISOUND, UNISOUND, , C:\TC\LIB\CS

`-d` (merge duplicate strings) matters: it lets the compiler cross-jump identical code tails and
lays out the string pool exactly as in the original.

### Building in 86Box

`tools/build.sh` copies the source and `BUILD.BAT` into an 86Box DOS VM, runs the build there
through [86box-mcp](https://github.com/dafugg/86box-mcp), copies the results back to
`build/out/`, and checks them against `UNISOUND.SHA256`. On `main`, where the fixes change the
binary, use `tools/build.sh --no-hash`; the hash check is for the `0.82a` branch. Copy
`local.env.example` to `local.env` and set the paths first. mtools must be installed.

## Layout

| Path | Contents |
|---|---|
| `src/UNISOUND.C` | The whole program in one module, as in the original: 79 functions and all initialized data |
| `BUILD.BAT`, `UNISOUND.SHA256` | Build commands and the hash of the original v0.82a |
| `tools/` | Build, matching and test tools (see below) |
| `data/funcs.json` | Function map of the author's code (0x369–0x9185), including switch jump tables |
| `data/syms.json` | Addresses of the runtime-library symbols inside UNISOUND.COM |
| `docs/NOTES.md` | How the toolchain and layout were identified |
| `docs/TESTING.md` | Results of testing in 86Box |

Tools:
- `build.sh [--no-hash]`: builds in the VM and verifies the hash, unless `--no-hash` is given.
- `try.sh DIR START [-v ADDR]`: compiles one .C file in the VM and compares its functions with the original from START on.
- `objmatch.py OBJ START [-v ADDR]`: the comparison itself, with OMF fixups masked.
- `fn.py ADDR | --list`: annotated disassembly of one author function.
- `funcmap.py` and `libsyms.py`: regenerate `data/*.json`.
- `libmatch.py`, `disasm.py`, `cmpcom.py`, `omf.py`: helpers.
- `cardtest.py`, `cardreport.py` and `dos/PIO.C`: the 86Box sound card tests described in `docs/TESTING.md`.

The analysis tools need Python with capstone. They also need the original v0.82a UNISOUND.COM,
which this repo does not include: put it in `orig/` or set `UNISOUND_COM`.

## Status

The source compiles without warnings. On the `0.82a` branch the output is byte-identical; on
`main` it differs only by the fixes above. Functions, globals, the card families (`CARD_*`) and
the main routine's locals have names. Globals that hold settings say in their comment which
BLASTER letter or command-line option sets them. Some names are best guesses from the code and
the UNISOUND manual (for example `CARD_RJA0`, `CARD_ADS7141`). Locals are named too, apart from
loop counters and other conventional short names. On the `0.82a` branch, any change must keep
`tools/build.sh` reporting IDENTICAL; a build takes about 6 seconds.
