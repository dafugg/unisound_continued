# UNISOUND 0.82a decompilation

C source that rebuilds **UNISOUND.COM v0.82a** byte for byte. UNISOUND is the "Universal ISA PnP
Sound Card Driver for DOS" by JazeFox. The rebuilt file has the same SHA-256 as the released
v0.82a binary:

    606f8faec686d56468bb53eecdf1e28e4d6cda7a8a2051c36b7bcec16c37b370  UNISOUND.COM

This applies only to version 0.82a; other releases will differ.

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
`build/out/`, and checks them against `UNISOUND.SHA256`. Copy `local.env.example` to `local.env`
and set the paths first. mtools must be installed.

## Layout

| Path | Contents |
|---|---|
| `src/UNISOUND.C` | The whole program in one module, as in the original: 79 functions and all initialized data |
| `BUILD.BAT`, `UNISOUND.SHA256` | Build commands and the expected hash |
| `tools/` | Build and matching tools (see below) |
| `data/funcs.json` | Function map of the author's code (0x369–0x9185), including switch jump tables |
| `data/syms.json` | Addresses of the runtime-library symbols inside UNISOUND.COM |
| `docs/NOTES.md` | How the toolchain and layout were identified |

Tools:
- `build.sh`: builds in the VM and verifies the hash.
- `try.sh DIR START [-v ADDR]`: compiles one .C file in the VM and compares its functions with the original from START on.
- `objmatch.py OBJ START [-v ADDR]`: the comparison itself, with OMF fixups masked.
- `fn.py ADDR | --list`: annotated disassembly of one author function.
- `funcmap.py` and `libsyms.py`: regenerate `data/*.json`.
- `libmatch.py`, `disasm.py`, `cmpcom.py`, `omf.py`: helpers.

The analysis tools need Python with capstone. They also need the original v0.82a UNISOUND.COM,
which this repo does not include: put it in `orig/` or set `UNISOUND_COM`.

## Status

The source compiles without warnings and the output is byte-identical. Functions, globals, the
card families (`CARD_*`) and the main routine's locals have names. Globals that hold settings
say in their comment which BLASTER letter or command-line option sets them. Some names are
best guesses from the code and the UNISOUND manual (for example `CARD_RJA0`, `CARD_ADS7141`).
Locals are named too, apart from loop counters and other conventional short names. Any change must keep `tools/build.sh`
reporting IDENTICAL; a build takes about 6 seconds.

## License

The tools, build files and documentation are under the MIT license (see [LICENSE](LICENSE)).
`src/UNISOUND.C` and `data/*.json` are not: they are reconstructed from, or derived from,
UNISOUND by JazeFox, and remain JazeFox's work.
