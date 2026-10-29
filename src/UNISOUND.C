/* UNISOUND v0.82a - Universal ISA PnP Sound Card Driver for DOS, (c) JazeFox 2019-2025.
   Reconstructed source. Build: tcc -c -mt -O -d UNISOUND.C (Turbo C++ 3.0), link with C0T + CS.LIB. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dos.h>
#include <conio.h>
#undef inp
#undef outp
#undef outpw

/* Result of pnp_map_devices: for each function, the number of the card's logical device that
   provides it, or -1 if there is none.
   ld[0] SB / main audio     ld[3] gameport            ld[6] control port
   ld[1] second audio (OPL,  ld[4] wavetable (AWE)     ld[7] device that gets disabled
         WSS, or SB on GUS)  ld[5] IDE                       (Yamaha, Aztech)
   ld[2] MPU-401                                       ld[8] extra (MPU2 on EWS64, OPL on ADS7141)
   x[3] is byte 3 of the first logical device id; 'h' (0x68) marks CARD_ES968B. */
struct ldinfo {
    int ld[9];
    char x[7];
};
int inp(unsigned portid);
int outp(unsigned portid, int value);
unsigned outpw(unsigned portid, unsigned value);

/* longest ANSI identifier string kept; the buffers hold ANSI_MAX + 1 bytes or more */
#define ANSI_MAX       63

/* card families (the type code in configure_pnp and the PnP resource walkers) */
#define CARD_CREATIVE  0x01  /* Creative SB16/AWE (CTL) */
#define CARD_ALS       0x02  /* Avance Logic ALS (ALS, RWB1) */
#define CARD_ES1868    0x03  /* ESS ES1868 (ESS1868, SEM3006) */
#define CARD_YAMAHA    0x04  /* Yamaha OPL3-SAx (YMH, NMX22, QHA08, MPI0003) */
#define CARD_ROCKWELL  0x05  /* Rockwell (RSS) */
#define CARD_ES968     0x06  /* ESS ES968 (ESS096x) */
#define CARD_OPTI925   0x07  /* OPTi 82C925 (OPT..25) */
#define CARD_CMI       0x08  /* C-Media (CMI) */
#define CARD_AD1816    0x09  /* Analog Devices AD1816 based (ADS71, AXS71, TER11/14, ...) */
#define CARD_AZTECH    0x0a  /* Aztech (AZT) */
#define CARD_CRYSTAL   0x0b  /* Crystal CS42xx (CSC, CDC, TBS2004) */
#define CARD_OPTI93X   0x0c  /* OPTi 82C931/82C933 (OPT..31, OPT..33) */
#define CARD_ES968B    0x0d  /* ES968 whose first logical device id byte is 0x68 */
#define CARD_ES1869    0x0e  /* ESS ES1869 (ESS1869, ESS0003, ESS0006) */
#define CARD_OPTI924   0x10  /* OPTi 82C924 (OPT..24) */
#define CARD_ADS7141   0x13  /* ADS7141 */
#define CARD_RJA0      0x2c  /* RJA0 */
#define CARD_ENSONIQ   0x37  /* Ensoniq (ENS4) */
#define CARD_TBS20     0x3b  /* Turtle Beach (TBS20) */
#define CARD_GUSPNP    0x40  /* Gravis UltraSound PnP / InterWave (GRV, ADV55, STB01) */

/* ---- global data (initialized; addresses in the original DGROUP) ---- */
int emu_init1[32] = {
    0x0c10, 0x8470, 0x14fe, 0xb488, 0x167f, 0xa470, 0x18e7, 0x84b5, 0x1b6e, 0x842a, 0x1f1d, 0x852a,
    0x0da3, 0x8f7c, 0x167e, 0xf254, 0x0000, 0x842a, 0x0001, 0x852a, 0x18e6, 0x8baa, 0x1b6d, 0xf234,
    0x229f, 0x8429, 0x2746, 0x8529, 0x1f1c, 0x86e7, 0x229e, 0xf224 };  /* EMU8000 init data 1 (regs 0x40-0x5f, data port) */
int emu_init2[32] = {
    0x0da4, 0x8429, 0x2c29, 0x8529, 0x2745, 0x87f6, 0x2c28, 0xf254, 0x383b, 0x8428, 0x320f, 0x8528,
    0x320e, 0x8f02, 0x1341, 0xf264, 0x3eb6, 0x8428, 0x3eb9, 0x8528, 0x383a, 0x8fa9, 0x3eb5, 0xf294,
    0x3eb7, 0x8474, 0x3eba, 0x8575, 0x3eb8, 0xc4c3, 0x3ebb, 0xc5c3 };  /* EMU8000 init data 2 (regs 0x40-0x5f, data port +2) */
int emu_init3[32] = {
    0x0000, 0xa404, 0x0001, 0xa504, 0x141f, 0x8671, 0x14fd, 0x8287, 0x3ebc, 0xe610, 0x3ec8, 0x8c7b,
    0x031a, 0x87e6, 0x3ec8, 0x86f7, 0x3ec0, 0x821e, 0x3ebe, 0xd280, 0x3ebd, 0x821f, 0x3eca, 0x8386,
    0x3ec1, 0x8c03, 0x3ec9, 0x831e, 0x3eca, 0x8c4c, 0x3ebf, 0x8c55 };  /* EMU8000 init data 3 (regs 0x60-0x7f, data port) */
int emu_init4[32] = {
    0x3ec9, 0xc280, 0x3ec4, 0xbc84, 0x3ec8, 0x8ead, 0x3ec8, 0xd380, 0x3ec2, 0x8f7e, 0x3ecb, 0x8219,
    0x3ecb, 0xd2e6, 0x3ec5, 0x831f, 0x3ec6, 0xc380, 0x3ec3, 0xb27f, 0x3ec9, 0x8265, 0x3ec9, 0x8319,
    0x1342, 0xd3e6, 0x3ec7, 0xb37f, 0x0000, 0x8365, 0x1420, 0x9570 };  /* EMU8000 init data 4 (regs 0x60-0x7f, data port +2) */
unsigned char cs4232_fw_hdr[8] = {
    0xaa, 0x99, 0x2a, 0xc2, 0x53, 0x02, 0x12, 0x86 };  /* CS4232 firmware header (8 bytes) */
unsigned char cs4232_fw[3213] = {
    0xaa, 0x76, 0x21, 0x74, 0x10, 0x12, 0x2b, 0x9b, 0x74, 0x20, 0x78, 0x05,
    0xf2, 0x74, 0x09, 0x12, 0x17, 0x75, 0xe5, 0x15, 0x44, 0x80, 0x79, 0x05,
    0xf3, 0xf5, 0x15, 0x12, 0x17, 0x7f, 0x22, 0x79, 0x03, 0xe3, 0xa2, 0xe2,
    0x92, 0x26, 0xa2, 0xe3, 0x92, 0x27, 0x22, 0x12, 0x03, 0x90, 0xc2, 0x2a,
    0x75, 0x1e, 0x20, 0xe4, 0x12, 0x2b, 0x9b, 0x22, 0xa9, 0x16, 0x77, 0x33,
    0xe9, 0x04, 0x54, 0x03, 0x44, 0xa8, 0xf5, 0x16, 0xc2, 0x2f, 0x22, 0xa9,
    0x16, 0x77, 0x42, 0xe9, 0x04, 0x54, 0x03, 0x44, 0xa8, 0xf9, 0x77, 0xf8,
    0x04, 0x54, 0x03, 0x44, 0xa8, 0xf5, 0x16, 0xc2, 0x2f, 0x22, 0x10, 0xd5,
    0x05, 0x10, 0xd1, 0x09, 0x21, 0xe5, 0x31, 0xe6, 0x85, 0x5f, 0xa8, 0x80,
    0x05, 0x31, 0xf7, 0x85, 0x5f, 0xa8, 0x22, 0x79, 0xe3, 0x90, 0x20, 0x90,
    0xe7, 0xf0, 0xa3, 0xd9, 0xfb, 0xe5, 0x88, 0xf0, 0x90, 0x25, 0x73, 0x22,
    0x79, 0xe3, 0x90, 0x20, 0x90, 0xe0, 0xf7, 0xa3, 0xd9, 0xfb, 0xe0, 0xf5,
    0x88, 0x90, 0x25, 0x73, 0x75, 0x89, 0x16, 0x75, 0x8b, 0xff, 0x75, 0x8d,
    0xff, 0x75, 0xb8, 0x07, 0x22, 0x90, 0x25, 0x73, 0x30, 0x04, 0x03, 0x12,
    0x1f, 0x62, 0x30, 0x00, 0x03, 0x12, 0x15, 0xe6, 0x30, 0x01, 0x03, 0x12,
    0x29, 0xaf, 0x30, 0x02, 0x03, 0x12, 0x29, 0xaf, 0x30, 0x05, 0x03, 0x12,
    0x16, 0x65, 0x30, 0x06, 0x06, 0x12, 0x16, 0xb1, 0x12, 0x29, 0xaf, 0x30,
    0x07, 0x06, 0x12, 0x16, 0xe9, 0x12, 0x29, 0xaf, 0x75, 0x20, 0x00, 0x22,
    0x20, 0x97, 0x09, 0x53, 0xa8, 0xfb, 0x12, 0x04, 0x2c, 0x43, 0xa8, 0x04,
    0x22, 0x30, 0x4b, 0x08, 0xe5, 0x35, 0x70, 0x02, 0x15, 0x36, 0x15, 0x35,
    0x75, 0x4e, 0x02, 0x22, 0x10, 0x19, 0x02, 0x80, 0x08, 0x78, 0x00, 0xe2,
    0x78, 0x07, 0xf2, 0x61, 0x91, 0x78, 0x11, 0xe2, 0xc0, 0x01, 0xc0, 0xf0,
    0xc0, 0xd0, 0xc0, 0x02, 0x71, 0x16, 0xe5, 0x30, 0xb4, 0x01, 0x02, 0x61,
    0x89, 0x43, 0x08, 0x40, 0x12, 0x2a, 0x5c, 0x80, 0xf6, 0x12, 0x28, 0xba,
    0x79, 0x01, 0xe3, 0x75, 0x21, 0x3f, 0x75, 0x49, 0x11, 0x75, 0x4c, 0x11,
    0x51, 0x14, 0x75, 0x1a, 0x80, 0x31, 0x92, 0x75, 0x81, 0xe3, 0x12, 0x25,
    0x9d, 0x31, 0xd1, 0x43, 0xa8, 0x01, 0x00, 0x53, 0xa8, 0xfe, 0x10, 0x50,
    0x02, 0x80, 0x03, 0x12, 0x1a, 0x8d, 0xb1, 0xfc, 0x12, 0x03, 0xd9, 0xd1,
    0x02, 0x12, 0x2d, 0xf0, 0xb0, 0x11, 0x92, 0xe0, 0xa2, 0x2a, 0xa0, 0xb5,
    0x82, 0xe0, 0x50, 0x03, 0x79, 0x0f, 0xe3, 0x71, 0xca, 0x51, 0x4b, 0x91,
    0xe5, 0x53, 0xa8, 0xfb, 0x10, 0x10, 0x02, 0x80, 0x26, 0xc2, 0x8e, 0xd2,
    0xab, 0xa2, 0x1c, 0x40, 0x13, 0xa2, 0x1d, 0x50, 0x0a, 0x43, 0x08, 0x40,
    0x12, 0x1a, 0x01, 0xd1, 0xd7, 0x80, 0x0b, 0x12, 0x25, 0xd8, 0x61, 0x0a,
    0x43, 0x08, 0x40, 0x12, 0x1a, 0x01, 0xd2, 0x1f, 0x12, 0x17, 0x7f, 0x43,
    0xa8, 0x04, 0x51, 0x4b, 0x91, 0xe5, 0x12, 0x13, 0x34, 0x80, 0x96, 0xa2,
    0x17, 0x72, 0x16, 0x72, 0x15, 0x72, 0x2d, 0x50, 0x06, 0xfa, 0x12, 0x13,
    0x66, 0x80, 0x25, 0xc2, 0x13, 0x30, 0x28, 0x05, 0x12, 0x02, 0xbe, 0x80,
    0x1b, 0xb4, 0x10, 0x12, 0x78, 0x00, 0xf2, 0xe5, 0x30, 0xb4, 0x01, 0x06,
    0x12, 0x03, 0x90, 0xd2, 0x19, 0x22, 0x12, 0x00, 0xdd, 0x22, 0x75, 0x30,
    0x00, 0x12, 0x00, 0xa1, 0x22, 0x75, 0x1e, 0x00, 0x74, 0x0c, 0x12, 0x2b,
    0x9b, 0x74, 0x40, 0x79, 0x05, 0xf3, 0x74, 0x49, 0x12, 0x2b, 0x9b, 0x74,
    0x04, 0x79, 0x05, 0xf3, 0x75, 0x15, 0x04, 0x74, 0x10, 0x12, 0x2b, 0x9b,
    0x74, 0x00, 0x79, 0x05, 0xf3, 0x74, 0x17, 0x12, 0x2b, 0x9b, 0x74, 0x00,
    0x79, 0x05, 0xf3, 0x74, 0x0a, 0x12, 0x2b, 0x9b, 0x74, 0x20, 0x79, 0x05,
    0xf3, 0x79, 0xe0, 0x77, 0x20, 0x22, 0xd0, 0x02, 0xd0, 0xd0, 0xd0, 0xf0,
    0xd0, 0x01, 0xe5, 0x5f, 0xd0, 0xa8, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x90, 0x25, 0x73, 0x75, 0x26,
    0xff, 0x75, 0x27, 0xff, 0x75, 0x28, 0x03, 0x75, 0x13, 0xff, 0x75, 0x1f,
    0x00, 0x75, 0x14, 0xff, 0x22, 0x79, 0x06, 0xe5, 0x29, 0x60, 0x0b, 0xe3,
    0x30, 0xe1, 0xf8, 0xe5, 0x4f, 0x64, 0x80, 0x79, 0x07, 0xf3, 0x22, 0x10,
    0x4c, 0x01, 0x22, 0x30, 0x4b, 0x0a, 0xc2, 0x4b, 0xe5, 0x4d, 0x64, 0x80,
    0xf5, 0x4f, 0x80, 0x1e, 0xe5, 0x15, 0xa2, 0xe0, 0x82, 0xe6, 0x40, 0x02,
    0x80, 0x36, 0x30, 0x4a, 0x04, 0xb1, 0xba, 0x80, 0x0d, 0x30, 0x49, 0x05,
    0x12, 0x25, 0x58, 0x80, 0x05, 0x30, 0x48, 0x24, 0x91, 0x7f, 0x79, 0x06,
    0xe3, 0x30, 0xe0, 0x1a, 0x79, 0x06, 0xf3, 0xe5, 0x4e, 0x24, 0xff, 0x50,
    0x04, 0xf5, 0x4e, 0x80, 0x0d, 0x79, 0x0f, 0xf3, 0x20, 0x2a, 0x07, 0x12,
    0x2b, 0x32, 0x75, 0x29, 0x00, 0x22, 0x91, 0x1c, 0x22, 0x79, 0x0f, 0xe3,
    0xc0, 0xa8, 0x75, 0xa8, 0x00, 0x30, 0x2b, 0x03, 0xd0, 0xa8, 0x22, 0x79,
    0x0e, 0xf3, 0xd0, 0xa8, 0x22, 0x8a, 0xf0, 0xe5, 0x50, 0x10, 0xf3, 0x10,
    0x23, 0x23, 0x23, 0x25, 0xf0, 0x12, 0x2c, 0xb8, 0xa2, 0xe7, 0x92, 0xe4,
    0xc2, 0xe7, 0x80, 0x08, 0x23, 0x23, 0x23, 0x25, 0xf0, 0x12, 0x2c, 0x19,
    0x25, 0x4f, 0x20, 0xd2, 0x04, 0xf5, 0x4f, 0x80, 0x0a, 0x40, 0x05, 0x75,
    0x4f, 0x7f, 0x80, 0x03, 0x75, 0x4f, 0xff, 0xea, 0x12, 0x2c, 0x3c, 0x25,
    0x50, 0x20, 0xe7, 0x05, 0xb4, 0x03, 0x07, 0x80, 0x0c, 0x75, 0x50, 0x00,
    0x80, 0x09, 0x40, 0x05, 0x75, 0x50, 0x03, 0x80, 0x02, 0xf5, 0x50, 0x22,
    0xe5, 0x4d, 0xc4, 0x54, 0x0c, 0x03, 0x03, 0xfa, 0x91, 0xaa, 0x71, 0xb8,
    0xe5, 0x4d, 0xc4, 0x54, 0x03, 0xfa, 0x91, 0xaa, 0x71, 0xb8, 0xe5, 0x4d,
    0x54, 0x0c, 0x03, 0x03, 0xfa, 0x91, 0xaa, 0x71, 0xb8, 0xe5, 0x4d, 0x54,
    0x03, 0xfa, 0x91, 0xaa, 0x71, 0xb8, 0x22, 0x8a, 0xf0, 0xe5, 0x50, 0x23,
    0x23, 0x25, 0xf0, 0x12, 0x2b, 0xf6, 0x25, 0x4f, 0x20, 0xd2, 0x04, 0xf5,
    0x4f, 0x80, 0x0a, 0x40, 0x05, 0x75, 0x4f, 0x7f, 0x80, 0x03, 0x75, 0x4f,
    0xff, 0xea, 0x12, 0x2c, 0x40, 0x25, 0x50, 0x20, 0xe7, 0x05, 0xb4, 0x05,
    0x07, 0x80, 0x0c, 0x75, 0x50, 0x00, 0x80, 0x09, 0x40, 0x05, 0x75, 0x50,
    0x05, 0x80, 0x02, 0xf5, 0x50, 0x22, 0x30, 0x26, 0x03, 0x12, 0x1e, 0xf5,
    0x30, 0x27, 0x03, 0x12, 0x1f, 0x37, 0x30, 0x25, 0x09, 0x12, 0x1f, 0x4e,
    0x30, 0x23, 0x03, 0x12, 0x1f, 0x1e, 0x10, 0x22, 0x02, 0x80, 0x0a, 0xe5,
    0x3b, 0xb4, 0xff, 0x02, 0xc2, 0x20, 0x12, 0x1e, 0x79, 0x22, 0x78, 0x11,
    0xe2, 0x20, 0xe0, 0x07, 0xc0, 0x01, 0x12, 0x28, 0xba, 0xd0, 0x01, 0x78,
    0x00, 0xf2, 0x61, 0x91, 0x12, 0x2b, 0x32, 0x12, 0x17, 0x7f, 0x78, 0x00,
    0xf2, 0xaa, 0x35, 0xab, 0x36, 0xea, 0x24, 0xff, 0xfa, 0xeb, 0x34, 0xff,
    0xfb, 0x50, 0x03, 0xd2, 0x10, 0x22, 0x75, 0x37, 0x01, 0x75, 0x38, 0x00,
    0x75, 0x39, 0x00, 0x12, 0x04, 0x04, 0xd2, 0x8e, 0x22, 0xa2, 0x1c, 0x72,
    0x1d, 0x40, 0x07, 0x53, 0x08, 0xbf, 0x78, 0x00, 0xf2, 0x22, 0xb1, 0x1f,
    0x22, 0x71, 0xb8, 0x71, 0xb8, 0x71, 0xb8, 0x22, 0x12, 0x03, 0x84, 0x79,
    0x0f, 0xe3, 0x78, 0x00, 0xf2, 0x12, 0x2b, 0x28, 0xe5, 0x81, 0x24, 0xfc,
    0xf5, 0x81, 0x61, 0x89, 0xd2, 0x07, 0x78, 0x11, 0xe2, 0x44, 0x11, 0xf5,
    0x4c, 0xc2, 0x0f, 0x12, 0x29, 0xa3, 0x61, 0x89, 0x02, 0x1b, 0x77, 0x00,
    0xe1, 0x81, 0xe1, 0x9a, 0xd2, 0x2c, 0xa1, 0x0d, 0x20, 0x20, 0x02, 0xd2,
    0x26, 0x02, 0x1e, 0x35, 0x02, 0x1e, 0x61, 0x02, 0x1d, 0x8f, 0xc2, 0x8e,
    0x75, 0xa8, 0x9e, 0x22, 0x41, 0x67, 0x74, 0xff, 0x78, 0x12, 0xf2, 0x02,
    0x0e, 0xb5, 0x02, 0x29, 0x91, 0x00, 0x00, 0x00, 0xa1, 0x8f, 0xa1, 0x97,
    0x02, 0x1e, 0x6b, 0xe5, 0x4d, 0xc4, 0x54, 0x0f, 0xfa, 0x91, 0x30, 0x71,
    0xb8, 0xe5, 0x4d, 0x54, 0x0f, 0xfa, 0x91, 0x30, 0x71, 0xb8, 0x22, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xa1, 0x9a, 0x02, 0x1d, 0x8f, 0xc2, 0x8e, 0xd2,
    0xab, 0xc2, 0x10, 0x79, 0x0f, 0xf3, 0x22, 0x00, 0x02, 0x2a, 0x8d, 0x00,
    0xe1, 0xbc, 0xe1, 0xc7, 0x02, 0x1e, 0x27, 0x00, 0x78, 0x00, 0xf2, 0x78,
    0x0b, 0xe2, 0xf4, 0xf5, 0x4d, 0xd2, 0x4c, 0x61, 0x91, 0x30, 0xb5, 0x02,
    0xc2, 0x11, 0x22, 0x30, 0x51, 0x01, 0x22, 0x53, 0xa8, 0xfb, 0x12, 0x2d,
    0xf0, 0x50, 0x22, 0x79, 0x03, 0xe3, 0x20, 0xe4, 0x1c, 0xaa, 0x35, 0xab,
    0x36, 0xea, 0x24, 0xf0, 0xfa, 0xeb, 0x34, 0xff, 0xfb, 0x50, 0x0e, 0x10,
    0x1f, 0x02, 0x80, 0x09, 0x20, 0x2a, 0x03, 0x12, 0x2b, 0x32, 0x12, 0x2d,
    0xd6, 0x43, 0xa8, 0x04, 0x22, 0x79, 0xbd, 0xf1, 0x3d, 0x83, 0x22, 0xdd,
    0xbd, 0xbd, 0xbd, 0x61, 0xbd, 0x8d, 0x7a, 0xbd, 0xbd, 0xbd, 0xbd, 0x30,
    0xbd, 0xbd, 0xbd, 0x55, 0xbd, 0xbd, 0xbd, 0x52, 0xbd, 0xb6, 0xb6, 0xbd,
    0xbd, 0xbd, 0xbd, 0x00, 0xbd, 0xbd, 0xbd, 0xe8, 0xda, 0xbd, 0xbd, 0xcf,
    0xb9, 0xbd, 0xc4, 0xf1, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0x7b,
    0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0x70, 0x6a, 0x57, 0x47, 0x34,
    0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0x29, 0xbd, 0xbd, 0xbd,
    0xb6, 0xb6, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd,
    0x2e, 0x25, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xfe, 0xf5, 0xbd, 0xbd,
    0xbd, 0xbd, 0xbd, 0xbd, 0x19, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd,
    0x21, 0x8f, 0x09, 0xbd, 0xf9, 0x86, 0xbd, 0xbd, 0xbd, 0xd7, 0xbd, 0x9e,
    0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0x9b, 0xd1, 0x9d, 0xbd, 0xae, 0xbd, 0xbd,
    0xbd, 0xcb, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xab, 0xa5,
    0xbd, 0xba, 0xbd, 0xbd, 0xbd, 0xc3, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd,
    0x74, 0x10, 0x12, 0x2b, 0x9b, 0xe4, 0x78, 0x05, 0xf2, 0x74, 0x09, 0x12,
    0x17, 0x75, 0xe5, 0x15, 0x54, 0x7f, 0x79, 0x05, 0xf3, 0xf5, 0x15, 0x12,
    0x17, 0x7f, 0x22, 0x78, 0x11, 0xe2, 0xc0, 0xe0, 0x78, 0xe3, 0xe6, 0x60,
    0x03, 0x02, 0x1d, 0xb0, 0xd0, 0xe0, 0xb4, 0x33, 0x13, 0xd2, 0xd5, 0xe5,
    0x5f, 0xd0, 0x5f, 0xd0, 0x5f, 0xd0, 0x5f, 0xd0, 0x5f, 0x78, 0x00, 0xc0,
    0x00, 0x02, 0x27, 0x2a, 0xb4, 0xcc, 0x04, 0xd2, 0xd1, 0x80, 0xe8, 0xb4,
    0x3c, 0x06, 0x78, 0x00, 0xe2, 0x75, 0x87, 0x02, 0x02, 0x1d, 0xd5, 0xc0,
    0xe0, 0x78, 0x00, 0xe2, 0xd0, 0xe0, 0x02, 0x27, 0xb6, 0x00, 0x79, 0x02,
    0x12, 0x27, 0x3d, 0x02, 0x2d, 0x37, 0x14, 0x54, 0xf0, 0x60, 0x21, 0xe5,
    0xf0, 0x24, 0xb6, 0xe5, 0xf0, 0x50, 0x16, 0x24, 0x8b, 0x50, 0x15, 0xe5,
    0xf0, 0x24, 0x56, 0xe5, 0xf0, 0x50, 0x08, 0x24, 0x2f, 0x50, 0x09, 0xe5,
    0xf0, 0x24, 0xd9, 0x24, 0xd5, 0x24, 0xf0, 0x22, 0x15, 0x81, 0x15, 0x81,
    0xe9, 0x22, 0x78, 0x13, 0x74, 0x00, 0xf2, 0x75, 0x2e, 0x01, 0xd2, 0x6a,
    0xc2, 0x69, 0xc2, 0x68, 0xc2, 0x6c, 0x90, 0x25, 0x73, 0x75, 0xb8, 0x07,
    0x41, 0xa4, 0xc0, 0x01, 0xc0, 0xf0, 0xc0, 0xd0, 0xc0, 0x02, 0xe5, 0x3d,
    0x54, 0x7d, 0x03, 0x10, 0xe5, 0x05, 0x90, 0x28, 0x4b, 0x80, 0x03, 0x90,
    0x2b, 0x7c, 0x73, 0xe5, 0x3d, 0x30, 0xe5, 0x07, 0x74, 0xfd, 0x78, 0x00,
    0xf2, 0x61, 0x91, 0x90, 0x1a, 0x97, 0x74, 0xb6, 0xc0, 0xe0, 0x74, 0x27,
    0xc0, 0xe0, 0xc0, 0xa8, 0x02, 0x1b, 0xab, 0x90, 0x25, 0x73, 0xd0, 0xa8,
    0x22, 0x90, 0x27, 0x2a, 0xc0, 0x82, 0xc0, 0x83, 0xc0, 0xa8, 0xc1, 0xf2,
    0x90, 0x27, 0xb6, 0xc0, 0x82, 0xc0, 0x83, 0xc0, 0xa8, 0x02, 0x1e, 0x0a,
    0xea, 0x24, 0xf0, 0xfa, 0xeb, 0x34, 0xff, 0xfb, 0x50, 0x2e, 0x20, 0x0b,
    0x05, 0x85, 0x44, 0xe0, 0x80, 0x03, 0x75, 0xe0, 0x00, 0x30, 0xe1, 0x20,
    0xe5, 0x35, 0x24, 0xff, 0xf5, 0x35, 0xe5, 0x36, 0x34, 0xff, 0xf5, 0x36,
    0xc3, 0xe5, 0x36, 0x13, 0xf5, 0x36, 0xe5, 0x35, 0x13, 0xf5, 0x35, 0x75,
    0x3a, 0x10, 0x12, 0x1a, 0x77, 0x02, 0x18, 0x77, 0x75, 0x3a, 0x00, 0x12,
    0x1a, 0x77, 0x02, 0x18, 0x1b, 0x20, 0x4b, 0x04, 0x75, 0x4e, 0x03, 0x22,
    0xe5, 0x35, 0x24, 0xff, 0xf5, 0x35, 0xe5, 0x36, 0x34, 0xff, 0xf5, 0x36,
    0x75, 0x4e, 0x03, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x22, 0x02, 0x2c, 0x70, 0xd2, 0x00, 0x78, 0x11, 0xe2,
    0x44, 0x11, 0xf5, 0x3f, 0xc2, 0x08, 0x12, 0x29, 0xa3, 0x02, 0x23, 0x89,
    0x21, 0x62, 0x61, 0x40, 0x01, 0x3a, 0x01, 0x73, 0x21, 0x76, 0x61, 0xa8,
    0x21, 0x39, 0x21, 0x4a, 0x02, 0x2a, 0x84, 0x79, 0x06, 0xf3, 0xc0, 0xd0,
    0x12, 0x03, 0xd9, 0x78, 0x00, 0xf2, 0xd0, 0xd0, 0x22, 0x00, 0x00, 0x00,
    0x00, 0x02, 0x2c, 0xb4, 0x78, 0x11, 0xe2, 0x44, 0x11, 0x54, 0x0f, 0xf8,
    0xc4, 0x48, 0xd2, 0x05, 0xf5, 0x48, 0xc2, 0x0d, 0x31, 0xa3, 0x02, 0x23,
    0x89, 0x20, 0x4b, 0x04, 0x75, 0x4e, 0x01, 0x22, 0xe5, 0x35, 0x24, 0xff,
    0xf5, 0x35, 0xe5, 0x36, 0x34, 0xff, 0xf5, 0x36, 0x75, 0x4e, 0x01, 0x22,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79, 0xd0,
    0x77, 0x1b, 0x79, 0xd1, 0x77, 0x18, 0x79, 0xd2, 0x77, 0x77, 0x79, 0xd3,
    0x77, 0x18, 0x22, 0x75, 0x29, 0x00, 0x75, 0x25, 0x00, 0x75, 0x34, 0x03,
    0x75, 0x22, 0x00, 0x75, 0x23, 0x05, 0x75, 0x4f, 0x00, 0x75, 0x50, 0x00,
    0x75, 0x30, 0x00, 0x79, 0xdc, 0x77, 0x03, 0xc2, 0x8e, 0x75, 0x17, 0xa8,
    0x75, 0x16, 0xa8, 0x74, 0xaa, 0x79, 0x01, 0xf3, 0x79, 0xd7, 0x77, 0x74,
    0x79, 0xd8, 0x77, 0xff, 0x79, 0xd9, 0x77, 0x07, 0x79, 0xda, 0x77, 0x00,
    0x12, 0x21, 0x76, 0x43, 0x08, 0x40, 0x71, 0x32, 0x79, 0x0e, 0xe3, 0x10,
    0x51, 0x1c, 0x74, 0x06, 0x71, 0x9b, 0xe5, 0x11, 0x44, 0x80, 0x79, 0x05,
    0xf3, 0xf5, 0x11, 0x74, 0x07, 0x71, 0x9b, 0xe5, 0x12, 0x44, 0x80, 0x79,
    0x05, 0xf3, 0xf5, 0x12, 0x80, 0x18, 0x53, 0x27, 0xa0, 0x53, 0x28, 0x01,
    0x75, 0x20, 0xf7, 0x12, 0x23, 0x4c, 0x75, 0x11, 0x80, 0x75, 0x12, 0x80,
    0x12, 0x1f, 0xc0, 0x12, 0x22, 0x14, 0x79, 0x06, 0xf3, 0x22, 0xd2, 0x02,
    0x78, 0x11, 0xe2, 0x44, 0x11, 0xf5, 0x43, 0xc2, 0x0a, 0x12, 0x29, 0xa3,
    0x02, 0x23, 0x89, 0x78, 0x11, 0xe2, 0x44, 0x11, 0xf5, 0x44, 0xc2, 0x0b,
    0x12, 0x29, 0xa3, 0x02, 0x23, 0x89, 0x78, 0x00, 0xe2, 0x90, 0x25, 0x73,
    0x02, 0x23, 0x89, 0x78, 0x11, 0xe2, 0x75, 0x20, 0xf7, 0x75, 0x21, 0x3f,
    0x75, 0x49, 0x11, 0x75, 0x4c, 0x11, 0x31, 0xa3, 0x02, 0x23, 0x89, 0x78,
    0x11, 0xe2, 0x44, 0x11, 0x54, 0x0f, 0xf8, 0xc4, 0x48, 0xf8, 0xe5, 0x49,
    0x45, 0x3f, 0x58, 0xf5, 0x49, 0xd2, 0x06, 0xc2, 0x0e, 0x31, 0xa3, 0x02,
    0x23, 0x89, 0xc0, 0x01, 0x20, 0x2a, 0x04, 0x71, 0x32, 0xc2, 0x11, 0x11,
    0x5e, 0xc2, 0x1f, 0xd0, 0x01, 0x02, 0x23, 0x91, 0x12, 0x22, 0x14, 0x78,
    0x00, 0xf2, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79, 0xda, 0xe7, 0x70,
    0x2b, 0x20, 0x0a, 0x05, 0x85, 0x43, 0xe0, 0x80, 0x03, 0x75, 0xe0, 0x00,
    0x30, 0xe1, 0x1d, 0x20, 0xe2, 0x1f, 0x74, 0xe0, 0xca, 0x74, 0x00, 0x71,
    0x9b, 0xca, 0x79, 0x05, 0xf3, 0xf5, 0x09, 0xca, 0x74, 0x01, 0x71, 0x9b,
    0xca, 0x79, 0x05, 0xf3, 0xf5, 0x0a, 0x80, 0x43, 0x12, 0x15, 0x3e, 0x80,
    0x3e, 0xe5, 0x0b, 0xb4, 0x17, 0x02, 0x80, 0x0b, 0x50, 0x09, 0x74, 0x17,
    0xc3, 0x95, 0x0b, 0x44, 0x60, 0x80, 0x02, 0x74, 0x60, 0xca, 0x74, 0x00,
    0x71, 0x9b, 0xca, 0x79, 0x05, 0xf3, 0xf5, 0x09, 0xe5, 0x0c, 0xb4, 0x17,
    0x02, 0x80, 0x0b, 0x50, 0x09, 0x74, 0x17, 0xc3, 0x95, 0x0c, 0x44, 0x60,
    0x80, 0x02, 0x74, 0x60, 0xca, 0x74, 0x01, 0x71, 0x9b, 0xca, 0x79, 0x05,
    0xf3, 0xf5, 0x0a, 0x22, 0xd2, 0x04, 0x78, 0x11, 0xe2, 0x44, 0x11, 0xf5,
    0x46, 0xc2, 0x0c, 0x74, 0x0c, 0x71, 0x9b, 0x74, 0x40, 0x79, 0x05, 0xf3,
    0x31, 0xa3, 0x02, 0x23, 0x89, 0xd2, 0x05, 0x78, 0x11, 0xe2, 0x44, 0x11,
    0xf5, 0x48, 0xc2, 0x0d, 0x31, 0xa3, 0x02, 0x23, 0x89, 0xd2, 0x06, 0x78,
    0x11, 0xe2, 0x44, 0x11, 0xf5, 0x49, 0xc2, 0x0e, 0x31, 0xa3, 0x02, 0x23,
    0x89, 0x30, 0x1c, 0x21, 0x20, 0x4d, 0x1e, 0xe5, 0x29, 0x60, 0x1a, 0xc2,
    0x1c, 0x12, 0x19, 0xec, 0x12, 0x13, 0xcf, 0xd2, 0x4d, 0x12, 0x17, 0x7f,
    0x78, 0x00, 0xf2, 0x79, 0x06, 0xf3, 0x43, 0xa8, 0x04, 0x12, 0x24, 0x1c,
    0x22, 0x12, 0x25, 0x48, 0x22, 0x78, 0x00, 0xe2, 0x90, 0x25, 0x73, 0x02,
    0x23, 0x89, 0x78, 0x00, 0xe2, 0xa2, 0xe7, 0x72, 0xe3, 0x92, 0xe7, 0x02,
    0x1d, 0x85, 0x79, 0x04, 0xe3, 0x54, 0x80, 0x70, 0xf9, 0x22, 0xe5, 0x29,
    0x79, 0xde, 0xf7, 0x75, 0x29, 0x00, 0x70, 0x12, 0xe5, 0x15, 0x79, 0xdd,
    0xf7, 0x12, 0x2d, 0xf0, 0x40, 0x08, 0x20, 0x1c, 0x07, 0x20, 0x1d, 0x04,
    0x80, 0x02, 0x71, 0x32, 0x30, 0xb5, 0x0c, 0x79, 0x06, 0xf3, 0x20, 0x2a,
    0x06, 0x79, 0xdd, 0xe7, 0x54, 0xfc, 0xf7, 0xd2, 0x2b, 0x12, 0x21, 0x76,
    0x22, 0x00, 0x00, 0x00, 0x00, 0xe5, 0x15, 0xa2, 0xe0, 0xb0, 0xe6, 0x40,
    0x31, 0xa2, 0xe1, 0xb0, 0xe7, 0x40, 0x38, 0x10, 0x2b, 0x02, 0x80, 0x26,
    0x79, 0xde, 0xe7, 0x70, 0x0b, 0x79, 0xdd, 0xe7, 0x20, 0xe0, 0x12, 0x20,
    0xe1, 0x28, 0x80, 0x16, 0xf5, 0x29, 0x30, 0x4d, 0x11, 0x20, 0x4c, 0x0e,
    0x12, 0x24, 0x1c, 0x80, 0x09, 0x43, 0x08, 0x40, 0x12, 0x13, 0xcf, 0x12,
    0x17, 0x7f, 0xe5, 0x13, 0x20, 0xe4, 0x05, 0x12, 0x18, 0x1b, 0x80, 0x03,
    0x12, 0x18, 0x77, 0xc2, 0x2b, 0x22, 0x12, 0x26, 0xd7, 0x12, 0x13, 0xb7,
    0x22, 0x78, 0x04, 0x79, 0x00, 0xd9, 0xfe, 0xd8, 0xfa, 0x22, 0x00, 0x74,
    0x09, 0x71, 0x9b, 0xe5, 0x15, 0x54, 0xfc, 0x79, 0x05, 0xf3, 0xf5, 0x15,
    0x22, 0x78, 0x11, 0xe2, 0x44, 0x11, 0x54, 0x0f, 0xf8, 0xc4, 0x48, 0xf5,
    0x46, 0xc2, 0x0c, 0xd2, 0x04, 0x31, 0xa3, 0x02, 0x23, 0x89, 0x12, 0x26,
    0xd7, 0x12, 0x00, 0xb7, 0x22, 0x00, 0x79, 0x06, 0xf3, 0x74, 0x0a, 0x71,
    0x9b, 0x79, 0xe0, 0xe7, 0x44, 0x02, 0xf7, 0x79, 0x05, 0xf3, 0x22, 0x74,
    0x0a, 0x71, 0x9b, 0x79, 0xe0, 0xe7, 0x54, 0xfd, 0xf7, 0x79, 0x05, 0xf3,
    0x22, 0x21, 0x59, 0x41, 0x23, 0x21, 0x59, 0x41, 0x3c, 0x41, 0x4c, 0x21,
    0x59, 0x21, 0x59, 0x02, 0x25, 0x73, 0x00, 0x74, 0x0d, 0x71, 0x9b, 0x74,
    0x4d, 0x79, 0x05, 0xf3, 0xd2, 0x52, 0x22, 0x00, 0x53, 0x08, 0x40, 0x45,
    0x08, 0x45, 0x1e, 0x79, 0x04, 0xf3, 0xf5, 0x08, 0x22, 0xd2, 0x01, 0x78,
    0x11, 0xe2, 0x44, 0x11, 0xf5, 0x42, 0xc2, 0x09, 0x31, 0xa3, 0x02, 0x23,
    0x89, 0x00, 0x00, 0x00, 0x00, 0x71, 0x6e, 0x74, 0x09, 0x12, 0x17, 0x75,
    0xe5, 0x15, 0x44, 0x40, 0x79, 0x05, 0xf3, 0xf5, 0x15, 0x75, 0x3a, 0x00,
    0x12, 0x1a, 0x77, 0x02, 0x18, 0x1b, 0xf5, 0x38, 0xe5, 0x37, 0x24, 0x01,
    0xf5, 0x37, 0xe5, 0x38, 0x34, 0x00, 0xf5, 0x38, 0x40, 0x05, 0x75, 0x39,
    0x00, 0x80, 0x03, 0x75, 0x39, 0x01, 0x12, 0x04, 0x04, 0xd2, 0x8e, 0x02,
    0x03, 0x8d, 0x00, 0xb4, 0x0d, 0x03, 0x74, 0x14, 0x22, 0x04, 0x83, 0x22,
    0x00, 0x02, 0xff, 0x01, 0x00, 0x05, 0xfe, 0xff, 0x00, 0x0a, 0xfc, 0xfe,
    0x00, 0xc0, 0xf8, 0xfc, 0x00, 0x28, 0xf0, 0xf8, 0x00, 0x30, 0xe0, 0xd0,
    0x01, 0x88, 0x04, 0x83, 0x22, 0x00, 0xff, 0xfe, 0xfd, 0xfc, 0xfc, 0xfb,
    0xfa, 0xfe, 0xfd, 0xfb, 0xf9, 0xf7, 0xf7, 0xf5, 0xf3, 0xfc, 0xfa, 0xf6,
    0xf2, 0xee, 0xee, 0xea, 0xe6, 0xf8, 0xf4, 0xec, 0xe4, 0xdc, 0xd4, 0xcc,
    0xc4, 0x24, 0x21, 0x83, 0x22, 0x04, 0x83, 0x22, 0xff, 0x01, 0xff, 0x01,
    0x00, 0x00, 0x00, 0x02, 0x22, 0x58, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0xff, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x01, 0x01, 0x83, 0x22, 0x8a, 0x01, 0x20, 0x01, 0x0b,
    0xea, 0xf3, 0xf9, 0x5f, 0x7e, 0x6b, 0xd5, 0x01, 0x00, 0x01, 0x00, 0x01,
    0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x3a, 0x01, 0x38, 0x01, 0x4b, 0x01, 0x49, 0x01, 0x5c, 0x01, 0x5a, 0x01,
    0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x14, 0x24, 0x48, 0x83, 0x22, 0x04, 0x83, 0x22, 0x00, 0x01, 0x02, 0x03,
    0x04, 0x06, 0x07, 0x08, 0x00, 0x03, 0x05, 0x07, 0x09, 0x0d, 0x0f, 0x81,
    0x00, 0x06, 0x0a, 0x0e, 0x82, 0x8a, 0x8e, 0x22, 0x00, 0x0c, 0x84, 0x8c,
    0x24, 0x2c, 0xa4, 0xac, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xaa, 0x35, 0xab, 0x36, 0x02, 0x27, 0xd3, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x02, 0x02,
    0x02, 0x25, 0x03, 0x03, 0x2b, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03,
    0x00, 0x03, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x83, 0x22, 0x00, 0x02, 0x02, 0x02, 0x01, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x01, 0x02, 0x02, 0x02, 0x2b, 0x02, 0x02, 0x02, 0x01, 0x02,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x01, 0x02, 0x02, 0x02, 0x01, 0x01,
    0x02, 0x02, 0x01, 0x01, 0x02, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x01, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x01, 0x02,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x00,
    0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02, 0x01, 0x02, 0x02, 0x01, 0x01, 0x02, 0x02, 0x02,
    0x00, 0x02, 0x21, 0x02, 0x02, 0x02, 0x02, 0x02, 0x01, 0x00, 0x02, 0x02,
    0x01, 0x02, 0x02, 0x02, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x02, 0x21, 0x01, 0x02, 0x21, 0x02, 0x02, 0x02, 0x00, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x20, 0xb5, 0x05, 0x79, 0x0f, 0xf3, 0xc2, 0x11, 0x22,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xe5, 0x15, 0xa2, 0xe0, 0xb0, 0xe6, 0x50,
    0x01, 0x22, 0xa2, 0xe1, 0xb0, 0xe7, 0x22, 0x0e, 0x01 };  /* CS4232 firmware body (0xc8d bytes) */
int sb_port = 0x0220;  /* SB base port (BLASTER A) */
int opl_port = 0x0388;  /* OPL port (F) */
int awe_port = 0x0620;  /* AWE port (E) */
int mpu_port = 0x0330;  /* MPU-401 port (P) */
int joy_port = 0x0299;  /* gameport (J); 0x299 = not set */
int wss_port = 0x0530;  /* WSS codec port (W) */
int yamaha_ctl_port = 0x0370;  /* Yamaha OPL3-SAx control port (R) */
int crystal_ctl_port = 0x0120;  /* Crystal control port */
int opti_ctl_port = 0x0f0c;  /* OPTi control port */
int ess_ctl_port = 0x0800;  /* ESS control port (R) */
int mpu2_port = 0x0310;  /* 2nd MPU port (M) */
int ide_port = 0;  /* IDE port (C) */
int sb_irq = 5;  /* IRQ (I) */
int irq2 = 0;  /* 2nd IRQ (Q) */
int mpu_irq = -1;  /* MPU IRQ (U); -1 = auto */
int ide_irq = 0;  /* IDE IRQ (Y) */
int sb_dma = 1;  /* DMA (D) */
int sb_hdma = 1;  /* high DMA (H) */
int wss_dma = 0;  /* WSS DMA (K) */
int aux_ctl_port = 0x0250;  /* control port for Rockwell / GUS PnP / Ensoniq (R); TEA6330T port on GUS */
int gus_port = 0x0240;  /* GUS port (ULTRASND) */
int gus_dma = 3;  /* GUS DMA 1 */
int gus_dma2 = 3;  /* GUS DMA 2 */
int gus_irq = 0x000b;  /* GUS IRQ */
int gus_irq2 = 0;  /* GUS IRQ 2 */
unsigned pnp_addr_port = 0x0279;  /* PnP address port */
unsigned pnp_wdata_port = 0x0a79;  /* PnP write data port */
int pnp_rdata_port = 0;  /* PnP read data port (found by isolation) */
unsigned char master_reg = 0xdd;  /* master volume, mixer register value */
int master_vol = 0x0055;  /* master volume 0-99 (/Vnn) */
unsigned char fm_reg = 0xcc;  /* FM volume register value */
int fm_vol = 0x0050;  /* FM volume (/VFnn) */
unsigned char wave_reg = 0xcc;  /* wave volume register value */
int wave_vol = 0x0050;  /* wave volume (/VWnn) */
unsigned char line_reg = 0;  /* line-in volume register value */
int line_vol = 0;  /* line-in volume (/VLnn) */
unsigned char cd_reg = 0;  /* CD volume register value */
int cd_vol = 0;  /* CD volume (/VCnn) */
unsigned char mic_reg = 0;  /* mic volume register value */
int mic_vol = 0;  /* mic volume (/VMnn) */
unsigned char pcspk_reg = 0;  /* PC speaker volume, mixer bits 7-5 */
int pcspk_vol = 0;  /* PC speaker volume (/VPnn) */
unsigned char treble_reg = 0x80;  /* treble, mixer high nibble */
int treble_lvl = -1;  /* treble (/VTnn); -1 = not set */
unsigned char bass_reg = 0x80;  /* bass, mixer high nibble */
int bass_lvl = -1;  /* bass / AuxB (/VBnn); -1 = not set */
char opt_cs = 0;  /* /CS: initialize a non-PnP Creative card */
unsigned char opt_read = 0;  /* /R: read settings only */
int opt_disable = 0;  /* /D: disable the card */
char opt_xxf = 0;  /* /XXF: ignore bad checksums; set to 2 when one was bad, 1 when good */
unsigned char isolate_limit = 4;  /* isolation gives up after this many bits without a response (/FS: 0x50) */
int awe_chorus = 0;  /* AWE chorus amount 00-99 (/VUnn) */
int awe_reverb = 0;  /* AWE reverb amount 00-99 (/VRnn) */
unsigned opti93x_pwd_port = 0x0f8d;  /* OPTi 93x password port */
int opt_xof = -1;  /* /XOFi = 1, /XOFe = 0, /XRU = 8; -1 = default */
int opt_xos = 0;  /* /XOS1: S/PDIF on */
int opt_xom = 2;  /* /XOM1 = 1, /XOM0 = 0; 2 = default */
int opt_xgc = -1;  /* /XGCn: GUS PnP IDE port; -1 = default */
int opt_xgl = 0;  /* /XGL: limit GUS RAM to 512K */
int opt_xgj = 0;  /* /XGJ: GUS joystick */
int opt_xwss = 0;  /* /XWSS: WSS mode */
int opt_xea = 0;  /* /XEA: alternate ES1868 init */
int opt_xnet = 0;  /* /XNET: Realtek PnP network card workaround */
int opt_x3d = 0;  /* /X3Dn: 3D level */
int opt_xxg = 0;  /* /XXGn: SB16 gain */
int nopnp_mode = 0;  /* /NOPNP = 1; 2 = OPTi control chip found */

/* ---- functions ---- */
void print_banner(void);
void print_help(void);
int configure_pnp(int card, int list, int pass, unsigned port);
void pnp_send_key(void);
void pnp_write(unsigned char reg, unsigned char val);
unsigned char pnp_read(unsigned char reg);
void pnp_write16(unsigned char reg, unsigned val);
unsigned pnp_read16(unsigned char reg);
void idx45_write(unsigned port, unsigned char idx, unsigned char val);
void idx01_write(unsigned port, unsigned char idx, unsigned char val);
void idx45_write_slow(unsigned port, unsigned char idx, unsigned char val);
unsigned char opti92x_read(unsigned port, unsigned char reg);
void opti92x_write(unsigned port, unsigned char reg, unsigned char val);
void opti93x_write(unsigned port, unsigned char idx, unsigned char val);
unsigned char opti93x_read(unsigned port, unsigned char idx);
int pnp_isolate(unsigned port, unsigned char *buf);
char *pnp_vendor_str(unsigned long *id);
int parse_ultrasnd(void);
int parse_blaster(void);
int pnp_read_ansi_name(unsigned char csn, unsigned char *buf, int type);
int pnp_map_devices(unsigned char csn, char *name, int *ld, int type);
int pnp_read_res(unsigned char *buf, int n);
int pnp_read_tag(unsigned char *tag, unsigned *len);
int pnp_read_str(char *buf, unsigned len);
void awe_init(unsigned port);
void sb_print_dsp_ver(int port);
int init_nonpnp_sb16(void);
void sb16_set_irq_dma(int port, unsigned char irq, unsigned char dma, unsigned char hdma);
unsigned char sb16_get_irq(int port);
unsigned char sb16_get_dma(int port);
unsigned char awe_detect(int port);
void print_mixer(char *name, int x3d, int gain);
void sbpro_mixer_set(void);
void sb16_mixer_set(void);
void sb16_mixer_switches(void);
void opti92x_init(int mode);
void opti93x_init(void);
void sam9407_upload(unsigned base);
unsigned char iw_mem_read(unsigned long addr);
void iw_mem_write(unsigned long addr, unsigned char val);
int iw_list_roms(void);
int iw_detect_dram(unsigned long *dst);
void iw_write(unsigned long reg, unsigned val);
void iw_init(void);
void iw_set_master(int flag, unsigned char v);
void iw_set_dac(int flag, unsigned char v);
void iw_set_synth(int flag, unsigned char v);
void iw_set_line(int flag, unsigned char v);
void iw_set_cd(int flag, unsigned char v);
void iw_set_mic(int flag, unsigned char v);
void emu_wait_read(void);
void emu_wait_write(void);
int emu_detect_dram(void);
void emu_dma_open(int odd_read);
void emu_dma_close(void);
void emu_write32(unsigned reg, unsigned port, unsigned long val);
void emu_write16(unsigned reg, unsigned port, unsigned val);
unsigned emu_read16(unsigned reg, unsigned port);
unsigned long emu_read32(unsigned reg, unsigned port);
int emu_init(int ver);
void use_alt_pnp_ports(void);
void send_alt_pnp_key(void);
void cs4232_upload_fw(void);
void crystal_ext_init(int card);
void cs_ctl_write(unsigned char reg, unsigned char val);
void cs_ctl_setbits(unsigned char reg, unsigned char bits);
void cs_xreg_write(unsigned char unit, unsigned char val);
int opl_detect(void);
void opl_write();   /* no prototype in the original: callers push promoted ints */
void xnet_workaround(int port);
void tea6330_write(unsigned char reg, unsigned char val);
void i2c_write_byte(unsigned char v);
void i2c_lines(unsigned char v);
unsigned char opti928_read(unsigned port);
void opti928_write(unsigned port, unsigned char val);
int init_opti928(void);
void probe_opti_ctl(void);
int init_opti930(void);
int init_es688(void);
void sb_dsp_flush(int port);

/* Entry point. Uppercases every argument and parses the options into the global settings:
   /Cn /CL /CS (card selection), /Vxnn (volumes and AWE effects), /F and /FS (wider isolation
   scan), /R (read only), /D (disable), the /Xxxx extended options and /NOPNP. /H or /? prints the
   help and exits. Then calls configure_pnp and returns its result as the exit code
   (0 = success, 1 = no card configured).
   Note: the loop runs to i <= argc, one past the last argument, so argv[argc] (NULL) is read too. */
int main(int argc, char *argv[])
{
    int full_scan = 0;
    int scan_port = 0x20b;
    int card = 1;
    int list = 0;
    int i = 0;
    int j = 0;

    if (argc > 1) {
        for (i = 1; i < argc; i++) {
            for (j = 1; j <= strlen(argv[i]); j++)
                argv[i][j] = toupper(argv[i][j]);
            if (argv[i][0] == '/' || argv[i][0] == '-') {
                if (argv[i][1] == 'C') {
                    if (argv[i][2] == 'L') {
                        card = 10;
                        list = 1;
                    } else if (list < 1) {
                        if (argv[i][2] == 'S')
                            opt_cs = 1;
                        if (isdigit(argv[i][2]))
                            card = argv[i][2] - '0';
                    }
                }
                if (argv[i][1] == 'V') {
                    if (isdigit(argv[i][2]) && isdigit(argv[i][3])) {
                        master_vol = argv[i][3] + (argv[i][2] - '0') * 10 - '0';
                        master_reg = master_vol * 15 / 95 * 17;
                    }
                    if (argv[i][2] == 'F' && isdigit(argv[i][3]) && isdigit(argv[i][4])) {
                        fm_vol = argv[i][4] + (argv[i][3] - '0') * 10 - '0';
                        fm_reg = fm_vol * 15 / 95 * 17;
                    }
                    if (argv[i][2] == 'W' && isdigit(argv[i][3]) && isdigit(argv[i][4])) {
                        wave_vol = argv[i][4] + (argv[i][3] - '0') * 10 - '0';
                        wave_reg = wave_vol * 15 / 95 * 17;
                    }
                    if (argv[i][2] == 'L' && isdigit(argv[i][3]) && isdigit(argv[i][4])) {
                        line_vol = argv[i][4] + (argv[i][3] - '0') * 10 - '0';
                        line_reg = line_vol * 15 / 95 * 17;
                    }
                    if (argv[i][2] == 'C' && isdigit(argv[i][3]) && isdigit(argv[i][4])) {
                        cd_vol = argv[i][4] + (argv[i][3] - '0') * 10 - '0';
                        cd_reg = cd_vol * 15 / 95 * 17;
                    }
                    if (argv[i][2] == 'M' && isdigit(argv[i][3]) && isdigit(argv[i][4])) {
                        mic_vol = argv[i][4] + (argv[i][3] - '0') * 10 - '0';
                        mic_reg = mic_vol * 15 / 95 * 17;
                    }
                    if (argv[i][2] == 'P' && isdigit(argv[i][3]) && isdigit(argv[i][4])) {
                        pcspk_vol = argv[i][4] + (argv[i][3] - '0') * 10 - '0';
                        pcspk_reg = pcspk_vol * 7 / 95 << 5;
                    }
                    if (argv[i][2] == 'T' && isdigit(argv[i][3]) && isdigit(argv[i][4])) {
                        treble_lvl = argv[i][4] + (argv[i][3] - '0') * 10 - '0';
                        treble_reg = treble_lvl * 15 / 93 << 4;
                    }
                    if (argv[i][2] == 'B' && isdigit(argv[i][3]) && isdigit(argv[i][4])) {
                        bass_lvl = argv[i][4] + (argv[i][3] - '0') * 10 - '0';
                        bass_reg = bass_lvl * 15 / 93 << 4;
                    }
                    if (argv[i][2] == 'U' && isdigit(argv[i][3]) && isdigit(argv[i][4]))
                        awe_chorus = argv[i][4] + (argv[i][3] - '0') * 10 - '0';
                    if (argv[i][2] == 'R' && isdigit(argv[i][3]) && isdigit(argv[i][4]))
                        awe_reverb = argv[i][4] + (argv[i][3] - '0') * 10 - '0';
                }
                if (argv[i][1] == 'F') {
                    full_scan = 1;
                    if (argv[i][2] == 'S') {
                        scan_port = 0x20b;
                        isolate_limit = 0x50;
                    }
                }
                if (argv[i][1] == 'R')
                    opt_read = 1;
                if (argv[i][1] == 'D')
                    opt_disable = 1;
                if (argv[i][1] == 'X') {
                    if (argv[i][2] == 'O') {
                        if (argv[i][3] == 'F') {
                            if (argv[i][4] == 'I')
                                opt_xof = 1;
                            else if (argv[i][4] == 'E')
                                opt_xof = 0;
                        }
                        if (argv[i][3] == 'S' && argv[i][4] == '1')
                            opt_xos = 1;
                        if (argv[i][3] == 'M') {
                            if (argv[i][4] == '1')
                                opt_xom = 1;
                            else if (argv[i][4] == '0')
                                opt_xom = 0;
                        }
                    }
                    if (argv[i][2] == 'G') {
                        if (argv[i][3] == 'C') {
                            int gc_sel = argv[i][4] - '0';
                            if (gc_sel > 1 && gc_sel < 6) {
                                switch (gc_sel) {
                                case 2: opt_xgc = 0x170; break;
                                case 3: opt_xgc = 0x1e8; break;
                                case 4: opt_xgc = 0x168; break;
                                case 5: opt_xgc = 0; break;
                                }
                            }
                        }
                        if (argv[i][3] == 'L')
                            opt_xgl = 1;
                        if (argv[i][3] == 'J')
                            opt_xgj = 1;
                    }
                    if (argv[i][2] == 'W' && argv[i][3] == 'S')
                        opt_xwss = 1;
                    if (argv[i][2] == 'E' && argv[i][3] == 'A')
                        opt_xea = 1;
                    if (argv[i][2] == 'X') {
                        if (argv[i][3] == 'F')
                            opt_xxf = 1;
                        if (argv[i][3] == 'G' && isdigit(argv[i][4]))
                            opt_xxg = argv[i][4] - '0';
                    }
                    if (argv[i][2] == 'N' && argv[i][3] == 'E')
                        opt_xnet = 1;
                    if (argv[i][2] == '3' && isdigit(argv[i][4]))
                        opt_x3d = argv[i][4] - '0';
                    if (argv[i][2] == 'R' && argv[i][3] == 'U')
                        opt_xof = 8;
                }
                if (argv[i][1] == 'N' && argv[i][2] == 'O') {
                    nopnp_mode = 1;
                    probe_opti_ctl();
                }
                if (argv[i][1] == 'H' || argv[i][1] == '?') {
                    print_help();
                    return 0;
                }
            }
        }
    }
    return configure_pnp(card, list, full_scan, scan_port);
}

/* Print the program name and version line, then a 73-character rule of box-drawing characters
   (0xC4). */
void print_banner(void)
{
    char line[74];

    puts("Universal ISA PnP Sound Card Driver for DOS v0.82a. (c) JazeFox 2019-2025\0");
    memset(line, 0xc4, 73);
    line[73] = 0;
    printf("%s\n", line);
}

/* Print the banner and the usage screen for /H and /?. */
void print_help(void)
{
    print_banner();
    puts("Options:");
    puts("   /Cn    Select PnP card number 'n' to initialize/read (1-9)");
    puts("   /CL    List all PnP cards present in the system");
    puts("   /R     Read PnP card current settings only");
    puts("   /D     Disable PnP sound card");
    puts("   /Vnn   Set Master Volume (00-99)");
    puts("   /VWnn  Set Wave Volume (00-99)");
    puts("   /VFnn  Set FM/Synth/Wavetable Volume (00-99)");
    puts("   /VLnn  Set Line-in Volume (00-99)");
    puts("   /VCnn  Set CD Volume (00-99)");
    puts("   /VMnn  Set Mic Volume (00-99)");
    puts("   /VPnn  Set PC Speaker-in Volume (00-99) (Creative)");
    puts("   /VTnn  and /VBnn Set Treble/Bass/AuxB level (00-99) On supported cards");
    puts("   /VRnn  and /VUnn Set FM Reverb/Chorus amount (00-99) (SB AWE only)");
    puts("   /Xxxx  eXtended settings (See TXT section 5)");
    puts("   /NOPNP Detect & init some Non-PnP cards (See TXT end of section 3)");
    puts("   /H /?  This help screen");
    puts("\nUNISOUND (w/o options) initializes 1st card found with default vols.");
    puts("If BLASTER/ULTRASND env var is found, values will be used to configure");
    puts("the card. If not, default values will be used. This program is FREEWARE!");
}

/* The main routine: find the PnP sound card, assign its resources and set up its mixer.

   card   which card to configure (/Cn); with /CL it is 10 and every card found is listed instead
   list   1 for /CL: also print each card's ANSI name
   pass   0 for a normal scan, 1 with /F: where the isolation scan of read-data ports starts
   port   first read-data port to try (0x20B)

   Steps:
   1. /CS goes straight to init_nonpnp_sb16. /NOPNP tries init_es688, init_opti930, init_opti928 and
      init_nonpnp_sb16 in turn.
   2. Send the initiation key and isolate cards, scanning read-data ports from `port` upward
      until one answers. Each isolated card gets the next CSN.
   3. Identify each card by its vendor id (CTL, ALS, ESS..., see the CARD_* list). Cards before
      number `card` are listed and skipped. Stop at card `card` if it is supported, and abort if
      it isn't. With /CL (card = 10) every card is listed.
   4. Read the card's ANSI name and map its logical devices to functions (pnp_map_devices).
   5. Load settings from BLASTER, or ULTRASND for GUS PnP. /D zeroes them to disable the
      card. Then write, or with /R read back, each logical device's ports, IRQs and DMAs. The PnP
      register numbers differ per family (the reg_* locals), and so do a lot of special cases.
   6. Leave the PnP configuration state and set up the card's mixer, firmware and extras
      (AWE, SAM9407, CS4232 firmware, InterWave, TEA6330T tone control).
   If no supported PnP card is found, it falls back to init_nonpnp_sb16.
   Returns 0 on success, 1 on failure. */
int configure_pnp(int card, int list, int pass, unsigned port)
{
    char *id = 0;               /* EISA id string of the current card */
    char buf[9] = {0};          /* serial identifier read by isolation */
    int found = 0;              /* isolation found a card */
    unsigned char csn = 0;      /* card select number */
    unsigned char cardno = 0;   /* card counter for /CL listing */
    int nsup = 0;               /* number of supported cards found */
    register int type = 0;      /* card family code */
    unsigned char ess_maxi = 0;      /* ESS: Maxi Sound variant */
    unsigned char cs_variant = 0;      /* Crystal: 2 = Orpheus, 3 = Turtle Beach, 1/4/7 by id */
    int als_variant = 0;                /* ALS: sub-variant */
    unsigned char cmi_variant = 0;      /* CMI: 1 = AD- name; RSS: 8 = RSS58 */
    int ctl_state = 0;                /* Creative: CQM/AWE state */
    unsigned char nlisted = 0;      /* cards listed */
    unsigned char reg_sb = 0x60;   /* PnP config registers used for each resource; vary by card */
    unsigned char reg_mpu = 0x62;
    unsigned char reg_opl = 0x64;
    unsigned char reg_wss = 0x62;
    unsigned char reg_irq = 0x70;
    unsigned char reg_irq2 = 0x72;
    unsigned char reg_dma = 0x74;
    unsigned char reg_dma2 = 0x75;
    int ret = 0;                /* return value */

    print_banner();
    if (opt_cs == 1) {
        if (init_nonpnp_sb16() == 0)
            return 1;
        return 0;
    }
    if (nopnp_mode == 1) {
        if (init_es688() == 0 && init_opti930() == 0 && init_opti928() == 0 && init_nonpnp_sb16() == 0) {
            puts("No card detected");
            return 1;
        }
        return 0;
    }
    pnp_write(2, 2);
    delay(2);
    pnp_send_key();
    pnp_write(2, 4);
    pnp_write(2, 2);
    delay(2);
    pnp_send_key();
    pnp_write(3, 0);
    while (found == 0 && pass < 2) {
        if (pass == 1)
            pnp_write(0, port >> 1 >> 1);
        delay(1);
        found = pnp_isolate(port, buf);
        if (found == 0) {
            if (pass == 1)
                pnp_write(3, 0);
            delay(1);
        }
        if (found != 0) {
            pnp_rdata_port = port;
            if (isolate_limit < 10) {
                pnp_write(3, 0);
                pnp_write(0, pnp_rdata_port >> 1 >> 1);
                delay(1);
                pnp_isolate(pnp_rdata_port, buf);
            }
        } else
            pnp_rdata_port = 0;
        if ((port >= 0x27b && pass == 0) || port >= 0x393) {
            pass++;
            port = 0x1eb;
        }
        if (pass == 0)
            port += 8;
        else
            port += 0x20;
    }
    while (found != 0) {
        csn++;
        pnp_write(6, csn);
        delay(1);
        id = pnp_vendor_str((unsigned long *)buf);
        if (id && strlen(id) > 5 && nsup < 6) {
            if (memcmp(id, "CTL", 3) == 0) {
                type = CARD_CREATIVE;
                if (memcmp(id + 5, "F4", 2) == 0)
                    ctl_state = 2;
            } else if (memcmp(id, "ALS", 3) == 0) {
                type = CARD_ALS;
                if (memcmp(id + 5, "07", 2) == 0)
                    als_variant = 1;
                else if (memcmp(id + 3, "0001", 4) == 0)
                    als_variant = 3;
            } else if (memcmp(id, "RWB1", 4) == 0) {
                type = CARD_ALS;
                als_variant = 2;
            } else if (memcmp(id, "ESS", 3) == 0) {
                if (memcmp(id + 3, "096", 3) == 0)
                    type = CARD_ES968;
                else if (memcmp(id + 3, "1868", 4) == 0)
                    type = CARD_ES1868;
                else if (memcmp(id + 3, "0003", 4) == 0)
                    type = CARD_ES1869;
                else if (memcmp(id + 3, "0006", 4) == 0)
                    type = CARD_ES1869;
                else if (memcmp(id + 3, "1869", 4) == 0)
                    type = CARD_ES1869;
            } else if (memcmp(id, "SEM3006", 7) == 0)
                type = CARD_ES1868;
            else if (memcmp(id, "YMH", 3) == 0)
                type = CARD_YAMAHA;
            else if (memcmp(id, "NMX22", 5) == 0)
                type = CARD_YAMAHA;
            else if (memcmp(id, "QHA08", 5) == 0)
                type = CARD_YAMAHA;
            else if (memcmp(id, "MPI0003", 7) == 0)
                type = CARD_YAMAHA;
            else if (memcmp(id, "RSS", 3) == 0) {
                type = CARD_ROCKWELL;
                if (memcmp(id + 3, "58", 2) == 0)
                    cmi_variant = 8;
            } else if (memcmp(id, "OPT", 3) == 0) {
                if (memcmp(id + 5, "25", 2) == 0)
                    type = CARD_OPTI925;
                else if (memcmp(id + 5, "31", 2) == 0)
                    type = CARD_OPTI93X;
                else if (memcmp(id + 5, "33", 2) == 0)
                    type = CARD_OPTI93X;
                else if (memcmp(id + 5, "24", 2) == 0)
                    type = CARD_OPTI924;
            } else if (memcmp(id, "CMI", 3) == 0)
                type = CARD_CMI;
            else if (memcmp(id, "ADS7141", 7) == 0)
                type = CARD_ADS7141;
            else if (memcmp(id, "ADS71", 5) == 0 || memcmp(id, "AXS71", 5) == 0 ||
                     memcmp(id, "TER11", 5) == 0 || memcmp(id, "TER14", 5) == 0 ||
                     memcmp(id, "AZT1022", 7) == 0 || memcmp(id, "LWC1061", 7) == 0 ||
                     memcmp(id, "MDK1605", 7) == 0 || memcmp(id, "SMM7180", 7) == 0)
                type = CARD_AD1816;
            else if (memcmp(id, "AZT", 3) == 0) {
                if (memcmp(id + 3, "232", 3) == 0 || memcmp(id + 3, "1006", 4) == 0 ||
                    memcmp(id + 3, "1008", 4) == 0 || memcmp(id + 3, "1023", 4) == 0 ||
                    memcmp(id + 3, "3011", 4) == 0 || memcmp(id + 3, "3000", 4) == 0 ||
                    memcmp(id + 3, "3001", 4) == 0 || memcmp(id + 3, "1009", 4) == 0 ||
                    memcmp(id + 3, "1007", 4) == 0 || memcmp(id + 3, "1005", 4) == 0 ||
                    memcmp(id + 3, "1015", 4) == 0 || memcmp(id + 3, "1016", 4) == 0 ||
                    memcmp(id + 3, "1017", 4) == 0 || memcmp(id + 3, "3002", 4) == 0 ||
                    memcmp(id + 3, "3003", 4) == 0 || memcmp(id + 3, "1026", 4) == 0 ||
                    memcmp(id + 3, "3005", 4) == 0 || memcmp(id + 3, "3013", 4) == 0)
                    type = CARD_AZTECH;
            } else if (memcmp(id, "CSC", 3) == 0 || memcmp(id, "CDC", 3) == 0) {
                if (memcmp(id + 5, "3", 1) == 0) {
                    type = CARD_CRYSTAL;
                    if (memcmp(id + 6, "7", 1) == 0)
                        cs_variant = 7;
                    else
                        cs_variant = 0;
                    if (memcmp(id + 6, "2", 1) == 0)
                        cs_variant = 1;
                    if (memcmp(id + 3, "A836", 4) == 0)
                        cs_variant = 4;
                } else if (memcmp(id + 4, "25", 2) == 0 || memcmp(id + 4, "025", 3) == 0)
                    type = CARD_CRYSTAL;
            } else if (memcmp(id, "TBS2004", 7) == 0)
                type = CARD_CRYSTAL;
            else if (memcmp(id, "RJA0", 4) == 0)
                type = CARD_RJA0;
            else if (memcmp(id, "ENS4", 4) == 0)
                type = CARD_ENSONIQ;
            else if (memcmp(id, "TBS20", 5) == 0)
                type = CARD_TBS20;
            else if (memcmp(id, "GRV", 3) == 0 || memcmp(id, "ADV55", 5) == 0 ||
                     memcmp(id, "STB01", 5) == 0)
                type = CARD_GUSPNP;
        }
        cardno++;
        if (card > 1) {
            printf("Card #%d [%s] ", cardno, id);
            if (list != 1)
                printf("Skipped.");
            else {
                char text[0x46] = {0};
                if (pnp_read_ansi_name(csn, text, type))
                    printf("%s", text);
            }
            if (opt_xxf == 2)
                printf(" (BAD CHECKSUM!)");
            type = 0;
            printf("\n");
            nlisted++;
        }
        if (type > 0) {
            nsup++;
            break;
        }
        if (card > 1)
            card--;
        else {
            printf("Unknown PnP card [%s] found. \nAborting...\n", id);
            pnp_write(3, 0);
            delay(1);
            pnp_write(2, 2);
            return 1;
        }
        pnp_write(3, 0);
        delay(1);
        found = pnp_isolate(pnp_rdata_port, buf);
    }
    if (nsup > 0) {
        char name[0x48] = {0};
        unsigned char lcsn = 0; /* CSN of the supported card */
        struct ldinfo ld = {{-1, -1, -1, -1, -1, -1, -1, -1, -1}};
        if (pnp_map_devices(csn, name, ld.ld, type))
            lcsn = csn;
        if (lcsn > 0) {
            printf("PnP card found: [%s] %s", id, name);
            if (opt_xxf == 2)
                printf(" (BAD CHECKSUM!)");
            puts("");
            if (opt_disable != 0 && !opt_read) {
                puts("Disabling card's logical devices...");
                sb_port = 0;
                opl_port = 0;
                awe_port = 0;
                mpu_port = 0;
                joy_port = 0;
                wss_port = 0;
                yamaha_ctl_port = 0;
                crystal_ctl_port = 0;
                opti_ctl_port = 0;
                ess_ctl_port = 0;
                mpu2_port = 0;
                ide_port = 0;
                sb_irq = 0;
                irq2 = 0;
                mpu_irq = 0;
                sb_dma = 4;
                sb_hdma = 4;
                wss_dma = 4;
                aux_ctl_port = 0;
                gus_port = 0;
                gus_dma = 4;
                gus_dma2 = 4;
                gus_irq = 0;
                gus_irq2 = 0;
            }
            if (opt_read != 0)
                puts("Reading card current settings...\n");
            else if (opt_disable == 0) {
                if (type == CARD_GUSPNP) {
                    if (parse_ultrasnd() == 0)
                        puts("Loading Unisound default settings...");
                    else
                        printf("%s environment var found! Loading settings...\n", "ULTRASND");
                } else {
                    if (parse_blaster() == 0)
                        puts("Loading Unisound default settings...");
                    else
                        printf("%s environment var found! Loading settings...\n", "BLASTER");
                }
            }
            if (ld.x[3] == 'h')
                type = CARD_ES968B;
            if (type == CARD_CREATIVE) {
                reg_mpu = 0x62;
                reg_opl = 0x64;
            }
            if (type == CARD_ALS) {
                reg_mpu = 0x60;
                reg_opl = 0x60;
            }
            if (type == CARD_ES1868) {
                reg_mpu = 0x64;
                reg_opl = 0x62;
            }
            if (type == CARD_YAMAHA) {
                reg_mpu = 0x66;
                reg_opl = 0x64;
                reg_wss = 0x62;
            }
            if (type == CARD_ROCKWELL) {
                reg_mpu = 0x60;
                reg_opl = 0x62;
                sb_hdma = 4;
            }
            if (type == CARD_ES968) {
                reg_mpu = 0x60;
                reg_opl = 0x60;
            }
            if (type == CARD_ES968B) {
                reg_mpu = 0x64;
                reg_opl = 0x62;
            }
            if (type == CARD_OPTI925) {
                reg_mpu = 0x60;
                reg_opl = 0x64;
                reg_wss = 0x62;
                if (nopnp_mode == 2)
                    opti_ctl_port += 0x80;
            }
            if (type == CARD_OPTI924) {
                reg_mpu = 0x60;
                reg_opl = 0x64;
                reg_wss = 0x62;
                if (nopnp_mode == 2)
                    opti_ctl_port += 0x80;
            }
            if (type == CARD_CMI) {
                reg_mpu = 0x60;
                reg_opl = 0x62;
                reg_wss = 0x60;
            }
            if (type == CARD_AD1816) {
                reg_mpu = 0x60;
                reg_opl = 0x62;
                reg_wss = 0x64;
            }
            if (type == CARD_ADS7141) {
                reg_mpu = 0x60;
                reg_opl = 0x60;
                reg_wss = 0x60;
                sb_hdma = 4;
            }
            if (type == CARD_AZTECH) {
                reg_mpu = 0x60;
                reg_opl = 0x62;
                reg_wss = 0x64;
                if (wss_port != 0)
                    wss_port += 4;
            }
            if (type == CARD_CRYSTAL) {
                reg_mpu = 0x60;
                reg_opl = 0x62;
                reg_wss = 0x60;
                if (wss_port != 0)
                    wss_port += 4;
                reg_sb = 0x64;
                irq2 = sb_irq;
            }
            if (type == CARD_OPTI93X) {
                reg_mpu = 0x60;
                reg_opl = 0x62;
                reg_wss = 0x60;
                if (wss_port != 0)
                    wss_port += 4;
                reg_sb = 0x64;
                if (wss_port != 0)
                    opti_ctl_port -= 0x100;
            }
            if (type == CARD_ES1869) {
                reg_mpu = 0x64;
                reg_opl = 0x62;
                if (opt_xea < 1 && (ess_ctl_port == 0x800 || ess_ctl_port == 0))
                    ess_ctl_port = 0x660;
            }
            if (type == CARD_GUSPNP) {
                reg_mpu = 0x60;
                reg_opl = 0x62;
                sb_port = gus_port;
                sb_dma = gus_dma;
                sb_hdma = gus_dma2;
                sb_irq = gus_irq;
            }
            if (type == CARD_ES1868 && name[0] == 'M' && name[1] == 'a' && name[2] == 'x')
                ess_maxi = 1;
            if (type == CARD_CRYSTAL && memcmp(name, "Orp", 3) == 0) {
                cs_variant = 2;
                if (opt_xof == -1)
                    opt_xof = 0;
            }
            if (type == CARD_CRYSTAL && memcmp(name, "Tur", 3) == 0 && cs_variant != 1)
                cs_variant = 3;
            if (type == CARD_ALS && als_variant == 3) {
                reg_dma = 0x75;
                reg_dma2 = 0x74;
            }
            if (type == CARD_CRYSTAL && (cs_variant == 1 || cs_variant == 4) && crystal_ctl_port == 0x120)
                crystal_ctl_port = wss_port + 4;
            if (type == CARD_CMI && memcmp(name, "AD-", 3) == 0)
                cmi_variant = 1;
            if (type == CARD_ROCKWELL && cmi_variant == 8 && memcmp(name, "Zol", 3) == 0)
                opt_xof = 8;
            if (((type == CARD_ES1869 && opt_xea == 0) || (type == CARD_ES1868 && opt_xea == 1)) && !opt_read)
                use_alt_pnp_ports();
            pnp_write(3, lcsn);
            delay(1);
            if (ld.ld[0] >= 0) {
                pnp_write(7, ld.ld[0]);
                pnp_write16(reg_sb, sb_port);
                if (opt_read != 0)
                    sb_port = pnp_read16(reg_sb);
                printf("ADD:%X", sb_port);
                if (type == CARD_GUSPNP) {
                    int iw_off = 0x100;
                    int codec_off = 0x10c;
                    if (opt_disable != 0) {
                        iw_off = 0;
                        codec_off = 0;
                    }
                    pnp_write16(0x62, sb_port + iw_off);
                    pnp_write16(0x64, sb_port + codec_off);
                    if (opt_read != 0)
                        sb_port = pnp_read16(0x64) - codec_off;
                    printf(" CODEC:%X", sb_port + codec_off);
                }
                if (type == CARD_YAMAHA || type == CARD_OPTI925 || type == CARD_AD1816 || type == CARD_AZTECH || type == CARD_OPTI93X ||
                    type == CARD_CRYSTAL || type == CARD_OPTI924) {
                    pnp_write16(reg_wss, wss_port);
                    if (opt_read != 0)
                        wss_port = pnp_read16(reg_wss);
                    printf(" WSS:%X", wss_port);
                }
                if (type == CARD_CREATIVE || type == CARD_ES1868 || type == CARD_YAMAHA || type == CARD_ROCKWELL || type == CARD_OPTI925 ||
                    type == CARD_OPTI924 || type == CARD_AD1816 || type == CARD_AZTECH || type == CARD_OPTI93X || type == CARD_CRYSTAL ||
                    type == CARD_ES968B || type == CARD_ES1869) {
                    pnp_write16(reg_opl, opl_port);
                    if (opt_read != 0) {
                        opl_port = pnp_read16(reg_opl);
                        if (type == CARD_ES968B && opl_port == 0 && (pnp_read(reg_dma) & 7) != 4)
                            opl_port = 0x388;
                    }
                    if (type == CARD_CREATIVE && ctl_state == 2)
                        printf(" CQM:%X", opl_port);
                    else
                        printf(" OPL:%X", opl_port);
                    if (type == CARD_CRYSTAL)
                        pnp_write(reg_irq2, 0);
                }
                if (type == CARD_CREATIVE || type == CARD_ES1868 || type == CARD_YAMAHA || type == CARD_ES968B || type == CARD_ES1869) {
                    if (type == CARD_ES1868 && ess_maxi == 1) {
                        pnp_write16(reg_mpu, mpu2_port);
                        if (opt_read != 0)
                            mpu2_port = pnp_read16(reg_mpu);
                        printf(" MPU2:%X", mpu2_port);
                    } else {
                        pnp_write16(reg_mpu, mpu_port);
                        if (opt_read != 0) {
                            mpu_port = pnp_read16(reg_mpu);
                            if (type == CARD_ES968B && mpu_port == 0 && (pnp_read(reg_dma) & 7) != 4)
                                mpu_port = 0x330;
                        }
                        printf(" MPU:%X", mpu_port);
                    }
                }
                if (type == CARD_YAMAHA) {
                    pnp_write16(0x68, yamaha_ctl_port);
                    if (opt_read != 0)
                        yamaha_ctl_port = pnp_read16(0x68);
                    printf(" CTR:%X", yamaha_ctl_port);
                }
                if (type == CARD_OPTI925 || type == CARD_OPTI93X) {
                    int ctl_off = 1;
                    if (opt_disable != 0)
                        ctl_off = 0;
                    pnp_write16(0x66, opti_ctl_port + ctl_off);
                    if (opt_read != 0)
                        opti_ctl_port = pnp_read16(0x66) - ctl_off;
                    printf(" CTR:%X", opti_ctl_port + ctl_off);
                }
                if (type == CARD_YAMAHA || type == CARD_GUSPNP)
                    irq2 = 0;
                else if (type != CARD_CMI)
                    irq2 = sb_irq;
                pnp_write(reg_irq, sb_irq);
                if (type != CARD_YAMAHA && type != CARD_OPTI925 && type != CARD_OPTI924 && type != CARD_GUSPNP && type != CARD_OPTI93X &&
                    type != CARD_CMI)
                    pnp_write(reg_irq + 1, 2);
                if (type != CARD_ES968 && type != CARD_OPTI925 && type != CARD_OPTI924 && type != CARD_AD1816 && type != CARD_ADS7141 &&
                    type != CARD_OPTI93X && type != CARD_CRYSTAL && type != CARD_ES968B && type != CARD_CMI)
                    pnp_write(reg_irq2, irq2);
                if (type != CARD_YAMAHA && type != CARD_ES968 && type != CARD_OPTI925 && type != CARD_OPTI924 && type != CARD_CMI &&
                    type != CARD_GUSPNP && type != CARD_AD1816 && type != CARD_OPTI93X && type != CARD_ES968B && type != CARD_ADS7141)
                    pnp_write(reg_irq2 + 1, 2);
                if (opt_read != 0)
                    sb_irq = pnp_read(reg_irq) & 0xf;
                if (opt_read != 0)
                    irq2 = pnp_read(reg_irq2) & 0xf;
                printf(" IRQ:%d", sb_irq);
                if ((type == CARD_YAMAHA && irq2 != 0) || (type == CARD_YAMAHA && opt_read != 0))
                    printf("/%d", irq2);
                if (sb_dma == sb_hdma && opt_disable == 0) {
                    if (type == CARD_YAMAHA || type == CARD_AZTECH) {
                        sb_dma = 1;
                        sb_hdma = 3;
                    }
                    if (type == CARD_CMI)
                        sb_hdma = 5;
                    if (type == CARD_ALS && als_variant == 3)
                        sb_hdma = 5;
                    if (type == CARD_ES1868 || type == CARD_ES1869)
                        sb_hdma = 0;
                }
                if (type == CARD_CRYSTAL && sb_hdma > 3)
                    sb_hdma = 4;
                if (type == CARD_OPTI925 || type == CARD_OPTI93X || type == CARD_OPTI924) {
                    if (sb_dma == sb_hdma || sb_hdma > 4)
                        sb_hdma = 4;
                    pnp_write(reg_dma2, sb_hdma);
                    if (opt_read != 0)
                        sb_hdma = pnp_read(reg_dma2) & 7;
                }
                pnp_write(reg_dma, sb_dma);
                if (opt_read != 0)
                    sb_dma = pnp_read(reg_dma) & 7;
                printf(" DMA:%d", sb_dma);
                if (type != CARD_ES968 && type != CARD_ES968B && als_variant != 1 && als_variant != 2 &&
                    (sb_hdma != 4 || (opt_disable != 0 && sb_hdma == 4) || (type == CARD_CMI && sb_hdma == 4))) {
                    pnp_write(reg_dma2, sb_hdma);
                    if (opt_read != 0)
                        sb_hdma = pnp_read(reg_dma2) & 7;
                    printf("/%d", sb_hdma);
                }
                pnp_write(0x30, opt_disable != 0 ? 0 : 1);
            }
            if (ld.ld[1] >= 0) {
                pnp_write(7, ld.ld[1]);
                if (type == CARD_ALS || type == CARD_ES968 || (type == CARD_CMI && cmi_variant < 1)) {
                    pnp_write16(reg_opl, opl_port);
                    if (opt_read != 0)
                        opl_port = pnp_read16(reg_opl);
                    printf(" OPL:%X", opl_port);
                }
                if (type == CARD_CMI || type == CARD_ADS7141) {
                    unsigned char irq = irq2;
                    unsigned char dma = wss_dma;
                    if (opt_disable != 0) {
                        irq = 0;
                        dma = 4;
                    }
                    pnp_write16(reg_wss, wss_port);
                    pnp_write(reg_irq, irq);
                    pnp_write(reg_irq + 1, 2);
                    pnp_write(reg_dma, dma);
                    if (type == CARD_ADS7141) {
                        pnp_write(reg_dma2, dma);
                        if (opt_read != 0)
                            dma = pnp_read(reg_dma2) & 7;
                    }
                    if (opt_read != 0)
                        wss_port = pnp_read16(reg_wss);
                    if (opt_read != 0)
                        irq = pnp_read(reg_irq) & 0xf;
                    if (opt_read != 0)
                        dma = pnp_read(reg_dma) & 7;
                    if (opt_read != 0 && type == CARD_ADS7141)
                        dma = pnp_read(reg_dma2) & 7;
                    printf(" WSS:%X/I%d/D%d", wss_port, irq, dma);
                }
                if (type == CARD_CRYSTAL && cs_variant == 4) {
                    pnp_write16(reg_mpu, mpu2_port);
                    if (opt_read != 0)
                        mpu2_port = pnp_read16(reg_mpu);
                    printf(" MPU2:%X", mpu2_port);
                }
                if (opt_disable != 0)
                    pnp_write(0x30, 0);
                if (type == CARD_GUSPNP) {
                    pnp_write16(0x60, 0);
                    pnp_write(reg_irq, 0);
                    pnp_write(0x30, 0);
                    if (opt_read != 0) {
                        int rd_port = pnp_read16(0x60);
                        unsigned char rd_irq = pnp_read(reg_irq) & 0xf;
                        printf(" SB:%X/I%d", rd_port, rd_irq);
                    }
                } else
                    pnp_write(0x30, opt_disable != 0 ? 0 : 1);
            }
            if (ld.ld[2] >= 0) {
                unsigned char enable = 1;
                unsigned char irq = sb_irq;
                irq = mpu_irq < 0 ? sb_irq : mpu_irq;
                pnp_write(7, ld.ld[2]);
                if (type == CARD_ALS || type == CARD_ROCKWELL || type == CARD_ES968 || type == CARD_OPTI925 || type == CARD_OPTI924 ||
                    type == CARD_CMI || (type == CARD_ES1868 && ess_maxi == 1) || type == CARD_AD1816 || type == CARD_ADS7141 ||
                    type == CARD_AZTECH || type == CARD_OPTI93X || type == CARD_CRYSTAL || type == CARD_RJA0 || type == CARD_TBS20) {
                    if (type == CARD_CRYSTAL && opt_xom == 0)
                        mpu_port = 0;
                    pnp_write16(0x60, mpu_port);
                    if (mpu_irq < 0) {
                        if (type == CARD_ALS || (type == CARD_ES1868 && ess_maxi == 1)) {
                            irq = 2;
                            if (als_variant > 1)
                                irq = 9;
                        }
                        if (type == CARD_OPTI925 || type == CARD_CMI || type == CARD_AD1816 || type == CARD_ADS7141 || type == CARD_AZTECH ||
                            type == CARD_OPTI93X || type == CARD_OPTI924 || type == CARD_CRYSTAL || type == CARD_RJA0 ||
                            type == CARD_TBS20) {
                            irq = 9;
                            enable = 1;
                        }
                        if (type == CARD_ES968)
                            irq = 7;
                    } else
                        irq = mpu_irq;
                }
                if (mpu_port == 0) {
                    irq = 0;
                    enable = 0;
                }
                if (type == CARD_GUSPNP) {
                    pnp_write16(0x60, 0);
                    pnp_write(0x70, 0);
                    enable = 0;
                } else {
                    pnp_write(0x70, irq);
                    pnp_write(0x71, 2);
                }
                if (opt_read != 0)
                    mpu_port = pnp_read16(0x60);
                if (opt_read != 0)
                    irq = pnp_read(0x70) & 0xf;
                pnp_write(0x30, (!enable || opt_disable != 0) ? 0 : 1);
                if (enable != 0 || opt_read != 0)
                    printf(" MPU:%X/I%d", mpu_port, irq);
            }
            if (ld.ld[6] >= 0) {
                pnp_write(7, ld.ld[6]);
                if (type == CARD_ES1868 || type == CARD_ES1869) {
                    if ((type != CARD_ES1869 && opt_xea == 0) || (type == CARD_ES1869 && opt_xea == 1))
                        pnp_write16(0x60, ess_ctl_port);
                    if (opt_read != 0)
                        ess_ctl_port = pnp_read16(0x60);
                    printf(" CTR:%X", ess_ctl_port);
                }
                if (type == CARD_CRYSTAL || (type == CARD_CMI && cmi_variant == 1)) {
                    pnp_write16(0x60, crystal_ctl_port);
                    if (opt_read != 0)
                        crystal_ctl_port = pnp_read16(0x60);
                    printf(" CTR:%X", crystal_ctl_port);
                }
                if (type == CARD_ROCKWELL) {
                    pnp_write16(0x60, aux_ctl_port);
                    if (opt_read != 0)
                        aux_ctl_port = pnp_read16(0x60);
                    printf(" CTR:%X", aux_ctl_port);
                }
                if (type == CARD_GUSPNP) {
                    int ctl = 0x350;
                    if (aux_ctl_port >= 0x300)
                        ctl = aux_ctl_port;
                    else
                        aux_ctl_port = ctl;
                    pnp_write16(0x60, ctl);
                    if (opt_read != 0)
                        ctl = pnp_read16(0x60);
                    printf(" CTR:%X", ctl);
                }
                if (type == CARD_ENSONIQ) {
                    int ctl = 0x2180;
                    if (aux_ctl_port >= 0x2100)
                        ctl = aux_ctl_port;
                    pnp_write16(0x62, wss_port);
                    pnp_write(0x70, sb_irq);
                    if (opt_read != 0)
                        wss_port = pnp_read16(0x62);
                    if (opt_read != 0)
                        sb_irq = pnp_read(0x70) & 0xf;
                    printf(" WSS:%X/I%d", wss_port, sb_irq);
                    if (mpu_irq < 0)
                        mpu_irq = sb_irq;
                    if (opt_read != 0)
                        mpu_port = pnp_read16(0x60);
                    if (opt_read != 0)
                        mpu_irq = pnp_read(0x72) & 0xf;
                    printf(" MPU:%X/I%d", mpu_port, mpu_irq);
                    pnp_write(0x74, sb_dma);
                    pnp_write(0x75, sb_hdma);
                    if (opt_read != 0)
                        sb_dma = pnp_read(0x74) & 7;
                    if (opt_read != 0)
                        sb_hdma = pnp_read(0x75) & 7;
                    printf(" DMA:%d/%d", sb_dma, sb_hdma);
                    pnp_write16(0x64, ctl);
                    if (opt_read != 0)
                        ctl = pnp_read16(0x64);
                    printf(" CTR:%X", ctl);
                }
                if (type == CARD_OPTI924) {
                    int ctl_off = 1;
                    if (opt_disable != 0)
                        ctl_off = 0;
                    pnp_write16(0x60, opti_ctl_port + ctl_off);
                    if (opt_read != 0)
                        opti_ctl_port = pnp_read16(0x60) - ctl_off;
                    printf(" CTR:%X", opti_ctl_port + ctl_off);
                    pnp_write(0x70, 0);
                }
                pnp_write(0x30, (opt_disable != 0 || type == CARD_OPTI924) ? 0 : 1);
            }
            if (ld.ld[3] >= 0) {
                if (joy_port == 0x299) {
                    if (type == CARD_YAMAHA || type == CARD_ROCKWELL || type == CARD_OPTI925 || type == CARD_OPTI924 || type == CARD_ES968 ||
                        type == CARD_ES968B || type == CARD_ES1869 || type == CARD_AD1816 || type == CARD_ADS7141 || type == CARD_ES1868)
                        joy_port = 0x201;
                    else
                        joy_port = 0x200;
                }
                if (type == CARD_GUSPNP) {
                    if (opt_xgj == 1)
                        joy_port = 0x201;
                    else
                        joy_port = 0;
                }
                pnp_write(7, ld.ld[3]);
                pnp_write16(0x60, joy_port);
                pnp_write(0x30, (joy_port == 0 || opt_disable != 0) ? 0 : 1);
                if (opt_read != 0 || joy_port > 0) {
                    if (opt_read != 0)
                        joy_port = pnp_read16(0x60);
                    printf(" JOY:%X", joy_port);
                }
            }
            if (ld.ld[4] >= 0) {
                pnp_write(7, ld.ld[4]);
                if (type == CARD_CREATIVE) {
                    pnp_write16(0x60, awe_port);
                    if (opt_read != 0)
                        awe_port = pnp_read16(0x60);
                    printf(" AWE:%X", awe_port);
                    pnp_write16(0x62, awe_port + 0x400);
                    pnp_write16(0x64, awe_port + 0x800);
                    if (ctl_state == 2 && opl_port != 0x388 && opl_detect() == 1)
                        printf(" OPL3 detected:388");
                    if (ld.ld[0] >= 0)
                        ctl_state = 1;
                    else
                        ctl_state = 3;
                }
                pnp_write(0x30, opt_disable != 0 ? 0 : 1);
            }
            if (ld.ld[5] >= 0) {
                unsigned char irq = 0;
                unsigned char reg_ide = 0x60;
                unsigned char reg_ide2 = 0x62;
                if (type == CARD_OPTI925) {
                    reg_ide = 0x62;
                    reg_ide2 = 0x63;
                }
                if (type == CARD_OPTI924) {
                    reg_ide = 0x62;
                    reg_ide2 = 0x60;
                }
                if (type == CARD_GUSPNP && opt_xgc > 0)
                    ide_port = opt_xgc;
                if (ide_port == 0x168)
                    irq = 0xa;
                else if (ide_port == 0x1e8)
                    irq = 0xb;
                else if (ide_port == 0x170)
                    irq = 0xf;
                else if (ide_port == 0)
                    irq = 0;
                if (ide_irq > 0 && ide_port > 0)
                    irq = ide_irq;
                pnp_write(7, ld.ld[5]);
                pnp_write16(reg_ide, ide_port);
                pnp_write(0x70, irq);
                if (type == CARD_CREATIVE || type == CARD_GUSPNP || type == CARD_ALS || type == CARD_OPTI925 || type == CARD_OPTI93X ||
                    type == CARD_YAMAHA || type == CARD_AZTECH || type == CARD_ES1869 || type == CARD_CRYSTAL || type == CARD_OPTI924) {
                    pnp_write16(reg_ide2, ide_port + (irq > 0) * 0x206);
                    pnp_write(0x71, 2);
                }
                if (type == CARD_GUSPNP)
                    pnp_write(0x74, 0);
                pnp_write(0x30, (ide_port == 0 || opt_disable != 0) ? 0 : 1);
                if (opt_read != 0 || ide_port > 0) {
                    if (opt_read != 0)
                        ide_port = pnp_read16(0x60);
                    printf(" IDE:%X", ide_port);
                    if (opt_read != 0)
                        irq = pnp_read(0x70) & 0xf;
                    printf("/I%d", irq);
                }
            }
            if (ld.ld[7] >= 0) {
                pnp_write(7, ld.ld[7]);
                pnp_write(0x30, (type == CARD_YAMAHA || type == CARD_AZTECH || opt_disable != 0) ? 0 : 1);
                if (type == CARD_YAMAHA || type == CARD_AZTECH) {
                    pnp_write16(0x60, 0);
                    pnp_write(0x70, 0);
                }
            }
            if (ld.ld[8] >= 0) {
                pnp_write(7, ld.ld[8]);
                if (type == CARD_CRYSTAL && cs_variant == 4) {
                    int mpu2_cfg = 0x328;
                    if (mpu2_port > 0)
                        mpu2_cfg = mpu2_port + 8;
                    pnp_write16(0x60, mpu2_cfg);
                    if (opt_read != 0)
                        mpu_port = pnp_read16(0x60);
                }
                if (type == CARD_ADS7141) {
                    pnp_write16(reg_opl, opl_port);
                    if (opt_read != 0)
                        opl_port = pnp_read16(reg_opl);
                    printf(" OPL:%X", opl_port);
                }
                /* no card family is 0x63, so that test is always false */
                pnp_write(0x30, (type == 0x63 || type == CARD_AZTECH || opt_disable != 0) ? 0 : 1);
            }
            if (type == CARD_OPTI925 || type == CARD_OPTI924) {
                delay(10);
                pnp_write(7, 5);
                pnp_write(0x70, 0);
                pnp_write(0x30, 1);
            }
            printf("\n");
            if (type == CARD_ADS7141) {
                if (opt_xwss != 0) {
                    pnp_write(0x20, 8);
                    delay(0x78);
                    outportb(wss_port, 4);
                    _AL = (100 - fm_vol) * 0x1f / 0x61;
                    outportb(wss_port + 2, _AL);
                    outportb(wss_port, 5);
                    _AL = (100 - fm_vol) * 0x1f / 0x61;
                    outportb(wss_port + 2, _AL);
                } else {
                    pnp_write(0x20, 0);
                    delay(0x50);
                }
            }
            pnp_write(3, 0);
            delay(1);
            if (opt_read != 0) {
                pnp_write(2, 2);
                return 0;
            }
            if ((ld.ld[4] < 0 || type != CARD_CREATIVE) && opt_disable == 0) {
                printf("Initialization done. ");
                if (type != CARD_CREATIVE)
                    puts("");
            }
            if (opt_disable != 0)
                puts("Card disabled.");
        } else {
            type = 0;
            printf("No supported PnP card detected.");
            if (nlisted < 1)
                ret = !init_nonpnp_sb16();
        }
    } else {
        type = 0;
        if (list != 1)
            printf("No PnP card detected.");
        if (nlisted < 1 && list != 1)
            ret = !init_nonpnp_sb16();
    }
    pnp_write(2, 2);
    delay(1);
    pnp_write(2, 2);
    delay(1);
    if (opt_disable != 0)
        return 0;
    if (type == CARD_YAMAHA) {
        int mix_port = wss_port + 4;
        if (treble_lvl < 0)
            treble_lvl = 0;
        if (bass_lvl < 0)
            bass_lvl = 0;
        idx01_write(yamaha_ctl_port, 0xa, 0x80);
        idx01_write(yamaha_ctl_port, 1, 0);
        idx01_write(yamaha_ctl_port, 0x10, 0);
        idx01_write(yamaha_ctl_port, 0x12, 0);
        idx01_write(yamaha_ctl_port, 0x13, 0);
        idx01_write(yamaha_ctl_port, 0x17, 0);
        idx01_write(yamaha_ctl_port, 3, 3);
        idx01_write(yamaha_ctl_port, 6, 0x34);
        idx01_write(yamaha_ctl_port, 0x14, (opt_x3d | opt_x3d << 4) & 0x77);
        idx01_write(yamaha_ctl_port, 0x15, bass_lvl * 7 / 0x61 | (bass_lvl * 7 / 0x61) << 4);
        idx01_write(yamaha_ctl_port, 0x16, treble_lvl * 7 / 0x61 | (treble_lvl * 7 / 0x61) << 4);
        outportb(sb_port + 6, 1);
        delay(4);
        outportb(sb_port + 6, 0);
        inportb(sb_port + 0xe);
        delay(1);
        inportb(sb_port + 0xe);
        inportb(sb_port + 0xa);
        delay(1);
        inportb(sb_port + 0xa);
        idx45_write(sb_port, 0x22, master_reg);
        idx45_write(sb_port, 0xe, 2);
        idx01_write(yamaha_ctl_port, 2, 0x30);
        idx45_write(sb_port, 0, 0xee);
        idx45_write(sb_port, 0x22, 0x11);
        sbpro_mixer_set();
        if (master_reg > 0)
            master_reg = (100 - master_vol) * 0xf / 0x61;
        else
            master_reg = 0x80;
        if (fm_reg > 0)
            fm_reg = (100 - fm_vol) * 0x1f / 0x61;
        else
            fm_reg = 0x88;
        if (wave_reg > 0)
            wave_reg = (100 - wave_vol) * 0x3f / 0x61;
        else
            wave_reg = 0x80;
        if (line_reg > 0)
            line_reg = (100 - line_vol) * 0x1f / 0x61;
        else
            line_reg = 0x88;
        if (cd_reg > 0)
            cd_reg = (100 - cd_vol) * 0x1f / 0x61;
        else
            cd_reg = 0x88;
        if (mic_reg > 0)
            mic_reg = (100 - mic_vol) * 0x1f / 0x61;
        else
            mic_reg = 0x88;
        idx01_write(mix_port, 0xc, 0x40);
        idx01_write(mix_port, 4, fm_reg);
        idx01_write(mix_port, 5, fm_reg);
        idx01_write(mix_port, 6, wave_reg);
        idx01_write(mix_port, 7, wave_reg);
        idx01_write(mix_port, 0x12, line_reg);
        idx01_write(mix_port, 0x13, line_reg);
        idx01_write(mix_port, 2, cd_reg);
        idx01_write(mix_port, 3, cd_reg);
        idx01_write(yamaha_ctl_port, 9, mic_reg);
        idx01_write(yamaha_ctl_port, 7, master_reg);
        idx01_write(yamaha_ctl_port, 8, master_reg);
        print_mixer("Yamaha", opt_x3d & 7, -1);
    }
    if (type == CARD_CREATIVE) {
        if (ctl_state > 0 && awe_port > 0) {
            delay(0x28);
            outportb(sb_port + 4, 0xfe);
            outportb(sb_port + 4, 0xfd);
            delay(1);
            awe_init(awe_port);
            delay(1);
            outportb(sb_port + 0xc, 0xdd);
        }
        if (ctl_state < 3) {
            sb_print_dsp_ver(sb_port);
            sb16_mixer_set();
            print_mixer("SB", opt_x3d > 0 ? 1 : -1, opt_xxg);
        }
    }
    if (type == CARD_ALS) {
        int shown_3d = opt_x3d;
        if (als_variant == 1 || als_variant == 2) {
            sbpro_mixer_set();
            opt_x3d = -1;
        } else if (als_variant != 3) {
            int x3d_reg = opt_x3d > 0 ? 0xc4 : 0x84;
            sb16_mixer_set();
            idx45_write(sb_port, 0x50, x3d_reg);
            x3d_reg = opt_x3d > 0 ? 0 : 0xa;
            idx45_write(sb_port, 0x51, x3d_reg);
        } else {
            sb16_mixer_set();
            opt_x3d = -1;
        }
        if (opt_x3d > 0)
            shown_3d = 1;
        else
            shown_3d = opt_x3d;
        if (als_variant != 2)
            print_mixer("ALS", shown_3d, -1);
        else
            print_mixer("Diamond", -1, -1);
    }
    if (type == CARD_ES1868 && ess_maxi == 1)
        sam9407_upload(mpu_port);
    if (type == CARD_ES1868 || type == CARD_ES968 || type == CARD_ES968B || type == CARD_ES1869) {
        int shown_3d = -1;
        sbpro_mixer_set();
        idx45_write(sb_port, 0x3a, fm_reg);
        outportb(mpu_port + 1, 0xff);
        delay(5);
        outportb(mpu_port + 1, 0x3f);
        delay(5);
        outportb(mpu_port, 0xf0);
        delay(1);
        outportb(mpu_port, 0);
        delay(1);
        outportb(mpu_port, 0);
        delay(1);
        outportb(mpu_port, 0x7b);
        delay(1);
        outportb(mpu_port, 1);
        delay(1);
        outportb(mpu_port, 0xf7);
        delay(1);
        outportb(mpu_port, 0xf0);
        delay(1);
        outportb(mpu_port, 0);
        delay(1);
        outportb(mpu_port, 0);
        delay(1);
        outportb(mpu_port, 0x7b);
        delay(1);
        outportb(mpu_port, 4);
        delay(1);
        outportb(mpu_port, 0xf7);
        delay(1);
        idx45_write(sb_port, 0x48, 0x10);
        idx45_write(sb_port, 0x36, fm_reg);
        outportb(mpu_port + 1, 0xff);
        delay(5);
        if (bass_lvl > -1)
            idx45_write(sb_port, 0x3a, bass_lvl * 0xf / 0x5f * 0x11);
        if (type == CARD_ES1869) {
            int x3d_ctl = opt_x3d > 0 ? 0xc : 0;
            int x3d_level = opt_x3d * 0x3f / 9;
            idx45_write(sb_port, 0x52, x3d_level);
            idx45_write(sb_port, 0x50, x3d_ctl);
            shown_3d = opt_x3d;
        }
        print_mixer("ESS", shown_3d, -1);
    }
    if (type == CARD_ROCKWELL) {
        int fm_level = fm_vol * 0x1f / 0x61;
        sbpro_mixer_set();
        if (opt_xof == 8) {
            outpw(aux_ctl_port, 0xc00e);
            inportb(aux_ctl_port + 5);
            delay(200);
            outp(aux_ctl_port, 0x15);
            inportb(aux_ctl_port + 5);
            delay(200);
            idx45_write(sb_port, 0x28, fm_reg);
        }
        idx45_write(sb_port, 0xe, 0x22);
        fm_level = fm_level << 6;
        outpw(aux_ctl_port, 0x32);
        inportb(aux_ctl_port + 5);
        delay(300);
        outpw(aux_ctl_port, 0x282e | fm_level);
        inportb(aux_ctl_port + 5);
        delay(300);
        outpw(aux_ctl_port, 0x682e | fm_level);
        inportb(aux_ctl_port + 5);
        print_mixer("Rockwell", -1, -1);
    }
    if (type == CARD_CMI) {
        int x3d_bit = opt_x3d > 0 ? 0x20 : 0;
        int gain_bit = opt_xxg > 0 ? 0 : 0x40;
        int xxg9_bit = opt_xxg == 9 ? 0 : 0x40;
        int gain = opt_xxg;
        idx45_write(wss_port, 0xc, 0x4a);
        idx45_write(wss_port, 0x10, xxg9_bit | x3d_bit);
        idx45_write(wss_port, 0x11, 0x3f | gain_bit);
        idx45_write(wss_port, 0x17, 3);
        idx45_write(wss_port, 0x16, fm_reg);
        idx45_write(wss_port, 0x14, line_reg);
        idx45_write(wss_port, 0x18, 0xff);
        idx45_write(wss_port, 0xc, 0xa);
        sb16_set_irq_dma(sb_port, sb_irq, sb_dma, sb_hdma);
        sb16_mixer_set();
        if (cmi_variant == 1) {
            if (line_vol == 0)
                line_reg = 0;
            else if (line_vol > 0 && line_vol < 0x32)
                line_reg = 0x60;
            else if (line_vol > 0x31)
                line_reg = 0x66;
            if (mic_vol == 0)
                line_reg &= 0xee;
            else if (mic_vol > 0 && mic_vol < 0x32)
                line_reg |= 0x10;
            else if (mic_vol > 0x31)
                line_reg |= 0x11;
            idx45_write(wss_port, 4, (100 - fm_vol) * 0x1f / 0x61);
            idx45_write(wss_port, 5, (100 - fm_vol) * 0x1f / 0x61);
            idx45_write(wss_port, 6, (100 - wave_vol) * 0x1f / 0x61);
            idx45_write(wss_port, 7, (100 - wave_vol) * 0x1f / 0x61);
            outportb(crystal_ctl_port, line_reg);
        }
        print_mixer("CMI", opt_x3d > 0 ? 1 : 0, gain > 0 ? 1 : 0);
    }
    if (type == CARD_AD1816) {
        unsigned char fm_att = (100 - fm_vol) * 0x3f / 0x61;
        int x3d_word = opt_x3d * 0xf / 9 << 9;
        sbpro_mixer_set();
        outportb(wss_port, 0x27);
        outpw(wss_port + 2, x3d_word);
        outportb(wss_port, 0x2c);
        outpw(wss_port + 2, 0);
        if (fm_vol > 2) {
            outportb(wss_port, 0x10);
            outpw(wss_port + 2, fm_att << 8 | fm_att);
            outportb(wss_port, 0x23);
            outpw(wss_port + 2, 0xac44);
            outportb(wss_port, 0x24);
            outpw(wss_port + 2, 0xac44);
            outportb(wss_port, 0x20);
            outpw(wss_port + 2, 0xf5);
            outportb(wss_port, 6);
            outpw(wss_port + 2, fm_att << 8 | fm_att);
            outportb(wss_port, 7);
            outpw(wss_port + 2, fm_att << 8 | fm_att);
        } else {
            outportb(wss_port, 6);
            outpw(wss_port + 2, 0x8080);
            outportb(wss_port, 7);
            outpw(wss_port + 2, 0x8080);
            outportb(wss_port, 0x20);
            outpw(wss_port + 2, 0xf0);
            outportb(wss_port, 0x10);
            outpw(wss_port + 2, 0x8888);
        }
        print_mixer("ADI", opt_x3d, -1);
    }
    if (type == CARD_ADS7141) {
        if (opt_xwss == 0)
            sbpro_mixer_set();
        print_mixer("ADI", -1, -1);
    }
    if (type == CARD_AZTECH) {
        sbpro_mixer_set();
        idx45_write(sb_port, 0xe, 0x22);
        print_mixer("Aztech", -1, -1);
        printf("Card mode: ");
        if (opt_xwss > 0) {
            outportb(sb_port + 0xc, 9);
            delay(10);
            outportb(sb_port + 0xc, 0x80);
            puts("[WSS]");
        } else {
            outportb(sb_port + 0xc, 9);
            delay(10);
            outportb(sb_port + 0xc, 0x81);
            puts("[SBPro]");
        }
    }
    if (type == CARD_OPTI925 || type == CARD_OPTI924) {
        delay(0x14);
        opti92x_init(type == CARD_OPTI925 ? 5 : 4);
    }
    if (type == CARD_OPTI93X) {
        delay(0x14);
        opti93x_init();
        print_mixer("OPTi", opt_x3d > 0 ? 1 : -1, -1);
    }
    if (type == CARD_CRYSTAL) {
        int shown_3d = -1;
        int x3d_reg = opt_x3d * 0xf / 9 | ~(opt_x3d * 0xf / 9) << 4;
        if (cs_variant == 1)
            cs4232_upload_fw();
        if (cs_variant == 4)
            puts("EWS64 firmware not supported. Output will be silent.");
        outportb(sb_port + 6, 1);
        delay(1);
        outportb(sb_port + 6, 0);
        delay(0x14);
        sbpro_mixer_set();
        delay(10);
        idx45_write(sb_port, 0x22, 0);
        idx45_write(sb_port, 4, 0);
        if (cs_variant == 2 || cs_variant == 3 || cs_variant == 7) {
            crystal_ext_init(cs_variant);
            delay(10);
            if (opt_x3d < 1)
                x3d_reg = 0;
            cs_ctl_write(2, x3d_reg);
            if (cs_variant == 7)
                cs_ctl_write(3, opt_x3d > 0 ? 0x80 : 0);
            shown_3d = opt_x3d;
        }
        if (sb_dma == sb_hdma) {
            idx01_write(wss_port, 0x49, 4);
            idx01_write(wss_port, 0x4a, 6);
            delay(5);
        }
        delay(5);
        sbpro_mixer_set();
        if (cs_variant == 1 && mic_vol > 0) {
            idx01_write(wss_port, 0x40, (mic_reg & 0xf) + 0x80);
            idx01_write(wss_port, 0x41, (mic_reg & 0xf) + 0x80);
            idx01_write(wss_port, 0x46, 2);
            idx01_write(wss_port, 0x47, 2);
        }
        if (cs_variant == 2 || (cs_variant == 3 && opt_xos == 1)) {
            int i = 0;
            idx01_write(wss_port, 0xc, 0xea);
            if (fm_vol > 0)
                for (i = 0; i < 2; i++)
                    cs_xreg_write(i, 0x20 | (100 - fm_vol) * 0x1f / 0x61);
            else
                for (i = 0; i < 2; i++)
                    cs_xreg_write(i, 0xff);
        }
        if (cs_variant != 4)
            print_mixer("Crystal", shown_3d, -1);
    }
    if (type == CARD_ENSONIQ) {
        idx45_write(wss_port, 6, (100 - wave_vol) * 0x3f / 0x61);
        idx45_write(wss_port, 7, (100 - wave_vol) * 0x3f / 0x61);
        idx45_write(wss_port, 2, (100 - line_vol) * 0x1f / 0x61);
        idx45_write(wss_port, 3, (100 - line_vol) * 0x1f / 0x61);
        idx45_write(wss_port, 4, (100 - cd_vol) * 0x1f / 0x61);
        idx45_write(wss_port, 5, (100 - cd_vol) * 0x1f / 0x61);
        printf("Ensoniq WSS Mixer [PCM:%d LIN:%d CD:%d]", wave_vol, line_vol, cd_vol);
    }
    if (type == CARD_TBS20) {
        outportb(mpu_port + 1, 0xff);
        delay(5);
    }
    if (type == CARD_GUSPNP) {
        delay(10);
        master_reg = (100 - master_vol) * 0x1f / 0x61;
        fm_reg = (100 - fm_vol) * 0x1f / 0x61;
        wave_reg = (100 - wave_vol) * 0x3f / 0x61;
        line_reg = (100 - line_vol) * 0x1f / 0x61;
        cd_reg = (100 - cd_vol) * 0x1f / 0x61;
        mic_reg = (100 - mic_vol) * 0x1f / 0x61;
        iw_init();
        printf("InterWave Mixer [VOL:%d DAC:%d SYNTH:%d LIN:%d CD:%d MIC:%d]\n",
               master_vol, wave_vol, fm_vol, line_vol, cd_vol, mic_vol);
        if (aux_ctl_port >= 0x300) {
            outportb(aux_ctl_port, 0);
            if (inportb(aux_ctl_port) == 0xfd) {
                if (treble_lvl < 0)
                    treble_lvl = 0x32;
                if (bass_lvl < 0)
                    bass_lvl = 0x32;
                tea6330_write(5, 0xc0);
                tea6330_write(3, treble_lvl * 0xf / 0x60);
                tea6330_write(2, bass_lvl * 0xf / 0x60);
                tea6330_write(5, 0x40);
                tea6330_write(0, 0x35);
                tea6330_write(1, 0x35);
                tea6330_write(4, pcspk_vol);
                printf("TEA6330T Init done. [TREB:%d BASS:%d]", treble_lvl, bass_lvl);
            }
        }
    }
    printf("\n");
    if (opt_xnet > 0)
        xnet_workaround(pnp_rdata_port);
    return ret;
}

/* Send the 32-byte ISA PnP initiation key (preceded by two zero writes to reset the LFSR) to the
   address port, which moves all PnP cards from Wait for Key into the Sleep state. */
void pnp_send_key(void)
{
    unsigned char key[34] = {
        0x00, 0x00, 0x6a, 0xb5, 0xda, 0xed, 0xf6, 0xfb, 0x7d, 0xbe, 0xdf, 0x6f,
        0x37, 0x1b, 0x0d, 0x86, 0xc3, 0x61, 0xb0, 0x58, 0x2c, 0x16, 0x8b, 0x45,
        0xa2, 0xd1, 0xe8, 0x74, 0x3a, 0x9d, 0xce, 0xe7, 0x73, 0x39 };
    int i = 0;
    for (i = 0; i < 34; i++)
        outportb(pnp_addr_port, key[i]);
    delay(1);
}

/* Write val to PnP register reg (address port, then write-data port). With /R, writes to the
   logical-device configuration registers 0x30-0x80 are skipped, so the card is only read. */
void pnp_write(unsigned char reg, unsigned char val)
{
    if (opt_read == 0 || reg < 0x30 || reg > 0x80) {
        outportb(pnp_addr_port, reg);
        outportb(pnp_wdata_port, val);
    }
}

/* Read PnP register reg: select it on the address port and read the read-data port. */
unsigned char pnp_read(unsigned char reg)
{
    outportb(pnp_addr_port, reg);
    return inportb(pnp_rdata_port);
}

/* Write a 16-bit value to the PnP register pair reg/reg+1, high byte first, as used for I/O base
   addresses. Skipped under /R for the I/O range registers 0x60-0x80. */
void pnp_write16(unsigned char reg, unsigned val)
{
    if (opt_read == 0 || reg < 0x60 || reg > 0x80) {
        pnp_write(reg, (val >> 8) & 0xff);
        pnp_write(reg + 1, val & 0xff);
    }
}

/* Read a 16-bit value from the PnP register pair reg/reg+1, high byte first. */
unsigned pnp_read16(unsigned char reg)
{
    unsigned v = 0;
    v = (pnp_read(reg) << 8) & 0xff00;
    return v + pnp_read(reg + 1);
}

/* Indexed register write with the index at port+4 and the data at port+5. This is the SB mixer
   layout (base+4/+5), also used for WSS codecs placed 4 bytes above their base. */
void idx45_write(unsigned port, unsigned char idx, unsigned char val)
{
    outportb(port + 4, idx);
    outportb(port + 5, val);
}

/* Indexed register write with the index at port and the data at port+1, with a 1 ms delay after
   each access. Used for the Yamaha OPL3-SAx control port and Crystal/WSS codec registers. */
void idx01_write(unsigned port, unsigned char idx, unsigned char val)
{
    outportb(port, idx);
    delay(1);
    outportb(port + 1, val);
    delay(1);
}

/* Like idx45_write (index at port+4, data at port+5) but with long delays (17 ms and 16 ms) for
   codecs that need time between accesses, such as the OPTi and ESS ones. */
void idx45_write_slow(unsigned port, unsigned char idx, unsigned char val)
{
    outportb(port + 4, idx);
    delay(0x11);
    outportb(port + 5, val);
    delay(0x10);
}

/* Read register reg of an OPTi 82C924/925 control chip at port. Every access is unlocked by
   writing the password 0xE5 to port+3. Registers 0-7 are at port+reg; higher ones are reached
   indirectly through port+8 (index) and port+9 (data). */
unsigned char opti92x_read(unsigned port, unsigned char reg)
{
    unsigned char v;
    outportb(port + 3, 0xe5);
    if (reg > 7) {
        outportb(port + 8, reg);
        outportb(port + 3, 0xe5);
        v = inportb(port + 9);
    } else
        v = inportb(port + reg);
    return v;
}

/* Write val to register reg of an OPTi 82C924/925 control chip, with the same password unlock and
   direct/indirect addressing as opti92x_read, then wait 2 ms. */
void opti92x_write(unsigned port, unsigned char reg, unsigned char val)
{
    outportb(port + 3, 0xe5);
    if (reg > 7) {
        outportb(port + 8, reg);
        outportb(port + 3, 0xe5);
        outportb(port + 9, val);
    } else
        outportb(port + reg, val);
    delay(2);
}

/* Write val to index idx of an OPTi 82C93x register pair at port/port+1. Both the index and the
   data write are preceded by the password 0xE4 written to opti93x_pwd_port. */
void opti93x_write(unsigned port, unsigned char idx, unsigned char val)
{
    outportb(opti93x_pwd_port, 0xe4);
    outportb(port, idx);
    outportb(opti93x_pwd_port, 0xe4);
    outportb(port + 1, val);
    delay(2);
}

/* Read index idx of an OPTi 82C93x register pair at port/port+1, using the same 0xE4 password
   sequence as opti93x_write, then wait 5 ms. */
unsigned char opti93x_read(unsigned port, unsigned char idx)
{
    unsigned char v;
    outportb(opti93x_pwd_port, 0xe4);
    outportb(port, idx);
    outportb(opti93x_pwd_port, 0xe4);
    v = inportb(port + 1);
    delay(5);
    return v;
}

/* Run one ISA PnP isolation pass on read-data port `port` and store the winning card's 72-bit
   serial identifier in buf[9] (vendor id, serial number, checksum). Each bit is read as a pair of
   reads, 0x55 then 0xAA meaning 1. The LFSR checksum of the first 64 bits must match byte 8,
   unless /XXF is given, in which case opt_xxf records whether it matched (1) or not (2). Gives up
   early when nothing answered within the first isolate_limit bits.
   Returns 1 if a card was isolated, 0 otherwise (also for ports below 0x200 or a NULL buf). */
int pnp_isolate(unsigned port, unsigned char *buf)
{
    int i = 0;
    int bit = 0;
    int found = 0;
    int lfsr = 0x6a;
    if (port < 0x200 || !buf)
        return 0;
    outportb(pnp_addr_port, 1);
    delay(1);
    memset(buf, 0, 9);
    for (i = 0; i < 0x48; i++) {
        bit = inportb(port) == 0x55;
        delay(1);
        bit = inportb(port) == 0xaa && bit;
        delay(1);
        found = found || bit;
        if (!found && isolate_limit < i)
            break;
        if (i < 0x40)
            lfsr = (lfsr >> 1) | (((lfsr ^ (lfsr >> 1) ^ bit) << 7) & 0xff);
        buf[i / 8] = (buf[i / 8] >> 1) | (bit ? 0x80 : 0);
    }
    if (opt_xxf)
        found = found != 0;
    else
        found = found && buf[8] == lfsr;
    if (opt_xxf) {
        if (buf[8] != lfsr)
            opt_xxf = 2;
        else
            opt_xxf = 1;
    }
    return found != 0;
}

/* buffer for pnp_vendor_str */
char vendor_str[9] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };  /* vendor id string buffer */

/* Decode the compressed EISA vendor id at *id (three 5-bit letters and a 16-bit product number,
   byte-swapped) into a string like "CTL0044" in the static vendor_str buffer, and return it. */
char *pnp_vendor_str(unsigned long *id)
{
    register unsigned long *p = id;
    unsigned long v = *p;
    sprintf(vendor_str, "%c%c%c%X%X%X%X",
        (unsigned char)(((v >> 2) & 0x1f) + 0x40),
        (unsigned char)(((v & 3) << 1 << 1 << 1) + ((v >> 13) & 7) + 0x40),
        (unsigned char)(((v >> 8) & 0x1f) + 0x40),
        (unsigned char)((v >> 20) & 0xf),
        (unsigned char)((v >> 16) & 0xf),
        (unsigned char)((v >> 28) & 0xf),
        (unsigned char)((v >> 24) & 0xf));
    return vendor_str;
}

/* Read the ULTRASND environment variable ("port,dma,dma2,irq,irq2") into gus_port, gus_dma,
   gus_dma2, gus_irq and gus_irq2. Out-of-range fields are ignored.
   Returns 1 if the variable exists and has at least 3 fields, else 0. */
int parse_ultrasnd(void)
{
    char *env = 0;
    char *dup = 0;
    int dec = 0;
    int hex = 0;
    char *tok;
    unsigned char field;
    env = getenv("ULTRASND");
    if (env == 0)
        return 0;
    dup = strdup(env);
    tok = strtok(dup, ",");
    field = 0;
    while (field < 5 && tok != 0) {
        dec = strtol(tok, 0, 10);
        hex = strtol(tok, 0, 16);
        switch (field) {
        case 0:
            if (hex == 0x220 || hex == 0x230 || hex == 0x240 || hex == 0x250 || hex == 0x260) {
                gus_port = hex;
                break;
            }
            break;
        case 1:
            if (dec >= 0 && dec <= 7) {
                gus_dma = dec;
                break;
            }
            break;
        case 2:
            if (dec >= 0 && dec <= 7) {
                gus_dma2 = dec;
                break;
            }
            break;
        case 3:
            if (dec >= 0 && dec <= 15) {
                gus_irq = dec;
                break;
            }
            break;
        case 4:
            if (dec >= 0 && dec <= 15) {
                gus_irq2 = dec;
                break;
            }
            break;
        default:
            break;
        }
        tok = strtok(0, ",");
        field++;
    }
    free(dup);
    if (field < 3)
        return 0;
    return 1;
}

/* Read the BLASTER environment variable (space-separated letter/value fields) into the port, IRQ
   and DMA globals: A sb_port, I sb_irq, D sb_dma, H sb_hdma, P mpu_port, E awe_port, J joy_port,
   W wss_port, C ide_port, R the control ports, M mpu2_port, F opl_port, U mpu_irq, Q irq2,
   K wss_dma, Y ide_irq. Out-of-range values are ignored.
   Returns 1 if the variable exists, else 0. */
int parse_blaster(void)
{
    char *env = 0;
    char *dup = 0;
    int dec = 0;
    int hex = 0;
    char *tok;
    unsigned char field;
    char letter;
    env = getenv("BLASTER");
    if (env == 0)
        return 0;
    dup = strdup(env);
    tok = strtok(dup, " ");
    field = 0;
    while (field < 12 && tok != 0) {
        if (isalpha(*tok) && strlen(tok) > 1) {
            letter = *tok;
            tok++;
            dec = strtol(tok, 0, 10);
            hex = strtol(tok, 0, 16);
            switch (toupper(letter)) {
            case 'A':
                if (hex >= 0x220 && hex <= 0x280) {
                    sb_port = hex;
                    break;
                }
                break;
            case 'I':
                if (dec >= 0 && dec <= 15) {
                    sb_irq = dec;
                    break;
                }
                break;
            case 'D':
                if (hex >= 0 && hex <= 7) {
                    sb_dma = hex;
                    break;
                }
                break;
            case 'H':
                if (hex >= 0 && hex <= 7) {
                    sb_hdma = hex;
                    break;
                }
                break;
            case 'P':
                if ((hex >= 0x300 && hex <= 0x3f0) || hex == 0) {
                    mpu_port = hex;
                    break;
                }
                break;
            case 'E':
                if ((hex >= 0x620 && hex <= 0x680) || hex == 0) {
                    awe_port = hex;
                    break;
                }
                break;
            case 'J':
                if ((hex >= 0x200 && hex < 0x212) || hex == 0) {
                    joy_port = hex;
                    break;
                }
                break;
            case 'W':
                wss_port = hex;
                break;
            case 'C':
                if (hex == 0x168 || hex == 0x1e8 || hex == 0x170 || hex == 0) {
                    ide_port = hex;
                    break;
                }
                break;
            case 'R':
                yamaha_ctl_port = hex;
                ess_ctl_port = hex;
                aux_ctl_port = hex;
                break;
            case 'M':
                if ((hex >= 0x300 && hex <= 0x380) || hex == 0) {
                    mpu2_port = hex;
                    break;
                }
                break;
            case 'F':
                if ((hex >= 0x300 && hex <= 0x3ff) || hex == 0) {
                    opl_port = hex;
                    break;
                }
                break;
            case 'U':
                if (dec >= 0 && dec <= 15) {
                    mpu_irq = dec;
                    break;
                }
                break;
            case 'Q':
                if (dec >= 0 && dec <= 15) {
                    irq2 = dec;
                    break;
                }
                break;
            case 'K':
                if (hex >= 0 && hex <= 7) {
                    wss_dma = hex;
                    break;
                }
                break;
            case 'Y':
                if (dec >= 2 && dec <= 15) {
                    ide_irq = dec;
                    break;
                }
                break;
            default:
                break;
            }
        }
        tok = strtok(0, " ");
        field++;
    }
    free(dup);
    if (field < 1)
        return 0;
    return 1;
}

/* Wake card csn and read its resource data up to the first ANSI identifier string (tag 0x82),
   which is copied NUL-terminated into buf. Used to list cards with /CL. Crystal cards need the
   initiation key again first. The 9 serial-id bytes at the start of the resource data are skipped.
   Returns 1 on success, 0 if the resource data could not be read. */
int pnp_read_ansi_name(unsigned char csn, unsigned char *buf, int type)
{
    int done;
    int ok = 1;
    unsigned char tag;
    unsigned len;
    if (csn > 0 && buf) {
        if (type == CARD_CRYSTAL) {
            pnp_write(2, 2);
            delay(1);
            pnp_send_key();
        }
        pnp_write(3, csn);
        delay(1);
        if (pnp_read_res(0, 9)) {
            tag = 0;
            len = 0;
            done = 0;
            *buf = 0;
            while (!done) {
                if (pnp_read_tag(&tag, &len)) {
                    if (tag == 0x82) {
                        if (!pnp_read_str(buf, len))
                            ok = 0;
                        done = 1;
                    }
                    if (tag == 0xf)
                        done = 1;
                } else {
                    ok = 0;
                    done = 1;
                }
            }
        }
        pnp_write(3, 0);
    } else
        ok = 0;
    return ok;
}

/* Wake card csn and walk its whole resource data. The first ANSI string becomes the card's name.
   For each logical device (tag 2) it fills in ld[], the logical device number for each function
   (see struct ldinfo). The mapping depends on the card family `type`, its name and option flags,
   and later ANSI strings starting "Aud", "Gam", "Wav" or "IDE" override it. Also stores byte 3 of
   the first logical device id after ld[] (ldinfo.x[3]), which marks ES968 variants.
   Returns 1 on success, 0 if the resource data could not be read. */
int pnp_map_devices(unsigned char csn, char *name, int *ld, int type)
{
    int ok = csn > 0 && name != 0;
    unsigned char tag = 0;
    unsigned len = 0;
    int done = 0;
    int ldn = 0;
    int rev10 = 0;
    int ad_name = 0;
    int ews = 0;
    int maxi = 0;
    if (type == CARD_CRYSTAL) {
        pnp_write(2, 2);
        delay(1);
        pnp_send_key();
    }
    pnp_write(3, csn);
    delay(1);
    if (ok)
        ok = pnp_read_res(0, 9);
    if (ok)
        *name = 0;
    while (ok && !done) {
        unsigned char ldid[7] = { 0 };
        delay(1);
        ok = pnp_read_tag(&tag, &len);
        delay(1);
        if (!ok)
            break;
        switch (tag) {
        case 2:
            ldn++;
            ok = pnp_read_res(ldid, len);
            if (ldn == 1) {
                ((unsigned char *)ld)[0x15] = ldid[3];
                if (type == CARD_ES968 && ldid[3] == 0x68)
                    type = CARD_ES968B;
                if (type == CARD_ALS && name[3] == '1' && name[4] == '0')
                    rev10 = 3;
                if (type == CARD_CMI && memcmp(name, "AD-", 3) == 0)
                    ad_name = 1;
                if (type == CARD_CRYSTAL && memcmp(name + 12, "EW", 2) == 0)
                    ews = 4;
                if (type == CARD_ES1868 && memcmp(name, "Ma", 2) == 0)
                    maxi = 1;
            }
            if (type == CARD_ALS) {
                if (ldn == 1) ld[0] = 0;
                if (ldn == 2) ld[1] = 1;
                if (ldn == 3) ld[3] = 2;
                if (ldn == 4) ld[2] = 3;
                if (ldn == 5) ld[5] = 4;
            }
            if (type == CARD_ES1868) {
                if (ldn == 1) ld[6] = 0;
                if (ldn == 2) ld[0] = 1;
                if (ldn == 3) ld[3] = 2;
                if (maxi == 1) {
                    if (ldn == 4) ld[2] = 3;
                } else {
                    if (ldn == 4) ld[5] = 3;
                }
            }
            if (type == CARD_YAMAHA) {
                if (ldn == 1) ld[0] = 0;
                if (ldn == 2) ld[3] = 1;
                if (ldn == 3) ld[7] = 2;
                if (ldn == 4) ld[5] = 3;
            }
            if (type == CARD_ROCKWELL) {
                if (ldn == 1) ld[6] = 0;
                if (ldn == 2) ld[0] = 1;
                if (ldn == 3) ld[2] = 2;
                if (ldn == 6) ld[3] = 5;
            }
            if (type == CARD_ES968) {
                if (ldn == 1) ld[0] = 0;
                if (ldn == 2) ld[1] = 1;
                if (ldn == 3) ld[3] = 2;
                if (ldn == 4) ld[2] = 3;
                if (ldn == 5) ld[5] = 4;
            }
            if (type == CARD_OPTI925 || type == CARD_OPTI93X) {
                if (ldn == 2) ld[0] = 1;
                if (ldn == 3) ld[3] = 2;
                if (ldn == 4) ld[2] = 3;
                ld[5] = 0;
            }
            if (type == CARD_OPTI924) {
                if (ldn == 1) ld[0] = 0;
                if (ldn == 2) ld[3] = 1;
                if (ldn == 3) ld[2] = 2;
                if (ldn == 4) ld[5] = 3;
                if (ldn == 6) ld[6] = 5;
            }
            if (type == CARD_CMI) {
                if (ad_name == 0) {
                    if (ldn == 1) ld[1] = 0;
                    if (ldn == 2) ld[2] = 1;
                    if (ldn == 3) ld[3] = 2;
                    if (ldn == 4) ld[0] = 3;
                } else if (ad_name == 1) {
                    if (ldn == 1) ld[0] = 0;
                    if (ldn == 2) ld[2] = 1;
                    if (ldn == 3) ld[3] = 2;
                    if (ldn == 4) ld[1] = 3;
                    if (ldn == 5) ld[6] = 4;
                }
            }
            if (type == CARD_AD1816) {
                if (ldn == 1) ld[0] = 0;
                if (ldn == 2) ld[2] = 1;
                if (ldn == 3) ld[3] = 2;
            }
            if (type == CARD_ADS7141) {
                if (ldn == 1) ld[1] = 0;
                if (ldn == 2) ld[0] = 1;
                if (ldn == 3) ld[8] = 2;
                if (ldn == 4) ld[2] = 3;
                if (ldn == 5) ld[3] = 4;
                if (ldn == 6) ld[7] = 5;
            }
            if (type == CARD_GUSPNP) {
                if (ldn == 1) ld[0] = 0;
                if (ldn == 2) ld[5] = 1;
                if (ldn == 3) ld[3] = 2;
                if (ldn == 4) ld[1] = 3;
                if (ldn == 5) ld[2] = 4;
                if (ldn == 6 || opt_xgc == 0) ld[6] = 5;
            }
            if (type == CARD_AZTECH) {
                if (ldn == 1) ld[5] = 0;
                if (ldn == 2) ld[0] = 1;
                ld[2] = 2;
                ld[3] = 3;
                ld[7] = 4;
                ld[8] = 5;
            }
            if (type == CARD_CRYSTAL) {
                if (ldn == 1) ld[0] = 0;
                if (ldn == 2) ld[3] = 1;
                if (ldn == 3) ld[6] = 2;
                if (ldn == 4 || opt_xom == 1) ld[2] = 3;
                if (ews == 4) {
                    if (ldn == 5) ld[1] = 4;
                    if (ldn == 6) ld[8] = 5;
                } else {
                    if (ldn == 5) ld[5] = 4;
                }
            }
            if (type == CARD_ES968B) {
                if (ldn == 1) ld[0] = 0;
                if (ldn == 2) ld[3] = 1;
            }
            if (type == CARD_ES1869) {
                if (ldn == 1) ld[6] = 0;
                if (ldn == 2) ld[0] = 1;
                if (ldn == 3) ld[3] = 2;
                if (ldn == 4) ld[5] = 3;
            }
            if (type == CARD_RJA0) {
                if (ldn == 1) ld[2] = 0;
            }
            if (type == CARD_ENSONIQ) {
                if (ldn == 1) ld[6] = 0;
                if (ldn == 2) ld[3] = 1;
            }
            if (type == CARD_TBS20) {
                if (ldn == 2) ld[2] = 1;
                ld[3] = 2;
            }
            if (type == CARD_ALS && rev10 == 3 && ldn == 4 && ld[2] == 3) {
                done = 1;
                ld[5] = 4;
                break;
            }
            break;
        case 0xf:
            ok = pnp_read_res(0, len);
            done = 1;
            break;
        case 0x82:
            if (*name == 0) {
                ok = pnp_read_str(name, len);
            } else {
                char str[64] = { 0 };
                ok = pnp_read_str(str, len);
                if (ok) {
                    if (str[0] == 'A' && str[1] == 'u' && str[2] == 'd')
                        ld[0] = ldn - 1;
                    else if (str[0] == 'G' && str[1] == 'a' && str[2] == 'm')
                        ld[3] = ldn - 1;
                    else if (str[0] == 'W' && str[1] == 'a' && str[2] == 'v')
                        ld[4] = ldn - 1;
                    else if (str[0] == 'I' && str[1] == 'D' && str[2] == 'E')
                        ld[5] = ldn - 1;
                }
            }
            break;
        default:
            if (len > 0)
                ok = pnp_read_res(0, len);
        }
    }
    pnp_write(3, 0);
    delay(1);
    return ok;
}

/* Read n bytes of the selected card's resource data into buf (or discard them if buf is NULL).
   For each byte, polls the status register (5) for up to 15 ms until bit 0 says data is ready,
   then reads the resource data register (4).
   Returns 1 on success, 0 on a timeout. */
int pnp_read_res(unsigned char *buf, int n)
{
    int i = 0;
    unsigned char ready = 0;
    unsigned char data = 0;
    unsigned char tries = 15;
    while (i < n) {
        tries = 15;
        outportb(pnp_addr_port, 5);
        while (tries--) {
            ready = inportb(pnp_rdata_port) & 1;
            if (!ready)
                delay(1);
            else
                break;
        }
        outportb(pnp_addr_port, 4);
        if (ready != 0) {
            data = inportb(pnp_rdata_port);
            if (buf)
                buf[i] = data;
            i++;
        } else
            return 0;
    }
    return 1;
}

/* Read the next resource data tag. For a large item (bit 7 set) *tag is the whole byte and *len
   the following 16-bit length; for a small item *tag is the item name (bits 6-3) and *len the
   length (bits 2-0).
   Returns 1 on success, 0 on a read error or an invalid tag/length. */
int pnp_read_tag(unsigned char *tag, unsigned *len)
{
    int ok = 0;
    unsigned char hdr = 0;
    unsigned char lenb[2] = { 0, 0 };
    ok = pnp_read_res(&hdr, 1);
    if (ok) {
        if (hdr & 0x80) {
            *tag = hdr;
            ok = pnp_read_res(lenb, 2);
            if (ok)
                *len = (lenb[1] << 8) | lenb[0];
        } else {
            *tag = (hdr >> 1 >> 1 >> 1) & 0xf;
            *len = hdr & 7;
        }
        ok = (ok && *tag != 0xff && *len != 0xffff) ? 1 : 0;
    }
    return ok;
}

/* Read an ANSI identifier string of len bytes from the resource data into buf and NUL-terminate
   it. At most ANSI_MAX bytes are kept; the rest is read and discarded.
   Returns 1 on success, 0 on a read error. */
int pnp_read_str(char *buf, unsigned len)
{
    unsigned keep = len > ANSI_MAX ? ANSI_MAX : len;
    int ok = pnp_read_res(buf, keep) && pnp_read_res(0, len - keep);
    buf[ok ? keep : 0] = 0;
    return ok;
}

/* Check for an EMU8000 at port (the AWE base, SB base + 0x400) and initialize it. Prints a warning
   and returns if the chip's status registers don't respond. The chip version is guessed from SB
   mixer registers 0xFE/0xFD (both 0xFF means version 1). Then runs emu_init, which also sets
   reverb and chorus, and prints the detected sample RAM. */
void awe_init(unsigned port)
{
    unsigned char ver = 2;
    unsigned char mix_fe;
    unsigned char mix_fd;
    unsigned char status;
    char *msg = "AWE EMU8000 chip not found. WaveTable and/or FM may not be available\nInitialization done.\n";
    outportb(port + 0x802, 0x3d);
    outportb(port + 0x400, 0x59);
    outportb(port + 0x802, 0x3e);
    outportb(port + 0x400, 0x20);
    outportb(port + 0x802, 0x3f);
    outportb(port + 0x400, 0);
    outportb(port + 0x802, 0x3d);
    status = inportb(port + 0x400);
    if ((status & 0x7e) != 0x58) {
        printf(msg);
        return;
    }
    outportb(port + 0x802, 0x3e);
    status = inportb(port + 0x400);
    if ((status & 3) != 3) {
        printf(msg);
        return;
    }
    delay(1);
    outportb(port - 0x400 + 4, 0xfe);
    delay(1);
    mix_fe = inportb(port - 0x400 + 5);
    outportb(port - 0x400 + 4, 0xfd);
    delay(1);
    mix_fd = inportb(port - 0x400 + 5);
    if (mix_fe == 0xff && mix_fd == 0xff)
        ver = 1;
    printf("AWE [%Xh] EMU8000 v%d Synth chip found. ", port, ver);
    emu_write16(0x3f, port + 0x400, 0);
    printf("\nInitializing EMU8K... ");
    printf("AWE RAM detected: [%dK] ", emu_init(ver));
    if (awe_chorus > 0 || awe_reverb > 0)
        printf("\nEffects [Reverb:%d Chorus:%d] ", awe_reverb, awe_chorus);
    printf("\nInitialization done. ");
}

/* Ask the SB DSP at port for its version (command 0xE1) and print it if it looks like an SB16
   (major version 4); otherwise just print a newline. */
void sb_print_dsp_ver(int port)
{
    unsigned char maj = 0;
    unsigned char min = 0;
    unsigned char ok = 0;
    unsigned char i = 0;
    outportb(port + 0xc, 0xe1);
    delay(1);
    for (i = 0; i < 20; i++) {
        maj = inportb(port + 0xa);
        delay(1);
        if (maj > 0 && maj <= 4)
            break;
    }
    if (maj == 4) {
        min = inportb(port + 0xa);
        delay(1);
        if (min > 0 && min < 0x20)
            ok = 1;
    }
    if (ok > 0)
        printf("SB DSP: v%d.%02d \n", maj, min);
    else
        puts("");
}

/* Detect and configure a non-PnP (jumpered) SB16 or AWE32, used for /CS and as a fallback when
   no PnP card is found. Probes 0x220-0x280 for a DSP of version 4.xx that answers the 0xE3
   copyright command. Then:
   - applies the IRQ and DMA (from BLASTER, or the defaults) through the SB16 mixer's interrupt/DMA setup registers,
   - sets the MPU-401 address,
   - reports the settings and any MPU-401 or EMU8000 found (and initializes the EMU8000),
   - programs the mixer.
   Returns 1 if a card was found, else 0. */
int init_nonpnp_sb16(void)
{
    int port = 0x220;
    unsigned char irq = sb_irq;
    unsigned char dma = sb_dma;
    unsigned char hdma = sb_hdma;
    unsigned mpu = 0;
    int awe = awe_port;
    unsigned char i;
    unsigned char k;
    unsigned char maj = 0;
    unsigned char min = 0;
    unsigned char resp = 0;
    unsigned char irqr = 2;
    unsigned char dmar = 2;
    unsigned char mixreg = 0;
    char *name = "";
    unsigned char type = 0;
    int blaster = 0;
    for (i = 0; i < 4; i++) {
        outportb(port + 0xc, 0xe1);
        delay(1);
        for (k = 0; k < 20; k++) {
            maj = inportb(port + 0xa);
            delay(1);
            if (maj <= 4)
                break;
        }
        if (maj == 4) {
            min = inportb(port + 0xa);
            delay(1);
            if (min < 0x20) {
                outportb(port + 0xc, 0xe3);
                delay(1);
                resp = inportb(port + 0xa);
                delay(1);
                if (resp == 0x43) {
                    for (k = 0; k < 100; k++) {
                        resp = inportb(port + 0xa);
                        delay(1);
                        if (resp == 0)
                            break;
                    }
                    name = "SB16";
                    type = 1;
                    if (min <= 5)
                        type = 2;
                    break;
                }
            }
        }
        port += 0x20;
    }
    if (type > 0) {
        blaster = parse_blaster();
        if (blaster != 0) {
            irq = sb_irq;
            dma = sb_dma;
            hdma = sb_hdma;
            awe = awe_port;
        }
        printf("\n%s detected at: [%Xh] DSP: v%d.%02d\n", name, port, maj, min);
        sb16_set_irq_dma(port, irq, dma, hdma);
        outportb(port + 4, 0x84);
        delay(1);
        mixreg = inportb(port + 5);
        delay(1);
        if (mixreg > 0x7f && mixreg < 0xf0) {
            if (mpu_port == 0)
                outportb(port + 5, 0xe2);
            else if (mpu_port == 0x300)
                outportb(port + 5, 0xe4);
            else
                outportb(port + 5, 0xe0);
        }
        delay(20);
        irqr = sb16_get_irq(port);
        delay(20);
        dmar = sb16_get_dma(port);
        delay(1);
        if (blaster == 0 && type < 2)
            puts("Loading default values...");
        printf("Settings: ADD:%X IRQ:%X DMA:%d", port, irqr, dmar & 0xf);
        printf(" HDMA:%d", (dmar >> 4) > 0 ? dmar >> 4 : dmar & 0xf);
        if (inportb(0x331) == 0xbf || inportb(0x331) == 0x3f)
            mpu = 0x330;
        else if (inportb(0x301) == 0xbf || inportb(0x301) == 0x3f)
            mpu = 0x300;
        else
            mpu = 0;
        if (mpu > 0)
            printf(" MPU:%X", mpu);
        if (awe_detect(port)) {
            awe = port + 0x400;
            printf(" AWE:%X\n", awe);
            awe_init(awe);
            printf("\n");
        } else
            printf("\n");
        sb16_mixer_set();
        print_mixer("SB", -1, opt_xxg);
        return 1;
    }
    if (opt_cs == 1)
        puts("No SB16 or AWE non-PNP card detected.");
    return 0;
}

/* Program an SB16's IRQ select (mixer register 0x80) and DMA select (0x81) for the given IRQ
   (2/5/7/10, default 5), 8-bit DMA (0/1/3, default 1) and 16-bit DMA (5/6/7; 4 = none). */
void sb16_set_irq_dma(int port, unsigned char irq, unsigned char dma, unsigned char hdma)
{
    unsigned char irqv = 2;
    unsigned char dmav = 2;
    switch (irq) {
    case 2: irqv = 1; break;
    case 5: irqv = 2; break;
    case 7: irqv = 4; break;
    case 10: irqv = 8; break;
    default: irqv = 2;
    }
    outportb(port + 4, 0x80);
    delay(1);
    outportb(port + 5, irqv);
    delay(1);
    switch (dma) {
    case 0: dmav = 1; break;
    case 1: dmav = 2; break;
    case 3: dmav = 8; break;
    default: dmav = 2;
    }
    if (dma != hdma) {
        switch (hdma) {
        case 5: dmav += 0x20; break;
        case 6: dmav += 0x40; break;
        case 7: dmav += 0x80; break;
        case 4: dmav += 0; break;
        default: dmav += 0x20;
        }
    }
    outportb(port + 4, 0x81);
    delay(1);
    outportb(port + 5, dmav);
}

/* Read an SB16's IRQ select register (mixer 0x80) and return the IRQ number (2/5/7/10, 0 if
   unknown). */
unsigned char sb16_get_irq(int port)
{
    unsigned char v;
    outportb(port + 4, 0x80);
    delay(1);
    v = inportb(port + 5);
    switch (v & 0xf) {
    case 1: return 2;
    case 2: return 5;
    case 4: return 7;
    case 8: return 10;
    default: return 0;
    }
}

/* Read an SB16's DMA select register (mixer 0x81) and return high DMA << 4 | low DMA. */
unsigned char sb16_get_dma(int port)
{
    unsigned char lo;
    unsigned char hi;
    unsigned char res = 0;
    unsigned char v;
    outportb(port + 4, 0x81);
    delay(1);
    v = inportb(port + 5);
    lo = v & 0xb;
    hi = (v >> 4) & 0xe;
    switch (hi) {
    case 2: res = 5; break;
    case 4: res = 6; break;
    case 8: res = 7; break;
    default: res = 0;
    }
    res = res << 4;
    switch (lo) {
    case 1: res += 0; break;
    case 2: res += 1; break;
    case 8: res += 3; break;
    default: res += 0;
    }
    return res;
}

/* Return 1 if an EMU8000 answers at port + 0x400 (the same status register test as awe_init),
   else 0. */
unsigned char awe_detect(int port)
{
    unsigned char st;
    port += 0x400;
    outportb(port + 0x802, 0x3d);
    outportb(port + 0x400, 0x59);
    outportb(port + 0x802, 0x3e);
    outportb(port + 0x400, 0x20);
    outportb(port + 0x802, 0x3f);
    outportb(port + 0x400, 0);
    outportb(port + 0x802, 0x3d);
    delay(10);
    st = inportb(port + 0x400);
    if ((st & 0x7e) != 0x58)
        return 0;
    outportb(port + 0x802, 0x3e);
    delay(10);
    st = inportb(port + 0x400);
    if ((st & 3) != 3)
        return 0;
    return 1;
}

/* Print the "<name> Mixer [VOL:.. WAV:.. ...]" summary line with the requested volume levels.
   SB and Yamaha cards also show tone controls, ESS shows AuxB, and gain and 3D are shown when
   they are > -1. */
void print_mixer(char *name, int x3d, int gain)
{
    printf("%s Mixer [VOL:%d WAV:%d FM:%d LIN:%d CD:%d MIC:%d", name,
           master_vol, wave_vol, fm_vol, line_vol, cd_vol, mic_vol);
    if (memcmp(name, "SB", 2) == 0)
        printf(" PC:%d TRB:%d BAS:%d", pcspk_vol, treble_lvl, bass_lvl);
    if (memcmp(name, "Ya", 2) == 0)
        printf(" TREB:%d BASS:%d", treble_lvl, bass_lvl);
    if (memcmp(name, "ES", 2) == 0 && bass_lvl > -1)
        printf(" AUXB:%d", bass_lvl);
    if (gain > -1)
        printf(" GAIN:%d", gain);
    if (x3d > -1)
        printf(" 3D:%d", x3d);
    puts("]");
}

/* Write the SB Pro-compatible mixer registers at sb_port: master (0x22), voice (0x04),
   FM (0x26), line (0x2E), CD (0x28) and mic (0x0A, 3 bits). Used by most non-Creative cards. */
void sbpro_mixer_set(void)
{
    unsigned char mic;
    mic = mic_reg = mic_vol * 7 / 97;
    idx45_write(sb_port, 0x22, master_reg);
    idx45_write(sb_port, 4, wave_reg);
    idx45_write(sb_port, 0x26, fm_reg);
    idx45_write(sb_port, 0x2e, line_reg);
    idx45_write(sb_port, 0x28, cd_reg);
    idx45_write(sb_port, 0xa, mic);
}

/* Write the SB16 mixer registers at sb_port: left/right master, voice, FM, CD, line and mic
   volumes, input gain from /XXGn (1, 2 or 4; default 2), PC speaker, treble and bass, the output
   switches (sb16_mixer_switches) and register 0x90 (1 with /X3D, else 0). */
void sb16_mixer_set(void)
{
    int idx = sb_port + 4;
    int dat = sb_port + 5;
    int master = master_reg & 0xf0;
    int voice = fm_reg & 0xf0;
    int fm = wave_reg & 0xf0;
    int line = cd_reg & 0xf0;
    unsigned char mic = mic_vol * 255 / 97;
    int gain = 0x40;
    int agc = opt_x3d > 0 ? 1 : 0;
    switch (opt_xxg) {
    case 1: gain = 0; break;
    case 4: gain = 0x80; break;
    default: gain = 0x40; opt_xxg = 2;
    }
    if (treble_lvl < 0)
        treble_lvl = 0x32;
    if (bass_lvl < 0)
        bass_lvl = 0x32;
    sb16_mixer_switches();
    outportb(idx, 0x34); outportb(dat, voice);
    outportb(idx, 0x35); outportb(dat, voice);
    outportb(idx, 0x32); outportb(dat, fm);
    outportb(idx, 0x33); outportb(dat, fm);
    outportb(idx, 0x36); outportb(dat, line);
    outportb(idx, 0x37); outportb(dat, line);
    outportb(idx, 0x2e); outportb(dat, line_reg);
    outportb(idx, 0x38); outportb(dat, line_reg);
    outportb(idx, 0x39); outportb(dat, line_reg);
    outportb(idx, 0x0a); outportb(dat, mic_reg);
    outportb(idx, 0x3a); outportb(dat, mic);
    outportb(idx, 0x41); outportb(dat, gain);
    outportb(idx, 0x42); outportb(dat, gain);
    outportb(idx, 0x3b); outportb(dat, pcspk_reg);
    outportb(idx, 0x44); outportb(dat, treble_reg);
    outportb(idx, 0x45); outportb(dat, treble_reg);
    outportb(idx, 0x46); outportb(dat, bass_reg);
    outportb(idx, 0x47); outportb(dat, bass_reg);
    outportb(idx, 0x30); outportb(dat, master);
    outportb(idx, 0x31); outportb(dat, master);
    outportb(idx, 0x22); outportb(dat, master_reg);
    outportb(idx, 0x90); outportb(dat, agc);
}

/* Update the SB16 output mixer switches (register 0x3C): enable mic, CD and line outputs when
   their volume is non-zero, disable them otherwise. */
void sb16_mixer_switches(void)
{
    unsigned char v = 0;
    outportb(sb_port + 4, 0x3c);
    v = inportb(sb_port + 5);
    if (mic_reg > 0)
        v |= 1;
    else
        v &= 0xfe;
    if (cd_reg > 0)
        v |= 6;
    else
        v &= 0xf9;
    if (line_reg > 0)
        v |= 0x18;
    else
        v &= 0xe7;
    idx45_write(sb_port + 5, 0x3c, v);
}

/* Initialize an OPTi 82C925 (mode 5, CARD_OPTI925) or 82C924 (mode 4, CARD_OPTI924) through its
   control chip at opti_ctl_port. Then:
   - converts the volume settings to codec attenuation values,
   - resets the chip and programs the SB base, MPU, IDE and WSS/SB mode configuration registers,
   - initializes the WSS codec at wss_port and sets its mixer,
   - resets the SB DSP and prints the mixer and card mode. */
void opti92x_init(int mode)
{
    unsigned char cfg3 = 0xf0;
    unsigned char cfg6 = 0x87;
    int mpu_sel = 0;
    int cfg1 = 0;
    int cfg7_keep = 0;
    cd_reg = (100 - cd_vol) * 31 / 97;
    mic_reg = 0xc0 | ((100 - mic_vol) * 31 / 97) << 1;
    line_reg = (100 - line_vol) * 31 / 97;
    wave_reg = (100 - wave_vol) * 31 / 97;
    fm_reg = (100 - fm_vol) * 31 / 97;
    if (fm_vol > master_vol)
        fm_reg = (100 - master_vol) * 31 / 97;
    if (cd_vol < 1)
        cd_reg = 0x88;
    if (mic_vol < 1)
        mic_reg = 0x10;
    if (line_vol < 1)
        line_reg = 0x88;
    if (wave_vol < 1)
        wave_reg = 0x38;
    if (fm_reg < 9)
        fm_reg = 9;
    if (sb_port == 0x240)
        cfg3 = 0xf4;
    if (ide_port > 0)
        cfg1 |= 8;
    if (opt_xwss > 0)
        cfg1 |= 0x80;
    switch (mpu_port) {
    case 0x330: mpu_sel = 4; break;
    case 0x320: mpu_sel = 5; break;
    case 0x310: mpu_sel = 6; break;
    case 0x300: mpu_sel = 7; break;
    }
    cfg6 = 7 | mpu_sel << 5;
    cfg7_keep = opti92x_read(opti_ctl_port, 7) & 0x40;
    opti92x_write(opti_ctl_port, 1, 0x40);
    opti92x_write(opti_ctl_port, 1, 0);
    delay(120);
    if (mode == 5)
        opti92x_write(opti_ctl_port, 0xd, 2);
    outportb(wss_port + 6, 0);
    delay(250);
    opti92x_write(opti_ctl_port, 1, cfg1);
    opti92x_write(opti_ctl_port, 3, cfg3);
    opti92x_write(opti_ctl_port, 4, 0xa2);
    opti92x_write(opti_ctl_port, 6, cfg6);
    opti92x_write(opti_ctl_port, 7, wave_reg | cfg7_keep);
    opti92x_write(opti_ctl_port, 2, 0x23);
    opti92x_write(opti_ctl_port, 2, 3);
    opti92x_write(opti_ctl_port, 5, 0x9b);
    outportb(wss_port, 0);
    idx45_write_slow(wss_port, 0x48, 7);
    delay(50);
    idx45_write_slow(wss_port, 0x49, 8);
    delay(50);
    idx45_write_slow(wss_port, 0x49, 0);
    delay(50);
    idx45_write_slow(wss_port, 0x2a, 2);
    idx45_write_slow(wss_port, 0x2b, 0);
    delay(50);
    idx45_write_slow(wss_port, 0x20, 0xc0);
    idx45_write_slow(wss_port, 0x21, 0);
    opti92x_write(opti_ctl_port, 5, 0xab);
    opti92x_write(opti_ctl_port, 5, 0xbb);
    idx45_write_slow(wss_port, 0x48, 3);
    delay(50);
    idx45_write_slow(wss_port, 0x2a, 2);
    idx45_write_slow(wss_port, 0x2b, 0);
    idx45_write_slow(wss_port, 0x29, 0);
    opti92x_write(opti_ctl_port, 5, 0xab);
    opti92x_write(opti_ctl_port, 5, 0x9b);
    idx45_write_slow(wss_port, 0x24, fm_reg);
    idx45_write_slow(wss_port, 0x25, fm_reg);
    idx45_write_slow(wss_port, 0x22, cd_reg);
    idx45_write_slow(wss_port, 0x23, cd_reg);
    idx45_write_slow(wss_port, 0x2c, 0xca);
    if (mode == 5) {
        idx45_write_slow(wss_port, 0x10, 0x3f & mic_reg);
        idx45_write_slow(wss_port, 0x11, mic_reg);
    }
    idx45_write_slow(wss_port, 0x12, line_reg);
    idx45_write_slow(wss_port, 0x13, line_reg);
    idx45_write_slow(wss_port, 0x2c, 0x8a);
    opti92x_write(opti_ctl_port, 5, 0xab);
    idx45_write_slow(sb_port, 0x22, master_reg);
    outportb(sb_port + 6, 1);
    delay(5);
    outportb(sb_port + 6, 0);
    delay(5);
    outportb(sb_port + 0xc, 0xf2);
    delay(5);
    delay(50);
    opti92x_write(opti_ctl_port, 5, mode == 5 ? 0xa5 : 0xaf);
    print_mixer("OPTi", -1, -1);
    printf("Card mode: ");
    puts(opt_xwss > 0 ? "[WSS]" : "[SBPro]");
}

/* Initialize an OPTi 82C931/82C933 (CARD_OPTI93X). Programs the control registers through the
   password-protected port at opti_ctl_port + 2, then the WSS codec at wss_port - 4: output
   levels, 3D bits from /X3D, mode and calibration. Finishes with the SB Pro mixer. */
void opti93x_init(void)
{
    register unsigned codec = wss_port + -4;
    register unsigned mix = opti_ctl_port + 2;
    unsigned char x3d_bits = opt_x3d > 0 ? 0x60 : 0;

    opti93x_write(mix, 0x14, 0);
    opti93x_write(mix, 4, 0x16);
    opti93x_write(mix, 5, 0x7f);
    opti93x_write(mix, 6, 0x83);
    opti93x_write(mix, 10, 1);
    opti93x_write(mix, 0x13, 0);
    opti93x_write(mix, 0x14, 0);
    opti93x_write(mix, 0x12, 0x89);
    opti93x_write(mix, 0x15, 0x80);
    opti93x_write(mix, 0x1a, 1);
    delay(20);
    outportb(codec, 0);
    delay(32);
    idx45_write_slow(codec, 0x29, 0);
    idx45_write_slow(codec, 0x2a, 2);
    idx45_write_slow(codec, 0x2b, 0);
    idx45_write_slow(codec, 0x48, 3);
    delay(100);
    idx45_write_slow(codec, 0x30, (unsigned char)((100 - fm_vol) * 15 / 97) << 1);
    idx45_write_slow(codec, 0x31, (unsigned char)((100 - fm_vol) * 15 / 97) << 1 | x3d_bits);
    idx45_write_slow(codec, 0x20, 0xac);
    idx45_write_slow(codec, 0x21, 0xac);
    idx45_write_slow(codec, 0x36, 5);
    idx45_write_slow(codec, 0x37, 5);
    idx45_write_slow(codec, 0x26, 0x84);
    idx45_write_slow(codec, 0x27, 0x84);
    delay(80);
    sbpro_mixer_set();
}

/* SAM9407 boot loader and start sequence for sam9407_upload */
unsigned sam9407_boot[256] = {
    0xd0ce, 0x0111, 0xd0ce, 0x01d4, 0x0001, 0x0003, 0x0004, 0x0006, 0x0001, 0x0003, 0x0002, 0x0002,
    0x0006, 0x0002, 0x0001, 0x0006, 0x0006, 0x7a0c, 0xe628, 0x0001, 0xd448, 0x1010, 0xc4cb, 0xd1cb,
    0xe2fe, 0x4f01, 0xe3fc, 0x4e0d, 0xe0fa, 0x4700, 0x8407, 0xd148, 0x0104, 0x9107, 0x7a08, 0x7a09,
    0xc590, 0xd1cb, 0xe2fe, 0x4f01, 0xe3ee, 0xc74d, 0x6dfa, 0xd44a, 0x012e, 0xc449, 0x7816, 0x7819,
    0x7821, 0x781d, 0x782e, 0x7830, 0x7835, 0x783a, 0x783f, 0x7849, 0x784c, 0x786f, 0x786e, 0x7914,
    0x01f8, 0x7a10, 0x7a11, 0x7915, 0x0000, 0x7913, 0x0007, 0x7a12, 0xd1ca, 0xc44f, 0xc4c4, 0xd0ce,
    0x01cd, 0xc64f, 0xc54f, 0xc44f, 0xcb4c, 0xd5c4, 0x78c4, 0xc64f, 0xc74f, 0xcf4c, 0xc64f, 0xc54f,
    0xcb4c, 0x3d09, 0xc64f, 0xc54f, 0xcb4c, 0x3d08, 0xd449, 0x0130, 0xe302, 0xc480, 0x786e, 0xcf80,
    0x78b2, 0xc04f, 0xc4c9, 0x7869, 0xc64f, 0xc54f, 0xcb4c, 0xc04f, 0xc5cb, 0x78a9, 0xc54f, 0xc44f,
    0xc94a, 0xd1ce, 0x8405, 0x785d, 0xc54f, 0xc44f, 0xc94a, 0xd1ce, 0x8406, 0x7857, 0xc74f, 0xc64f,
    0xcd4e, 0xc74f, 0xc54f, 0xcb4e, 0xc74f, 0xc44f, 0xc94e, 0xd1cf, 0x7892, 0xc64f, 0xc54f, 0xcb4c,
    0xc549, 0xc04f, 0x0001, 0x0400, 0xc4cb, 0xd148, 0x0015, 0x0406, 0xc04f, 0xd0c1, 0x7d01, 0x6cfc,
    0xd0ca, 0x8418, 0x0001, 0xc4cb, 0xd1c9, 0x0001, 0x840c, 0xe901, 0x0000, 0x7803, 0xe911, 0xd048,
    0xffff, 0x7b00, 0xe920, 0xd1c8, 0xc04f, 0xc14f, 0xc24f, 0xc34f, 0xc44f, 0xc54f, 0xc64f, 0xc74f,
    0xd1ca, 0x8704, 0x0001, 0x0410, 0xc4cb, 0xc54f, 0xc44f, 0xc94a, 0x3c0d, 0xc54f, 0xc44f, 0xc94a,
    0x3c0f, 0xc54f, 0xc44f, 0xc94a, 0x3c0e, 0x0001, 0xd448, 0x2010, 0xd548, 0x3010, 0xd749, 0x013a,
    0xe304, 0xd448, 0x1010, 0xd548, 0x1010, 0xc4cb, 0xc5cb, 0x0006, 0xc4cb, 0x7b0d, 0xe3fe, 0xd0ce,
    0x0113, 0x0006, 0xc4cb, 0x0007, 0xc849, 0xc4cb, 0xd0ce, 0x0113, 0xc74d, 0xc64d, 0xc54d, 0xc44d,
    0xc34d, 0xc24d, 0xc14d, 0xc04d, 0x7a05, 0x840c, 0x4100, 0xe101, 0x4104, 0xe301, 0x4201, 0xe501,
    0x4302, 0xd94a, 0xd94b, 0x3c0c, 0x0001, 0x0400, 0xc4cb, 0xd0c8, 0xd148, 0x0010, 0x0406, 0xc0c1,
    0xc04d, 0x7c01, 0x6cfc, 0xd0cf, 0x013b, 0xd448, 0x55aa, 0x78d5, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000 };  /* SAM9407 boot loader words */
unsigned char sam9407_start[37] = {
    0x09, 0x00, 0x02, 0x3f, 0xc0, 0x00, 0xc1, 0x00, 0xc2, 0x00, 0xc3, 0x00,
    0xc4, 0x00, 0xc5, 0x00, 0xc6, 0x00, 0xc7, 0x00, 0xc8, 0x00, 0xc9, 0x00,
    0xca, 0x00, 0xcb, 0x00, 0xcc, 0x00, 0xcd, 0x00, 0xce, 0x00, 0xcf, 0x00,
    0xff };  /* SAM9407 init byte sequence */

/* Load SAM9407.BIN into the Dream SAM9407 wavetable chip of a Maxi Sound card through its MPU
   port at base. The file must be 4 KB to 128 KB and have the 0xD0CE signature at offset 0x400.
   Sequence:
   1. Put the chip into boot mode and send the 256-word boot loader (sam9407_boot) to port+2.
   2. Send the start command and stream the file from offset 0x400 as 16-bit words.
   3. Send the sam9407_start command bytes.
   Prints a message and returns if the file is missing or bad. */
void sam9407_upload(unsigned base)
{
    register unsigned port = base;
    unsigned word = 0;
    unsigned long size;
    register FILE *fp;
    int i;
    int lo, hi;
    unsigned out;

    fp = fopen("SAM9407.BIN", "rb");
    if (fp == NULL) {
        puts("SAM9407.BIN firmware file not found. Sound will be silent.");
        return;
    }
    /* TC 3.0 pushes long 0 as xor ax,ax/push ax/push ax; BC 3.1 emits xor ax,ax/xor dx,dx */
    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);
    rewind(fp);
    if (size > 0x20000L || size < 0x1000L) {
        puts("Bad SAM9407 firmware file.");
        fclose(fp);
        return;
    }
    fseek(fp, 0x400L, SEEK_SET);
    word = fgetc(fp);
    word = (fgetc(fp) << 8) | word;
    if (word != 0xd0ce) {
        puts("Bad SAM9407 firmware file.");
        fclose(fp);
        return;
    }
    fseek(fp, 0L, SEEK_SET);     /* same long-0 difference as above */
    outportb(port + 1, 0x3f);
    outportb(port + 1, 0x48);
    outportb(port, 0);
    outportb(port + 1, 0x70);
    outportb(port, 0x11);
    delay(20);
    for (i = 0; i < 0x100; i++)
        outpw(port + 2, sam9407_boot[i]);
    delay(20);
    outportb(port + 1, 0x0b);
    outportb(port + 1, 0);
    outportb(port + 1, 2);
    outportb(port + 1, 0);
    outportb(port + 1, 0);
    outportb(port + 1, 0x93);
    outportb(port + 1, 0x66);
    delay(20);
    fseek(fp, 0x400L, SEEK_SET);
    i = 0;
    printf("Uploading SAM9407 firmware");
    while ((lo = fgetc(fp)) != EOF) {
        hi = fgetc(fp);
        if (hi == EOF)
            hi = 0;
        word = hi << 8 | lo;
        outpw(port + 2, word);
        i++;
        if (i % 5000 == 0)
            printf(".");
    }
    puts(" Done.");
    fclose(fp);
    delay(100);
    for (i = 0; i < 0x25; i++) {
        if (i < 4 || i == 0x24)
            out = port + 1;
        else
            out = port;
        outportb(out, sam9407_start[i]);
    }
}

/* InterWave: read one byte of on-board DRAM/ROM at 24-bit address addr. Sets the local memory
   address registers 0x43/0x44 through the register select at sb_port + 0x103 (sb_port holds
   the GUS base for these cards), then reads the DRAM I/O port at + 0x107. */
unsigned char iw_mem_read(unsigned long addr)
{
    outportb(sb_port + 0x103, 0x43);
    outpw(sb_port + 0x104, (unsigned)addr);
    outportb(sb_port + 0x103, 0x44);
    outportb(sb_port + 0x105, (unsigned char)(addr >> 16));
    return inportb(sb_port + 0x107);
}

/* InterWave: write one byte val to on-board DRAM at 24-bit address addr (see iw_mem_read). */
void iw_mem_write(unsigned long addr, unsigned char val)
{
    outportb(sb_port + 0x103, 0x43);
    outpw(sb_port + 0x104, (unsigned)addr);
    outportb(sb_port + 0x103, 0x44);
    outportb(sb_port + 0x105, (unsigned char)(addr >> 16));
    outportb(sb_port + 0x107, val);
}

/* InterWave: switch to ROM access and scan the four 4 MB ROM banks for a sample ROM header
   ("INTR" signature with a zero checksum over 512 bytes). Prints each bank's name and size, or
   "ROM: Not found". Restores the memory configuration afterwards. Always returns 0. */
int iw_list_roms(void)
{
    unsigned char old = 0;
    unsigned char sum = 0;
    unsigned char mask = 0;
    unsigned mcfg = 0;
    unsigned long size = 0;
    unsigned long addr = 0;
    char sig[5] = { 0 };
    char name[17] = { 0 };     /* 16 characters + NUL */
    register int i = 0;
    register int n = 0;

    outportb(sb_port + 0x103, 0x99);
    old = inportb(sb_port + 0x105);
    outportb(sb_port + 0x103, 0x19);
    outportb(sb_port + 0x105, old | 1);
    outportb(sb_port + 0x103, 0x53);
    outportb(sb_port + 0x105, 3);
    outportb(sb_port + 0x103, 0x52);
    mcfg = inpw(sb_port + 0x104) & 0xff10;
    outpw(sb_port + 0x104, mcfg | 0x4c);
    for (addr = 0; addr < 0x1000000L; addr += 0x400000L) {
        for (i = 0; i < 4; i++)
            sig[i] = iw_mem_read(addr + i);
        if (strncmp(sig, "INTR", 4) != 0)
            continue;
        sum = 0;
        for (i = 0; i < 0x200; i++)
            sum += iw_mem_read(addr + i);
        if (sum != 0)
            continue;
        n++;
        mask |= 1 << (addr >> 22);
        for (i = 0; i < 16; i++)
            name[i] = iw_mem_read(addr + i + 10);
        size = ((unsigned long)iw_mem_read(addr + 0x2b) << 24) +
               ((unsigned long)iw_mem_read(addr + 0x2a) << 16) +
               ((unsigned long)iw_mem_read(addr + 0x29) << 8) +
               (unsigned)iw_mem_read(addr + 0x28);
        if (n > 0)
            printf("ROM Bank #%d: [%s] Size: %dK \n", n, name, size >> 10);
    }
    if (n < 1)
        puts("ROM: Not found");
    outportb(sb_port + 0x103, 0x53);
    outportb(sb_port + 0x105, 0);
    outportb(sb_port + 0x103, 0x19);
    outportb(sb_port + 0x105, old);
    return 0;
}

/* detect on-board RAM size per bank, program the memory config register; returns total KB */

/* InterWave: size the on-board DRAM. Zeroes the first byte of every 64 KB block, then tests each of the four 4 MB banks with a
   pattern and finds its size by stepping 64 KB at a time until the pattern shows up again
   (address wrap). Then programs the memory
   configuration register to the matching layout, forced to 512 KB with /XGL.
   If dst is not NULL, each bank's size in bytes is stored there.
   Returns the total in KB. */
int iw_detect_dram(unsigned long *dst)
{
    register unsigned long *out = dst;
    unsigned long sizes[4] = { 0 };
    unsigned long addr = 0;
    unsigned long base = 0;
    unsigned long off = 0;
    unsigned char bank = 0;
    unsigned char cfg = 0;
    unsigned char old = 0;
    unsigned char found = 0;
    register unsigned mcfg = 0;
    int total = 0;

    outportb(sb_port + 0x103, 0x99);
    old = inportb(sb_port + 0x105);
    outportb(sb_port + 0x103, 0x19);
    outportb(sb_port + 0x105, old | 1);
    outportb(sb_port + 0x103, 0x53);
    outportb(sb_port + 0x105, 1);
    outportb(sb_port + 0x103, 0x52);
    mcfg = inpw(sb_port + 0x104) & 0xfff0;
    outpw(sb_port + 0x104, mcfg | 0xc);
    for (; addr < 0x1000000L; addr += 0x10000L)
        iw_mem_write(addr, 0);
    for (bank = 0; bank < 4; bank++) {
        iw_mem_write(base, 0xaa);
        iw_mem_write(base + 1, 0x55);
        if (iw_mem_read(base) == 0xaa && iw_mem_read(base + 1) == 0x55)
            found = 1;
        if (found) {
            while (off < 0x400000L) {
                sizes[bank] += 0x10000L;
                off += 0x10000L;
                addr = base + off;
                if (iw_mem_read(addr) == 0xaa)
                    break;
            }
        }
        if (out)
            *out++ = sizes[bank];
        sizes[bank] >>= 10;
        base += 0x400000L;
        off = 0;
        found = 0;
    }
    for (addr = 0; addr < 0x200000L; addr += 0x400) {
        iw_mem_write(addr, 0);
        iw_mem_write(addr + 1, 0);
    }
    outportb(sb_port + 0x103, 0x52);
    if (sizes[0] == 0x100 && sizes[1] == 0 && sizes[2] == 0 && sizes[3] == 0)
        cfg = 0;
    else if (sizes[0] == 0x100 && sizes[1] == 0x100 && sizes[2] == 0 && sizes[3] == 0)
        cfg = 1;
    else if (sizes[0] == 0x100 && sizes[1] == 0x100 && sizes[2] == 0x100 && sizes[3] == 0x100)
        cfg = 2;
    else if (sizes[0] == 0x100 && sizes[1] == 0x400 && sizes[2] == 0 && sizes[3] == 0)
        cfg = 3;
    else if (sizes[0] == 0x100 && sizes[1] == 0x400 && sizes[2] == 0x400 && sizes[3] == 0x400)
        cfg = 4;
    else if (sizes[0] == 0x100 && sizes[1] == 0x100 && sizes[2] == 0x400 && sizes[3] == 0)
        cfg = 5;
    else if (sizes[0] == 0x100 && sizes[1] == 0x100 && sizes[2] == 0x400 && sizes[3] == 0x400)
        cfg = 6;
    else if (sizes[0] == 0x400 && sizes[1] == 0 && sizes[2] == 0 && sizes[3] == 0)
        cfg = 7;
    else if (sizes[0] == 0x400 && sizes[1] == 0x400 && sizes[2] == 0 && sizes[3] == 0)
        cfg = 8;
    else if (sizes[0] == 0x400 && sizes[1] == 0x400 && sizes[2] == 0x400 && sizes[3] == 0x400)
        cfg = 9;
    else if (sizes[0] == 0x1000 && sizes[1] == 0 && sizes[2] == 0 && sizes[3] == 0)
        cfg = 10;
    else if (sizes[0] == 0x1000 && sizes[1] == 0x1000 && sizes[2] == 0 && sizes[3] == 0)
        cfg = 11;
    else if (sizes[0] == 0x1000 && sizes[1] < 0x1000 && sizes[2] == 0 && sizes[3] == 0)
        cfg = 10;
    else if (sizes[0] == 0x400 && sizes[1] > 0x400 && sizes[2] == 0 && sizes[3] == 0)
        cfg = 8;
    else if (sizes[0] == 0x400 && sizes[1] < 0x400 && sizes[2] == 0 && sizes[3] == 0)
        cfg = 7;
    else if (sizes[0] == 0x100 && sizes[1] > 0x100 && sizes[2] == 0 && sizes[3] == 0)
        cfg = 3;
    else
        cfg = 12;
    if (opt_xgl == 1)
        cfg = 1;
    cfg |= 0x80;
    outpw(sb_port + 0x104, mcfg | cfg);
    outportb(sb_port + 0x103, 0x19);
    outportb(sb_port + 0x105, old);
    total = sizes[0] + sizes[1] + sizes[2] + sizes[3];
    return total;
}

/* InterWave register write. reg encodes type << 16 | index << 8 | width; all ports are relative to
   sb_port, which holds the GUS base for these cards:
   - types 1-0x0E: write the low byte of val at base + width
   - types 0x0F-0x18: control register `index` selected through base + 0x0F (with the register
     control at + 0x103/0x105 unlocked around it); types 0x0F/0x10 also flip bit 6 at base + 0
   - types 0x1A-0x21: write at base + 0x100 + width, or a word at + 0x104 when width is 4
   - types 0x22-0x5B: GF1 register `index` through the select at + 0x103, data at + 0x104/+ 0x105
   - type 0x5C: the DRAM I/O port at + 0x107
   - types 0x5D-0x61: write at base + 0x10C + width
   - types 0x62-0x81: codec register `index` through base + 0x10C/+ 0x10D
   Types 0x19 and 0x1B are ignored. The constants passed by iw_init and the iw_set_* functions
   follow this encoding. */
void iw_write(unsigned long reg, unsigned val)
{
    unsigned char sel;
    unsigned char ctl_f;
    unsigned char saved;
    unsigned type;
    unsigned width;

    width = (unsigned char)reg;
    type = (unsigned)(reg >> 16);
    sel = reg >> 8;
    if (type >= 1 && type <= 0x19) {
        if (type <= 0xe) {
            outp(sb_port + width, (unsigned char)val);
            return;
        }
        if (type == 0x19)
            return;
        {
            outp(sb_port + 0x103, 0x5b);
            saved = inp(sb_port + 0x105);
            outp(sb_port + 0x105, (unsigned char)(saved | 9));
            ctl_f = inp(sb_port + 0xf);
            ctl_f = (ctl_f & 0xf8) | sel;
            outp(sb_port + 0xf, ctl_f);
            if (reg == 0xf000bL || reg == 0x10000bL) {
                ctl_f = inp(sb_port);
                outp(sb_port, (unsigned char)(reg == 0xf000bL ? ctl_f & 0xbf : ctl_f | 0x40));
            }
            outp(sb_port + 0xb, (unsigned char)val);
            outp(sb_port + 0x103, 0x5b);
            outp(sb_port + 0x105, saved);
        }
    } else if (type >= 0x1a && type <= 0x5c) {
        if (type == 0x5c) {
            outp(sb_port + 0x107, (unsigned char)val);
        } else if (type != 0x1b) {
            if (type >= 0x1a && type <= 0x21) {
                if (width == 4)
                    outpw(sb_port + 0x104, val);
                else
                    outp(sb_port + 0x100 + width, (unsigned char)val);
            } else {
                outp(sb_port + 0x103, sel);
                if (width == 4)
                    outpw(sb_port + 0x104, val);
                else
                    outp(sb_port + 0x105, (unsigned char)val);
            }
        }
    } else if (type >= 0x5c && type <= 0x81) {
        if (type <= 0x61) {
            outp(sb_port + 0x10c + width, (unsigned char)val);
        } else {
            saved = inp(sb_port + 0x10c);
            saved = (saved & 0xe0) + sel;
            outp(sb_port + 0x10c, saved);
            outp(sb_port + 0x10d, (unsigned char)val);
        }
    }
    return;
}

/* InterWave (GUS PnP) initialization:
   - programs the GF1 and codec control registers and initializes the 32 synth voices,
   - lists ROM banks and sizes the DRAM (printing a warning when there is none),
   - sets the master, DAC, synth, line, CD and mic levels, muting the inputs whose volume is 0. */
void iw_init(void)
{
    int i;
    unsigned ram = 0;

    iw_write(0x5b6005L, 0);
    iw_write(0x565b05L, 0x15);
    iw_write(0x10000L, mic_vol > 0 ? 0xc : 8);
    iw_write(0x555a05L, 0x7f);
    iw_write(0x454505L, 0);
    iw_write(0x545905L, 0x1f);
    iw_write(0x565b05L, 0x15);
    iw_write(0x4a4c05L, 0);
    delay(2);
    iw_write(0x4a4c05L, 1);
    iw_write(0x220005L, 2);
    iw_write(0x2f0d05L, 2);
    for (i = 0; i < 0x20; i++) {
        iw_write(0x1e0002L, i);
        iw_write(0x3d1b04L, 2);
        iw_write(0x2e0c04L, 2);
        iw_write(0x371505L, 2);
        iw_write(0x220005L, 2);
        iw_write(0x2f0d05L, 2);
    }
    iw_write(0x4a4c05L, 7);
    delay(2);
    iw_write(0x555a05L, 0x7f);
    iw_write(0x6c0a01L, 0);
    iw_write(0x6e0c01L, 0x6c);
    iw_write(0x6b0901L, 0);
    iw_write(0x721001L, 0x81);
    iw_write(0x731101L, 0xc2);
    iw_write(0x6c0a01L, 0);
    delay(10);
    iw_list_roms();
    delay(10);
    ram = iw_detect_dram(0);
    delay(10);
    printf("RAM detected: %d KB", ram);
    delay(2);
    if (ram < 1)
        printf(" - You need some RAM to get GUS mode working!!");
    else if (opt_xgl == 1)
        printf(" (Forced to 512 KB)");
    printf("\n");
    iw_set_synth(0, fm_reg);
    iw_set_dac(0, wave_reg);
    iw_set_master(0, master_reg);
    iw_set_line(line_vol <= 0, line_reg);
    iw_set_cd(cd_vol <= 0, cd_reg);
    iw_set_mic(mic_vol <= 0, mic_reg);
}

/* InterWave codec: set the left/right master output attenuation (codec registers 0x19/0x1B) to v,
   and mute them if flag is non-zero. */
void iw_set_master(int flag, unsigned char v)
{
    iw_write(0x7b1901L, v | (flag ? 0x80 : 0));
    iw_write(0x7d1b01L, v | (flag ? 0x80 : 0));
}

/* InterWave codec: set the left/right DAC (wave) attenuation (registers 0x06/0x07) to v, muted if
   flag. */
void iw_set_dac(int flag, unsigned char v)
{
    iw_write(0x680601L, v | (flag ? 0x80 : 0));
    iw_write(0x690701L, v | (flag ? 0x80 : 0));
}

/* InterWave codec: set the left/right AUX1 (synth) attenuation (registers 0x02/0x03) to v, muted
   if flag. */
void iw_set_synth(int flag, unsigned char v)
{
    iw_write(0x640201L, v | (flag ? 0x80 : 0));
    iw_write(0x650301L, v | (flag ? 0x80 : 0));
}

/* InterWave codec: set the left/right line-in attenuation (registers 0x12/0x13) to v, muted if
   flag. */
void iw_set_line(int flag, unsigned char v)
{
    iw_write(0x741201L, v | (flag ? 0x80 : 0));
    iw_write(0x751301L, v | (flag ? 0x80 : 0));
}

/* InterWave codec: set the left/right AUX2 (CD) attenuation (registers 0x04/0x05) to v, muted if
   flag. */
void iw_set_cd(int flag, unsigned char v)
{
    iw_write(0x660401L, v | (flag ? 0x80 : 0));
    iw_write(0x670501L, v | (flag ? 0x80 : 0));
}

/* InterWave codec: set the left/right mic attenuation (registers 0x16/0x17) to v, muted if
   flag. */
void iw_set_mic(int flag, unsigned char v)
{
    iw_write(0x781601L, v | (flag ? 0x80 : 0));
    iw_write(0x791701L, v | (flag ? 0x80 : 0));
}

/* EMU8000: wait (up to 2100 polls) until the DRAM read address register (SMALR, 0x34) no longer
   reports busy (bit 31). */
void emu_wait_read(void)
{
    int i = 0;
    while (emu_read32(0x34, awe_port + 0x400) & 0x80000000L) {
        i++;
        if (i > 0x834)
            break;
    }
}

/* EMU8000: wait (up to 2100 polls) until the DRAM write address register (SMALW, 0x36) no longer
   reports busy (bit 31). */
void emu_wait_write(void)
{
    int i = 0;
    while (emu_read32(0x36, awe_port + 0x400) & 0x80000000L) {
        i++;
        if (i > 0x834)
            break;
    }
}

/* EMU8000: size the sample DRAM. Writes a marker at the DRAM start (0x200000), then walks upward
   in 32 KB steps writing a test word, until the test word doesn't read back or the marker gets
   overwritten (address wrap). Opens and closes the DMA channels around the test.
   Returns the size in KB (0 when only the first step worked, at most 28 MB). */
int emu_detect_dram(void)
{
    int size = 0;

    emu_dma_open(1);
    emu_wait_write();
    emu_write32(0x36, awe_port + 0x400, 0x200000L);
    emu_write16(0x3a, awe_port + 0x400, 0xa5b6);
    emu_wait_write();
    emu_wait_read();
    emu_write32(0x34, awe_port + 0x400, 0x200000L);
    emu_read16(0x3a, awe_port + 0x400);
    if (emu_read16(0x3a, awe_port + 0x400) == 0xa5b6)
        size += 0x20;
    emu_wait_read();
    while (size < 0x7000) {
        emu_write32(0x36, awe_port + 0x400, ((long)size << 9) + 0x200000L);
        emu_write16(0x3a, awe_port + 0x400, 0x9de3);
        emu_wait_write();
        emu_write32(0x34, awe_port + 0x400, ((long)size << 9) + 0x200000L);
        emu_read16(0x3a, awe_port + 0x400);
        if (emu_read16(0x3a, awe_port + 0x400) != 0x9de3)
            break;
        emu_wait_read();
        emu_write32(0x34, awe_port + 0x400, 0x200000L);
        emu_read16(0x3a, awe_port + 0x400);
        if (emu_read16(0x3a, awe_port + 0x400) != 0xa5b6)
            break;
        emu_wait_read();
        size += 0x20;
    }
    emu_dma_close();
    if (size == 0x7000)
        size--;
    if (size == 0x20)
        size = 0;
    return size;
}

/* EMU8000: prepare the 30 voices used as DRAM DMA channels: silence them and set their envelope
   and address registers. Odd channels are set up for reading when odd_read is non-zero, all
   others for writing. */
void emu_dma_open(int odd_read)
{
    unsigned i;
    for (i = 0; i < 0x1e; i++) {
        emu_write16(i + 0xa0, awe_port + 0x400, 0x80);
        emu_write32(i + 0x60, awe_port, 0L);
        emu_write32(i + 0x40, awe_port, 0L);
        emu_write32(i + 0x20, awe_port, 0x40000000L);
        emu_write32(i, awe_port, 0x40000000L);
        emu_write32(i + 0xc0, awe_port, 0L);
        emu_write32(i + 0xe0, awe_port, 0L);
        emu_write32(i, awe_port + 0x400, (i & 1) && odd_read ? 0x04000000L : 0x06000000L);
    }
}

/* EMU8000: return the 30 DRAM DMA channels to normal voices. */
void emu_dma_close(void)
{
    unsigned i;
    for (i = 0; i < 0x1e; i++) {
        emu_write32(i, awe_port + 0x400, 0L);
        emu_write16(i + 0xa0, awe_port + 0x400, 0x807f);
    }
}

/* EMU8000: write a 32-bit value to register reg (selected through the pointer register at
   awe_port + 0x802) at data port `port` (low word) and port + 2 (high word). */
void emu_write32(unsigned reg, unsigned port, unsigned long val)
{
    outpw(awe_port + 0x802, reg);
    outpw(port, (unsigned)val);
    outpw(port + 2, (unsigned)(val >> 16));
}

/* EMU8000: write a 16-bit value to register reg at data port `port`. */
void emu_write16(unsigned reg, unsigned port, unsigned val)
{
    outpw(awe_port + 0x802, reg);
    outpw(port, val);
}

/* EMU8000: read a 16-bit value from register reg at data port `port`. */
unsigned emu_read16(unsigned reg, unsigned port)
{
    unsigned v;
    outpw(awe_port + 0x802, reg);
    v = inpw(port);
    return v;
}

/* EMU8000: read a 32-bit value from register reg at data port `port` (low word) and port + 2
   (high word). */
unsigned long emu_read32(unsigned reg, unsigned port)
{
    unsigned long lo;
    unsigned long hi;
    outpw(awe_port + 0x802, reg);
    lo = inpw(port);
    hi = inpw(port + 2);
    lo |= hi << 16;
    return lo;
}

/* EMU8000 initialization:
   1. Initialize all 32 voices and the effects engine.
   2. Write the four init tables (emu_init1-4) twice: the second time with some entries
      changed to enable the effects.
   3. Set up voices 30 and 31 as the FM passthrough, which sends the OPL output through the
      effects engine, with the /VR reverb amount (PTRX bits 15-8) and the /VU chorus amount
      (CSL bits 31-24).
   4. Size the DRAM and enable the audio output.
   ver is the chip version from awe_init. Returns the DRAM size in KB. */
int emu_init(int ver)
{
    int i = 0;
    int j = 1;
    unsigned pat_lo = 0x3ff;
    unsigned pat_hi = 0x30;
    unsigned pat_hi2 = 0x8030;
    unsigned spin = 0;
    unsigned target = 0;
    int mem = 0;
    long chorus_send = awe_chorus * 0xff / 0x63;
    unsigned long reverb_send = (unsigned long)(awe_reverb * 0xff / 0x63) << 8;

    emu_write16(0x3d, awe_port + 0x400, 0x59);
    emu_write16(0x3e, awe_port + 0x400, 0x20);
    emu_write16(0x3f, awe_port + 0x400, 0);
    for (i = 0; i < 0x20; i++) {
        emu_write16(i + 0xa0, awe_port + 0x400, 0x80);
        emu_write16(i + 0xc0, awe_port + 0x402, 0);
        emu_write16(i + 0xe0, awe_port + 0x400, 0);
        emu_write16(i, awe_port + 0x800, 0xd000);
        emu_write16(i + 0x20, awe_port + 0x800, 0xff00);
        emu_write16(i + 0x40, awe_port + 0x800, 0);
        emu_write16(i + 0x60, awe_port + 0x800, 0);
        emu_write16(i + 0x80, awe_port + 0x800, 0x18);
        emu_write16(i + 0xa0, awe_port + 0x800, 0x18);
        emu_write16(i + 0xc0, awe_port + 0x800, 0);
        emu_write16(i + 0xe0, awe_port + 0x402, 0);
        emu_write16(i + 0xa0, awe_port + 0x402, 0);
        emu_write16(i + 0x80, awe_port + 0x402, 0);
        emu_write16(i + 0x80, awe_port + 0x400, 0);
        emu_write16(i + 0xc0, awe_port + 0x400, 0);
    }
    emu_read16(0x3b, awe_port + 0x402);
    emu_read16(0x3b, awe_port + 0x402);
    emu_read16(0x3b, awe_port + 0x402);
    for (i = 0; i < 0x20; i++) {
        emu_write32(i + 0x20, awe_port, 0L);
        emu_write32(i + 0x60, awe_port, 0xffffL);
        emu_write32(i + 0xc0, awe_port, 0L);
        emu_write32(i + 0xe0, awe_port, 0L);
        emu_write32(i, awe_port, 0L);
        emu_write32(i + 0x40, awe_port, 0xffffL);
        emu_write32(i, awe_port + 0x400, 0L);
        emu_write32(i + 0xa0, awe_port, 0L);
        emu_write32(i + 0x80, awe_port, 0L);
        emu_write16(i + 0xe0, awe_port + 0x400, 0x807f);
        emu_write16(i + 0xa0, awe_port + 0x400, 0x807f);
    }
    emu_write32(0x34, awe_port + 0x400, 0L);
    emu_write32(0x35, awe_port + 0x400, 0L);
    emu_write32(0x36, awe_port + 0x400, 0L);
    emu_write32(0x35, awe_port + 0x400, 0L);

    j = 1;
    for (i = 0; i < 0x20; i += 2) {
        emu_write16(i + 0x40, awe_port + 0x400, pat_lo);
        emu_write16(j + 0x40, awe_port + 0x400, pat_hi);
        pat_lo += 0x400;
        pat_hi += 0x100;
        j += 2;
    }
    j = 1;
    pat_hi = 0x30;
    for (i = 0; i < 0x20; i += 2) {
        emu_write16(i + 0x40, awe_port + 0x402, pat_lo);
        emu_write16(j + 0x40, awe_port + 0x402, pat_hi);
        pat_lo += 0x400;
        pat_hi += 0x100;
        j += 2;
    }
    j = 1;
    pat_hi = 0x30;
    for (i = 0; i < 0x20; i += 2) {
        emu_write16(i + 0x60, awe_port + 0x400, pat_lo);
        emu_write16(j + 0x60, awe_port + 0x400, pat_hi);
        pat_lo += 0x400;
        pat_hi += 0x100;
        j += 2;
    }
    j = 1;
    pat_hi = 0x30;
    for (i = 0; i < 0x20; i += 2) {
        emu_write16(i + 0x60, awe_port + 0x402, pat_lo);
        emu_write16(j + 0x60, awe_port + 0x402, pat_hi);
        pat_lo += 0x400;
        pat_hi += 0x100;
        j += 2;
    }
    for (i = 0; i < 0x14; i++)
        emu_read16(0x3b, awe_port + 0x402);

    spin = 0;
    outpw(awe_port + 0x802, 0x3b);
    target = inpw(awe_port + 0x402);
    target += 0x400;
    if (target - 0x400 < target) {
        while (inpw(awe_port + 0x402) > target && spin < 0xfff0)
            spin++;
    }
    spin = 0;
    while (inpw(awe_port + 0x402) < target && spin < 0xfff0)
        spin++;

    j = 1;
    pat_lo = 0x3ff;
    pat_hi2 = 0x8030;
    for (i = 0; i < 0x20; i += 2) {
        emu_write16(i + 0x40, awe_port + 0x400, pat_lo);
        emu_write16(j + 0x40, awe_port + 0x400, pat_hi2);
        pat_lo += 0x400;
        pat_hi2 += 0x100;
        j += 2;
    }
    j = 1;
    pat_hi2 = 0x8030;
    for (i = 0; i < 0x20; i += 2) {
        emu_write16(i + 0x40, awe_port + 0x402, pat_lo);
        emu_write16(j + 0x40, awe_port + 0x402, pat_hi2);
        pat_lo += 0x400;
        pat_hi2 += 0x100;
        j += 2;
    }
    j = 1;
    pat_hi2 = 0x8030;
    for (i = 0; i < 0x20; i += 2) {
        emu_write16(i + 0x60, awe_port + 0x400, pat_lo);
        emu_write16(j + 0x60, awe_port + 0x400, pat_hi2);
        pat_lo += 0x400;
        pat_hi2 += 0x100;
        j += 2;
    }
    j = 1;
    pat_hi2 = 0x8030;
    for (i = 0; i < 0x20; i += 2) {
        emu_write16(i + 0x60, awe_port + 0x402, pat_lo);
        emu_write16(j + 0x60, awe_port + 0x402, pat_hi2);
        pat_lo += 0x400;
        pat_hi2 += 0x100;
        j += 2;
    }
    for (i = 0; i < 0x14; i++)
        emu_write32(i + 0x20, awe_port + 0x400, 0L);

    for (i = 0; i < 0x20; i++)
        emu_write16(i + 0x40, awe_port + 0x400, emu_init1[i]);
    for (i = 0; i < 0x20; i++)
        emu_write16(i + 0x40, awe_port + 0x402, emu_init2[i]);
    for (i = 0; i < 0x20; i++)
        emu_write16(i + 0x60, awe_port + 0x400, emu_init3[i]);
    for (i = 0; i < 0x20; i++)
        emu_write16(i + 0x60, awe_port + 0x402, emu_init4[i]);

    emu_write32(0x29, awe_port + 0x400, 0L);
    emu_write32(0x2a, awe_port + 0x400, 0x83L);
    emu_write32(0x2d, awe_port + 0x400, 0x8000L);
    emu_write32(0x2e, awe_port + 0x400, 0L);

    emu_init1[13] = 0xf7c;
    emu_init1[15] = 0x7254;
    emu_init1[21] = 0xbaa;
    emu_init1[23] = 0x7234;
    emu_init1[29] = 0x6e7;
    emu_init1[31] = 0x7224;
    emu_init2[5] = 0x7f6;
    emu_init2[7] = 0x7254;
    emu_init2[13] = 0xf02;
    emu_init2[15] = 0x7264;
    emu_init2[21] = 0xfa9;
    emu_init2[23] = 0x7294;
    emu_init2[29] = 0x44c3;
    emu_init2[31] = 0x45c3;
    emu_init3[5] = 0x671;
    emu_init3[7] = 0x287;
    emu_init3[11] = 0xc7b;
    emu_init3[13] = 0x7e6;
    emu_init3[21] = 0x21f;
    emu_init3[23] = 0x386;
    emu_init3[25] = 0xc03;
    emu_init3[27] = 0x31e;
    emu_init3[31] = 0xc55;
    emu_init4[5] = 0xead;
    emu_init4[11] = 0x129;
    emu_init4[15] = 0x31f;
    emu_init4[19] = 0x327f;
    emu_init4[21] = 0x265;
    emu_init4[27] = 0x337f;

    for (i = 0; i < 0x20; i++)
        emu_write16(i + 0x40, awe_port + 0x400, emu_init1[i]);
    for (i = 0; i < 0x20; i++)
        emu_write16(i + 0x40, awe_port + 0x402, emu_init2[i]);
    for (i = 0; i < 0x20; i++)
        emu_write16(i + 0x60, awe_port + 0x400, emu_init3[i]);
    for (i = 0; i < 0x20; i++)
        emu_write16(i + 0x60, awe_port + 0x402, emu_init4[i]);

    chorus_send = chorus_send << 24;
    emu_write16(0xbe, awe_port + 0x400, 0x80);
    emu_write32(0xde, awe_port, 0xffffffe0L);
    emu_write32(0xfe, awe_port, 0xffffe8L | chorus_send);
    emu_write32(0x3e, awe_port, reverb_send);
    emu_write32(0x1e, awe_port, 0L);
    emu_write32(0x1e, awe_port + 0x400, 0xffffe3L);
    emu_write16(0xbf, awe_port + 0x400, 0x80);
    emu_write32(0xdf, awe_port, 0xfffff0L);
    emu_write32(0xff, awe_port, 0xfffff8L | chorus_send);
    emu_write32(0x3f, awe_port, 0xff | reverb_send);
    emu_write32(0x1f, awe_port, 0x8000L);
    emu_write32(0x1f, awe_port + 0x400, 0xfffff3L);
    emu_write32(0x3e, awe_port, (unsigned long)(ver == 1 ? 0x4828u : 0x4800u) << 16);
    outpw(awe_port + 0x802, 0x3c);
    outpw(awe_port + 0x400, 0);
    emu_write32(0x7e, awe_port, 0xffffffffL);
    emu_write32(0x7f, awe_port, 0xffffffffL);
    emu_write16(0x3e, awe_port + 0x400, 0x20);
    emu_write16(0x3f, awe_port + 0x400, 0);
    mem = emu_detect_dram();
    emu_write16(0x3f, awe_port + 0x400, 4);
    outpw(awe_port + 0x802, 0x3e);
    outpw(awe_port + 0x802, 0xa0);
    outpw(awe_port + 0x802, 0x3e);
    outpw(awe_port + 0x802, 0xfe);
    return mem;
}

/* ESS ES1869, or ES1868 with /XEA (the "alternate init" in the manual): put the cards back into
   Wait for Key and send send_alt_pnp_key, which apparently tells the chip to use ess_ctl_port as
   its PnP address port. Then use ess_ctl_port and ess_ctl_port + 1 as the PnP address and
   write-data ports from now on. */
void use_alt_pnp_ports(void)
{
    pnp_write(2, 2);
    delay(2);
    send_alt_pnp_key();
    pnp_addr_port = ess_ctl_port;
    pnp_wdata_port = ess_ctl_port + 1;
}

/* Send the alternate 32-byte key, XORed with 0..3 in four rounds, each round followed by
   the low and high bytes of ess_ctl_port, to the current PnP address port. */
void send_alt_pnp_key(void)
{
    unsigned char key[32] = {
        0x66, 0xa1, 0xc2, 0xf1, 0xea, 0xe7, 0x71, 0xaa, 0xc7, 0x63, 0x33, 0x1b,
        0x0d, 0x96, 0xdb, 0x6d, 0xa4, 0x50, 0x28, 0x16, 0x9b, 0x4d, 0xb6, 0xc9,
        0xf4, 0x78, 0x3e, 0x8d, 0xd6, 0xfb, 0x7f, 0x3d };
    int i = 0;
    int j = 0;
    for (j = 0; j < 4; j++) {
        for (i = 0; i < 32; i++)
            outportb(pnp_addr_port, key[i] ^ j);
        delay(1);
        outportb(pnp_addr_port, ess_ctl_port & 0xff);
        outportb(pnp_addr_port, (ess_ctl_port >> 8) & 0xff);
    }
}

/* Upload the built-in CS4232 firmware patch (cs4232_fw_hdr and cs4232_fw) through the Crystal
   control port (crystal_ctl_port + 5, each part terminated by a write to + 6). Codec register
   0x0C is set to 0xC0 during the upload and back to 0 afterwards. */
void cs4232_upload_fw(void)
{
    int i = 0;
    printf("Loading CS4232 Firmware");
    idx45_write(wss_port, 0x0c, 0xc0);
    putchar('.');
    for (i = 0; i < 8; i++)
        outportb(crystal_ctl_port + 5, cs4232_fw_hdr[i]);
    outportb(crystal_ctl_port + 6, 0);
    delay(2);
    putchar('.');
    for (i = 0; i < 0xc8d; i++)
        outportb(crystal_ctl_port + 5, cs4232_fw[i]);
    outportb(crystal_ctl_port + 6, 0);
    delay(2);
    putchar('.');
    idx45_write(wss_port, 0x0c, 0);
    puts(" done.");
}

/* Extra setup for Crystal CS4236/4237-based cards. card is the variant from configure_pnp
   (2 = Orpheus, 3 = Turtle Beach, 7 = other cards with the same options). It:
   - selects the FM source: internal CrystalFM with /XOFi (the default for variant 7), otherwise an
     external Yamaha OPL3,
   - switches S/PDIF output on with /XOS1 (or off), including the 3D level from /X3D,
   - sets the wave and master volumes through the codec's extended registers,
   - with S/PDIF on, enables the line, CD, mic and FM inputs that have a volume set. */
void crystal_ext_init(int card)
{
    unsigned char ctl = 0;
    unsigned char vol1 = 0;
    unsigned char vol2 = 0;
    int surr;
    int val;
    int i;

    surr = (opt_x3d * 15 / 9) | (~(opt_x3d * 15 / 9) << 4);
    if (opt_x3d < 1)
        surr = 0;
    vol1 = (100 - master_vol) * 63 / 97;
    vol2 = (100 - wave_vol) * 63 / 97;
    if (vol1 > 63)
        vol1 = 63;
    if (vol2 > 63)
        vol2 = 63;
    if (card == 2)
        printf("Orpheus");
    else {
        printf("Special");
        if (opt_xof == -1)
            opt_xof = 1;
    }
    printf(" settings: ");
    idx01_write(wss_port, 0x0c, 0xea);
    idx01_write(wss_port, 0x17, 0x48);
    delay(5);
    ctl = inportb(wss_port + 1);
    if (card == 2 || card == 7) {
        if (opt_xof == 1) {
            idx01_write(wss_port, 0x17, 0x48);
            outportb(wss_port + 1, ctl | 0x10);
            printf("FM: [CrystalFM]  ");
        } else {
            idx01_write(wss_port, 0x17, 0x48);
            outportb(wss_port + 1, ctl & 0xef);
            delay(5);
            if (card == 2)
                cs_ctl_setbits(8, 8);
            printf("FM: [Yamaha OPL3]  ");
        }
    }
    delay(10);
    if (card == 2 || card == 3) {
        if (opt_xos == 1) {
            cs_ctl_setbits(8, 4);
            idx01_write(wss_port, 0x4c, 0x60);
            idx01_write(wss_port, 0x10, 0x02);
            delay(10);
            idx01_write(wss_port, 0x51, 0x02);
            delay(5);
            idx01_write(wss_port, 0x50, 0x82);
            delay(10);
            idx01_write(wss_port, 0x5c, 0x50);
            idx01_write(wss_port, 0x48, 0x5b);
            delay(5);
            idx01_write(wss_port, 0x49, 0x00);
            idx01_write(wss_port, 0x4a, 0x06);
            idx01_write(wss_port, 0x4d, 0x01);
            idx01_write(wss_port, 0x40, 0x00);
            idx01_write(wss_port, 0x41, 0x00);
            delay(5);
            cs_ctl_write(4, 0x40);
            cs_ctl_write(3, 0x20);
            cs_ctl_write(6, 0x00);
            cs_ctl_write(5, 0x04);
            cs_ctl_write(4, 0x80);
            cs_ctl_write(4, 0xc0);
            cs_ctl_write(2, 0x77);
            cs_ctl_write(3, 0xa0);
            delay(20);
            idx01_write(wss_port, 0x1a, 0xc2);
            delay(60);
            idx01_write(wss_port, 0x0f, 0xaf);
            idx01_write(wss_port, 0x0e, 0x02);
            idx01_write(wss_port, 0x48, 0x5b);
            delay(5);
            idx01_write(wss_port, 0x49, 0x00);
            idx01_write(wss_port, 0x4a, 0x06);
            delay(5);
            cs_xreg_write(0x0b, 0xe0);
            cs_xreg_write(0x0d, 0x60);
            delay(10);
            cs_ctl_write(2, surr);
            printf("S/PDIF: [ON]");
        } else {
            idx01_write(wss_port, 0x10, 0x00);
            delay(10);
            cs_xreg_write(0x0b, 0xc0);
            cs_xreg_write(0x0d, 0x18);
            cs_xreg_write(0x0a, 0x3f);
            idx01_write(wss_port, 0x5c, 0x00);
            idx01_write(wss_port, 0x4d, 0x00);
            idx01_write(wss_port, 0x50, 0x00);
            delay(20);
            idx01_write(wss_port, 0x51, 0x00);
            cs_ctl_write(2, surr);
            cs_ctl_write(3, opt_x3d > 0 ? 0x80 : 0);
            cs_ctl_write(4, 0x00);
            printf("S/PDIF: [OFF]");
        }
    }
    delay(10);
    idx01_write(wss_port, 0x06, vol2);
    idx01_write(wss_port, 0x07, vol2);
    cs_xreg_write(0x0e, vol1);
    cs_xreg_write(0x0f, vol1);
    if ((card == 2 || card == 3) && opt_xos == 1) {
        val = 0;
        i = 0;
        if (line_vol > 0) {
            for (i = 2; i < 4; i++) {
                outportb(wss_port, i);
                val = inportb(wss_port + 1);
                outportb(wss_port + 1, (val & 0x1f) | 0xa0);
            }
        }
        if (cd_vol > 0) {
            for (i = 4; i < 6; i++) {
                outportb(wss_port, i);
                val = inportb(wss_port + 1);
                outportb(wss_port + 1, (val & 0x1f) | 0xa0);
            }
        }
        if (mic_vol > 0) {
            for (i = 2; i < 4; i++) {
                idx01_write(wss_port, 0x17, (i << 4) | 8);
                val = inportb(wss_port + 1);
                outportb(wss_port + 1, (val & 0x1f) | 0xa0);
            }
        }
        if (fm_vol > 0) {
            for (i = 0x12; i < 0x14; i++) {
                outportb(wss_port, i);
                val = inportb(wss_port + 1);
                outportb(wss_port + 1, (val & 0x1f) | 0xa0);
            }
        }
        if (cd_vol > 0)
            idx45_write(sb_port, 0x0c, 3);
        if (line_vol > 0)
            idx45_write(sb_port, 0x0c, 7);
    }
    puts("");
}

/* Crystal control port: write val to control register reg (index at crystal_ctl_port + 3, data
   at + 4). */
void cs_ctl_write(unsigned char reg, unsigned char val)
{
    outportb(crystal_ctl_port + 3, reg);
    delay(2);
    outportb(crystal_ctl_port + 4, val);
    delay(5);
}

/* Crystal control port: set `bits` in control register reg (read-modify-write). */
void cs_ctl_setbits(unsigned char reg, unsigned char bits)
{
    unsigned char v = 0;
    outportb(crystal_ctl_port + 3, reg);
    v = inportb(crystal_ctl_port + 4);
    delay(2);
    outportb(crystal_ctl_port + 4, v | bits);
    delay(5);
}

/* CS4236 extended register write: select extended register `unit` through codec index register
   23 (unit << 4 | 8) and write val to the codec data port. */
void cs_xreg_write(unsigned char unit, unsigned char val)
{
    idx01_write(wss_port, 0x17, (unit << 4) | 8);
    outportb(wss_port + 1, val);
    delay(5);
}

/* Detect an OPL2/OPL3 at 0x388 with the usual AdLib timer test: reset both timers, start timer 1
   with a 0xFF count and check that the status register shows its overflow.
   Returns 1 if found, else 0. */
int opl_detect(void)
{
    int stat1 = 0;
    int stat2 = 0;
    int found = 0;
    opl_write(4, 0x60);
    opl_write(4, 0x80);
    delay(10);
    stat1 = inportb(0x388);
    opl_write(2, 0xff);
    opl_write(4, 0x21);
    delay(10);
    stat2 = inportb(0x388);
    opl_write(4, 0x60);
    opl_write(4, 0x80);
    stat1 = stat1 & 0xe0;
    stat2 = stat2 & 0xe0;
    if (stat1 == 0 && stat2 == 0xc0)
        found = 1;
    else
        found = 0;
    return found;
}

/* Write val to OPL register reg at 0x388/0x389, with the delays the OPL2 needs after each write. */
void opl_write(unsigned char reg, unsigned char val)
{
    outportb(0x388, reg);
    delay(6);
    outportb(0x389, val);
    delay(8);
}

/* /XNET workaround for Realtek (and compatible) ISA PnP network cards. According to the manual
   they deactivate their logical devices when the PnP scan for the sound card runs. This sends
   their own initiation key, isolates the card on read-data port `port`, gives it CSN 0x81, writes
   0x81 to its activate register, and returns all cards to Wait for Key. */
void xnet_workaround(int port)
{
    unsigned char key[34] = {
        0x00, 0x00, 0xda, 0x6d, 0x36, 0x1b, 0x8d, 0x46, 0x23, 0x91, 0x48, 0xa4,
        0xd2, 0x69, 0x34, 0x9a, 0x4d, 0x26, 0x13, 0x89, 0x44, 0xa2, 0x51, 0x28,
        0x94, 0xca, 0x65, 0x32, 0x19, 0x0c, 0x86, 0x43, 0xa1, 0x50 };
    int i = 0;
    unsigned char buf[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    pnp_write(2, 2);
    delay(2);
    for (i = 0; i < 34; i++)
        outportb(pnp_addr_port, key[i]);
    delay(1);
    pnp_write(3, 0);
    delay(2);
    pnp_write(0, (port >> 1) >> 1);
    delay(1);
    pnp_isolate(port, buf);
    pnp_write(6, 0x81);
    delay(2);
    pnp_write(0x30, 0x81);
    delay(2);
    pnp_write(2, 2);
    delay(2);
}

/* Write val to register reg of the Philips TEA6330T tone/volume chip on some GUS PnP cards (I2C
   address 0x80), bit-banged through aux_ctl_port: start condition, address, register, data,
   stop condition. */
void tea6330_write(unsigned char reg, unsigned char val)
{
    i2c_lines(1);
    i2c_lines(0);
    i2c_lines(2);
    i2c_write_byte(0x80);
    i2c_write_byte(reg);
    i2c_write_byte(val);
    i2c_lines(1);
    i2c_lines(3);
    delay(2);
}

/* I2C: clock out the byte v, MSB first, then give one extra clock for the acknowledge bit. */
void i2c_write_byte(unsigned char v)
{
    int i = 0;
    unsigned char bit = 0;
    for (i = 7; i >= 0; i--) {
        bit = ((v >> i) & 1) << 1;
        i2c_lines(1 | bit);
        i2c_lines(bit);
    }
    i2c_lines(3);
    i2c_lines(2);
}

/* I2C: drive SCL (bit 0) and SDA (bit 1) to the levels in v. The port is written 3 times so the
   lines settle. */
void i2c_lines(unsigned char v)
{
    int i = 0;
    for (i = 0; i < 3; i++)
        outportb(aux_ctl_port, v);
}

/* OPTi 82C928 (and OAK OTI601): unlock with the password 0xE2 at 0xF8F and read the
   configuration register at `port`. Waits 30 ms first. */
unsigned char opti928_read(unsigned port)
{
    delay(30);
    outportb(0xf8f, 0xe2);
    return inportb(port);
}

/* OPTi 82C928: unlock with the password 0xE2 at 0xF8F and write val to the configuration
   register at `port`. */
void opti928_write(unsigned port, unsigned char val)
{
    outportb(0xf8f, 0xe2);
    outportb(port, val);
}

/* /NOPNP: detect and configure a non-PnP OAK OTI601 / OPTi 82C928 (configuration registers
   0xF8D-0xF91). Applies the SB/WSS base, gameport, IRQ (5/7/11) and DMA (0/1/3) from BLASTER or
   the defaults, initializes the WSS codec and its volumes, and selects SB or WSS mode (/XWSS).
   Returns 1 if the chip was found, else 0. */
int init_opti928(void)
{
    unsigned char id_f8f = 0;
    unsigned char id_f8d = 0;
    unsigned char cfg1 = 0x80;
    unsigned char cfg2 = 0;
    unsigned char codec_st = 0;
    id_f8d = opti928_read(0xf8d);
    id_f8f = opti928_read(0xf8f);
    if (id_f8d < 0xff && id_f8f < 0xff) {
        puts("Card detected: OAK OTI601 / OPTi 82C928");
        puts(parse_blaster() ? "Loading BLASTER values..." : "Loading default values...");
        if (sb_port == 0x240 || wss_port == 0xf40) {
            wss_port = 0xf40;
            cfg1 |= 0x20;
        } else {
            sb_port = 0x220;
            wss_port = 0x530;
            cfg1 &= 0xcf;
        }
        if (joy_port < 1)
            cfg1 |= 1;
        else {
            cfg1 &= 0xfe;
            joy_port = 0x200;
        }
        switch (sb_irq) {
        case 11: cfg2 |= 0x40; break;
        case 7: break;
        default: sb_irq = 5; cfg2 |= 0x80;
        }
        switch (sb_dma) {
        case 3: cfg2 |= 0x20; break;
        case 0: cfg2 |= 0x10; break;
        default: sb_dma = 1;
        }
        opti928_write(0xf8d, 0x40);
        delay(120);
        opti928_write(0xf8d, 0x80);
        delay(140);
        opti928_write(0xf8e, 3);
        delay(50);
        opti928_write(0xf8d, cfg1);
        delay(50);
        opti928_write(0xf8f, cfg2);
        opti928_write(0xf90, 2);
        delay(40);
        outportb(wss_port + 4, 0x40);
        idx45_write_slow(wss_port, 0x44, 0xf);
        idx45_write_slow(wss_port, 0x45, 0xf);
        idx45_write_slow(wss_port, 0x49, 0x40);
        delay(70);
        idx45_write_slow(wss_port, 0x4a, 2);
        idx45_write_slow(wss_port, 0x4d, 0);
        idx45_write_slow(wss_port, 0x4c, 0x8a);
        idx45_write_slow(wss_port, 0x48, 3);
        delay(60);
        idx45_write_slow(wss_port, 0x4e, 0xff);
        fm_reg = fm_vol > 0 ? (100 - fm_vol) * 31 / 97 : 0x80;
        wave_reg = wave_vol > 0 ? (100 - wave_vol) * 63 / 97 : 0x80;
        line_reg = line_vol > 0 ? (100 - line_vol) * 31 / 97 : 0x80;
        opti928_write(0xf91, 0x1a);
        outportb(wss_port + 4, 0x2c);
        codec_st = inportb(wss_port + 5);
        idx45_write_slow(wss_port, 0x24, fm_reg);
        idx45_write_slow(wss_port, 0x25, fm_reg);
        idx45_write_slow(wss_port, 0x26, wave_reg);
        idx45_write_slow(wss_port, 0x27, wave_reg);
        idx45_write_slow(wss_port, 0x22, line_reg);
        idx45_write_slow(wss_port, 0x23, line_reg);
        if (opt_xwss == 0)
            opti928_write(0xf8d, opti928_read(0xf8d) & 0x7f);
        delay(50);
        opti928_write(0xf91, (codec_st & 0xfd) == 2 ? 0x2a : 0x20);
        printf("Settings: ADD:%X IRQ:%d DMA:%d JOY:%X", sb_port, sb_irq, sb_dma, joy_port);
        if (opt_xwss > 0)
            printf(" [WSS mode: %X]", wss_port);
        else {
            printf(" [SB Mode]");
            delay(20);
            outportb(sb_port + 6, 1);
            delay(4);
            outportb(sb_port + 6, 0);
            delay(20);
            outportb(sb_port + 12, 0x20);
            printf("\nMixer [WAV:%d FM:%d LIN/CD:%d]\n", wave_vol, fm_vol, line_vol);
        }
        return 1;
    }
    return 0;
}

/* /NOPNP: if an OPTi 82C92x control chip answers at 0xF8C, enable its configuration through
   register 7 and set nopnp_mode to 2. configure_pnp then moves opti_ctl_port by 0x80. */
void probe_opti_ctl(void)
{
    if (opti92x_read(0xf8c, 1) < 0xff && opti92x_read(0xf8c, 3) < 0xff) {
        opti92x_write(0xf8c, 7, 0x40);
        nopnp_mode = 2;
    }
}

/* /NOPNP: detect and configure a non-PnP OPTi 82C930 (control registers at 0xE0E). Applies the
   SB base, WSS base, gameport, IRQ, DMA, MPU port and MPU IRQ from BLASTER or the defaults, then
   initializes the WSS codec, mixer and volumes.
   Returns 1 if the chip was found, else 0. */
int init_opti930(void)
{
    unsigned char cfg1 = 0;
    unsigned char cfg2 = 0;
    unsigned char cfg3 = 0x83;
    if (mpu_irq < 0)
        mpu_irq = 9;
    opti_ctl_port = 0xe0c;
    opti93x_pwd_port = 0xf8f;
    if (opti93x_read(0xe0e, 1) < 0xff && opti93x_read(0xe0e, 3) < 0xff && opti93x_read(0xe0e, 0x12) < 8) {
        puts("Card detected: OPTi 83C930");
        puts(parse_blaster() ? "Loading BLASTER values..." : "Loading default values...");
        if (sb_port == 0x240)
            cfg1 |= 0x80;
        else {
            sb_port = 0x220;
            cfg1 &= 0x7f;
        }
        switch (wss_port) {
        case 0xe80: cfg1 |= 0x10; break;
        case 0x604: cfg1 |= 0x30; break;
        default: wss_port = 0x530; cfg1 &= 0xcf;
        }
        if (joy_port < 1) {
            cfg1 |= 1;
            joy_port = 0;
        } else {
            cfg1 &= 0xfe;
            joy_port = 0x200;
        }
        switch (sb_irq) {
        case 11: cfg2 |= 0x20; break;
        case 10: cfg2 |= 0x18; break;
        case 9: cfg2 |= 0x10; break;
        case 7: cfg2 |= 8; break;
        default: sb_irq = 5; cfg2 |= 0x28;
        }
        switch (sb_dma) {
        case 3: cfg2 |= 3; break;
        case 0: cfg2 |= 1; break;
        default: sb_dma = 1; cfg2 |= 2;
        }
        switch (mpu_port) {
        case 0: cfg3 &= 0xef; break;
        case 0x300: cfg3 |= 0x60; break;
        case 0x310: cfg3 |= 0x40; break;
        case 0x320: cfg3 |= 0x20; break;
        default: mpu_port = 0x330;
        }
        switch (mpu_irq) {
        case 2: case 7: cfg3 |= 0x18; break;
        case 5: cfg3 |= 0x10; break;
        case 10: cfg3 |= 8; break;
        default: mpu_irq = 9;
        }
        delay(40);
        opti93x_write(opti_ctl_port + 2, 2, 0);
        opti93x_write(opti_ctl_port + 2, 1, cfg1);
        opti93x_write(opti_ctl_port + 2, 3, cfg2);
        opti93x_write(opti_ctl_port + 2, 4, 0x52);
        opti93x_write(opti_ctl_port + 2, 5, 0x3d);
        opti93x_write(opti_ctl_port + 2, 6, cfg3);
        opti93x_write(opti_ctl_port + 2, 0xa, 0x10);
        delay(20);
        idx45_write_slow(wss_port, 0x29, 0);
        idx45_write_slow(wss_port, 0x2a, 2);
        idx45_write_slow(wss_port, 0x2b, 0);
        idx45_write_slow(wss_port, 0x48, 3);
        delay(100);
        opti93x_write(opti_ctl_port + 2, 3, cfg2);
        idx45_write_slow(wss_port, 0x30, ((100 - fm_vol) * 15 / 97) << 1);
        idx45_write_slow(wss_port, 0x31, ((100 - fm_vol) * 15 / 97) << 1);
        idx45_write_slow(wss_port, 0x20, 0x89);
        idx45_write_slow(wss_port, 0x21, 0x89);
        idx45_write_slow(wss_port, 0x36, 7);
        idx45_write_slow(wss_port, 0x37, 7);
        delay(10);
        idx45_write_slow(wss_port, 0x26, 0x84);
        idx45_write_slow(wss_port, 0x27, 0x84);
        delay(80);
        sbpro_mixer_set();
        printf("Settings: ADD:%X IRQ:%d DMA:%d WSS:%X", sb_port, sb_irq, sb_dma, wss_port);
        printf(" MPU:%X/I%d JOY:%X", mpu_port, mpu_irq, joy_port);
        puts("");
        print_mixer("OPTi", -1, -1);
        return 1;
    }
    return 0;
}

/* Discard bytes waiting in the DSP read buffer at port (data-available is bit 7 of port + 0x0E),
   such as the 0xAA a DSP reset leaves or the reply to an earlier command, so the next command
   reads its own reply. Reads at most 16 bytes, so a port with no card answering 0xFF is harmless. */
void sb_dsp_flush(int port)
{
    int i;
    for (i = 0; i < 16 && (inportb(port + 0xe) & 0x80); i++)
        inportb(port + 0xa);
}

/* /NOPNP: detect and configure a non-PnP ESS ES688/ES1688. Probes 0x220-0x260 for the ESS
   identify command (0xE7 answering 0x68 and a version byte), then sets the IRQ and DMA through
   extended registers 0xB1/0xB2. On the ES1688 it also sets the MPU-401 and gameport (register
   0x40), then sets up the SB Pro mixer.
   Returns 1 if a card was found, else 0. */
int init_es688(void)
{
    int port = 0x220;
    int irqv = 0x16;
    int dmav = 0x1a;
    int mpu = 0;
    unsigned char ver = 0;
    for (port = 0x220; port <= 0x260; port += 0x10) {
        sb_dsp_flush(port);
        outportb(port + 0xc, 0xe7);
        delay(10);
        if (inportb(port + 0xa) == 0x68) {
            delay(10);
            ver = inportb(port + 0xa);
            break;
        }
    }
    if (ver >= 0x80) {
        outportb(port + 6, 3);
        delay(20);
        outportb(port + 6, 0);
        delay(80);
        sb_dsp_flush(port);
        outportb(port + 0xc, 0xc6);
        outportb(port + 4, 0x40);
        if (inportb(port + 5) != 0x18 || inportb(port + 5) < 0x68) {
            printf("Card detected: ES");
            printf(ver < 0x88 ? "668" : "1688");
            printf(" at [%xh] \n", port);
            puts(parse_blaster() ? "Loading BLASTER values..." : "Loading default values...");
            sb_port = port;
            delay(50);
            switch (sb_irq) {
            case 2: case 9: irqv -= 4; break;
            case 7: irqv += 4; break;
            case 10: irqv += 8; break;
            default: sb_irq = 5;
            }
            idx45_write_slow(sb_port + 8, 0xb1, irqv);
            switch (sb_dma) {
            case 0: dmav -= 4; break;
            case 3: dmav += 4; break;
            default: sb_dma = 1;
            }
            idx45_write_slow(sb_port + 8, 0xb2, dmav);
            if (ver >= 0x88) {
                switch (mpu_irq) {
                case 2: case 9: mpu |= 0x80; break;
                case 5: mpu |= 0xa0; break;
                case 7: mpu |= 0xc0; break;
                case 10: mpu |= 0xe0; break;
                default: mpu_irq = sb_irq; mpu |= 0x40;
                }
                if (joy_port > 0) {
                    mpu |= 2;
                    joy_port = 0x201;
                } else
                    joy_port = 0;
                mpu |= ((mpu_port & 0xf0) >> 1) | 1;
                idx45_write_slow(sb_port, 0x40, mpu);
            }
            printf("Settings: ADD:%X IRQ:%d DMA:%d", sb_port, sb_irq, sb_dma);
            if (ver >= 0x88)
                printf(" MPU:%X/I%d JOY:%X", mpu_port, mpu_irq, joy_port);
            puts("");
            sbpro_mixer_set();
            print_mixer("ESS", -1, -1);
            return 1;
        }
    }
    return 0;
}
