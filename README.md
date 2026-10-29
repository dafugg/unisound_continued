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

Fixes with a Reports entry follow up problems reported in the VOGONS thread; Reports links to the
posts that describe the problem, with a user's later posts about it numbered. The rest were found
by reading the decompiled source or turned up while testing a reported problem, and the ESS DSP
flush (44bf539) was found in 86Box before it was reported. The table starts with the reported
fixes confirmed on real hardware, then the others roughly by importance, and ends with bugs that
aren't fixed yet. The 86Box tests used 86Box on an ASUS PVI-486SP3C (`486sp3c`, PnP BIOS) and an Olystar LIL1429
(`ali1429`, no PnP BIOS) under MS-DOS 6.22, plus a Compaq Presario 224x for the ES1888 and the Compaq Presario 7100 486 and 586 for the
on-board ES1688; details
are in [docs/TESTING.md](docs/TESTING.md). A ✅ means the bug was reproduced in 86Box and the fix
shown to work. Every fix was also part of a 175-run regression check against all of 86Box's
sound cards, with no regressions; the changes in output it found are the intended ones.

Apart from the fixes, the `main` build prints its own version, "v0.82a-unofficial-fix-3"
([3245b96](https://github.com/dafugg/unisound_decomp/commit/3245b964ecfa7e1c0c371e1880ed84f8fb6e60cb)),
so its output can't be mistaken for JazeFox's release, and has a `/LOG` option
([fde5b60](https://github.com/dafugg/unisound_decomp/commit/fde5b603ddab9a934871e3470df7ac7f7ba66578))
that prints diagnostic lines (see [Reporting problems](#reporting-problems)).

| Commit | Description | Reports | Tested on 86Box | Tested on real hardware |
|---|---|---|---|---|
| [44bf539](https://github.com/dafugg/unisound_decomp/commit/44bf539d3d8c3c3ae78c3ae4d47f0d8035d5a694) | The ES688/ES1688 probe flushes stale bytes from the DSP read buffer, so `/NOPNP` finds the card after earlier commands. Found in 86Box | Vogons: [GigAHerZ](https://www.vogons.org/viewtopic.php?p=1451853#p1451853), [analog_programmer](https://www.vogons.org/viewtopic.php?p=1450935#p1450935) | ✅ `ess_es1688`, `/NOPNP` twice: the second run found nothing before, finds the card after. The regression run showed the same fix on `ess_es688`. Compaq Presario 7100 (`pl4600c`, `pl5600d`) on-board ES1688: a second `/NOPNP` found nothing before, finds and plays after | ✅ Compaq LTE 5000 (on-board ES1688) |
| [b2bea62](https://github.com/dafugg/unisound_decomp/commit/b2bea620ae5b4cde994b1ded36d4f561ce469bf2) | `/NOPNP` sends the ES1688 enable reads and resets the DSP before the ESS probe, so chips that start disabled (ES1788, ES1888) are found | Vogons: [analog_programmer](https://www.vogons.org/viewtopic.php?p=1450935#p1450935), [0xDEADBEEF](https://www.vogons.org/viewtopic.php?p=1338132#p1338132) | ✅ `ess_es1788`: "No card detected" before, found and playing after. It also finds the ES1688 on `gusextreme` and `gusvipermax` now. Compaq Presario 7100 (`pl4600c`) with the on-board ES1688 disabled in Setup: "No card detected" before, found and playing after | ✅ Compaq LTE 5000 (on-board ES1688) |
| [f049bb7](https://github.com/dafugg/unisound_decomp/commit/f049bb72777d9a1d718a522d2ac5a3a541082ca3) | An unknown PnP card (a network card, a PnP game port, an unsupported sound card) no longer stops the scan with "Aborting": it is skipped, the first supported card is configured, and if there is none the non-PnP SB16 fallback runs. `/Cn` with no card n now exits with ERRORLEVEL 1 | Vogons: [ajacocks](https://www.vogons.org/viewtopic.php?p=1204736#p1204736), [Retromachines](https://www.vogons.org/viewtopic.php?p=1286710#p1286710), [NightSprinter](https://www.vogons.org/viewtopic.php?p=1315676#p1315676), [AncapDude](https://www.vogons.org/viewtopic.php?p=1378838#p1378838) | ✅ `sb16` behind a 3C509B: "Unknown PnP card [TCM5094] found. Aborting..." before, the SB16 configured after; `ess_ess0100_pnp` + `sbawe32_pnp`: the AWE32 configured after; `/C3` past the last card: ERRORLEVEL 0 before, 1 after | ✅ |
| [33f2334](https://github.com/dafugg/unisound_decomp/commit/33f23346bf074d540b99470ecf758c8d77db25dd) | Options must match a whole option name; anything else prints "Unknown option" and exits with ERRORLEVEL 2. Before, `/DUMMY` disabled the card, `/NOTHING` ran `/NOPNP`, `/C10` meant card 1, and typos such as `/XAE` were ignored with ERRORLEVEL 0 | Vogons: [yeokm1](https://www.vogons.org/viewtopic.php?p=849250#p849250), [AndalusianRG](https://www.vogons.org/viewtopic.php?p=1004655#p1004655), [Cryptus](https://www.vogons.org/viewtopic.php?p=1245058#p1245058), [analog_programmer](https://www.vogons.org/viewtopic.php?p=1450935#p1450935) | ✅ `sb16_pnp`: 17 bad options rejected with ERRORLEVEL 2; every documented option gives the same output as before on seven cards | ✅ Compaq LTE 5000 |
| [33d7aa8](https://github.com/dafugg/unisound_decomp/commit/33d7aa826fa064c160940e5131c5c800caf06b36) | `/NOPNP`: the OPTi 82C924/925 probe (E5h to F8Fh, registers 1 and 3 below FFh) took any device answering at F8Dh/F8Fh for an OPTi, for example a card decoding 10 address bits at 38Dh/38Fh, and then skipped the ES688/ES1688 and other non-PnP probes for the PnP scan: "No PnP card detected." with a non-PnP ES1688. The probe now also requires F8Dh to read differently without the password right after (the OPTi closes its gate after one access, as Linux's opti9xx and OSS's mad16 check), and if the PnP scan configures nothing after an OPTi match the `/NOPNP` probes still run. The 82C928 probe ([6e1d892](https://github.com/dafugg/unisound_decomp/commit/6e1d892f2dc994e4741ce5fb4001b5ce24fd0a99)) had the same weakness and gets the same check | Vogons: [analog_programmer](https://www.vogons.org/viewtopic.php?p=1453069#p1453069) (`/NOPNP`, no BLASTER) | ✅ 86Box decodes 16 address bits, so an NE2000 at F80h stood in for the aliased device: with `ess_es1688`/`ess_es688`, "No PnP card detected." before, the chip found after. A test build without the new check showed the fallback finding it. With `sbawe32_pnp`, the NE2000 was taken for an 82C928 before the second commit. 86Box has no 82C924/925 | ❌ |
| [4c4cba6](https://github.com/dafugg/unisound_decomp/commit/4c4cba6e7e6be3254f2de9631ec56bf1c4fa9177) | ES688/ES1688 (`/NOPNP`): extended registers B1h (IRQ) and B2h (DMA) are written the way ESS's ESSCFG writes them: the register number and the value both go to 2xCh, each after waiting for 2xCh bit 7, and the value is the old one (read with C0h) with bits 3-2 set to the IRQ or DMA and bits 6 and 4 set, as the ESS data sheets ask. Before, the value went to 2xDh with no wait and without bit 6; ESS documents 2xCh for both bytes (ES1868 data sheet; the Linux es1688 and NetBSD ess drivers do the same), and a chip that doesn't treat 2xDh like 2xCh would get B2h's value in B1h, i.e. IRQ 2/9 | Vogons: [vico](https://www.vogons.org/viewtopic.php?p=1297347#p1297347), [p1348419](https://www.vogons.org/viewtopic.php?p=1348419#p1348419), [analog_programmer](https://www.vogons.org/viewtopic.php?p=1450935#p1450935) (possibly) | ❌ Only shown not to break anything: 86Box treats 2xDh like 2xCh (checked with PIO), so the old write works there too. With the fix, `ess_es688`, `ess_es1688`, `ess_es1788` and the Compaq Presario 7100 and 224x on-board chips read B1h/B2h back as set, DMAPLAY plays on the BLASTER IRQ and DMA, and an F2h interrupt arrives on it (`/LOG`, ESSINIT) | ✅ [analog_programmer](https://www.vogons.org/viewtopic.php?p=1453280#p1453280)'s ES1688, with ESSINIT: written this way, B1h/B2h read back 55h/5Ah as written; written the 0.82a way (`ESSINIT /OLD`), they stayed at 15h/1Ah |
| [3566977](https://github.com/dafugg/unisound_decomp/commit/35669776b28dc46c7b77e8b5ff60059224666667) | After the card is set up, BLASTER's A, I, D and H are compared with what the card got. If BLASTER is missing, or its A is the card's port but I, D or H differ, UNISOUND suggests a `SET BLASTER` line for AUTOEXEC.BAT that keeps the other fields (P, E, F, T...), which may describe other cards. A BLASTER whose A is another port is left alone when a DSP answers there, and gets a warning when nothing does. UNISOUND can't set BLASTER itself, and without it programs don't find the card | Vogons: [analog_programmer](https://www.vogons.org/viewtopic.php?p=1453069#p1453069) (no BLASTER) | ✅ PnP and non-PnP SB16, ES1688, AWE32 PnP with an SB16 at 240h: a suggestion without BLASTER, nothing when it matches, a warning for I7 D3 on a card left at I5 D1 and for an H on an ES1688; BLASTER=A240 pointing at the other SB16: nothing | ❌ |
| [6b5b127](https://github.com/dafugg/unisound_decomp/commit/6b5b127a12ef4202f79a035ccf750180b1882a26) | New option `/XSLAM`: a Crystal CS423x whose PnP key is disabled (set up by a laptop BIOS or CWDINIT) takes no part in isolation, and UNISOUND found nothing. `/XSLAM` sends the Crystal key and runs the normal Crystal setup with each PnP register write turned into the SLAM command that does the same (`15h` device, `47h`/`48h`/`42h` I/O, `22h`/`27h` IRQ, `2Ah`/`25h` DMA, `33h` activate, then `79h`), with the fixed CS423x layout. If no codec answers afterwards it says so and exits with ERRORLEVEL 1; `/R` can't be used with it. A Crystal card whose resource data holds only the serial identifier (the Gateway E-1000's on-board CS4236B) now gets the same fixed layout instead of "No supported PnP card detected." Prototype: `tools/dos/CSLAM.C` | Vogons: [Pickle](https://www.vogons.org/viewtopic.php?p=916242#p916242), [MartinausWien](https://www.vogons.org/viewtopic.php?p=1386273#p1386273), [AliceImagina](https://www.vogons.org/viewtopic.php?p=1396090#p1396090) | ✅ `cs4236b` with the PnP Key Disable EEPROM bit: "No PnP card detected." without it; with `/XSLAM` configured, WSSPLAY and DMAPLAY play on IRQ 5 DMA 1, `/D` turns it off. No Crystal chip: "No Crystal codec answered", ERRORLEVEL 1. Gateway E-1000: configured, SB plays; 86Box decodes the WSS codec there at 134h instead of 534h (the chip has no I/O descriptors), so the codec part can't be shown | ❌ |
| [a5af556](https://github.com/dafugg/unisound_decomp/commit/a5af5562c04316fd07fdcd4da2254ba864d6677f) | New option `/XESSB`: for an ES1868/ES1869 that isolation doesn't find (a chipset that doesn't pass A79h on, an on-board chip), UNISOUND sends the ESS bypass key with the control port (BLASTER `R`, 800h by default) to 279h and reaches the PnP registers through the control port, with no CSN. It accepts the chip only if the DSP answers E7h with 68h and mixer 40h reads 18h, 68h/69h, then the control port, and then runs the normal ES1868/ES1869 setup. Otherwise it says so and exits with ERRORLEVEL 1. The control port's own logical device is left as the key set it ([73ee3f4](https://github.com/dafugg/unisound_decomp/commit/73ee3f4b3f258ac3e1c6785ba1a4804db8322edb)). Prototype: `tools/dos/ESSBYP.C` | Vogons: [vsharun](https://www.vogons.org/viewtopic.php?p=1357653#p1357653), [Oetker](https://www.vogons.org/viewtopic.php?p=869608#p869608) | ✅ `ess_es1868`, `ess_es1869` with and without a PnP BIOS, `R800` and `A240 R660`: found, configured, DMAPLAY plays on the BLASTER IRQ and DMA. No ESS chip: ERRORLEVEL 1. 86Box always passes A79h on, so the case it is for can't be reproduced | ❌ |
| [68c10df](https://github.com/dafugg/unisound_decomp/commit/68c10dff676eab4d62e207feea839f5bcd1c36a6) | When no card is found, two lines point to `/XSLAM`, `/XESSB` and `UNISOUND /LOG > LOG.TXT` | — | ✅ the only change in the regression check | ❌ |
| [ac324d4](https://github.com/dafugg/unisound_decomp/commit/ac324d477e2e093506c6426b64eb1c1019acb5db) | `/LOG` also prints the DOS version and command line, every BLASTER field as parsed and ULTRASND, an isolation that answered with a bad checksum, each card's whole serial identifier, the selected card's resource data in hex and its logical device map, each configured logical device's registers read back after the setup, the WSS codec's I0-I31, the SB16 reply at each non-PnP base and the 82C930 probe's reads. The tiny-model startup needs the end of the BSS plus `_stklen` below 64K, so the stack is set to 900h | — | ✅ samples on 16 card and platform pairs and `/XSLAM`/`/XESSB`; nothing changes without `/LOG` | ❌ |
| [a45a2be](https://github.com/dafugg/unisound_decomp/commit/a45a2be9d76e208577d71a52fce300bad20bc7ad) | Non-PnP SB16/AWE32 (`/CS`, `/NOPNP` and the fallback): `/R` only reads the settings back, `/D` leaves the card alone. Before, both reprogrammed the card | — | ✅ `sb16`, `sbawe32` with `BLASTER=A220 I7 D0 H6`: `/R` changed mixer 80h/81h from 02/22 to 04/41 before, leaves them after | ❌ |
| [809361d](https://github.com/dafugg/unisound_decomp/commit/809361da10ed4e0f36cd52c0755ed70d689b4a19) | Crystal CS4237B-class cards (Orpheus, Turtle Beach, other CS4237B/CS4238B): the codec is put back in MODE 1 after the extended-register setup. WSS drivers that check I12, such as Tyrian's ("ERROR 142"), rejected it | Vogons: [vanfanel](https://www.vogons.org/viewtopic.php?p=1350792#p1350792) | ✅ `cs4236b` with its EEPROM changed to the CS4237B ID: I12 EAh before, 8Ah after; Tyrian WSS effects silent before, play after | ❌ |
| [758bfd8](https://github.com/dafugg/unisound_decomp/commit/758bfd8abd3bd90d2b0afcd35efa9604734c10a9) | ES1868/ES1869: BLASTER `U` sets the MPU-401 IRQ, in card-level PnP register 28h (0 for U0, P0 and `/D`), shown as `MPU:330/I9`. Before, U had no effect | Vogons: [crvs](https://www.vogons.org/viewtopic.php?p=908134#p908134) | ✅ register writes only: a trace of UNISOUND's PnP port writes shows 28h = 09 for U9, 00 for U0, P0 and `/D`. 86Box doesn't implement 28h, so the effect needs real hardware | ❌ |
| [c886b37](https://github.com/dafugg/unisound_decomp/commit/c886b37c28d8b4f7d123ab4d0cce458c1c596eda) | ES1868/ES1869: `H4` writes "no second DMA" to PnP 75h, and D = H also means no second DMA instead of DMA 0. Without H the second DMA is now none (`DMA:1/4`, was `1/0`) | Vogons: [Kahenraz](https://www.vogons.org/viewtopic.php?p=1444603#p1444603) | ✅ `ess_es1868`, `ess_es1869`: H4 after H3 left 75h at 3 before, 4 after | ❌ |
| [ac90c2d](https://github.com/dafugg/unisound_decomp/commit/ac90c2dbe4fc71a508b47721c42522d3204dda6b) | `/NOPNP` on an ES1887/ES1888: an IRQ pin the BIOS selected in mixer 7Fh is kept and printed, instead of printing the BLASTER IRQ the chip ignores. The IRQ and DMA shown come from B1h/B2h read back; ES1788 and ES1887/ES1888 are named | Vogons: [0xDEADBEEF](https://www.vogons.org/viewtopic.php?p=1338132#p1338132) | ✅ Compaq Presario 224x (on-board ES1888, BIOS IRQ 5) with `I7`: `IRQ:7` printed before, `IRQ:5` and a note after | ❌ |
| [58d3785](https://github.com/dafugg/unisound_decomp/commit/58d3785f0284ab81bb79e7458e25951635ba3946) | `/NOPNP` names: an ES688 was reported as "ES668" and an OPTi 82C930 as "OPTi 83C930" | Vogons: [Pr1ha](https://www.vogons.org/viewtopic.php?p=1393184#p1393184) | ✅ `ess_es688`: "ES668" before, "ES688" after. 86Box's 82C930 isn't found by `/NOPNP` (see the emulation notes), so that string was only checked in the binary | ❌ |
| [52b639c](https://github.com/dafugg/unisound_decomp/commit/52b639ce92d22ad01aba0db9d9719b38e89c24a0) | Creative: a BLASTER `F` that isn't a multiple of 4 is rounded down, with a message; the FM/CQM decodes four ports | Vogons: [Mr.Hunt](https://www.vogons.org/viewtopic.php?p=1393624#p1393624) | ✅ `sbawe64_gold` with F389: PnP 64h = 0389 and no AdLib at 388 before, 0388 after | ❌ |
| [65240e2](https://github.com/dafugg/unisound_decomp/commit/65240e2a3212407ff91b1a6b2922b47ec9c5f917) | Yamaha OPL3-SAx: BLASTER `K` sets the WSS DMA (the second DMA, PnP 75h) when it differs from D, and the WSS DMA is shown | Vogons: [TgamesFR](https://www.vogons.org/viewtopic.php?p=1371775#p1371775) ([2](https://www.vogons.org/viewtopic.php?p=1385213#p1385213)) | ✅ `ymf719` with D1 H1 K0: WSS on DMA 3 before, DMA 0 after (UNISOUND run twice; see TESTING.md) | ❌ |
| [fe79a24](https://github.com/dafugg/unisound_decomp/commit/fe79a24a4586430f166e8cf428334de173c7dfce) | Yamaha OPL3-SAx (YMF71x): the SB Pro master is set to level 1 (22h). In the YMF715's SB Pro mixer table the effective voice level depends on master (22h) and voice (04h) together, and any sum of 8 or more is 0 dB, so at UNISOUND's master DDh SB Pro stereo panning was lost (Wolf3D) and `/VW` had almost no effect on SB-mode audio. The master volume (`/V`) already went to the OPL3-SA3 master registers 07h/08h. aitotat's [FIXYMF](https://www.vogons.org/viewtopic.php?p=1036106#p1036106) does the same after UNISOUND | Vogons: [badmojo](https://www.vogons.org/viewtopic.php?p=1276551#p1276551) ([2](https://www.vogons.org/viewtopic.php?p=1373824#p1373824)), [--Freeman--](https://www.vogons.org/viewtopic.php?p=1452979#p1452979) | ✅ register writes only: `ymf719`, SB Pro master DDh before, 22h after (also with `/V50`, `/V00`); 07h/08h, the other mixer registers and the output unchanged. 86Box doesn't model the YMF715 table, so there SB Pro audio is about 20 dB quieter with this build | ❌ |
| [71ac681](https://github.com/dafugg/unisound_decomp/commit/71ac68182e51dc2a0eab69891489eb0946b15ac1) | `ULTRASND` parsing stops at the last field; a value with fewer than five fields gave IRQ 0 | — | ✅ `guspnp`, `ULTRASND=240,1,1`: `IRQ:0` before, `IRQ:11` after | ✅ Compaq LTE 5000 |
| [f0f3c49](https://github.com/dafugg/unisound_decomp/commit/f0f3c4983d5d82fc0039911fb4811f09d57e40b9) | `BLASTER` parsing stops at the last field instead of reading past the end of the string | — | ❌ No visible effect: `BLASTER=A220 I5 D1 H5 P330 T6` gives the same output with both builds | ✅ Compaq LTE 5000 |
| [57b36ad](https://github.com/dafugg/unisound_decomp/commit/57b36ade0588d6924a1459c90c50140c2ee28e76) | Ensoniq SoundscaPE VIVO (`ENS4…`): `/R` reads the IRQ and DMA registers as bytes, not as words that take in the next register | — | ❌ 86Box doesn't emulate the SoundscaPE VIVO (its only Ensoniq cards are the PCI AudioPCI models) | ❌ |
| [66d1b10](https://github.com/dafugg/unisound_decomp/commit/66d1b101702368b970c4aa1d492ba08899a78bf7) | `/R` reads the MPU port back from the register it was written to, on the Guillemot Maxi Sound 64 (ES1868 + SAM9407) and the cards with PnP IDs `RJA0…` and `TBS20…` (Turtle Beach) | — | ❌ 86Box doesn't emulate these cards (it has a plain ES1868, but not the Maxi Sound variant) | ❌ |
| [ce32fd2](https://github.com/dafugg/unisound_decomp/commit/ce32fd2472a11da985a03e8a4ad79028cd3fec46) | Option parsing no longer processes `argv[argc]`, the NULL after the last argument | — | ❌ No visible effect | ❌ |
| [ddb518b](https://github.com/dafugg/unisound_decomp/commit/ddb518b7edf0fe7c5d58d29f8161a60c0db523a5) | Card and device names are cut to the buffer size; a long ANSI name overflowed it | — | ✅ `sb16_pnp` with its PnP ROM patched to a 150-character name: `/CL` hung before, works after | ❌ |
| [d89926b](https://github.com/dafugg/unisound_decomp/commit/d89926b8799d1c1a254279b73d9ddbcd7d9fc1ca) | Guillemot Maxi Sound 64 (ES1868 + SAM9407): the SAM9407 firmware upload no longer sends a word past the end of the file | — | ❌ 86Box doesn't emulate the Maxi Sound 64 or the SAM9407 | ❌ |
| [23c6045](https://github.com/dafugg/unisound_decomp/commit/23c60457e6d81cbf492b87b21d842ccd58e9740f) | SB AWE32/64 FM reverb amount (`/VR50` to `/VR99`): the value went negative and set the pitch-target bits of FM passthrough voice 31 to `FFFF`. `/VR00` to `/VR49` were unaffected | — | ✅ `sbawe32_pnp`, `/VR99`, voice 31 PTRX: `FFFFFFFF` before, `0000FFFF` after | ❌ |
| [c96f1e8](https://github.com/dafugg/unisound_decomp/commit/c96f1e896e52fc641c6cacace324eaf012744625) | InterWave ROM bank names are NUL-terminated; a 16-character name ran into the next field | — | ✅ `guspnp` with `IWROM.BIN`'s bank name set to 16 characters: `[AMDGM_1_1_FULL16INTR]` before, `[AMDGM_1_1_FULL16]` after | ❌ |
| [0b8ded6](https://github.com/dafugg/unisound_decomp/commit/0b8ded635a05c977f601b5d1dfe9f5bbe566951e) | SB AWE32/AWE64 with 28 MB of RAM: reported as 28672K instead of 28671K | — | ✅ `sbawe32_pnp`, `sbawe64_gold`, `sbawe32` with 28672 KB | ❌ |
| [ad1c498](https://github.com/dafugg/unisound_decomp/commit/ad1c498519ea68d5e863a457d714bde041b406bc) | AZT2320 and Yamaha OPL3-SAx: D = H keeps D and moves only the second DMA (it was forced to 1/3). The AZT2320's second DMA is limited to 0, 1 and 3 | — | ✅ `azt2320`: D3 H3 gave 1/3, now 3/1; D1 H5 gave 1/5, now 1/3 | ❌ |
| [53c3685](https://github.com/dafugg/unisound_decomp/commit/53c368554d58b81cdc8add08ddd201405a56e2d7) | Aztech and Rockwell: SB Pro mixer 0Eh is left at 20h (mono), not 22h (stereo) | — | ✅ `azt2320`: mono playback L/R correlation 0.80 before, 1.00 after. 86Box has no Rockwell card | ❌ |
| [4369834](https://github.com/dafugg/unisound_decomp/commit/4369834a6474fcd767081cace27ba0f6adf647e7) | OAK OTI601 / OPTi 82C928 (`/NOPNP`): the test that picks the MC5 (F91h) value, `(codec_st & 0xfd) == 2`, was never true, so 20h was always written. With a CS4231/CS4248 codec, MC5 bit 1 (CFIX, the fix for that codec's synchronisation delays) and bit 3 (SB-mode command FIFO) stayed off. It now tests I12 bit 7, which is 1 on those codecs, as OSS mad16 and Linux opti92x-ad1848 do; AD1848 boards still get 20h. Found by cppcheck; the bits are from the 82C929 data sheet, since no 82C928 or OTI601 data sheet was found | — | ❌ 86Box doesn't emulate the 82C928 or the OTI601 | ❌ |
| Not fixed | OPTi 82C931: F (388h by default) is written to the FM base register (LD1 62h), which the card's resource data gives as 16-byte aligned. In 86Box that moves AdLib to 390h; real cards evidently still answer at 388h, so this is probably harmless. Writing 380h, as the BIOS and the Windows driver do, would match the resource data | — | ❌ 86Box moves AdLib to 390h, which real cards apparently don't | ❌ |

## Reporting problems

The build from `main` is unofficial: it is not a release by JazeFox, and JazeFox doesn't support it.
Before opening an issue here, check whether the problem also happens with the official v0.82a:

- **If it does**, report it in [the UNISOUND thread on VOGONS](https://www.vogons.org/viewtopic.php?t=72553)
  first, then open an issue here that links to your post.
- **If it happens only with the build from `main`**, open an issue here. Include the
  version line UNISOUND prints, your card, your BLASTER or ULTRASND setting and the command line.

Whatever the problem, run UNISOUND again with the same options plus `/LOG`, redirected to a file,
and attach the file: for example `UNISOUND /LOG > LOG.TXT`, or `UNISOUND /NOPNP /LOG > LOG.TXT`.
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

If UNISOUND finds no card, also try `/XSLAM` (a Crystal chip set up by the BIOS) and `/XESSB`
(an on-board ES1868/ES1869), with `/LOG`.

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
| `docs/TESTING.md` | Results of testing in 86Box |

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
derived from, UNISOUND by JazeFox, and remain his work.
