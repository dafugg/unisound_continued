# UNISOUND 0.82a decompilation + fork

[![Download Latest Release](https://img.shields.io/github/v/release/dafugg/unisound_continued?display_name=tag&label=Download%20Latest%20Release&style=for-the-badge)](https://github.com/dafugg/unisound_continued/releases/latest)

[![FreeDOS Build](https://img.shields.io/github/actions/workflow/status/dafugg/unisound_continued/build.yml?branch=main&label=FreeDOS%20Build&style=flat-square)](https://github.com/dafugg/unisound_continued/actions/workflows/build.yml)
[![FreeDOS Smoke Test](https://img.shields.io/github/actions/workflow/status/dafugg/unisound_continued/smoke.yml?branch=main&label=FreeDOS%20Smoke%20Test&style=flat-square)](https://github.com/dafugg/unisound_continued/actions/workflows/smoke.yml)
[![0.82a Byte-identical](https://img.shields.io/github/actions/workflow/status/dafugg/unisound_continued/0.82a.yml?branch=main&label=0.82a%20Byte-identical&style=flat-square)](https://github.com/dafugg/unisound_continued/actions/workflows/0.82a.yml)

C source for **UNISOUND.COM v0.82a**, the "Universal ISA PnP Sound Card Driver for DOS" by
JazeFox, decompiled so that it rebuilds the released binary byte for byte. UNISOUND is released
and discussed in [its thread on the VOGONS forum](https://www.vogons.org/viewtopic.php?t=72553).

It is also a fork: `main` adds fixes and more cards, and the `0.82a` branch keeps the
decompilation alone.

**This is an unofficial build. Don't yell at JazeFox if this burns down your house.** Nothing
here is made, endorsed or supported by JazeFox. UNISOUND, and the source and data reconstructed
from it, remain JazeFox's work; this repository is published only because JazeFox can no longer
be reached and the fixes are useful. If JazeFox asks, the reconstruction and the builds will be
taken down. A summary of what has changed since the official 0.82a, and why, is in
[docs/changes_since_official_082a.md](docs/changes_since_official_082a.md).

## Decompilation status

The branch `0.82a` is the byte-identical source: its head always rebuilds the released binary
byte for byte.

    606f8faec686d56468bb53eecdf1e28e4d6cda7a8a2051c36b7bcec16c37b370  UNISOUND.COM

Renames, comments and other changes that don't affect the code go there too, and any change must
keep `tools/build.sh` reporting IDENTICAL; a build takes about 6 seconds. CI checks this on every
push and pull request to `0.82a`, on every push to `main` and daily (the "0.82a Byte-identical"
badge above): it builds `0.82a` in 86Box under FreeDOS and fails unless the result has exactly
that hash. `main` adds the bug fixes in
[docs/changes_since_official_082a.md](docs/changes_since_official_082a.md) on top, so its build
no longer has that hash. All of this applies only to version 0.82a; other releases will differ.

The source compiles without warnings. Functions, globals, the card families (`CARD_*`) and the
main routine's locals have names. Globals that hold settings say in their comment which BLASTER
letter or command-line option sets them. Some names are best guesses from the code and the
UNISOUND manual (for example `CARD_RJA0`, `CARD_ADS7141`). Locals are named too, apart from loop
counters and other conventional short names. Beyond the byte-identical build, parts of the
decompilation have been checked by hand against the original binary in Ghidra 12.1.4.

## Supported sound cards

One small .COM file sets up an ISA sound card in DOS: no Creative CTCM, Intel ICU or vendor
drivers needed.

### PnP cards

- **Creative:** every Sound Blaster 16 and ViBRA16, AWE32, AWE64, SB32 PnP and the AWE upgrade
  card (CT1920). The EMU8000 is set up so FM and wavetable work without AWEUTIL, but no
  SoundFonts are loaded.
- **ESS AudioDrive:** ES1868 (including `SEM3006` cards) and ES1869 (including ES1869F cards
  with on-board wavetable, `ESS0003`), also on-board chips that the PnP scan misses (`/XESSB`),
  and ES1898. The ES1898 is identified as an ES1869 because it effectively is one: a minor,
  pin-compatible upgrade of the ES1869F ([dosdays](https://dosdays.co.uk/topics/Manufacturers/ess.php)).
  An ES1898F card with ID `ESS0003` works through both the PnP scan and `/XESSB`. Also ES968
  with an ES688 or ES1688. On the Maxi Sound 64 (ES1868 + SAM9407 versions only), the SAM9407
  firmware is uploaded, but the firmware itself isn't included.
- **Yamaha:** YMF715, YMF718 and YMF719 (OPL3-SAx, OPL3-SA3, AudioTrix XG-3D). ISA PnP models
  only: Yamaha chips built into laptops are usually configured through the BIOS and aren't
  supported.
- **Crystal:** CS4232, CS4235, CS4236/B, CS4237/B and CX4236/B, CX4237/B, including the Orpheus
  and Turtle Beach Malibu, and chips that the BIOS hides from PnP (`/XSLAM`).
- **Avance Logic:** ALS007, ALS100, ALS100+, ALS120 and ALS200, and DT-0197H/DT-0196H.
- **Aztech:** AZT2320.
- **Rockwell:** RWA01x (WaveArtist), including Zoltrix cards with the RWA011.
- **OPTi:** 82C924 and 82C925 in PnP mode, 82C931 and 82C933.
- **C-Media:** CMI8330 and CMI8329A.
- **Analog Devices:** AD1816/A, AD1815, AD18max10 and AD1812. Their WSS mode doesn't work with
  DOS software (a limitation of the chips). A few Terratec AD1816 cards need `/FS` to be detected.
- **Gravis Ultrasound PnP (AMD Interwave):** classic GUS compatibility only, with no SB emulation
  or MPU-401. Also the Compaq Interwave and STB Ultrasound Pro 32.
- **Roland Super MPU II:** MIDI only.
- **Ensoniq Soundscape VIVO:** limited. Only WSS audio, line-in, CD-in and the joystick port
  work; everything else needs Ensoniq's drivers.
- **Turtle Beach Multisound Pinnacle:** limited. Only the joystick port and MPU-401 (the
  Kurzweil wavetable) work. Some board revisions need `/FS`, and some may not work at all.

A few card IDs and the options `/XSLAM` and `/XESSB` are experimental: see
[Wanted](#wanted-testers-and-pnp-dumps).

### Non-PnP cards (`/NOPNP`, experimental; install only one card)

- **Creative:** SB16 and AWE32 (software-configurable models), with the same EMU8000 setup as
  the PnP cards.
- **ESS:** ES688, ES1688, ES1788 and ES1887/ES1888, including on-board chips such as the
  Compaq LTE 5000's and Presario 7100's ES1688 and the Presario 224x's ES1888, and the ES1688 on
  the Gravis UltraSound Extreme and ViperMax.
- **OPTi:** 82C924 and 82C925 in non-PnP mode, and 82C930. On the 82C930 the CD interface isn't
  supported.
- **OAK OTI601 "Mozart" and OPTi 82C928 (MAD16):** JazeFox's notes: poor compatibility with
  games, the card may need setting up again after some programs, and SB-mode audio is mono only;
  FM and WSS mode (`/XWSS`) are stereo.
- **OPTi 82C929 (MAD16 Pro):** for example the Boca Research Sound Expression 4294 and SE1440.
  SB Pro mode, with the MPU-401 (BLASTER P and U); experimental, see
  [Wanted](#wanted-testers-and-pnp-dumps).

On the OPTi 82C928, 82C929 and 82C930, `/NOPNP /XWSS` sets up Windows Sound System mode instead of
SB Pro mode, as OPTi's own SNDINIT does; OPTi's documentation treats them as one mode at a time.
The WSS base, DMA and IRQ come from `ULTRA16=port,dma,irq,record dma`, which programs such as
Mpxplay read to find the WSS codec, and not from BLASTER. The port is 530, E80, F40 or 604 (hex),
the DMAs 0, 1 or 3 and the IRQ 7, 9, 10 or 11, or also 5 on the 82C930 (the 82C928 and 82C929
use IRQ 7 instead). A missing or invalid field falls back to OPTi's default, 530h, DMA 0 or
IRQ 7, and if the port, DMA or IRQ field was missing or invalid UNISOUND prints the
`SET ULTRA16=` line to add to AUTOEXEC.BAT (`SET ULTRA16=530,0,7,0` without the variable). The
record DMA is optional. `SET WSS`, which some OPTi 82C930/931 drivers are said to read, is not
used: no format for it was found. On the 82C928 and 82C929 WSS mode turns the SB part's IRQ off;
the 82C930 keeps its SB part enabled with its own IRQ and DMA, as SNDINIT leaves it.

On GUS PnP (InterWave) cards, UNISOUND reads `ULTRASND=port,dma,dma2,irq,irq2` instead of
BLASTER. The GF1 goes at the port (220, 230, 240, 250 or 260 hex), its second range at port +
100h and the codec at port + 10Ch, with the two DMAs and the IRQ from the variable; without it,
UNISOUND uses 240, DMA 3 and 3, IRQ 11. Limitations:

- The fifth field, the MIDI IRQ, is read but not used: the second IRQ is left off, as are SB
  emulation and the MPU-401.
- The DMA and IRQ fields are only range-checked (DMA 0-7, IRQ 0-15), not checked against what
  the card supports. An out-of-range field keeps the default.
- With fewer than three fields UNISOUND says it is loading its defaults, but the fields given are
  still used.
- No `SET ULTRASND=` line is printed when the variable is missing, as there is for `ULTRA16`.
- `ULTRADIR` and Gravis's other variables are neither read nor set.
- On the GUS Extreme and ViperMax (`/NOPNP`), UNISOUND sets up only the ES1688, from BLASTER; the
  GF1 part and its `ULTRASND` are left to Gravis's software. Non-PnP GUS cards aren't supported.

This list comes from the source and hasn't been checked on a real card; see
[Wanted](#wanted-testers-and-pnp-dumps).

## Wanted: testers and PnP dumps

Some cards are recognised only from driver files, Linux's tables or 86Box tests with edited PnP
ROMs, because no one has tried them with these builds on a real card. They are experimental until
someone does. Other cards are left out until we know how their logical devices are laid out.

If you have one of these cards, please open a GitHub issue with:

- the card's name and revision, and the board it's in;
- the output of `UNISOUND /R` and of a plain `UNISOUND` run (if it's recognised);
- a dump of its PnP data: `pnpdump` under Linux, or `PNPDUMP` built from `tools/dos/PNPDUMP.C`.

### Experimental: recognised, not yet tried on a real card

| Card ID | Card | Set up as | Evidence |
|---|---|---|---|
| AZT1021 | Aztech AZT2320 cards | Aztech | Aztech's AUD1021.INF and PNPID.DAT; 86Box AZT2320 ROM with the id changed |
| AZT1022 | Aztech AZT2320 cards, and the AD1816A Aztech/Newcom SC-16 3D | Aztech, or AD1816 when the first logical device is AZT1018 | JazeFox's own card (VOGONS p836545), Linux ad1816a.c; 86Box ROMs with the ids changed |
| CSC??25, CDC??25 (e.g. CSC0225, CSC4625, CSCE825) | Crystal CS4235 | Crystal | Linux cs4236.c; 86Box's CS4235 (CSCA725) |
| RTL3000 | Realtek-branded ALS120 | ALS | Avance's A4SOUND.INF lists the same logical devices in the same order as ALS0120; not testable in 86Box |
| DXP3201 | MED3210 (InterWave) | GUS PnP | Linux interwave.c; 86Box GUS PnP ROM with the id changed |
| CDC1111 | Dynasonic Pro (InterWave) | GUS PnP | Linux interwave.c; 86Box GUS PnP ROM with the id changed |
| ESS1878, ESS1879 | ESS ES1878 / ES1879 | ES1869 | ESS's data sheets give them the ES1869's layout and bypass key; 86Box ES1869 with each chip's internal PnP ROM from its data sheet |
| none (`/NOPNP`) | OPTi 82C929 (MAD16 Pro): Boca Research Sound Expression 4294 and SE1440, miro PCM10, Acer Magic S20 | OPTi 82C929, SB Pro or WSS (`/XWSS`, `ULTRA16`) | its data sheet; 86Box `acermagic_s20` and `mirosound_pcm10`, where it leaves the same registers as OPTi's own SNDINIT |

### Held back: need a PnP dump first

| Card ID | Card | Would be set up as | What we need to know |
|---|---|---|---|
| MIR0924 | miro PCM12 / PCM20 (OPTi 82C924) in PnP mode | OPTi 924 | Whether the logical devices sit where OPTi's OPT0924 has them (Linux lists MIR0000/0002/0005 like OPT0000/0002/0005) |
| BVJ0440 | Turtle Beach Multisound Pinnacle (rev F?) | Pinnacle | Where the joystick and IDE devices are; UNISOUND's Pinnacle code expects the joystick on logical device 2, Linux's non-PnP setup has IDE there |

### Options not yet tried on real hardware

These find chips that PnP isolation misses. They work in 86Box, but no one has reported back from
a real machine yet. Please include `UNISOUND /LOG` output with the option.

| Option | For | Machines that need it |
|---|---|---|
| `/XSLAM` | Crystal CS423x chips whose PnP key is disabled, so they take no part in isolation | Laptops and boards whose BIOS or CWDINIT sets up the chip, where UNISOUND says "No PnP card detected." |
| `/XESSB` | ES1868/ES1869 chips that isolation doesn't find | On-board ES1868/ES1869, or chipsets that don't pass A79h on to the chip; 86Box always passes it on, so only a real machine can test this |

### Fixes for VOGONS reports, not yet tried on real hardware

These fix problems reported on VOGONS and work in 86Box, but no one has tried them on a real card
yet. Details and links to the reports are in
[docs/changes_since_official_082a.md](docs/changes_since_official_082a.md). Please include
`UNISOUND /LOG` output.

| Commit | Card | Fix | Reported by |
|---|---|---|---|
| [00806ef](https://github.com/dafugg/unisound_continued/commit/00806ef) | Any card | After setup, BLASTER's A, I, D and H are compared with what the card got | analog_programmer |
| [5281f0f](https://github.com/dafugg/unisound_continued/commit/5281f0f) | Crystal CS4237B/CS4238B cards (Orpheus, Turtle Beach) | The codec is put back in MODE 1 | vanfanel |
| [1adfd17](https://github.com/dafugg/unisound_continued/commit/1adfd17) | ES1868/ES1869 | BLASTER `U` sets the MPU-401 IRQ | crvs |
| [f73bb3f](https://github.com/dafugg/unisound_continued/commit/f73bb3f) | ES1868/ES1869 | `H4`, or D = H, means no second DMA | Kahenraz |
| [26f3356](https://github.com/dafugg/unisound_continued/commit/26f3356) | ES1887/ES1888 with `/NOPNP` | The IRQ the BIOS set up is kept | 0xDEADBEEF |
| [ff390e3](https://github.com/dafugg/unisound_continued/commit/ff390e3) | Creative cards | A BLASTER `F` that isn't a multiple of 4 is rounded down | Mr.Hunt |
| [50253a7](https://github.com/dafugg/unisound_continued/commit/50253a7) | Yamaha OPL3-SAx | BLASTER `K` sets the WSS DMA | TgamesFR |
| [ecbe7de](https://github.com/dafugg/unisound_continued/commit/ecbe7de) | Yamaha OPL3-SAx | The SB Pro master volume is set correctly | badmojo, --Freeman-- |

Also wanted: Dell, Compaq and other laptops whose ESS chip has a PnP BIOS ID from `ESS0100` to
`ESS0119` (ES688, ES888, ES1688, ES1698, ES1788, ES1887, ES1888 and ES1869B chips). ESS's device list
gives these IDs to on-board chips that the BIOS describes, not to ISA PnP cards, so UNISOUND's PnP
scan doesn't see them; `/NOPNP` may find the chip once the BIOS has enabled it.

Also wanted: someone with a real GUS PnP (InterWave) card to test `ULTRASND` handling (see the
limitations above): run `UNISOUND /LOG` with a full five-field `ULTRASND`, with fewer fields, and
without it, and report the output and whether GUS games and Gravis's own tools work afterwards.

## Reporting problems

The build from `main` is unofficial: it is not a release by JazeFox, and JazeFox doesn't support it.
Before opening an issue here, check whether the problem also happens with the official v0.82a:

- **If it does**, report it in [the UNISOUND thread on VOGONS](https://www.vogons.org/viewtopic.php?t=72553)
  first, then open an issue here that links to your post.
- **If it happens only with the build from `main`**, open an issue here. Include the
  version line UNISOUND prints, your card, your BLASTER or ULTRASND setting and the command line.

### What to run

Start each step from a cold boot (power off, not a reset) with no other sound drivers loaded,
and say in the issue what each one printed:

1. **The official v0.82a**, with the options you normally use.
2. **This build, with the same options plus `/LOG`**, redirected to a file you attach:
   `UNISOUND /LOG > LOG.TXT`, or `UNISOUND /NOPNP /LOG > LOG.TXT`.
3. **If no card is found**, also `/XSLAM` (a Crystal chip set up by the BIOS) and `/XESSB`
   (an ES1868/ES1869 that the PnP scan misses), each with `/LOG`.
4. **The card maker's own DOS setup program**, and whether it finds the card: ESSCFG for ESS
   chips, SNDINIT for OPTi, CTCM for Creative, CWDINIT for Crystal. Many different programs
   share each of these names, so tell us which one you ran: the version it prints, its size and
   date (`DIR`), its SHA-256 if you can work it out on another machine (`sha256sum ESSCFG.EXE`,
   or `certutil -hashfile ESSCFG.EXE SHA256` on Windows), and where the file came from (the
   card's driver disk, a download link, the name of the package).
5. **Your machine**: the board or model, the CPU, whether its BIOS is PnP, and the other cards
   in it with their ports.

`/XESSB` and `/XSLAM` skip the PnP scan, so they never read the card's ID and print a fixed
name for the chip instead. The same card can show up under different names: an ES1898F card is
`ESS0003` normally and `[ESS1869] (ESS bypass key)` with `/XESSB`, because the chip reports
itself as an ES1869.

### What `/LOG` shows

`/LOG` adds lines starting with `LOG: ` that show what UNISOUND found and wrote:

- the DOS version and the command line; BLASTER as given and every field as parsed, and ULTRASND;
- on the PnP path: the read data port, an isolation that answered with a bad checksum, each
  card's vendor ID, whole serial identifier and card type, the selected card's resource data in
  hex and which logical device it uses for what, and after the setup each configured logical
  device's registers 30h, 60h-65h and 70h-75h read back;
- the WSS codec's registers I0-I31 on Crystal, Yamaha, OPTi and GUS PnP cards;
- with `/NOPNP`: the OPTi 82C924/925, 82C930 and 82C928 probes' reads; the SB16 reply at each
  base; and for the ES688/ES1688, for each base tried, 2xEh/2xAh before anything, the reset byte
  and how long it took, the bytes flushed and the E7h reply, then B1h/B2h before and after, the
  bytes written, mixer 40h before and after, and which IRQ an F2h interrupt arrived on (the test
  hooks IRQ 3, 5, 7, 9 and 10 for a moment and restores them);
- with `/XESSB`: the E7h reply and the four mixer 40h reads after the bypass key.

### ESSINIT

For an ES688/ES1688-family chip that doesn't work, `tools/dos/ESSINIT.C` sets the chip up the
way ESSCFG does, from BLASTER's A, I and D, and prints each step: the enable reads if needed,
reset, E7h, B1h/B2h before, written and read back, and an F2h interrupt test. `ESSINIT /OLD`
writes B1h/B2h as v0.82a does, for comparison. It exits with 0 when the chip took the IRQ and DMA
and interrupted on that IRQ. Without BLASTER it uses UNISOUND's defaults, A220 I5 D1. It first
runs v0.82a's `/NOPNP` OPTi probe and says whether v0.82a would take the machine for an OPTi
82C924/925 and skip the ESS detection.

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
| `docs/changes_since_official_082a.md` | What changed since the official 0.82a, and why |
| `docs/TESTING.md` | Results of testing in 86Box |
| `docs/datasheets/` | Vendor data sheets used for the work, with their sources (see its README) |
| `docs/ROMS.md` | How the patched 86Box ROMs and EEPROMs used in the tests were made, with their SHA-256 |

Tools:
- `build.sh [--no-hash]`: builds in the VM and verifies the hash, unless `--no-hash` is given.
- `try.sh DIR START [-v ADDR]`: compiles one .C file in the VM and compares its functions with the original from START on.
- `objmatch.py OBJ START [-v ADDR]`: the comparison itself, with OMF fixups masked.
- `fn.py ADDR | --list`: annotated disassembly of one author function.
- `funcmap.py` and `libsyms.py`: regenerate `data/*.json`.
- `libmatch.py`, `disasm.py`, `cmpcom.py`, `omf.py`: helpers.
- `cardtest.py`, `cardreport.py` and `dos/PIO.C`: the 86Box sound card tests described in `docs/TESTING.md`.
- `dos/`: the DOS test programs used for the fixes (PNPDUMP, ESSREG, DMAPLAY, WSSPLAY, OPLDET, SBST, TRACEPNP), ESSINIT (ES688/ES1688 setup test) and the prototypes of `/XESSB` and `/XSLAM` (ESSBYP, CSLAM); `pnpdecode.py` decodes TRACEPNP logs. See `docs/TESTING.md`.

The analysis tools need Python with capstone. They also need the original v0.82a UNISOUND.COM,
which this repo does not include: put it in `orig/` or set `UNISOUND_COM`.

## License

The tools, DOS test programs, build files and documentation are under the MIT license (see
[LICENSE](LICENSE)). `src/UNISOUND.C` and `data/*.json` are not: they are reconstructed from, or
derived from, UNISOUND by JazeFox, and remain JazeFox's work.
