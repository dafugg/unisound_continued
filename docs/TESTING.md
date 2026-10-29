# Testing in 86Box

All results here come from emulation in 86Box. Six fixes have since also been confirmed on real
hardware: the ULTRASND and BLASTER parsing, option names (bug 12), unknown PnP cards (bug 11),
the ESS DSP flush (bug 10) and the ES1688 enable reads (bug 19). All but the unknown PnP cards ran
on a Compaq LTE 5000 with its on-board ES1688; the README table marks them.

## Environment

| Component | Version |
|---|---|
| 86Box | built from source (github.com/86Box/86Box), with the automation socket |
| ROM set | github.com/86Box/roms |
| 86box-mcp | github.com/dafugg/86box-mcp |
| Original build | UNISOUND.COM v0.82a, sha256 `606f8fae…b370` |
| Fixed build | branch `fixes` at `17868c1` (bugs 1-10) |
| Thread fixes build | branch `thread-fixes` at `50253a7` (bugs 11-24); each fix was tested with the build of its own commit |
| ESS init build | branch `ess-init`: `aee160e` (B1h/B2h through 2xCh), `05aa0de` (`/LOG`), `8afa28d` (ESSINIT), `d2f397b` (OPTi 82C924/925 probe), `4197954` (82C928 probe), `f91b9bc` (ESSINIT OPTi probe) |

Platforms, both a 486DX2-66 with 16 MB of RAM, booting MS-DOS 6.22:

- **pnp**: ASUS PVI-486SP3C (`486sp3c`), Award 4.51PG BIOS with the Plug and Play extension. The
  BIOS configures ISA PnP cards at boot.
- **nopnp**: Olystar LIL1429 (`ali1429`), AMIBIOS 04/04/93 with no PnP support, plus an ISA IDE
  controller. PnP cards start unconfigured.

For the ES1887/ES1888 (bug 20) the same DOS disk, with an MS-DOS MBR, was booted on a Compaq
Presario 224x (`presario2240`), whose on-board ES1888 the BIOS enables and puts on IRQ 5. 86Box
has the ES1887/ES1888 only as on-board devices.

For the Compaq on-board ES1688 (bugs 10 and 19 on a non-PnP laptop-style chip) the same disk was
booted on the two Compaq Presario 7100 machines, `pl4600c` (486DX2-66, PhoenixBIOS 4.04
PL4600C-II R0.09) and `pl5600d` (Pentium 75, PhoenixBIOS 4.04 PL5600D-I R1.40), both with 16 MB,
on-board video and IDE. They are the only 86Box machines with `ess_es1688_compaq`. Their Setup
has Integrated Peripherals > ESS1688, enabled at 220h, IRQ 5, DMA 1 by default; with it enabled
the BIOS sends the ES1688 enable reads and resets the DSP, so the chip answers at 220h before DOS
starts. With it disabled the chip stays waiting for the enable reads and every port reads FFh.
A fresh CMOS has no hard disk set and the BIOS doesn't detect one at POST ("Operating system not
found"): in Setup, Fixed Disk Setup > IDE Adapter 0 Master > Autotype, then Main Menu > "Save
Values to CMOS" (F10 didn't keep the drive type).

Some cases need more than one card or extra devices (a network card, the standalone PnP game
port, a second sound card), set through extra `86box.cfg` keys; a few need a patched card
EEPROM, such as the `cs4236b` image with the CS4237B ID (`CSCA737`, `CRD4237B-3`) used for bug
18, since 86Box has no selectable CS4237B.

`tools/cardtest.py` copies a platform's base VM (reflinked disk image and CMOS), sets `sndcard`,
boots, and runs each command with its output redirected to a file. A command that doesn't return
within 120 s of emulated time is recorded as a hang. `tools/dos/PIO.C` is a port I/O tool used to
read hardware state back.

## Baseline: original v0.82a against every 86Box sound card

Commands per run: `/CL`, `/R`, `UNISOUND`, `/R`, `/NOPNP`, `/CS`. The 84 ISA entries (including
"none") ran on both platforms and the 7 PCI cards on pnp: 175 runs. The 8 MCA cards and 3 AC'97
codecs can't be fitted to these machines.

| Outcome | Cards |
|---|---|
| Configured and read back | sb16_pnp, sb16_pnp_ide, sb32_pnp, sbawe32_pnp, sbawe32_ide_pnp, sbawe64, sbawe64_ide, sbawe64_value, sbawe64_gold, sb_goldfinch, sb_vibra16c, sb_vibra16cl, sb_vibra16xv, guspnp, guspnp_new, guspnp_nocd, guspnp_compaq, cs4232, cs4236b, ess_es1868, ess_es1869, ess_ess0968_pnp, ess_ess0968_pnp_es688, ad1816, opti_82c931, ymf718, ymf719, azt2320 (nopnp only) |
| "Unknown PnP card", aborts | cs4235, ess_ess0100_pnp, ess_ess0102_pnp (ids UNISOUND doesn't support) |
| Non-PnP SB16 fallback | sb16, sbawe32, sb_vibra16s |
| Not detected | the other ISA cards (none of them supported) and all PCI cards |
| Did not boot | azt2320 on pnp: the Award BIOS hangs at "Initialize Plug and Play Cards" (see the emulation notes) |

No UNISOUND run hung. The results were the same on both platforms apart from the first `/R`,
which shows the BIOS's configuration on pnp and an unconfigured card on nopnp.

### Emulation notes

- **OPTi 82C930** is not found by `/NOPNP`. UNISOUND identifies the chip by reading control
  registers 1, 3 and 0x12 (index 0x12 must be below 8). 86Box implements only MC1-MC12 for the
  930 and returns FF for index 0x12, so the check fails. Registers 1 and 3 read correctly with
  UNISOUND's password sequence (E4 to 0xF8F before the index write and again before the data
  access). The 82C930 datasheet (912-3000-022) also documents only MC1-MC12, so what a real
  930 returns at 0x12 is unknown, and neither side can be called wrong without real hardware.
- **EMU8000**: 86Box turns byte writes to the pointer register into word writes, and returns
  odd-address byte reads shifted right by 1 instead of 8. Use word I/O (`PIO O`, `PIO w`).
- **AZT2320** hangs the pnp board's BIOS before DOS starts
  ([86Box#8552](https://github.com/86Box/86Box/issues/8552)). The card's PnP data has a
  logical device "IDE CDROM DISABLED" with two I/O descriptors of base 0 and length 0. When
  this Award BIOS looks for a free range, it tries base 0, finds port 0 in use, and works out
  the next base as (0 + length + alignment - 1) rounded down to the alignment, which is 0
  again, so it loops forever. Changing the two lengths to 1 in a copy of the ROM lets it boot.
  Seven other PnP BIOSes in 86Box (four Award, two Intel/AMI, one Acer) boot with the
  unmodified card and configure it. The cause is this BIOS, not UNISOUND, which configures
  the card on nopnp.
- **ESS register 28h** (bug 15): 86Box's ES1868/ES1869 have no handler for the card-level
  vendor registers, so 28h ignores writes and reads back FFh, and `/R` shows the MPU IRQ as
  `I15`. 86Box always gives the MPU-401 the SB IRQ.
- **Yamaha control register 06h** (bug 24): 86Box applies 03h and 06h only when the PnP
  configuration changes, so after the first UNISOUND the WSS DMA still follows the power-on value
  61h. The second run gives the routing real hardware should have after the first.
- **ES1688 on the GUS Extreme and GUS ViperMax**: 86Box keeps it disabled until the ES1688
  enable reads, so `/NOPNP` finds it only after bug 19's fix.
- **ES1869**: init's gameport and IDE settings don't take effect on either platform
  ([86Box#8554](https://github.com/86Box/86Box/pull/8554)). UNISOUND configures this chip
  through ESS's bypass key: the key and the address 660h go to 279h, and the PnP registers are
  then reached through 660h/661h. `CTR:660` is that address. The ES1869 data sheet (Bypass Key,
  page 28) says the key sets the configuration device's (logical device 0) address register
  and activates it. 86Box moves the ports to 660h but leaves logical device 0's registers at
  their old values: 0 on nopnp, the BIOS's 800h on pnp. When UNISOUND then activates logical
  device 0, 86Box moves the ports to that stale address, so the gameport and IDE writes that
  follow are lost. `/R` then shows `CTR:0 JOY:0` on nopnp. On pnp it shows the BIOS's
  `CTR:800 JOY:201 IDE:168/I11`, so the gameport only looks right. A test program that follows
  the data sheet, without UNISOUND, shows the same thing. With the fix in #8554, `/R` after init
  shows `CTR:660 JOY:201 IDE:0/I0` on both platforms, and nothing else in the ES1868 and ES1869
  results changes.

## Bugs reproduced and fixed

Each case ran with the original and the fixed build on both platforms.

| Bug | How it was reproduced | Original | Fixed |
|---|---|---|---|
| 3 ULTRASND fields | guspnp, `ULTRASND=240,1,1` | `IRQ:0`, and `/R` reads IRQ 0 | `IRQ:11` |
| 3 control | guspnp, `ULTRASND=240,1,1,11,0` | `IRQ:11` | `IRQ:11` |
| 6 ANSI string length | sb16_pnp with its PnP ROM patched to a 150-character card name | `/CL` hangs | name cut to 63 characters, init and `/R` work |
| 8 reverb send sign | sbawe32_pnp, `/VR99`, then read PTRX of voice 31 (pointer 0x3F) | `FFFF FFFF` | `0000 FFFF` |
| 8 control | sbawe32_pnp, `/VR40` | `0000 67FF` | `0000 67FF` |
| 9 ROM bank name | guspnp with IWROM.BIN's bank name set to 16 characters | `[AMDGM_1_1_FULL16INTR]` | `[AMDGM_1_1_FULL16]` |
| 10 ESS DSP flush | ess_es1688, `/NOPNP` twice | second run: "No card detected" | detected both times |
| 11 unknown PnP card first | sb16 + 3C509B (TCM5094), sb16 + PnP game port (BOX0002), cs4235 (CSCA725) + sb16 at 240, ess_ess0100_pnp + sbawe32_pnp | "Unknown PnP card [...] found. Aborting...", ERRORLEVEL 1; the SB16 and the AWE32 untouched | "Card #1 [...] Unknown, skipped.", then the SB16 fallback or the AWE32 configured |
| 11 `/Cn` past the last card | the same setups, `/C3` | "No PnP card detected.", ERRORLEVEL 0 | "Card #3 not found.", ERRORLEVEL 1 |
| 12 option names | sb16_pnp, `/DUMMY` | card disabled, ERRORLEVEL 0 | "Unknown option: /DUMMY", ERRORLEVEL 2 |
| 12 option names | sb16_pnp, `/XAE`, `/XEM`, `/CN1`, `/C0`, `/C10`, `/FOO`, `/NOTHING`, `/HELLO`, `/RX`, `/VF123`, `/XGC1`, `/XPW1`, `/QQQ`, `C2` | ignored or misread, ERRORLEVEL 0 | rejected, ERRORLEVEL 2 |
| 12 control | every documented option, on sb16_pnp, guspnp, azt2320, ess_es1868, ymf719, cs4236b and sbawe32_pnp | | same output as before |
| 12 one-digit volumes | sb16_pnp, `/VL0 /V5 /VW99 /VT99 /VB7`; then `/VW100`, `/VL123`, `/VLX`, `/V`, `/VX` | `/VL0` and `/V5` ignored (v0.82a), or "Unknown option" (e75475b) | `VOL:5 LIN:0 TRB:99 BAS:7`; "Invalid value: /VW100 (0-99)" etc., ERRORLEVEL 2; `/VX` "Unknown option" |
| 13 non-PnP SB16 `/R` | sb16, sbawe32, `BLASTER=A220 I7 D0 H6`, mixer 80h/81h read with PIO | `/R` sets them from 02/22 to 04/41 | unchanged, "Reading card current settings..." |
| 13 non-PnP SB16 `/D` | the same | the card is configured | "Non-PnP card, can't be disabled. Card left unchanged." |
| 14 ES186x second DMA | ess_es1868, ess_es1869, PnP 75h read with PNPDUMP | H4 after H3: 75h = 03, `DMA:1`; D1 H1: 75h = 00 | 75h = 04, `DMA:1/4`, in both cases |
| 15 ES186x MPU IRQ | ess_es1868, ess_es1869, PnP writes logged with TRACEPNP | U9, U0: no write to 28h | 28h = 09, 00; 00 for P0 and `/D`; nothing written under `/R` |
| 16 names | ess_es688, `/NOPNP` | "ES668" | "ES688" |
| 17 AWE RAM | sbawe32_pnp, sbawe64_gold, sbawe32 with 28672 KB | `[28671K]` | `[28672K]` |
| 17 control | sbawe32_pnp 8192 KB, sbawe64_value 512 KB | `[8192K]`, `[512K]` | the same |
| 18 Crystal MODE 3 | cs4236b with the CS4237B-ID EEPROM, `BLASTER=A220 I5 D0 T4 W530`, then Tyrian 2.1 SETUP with WSS sound effects | I12 = EAh, "ERROR 142: Sound Effects disabled", silent | I12 = 8Ah, WSS accepted, effects play |
| 18 control | the same, extended registers read back in MODE 3 after `/XOFE /V40` and `/XOFI` | X4 = 84h/94h, XE/XF = 26h/09h | the same |
| 19 ES1688 enable reads | ess_es1788 (86Box keeps it disabled until the enable reads), `/NOPNP` | "No card detected" | found, DMAPLAY plays on IRQ 7, DMA 3 |
| 19 control | ess_es688, ess_es1688, sb16, sbprov2, `/NOPNP` | | same output as before |
| 10, 19 Compaq ES1688 | Presario 7100 (`pl4600c`, `pl5600d`), BIOS ESS1688 enabled, `BLASTER=A220 I5 D1 T4` and `A220 I7 D3 T4`, `/NOPNP` first after a cold boot | ES1688 at 220h; B1h/B2h = 14h/18h or 18h/1Ch; DMAPLAY plays on IRQ 5 / DMA 1 or IRQ 7 / DMA 3 | the same |
| 10 Compaq ES1688 | the same, plain `UNISOUND`, then `/NOPNP` twice | "No PnP card detected.", first `/NOPNP` works, second: "No card detected" | both `/NOPNP` work, DMAPLAY plays |
| 19 Compaq ES1688 | `pl4600c` with ESS1688 disabled in Setup, `/NOPNP` | "No card detected" | ES1688 found, DMAPLAY plays on IRQ 7 / DMA 3 |
| 20 ES1888 IRQ pin | Presario 224x, mixer 7Fh = 05h (BIOS), `BLASTER=A220 I7 D3 P330` | `IRQ:7` printed, the chip interrupts on 5 | "IRQ 7 ignored: the BIOS has set this chip to IRQ 5.", `IRQ:5` |
| 20 control | the same with 7Fh cleared first | | `IRQ:7`, interrupts on 7 |
| 21 AZT2320 DMA | azt2320, PnP 74h/75h | D3 H3: 01/03; D1 H5: 01/05; D0 H2: 00/02 | 03/01; 01/03; 00/03 |
| 22 Aztech stereo bit | azt2320, mixer 0Eh, mono DMA playback without touching 0Eh | 0Eh = 22h, L/R correlation 0.80 | 0Eh = 20h, correlation 1.00 |
| 23 Creative FM address | sbawe64_gold, F389, AdLib timer test | 64h = 0389, no AdLib at 388 | "FM address 389 is not a multiple of 4, using 388.", 64h = 0388, AdLib at 388 |
| 24 Yamaha WSS DMA | ymf719, ymf718, `D1 H1 K0`, UNISOUND run twice, then WSSPLAY and DMAPLAY | WSS on DMA 3 | WSS on DMA 0, SB still on DMA 1, `WSS-DMA:0` |

The patched ROMs lived in a separate copy of the ROM set used only for those runs.

Test tools used from bug 11 on, besides PIO, all in `tools/dos/` (build each with `tcc NAME.C`
in the VM; TRACEPNP with TASM and `TLINK /t`):

- **PNPDUMP** sends the initiation key and dumps the activate, I/O, IRQ and DMA registers of the
  given logical devices of one card. **ESSREG** reads the ESS extended registers B1h/B2h (DSP
  command C0h) and the SB Pro/ESS mixer registers without resetting the DSP.
- **TRACEPNP** runs UNISOUND.COM with the trap flag set and logs every `OUT DX,AL` to the PnP
  ports (279h, A79h) and to the ESS configuration ports (660h/661h, 800h/801h). Because DOS can
  return from INT 21h with `RETF 2` and lose the trap flag, it puts an `INT 3` after each `INT`
  instruction to resume tracing. `tools/pnpdecode.py` turns the log into the PnP register writes.
  86Box's ESS cards have no handler for register 28h, so this is how bug 15 was checked.
- **DMAPLAY** and **WSSPLAY** play one DMA block through the SB DSP or the WSS codec, with
  handlers on several IRQs and several DMA channels programmed, and report which ones the card
  used. **OPLDET** runs the AdLib timer test at a list of bases. **SBST** plays SB Pro stereo or
  mono and the captured audio is split into L and R.
- Tyrian 2.1 (freeware) for the Crystal WSS case. It isn't in the repo.
- **ESSINIT** sets up an ES688/ES1688-family chip as ESSCFG does and checks the IRQ with an F2h
  interrupt; `/OLD` writes B1h/B2h the v0.82a way (see below). Build it with
  `tcc -mt -lt -f- ESSINIT.C`.
- The prototypes of `/XESSB` and `/XSLAM`: **ESSBYP** finds and sets up an ES1868/ES1869
  through the ESS bypass key alone, and **CSLAM** configures a CS423x whose PnP key is disabled
  through the Crystal key and SLAM writes.

Not reproducible in 86Box:

- **1 Ensoniq /R**, **2 MPU readback** (ES1868 Maxi Sound, RJA0, TBS20) and **7 SAM9407
  upload**: 86Box doesn't emulate these cards.
- **4 BLASTER past the last field** and **5 argv[argc]**: they read or rewrite bytes of the PSP
  and change no output. A run with `BLASTER=A220 I5 D1 H5 P330 T6` gave the same results with
  both builds.

## ES688/ES1688 B1h/B2h writes and `/LOG`

v0.82a writes the B1h/B2h register number to 2xCh and the value to 2xDh, without waiting for the
write buffer. 86Box's DSP treats 2xDh like 2xCh on everything older than the SB16 (`sb_write`
masks bit 0 of the port), so the old writes work there and the fix can only be shown not to
break anything. PIO confirms it on `ess_es1688`: C0h then B1h to 22Ch reads back D8h, and C0h to
22Ch then B1h to 22Dh reads back D8h too. The fixed build writes both bytes to 2xCh after a busy
poll, with the old value's bits 3-2 replaced and bits 6 and 4 set, as ESSCFG v1.9 does.

`/NOPNP` from a cold boot, then ESSREG, DMAPLAY and ESSINIT. "F2h" is the IRQ that `/LOG`'s
interrupt test saw.

| Machine | BLASTER | B1h/B2h before, after | F2h | DMAPLAY | ESSINIT |
|---|---|---|---|---|---|
| nopnp `ess_es688`, `ess_es1688`, `ess_es1788` | `I7 D3` | 94/88 (80/80 on the ES1788), D8/DC | 7 | IRQ 7, DMA 3 | OK |
| the same | `I5 D1`, `I10 D0`, `I9 D1` | | 5, 10, 9 | IRQ 5 DMA 1, IRQ 10 DMA 0, IRQ 9 DMA 1 | OK |
| Presario 7100 `pl4600c`, ESS1688 enabled | `I7 D3` | 14/18 (set by the BIOS), 58/5C | 7 | IRQ 7, DMA 3 | OK |
| Presario 7100 `pl4600c`, ESS1688 disabled | `I7 D3` | 80/80, D8/DC | 7 | IRQ 7, DMA 3 | `I5 D1`, first after boot: enable reads sent, OK, DMAPLAY IRQ 5 DMA 1 |
| Presario 224x (ES1888, BIOS pin IRQ 5) | `I7 D3` | 94/88, D4/DC; "IRQ 7 ignored" as before | 5 | IRQ 5, DMA 3 | `I7`: interrupt on 5, FAILED (expected: the 7Fh pin wins); `I5 D1`: OK |

`ESSINIT /OLD` (2xDh, 12h/16h + select) also works in 86Box, as expected. The Compaq BIOS itself
writes 14h/18h, without bit 6, so bit 6 is not required for SB-compatible playback, at least in
86Box. The PIC masks read the same with PIO before and after `/NOPNP /LOG` and ESSINIT.

`/LOG` was also run on the PnP path (`sbawe32_pnp`, `ess_es1868`, `ymf719`, `sb16` behind a
3C509B): it prints the read data port and each card's ID, serial and type. Where UNISOUND prints
"No supported PnP card detected." without a newline before the SB16 fallback, the BLASTER line
follows on the same line.

## `/NOPNP` and the OPTi 82C924/925 probe

Before any non-PnP probe, `/NOPNP` writes E5h to F8Fh and reads OPTi 82C924/925 control
registers 1 and 3 (F8Dh, F8Fh). In v0.82a, if both read below FFh, it writes 40h to register 7,
skips the ES688/ES1688, 82C930, 82C928 and SB16 probes and runs the PnP scan (where the OPTi
moves `opti_ctl_port` by 80h). Any device answering at F8Dh/F8Fh passes, for example a card that
decodes 10 address bits and answers at 38Dh/38Fh. The fix also reads F8Dh again right after
register 1, without the password, and requires a different value (an OPTi closes its gate
after one access, as Linux's opti9xx and OSS's mad16 check), and runs the `/NOPNP` probes if the
PnP scan configures nothing after an OPTi match.

86Box decodes all 16 address bits, so on `ess_es1688` F88h-F8Fh read FFh with and without the
password (PIO) and the ES1688 can't produce the false match itself. 86Box has no 82C924/925
either. As a stand-in, an NE2000 (`ne2k`) was set to base F80h, so F8Dh/F8Fh are its counter
registers and read 00h with or without the password. `/NOPNP` from a cold boot, both platforms
(the ES columns give the same output on each):

| Build | ES1688 or ES688 + NE2000 at F80h | sbawe32_pnp + NE2000 at F80h | ES1688, ES688 alone |
|---|---|---|---|
| v0.82a, `ess-init` before the fix | "No PnP card detected." (the user's report) | OPTi match, PnP scan configures the AWE32 | found |
| fix, OPTi probe only (`d2f397b`) | no OPTi match (F8Dh 00 again), ES found | "Card detected: OAK OTI601 / OPTi 82C928": that probe had the same weakness | found, unchanged |
| fix with the 82C928 gate check too (`4197954`) | the same | no 82C928; "No card detected" (nopnp, the AWE32 isn't active) or the SB16 fallback (pnp), as without the NE2000 | found, unchanged |
| fix with the 82C924/925 gate check removed (test build) | OPTi match, "No PnP card detected.", then the ES found by the fallback | OPTi match, PnP scan configures the AWE32 | found, unchanged |

The last row checks the `nopnp_mode` 2 path: the PnP scan still configures a PnP card, and the
fallback finds the ESS chip. A real 82C924/925 or 82C928 wasn't available.

ESSINIT prints the same probe. On `ess_es1688` and on the Presario 7100 (`pl4600c`):

```
OPTi probe (UNISOUND /NOPNP looks for an 82C924/925 at F8Ch first):
  F88h-F8Fh, no password: FF FF FF FF FF FF FF FF
  388h-38Fh:               00 00 FF FF FF FF FF FF
  With E5h to F8Fh: reg 1 (F8Dh) = FF, F8Dh again = FF, reg 3 (F8Fh) = FF
  Not an OPTi 82C924/925: UNISOUND /NOPNP goes on to the ESS detection
```

With the NE2000 at F80h it prints "UNISOUND /NOPNP would take this for an OPTi 82C924/925 and
skip the ESS detection" and "(the fixed build wouldn't: F8Dh reads the same without the
password)". Both then set up the chip (RESULT: OK), also with BLASTER unset (A220 I5 D1).

## `/XSLAM`, `/XESSB`, the BLASTER suggestion and `/LOG`

**`/XSLAM`** was tested on `cs4236b` with the patched EEPROM that sets PnP Key Disable (RAM 4002h
bit 5), on the nopnp platform, with `BLASTER=A220 I5 D1 T4`. Plain `UNISOUND` prints "No PnP
card detected." `UNISOUND /XSLAM` prints `PnP card found: [SLAM] CS423x` and
`ADD:220 WSS:534 OPL:388 IRQ:5 DMA:1/1 MPU:330/I9 CTR:120 JOY:200`; WSSPLAY then plays through the
codec at 534h (I12 8Ah) and DMAPLAY through the DSP at 220h, both on IRQ 5 DMA 1, and the control
port answers at 120h. `/XSLAM /D` deactivates every device (220h reads FFh again) and `/XSLAM /R`
is refused. On `sb16` with no Crystal chip it prints "No Crystal codec answered after the SLAM
setup." with ERRORLEVEL 1. On the Gateway E-1000 (`tomahawk`, on-board CS4236B with only the
serial identifier as resource data) plain `UNISOUND` now configures the chip with the fixed layout
and DMAPLAY plays at 220h, but 86Box decodes logical device 0's codec base with 10 bits when there
are no I/O descriptors (`/R` reads `WSS:134`), so the codec at 534h doesn't answer there.

**`/XESSB`** was tested on `ess_es1868` and `ess_es1869` on both platforms: with no BLASTER
(control port 800h), with `A240 I7 D3 R660`, and again with `A220 I5 D1 R800`. Each time the chip
answered E7h with 68h and mixer 40h with `18 68`/`18 69` and the control port, was configured, and
DMAPLAY played on the BLASTER IRQ and DMA; the control port still answered afterwards. `/R` reads
`MPU:330/I15`, which is 86Box's answer for card register 28h through the control port. On `sb16`
it prints "No ES1868/ES1869 answered the ESS bypass key at 800." with ERRORLEVEL 1. A first
version also wrote and activated logical device 0 (the control port); in 86Box that moved the
port away, so the `/LOG` readback read FFh, and `bdc7684` leaves it as the key set it.

**The BLASTER suggestion** was checked on `sb16pnp+3c509b`, `sb16`, `ess_es1688` and
`awe32pnp+sb16-240+3c509b` on both platforms, with no BLASTER, a matching one, `A220 I7 D3 P300
E620` (the PnP SB16 left at I5 D1 by `/NOPNP`: warning, P and E kept), `A240` and `A260` (no DSP
there: warning; with the second SB16 at 240h: nothing), and H5 on the ES1688 (warning, H dropped).

**`/LOG`**: samples for `sb16_pnp`, `sbawe32_pnp`, `cs4236b`, `ymf719`, `guspnp`, `ess_es1688`,
`ess_es1869` and `adlib` on both platforms, each with `/LOG`, `/NOPNP /LOG` and `/R /LOG`, and for
`/XSLAM` and `/XESSB`. The first build with the longer `/LOG` printed nothing at all: the tiny-model
startup adds `_stklen` (the library's 4K) to the end of the BSS and stops when that passes 64K.
`_stklen` is now 900h, which leaves 280h of room; the stack in use is about 1K.

## Regression check

The fixed build ran the full baseline matrix again. 171 of the 175 runs gave identical output.
The other four are bug 10's fix: `/NOPNP` now finds the non-PnP ES688 and ES1688 (on both
platforms), where the original found nothing because earlier commands had left bytes in the DSP
read buffer.

The `thread-fixes` build ran the matrix twice more, after bug 17 and after bug 24, compared with
the `main` build. 155 of the 175 runs were identical after bug 17. The other 20 differ only as the
fixes intend: the unknown cards cs4235, ess_ess0100_pnp and ess_ess0102_pnp are skipped (11),
`/R` on the non-PnP sb16, sbawe32 and sb_vibra16s only reads (13), the ES1868/ES1869 show
`DMA:1/4` and `MPU:330/In` (14, 15), and the ES688 name (16). After bug 24 the further changes
were `/NOPNP` now finding the ess_es1788 and the ES1688 on gusextreme and gusvipermax (19; the ES1788 is also named, 20)
and the `WSS-DMA` field on ymf718 and ymf719 (24). azt2320 on pnp doesn't boot (see above).

The `ess-init` build (`05aa0de`, with `/LOG`) ran the full matrix against the `main` build: all 175
runs gave identical output (azt2320 on pnp doesn't boot with either).

With the OPTi probe fixes (`4197954`), all 175 runs gave the same output as `6eb15a0`
(azt2320 on pnp doesn't boot with either).

The build with `/XSLAM`, `/XESSB`, the no-card lines and the longer `/LOG` (`29c56b6`) ran the
matrix against `f2b97f7`: the only differences were the two new lines after "No PnP card
detected." or "No card detected" (495 outputs on the runs where no card is found); everything
else was byte-identical (azt2320 on pnp doesn't boot with either).
