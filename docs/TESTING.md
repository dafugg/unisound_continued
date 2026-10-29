# Testing in 86Box

All results here come from emulation in 86Box. None of the bugs or fixes below have been tested
on real hardware.

## Environment

| Component | Version |
|---|---|
| 86Box | built from source (github.com/86Box/86Box), with the automation socket |
| ROM set | github.com/86Box/roms |
| 86box-mcp | github.com/dafugg/86box-mcp |
| Original build | UNISOUND.COM v0.82a, sha256 `606f8fae…b370` |
| Fixed build | branch `fixes` at `44bf539` (bugs 1-10) |

Platforms, both a 486DX2-66 with 16 MB of RAM, booting MS-DOS 6.22:

- **pnp**: ASUS PVI-486SP3C (`486sp3c`), Award 4.51PG BIOS with the Plug and Play extension. The
  BIOS configures ISA PnP cards at boot.
- **nopnp**: Olystar LIL1429 (`ali1429`), AMIBIOS 04/04/93 with no PnP support, plus an ISA IDE
  controller. PnP cards start unconfigured.

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
| Did not boot | azt2320 on pnp: the Award BIOS hangs at "Initialize Plug and Play Cards" |

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
- **ES1869**: init prints `CTR:660`, which is the alternate PnP configuration port it switches to,
  not a value written to the card. On nopnp the gameport reads back as 0 after init although
  init reported 201. Cause not determined.

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

The patched ROMs lived in a separate copy of the ROM set used only for those runs.

Not reproducible in 86Box:

- **1 Ensoniq /R**, **2 MPU readback** (ES1868 Maxi Sound, RJA0, TBS20) and **7 SAM9407
  upload**: 86Box doesn't emulate these cards.
- **4 BLASTER past the last field** and **5 argv[argc]**: they read or rewrite bytes of the PSP
  and change no output. A run with `BLASTER=A220 I5 D1 H5 P330 T6` gave the same results with
  both builds.

## Regression check

The fixed build ran the full baseline matrix again. 171 of the 175 runs gave identical output.
The other four are bug 10's fix: `/NOPNP` now finds the non-PnP ES688 and ES1688 (on both
platforms), where the original found nothing because earlier commands had left bytes in the DSP
read buffer.
