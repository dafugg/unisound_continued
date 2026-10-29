# UNISOUND 0.82a decompilation

C source for **UNISOUND.COM v0.82a**, the "Universal ISA PnP Sound Card Driver for DOS" by
JazeFox, decompiled so that it rebuilds the released binary byte for byte. UNISOUND is released
and discussed in [its thread on the VOGONS forum](https://www.vogons.org/viewtopic.php?t=72553).

    606f8faec686d56468bb53eecdf1e28e4d6cda7a8a2051c36b7bcec16c37b370  UNISOUND.COM

The branch `0.82a` is the byte-identical source: its head always rebuilds the binary byte for
byte. Renames, comments and other changes that don't affect the code go there too, and must
keep `tools/build.sh` reporting IDENTICAL. `main` adds bug fixes on top, so its build no longer
has that hash. All of this applies only to version 0.82a; other releases will differ.

## Decompilation status

The source compiles without warnings. On the `0.82a` branch the output is byte-identical; on
`main` it differs only by the fixes below. Functions, globals, the card families (`CARD_*`) and
the main routine's locals have names. Globals that hold settings say in their comment which
BLASTER letter or command-line option sets them. Some names are best guesses from the code and
the UNISOUND manual (for example `CARD_RJA0`, `CARD_ADS7141`). Locals are named too, apart from
loop counters and other conventional short names. On the `0.82a` branch, any change must keep
`tools/build.sh` reporting IDENTICAL; a build takes about 6 seconds.

## Fixes & Changes after 0.82a

All of these except the ESS DSP flush were found by reading the decompiled source. The 86Box
tests used 86Box on an ASUS PVI-486SP3C (`486sp3c`, PnP BIOS) and an Olystar
LIL1429 (`ali1429`, no PnP BIOS) under MS-DOS 6.22; details are in
[docs/TESTING.md](docs/TESTING.md). A ✅ means the bug was reproduced in 86Box and the fix shown
to work. Every fix was also part of a 175-run regression check against all of 86Box's sound
cards, with no regressions.

| Commit | Description | Tested on 86Box | Tested on real hardware |
|---|---|---|---|
| [57b36ad](https://github.com/dafugg/unisound_decomp/commit/57b36ade0588d6924a1459c90c50140c2ee28e76) | Ensoniq SoundscaPE VIVO (`ENS4…`): `/R` reads the IRQ and DMA registers as bytes, not as words that take in the next register | ❌ 86Box doesn't emulate the SoundscaPE VIVO (its only Ensoniq cards are the PCI AudioPCI models) | ❌ |
| [66d1b10](https://github.com/dafugg/unisound_decomp/commit/66d1b101702368b970c4aa1d492ba08899a78bf7) | `/R` reads the MPU port back from the register it was written to, on the Guillemot Maxi Sound 64 (ES1868 + SAM9407) and the cards with PnP IDs `RJA0…` and `TBS20…` (Turtle Beach) | ❌ 86Box doesn't emulate these cards (it has a plain ES1868, but not the Maxi Sound variant) | ❌ |
| [71ac681](https://github.com/dafugg/unisound_decomp/commit/71ac68182e51dc2a0eab69891489eb0946b15ac1) | `ULTRASND` parsing stops at the last field; a value with fewer than five fields gave IRQ 0 | ✅ `guspnp`, `ULTRASND=240,1,1`: `IRQ:0` before, `IRQ:11` after | ❌ |
| [f0f3c49](https://github.com/dafugg/unisound_decomp/commit/f0f3c4983d5d82fc0039911fb4811f09d57e40b9) | `BLASTER` parsing stops at the last field instead of reading past the end of the string | ❌ No visible effect: `BLASTER=A220 I5 D1 H5 P330 T6` gives the same output with both builds | ❌ |
| [ce32fd2](https://github.com/dafugg/unisound_decomp/commit/ce32fd2472a11da985a03e8a4ad79028cd3fec46) | Option parsing no longer processes `argv[argc]`, the NULL after the last argument | ❌ No visible effect | ❌ |
| [ddb518b](https://github.com/dafugg/unisound_decomp/commit/ddb518b7edf0fe7c5d58d29f8161a60c0db523a5) | Card and device names are cut to the buffer size; a long ANSI name overflowed it | ✅ `sb16_pnp` with its PnP ROM patched to a 150-character name: `/CL` hung before, works after | ❌ |
| [d89926b](https://github.com/dafugg/unisound_decomp/commit/d89926b8799d1c1a254279b73d9ddbcd7d9fc1ca) | Guillemot Maxi Sound 64 (ES1868 + SAM9407): the SAM9407 firmware upload no longer sends a word past the end of the file | ❌ 86Box doesn't emulate the Maxi Sound 64 or the SAM9407 | ❌ |
| [23c6045](https://github.com/dafugg/unisound_decomp/commit/23c60457e6d81cbf492b87b21d842ccd58e9740f) | SB AWE32/64 FM reverb amount (`/VR50` to `/VR99`): the value went negative and set the pitch-target bits of FM passthrough voice 31 to `FFFF`. `/VR00` to `/VR49` were unaffected | ✅ `sbawe32_pnp`, `/VR99`, voice 31 PTRX: `FFFFFFFF` before, `0000FFFF` after | ❌ |
| [c96f1e8](https://github.com/dafugg/unisound_decomp/commit/c96f1e896e52fc641c6cacace324eaf012744625) | InterWave ROM bank names are NUL-terminated; a 16-character name ran into the next field | ✅ `guspnp` with `IWROM.BIN`'s bank name set to 16 characters: `[AMDGM_1_1_FULL16INTR]` before, `[AMDGM_1_1_FULL16]` after | ❌ |
| [44bf539](https://github.com/dafugg/unisound_decomp/commit/44bf539d3d8c3c3ae78c3ae4d47f0d8035d5a694) | The ES688/ES1688 probe flushes stale bytes from the DSP read buffer, so `/NOPNP` finds the card after earlier commands. Found in 86Box | ✅ `ess_es1688`, `/NOPNP` twice: the second run found nothing before, finds the card after. The regression run showed the same fix on `ess_es688` | ❌ |

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
