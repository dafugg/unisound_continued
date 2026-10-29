# Datasheets

Vendor documents used while working on UNISOUND, kept here so the references in the code and in
[TESTING.md](../TESTING.md) stay available. They belong to their vendors; the source of each copy
is listed below with its SHA-256.

ESS's data sheets, DEVNODES.DOC and the ES1688 product brief are from the ESS documentation
bundle mirrored by the ALSA project,
<https://sunsite.icm.edu.pl/Linux/alsa/manuals/ess/1/ess.tar.bz2> (sha256 `167cd4ab10c12469d91590b7eb66fa63f917a67d0955e2e769e98f227f75e2f8`),
folder `Document/`.

| File | Chip | Document | Source | SHA-256 |
|---|---|---|---|---|
| `ess/688.pdf` | ES688 | Product brief (scanned, no text layer; no register map) | <https://sunsite.icm.edu.pl/Linux/alsa/manuals/ess/688.pdf> | `f4c6bbc638fb27b38f3842498884820db0108507e5c83c14dffa1a24311ae797` |
| `ess/PB1688B.PDF` | ES1688 | Product brief (no register map) | ESS bundle | `4e313e0029b3b1c4f8476a1bfa67601d59e4b04a2cf39118de0f9aad1f5e8788` |
| `ess/es1888.pdf` | ES1888 | Design Guide Rev C, October 1995: DSP commands and extended mode, the nearest thing to an ES688/ES1688 programming reference | <https://sunsite.icm.edu.pl/Linux/linux-uk/alan/Kernel/Documents/ESS-tech/es1888.pdf> | `0b4980aadc659f3a35c63c43c156e6df69c98f5e143ffbf6bdc519dbec21251d` |
| `ess/DS1868B.PDF` | ES1868 | Data sheet, with an example PnP ROM (Appendix A) | ESS bundle | `f939b532d865050231f547ff874770ca706ca12bb97a3d7ce8a0e0e381e51c73` |
| `ess/DS1869.pdf` | ES1869 | Data sheet (SAM0023-121997), with an example PnP ROM (Appendix A) | ESS bundle | `f342f18b56032fa6c7115cc566bb07f7028982fb431127cf5c81fa4ece236b0d` |
| `ess/DS1878A.PDF` | ES1878 | Data sheet, with the chip's internal PnP ROM (Appendix A) | ESS bundle | `f663a85676879cf1c54faae8919a2c61e3ae4b300eb9e9fc9dbf429e2b551c2f` |
| `ess/ds1879a.pdf` | ES1879 | Data sheet, with the chip's internal PnP ROM (Appendix A) | ESS bundle | `c863b5d711770a0a9eccfd36f38cf267cd3943e56b0e2ad214b2f81320e04176` |
| `ess/DEVNODES.DOC` | all ESS AudioDrive | "ESS AudioDrive Device Nodes" v2.05, 1997-02-03 (Word 6): every legacy, PnP BIOS and ISA PnP ID per chip | ESS bundle | `6e026273e78821b8418af583683088e85af70addab511a99c5f04ecd69a430eb` |
| `opti/Opti 82C929.pdf` | OPTi 82C929 | Data sheet (scanned) | <https://raw.githubusercontent.com/JKnipperts/OPTi82c929_Driver/master/DOCS/Opti%2082C929.pdf> | `7751eaa2f98310ab51ec097b606cca44684867a9f3013f878ab94b6c01205173` |

Known errors: the ES1869 data sheet's example PnP ROM gives the control interface as ESS0000 (the
ES1868's; DEVNODES.DOC gives the ES1869 ESS0006) and prints one DMA tag as `002H` instead of
`02AH`. See "ESS PnP ROMs from the data sheets" in TESTING.md.

Not found anywhere: data sheets for the ES488, ES1488, ES968 and ES1898, a full ES688/ES1688
data sheet, and an OPTi 82C928 or OAK OTI-601 data sheet.
