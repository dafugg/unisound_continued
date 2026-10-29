# Test ROMs

The 86Box ROM and EEPROM files the tests in [TESTING.md](TESTING.md) changed or rebuilt. The files
themselves aren't in the repository, since most are modified copies of real cards' ROMs; this lists
how to make each from the original and the SHA-256 it should end up with. Each goes at the path
86Box uses for it, in a copy of the ROM set (`sound/...`) or of a VM's folder (`nvr/...`). Never
change the original: the tests used a separate copy, linked with `cp -al`, with only that file
replaced.

Unless noted, each one is a copy of the file from [86Box/roms](https://github.com/86Box/roms) at
`6122ea4` (CI's pin) with the bytes listed changed. For a changed card ID, the serial identifier's
checksum (byte 8, ISA PnP LFSR from 6Ah) is recomputed too, and the end tag's checksum when it
isn't 0.

| Name | File | Based on (sha256 of the original) | Change | Used for |
|---|---|---|---|---|
| `sb16-name150` | `sound/creative/CT2941 PnP.BIN` | 86Box `CT2941 PnP.BIN` (`6abaf06d…e2fa6f`) | the card's ANSI name lengthened to 150 characters, the resource data after it moved along | bug 6, ANSI string length |
| `iwrom-bank16` | `sound/gravis/IWROM.BIN` | 86Box `IWROM.BIN` (`783f2fec…110050`) | the first ROM bank name made 16 characters long (`AMDGM_1_1_FULL16`, bytes 14h-19h), and byte 1FFh | bug 9, ROM bank name |
| `azt2320-ide-len1` | `sound/azt2320/azt2320.bin` | 86Box `azt2320.bin` (`cdb4fb83…0821a1`) | the lengths of the two I/O descriptors of "IDE CDROM DISABLED" (bytes 4Bh and 53h) set from 0 to 1 | the 486SP3C BIOS hang ([86Box#8552](https://github.com/86Box/86Box/issues/8552)): the board boots with this |
| `cs4236b-pkd` | `nvr/cs4236b.nvr` | the EEPROM image 86Box writes for a new `cs4236b` (`7217cd21…eacc9a3`) | byte 6 (RAM 4002h) 80h to A0h: bit 5, PnP Key Disable | `/XSLAM` |
| `cs4236b-cs4237b` | `nvr/cs4236b.nvr` | the same | bytes 26 and 44: `6` to `7`, so the IDs read CSCA737 and "CRD4237B-3" | bug 18, Crystal MODE 3 (the CS4237B path) |
| `cs4236b-pkd-cs4237b` | `nvr/cs4236b.nvr` | the same | both of the above | `/XSLAM` with the CS4237B ID |
| `azt1021` | `sound/azt2320/azt2320.bin` | 86Box `azt2320.bin` | card ID AZT1021 | AZT1021 |
| `azt1022-azt2320` | `sound/azt2320/azt2320.bin` | 86Box `azt2320.bin` | card ID AZT1022 | the AZT1022 split, AZT2320 card |
| `azt1022-ad1816` | `sound/ad1816/ad1816.bin` | 86Box `ad1816.bin` (`756710e2…5d2a9b`) | card ID AZT1022, first logical device AZT1018 | the AZT1022 split, AD1816A card |
| `dxp3201` | `sound/gravis/OLDGRAV.ROM` | 86Box `OLDGRAV.ROM` (`286a852b…18ac1a6`) | card ID DXP3201, first logical device DXP0010 | DXP3201 |
| `cdc1111` | `sound/gravis/OLDGRAV.ROM` | 86Box `OLDGRAV.ROM` | card ID CDC1111, first logical device CDC1112 | CDC1111 |
| `es1878` | `sound/ess/ESS1869.BIN` | not 86Box's: `datasheet/es1878-internal.bin` | both checksums computed, zeros added to 397 bytes (86Box reads that many for `ESS1869.BIN`) | ESS1878, on `ess_es1869` |
| `es1879` | `sound/ess/ESS1869.BIN` | not 86Box's: `datasheet/es1879-internal.bin` | the same | ESS1879, on `ess_es1869` |

The `datasheet/` files are the PnP ROM listings in Appendix A of ESS's data sheets
([datasheets/ess](datasheets/README.md)), assembled as printed: every byte after
the 8-byte ESS header, with the data sheets' 00h checksum placeholders.

| File | Listing | Bytes |
|---|---|---|
| `datasheet/es1869-example.bin` | "ES1869 PnP ROM data example" (DS1869) | 397; differs from 86Box's `ESS1869.BIN` (`ee5f3cb5…b95c5d7c`) in four bytes, see "ESS PnP ROMs from the data sheets" in TESTING.md |
| `datasheet/es1878-internal.bin` | "ES1878 internal PnP resource ROM" v001 (DS1878A) | 252 |
| `datasheet/es1879-internal.bin` | "ES1879 internal PnP resource ROM" (DS1879A) | 280 |

SHA-256 of each result:

```
853584e4d2ce699dcc1fdfb26bc701766f4a5299a5e0d87a836f4aa2b518136f  azt1021/sound/azt2320/azt2320.bin
b958d12a0623503917875d29dacf13d62409fbcf3d6fb58f276b1faa605773b5  azt1022-ad1816/sound/ad1816/ad1816.bin
a3a9101f2754f60db1ee46d98874c5f9034693b005ef9ba14c33940b685ce187  azt1022-azt2320/sound/azt2320/azt2320.bin
d899954570cc5c4b4943c43bd4c6d860037f6cf8ad5ed5c3ffa3615374ecaadb  azt2320-ide-len1/sound/azt2320/azt2320.bin
b7f9ad221295488412c33ea3d77f92c1c52c05a337c64cd840387b7b68cb0078  cdc1111/sound/gravis/OLDGRAV.ROM
fb118b9e3b48a96e395c18172623b1b259e68ec3d6a4e2a51d7b556802000c47  cs4236b-cs4237b/nvr/cs4236b.nvr
1e31cc25128d34171763fc88c619e202176e1e9d6caada080dcd68b411eb7e35  cs4236b-pkd-cs4237b/nvr/cs4236b.nvr
0f6f1cc16f3c49e464e590a846bcabfbc19a5575fc8666b8abc175ab0dc1d8dc  cs4236b-pkd/nvr/cs4236b.nvr
669965c29f4fb1cf4a8db8864ce8373a61837cf0a88426f441cb252c14cd4e9f  datasheet/es1869-example.bin
02bbfe6a736ad4a635dd0d4ffb4e397245d57a1852f47a9bbba85d4b22bf2d1b  datasheet/es1878-internal.bin
d53b5367b24466d3e5e9663a6ead27d236de30b3f7059c0fa54a0a9b0e825fb1  datasheet/es1879-internal.bin
71b9dbf7f4b1562657560759549e942a2885a610beb07393eb21a3d46c4da6cd  dxp3201/sound/gravis/OLDGRAV.ROM
0c5630ae1343d99d9bd81922e1cf61f29a6626c3c05506703f6681f801ae20e1  es1878/sound/ess/ESS1869.BIN
621bc7d72714bed15ce63e3ac526e0d8982bc1791887e75905f842d975266822  es1879/sound/ess/ESS1869.BIN
c0e62d0669f61e8c7bb09b7cdef8baba887426ab7114d1812e1b11dd99d0a1e0  iwrom-bank16/sound/gravis/IWROM.BIN
7e9def6fb1800e6739cacf1ac481d7c38c724b8cf576c9b0589e16772080ab96  sb16-name150/sound/creative/CT2941 PnP.BIN
```
