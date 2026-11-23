# UNISOUND 0.82a decompilation + fork

[![Download Latest Release](https://img.shields.io/github/v/release/dafugg/unisound_continued?display_name=tag&label=Download%20Latest%20Release&style=for-the-badge)](https://github.com/dafugg/unisound_continued/releases/latest)

[![FreeDOS Build](https://img.shields.io/github/actions/workflow/status/dafugg/unisound_continued/build.yml?branch=main&label=FreeDOS%20Build&style=flat-square)](https://github.com/dafugg/unisound_continued/actions/workflows/build.yml)
[![FreeDOS Smoke Test](https://img.shields.io/github/actions/workflow/status/dafugg/unisound_continued/smoke.yml?branch=main&label=FreeDOS%20Smoke%20Test&style=flat-square)](https://github.com/dafugg/unisound_continued/actions/workflows/smoke.yml)
[![0.82a Byte-identical](https://img.shields.io/github/actions/workflow/status/dafugg/unisound_continued/0.82a.yml?branch=main&label=0.82a%20Byte-identical&style=flat-square)](https://github.com/dafugg/unisound_continued/actions/workflows/0.82a.yml)

C source for **UNISOUND.COM v0.82a**, the "Universal ISA PnP Sound Card Driver for DOS" by
JazeFox, decompiled so that it rebuilds the released binary byte for byte. UNISOUND is released
and discussed in [its thread on the VOGONS forum](https://www.vogons.org/viewtopic.php?t=72553);
this fork's unofficial builds have [their own thread](https://www.vogons.org/viewtopic.php?p=1456858).

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

One small program, UNISOUND.COM, sets up an ISA sound card in DOS: no Creative CTCM, Intel ICU or
vendor drivers needed.

### PnP cards

- **Creative:** every Sound Blaster 16 and ViBRA16, AWE32, AWE64, SB32 PnP and the AWE upgrade
  card (CT1920). The EMU8000 is set up so FM and wavetable work without AWEUTIL, but no
  SoundFonts are loaded.
- **ESS AudioDrive:** ES1868 (including `SEM3006` cards) and ES1869 (including ES1869F cards
  with on-board wavetable, `ESS0003`), also on-board chips that the PnP scan misses (`/XESSB`),
  and ES1898. The ES1898 is identified as an ES1869 because it effectively is one: a minor,
  pin-compatible upgrade of the ES1869F ([dosdays](https://dosdays.co.uk/topics/Manufacturers/ess.php)).
  An ES1898F card with ID `ESS0003` works through both the PnP scan and `/XESSB`. Also ES968
  with an ES688 or ES1688, and ES688/ES1688 PnP cards with ID `ESS0100` or `ESS0102`
  (experimental, see [Wanted](#wanted-testers-and-pnp-dumps)). On the Maxi Sound 64 (ES1868 +
  SAM9407 versions only), the SAM9407 firmware is uploaded, but the firmware itself isn't
  included. The file must be named `SAM9407.BIN` and be in the current directory when UNISOUND
  runs; without it UNISOUND prints `SAM9407.BIN firmware file not found. Sound will be silent.`
  and the card stays silent. The file must also be between 4 KB and 128 KB with the right
  header, or UNISOUND prints `Bad SAM9407 firmware file.` The MK1869, a modern ES1869 card,
  works with 0.82a according to its developer's
  [notes on UNISOUND](https://pcmidi.eu/mk1869.html) (`/X3D0` turns the 3D effect off, `/VBnn`
  sets AuxB, the wavetable header, apart from FM); it hasn't been tried with these builds.
- **Yamaha:** YMF715, YMF718 and YMF719 (OPL3-SAx, OPL3-SA3, AudioTrix XG-3D). ISA PnP models
  only: Yamaha chips built into laptops are usually configured through the BIOS and aren't
  supported.
- **Crystal:** CS4232, CS4235, CS4236/B, CS4237/B and CX4236/B, CX4237/B, including the Orpheus
  and Turtle Beach Malibu, and chips that the BIOS hides from PnP (`/XSLAM`). Cards whose PnP ID
  ends in 32 get UNISOUND's built-in CS4232 firmware patch (3213 bytes), with `Loading CS4232
  Firmware... done.` The Terratec EWS64 (`CSCA836`, `CDCA836`) gets its resources but no
  firmware, and UNISOUND prints `EWS64 firmware not supported. Output will be silent.` That its
  output stays silent comes from UNISOUND.TXT (its v0.76b notes) and hasn't been tried with
  these builds; a [VOGONS report by moturimi1](https://www.vogons.org/viewtopic.php?p=1352501#p1352501)
  says an EWS64 XL's MIDI at P310/P340 works.
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

The list above names chips. UNISOUND picks the setup from the card's PnP ID, using the table
`card_ids` in `src/UNISOUND.C` (`?` matches any character, and the first match wins):

- **Creative:** `CTL????`; `CTL??F4` is the AWE upgrade card.
- **Avance Logic:** `ALS????`, with variants for `ALS??07` and `ALS0001`; `RWB1???` is set up as
  an ALS card and shown as "Diamond"; `RTL3000` (Realtek ALS120, experimental).
- **ESS:** `ESS096?` (ES968), `ESS1868` and `SEM3006` (ES1868), `ESS0003`, `ESS0006` and
  `ESS1869` (ES1869), `ESS1878` and `ESS1879` (set up as ES1869, experimental), `ESS0100` and
  `ESS0102` (experimental).
- **Yamaha:** `YMH????`, `NMX22??`, `QHA08??` and `MPI0003`.
- **Rockwell:** `RSS????`; `RSS58??` cards whose name starts with "Zol" (Zoltrix) get the `/XRU`
  setup on their own.
- **OPTi:** `OPT??24`, `OPT??25`, `OPT??31` and `OPT??33`.
- **C-Media:** `CMI????`.
- **Analog Devices:** `ADS7141` has its own setup; `ADS71??`, `AXS71??`, `TER11??`, `TER14??`,
  `LWC1061`, `MDK1605` and `SMM7180` are AD1816 cards.
- **Aztech:** `AZT232?`, `AZT1005` to `AZT1009`, `AZT1015` to `AZT1017`, `AZT1021` to
  `AZT1023`, `AZT1026`, `AZT3000` to `AZT3003`, `AZT3005`, `AZT3011` and `AZT3013`. Other IDs in
  those ranges, such as `AZT1010` or `AZT3004`, are not recognised. `AZT1022` is an AD1816 card
  when its first logical device is `AZT1018`.
- **Crystal:** `CSC??3?` and `CDC??3?`; IDs ending in 32 get the CS4232 firmware, IDs ending in
  37 the CS4237 extended setup, and `CSCA836`/`CDCA836` are the EWS64. `CSC?25?`, `CSC??25`,
  `CDC?25?` and `CDC??25` are CS4235 cards, and `TBS2004` is a Crystal card. Cards whose name
  starts with "Orp" (Orpheus) or "Tur" (Turtle Beach Malibu) get their own extended setup.
- **Other:** `RJA0???` (Roland Super MPU II), `ENS4???` (Ensoniq Soundscape VIVO), `TBS20??`
  (Turtle Beach Pinnacle), and `GRV????`, `ADV55??`, `STB01??`, `DXP3201` and `CDC1111` (GUS PnP;
  the last two experimental).

### Non-PnP cards (`/NOPNP`, experimental; install only one card)

- **Creative:** SB16 and AWE32 (software-configurable models), with the same EMU8000 setup as
  the PnP cards.
- **ESS:** ES688, ES1688, ES1788 and ES1887/ES1888, including on-board chips such as the
  Compaq LTE 5000's and Presario 7100's ES1688 and the Presario 224x's ES1888, and the ES1688 on
  the Gravis UltraSound Extreme and ViperMax.
- **OPTi:** 82C924 and 82C925 in non-PnP mode, and 82C930. On the 82C930 only the IDE CD
  interface is supported, not the Mitsumi, Sony or Panasonic ones.
- **OAK OTI601 "Mozart" and OPTi 82C928 (MAD16):** JazeFox's notes: poor compatibility with
  games, the card may need setting up again after some programs, and SB-mode audio is mono only;
  FM and WSS mode (`/XWSS`) are stereo.
- **OPTi 82C929 (MAD16 Pro):** for example the Boca Research Sound Expression 4294 and SE1440.
  SB Pro mode, with the MPU-401 (BLASTER P and U). Tried on three real cards; see
  [Wanted](#wanted-testers-and-pnp-dumps).

On the OPTi 82C928, 82C929 and 82C930, `/NOPNP /XWSS` sets up Windows Sound System mode instead of
SB Pro mode, as OPTi's own SNDINIT does; OPTi's documentation treats them as one mode at a time.
The WSS base, DMA and IRQ come from `ULTRA16=port,dma,irq,record dma`, which programs such as
Mpxplay read to find the WSS codec, and not from BLASTER. The port is 530, E80, F40 or 604 (hex),
the DMAs 0, 1 or 3 and the IRQ 7, 9, 10 or 11, or also 5 on the 82C930 (the 82C928 and 82C929
use IRQ 7 instead). A missing or invalid field falls back to OPTi's default, 530h, DMA 0 or
IRQ 7, and if the port, DMA or IRQ field was missing or invalid UNISOUND prints the
`SET ULTRA16=` line to add to AUTOEXEC.BAT (`SET ULTRA16=530,0,7,0` without the variable). An
empty field keeps its place (`530,,10` is IRQ 10), and a field with no digits counts as invalid,
so `530,X,10` gets DMA 0 and the `SET ULTRA16=` line. The record DMA is optional. `SET WSS`,
which some OPTi 82C930/931 drivers are said to read, is not used: no format for it was found.
On the 82C928 and 82C929 WSS mode turns the SB part's IRQ off. On a real 82C930 the SB part
keeps answering, but once anything sets up WSS (`/XWSS`, or a program's own WSS driver, which
also works after a plain `/NOPNP`), it plays on the WSS DMA with no IRQ, so SB programs fail
until `UNISOUND /NOPNP` runs again. The same happens on a real
82C931 (PnP) after a program's WSS driver: SB plays with no IRQ until `UNISOUND` runs again.

On GUS PnP (InterWave) cards, UNISOUND reads `ULTRASND=port,dma,dma2,irq,irq2` instead of
BLASTER. The GF1 goes at the port (220, 230, 240, 250 or 260 hex), its second range at port +
100h and the codec at port + 10Ch, with the two DMAs and the IRQ from the variable; without it,
UNISOUND uses 240, DMA 3 and 3, IRQ 11. Limitations:

- The fifth field, the MIDI IRQ, is read but not used: the second IRQ is left off, as are SB
  emulation and the MPU-401.
- The joystick is off unless `/XGJ` is given: without it the gameport logical device is
  deactivated and no joystick port is set up.
- The DMA and IRQ fields are only range-checked (DMA 0-7, IRQ 0-15), not checked against what
  the card supports. An out-of-range field keeps the default.
- With fewer than three fields, or with no valid field, UNISOUND says it is loading its defaults,
  but any valid field given is still used.
- An empty field keeps the default for its place: `ULTRASND=250,,5,7` gives DMA 3/5 and IRQ 7.
  Before 1b27daf the later fields moved up one place (DMA 5/7, IRQ 11). A field with no digits,
  such as the X in `240,X,3,11`, is ignored too (13ef91c); before, it set 0.
- No `SET ULTRASND=` line is printed when the variable is missing, as there is for `ULTRA16`.
- `ULTRADIR` and Gravis's other variables are neither read nor set.
- On the GUS Extreme and ViperMax (`/NOPNP`), UNISOUND sets up only the ES1688, from BLASTER; the
  GF1 part and its `ULTRASND` are left to Gravis's software. Non-PnP GUS cards aren't supported.

This list comes from the source and hasn't been checked on a real card; see
[Wanted](#wanted-testers-and-pnp-dumps).

## Options and variables

UNISOUND.TXT, JazeFox's manual, comes with every build and describes the options (section 3 and
the help screen), the `/X` options (section 5) and the BLASTER fields. This section adds what
the source shows beyond the manual, and what these builds change. `UNISOUND /H` lists the
options too.

### Command-line options

- `/Cn` selects PnP card n, from 1 to 9. `/C0` and `/C10` are unknown options (ERRORLEVEL 2).
  With n above 1 and no card n, UNISOUND prints `Card #n not found.`, runs no non-PnP fallback
  and exits with ERRORLEVEL 1.
- `/CL` lists the PnP cards with their names and configures nothing (ERRORLEVEL 0).
- `/CS` sets up a non-PnP SB16 or AWE32 without the PnP scan. With `/CL`, or with `/Cn` for n
  above 1, UNISOUND prints `/CS cannot be used with /CL or /Cn.` and exits with ERRORLEVEL 2
  (3a339b7, c683a68); before, which one happened depended on the order of the options.
- `/F` and `/FS` are for a PnP card that isolation doesn't find at first: `/F` sets the read
  data port on every try, and `/FS` also reads every isolation bit, a slower scan. The manual
  mentions `/FS` for some Terratec AD1816 and Pinnacle cards; its v0.80a notes say `/F` was taken
  off the help screen but keeps working. These builds list both on the help screen.
- `/R` reads the card's settings back and writes nothing: on PnP cards, and with `/CS` and
  `/NOPNP` on the non-PnP SB16, ES688/ES1688-family chips and OPTi 82C928, 82C929 and 82C930.
  `/XSLAM /R` and `/XESSB /R` are refused (`/R can't read a chip set up through SLAM.`, `/R can't
  read a chip found through the ESS bypass key.`, ERRORLEVEL 1). With `/D` too, `/R` wins, and
  once a card is found UNISOUND says `/R given: /D ignored, the card will only be read.`
  (c65bb4a).
- `/D` turns a PnP card's logical devices off (`Card disabled.`, ERRORLEVEL 0). The OPTi 82C924
  and 82C925 in non-PnP mode are set up through their PnP registers, so `/D` turns them off too. A
  non-PnP card has nothing to turn off: on the SB16 and AWE32 (`/CS`, `/NOPNP` or the fallback
  after the PnP scan), ES688, ES1688, ES1788 and ES1887/ES1888, the OPTi 82C928, 82C929 and
  82C930 and the OAK OTI601, UNISOUND prints `Non-PnP card, can't be disabled. Card left
  unchanged.`, writes nothing and exits with ERRORLEVEL 1 (5603724). Before, `/NOPNP /D` set the
  ESS and OPTi chips up as if `/D` weren't there, and every route exited with 0.
- `/Vnn` and the other volume options take 0 to 99 as one or two digits, or as three digits
  with a leading zero: `/V099` is 99 and `/VW010` 10 (e6c7f3a). `/V100` and anything else print
  `Invalid value: /V100 (0-99)` and exit with ERRORLEVEL 2.

Without `/V` options the volumes are: master 85, FM and wave 80, line-in, CD, microphone and PC
speaker 0. (The manual's notes for an older version give line-in 50.) Treble and bass have no
default value of their own: Yamaha cards get 0, the TEA6330T on the Compaq InterWave and STB
Ultrasound Pro 32 gets 50 (as the manual says), cards with an SB16 mixer get its middle setting
(80h) and show 50, and on ESS cards AuxB follows the FM volume unless `/VB` is given.

### Extended options (`/X`)

Section 5 of UNISOUND.TXT describes them. The source applies some of them to more cards than
the manual names, or to fewer:

- `/X3Dn`: as the manual says, and also on the OPTi 82C931 (the same setup as the 82C933). On
  Crystal cards only the Orpheus, the Turtle Beach Malibu and cards whose ID ends in 37 get it. On
  ALS cards, those with `ALS??07`, `ALS0001` and `RWB1???` IDs don't. On Yamaha cards n is cut
  to 0-7, so 8 gives 0 and 9 gives 1. On Creative cards it sets SB16 mixer register 90h, also on
  the non-PnP SB16.
- `/XXGn`: on cards with an SB16 mixer (Creative, also the non-PnP SB16, C-Media and most ALS
  cards) 1 gives gain x1, 4 gives x4 and anything else x2. On C-Media cards any n but 0 turns
  loudness on.
- `/XWSS`: besides the cards the manual lists, the ADS7141 (its SB Pro mixer is then left alone)
  and, with `/NOPNP`, the OPTi 82C929 and 82C930 (see the OPTi paragraph above).
- `/XOS1` and `/XOS0` work on the Orpheus and the Turtle Beach Malibu only. `/XOFI` and `/XOFE`
  work on the Orpheus (default external OPL3) and on cards whose ID ends in 37 (default
  CrystalFM), not on the Turtle Beach Malibu. `/XOM1` and `/XOM0` work on every Crystal card.
- `/XGC5` is accepted besides the manual's `/XGC2` to `/XGC4`; it sets no IDE port.
- `/XNET` also works after the ES1869 setup (and the ES1868 one with `/XEA`), and with `/XESSB`
  and `/XSLAM`: UNISOUND moves the PnP ports back before it looks for the network card (08c6ca6).
  Before, the workaround went to the ESS control port and the network card stayed off.
- `/XSLAM` and `/XESSB` are these builds' own; see [Wanted](#wanted-testers-and-pnp-dumps).
  Together they print `/XSLAM cannot be used with /XESSB.` and exit with ERRORLEVEL 2 (3ece608).
  `/XESSB` also accepts the ES1878 and ES1879 and names them `[ESS1878]` and `[ESS1879]`
  (2aa4b38). That comes from their data sheets and is untested: 86Box has neither chip.

### BLASTER fields

The manual lists the fields. In the source they take these values; anything else is ignored:

| Field | What | Values |
|---|---|---|
| `A` | SB port | 220h-280h |
| `I` | SB IRQ | 0-15 |
| `D`, `H` | first and second DMA | 0-7 |
| `P` | MPU-401 port | 300h-3F0h, or 0 |
| `E` | EMU8000 port | 620h-680h, or 0 |
| `J` | game port | 200h-211h, or 0 |
| `W` | WSS port | 100h-FFFh, or 0 |
| `C` | IDE port | 168h, 1E8h, 170h, or 0 |
| `Y` | IDE IRQ | 2-15 |
| `R` | control port | 100h-FFFh, 2100h-21FFh (Ensoniq), or 0 |
| `M` | second MPU-401 port (Maxi Sound 64) | 300h-380h, or 0 |
| `F` | FM port | 300h-3FFh, or 0 |
| `U` | MPU-401 IRQ | 0-15 |
| `Q` | second IRQ | 0-15 |
| `K` | WSS DMA | 0-7 |

`I`, `U`, `Q` and `Y` are decimal, the others hex. `T` is read but not used. `R` sets the
Yamaha, ESS and other control ports at once. Changes in these builds:

- Up to 20 fields are read; 0.82a read 12, so a full BLASTER lost its last fields (010dcc2).
  Every word counts towards the 20, also `T` and unknown ones.
- A field with no digits after its letter (`IX`, `CX`) is ignored (13ef91c). Before, it set 0,
  so `CX` turned the IDE port off. The same goes for `UNISOUND_CD`, `ULTRASND` and `ULTRA16`.
- `W` and `R` are range-checked (11568cc). Before, they took any value, so `W9999` or `R40`
  aimed the codec or control port writes at whatever answers there, such as the timer at 40h.
- UNISOUND says `BLASTER environment var found! Loading settings...` only when at least one
  field was used, and `Loading Unisound default settings...` otherwise, for example for
  `BLASTER=T6` (06c6818). The same goes for `ULTRASND`.

### Exit codes

UNISOUND exits with ERRORLEVEL 0 when a card was set up, read (`/R`) or turned off (`/D` on a
PnP card), and after `/CL` and `/H`. It exits with 1 when nothing was set up:

- no card was found, by the PnP scan or by the non-PnP fallback after it;
- `/Cn` found no card n, or an unsupported one (`Aborting...`);
- a supported PnP card's resource data couldn't be read (6ea6f2a; before, ERRORLEVEL 0), and
  the fallback found nothing either. The fallback runs only if no card was listed or skipped
  first, so not with `/Cn` for n above 1;
- `/D` found no card (6ea6f2a; before, 0), or only a non-PnP card (5603724, see `/D` above);
- `/CS`, `/NOPNP`, `/XSLAM` or `/XESSB` found no card, or `/R` was refused with `/XSLAM` or
  `/XESSB`.

It exits with 2 for an unknown option, an invalid `/V` value, `/CS` with `/CL` or `/Cn`, and
`/XSLAM` with `/XESSB`.

### Other changes to the non-PnP setups

- Non-PnP SB16: BLASTER `I9` sets IRQ 9 (29da20b). It is the ISA bus's IRQ 2 line, so `/R`
  shows `IRQ:2`. v0.82a set IRQ 5 for `I9`.
- OPTi 82C928, 82C929 and 82C930: an IRQ or DMA the chip doesn't have is still replaced by IRQ 5
  or DMA 1, and UNISOUND now says so, for example `IRQ 3 not possible on OPTi 82C929, using 5.`
  (29da20b). The 82C928 and 82C929 take IRQ 5, 7 and 11 (82C928) or 10 (82C929), the 82C930
  IRQ 5, 7, 9, 10 and 11, and all three DMA 0, 1 and 3.
- ES1688, ES1788 and ES1887/ES1888: `P0` turns the MPU-401 off, and the Settings line shows
  `MPU:0` (8470856). Before, the MPU-401 stayed on at 300h.
- ES688/ES1688-family chips: if the chip reads back no DMA after the setup, UNISOUND prints `DMA
  n didn't take: the chip reads back no DMA.`, shows DMA 4 (none) as `/R` does, and suggests no
  BLASTER line (ac82477). Untested on a chip that does this: 86Box always takes the DMA.

## Planned: CD-ROM interfaces

Many of these cards have CD-ROM drive connectors: IDE, or Mitsumi, Sony or Panasonic. So far
UNISOUND sets up only IDE: on PnP cards that describe it as a logical device, and on the OPTi
82C924, 82C925 and 82C930 in non-PnP mode. BLASTER's `C` field gives the port and `Y` the IRQ (see
UNISOUND.TXT); on the GUS PnP, which reads ULTRASND instead, `/XGCn` sets the port. In 86Box
an ATAPI CD-ROM drive answers, with its IRQ, wherever UNISOUND puts the IDE port on the SB16
PnP, AWE32 PnP and AWE64 with IDE, the GUS PnP and the OPTi 82C931 (checked with
`tools/dos/ATAPIID.C`). The 82C931's resource data lists the IDE control port before the
command block, so the control port goes in register 60h; UNISOUND follows the order the
resource data gives. On real cards, the CT3600 (SB32 PnP) turns its IDE port on with
`C168 Y10` (no drive connected), and the Wavemaster 32 FGS (82C930) runs a CD-ROM drive (see
below).

The 82C930's IDE is fixed at 170h (secondary IDE) on IRQ 15, so `/NOPNP` takes only `C170`,
which turns it on, and `C0`, which turns it off; without `C` it stays as it was (for example
as OPTi's CDSETUP.SYS left it), and `Y` is ignored. The settings line shows `IDE:170/I15` while
it is on. As OPTi's CDSETUP.SYS does, UNISOUND writes the CD-type bits of control register 1
(data sheet values 000 none, 100 secondary IDE) with bits 2 and 1 inverted, 011 and 111; the
chip reads them back inverted again (100 for IDE), so a kept value is written back flipped. The
board's own secondary IDE channel must be off in its BIOS setup, as it uses the same ports. On
a Silicom Wavemaster 32 FGS with a Sony DDU1615 DVD-ROM drive, `C170` turns the port on and the
drive answers on IRQ 15 and reads through OAKCDROM.SYS and MSCDEX; after OPTi's CDSETUP.SYS,
UNISOUND without `C` leaves the drive answering. 86Box's 82C930 has no IDE and doesn't invert the bits on reads.

On the 82C924 and 82C925 the IDE is a PnP logical device, which UNISOUND sets as OPTi's
CDSETUP.SYS v1.04 does: command port in register 62h, control port in 60h, IRQ in 70h, and
the CD-type bits of control register 1 left at 000. Without `C` the device is left as it was
(for example as CDSETUP.SYS set it), and `/D` turns it off. 0.82a and fix-4 wrote the
82C925's control port over the low byte of the command port, so `C170` there gave a command
port of 103h. Untested: no 82C924 or 82C925 card was available, and 86Box has neither.

`C` and `Y` are UNISOUND's own BLASTER fields, and a program that parses every BLASTER field
might reject them or misread the rest. If that happens, take them out of BLASTER and put them
in `UNISOUND_CD` instead, for example `SET UNISOUND_CD=C170 Y15`. UNISOUND reads it after
BLASTER, so if both set a field, `UNISOUND_CD` wins.

Proprietary interfaces, and the CD interface on other non-PnP chips, are left as
they are (on the 82C929, for example). The aim is to get the
CD-ROM connectors working on every supported card. Reports of how a card's CD interface behaves
with its vendor's driver are welcome.

### Loading the CD-ROM driver

UNISOUND runs from AUTOEXEC.BAT, after CONFIG.SYS has loaded its drivers, so a CD-ROM driver in
CONFIG.SYS looks for the drive before UNISOUND has turned the port on. There are three ways
around it:

- **DEVLOAD** (as UNISOUND.TXT suggests): leave the CD-ROM driver out of CONFIG.SYS and load
  it from AUTOEXEC.BAT after UNISOUND with a loader such as FreeDOS's DEVLOAD (DOS 3.00 and
  later), for example:

      SET BLASTER=A220 I7 D3 P330 T3 C170
      UNISOUND /NOPNP
      DEVLOAD OAKCDROM.SYS /D:MSCD000
      MSCDEX /D:MSCD000

  Checked on the Wavemaster 32 FGS (82C930) under MS-DOS 6.22.
- **The card's own CD setup driver in CONFIG.SYS** (82C930; after fix-4 also the 82C924
  and 82C925), needing no other tools:
  OPTi's `CDSETUP.SYS /T:I` before the CD-ROM driver turns the IDE port on, and UNISOUND in
  AUTOEXEC.BAT without `C` leaves it on. On the Wavemaster 32 FGS the drive still answers
after UNISOUND. On other PnP cards
  UNISOUND turns the IDE port off when `C` is missing, so this doesn't apply there.
- **A warm reboot**: run UNISOUND with `C`, then press Ctrl+Alt+Del, so that the CD-ROM driver
  in CONFIG.SYS finds the port already on. This works only on motherboards that leave ISA cards
  alone on a warm reboot. The Intel SE440BX-2 resets them, so there the port was off again.

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
| ESS0100, ESS0102 | ESS ES688/ES1688 PnP cards | ES968 layout (SB and FM on LD0), MPU-401 on LD1 with its own IRQ, game port LD2, IDE LD3 | only 86Box's `ess_ess0100_pnp` and `ess_ess0102_pnp`, whose PnP data is a reconstruction, not a dump from a real card. A `UNISOUND /LOG` from a real card, and whether sound, FM and MIDI work, would settle it |

The OPTi 82C929 (`/NOPNP`) is no longer in this table: a Boca Research SE1440 (82C929), a NewCom
High Fidelity 16i and a Turtle Beach Monte Carlo (both 82C929A) work in SB mode, and the Monte
Carlo also in WSS mode (`/XWSS`); see [TESTING.md](docs/TESTING.md#real-hardware-with-fix-4).
In 86Box, `acermagic_s20` and `mirosound_pcm10` get the same registers as from OPTi's own
SNDINIT. WSS reports from other 82C929 cards are still wanted.

### Held back: need a PnP dump first

| Card ID | Card | Would be set up as | What we need to know |
|---|---|---|---|
| MIR0924 | miro PCM12 / PCM20 (OPTi 82C924) in PnP mode | OPTi 924 | Whether the logical devices sit where OPTi's OPT0924 has them (Linux lists MIR0000/0002/0005 like OPT0000/0002/0005) |
| BVJ0440 | Turtle Beach Multisound Pinnacle (rev F?) | Pinnacle | Where the joystick and IDE devices are; UNISOUND's Pinnacle code expects the joystick on logical device 2, Linux's non-PnP setup has IDE there |
| ENS3081 | Ensoniq Soundscape OPUS rev A (Gateway 2000 OEM; fix-4 skips it as unknown) | Soundscape PnP: one logical device, ENS0000, with the gate array at its port + 4, the WSS codec at port + 8, a WSS/SB IRQ, a MIDI IRQ and one DMA (Linux `snd-sscape`) | Nothing more from a dump: vintagefugg's rev A gave its resource data with fix-4-rc11's `/LOG` (ENS0000 at 330h or 350h, 16 bytes; a first IRQ of 5, 7 or 9, a second of 7, 9 or 10; DMA 0, 1 or 3; see [TESTING.md](docs/TESTING.md#real-hardware-with-fix-4)). What's left is the work below, tested on a real card |

The Soundscape OPUS stays unsupported for now. What could be added later, in this order:

1. PnP setup: assign ENS0000's port, IRQs and DMA, as a PnP BIOS does, so that Ensoniq's
   `SSINIT /I` works on machines without one.
2. WSS: turn on the OPUS chip's AD1845 codec and program the gate array's IRQ and DMA routing,
   as Linux does before its firmware load. Ensoniq's DOS drivers don't set the card up for WSS;
   under DOS it's used through SB emulation, MIDI and games' native Soundscape support.
3. MIDI and the synth: load Ensoniq's firmware (SCOPE.COD and SNDSCAPE.CO0-CO4, from the
   directory in `SET SNDSCAPE=`), as UNISOUND already does with SAM9407.BIN on the Maxi Sound 64.
   The files are Ensoniq's and would not be included.

Sound Blaster emulation is left out: it runs on the card's firmware and plays FM music on the
wavetable synth, which players report sounds poor, and it might need a TSR. SSINIT leaves 720
bytes resident (MEM /C), and what that part does isn't known yet; UNISOUND isn't a TSR. For Sound
Blaster games the card is better paired with a real SB-compatible card. Linux's driver needs
nothing resident for 1-3: it loads the firmware and then uses the card's MPU-401 port directly.

### Options not yet tried on a machine that needs them

These find chips that PnP isolation misses. They work in 86Box, but no one has reported back from
a machine that needs them yet. Please include `UNISOUND /LOG` output with the option.

| Option | For | Machines that need it |
|---|---|---|
| `/XSLAM` | Crystal CS423x chips whose PnP key is disabled, so they take no part in isolation. They get the generic Crystal variant: no CS4232 firmware upload, no Orpheus/Turtle Beach/CS4237 extended setup, no MODE-1 return | Laptops and boards whose BIOS or CWDINIT sets up the chip, where UNISOUND says "No PnP card detected." |
| `/XESSB` | ES1868/ES1869 chips that isolation doesn't find (also ES1878/ES1879, from their data sheets; untested). Without BLASTER `R` it tries control port 660h, then 800h | On-board ES1868/ES1869, or chipsets that don't pass A79h on to the chip; 86Box always passes it on, so only a real machine can test this. On a real ES1898F card in a Gateway 2000 4DX2-66 the key was taken and the chip set up, but isolation finds that card too |

### Fixes for VOGONS reports, not yet tried on real hardware

These fix problems reported on VOGONS and work in 86Box, but no one has tried them on a real card
yet. Details and links to the reports are in
[docs/changes_since_official_082a.md](docs/changes_since_official_082a.md). Please include
`UNISOUND /LOG` output.

| Commit | Card | Fix | Reported by |
|---|---|---|---|
| [00806ef](https://github.com/dafugg/unisound_continued/commit/00806ef) | Any card | After setup, BLASTER's A, I, D and H are compared with what the card got | analog_programmer |
| [5281f0f](https://github.com/dafugg/unisound_continued/commit/5281f0f) | Crystal CS4237B-class cards (Orpheus, Turtle Beach, and cards whose PnP ID ends in 37) | The codec is put back in MODE 1 | vanfanel |
| [1adfd17](https://github.com/dafugg/unisound_continued/commit/1adfd17) | ES1868/ES1869 | BLASTER `U` sets the MPU-401 IRQ | crvs |
| [f73bb3f](https://github.com/dafugg/unisound_continued/commit/f73bb3f) | ES1868/ES1869 | `H4`, or D = H, means no second DMA | Kahenraz |
| [26f3356](https://github.com/dafugg/unisound_continued/commit/26f3356) | ES1887/ES1888 with `/NOPNP` | The IRQ the BIOS set up is kept | 0xDEADBEEF |
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
- **If it happens only with the build from `main`**, post in
  [the VOGONS thread for these unofficial builds](https://www.vogons.org/viewtopic.php?p=1456858)
  or open an issue here. Include the version line UNISOUND prints, your card, your BLASTER or
  ULTRASND setting and the command line.

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
6. **A dump, if UNISOUND doesn't find or support your card, or we ask for one**:
   `UNIDUMP > DUMP.TXT`. UNIDUMP.EXE comes with fix-4 and later builds. It records the
   PnP BIOS's devices, every PnP card's resource data and settings, the non-PnP sound chips it
   can find, who owns each IRQ, the drivers and TSRs loaded, CONFIG.SYS and AUTOEXEC.BAT, and a
   short summary of the CPU, memory, BIOS and PCI devices. It shows each step on screen as it
   goes; if it hangs, tell us the last one and run it again with `/SKIP:` and that section's
   name (`UNIDUMP /?` lists them). `UNIDUMP /NOFILES > DUMP.TXT` leaves CONFIG.SYS and
   AUTOEXEC.BAT out if you'd rather not share them.

For more about the machine itself, MS-DOS 6.x's `MSD /F MSD.TXT` writes Microsoft Diagnostics'
whole report to a file, and the freeware HWiNFO for DOS can save a report too. They add to
UNIDUMP rather than replace it: UNIDUMP shows the PnP cards and sound chips the way UNISOUND
sees them.

`/XESSB` and `/XSLAM` skip the PnP scan, so they never read the card's ID. `/XSLAM` prints a
fixed name, and `/XESSB` names the chip from the part number it reads, `[ESS1868]` to
`[ESS1879]`. The same card can show up under different names: an ES1898F card is `ESS0003`
normally and `[ESS1869] (ESS bypass key)` with `/XESSB`, because the chip reports itself as an
ES1869.

### What `/LOG` shows

`/LOG` adds lines starting with `LOG: ` that show what UNISOUND found and wrote:

- the DOS version and the command line; BLASTER as given and every field as parsed, UNISOUND_CD
  and ULTRASND;
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
- with `/XESSB`: for each control port tried, the E7h reply and the four mixer 40h reads after the
  bypass key.

### ESSINIT

For an ES688/ES1688-family chip that doesn't work, `tools/dos/ESSINIT.C` sets the chip up the
way ESSCFG does, from BLASTER's A, I and D, and prints each step: the enable reads if needed,
reset, E7h, B1h/B2h before, written and read back, and an F2h interrupt test. `ESSINIT /OLD`
writes B1h/B2h as v0.82a does, for comparison. It exits with 0 when the chip took the IRQ and DMA
and interrupted on that IRQ. Without BLASTER it uses UNISOUND's defaults, A220 I5 D1. It first
runs v0.82a's `/NOPNP` OPTi probe and says whether v0.82a would take the machine for an OPTi
82C924/925 and skip the ESS detection.

## Building

You need **Turbo C++ 3.0** (TCC.EXE of February 1992, with its own C0M.OBJ and CM.LIB, and C0T.OBJ
and CS.LIB for the `0.82a` branch).
Borland C++ 3.0 and 3.1 produce slightly different code. In DOS, with `C:\TC\BIN` on the PATH and
`C:\TC\BIN\TURBOC.CFG` containing `-IC:\TC\INCLUDE -LC:\TC\LIB`:

    copy src\UNISOUND.C .
    BUILD.BAT

`BUILD.BAT` runs:

    TCC -c -mm -O -d UNISOUND.C
    TLINK /c C:\TC\LIB\C0M UNISOUND, UNISOUND, , C:\TC\LIB\CM
    IF EXIST UNISOUND.COM DEL UNISOUND.COM
    REN UNISOUND.EXE UNISOUND.COM

On `main`, UNISOUND.COM is a medium-model EXE: it starts with an `MZ` header, and DOS (2.0 and
later, FreeDOS too) loads it as an EXE whatever its extension, so `UNISOUND` lines in AUTOEXEC.BAT
keep working. The data has one 64K segment; the code is in two far segments, one for UNISOUND.C and
one for the runtime library, so UNISOUND.C has a 64K code segment of its own. fix-4 was a
small-model EXE, with all the code in one 64K segment. v0.82a was a tiny-model .COM, with code,
data and stack in one 64K segment, and the fixes had filled it. The `0.82a` branch still builds the
tiny-model .COM (`TCC -c -mt`, `TLINK /t` with C0T).

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
| `src/UNISOUND.C` | The whole program in one module, as in the original: 111 functions (79 in 0.82a) and all initialized data |
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
- `dos/`: the DOS test programs used for the fixes (PNPDUMP, ESSREG, DMAPLAY, WSSPLAY, OPLDET, SBST, TRACEPNP, PSSDUMP, ATAPIID), UNIDUMP (one dump of the sound, PnP and DOS state of a machine), ESSINIT (ES688/ES1688 setup test) and the prototypes of `/XESSB` and `/XSLAM` (ESSBYP, CSLAM); `pnpdecode.py` decodes TRACEPNP logs. See `docs/TESTING.md`.

The analysis tools need Python with capstone. They also need the original v0.82a UNISOUND.COM,
which this repo does not include: put it in `orig/` or set `UNISOUND_COM`.

## License

The tools, DOS test programs, build files and documentation are under the MIT license (see
[LICENSE](LICENSE)). `src/UNISOUND.C` and `data/*.json` are not: they are reconstructed from, or
derived from, UNISOUND by JazeFox, and remain JazeFox's work.
