# Where 86Box differs from real hardware

UNISOUND is tested in 86Box first (see [TESTING.md](TESTING.md)), then on real cards. This page
collects every case where 86Box behaved differently from a real card, so that they can be
reported to the [86Box project](https://github.com/86Box/86Box). Before filing one, check it
again against current 86Box: these were seen with the builds listed in TESTING.md's
[Environment](TESTING.md#environment), and 86Box changes quickly.

The real-hardware tests are vintagefugg's, on an Intel SE440BX-2 with a Pentium III 450 under
MS-DOS 6.22 unless noted; the details are in TESTING.md's
[Real hardware with fix-4](TESTING.md#real-hardware-with-fix-4) table.

## Seen on real hardware

| Chip | 86Box (card) | Real hardware | Effect on UNISOUND | Upstream |
|---|---|---|---|---|
| OPTi 82C930 | Control register 12h reads FFh (`opti_82c930`); only MC1-MC12 are implemented, as in the data sheet | Silicom Wavemaster 32 FGS (MegaImage32): 12h reads 00h | `/NOPNP` doesn't find the 86Box chip, whose detection needs 12h below 8; tests use a build without that check | not filed |
| OPTi 82C930 | After `/XWSS` (the WSS configuration register written), the SB part still plays on its own IRQ and DMA (`opti_82c930`, DMAPLAY IRQ 5, DMA 1) | Wavemaster 32 FGS: after any WSS setup (`/XWSS`, or a game's own) the SB part plays on the WSS DMA with no IRQ until `/NOPNP` runs again; SB register 3 read 01h after Tyrian 2000's WSS setup | 86Box can't show the one-mode-at-a-time behaviour that fix d8ccc45 is for | not filed |
| OPTi 82C931 | Writing 388h to the FM base register (LD1 62h), which the resource data gives as 16-byte aligned, moves AdLib to 390h (`opti_82c931`) | BTC 1817DSL: with the same write, the FM chip answers at 388h only (OPLDET; Tyrian 2000's FM music works) | Harmless on the real card; in 86Box FM is at 390h | not filed |
| Creative DSP 3.02 | After boot the DSP's read buffer is empty, so E1h without a reset reads the version (`sbprov2`) | Sound Blaster Pro 2 (Am5x86-P75): after a cold boot the reset reply AAh is still in the read buffer, so fix-3's check read `E1 AA` | Hid fix-3's missing DSP reset; fixed in 19a0fe1 | not filed |

Not yet compared with 86Box: the BTC 1817DSL (82C931) also loses its SB IRQ after Mpxplay's WSS
driver until `UNISOUND` runs again, as the 82C930 does.

## 86Box departs from the data sheet; no real card checked yet

| Chip | 86Box | Expected | Upstream |
|---|---|---|---|
| ESS ES1869 | After the bypass key, logical device 0's address registers keep their old values, so activating it moves the ports to that stale address and the gameport and IDE writes that follow are lost | The key sets logical device 0's address and activates it (ES1869 data sheet, Bypass Key, page 28) | [86Box#8554](https://github.com/86Box/86Box/pull/8554) (fix) |
| ESS ES1868/ES1869 | No handler for the card-level vendor registers: 28h ignores writes and reads FFh, and the MPU-401 always gets the SB IRQ | 28h holds the MPU-401 IRQ | not filed |
| Yamaha OPL3-SAx | Control registers 03h and 06h take effect only when the PnP configuration changes, so after the first UNISOUND the WSS DMA still follows the power-on value 61h | They take effect when written | not filed |
| EMU8000 (AWE32/AWE64) | Byte writes to the pointer register become word writes; odd-address byte reads are shifted right by 1 instead of 8 | Byte I/O as on the ISA bus | not filed |
| ESS ES1688 (GUS Extreme, ViperMax) | Disabled until the ES1688 enable reads | Not known; UNISOUND does the enable reads since fix 19 | not filed |

## Not 86Box bugs, but worth knowing

- **AZT2320 on the ASUS PVI-486SP3C:** the board's Award BIOS hangs on the card's "IDE CDROM
  DISABLED" device, whose I/O descriptors have length 0
  ([86Box#8552](https://github.com/86Box/86Box/issues/8552)). The BIOS loops; seven other
  PnP BIOSes in 86Box boot with the card.
- **OPTi 82C931 IDE:** 86Box's 82C931 PnP ROM (`adsrom.bin`) lists the IDE control port before
  the command block, and 86Box follows the resource data. The real BTC 1817DSL's ROM has no IDE
  device, so its order couldn't be checked; UNISOUND now follows the resource data (9ac8e98).
- **Cards 86Box doesn't emulate**, so only real cards can test them: Echo PSS cards (TNC Sound
  16 DSP XO), OPTi 82C928 and OAK OTI601, the ALS1x0 (RTL3000), the Ensoniq Soundscape OPUS, and
  any sound card's Mitsumi, Sony or Panasonic CD-ROM interface (86Box has those only as separate
  controllers).
