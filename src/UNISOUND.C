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

struct ldinfo {
    int ld[9];
    char x[7];
};
#ifdef __BORLANDC__
typedef unsigned port_t;
#else
typedef int port_t;
#endif
int inp(unsigned portid);
int outp(unsigned portid, int value);
unsigned outpw(unsigned portid, unsigned value);

/* ---- global data (initialized; addresses in the original DGROUP) ---- */
int g_b3f2[32] = {
    0x0c10, 0x8470, 0x14fe, 0xb488, 0x167f, 0xa470, 0x18e7, 0x84b5, 0x1b6e, 0x842a, 0x1f1d, 0x852a,
    0x0da3, 0x8f7c, 0x167e, 0xf254, 0x0000, 0x842a, 0x0001, 0x852a, 0x18e6, 0x8baa, 0x1b6d, 0xf234,
    0x229f, 0x8429, 0x2746, 0x8529, 0x1f1c, 0x86e7, 0x229e, 0xf224 };  /* EMU8000 init data 1 (reg 0x40, data1) */
int g_b432[32] = {
    0x0da4, 0x8429, 0x2c29, 0x8529, 0x2745, 0x87f6, 0x2c28, 0xf254, 0x383b, 0x8428, 0x320f, 0x8528,
    0x320e, 0x8f02, 0x1341, 0xf264, 0x3eb6, 0x8428, 0x3eb9, 0x8528, 0x383a, 0x8fa9, 0x3eb5, 0xf294,
    0x3eb7, 0x8474, 0x3eba, 0x8575, 0x3eb8, 0xc4c3, 0x3ebb, 0xc5c3 };  /* EMU8000 init data 2 (reg 0x40, data2) */
int g_b472[32] = {
    0x0000, 0xa404, 0x0001, 0xa504, 0x141f, 0x8671, 0x14fd, 0x8287, 0x3ebc, 0xe610, 0x3ec8, 0x8c7b,
    0x031a, 0x87e6, 0x3ec8, 0x86f7, 0x3ec0, 0x821e, 0x3ebe, 0xd280, 0x3ebd, 0x821f, 0x3eca, 0x8386,
    0x3ec1, 0x8c03, 0x3ec9, 0x831e, 0x3eca, 0x8c4c, 0x3ebf, 0x8c55 };  /* EMU8000 init data 3 (reg 0x60, data1) */
int g_b4b2[32] = {
    0x3ec9, 0xc280, 0x3ec4, 0xbc84, 0x3ec8, 0x8ead, 0x3ec8, 0xd380, 0x3ec2, 0x8f7e, 0x3ecb, 0x8219,
    0x3ecb, 0xd2e6, 0x3ec5, 0x831f, 0x3ec6, 0xc380, 0x3ec3, 0xb27f, 0x3ec9, 0x8265, 0x3ec9, 0x8319,
    0x1342, 0xd3e6, 0x3ec7, 0xb37f, 0x0000, 0x8365, 0x1420, 0x9570 };  /* EMU8000 init data 4 (reg 0x60, data2) */
unsigned char g_b4f2[8] = {
    0xaa, 0x99, 0x2a, 0xc2, 0x53, 0x02, 0x12, 0x86 };  /* CS4232 firmware header (8 bytes) */
unsigned char g_b4fa[3213] = {
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
int g_c187 = 0x0220;  /* SB base port */
int g_c189 = 0x0388;  /* OPL port */
int g_c18b = 0x0620;  /* AWE port */
int g_c18d = 0x0330;  /* MPU-401 port */
int g_c18f = 0x0299;  /* gameport */
int g_c191 = 0x0530;  /* WSS / codec port */
int g_c193 = 0x0370;  /* control port (Yamaha) */
int g_c195 = 0x0120;  /* control port (CS) */
int g_c197 = 0x0f0c;  /* control port (OPTi) */
int g_c199 = 0x0800;  /* control port (ESS) */
int g_c19b = 0x0310;  /* 2nd MPU port */
int g_c19d = 0;  /* IDE port */
int g_c19f = 5;  /* IRQ */
int g_c1a1 = 0;  /* 2nd IRQ */
int g_c1a3 = -1;  /* MPU IRQ */
int g_c1a5 = 0;  /* IDE IRQ */
int g_c1a7 = 1;  /* DMA 1 */
int g_c1a9 = 1;  /* DMA 2 */
int g_c1ab = 0;  /* WSS DMA */
int g_c1ad = 0x0250;  /* control port (Rockwell / InterWave) */
int g_c1af = 0x0240;  /* GUS port (ULTRASND) */
int g_c1b1 = 3;  /* GUS DMA 1 */
int g_c1b3 = 3;  /* GUS DMA 2 */
int g_c1b5 = 0x000b;  /* GUS IRQ */
int g_c1b7 = 0;  /* GUS IRQ 2 */
unsigned g_c1b9 = 0x0279;  /* PnP address port (0x279) */
unsigned g_c1bb = 0x0a79;  /* PnP write data port (0xa79) */
int g_c1bd = 0;  /* PnP read data port */
unsigned char g_c1bf = 0xdd;  /* master vol (register value) */
int g_c1c0 = 0x0055;  /* master vol (0-99) */
unsigned char g_c1c2 = 0xcc;  /* wave vol reg */
int g_c1c3 = 0x0050;  /* wave vol */
unsigned char g_c1c5 = 0xcc;  /* FM vol reg */
int g_c1c6 = 0x0050;  /* FM vol */
unsigned char g_c1c8 = 0;  /* line vol reg */
int g_c1c9 = 0;  /* line vol */
unsigned char g_c1cb = 0;  /* CD vol reg */
int g_c1cc = 0;  /* CD vol */
unsigned char g_c1ce = 0;  /* mic vol reg */
int g_c1cf = 0;  /* mic vol */
unsigned char g_c1d1 = 0;  /* PC speaker volume, mixer bits 7-5 */
int g_c1d2 = 0;  /* PC speaker vol / AuxB */
unsigned char g_c1d4 = 0x80;  /* treble level, mixer high nibble */
int g_c1d5 = -1;  /* treble */
unsigned char g_c1d7 = 0x80;  /* bass level, mixer high nibble */
int g_c1d8 = -1;  /* bass */
char g_c1da = 0;  /* /NOPNP mode */
unsigned char g_c1db = 0;  /* read-only mode (/R) */
int g_c1dc = 0;  /* disable mode (/D) */
char g_c1de = 0;  /* resource data checksum state (2 = bad) */
unsigned char g_c1df = 4;  /* selected card number */
int g_c1e0 = 0;  /* AWE chorus amount 00-99 (/VUnn) */
int g_c1e2 = 0;  /* AWE reverb amount 00-99 (/VRnn) */
unsigned g_c1e4 = 0x0f8d;  /* codec index port */
int g_c1e6 = -1;  /* extended option */
int g_c1e8 = 0;  /* extended option */
int g_c1ea = 2;  /* extended option (MPU enable) */
int g_c1ec = -1;  /* IDE port override */
int g_c1ee = 0;  /* /XGL */
int g_c1f0 = 0;  /* gameport enable */
int g_c1f2 = 0;  /* card mode (WSS) */
int g_c1f4 = 0;  /* ESS option */
int g_c1f6 = 0;  /* extra option */
int g_c1f8 = 0;  /* mixer option (SPDIF/3D...) */
int g_c1fa = 0;  /* mixer option 2 */
int g_c1fc = 0;  /* non-PnP mode */

/* ---- functions ---- */
void f_0bad(void);
void f_0be2(void);
int f_0c8a(int a, int b, int pass, unsigned port);
void f_3c21(void);
void f_3c59(unsigned char reg, unsigned char val);
unsigned char f_3c81(unsigned char reg);
void f_3c93(unsigned char reg, unsigned val);
unsigned f_3ccf(unsigned char reg);
void f_3cfd(unsigned port, unsigned char idx, unsigned char val);
void f_3d19(unsigned port, unsigned char idx, unsigned char val);
void f_3d40(unsigned port, unsigned char idx, unsigned char val);
unsigned char f_3d6c(unsigned port, unsigned char reg);
void f_3daa(unsigned port, unsigned char reg, unsigned char val);
void f_3def(unsigned port, unsigned char idx, unsigned char val);
unsigned char f_3e1c(unsigned port, unsigned char idx);
int f_3e51(unsigned port, unsigned char *buf);
char *f_3fb6(unsigned long *id);
int f_405e(void);
int f_4192(void);
int f_4425(unsigned char csn, unsigned char *buf, int type);
int f_44fa(unsigned char csn, char *name, int *ld, int type);
int f_4b76(unsigned char *buf, int n);
int f_4bfc(unsigned char *tag, int *len);
void f_4c96(unsigned port);
void f_4dec(int port);
int f_4e94(void);
void f_5175(int port, unsigned char irq, unsigned char dma, unsigned char hdma);
unsigned char f_5270(int port);
unsigned char f_52d1(int port);
unsigned char f_5378(int port);
void f_5419(char *name, int d3, int gain);
void f_54e9(void);
void f_556d(void);
void f_5715(void);
void f_577f(int mode);
void f_5c48(void);
void f_5de3(unsigned base);
unsigned char f_5fc9(unsigned long addr);
void f_6008(unsigned long addr, unsigned char val);
int f_604a(void);
int f_62c1(unsigned long *dst);
void f_6895(unsigned long reg, unsigned val);
void f_6a91(void);
void f_6d3d(int flag, unsigned char v);
void f_6d8d(int flag, unsigned char v);
void f_6ddd(int flag, unsigned char v);
void f_6e2d(int flag, unsigned char v);
void f_6e7d(int flag, unsigned char v);
void f_6ecd(int flag, unsigned char v);
void f_6f1d(void);
void f_6f4a(void);
int f_6f77(void);
void f_70e5(int a);
void f_71b2(void);
void f_71ec(unsigned reg, unsigned port, unsigned long val);
void f_721c(unsigned reg, unsigned port, unsigned val);
unsigned f_723b(unsigned reg, unsigned port);
unsigned long f_725e(unsigned reg, unsigned port);
int f_72a5(int a);
void f_7d59(void);
void f_7d81(void);
void f_7de9(void);
void f_7ed9(int card);
void f_8474(unsigned char reg, unsigned char val);
void f_849f(unsigned char reg, unsigned char bits);
void f_84e1(unsigned char unit, unsigned char val);
int f_850e(void);
void f_85ba();   /* no prototype in the original: callers push promoted ints */
void f_85dd(int port);
void f_86aa(unsigned char reg, unsigned char val);
void f_86f1(unsigned char v);
void f_8745(unsigned char v);
unsigned char f_8760(unsigned port);
void f_8777(unsigned port, unsigned char val);
int f_8789(void);
void f_8b2d(void);
int f_8b68(void);
int f_8f2f(void);

/* ======== chunk 1 ======== */
#define g_c1da (*(unsigned char *)&g_c1da)
#define g_c1de (*(unsigned char *)&g_c1de)
/* main: parse command line options, then run the card configuration */
int main(int argc, char *argv[])
{
    int fs = 0;
    int fsval = 0x20b;
    int card = 1;
    int list = 0;
    int i = 0;
    int j = 0;

    if (argc > 1) {
        for (i = 1; i <= argc; i++) {
            for (j = 1; j <= strlen(argv[i]); j++)
                argv[i][j] = toupper(argv[i][j]);
            if (argv[i][0] == '/' || argv[i][0] == '-') {
                if (argv[i][1] == 'C') {
                    if (argv[i][2] == 'L') {
                        card = 10;
                        list = 1;
                    } else if (list < 1) {
                        if (argv[i][2] == 'S')
                            g_c1da = 1;
                        if (isdigit(argv[i][2]))
                            card = argv[i][2] - '0';
                    }
                }
                if (argv[i][1] == 'V') {
                    if (isdigit(argv[i][2]) && isdigit(argv[i][3])) {
                        g_c1c0 = argv[i][3] + (argv[i][2] - '0') * 10 - '0';
                        g_c1bf = g_c1c0 * 15 / 95 * 17;
                    }
                    if (argv[i][2] == 'F' && isdigit(argv[i][3]) && isdigit(argv[i][4])) {
                        g_c1c3 = argv[i][4] + (argv[i][3] - '0') * 10 - '0';
                        g_c1c2 = g_c1c3 * 15 / 95 * 17;
                    }
                    if (argv[i][2] == 'W' && isdigit(argv[i][3]) && isdigit(argv[i][4])) {
                        g_c1c6 = argv[i][4] + (argv[i][3] - '0') * 10 - '0';
                        g_c1c5 = g_c1c6 * 15 / 95 * 17;
                    }
                    if (argv[i][2] == 'L' && isdigit(argv[i][3]) && isdigit(argv[i][4])) {
                        g_c1c9 = argv[i][4] + (argv[i][3] - '0') * 10 - '0';
                        g_c1c8 = g_c1c9 * 15 / 95 * 17;
                    }
                    if (argv[i][2] == 'C' && isdigit(argv[i][3]) && isdigit(argv[i][4])) {
                        g_c1cc = argv[i][4] + (argv[i][3] - '0') * 10 - '0';
                        g_c1cb = g_c1cc * 15 / 95 * 17;
                    }
                    if (argv[i][2] == 'M' && isdigit(argv[i][3]) && isdigit(argv[i][4])) {
                        g_c1cf = argv[i][4] + (argv[i][3] - '0') * 10 - '0';
                        g_c1ce = g_c1cf * 15 / 95 * 17;
                    }
                    if (argv[i][2] == 'P' && isdigit(argv[i][3]) && isdigit(argv[i][4])) {
                        g_c1d2 = argv[i][4] + (argv[i][3] - '0') * 10 - '0';
                        g_c1d1 = g_c1d2 * 7 / 95 << 5;
                    }
                    if (argv[i][2] == 'T' && isdigit(argv[i][3]) && isdigit(argv[i][4])) {
                        g_c1d5 = argv[i][4] + (argv[i][3] - '0') * 10 - '0';
                        g_c1d4 = g_c1d5 * 15 / 93 << 4;
                    }
                    if (argv[i][2] == 'B' && isdigit(argv[i][3]) && isdigit(argv[i][4])) {
                        g_c1d8 = argv[i][4] + (argv[i][3] - '0') * 10 - '0';
                        g_c1d7 = g_c1d8 * 15 / 93 << 4;
                    }
                    if (argv[i][2] == 'U' && isdigit(argv[i][3]) && isdigit(argv[i][4]))
                        g_c1e0 = argv[i][4] + (argv[i][3] - '0') * 10 - '0';
                    if (argv[i][2] == 'R' && isdigit(argv[i][3]) && isdigit(argv[i][4]))
                        g_c1e2 = argv[i][4] + (argv[i][3] - '0') * 10 - '0';
                }
                if (argv[i][1] == 'F') {
                    fs = 1;
                    if (argv[i][2] == 'S') {
                        fsval = 0x20b;
                        g_c1df = 0x50;
                    }
                }
                if (argv[i][1] == 'R')
                    g_c1db = 1;
                if (argv[i][1] == 'D')
                    g_c1dc = 1;
                if (argv[i][1] == 'X') {
                    if (argv[i][2] == 'O') {
                        if (argv[i][3] == 'F') {
                            if (argv[i][4] == 'I')
                                g_c1e6 = 1;
                            else if (argv[i][4] == 'E')
                                g_c1e6 = 0;
                        }
                        if (argv[i][3] == 'S' && argv[i][4] == '1')
                            g_c1e8 = 1;
                        if (argv[i][3] == 'M') {
                            if (argv[i][4] == '1')
                                g_c1ea = 1;
                            else if (argv[i][4] == '0')
                                g_c1ea = 0;
                        }
                    }
                    if (argv[i][2] == 'G') {
                        if (argv[i][3] == 'C') {
                            int n = argv[i][4] - '0';
                            if (n > 1 && n < 6) {
                                switch (n) {
                                case 2: g_c1ec = 0x170; break;
                                case 3: g_c1ec = 0x1e8; break;
                                case 4: g_c1ec = 0x168; break;
                                case 5: g_c1ec = 0; break;
                                }
                            }
                        }
                        if (argv[i][3] == 'L')
                            g_c1ee = 1;
                        if (argv[i][3] == 'J')
                            g_c1f0 = 1;
                    }
                    if (argv[i][2] == 'W' && argv[i][3] == 'S')
                        g_c1f2 = 1;
                    if (argv[i][2] == 'E' && argv[i][3] == 'A')
                        g_c1f4 = 1;
                    if (argv[i][2] == 'X') {
                        if (argv[i][3] == 'F')
                            g_c1de = 1;
                        if (argv[i][3] == 'G' && isdigit(argv[i][4]))
                            g_c1fa = argv[i][4] - '0';
                    }
                    if (argv[i][2] == 'N' && argv[i][3] == 'E')
                        g_c1f6 = 1;
                    if (argv[i][2] == '3' && isdigit(argv[i][4]))
                        g_c1f8 = argv[i][4] - '0';
                    if (argv[i][2] == 'R' && argv[i][3] == 'U')
                        g_c1e6 = 8;
                }
                if (argv[i][1] == 'N' && argv[i][2] == 'O') {
                    g_c1fc = 1;
                    f_8b2d();
                }
                if (argv[i][1] == 'H' || argv[i][1] == '?') {
                    f_0be2();
                    return 0;
                }
            }
        }
    }
    return f_0c8a(card, list, fs, fsval);
}

/* print the program banner followed by a horizontal rule */
void f_0bad(void)
{
    char line[74];

    puts("Universal ISA PnP Sound Card Driver for DOS v0.82a. (c) JazeFox 2019-2025\0");
    memset(line, 0xc4, 73);
    line[73] = 0;
    printf("%s\n", line);
}

/* print the usage/help screen */
void f_0be2(void)
{
    f_0bad();
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
#undef g_c1da
#undef g_c1de

/* ======== chunk 2 ======== */
/* main card detection / configuration routine */
int f_0c8a(int a, int b, int pass, unsigned port)
{
    char *id = 0;               /* EISA id string of the current card */
    char buf[9] = {0};          /* serial identifier read by isolation */
    int found = 0;              /* isolation found a card */
    unsigned char csn = 0;      /* card select number */
    unsigned char cardno = 0;   /* card counter for /CL listing */
    int nsup = 0;               /* number of supported cards found */
    register int type = 0;      /* card family code */
    unsigned char f11 = 0;      /* ESS: Maxi Sound variant */
    unsigned char f12 = 0;      /* Crystal: sub-variant (1,2,3,4,7) */
    int f14 = 0;                /* ALS: sub-variant */
    unsigned char f15 = 0;      /* CMI / RSS sub-variant */
    int f18 = 0;                /* Creative: CQM/AWE state */
    unsigned char f19 = 0;      /* cards listed */
    unsigned char r1a = 0x60;   /* PnP config registers for SB base, MPU, OPL, WSS, IRQ1/2, DMA1/2 */
    unsigned char r1b = 0x62;
    unsigned char r1c = 0x64;
    unsigned char r1d = 0x62;
    unsigned char r1e = 0x70;
    unsigned char r1f = 0x72;
    unsigned char r20 = 0x74;
    unsigned char r21 = 0x75;
    int ret = 0;                /* return value */

    f_0bad();
    if (g_c1da == 1) {
        if (f_4e94() == 0)
            return 1;
        return 0;
    }
    if (g_c1fc == 1) {
        if (f_8f2f() == 0 && f_8b68() == 0 && f_8789() == 0 && f_4e94() == 0) {
            puts("No card detected");
            return 1;
        }
        return 0;
    }
    f_3c59(2, 2);
    delay(2);
    f_3c21();
    f_3c59(2, 4);
    f_3c59(2, 2);
    delay(2);
    f_3c21();
    f_3c59(3, 0);
    while (found == 0 && pass < 2) {
        if (pass == 1)
            f_3c59(0, port >> 1 >> 1);
        delay(1);
        found = f_3e51(port, buf);
        if (found == 0) {
            if (pass == 1)
                f_3c59(3, 0);
            delay(1);
        }
        if (found != 0) {
            g_c1bd = port;
            if (g_c1df < 10) {
                f_3c59(3, 0);
                f_3c59(0, g_c1bd >> 1 >> 1);
                delay(1);
                f_3e51(g_c1bd, buf);
            }
        } else
            g_c1bd = 0;
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
        f_3c59(6, csn);
        delay(1);
        id = f_3fb6(buf);
        if (id && strlen(id) > 5 && nsup < 6) {
            if (memcmp(id, "CTL", 3) == 0) {
                type = 1;
                if (memcmp(id + 5, "F4", 2) == 0)
                    f18 = 2;
            } else if (memcmp(id, "ALS", 3) == 0) {
                type = 2;
                if (memcmp(id + 5, "07", 2) == 0)
                    f14 = 1;
                else if (memcmp(id + 3, "0001", 4) == 0)
                    f14 = 3;
            } else if (memcmp(id, "RWB1", 4) == 0) {
                type = 2;
                f14 = 2;
            } else if (memcmp(id, "ESS", 3) == 0) {
                if (memcmp(id + 3, "096", 3) == 0)
                    type = 6;
                else if (memcmp(id + 3, "1868", 4) == 0)
                    type = 3;
                else if (memcmp(id + 3, "0003", 4) == 0)
                    type = 0xe;
                else if (memcmp(id + 3, "0006", 4) == 0)
                    type = 0xe;
                else if (memcmp(id + 3, "1869", 4) == 0)
                    type = 0xe;
            } else if (memcmp(id, "SEM3006", 7) == 0)
                type = 3;
            else if (memcmp(id, "YMH", 3) == 0)
                type = 4;
            else if (memcmp(id, "NMX22", 5) == 0)
                type = 4;
            else if (memcmp(id, "QHA08", 5) == 0)
                type = 4;
            else if (memcmp(id, "MPI0003", 7) == 0)
                type = 4;
            else if (memcmp(id, "RSS", 3) == 0) {
                type = 5;
                if (memcmp(id + 3, "58", 2) == 0)
                    f15 = 8;
            } else if (memcmp(id, "OPT", 3) == 0) {
                if (memcmp(id + 5, "25", 2) == 0)
                    type = 7;
                else if (memcmp(id + 5, "31", 2) == 0)
                    type = 0xc;
                else if (memcmp(id + 5, "33", 2) == 0)
                    type = 0xc;
                else if (memcmp(id + 5, "24", 2) == 0)
                    type = 0x10;
            } else if (memcmp(id, "CMI", 3) == 0)
                type = 8;
            else if (memcmp(id, "ADS7141", 7) == 0)
                type = 0x13;
            else if (memcmp(id, "ADS71", 5) == 0 || memcmp(id, "AXS71", 5) == 0 ||
                     memcmp(id, "TER11", 5) == 0 || memcmp(id, "TER14", 5) == 0 ||
                     memcmp(id, "AZT1022", 7) == 0 || memcmp(id, "LWC1061", 7) == 0 ||
                     memcmp(id, "MDK1605", 7) == 0 || memcmp(id, "SMM7180", 7) == 0)
                type = 9;
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
                    type = 0xa;
            } else if (memcmp(id, "CSC", 3) == 0 || memcmp(id, "CDC", 3) == 0) {
                if (memcmp(id + 5, "3", 1) == 0) {
                    type = 0xb;
                    if (memcmp(id + 6, "7", 1) == 0)
                        f12 = 7;
                    else
                        f12 = 0;
                    if (memcmp(id + 6, "2", 1) == 0)
                        f12 = 1;
                    if (memcmp(id + 3, "A836", 4) == 0)
                        f12 = 4;
                } else if (memcmp(id + 4, "25", 2) == 0 || memcmp(id + 4, "025", 3) == 0)
                    type = 0xb;
            } else if (memcmp(id, "TBS2004", 7) == 0)
                type = 0xb;
            else if (memcmp(id, "RJA0", 4) == 0)
                type = 0x2c;
            else if (memcmp(id, "ENS4", 4) == 0)
                type = 0x37;
            else if (memcmp(id, "TBS20", 5) == 0)
                type = 0x3b;
            else if (memcmp(id, "GRV", 3) == 0 || memcmp(id, "ADV55", 5) == 0 ||
                     memcmp(id, "STB01", 5) == 0)
                type = 0x40;
        }
        cardno++;
        if (a > 1) {
            printf("Card #%d [%s] ", cardno, id);
            if (b != 1)
                printf("Skipped.");
            else {
                char text[0x46] = {0};
                if (f_4425(csn, text, type))
                    printf("%s", text);
            }
            if (g_c1de == 2)
                printf(" (BAD CHECKSUM!)");
            type = 0;
            printf("\n");
            f19++;
        }
        if (type > 0) {
            nsup++;
            break;
        }
        if (a > 1)
            a--;
        else {
            printf("Unknown PnP card [%s] found. \nAborting...\n", id);
            f_3c59(3, 0);
            delay(1);
            f_3c59(2, 2);
            return 1;
        }
        f_3c59(3, 0);
        delay(1);
        found = f_3e51(g_c1bd, buf);
    }
    if (nsup > 0) {
        char name[0x48] = {0};
        unsigned char lcsn = 0; /* CSN of the supported card */
        struct ldinfo ld = {{-1, -1, -1, -1, -1, -1, -1, -1, -1}};
        if (f_44fa(csn, name, &ld, type))
            lcsn = csn;
        if (lcsn > 0) {
            printf("PnP card found: [%s] %s", id, name);
            if (g_c1de == 2)
                printf(" (BAD CHECKSUM!)");
            puts("");
            if (g_c1dc != 0 && !g_c1db) {
                puts("Disabling card's logical devices...");
                g_c187 = 0;
                g_c189 = 0;
                g_c18b = 0;
                g_c18d = 0;
                g_c18f = 0;
                g_c191 = 0;
                g_c193 = 0;
                g_c195 = 0;
                g_c197 = 0;
                g_c199 = 0;
                g_c19b = 0;
                g_c19d = 0;
                g_c19f = 0;
                g_c1a1 = 0;
                g_c1a3 = 0;
                g_c1a7 = 4;
                g_c1a9 = 4;
                g_c1ab = 4;
                g_c1ad = 0;
                g_c1af = 0;
                g_c1b1 = 4;
                g_c1b3 = 4;
                g_c1b5 = 0;
                g_c1b7 = 0;
            }
            if (g_c1db != 0)
                puts("Reading card current settings...\n");
            else if (g_c1dc == 0) {
                if (type == 0x40) {
                    if (f_405e() == 0)
                        puts("Loading Unisound default settings...");
                    else
                        printf("%s environment var found! Loading settings...\n", "ULTRASND");
                } else {
                    if (f_4192() == 0)
                        puts("Loading Unisound default settings...");
                    else
                        printf("%s environment var found! Loading settings...\n", "BLASTER");
                }
            }
            if (ld.x[3] == 'h')
                type = 0xd;
            if (type == 1) {
                r1b = 0x62;
                r1c = 0x64;
            }
            if (type == 2) {
                r1b = 0x60;
                r1c = 0x60;
            }
            if (type == 3) {
                r1b = 0x64;
                r1c = 0x62;
            }
            if (type == 4) {
                r1b = 0x66;
                r1c = 0x64;
                r1d = 0x62;
            }
            if (type == 5) {
                r1b = 0x60;
                r1c = 0x62;
                g_c1a9 = 4;
            }
            if (type == 6) {
                r1b = 0x60;
                r1c = 0x60;
            }
            if (type == 0xd) {
                r1b = 0x64;
                r1c = 0x62;
            }
            if (type == 7) {
                r1b = 0x60;
                r1c = 0x64;
                r1d = 0x62;
                if (g_c1fc == 2)
                    g_c197 += 0x80;
            }
            if (type == 0x10) {
                r1b = 0x60;
                r1c = 0x64;
                r1d = 0x62;
                if (g_c1fc == 2)
                    g_c197 += 0x80;
            }
            if (type == 8) {
                r1b = 0x60;
                r1c = 0x62;
                r1d = 0x60;
            }
            if (type == 9) {
                r1b = 0x60;
                r1c = 0x62;
                r1d = 0x64;
            }
            if (type == 0x13) {
                r1b = 0x60;
                r1c = 0x60;
                r1d = 0x60;
                g_c1a9 = 4;
            }
            if (type == 0xa) {
                r1b = 0x60;
                r1c = 0x62;
                r1d = 0x64;
                if (g_c191 != 0)
                    g_c191 += 4;
            }
            if (type == 0xb) {
                r1b = 0x60;
                r1c = 0x62;
                r1d = 0x60;
                if (g_c191 != 0)
                    g_c191 += 4;
                r1a = 0x64;
                g_c1a1 = g_c19f;
            }
            if (type == 0xc) {
                r1b = 0x60;
                r1c = 0x62;
                r1d = 0x60;
                if (g_c191 != 0)
                    g_c191 += 4;
                r1a = 0x64;
                if (g_c191 != 0)
                    g_c197 -= 0x100;
            }
            if (type == 0xe) {
                r1b = 0x64;
                r1c = 0x62;
                if (g_c1f4 < 1 && (g_c199 == 0x800 || g_c199 == 0))
                    g_c199 = 0x660;
            }
            if (type == 0x40) {
                r1b = 0x60;
                r1c = 0x62;
                g_c187 = g_c1af;
                g_c1a7 = g_c1b1;
                g_c1a9 = g_c1b3;
                g_c19f = g_c1b5;
            }
            if (type == 3 && name[0] == 'M' && name[1] == 'a' && name[2] == 'x')
                f11 = 1;
            if (type == 0xb && memcmp(name, "Orp", 3) == 0) {
                f12 = 2;
                if (g_c1e6 == -1)
                    g_c1e6 = 0;
            }
            if (type == 0xb && memcmp(name, "Tur", 3) == 0 && f12 != 1)
                f12 = 3;
            if (type == 2 && f14 == 3) {
                r20 = 0x75;
                r21 = 0x74;
            }
            if (type == 0xb && (f12 == 1 || f12 == 4) && g_c195 == 0x120)
                g_c195 = g_c191 + 4;
            if (type == 8 && memcmp(name, "AD-", 3) == 0)
                f15 = 1;
            if (type == 5 && f15 == 8 && memcmp(name, "Zol", 3) == 0)
                g_c1e6 = 8;
            if (((type == 0xe && g_c1f4 == 0) || (type == 3 && g_c1f4 == 1)) && !g_c1db)
                f_7d59();
            f_3c59(3, lcsn);
            delay(1);
            if (ld.ld[0] >= 0) {
                f_3c59(7, ld.ld[0]);
                f_3c93(r1a, g_c187);
                if (g_c1db != 0)
                    g_c187 = f_3ccf(r1a);
                printf("ADD:%X", g_c187);
                if (type == 0x40) {
                    int o1 = 0x100;
                    int o2 = 0x10c;
                    if (g_c1dc != 0) {
                        o1 = 0;
                        o2 = 0;
                    }
                    f_3c93(0x62, g_c187 + o1);
                    f_3c93(0x64, g_c187 + o2);
                    if (g_c1db != 0)
                        g_c187 = f_3ccf(0x64) - o2;
                    printf(" CODEC:%X", g_c187 + o2);
                }
                if (type == 4 || type == 7 || type == 9 || type == 0xa || type == 0xc ||
                    type == 0xb || type == 0x10) {
                    f_3c93(r1d, g_c191);
                    if (g_c1db != 0)
                        g_c191 = f_3ccf(r1d);
                    printf(" WSS:%X", g_c191);
                }
                if (type == 1 || type == 3 || type == 4 || type == 5 || type == 7 ||
                    type == 0x10 || type == 9 || type == 0xa || type == 0xc || type == 0xb ||
                    type == 0xd || type == 0xe) {
                    f_3c93(r1c, g_c189);
                    if (g_c1db != 0) {
                        g_c189 = f_3ccf(r1c);
                        if (type == 0xd && g_c189 == 0 && (f_3c81(r20) & 7) != 4)
                            g_c189 = 0x388;
                    }
                    if (type == 1 && f18 == 2)
                        printf(" CQM:%X", g_c189);
                    else
                        printf(" OPL:%X", g_c189);
                    if (type == 0xb)
                        f_3c59(r1f, 0);
                }
                if (type == 1 || type == 3 || type == 4 || type == 0xd || type == 0xe) {
                    if (type == 3 && f11 == 1) {
                        f_3c93(r1b, g_c19b);
                        if (g_c1db != 0)
                            g_c19b = f_3ccf(r1b);
                        printf(" MPU2:%X", g_c19b);
                    } else {
                        f_3c93(r1b, g_c18d);
                        if (g_c1db != 0) {
                            g_c18d = f_3ccf(r1b);
                            if (type == 0xd && g_c18d == 0 && (f_3c81(r20) & 7) != 4)
                                g_c18d = 0x330;
                        }
                        printf(" MPU:%X", g_c18d);
                    }
                }
                if (type == 4) {
                    f_3c93(0x68, g_c193);
                    if (g_c1db != 0)
                        g_c193 = f_3ccf(0x68);
                    printf(" CTR:%X", g_c193);
                }
                if (type == 7 || type == 0xc) {
                    int o = 1;
                    if (g_c1dc != 0)
                        o = 0;
                    f_3c93(0x66, g_c197 + o);
                    if (g_c1db != 0)
                        g_c197 = f_3ccf(0x66) - o;
                    printf(" CTR:%X", g_c197 + o);
                }
                if (type == 4 || type == 0x40)
                    g_c1a1 = 0;
                else if (type != 8)
                    g_c1a1 = g_c19f;
                f_3c59(r1e, g_c19f);
                if (type != 4 && type != 7 && type != 0x10 && type != 0x40 && type != 0xc &&
                    type != 8)
                    f_3c59(r1e + 1, 2);
                if (type != 6 && type != 7 && type != 0x10 && type != 9 && type != 0x13 &&
                    type != 0xc && type != 0xb && type != 0xd && type != 8)
                    f_3c59(r1f, g_c1a1);
                if (type != 4 && type != 6 && type != 7 && type != 0x10 && type != 8 &&
                    type != 0x40 && type != 9 && type != 0xc && type != 0xd && type != 0x13)
                    f_3c59(r1f + 1, 2);
                if (g_c1db != 0)
                    g_c19f = f_3c81(r1e) & 0xf;
                if (g_c1db != 0)
                    g_c1a1 = f_3c81(r1f) & 0xf;
                printf(" IRQ:%d", g_c19f);
                if ((type == 4 && g_c1a1 != 0) || (type == 4 && g_c1db != 0))
                    printf("/%d", g_c1a1);
                if (g_c1a7 == g_c1a9 && g_c1dc == 0) {
                    if (type == 4 || type == 0xa) {
                        g_c1a7 = 1;
                        g_c1a9 = 3;
                    }
                    if (type == 8)
                        g_c1a9 = 5;
                    if (type == 2 && f14 == 3)
                        g_c1a9 = 5;
                    if (type == 3 || type == 0xe)
                        g_c1a9 = 0;
                }
                if (type == 0xb && g_c1a9 > 3)
                    g_c1a9 = 4;
                if (type == 7 || type == 0xc || type == 0x10) {
                    if (g_c1a7 == g_c1a9 || g_c1a9 > 4)
                        g_c1a9 = 4;
                    f_3c59(r21, g_c1a9);
                    if (g_c1db != 0)
                        g_c1a9 = f_3c81(r21) & 7;
                }
                f_3c59(r20, g_c1a7);
                if (g_c1db != 0)
                    g_c1a7 = f_3c81(r20) & 7;
                printf(" DMA:%d", g_c1a7);
                if (type != 6 && type != 0xd && f14 != 1 && f14 != 2 &&
                    (g_c1a9 != 4 || (g_c1dc != 0 && g_c1a9 == 4) || (type == 8 && g_c1a9 == 4))) {
                    f_3c59(r21, g_c1a9);
                    if (g_c1db != 0)
                        g_c1a9 = f_3c81(r21) & 7;
                    printf("/%d", g_c1a9);
                }
                f_3c59(0x30, g_c1dc != 0 ? 0 : 1);
            }
            if (ld.ld[1] >= 0) {
                f_3c59(7, ld.ld[1]);
                if (type == 2 || type == 6 || (type == 8 && f15 < 1)) {
                    f_3c93(r1c, g_c189);
                    if (g_c1db != 0)
                        g_c189 = f_3ccf(r1c);
                    printf(" OPL:%X", g_c189);
                }
                if (type == 8 || type == 0x13) {
                    unsigned char irq = g_c1a1;
                    unsigned char dma = g_c1ab;
                    if (g_c1dc != 0) {
                        irq = 0;
                        dma = 4;
                    }
                    f_3c93(r1d, g_c191);
                    f_3c59(r1e, irq);
                    f_3c59(r1e + 1, 2);
                    f_3c59(r20, dma);
                    if (type == 0x13) {
                        f_3c59(r21, dma);
                        if (g_c1db != 0)
                            dma = f_3c81(r21) & 7;
                    }
                    if (g_c1db != 0)
                        g_c191 = f_3ccf(r1d);
                    if (g_c1db != 0)
                        irq = f_3c81(r1e) & 0xf;
                    if (g_c1db != 0)
                        dma = f_3c81(r20) & 7;
                    if (g_c1db != 0 && type == 0x13)
                        dma = f_3c81(r21) & 7;
                    printf(" WSS:%X/I%d/D%d", g_c191, irq, dma);
                }
                if (type == 0xb && f12 == 4) {
                    f_3c93(r1b, g_c19b);
                    if (g_c1db != 0)
                        g_c19b = f_3ccf(r1b);
                    printf(" MPU2:%X", g_c19b);
                }
                if (g_c1dc != 0)
                    f_3c59(0x30, 0);
                if (type == 0x40) {
                    f_3c93(0x60, 0);
                    f_3c59(r1e, 0);
                    f_3c59(0x30, 0);
                    if (g_c1db != 0) {
                        int p = f_3ccf(0x60);
                        unsigned char q = f_3c81(r1e) & 0xf;
                        printf(" SB:%X/I%d", p, q);
                    }
                } else
                    f_3c59(0x30, g_c1dc != 0 ? 0 : 1);
            }
            if (ld.ld[2] >= 0) {
                unsigned char en = 1;
                unsigned char irq = g_c19f;
                irq = g_c1a3 < 0 ? g_c19f : g_c1a3;
                f_3c59(7, ld.ld[2]);
                if (type == 2 || type == 5 || type == 6 || type == 7 || type == 0x10 ||
                    type == 8 || (type == 3 && f11 == 1) || type == 9 || type == 0x13 ||
                    type == 0xa || type == 0xc || type == 0xb || type == 0x2c || type == 0x3b) {
                    if (type == 0xb && g_c1ea == 0)
                        g_c18d = 0;
                    f_3c93(0x60, g_c18d);
                    if (g_c1a3 < 0) {
                        if (type == 2 || (type == 3 && f11 == 1)) {
                            irq = 2;
                            if (f14 > 1)
                                irq = 9;
                        }
                        if (type == 7 || type == 8 || type == 9 || type == 0x13 || type == 0xa ||
                            type == 0xc || type == 0x10 || type == 0xb || type == 0x2c ||
                            type == 0x3b) {
                            irq = 9;
                            en = 1;
                        }
                        if (type == 6)
                            irq = 7;
                    } else
                        irq = g_c1a3;
                }
                if (g_c18d == 0) {
                    irq = 0;
                    en = 0;
                }
                if (type == 0x40) {
                    f_3c93(0x60, 0);
                    f_3c59(0x70, 0);
                    en = 0;
                } else {
                    f_3c59(0x70, irq);
                    f_3c59(0x71, 2);
                }
                if (g_c1db != 0)
                    g_c18d = f_3ccf(r1b);
                if (g_c1db != 0)
                    irq = f_3c81(0x70) & 0xf;
                f_3c59(0x30, (!en || g_c1dc != 0) ? 0 : 1);
                if (en != 0 || g_c1db != 0)
                    printf(" MPU:%X/I%d", g_c18d, irq);
            }
            if (ld.ld[6] >= 0) {
                f_3c59(7, ld.ld[6]);
                if (type == 3 || type == 0xe) {
                    if ((type != 0xe && g_c1f4 == 0) || (type == 0xe && g_c1f4 == 1))
                        f_3c93(0x60, g_c199);
                    if (g_c1db != 0)
                        g_c199 = f_3ccf(0x60);
                    printf(" CTR:%X", g_c199);
                }
                if (type == 0xb || (type == 8 && f15 == 1)) {
                    f_3c93(0x60, g_c195);
                    if (g_c1db != 0)
                        g_c195 = f_3ccf(0x60);
                    printf(" CTR:%X", g_c195);
                }
                if (type == 5) {
                    f_3c93(0x60, g_c1ad);
                    if (g_c1db != 0)
                        g_c1ad = f_3ccf(0x60);
                    printf(" CTR:%X", g_c1ad);
                }
                if (type == 0x40) {
                    int p = 0x350;
                    if (g_c1ad >= 0x300)
                        p = g_c1ad;
                    else
                        g_c1ad = p;
                    f_3c93(0x60, p);
                    if (g_c1db != 0)
                        p = f_3ccf(0x60);
                    printf(" CTR:%X", p);
                }
                if (type == 0x37) {
                    int p = 0x2180;
                    if (g_c1ad >= 0x2100)
                        p = g_c1ad;
                    f_3c93(0x62, g_c191);
                    f_3c59(0x70, g_c19f);
                    if (g_c1db != 0)
                        g_c191 = f_3ccf(0x62);
                    if (g_c1db != 0)
                        g_c19f = f_3ccf(0x70) & 0xf;
                    printf(" WSS:%X/I%d", g_c191, g_c19f);
                    if (g_c1a3 < 0)
                        g_c1a3 = g_c19f;
                    if (g_c1db != 0)
                        g_c18d = f_3ccf(0x60);
                    if (g_c1db != 0)
                        g_c1a3 = f_3ccf(0x72) & 0xf;
                    printf(" MPU:%X/I%d", g_c18d, g_c1a3);
                    f_3c59(0x74, g_c1a7);
                    f_3c59(0x75, g_c1a9);
                    if (g_c1db != 0)
                        g_c1a7 = f_3ccf(0x74) & 7;
                    if (g_c1db != 0)
                        g_c1a9 = f_3ccf(0x75) & 7;
                    printf(" DMA:%d/%d", g_c1a7, g_c1a9);
                    f_3c93(0x64, p);
                    if (g_c1db != 0)
                        p = f_3ccf(0x64);
                    printf(" CTR:%X", p);
                }
                if (type == 0x10) {
                    int o = 1;
                    if (g_c1dc != 0)
                        o = 0;
                    f_3c93(0x60, g_c197 + o);
                    if (g_c1db != 0)
                        g_c197 = f_3ccf(0x60) - o;
                    printf(" CTR:%X", g_c197 + o);
                    f_3c59(0x70, 0);
                }
                f_3c59(0x30, (g_c1dc != 0 || type == 0x10) ? 0 : 1);
            }
            if (ld.ld[3] >= 0) {
                if (g_c18f == 0x299) {
                    if (type == 4 || type == 5 || type == 7 || type == 0x10 || type == 6 ||
                        type == 0xd || type == 0xe || type == 9 || type == 0x13 || type == 3)
                        g_c18f = 0x201;
                    else
                        g_c18f = 0x200;
                }
                if (type == 0x40) {
                    if (g_c1f0 == 1)
                        g_c18f = 0x201;
                    else
                        g_c18f = 0;
                }
                f_3c59(7, ld.ld[3]);
                f_3c93(0x60, g_c18f);
                f_3c59(0x30, (g_c18f == 0 || g_c1dc != 0) ? 0 : 1);
                if (g_c1db != 0 || g_c18f > 0) {
                    if (g_c1db != 0)
                        g_c18f = f_3ccf(0x60);
                    printf(" JOY:%X", g_c18f);
                }
            }
            if (ld.ld[4] >= 0) {
                f_3c59(7, ld.ld[4]);
                if (type == 1) {
                    f_3c93(0x60, g_c18b);
                    if (g_c1db != 0)
                        g_c18b = f_3ccf(0x60);
                    printf(" AWE:%X", g_c18b);
                    f_3c93(0x62, g_c18b + 0x400);
                    f_3c93(0x64, g_c18b + 0x800);
                    if (f18 == 2 && g_c189 != 0x388 && f_850e() == 1)
                        printf(" OPL3 detected:388");
                    if (ld.ld[0] >= 0)
                        f18 = 1;
                    else
                        f18 = 3;
                }
                f_3c59(0x30, g_c1dc != 0 ? 0 : 1);
            }
            if (ld.ld[5] >= 0) {
                unsigned char irq = 0;
                unsigned char rp = 0x60;
                unsigned char rp2 = 0x62;
                if (type == 7) {
                    rp = 0x62;
                    rp2 = 0x63;
                }
                if (type == 0x10) {
                    rp = 0x62;
                    rp2 = 0x60;
                }
                if (type == 0x40 && g_c1ec > 0)
                    g_c19d = g_c1ec;
                if (g_c19d == 0x168)
                    irq = 0xa;
                else if (g_c19d == 0x1e8)
                    irq = 0xb;
                else if (g_c19d == 0x170)
                    irq = 0xf;
                else if (g_c19d == 0)
                    irq = 0;
                if (g_c1a5 > 0 && g_c19d > 0)
                    irq = g_c1a5;
                f_3c59(7, ld.ld[5]);
                f_3c93(rp, g_c19d);
                f_3c59(0x70, irq);
                if (type == 1 || type == 0x40 || type == 2 || type == 7 || type == 0xc ||
                    type == 4 || type == 0xa || type == 0xe || type == 0xb || type == 0x10) {
                    f_3c93(rp2, g_c19d + (irq > 0) * 0x206);
                    f_3c59(0x71, 2);
                }
                if (type == 0x40)
                    f_3c59(0x74, 0);
                f_3c59(0x30, (g_c19d == 0 || g_c1dc != 0) ? 0 : 1);
                if (g_c1db != 0 || g_c19d > 0) {
                    if (g_c1db != 0)
                        g_c19d = f_3ccf(0x60);
                    printf(" IDE:%X", g_c19d);
                    if (g_c1db != 0)
                        irq = f_3c81(0x70) & 0xf;
                    printf("/I%d", irq);
                }
            }
            if (ld.ld[7] >= 0) {
                f_3c59(7, ld.ld[7]);
                f_3c59(0x30, (type == 4 || type == 0xa || g_c1dc != 0) ? 0 : 1);
                if (type == 4 || type == 0xa) {
                    f_3c93(0x60, 0);
                    f_3c59(0x70, 0);
                }
            }
            if (ld.ld[8] >= 0) {
                f_3c59(7, ld.ld[8]);
                if (type == 0xb && f12 == 4) {
                    int p = 0x328;
                    if (g_c19b > 0)
                        p = g_c19b + 8;
                    f_3c93(0x60, p);
                    if (g_c1db != 0)
                        g_c18d = f_3ccf(0x60);
                }
                if (type == 0x13) {
                    f_3c93(r1c, g_c189);
                    if (g_c1db != 0)
                        g_c189 = f_3ccf(r1c);
                    printf(" OPL:%X", g_c189);
                }
                f_3c59(0x30, (type == 0x63 || type == 0xa || g_c1dc != 0) ? 0 : 1);
            }
            if (type == 7 || type == 0x10) {
                delay(10);
                f_3c59(7, 5);
                f_3c59(0x70, 0);
                f_3c59(0x30, 1);
            }
            printf("\n");
            if (type == 0x13) {
                if (g_c1f2 != 0) {
                    f_3c59(0x20, 8);
                    delay(0x78);
                    outportb(g_c191, 4);
                    _AL = (100 - g_c1c3) * 0x1f / 0x61;
                    outportb(g_c191 + 2, _AL);
                    outportb(g_c191, 5);
                    _AL = (100 - g_c1c3) * 0x1f / 0x61;
                    outportb(g_c191 + 2, _AL);
                } else {
                    f_3c59(0x20, 0);
                    delay(0x50);
                }
            }
            f_3c59(3, 0);
            delay(1);
            if (g_c1db != 0) {
                f_3c59(2, 2);
                return 0;
            }
            if ((ld.ld[4] < 0 || type != 1) && g_c1dc == 0) {
                printf("Initialization done. ");
                if (type != 1)
                    puts("");
            }
            if (g_c1dc != 0)
                puts("Card disabled.");
        } else {
            type = 0;
            printf("No supported PnP card detected.");
            if (f19 < 1)
                ret = !f_4e94();
        }
    } else {
        type = 0;
        if (b != 1)
            printf("No PnP card detected.");
        if (f19 < 1 && b != 1)
            ret = !f_4e94();
    }
    f_3c59(2, 2);
    delay(1);
    f_3c59(2, 2);
    delay(1);
    if (g_c1dc != 0)
        return 0;
    if (type == 4) {
        int p = g_c191 + 4;
        if (g_c1d5 < 0)
            g_c1d5 = 0;
        if (g_c1d8 < 0)
            g_c1d8 = 0;
        f_3d19(g_c193, 0xa, 0x80);
        f_3d19(g_c193, 1, 0);
        f_3d19(g_c193, 0x10, 0);
        f_3d19(g_c193, 0x12, 0);
        f_3d19(g_c193, 0x13, 0);
        f_3d19(g_c193, 0x17, 0);
        f_3d19(g_c193, 3, 3);
        f_3d19(g_c193, 6, 0x34);
        f_3d19(g_c193, 0x14, (g_c1f8 | g_c1f8 << 4) & 0x77);
        f_3d19(g_c193, 0x15, g_c1d8 * 7 / 0x61 | (g_c1d8 * 7 / 0x61) << 4);
        f_3d19(g_c193, 0x16, g_c1d5 * 7 / 0x61 | (g_c1d5 * 7 / 0x61) << 4);
        outportb(g_c187 + 6, 1);
        delay(4);
        outportb(g_c187 + 6, 0);
        inportb(g_c187 + 0xe);
        delay(1);
        inportb(g_c187 + 0xe);
        inportb(g_c187 + 0xa);
        delay(1);
        inportb(g_c187 + 0xa);
        f_3cfd(g_c187, 0x22, g_c1bf);
        f_3cfd(g_c187, 0xe, 2);
        f_3d19(g_c193, 2, 0x30);
        f_3cfd(g_c187, 0, 0xee);
        f_3cfd(g_c187, 0x22, 0x11);
        f_54e9();
        if (g_c1bf > 0)
            g_c1bf = (100 - g_c1c0) * 0xf / 0x61;
        else
            g_c1bf = 0x80;
        if (g_c1c2 > 0)
            g_c1c2 = (100 - g_c1c3) * 0x1f / 0x61;
        else
            g_c1c2 = 0x88;
        if (g_c1c5 > 0)
            g_c1c5 = (100 - g_c1c6) * 0x3f / 0x61;
        else
            g_c1c5 = 0x80;
        if (g_c1c8 > 0)
            g_c1c8 = (100 - g_c1c9) * 0x1f / 0x61;
        else
            g_c1c8 = 0x88;
        if (g_c1cb > 0)
            g_c1cb = (100 - g_c1cc) * 0x1f / 0x61;
        else
            g_c1cb = 0x88;
        if (g_c1ce > 0)
            g_c1ce = (100 - g_c1cf) * 0x1f / 0x61;
        else
            g_c1ce = 0x88;
        f_3d19(p, 0xc, 0x40);
        f_3d19(p, 4, g_c1c2);
        f_3d19(p, 5, g_c1c2);
        f_3d19(p, 6, g_c1c5);
        f_3d19(p, 7, g_c1c5);
        f_3d19(p, 0x12, g_c1c8);
        f_3d19(p, 0x13, g_c1c8);
        f_3d19(p, 2, g_c1cb);
        f_3d19(p, 3, g_c1cb);
        f_3d19(g_c193, 9, g_c1ce);
        f_3d19(g_c193, 7, g_c1bf);
        f_3d19(g_c193, 8, g_c1bf);
        f_5419("Yamaha", g_c1f8 & 7, -1);
    }
    if (type == 1) {
        if (f18 > 0 && g_c18b > 0) {
            delay(0x28);
            outportb(g_c187 + 4, 0xfe);
            outportb(g_c187 + 4, 0xfd);
            delay(1);
            f_4c96(g_c18b);
            delay(1);
            outportb(g_c187 + 0xc, 0xdd);
        }
        if (f18 < 3) {
            f_4dec(g_c187);
            f_556d();
            f_5419("SB", g_c1f8 > 0 ? 1 : -1, g_c1fa);
        }
    }
    if (type == 2) {
        int x = g_c1f8;
        if (f14 == 1 || f14 == 2) {
            f_54e9();
            g_c1f8 = -1;
        } else if (f14 != 3) {
            int y = g_c1f8 > 0 ? 0xc4 : 0x84;
            f_556d();
            f_3cfd(g_c187, 0x50, y);
            y = g_c1f8 > 0 ? 0 : 0xa;
            f_3cfd(g_c187, 0x51, y);
        } else {
            f_556d();
            g_c1f8 = -1;
        }
        if (g_c1f8 > 0)
            x = 1;
        else
            x = g_c1f8;
        if (f14 != 2)
            f_5419("ALS", x, -1);
        else
            f_5419("Diamond", -1, -1);
    }
    if (type == 3 && f11 == 1)
        f_5de3(g_c18d);
    if (type == 3 || type == 6 || type == 0xd || type == 0xe) {
        int x = -1;
        f_54e9();
        f_3cfd(g_c187, 0x3a, g_c1c2);
        outportb(g_c18d + 1, 0xff);
        delay(5);
        outportb(g_c18d + 1, 0x3f);
        delay(5);
        outportb(g_c18d, 0xf0);
        delay(1);
        outportb(g_c18d, 0);
        delay(1);
        outportb(g_c18d, 0);
        delay(1);
        outportb(g_c18d, 0x7b);
        delay(1);
        outportb(g_c18d, 1);
        delay(1);
        outportb(g_c18d, 0xf7);
        delay(1);
        outportb(g_c18d, 0xf0);
        delay(1);
        outportb(g_c18d, 0);
        delay(1);
        outportb(g_c18d, 0);
        delay(1);
        outportb(g_c18d, 0x7b);
        delay(1);
        outportb(g_c18d, 4);
        delay(1);
        outportb(g_c18d, 0xf7);
        delay(1);
        f_3cfd(g_c187, 0x48, 0x10);
        f_3cfd(g_c187, 0x36, g_c1c2);
        outportb(g_c18d + 1, 0xff);
        delay(5);
        if (g_c1d8 > -1)
            f_3cfd(g_c187, 0x3a, g_c1d8 * 0xf / 0x5f * 0x11);
        if (type == 0xe) {
            int a = g_c1f8 > 0 ? 0xc : 0;
            int b = g_c1f8 * 0x3f / 9;
            f_3cfd(g_c187, 0x52, b);
            f_3cfd(g_c187, 0x50, a);
            x = g_c1f8;
        }
        f_5419("ESS", x, -1);
    }
    if (type == 5) {
        int v = g_c1c3 * 0x1f / 0x61;
        f_54e9();
        if (g_c1e6 == 8) {
            outpw(g_c1ad, 0xc00e);
            inportb(g_c1ad + 5);
            delay(200);
            outp(g_c1ad, 0x15);
            inportb(g_c1ad + 5);
            delay(200);
            f_3cfd(g_c187, 0x28, g_c1c2);
        }
        f_3cfd(g_c187, 0xe, 0x22);
        v = v << 6;
        outpw(g_c1ad, 0x32);
        inportb(g_c1ad + 5);
        delay(300);
        outpw(g_c1ad, 0x282e | v);
        inportb(g_c1ad + 5);
        delay(300);
        outpw(g_c1ad, 0x682e | v);
        inportb(g_c1ad + 5);
        f_5419("Rockwell", -1, -1);
    }
    if (type == 8) {
        int a = g_c1f8 > 0 ? 0x20 : 0;
        int b = g_c1fa > 0 ? 0 : 0x40;
        int c = g_c1fa == 9 ? 0 : 0x40;
        int d = g_c1fa;
        f_3cfd(g_c191, 0xc, 0x4a);
        f_3cfd(g_c191, 0x10, c | a);
        f_3cfd(g_c191, 0x11, 0x3f | b);
        f_3cfd(g_c191, 0x17, 3);
        f_3cfd(g_c191, 0x16, g_c1c2);
        f_3cfd(g_c191, 0x14, g_c1c8);
        f_3cfd(g_c191, 0x18, 0xff);
        f_3cfd(g_c191, 0xc, 0xa);
        f_5175(g_c187, g_c19f, g_c1a7, g_c1a9);
        f_556d();
        if (f15 == 1) {
            if (g_c1c9 == 0)
                g_c1c8 = 0;
            else if (g_c1c9 > 0 && g_c1c9 < 0x32)
                g_c1c8 = 0x60;
            else if (g_c1c9 > 0x31)
                g_c1c8 = 0x66;
            if (g_c1cf == 0)
                g_c1c8 &= 0xee;
            else if (g_c1cf > 0 && g_c1cf < 0x32)
                g_c1c8 |= 0x10;
            else if (g_c1cf > 0x31)
                g_c1c8 |= 0x11;
            f_3cfd(g_c191, 4, (100 - g_c1c3) * 0x1f / 0x61);
            f_3cfd(g_c191, 5, (100 - g_c1c3) * 0x1f / 0x61);
            f_3cfd(g_c191, 6, (100 - g_c1c6) * 0x1f / 0x61);
            f_3cfd(g_c191, 7, (100 - g_c1c6) * 0x1f / 0x61);
            outportb(g_c195, g_c1c8);
        }
        f_5419("CMI", g_c1f8 > 0 ? 1 : 0, d > 0 ? 1 : 0);
    }
    if (type == 9) {
        unsigned char v = (100 - g_c1c3) * 0x3f / 0x61;
        int w = g_c1f8 * 0xf / 9 << 9;
        f_54e9();
        outportb(g_c191, 0x27);
        outpw(g_c191 + 2, w);
        outportb(g_c191, 0x2c);
        outpw(g_c191 + 2, 0);
        if (g_c1c3 > 2) {
            outportb(g_c191, 0x10);
            outpw(g_c191 + 2, v << 8 | v);
            outportb(g_c191, 0x23);
            outpw(g_c191 + 2, 0xac44);
            outportb(g_c191, 0x24);
            outpw(g_c191 + 2, 0xac44);
            outportb(g_c191, 0x20);
            outpw(g_c191 + 2, 0xf5);
            outportb(g_c191, 6);
            outpw(g_c191 + 2, v << 8 | v);
            outportb(g_c191, 7);
            outpw(g_c191 + 2, v << 8 | v);
        } else {
            outportb(g_c191, 6);
            outpw(g_c191 + 2, 0x8080);
            outportb(g_c191, 7);
            outpw(g_c191 + 2, 0x8080);
            outportb(g_c191, 0x20);
            outpw(g_c191 + 2, 0xf0);
            outportb(g_c191, 0x10);
            outpw(g_c191 + 2, 0x8888);
        }
        f_5419("ADI", g_c1f8, -1);
    }
    if (type == 0x13) {
        if (g_c1f2 == 0)
            f_54e9();
        f_5419("ADI", -1, -1);
    }
    if (type == 0xa) {
        f_54e9();
        f_3cfd(g_c187, 0xe, 0x22);
        f_5419("Aztech", -1, -1);
        printf("Card mode: ");
        if (g_c1f2 > 0) {
            outportb(g_c187 + 0xc, 9);
            delay(10);
            outportb(g_c187 + 0xc, 0x80);
            puts("[WSS]");
        } else {
            outportb(g_c187 + 0xc, 9);
            delay(10);
            outportb(g_c187 + 0xc, 0x81);
            puts("[SBPro]");
        }
    }
    if (type == 7 || type == 0x10) {
        delay(0x14);
        f_577f(type == 7 ? 5 : 4);
    }
    if (type == 0xc) {
        delay(0x14);
        f_5c48();
        f_5419("OPTi", g_c1f8 > 0 ? 1 : -1, -1);
    }
    if (type == 0xb) {
        int x = -1;
        int y = g_c1f8 * 0xf / 9 | ~(g_c1f8 * 0xf / 9) << 4;
        if (f12 == 1)
            f_7de9();
        if (f12 == 4)
            puts("EWS64 firmware not supported. Output will be silent.");
        outportb(g_c187 + 6, 1);
        delay(1);
        outportb(g_c187 + 6, 0);
        delay(0x14);
        f_54e9();
        delay(10);
        f_3cfd(g_c187, 0x22, 0);
        f_3cfd(g_c187, 4, 0);
        if (f12 == 2 || f12 == 3 || f12 == 7) {
            f_7ed9(f12);
            delay(10);
            if (g_c1f8 < 1)
                y = 0;
            f_8474(2, y);
            if (f12 == 7)
                f_8474(3, g_c1f8 > 0 ? 0x80 : 0);
            x = g_c1f8;
        }
        if (g_c1a7 == g_c1a9) {
            f_3d19(g_c191, 0x49, 4);
            f_3d19(g_c191, 0x4a, 6);
            delay(5);
        }
        delay(5);
        f_54e9();
        if (f12 == 1 && g_c1cf > 0) {
            f_3d19(g_c191, 0x40, (g_c1ce & 0xf) + 0x80);
            f_3d19(g_c191, 0x41, (g_c1ce & 0xf) + 0x80);
            f_3d19(g_c191, 0x46, 2);
            f_3d19(g_c191, 0x47, 2);
        }
        if (f12 == 2 || (f12 == 3 && g_c1e8 == 1)) {
            int i = 0;
            f_3d19(g_c191, 0xc, 0xea);
            if (g_c1c3 > 0)
                for (i = 0; i < 2; i++)
                    f_84e1(i, 0x20 | (100 - g_c1c3) * 0x1f / 0x61);
            else
                for (i = 0; i < 2; i++)
                    f_84e1(i, 0xff);
        }
        if (f12 != 4)
            f_5419("Crystal", x, -1);
    }
    if (type == 0x37) {
        f_3cfd(g_c191, 6, (100 - g_c1c6) * 0x3f / 0x61);
        f_3cfd(g_c191, 7, (100 - g_c1c6) * 0x3f / 0x61);
        f_3cfd(g_c191, 2, (100 - g_c1c9) * 0x1f / 0x61);
        f_3cfd(g_c191, 3, (100 - g_c1c9) * 0x1f / 0x61);
        f_3cfd(g_c191, 4, (100 - g_c1cc) * 0x1f / 0x61);
        f_3cfd(g_c191, 5, (100 - g_c1cc) * 0x1f / 0x61);
        printf("Ensoniq WSS Mixer [PCM:%d LIN:%d CD:%d]", g_c1c6, g_c1c9, g_c1cc);
    }
    if (type == 0x3b) {
        outportb(g_c18d + 1, 0xff);
        delay(5);
    }
    if (type == 0x40) {
        delay(10);
        g_c1bf = (100 - g_c1c0) * 0x1f / 0x61;
        g_c1c2 = (100 - g_c1c3) * 0x1f / 0x61;
        g_c1c5 = (100 - g_c1c6) * 0x3f / 0x61;
        g_c1c8 = (100 - g_c1c9) * 0x1f / 0x61;
        g_c1cb = (100 - g_c1cc) * 0x1f / 0x61;
        g_c1ce = (100 - g_c1cf) * 0x1f / 0x61;
        f_6a91();
        printf("InterWave Mixer [VOL:%d DAC:%d SYNTH:%d LIN:%d CD:%d MIC:%d]\n",
               g_c1c0, g_c1c6, g_c1c3, g_c1c9, g_c1cc, g_c1cf);
        if (g_c1ad >= 0x300) {
            outportb(g_c1ad, 0);
            if (inportb(g_c1ad) == 0xfd) {
                if (g_c1d5 < 0)
                    g_c1d5 = 0x32;
                if (g_c1d8 < 0)
                    g_c1d8 = 0x32;
                f_86aa(5, 0xc0);
                f_86aa(3, g_c1d5 * 0xf / 0x60);
                f_86aa(2, g_c1d8 * 0xf / 0x60);
                f_86aa(5, 0x40);
                f_86aa(0, 0x35);
                f_86aa(1, 0x35);
                f_86aa(4, g_c1d2);
                printf("TEA6330T Init done. [TREB:%d BASS:%d]", g_c1d5, g_c1d8);
            }
        }
    }
    printf("\n");
    if (g_c1f6 > 0)
        f_85dd(g_c1bd);
    return ret;
}

/* ======== chunk 3 ======== */
#define g_c1de (*(unsigned char *)&g_c1de)
#define g_c187 (*(unsigned *)&g_c187)
#define g_c189 (*(unsigned *)&g_c189)
#define g_c18b (*(unsigned *)&g_c18b)
#define g_c18d (*(unsigned *)&g_c18d)
#define g_c18f (*(unsigned *)&g_c18f)
#define g_c191 (*(unsigned *)&g_c191)
#define g_c193 (*(unsigned *)&g_c193)
#define g_c199 (*(unsigned *)&g_c199)
#define g_c19b (*(unsigned *)&g_c19b)
#define g_c19d (*(unsigned *)&g_c19d)
#define g_c1ad (*(unsigned *)&g_c1ad)
#define g_c1af (*(unsigned *)&g_c1af)
#define g_c1bd (*(unsigned *)&g_c1bd)
/* PnP initiation key */
void f_3c21(void)
{
    unsigned char key[34] = {
        0x00, 0x00, 0x6a, 0xb5, 0xda, 0xed, 0xf6, 0xfb, 0x7d, 0xbe, 0xdf, 0x6f,
        0x37, 0x1b, 0x0d, 0x86, 0xc3, 0x61, 0xb0, 0x58, 0x2c, 0x16, 0x8b, 0x45,
        0xa2, 0xd1, 0xe8, 0x74, 0x3a, 0x9d, 0xce, 0xe7, 0x73, 0x39 };
    int i = 0;
    for (i = 0; i < 34; i++)
        outportb(g_c1b9, key[i]);
    delay(1);
}

/* PnP register write (skipped for 0x30..0x80 when g_c1db set) */
void f_3c59(unsigned char reg, unsigned char val)
{
    if (g_c1db == 0 || reg < 0x30 || reg > 0x80) {
        outportb(g_c1b9, reg);
        outportb(g_c1bb, val);
    }
}

/* PnP register read */
unsigned char f_3c81(unsigned char reg)
{
    outportb(g_c1b9, reg);
    return inportb(g_c1bd);
}

/* PnP 16-bit register write (high byte first) */
void f_3c93(unsigned char reg, unsigned val)
{
    if (g_c1db == 0 || reg < 0x60 || reg > 0x80) {
        f_3c59(reg, (val >> 8) & 0xff);
        f_3c59(reg + 1, val & 0xff);
    }
}

/* PnP 16-bit register read (high byte first) */
unsigned f_3ccf(unsigned char reg)
{
    unsigned v = 0;
    v = (f_3c81(reg) << 8) & 0xff00;
    return v + f_3c81(reg + 1);
}

/* indexed write: index at port+4, data at port+5 */
void f_3cfd(unsigned port, unsigned char idx, unsigned char val)
{
    register unsigned char a, b;    /* unused, but they reserve BX/CX */
    outportb(port + 4, idx);
    outportb(port + 5, val);
}

/* indexed write: index at port, data at port+1, with delays */
void f_3d19(unsigned port, unsigned char idx, unsigned char val)
{
    outportb(port, idx);
    delay(1);
    outportb(port + 1, val);
    delay(1);
}

/* indexed write: index at port+4, data at port+5, with delays */
void f_3d40(unsigned port, unsigned char idx, unsigned char val)
{
    outportb(port + 4, idx);
    delay(0x11);
    outportb(port + 5, val);
    delay(0x10);
}

/* banked register read (unlock 0xe5 at port+3; regs >7 via port+8/port+9) */
unsigned char f_3d6c(unsigned port, unsigned char reg)
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

/* banked register write (unlock 0xe5 at port+3; regs >7 via port+8/port+9) */
void f_3daa(unsigned port, unsigned char reg, unsigned char val)
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

/* unlocked indexed write (0xe4 to g_c1e4 before each access) */
void f_3def(unsigned port, unsigned char idx, unsigned char val)
{
    outportb(g_c1e4, 0xe4);
    outportb(port, idx);
    outportb(g_c1e4, 0xe4);
    outportb(port + 1, val);
    delay(2);
}

/* unlocked indexed read (0xe4 to g_c1e4 before each access) */
unsigned char f_3e1c(unsigned port, unsigned char idx)
{
    unsigned char v;
    outportb(g_c1e4, 0xe4);
    outportb(port, idx);
    outportb(g_c1e4, 0xe4);
    v = inportb(port + 1);
    delay(5);
    return v;
}

/* PnP isolation: read 72 bit pairs from read port into 9-byte serial id, check LFSR checksum */
int f_3e51(unsigned port, unsigned char *buf)
{
    int i = 0;
    int bit = 0;
    int found = 0;
    int lfsr = 0x6a;
    if (port < 0x200 || !buf)
        return 0;
    outportb(g_c1b9, 1);
    delay(1);
    memset(buf, 0, 9);
    for (i = 0; i < 0x48; i++) {
        bit = inportb(port) == 0x55;
        delay(1);
        bit = inportb(port) == 0xaa && bit;
        delay(1);
        found = found || bit;
        if (!found && g_c1df < i)
            break;
        if (i < 0x40)
            lfsr = (lfsr >> 1) | (((lfsr ^ (lfsr >> 1) ^ bit) << 7) & 0xff);
        buf[i / 8] = (buf[i / 8] >> 1) | (bit ? 0x80 : 0);
    }
    if (g_c1de)
        found = found != 0;
    else
        found = found && buf[8] == lfsr;
    if (g_c1de) {
        if (buf[8] != lfsr)
            g_c1de = 2;
        else
            g_c1de = 1;
    }
    return found != 0;
}

/* format compressed EISA/PnP vendor id as string */
char g_c2d0[9] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };  /* vendor id string buffer */

char *f_3fb6(unsigned long *id)
{
    register unsigned long *p = id;
    unsigned long v = *p;
    sprintf(g_c2d0, "%c%c%c%X%X%X%X",
        (unsigned char)(((v >> 2) & 0x1f) + 0x40),
        (unsigned char)(((v & 3) << 1 << 1 << 1) + ((v >> 13) & 7) + 0x40),
        (unsigned char)(((v >> 8) & 0x1f) + 0x40),
        (unsigned char)((v >> 20) & 0xf),
        (unsigned char)((v >> 16) & 0xf),
        (unsigned char)((v >> 28) & 0xf),
        (unsigned char)((v >> 24) & 0xf));
    return g_c2d0;
}

/* parse ULTRASND environment variable */
int f_405e(void)
{
    char *env = 0;
    char *dup = 0;
    int dec = 0;
    int hex = 0;
    char *tok;
    unsigned char n;
    env = getenv("ULTRASND");
    if (env == 0)
        return 0;
    dup = strdup(env);
    tok = strtok(dup, ",");
    n = 0;
    while (n < 5 || tok != 0) {
        dec = strtol(tok, 0, 10);
        hex = strtol(tok, 0, 16);
        switch (n) {
        case 0:
            if (hex == 0x220 || hex == 0x230 || hex == 0x240 || hex == 0x250 || hex == 0x260) {
                g_c1af = hex;
                break;
            }
            break;
        case 1:
            if (dec >= 0 && dec <= 7) {
                g_c1b1 = dec;
                break;
            }
            break;
        case 2:
            if (dec >= 0 && dec <= 7) {
                g_c1b3 = dec;
                break;
            }
            break;
        case 3:
            if (dec >= 0 && dec <= 15) {
                g_c1b5 = dec;
                break;
            }
            break;
        case 4:
            if (dec >= 0 && dec <= 15) {
                g_c1b7 = dec;
                break;
            }
            break;
        default:
            break;
        }
        tok = strtok(0, ",");
        n++;
    }
    free(dup);
    if (n < 3)
        return 0;
    return 1;
}

/* parse BLASTER environment variable */
int f_4192(void)
{
    char *env = 0;
    char *dup = 0;
    int dec = 0;
    int hex = 0;
    char *tok;
    unsigned char n;
    char c;
    env = getenv("BLASTER");
    if (env == 0)
        return 0;
    dup = strdup(env);
    tok = strtok(dup, " ");
    n = 0;
    while (n < 12 || tok != 0) {
        if (isalpha(*tok) && strlen(tok) > 1) {
            c = *tok;
            tok++;
            dec = strtol(tok, 0, 10);
            hex = strtol(tok, 0, 16);
            switch (toupper(c)) {
            case 'A':
                if (hex >= 0x220 && hex <= 0x280) {
                    g_c187 = hex;
                    break;
                }
                break;
            case 'I':
                if (dec >= 0 && dec <= 15) {
                    g_c19f = dec;
                    break;
                }
                break;
            case 'D':
                if (hex >= 0 && hex <= 7) {
                    g_c1a7 = hex;
                    break;
                }
                break;
            case 'H':
                if (hex >= 0 && hex <= 7) {
                    g_c1a9 = hex;
                    break;
                }
                break;
            case 'P':
                if ((hex >= 0x300 && hex <= 0x3f0) || hex == 0) {
                    g_c18d = hex;
                    break;
                }
                break;
            case 'E':
                if ((hex >= 0x620 && hex <= 0x680) || hex == 0) {
                    g_c18b = hex;
                    break;
                }
                break;
            case 'J':
                if ((hex >= 0x200 && hex < 0x212) || hex == 0) {
                    g_c18f = hex;
                    break;
                }
                break;
            case 'W':
                g_c191 = hex;
                break;
            case 'C':
                if (hex == 0x168 || hex == 0x1e8 || hex == 0x170 || hex == 0) {
                    g_c19d = hex;
                    break;
                }
                break;
            case 'R':
                g_c193 = hex;
                g_c199 = hex;
                g_c1ad = hex;
                break;
            case 'M':
                if ((hex >= 0x300 && hex <= 0x380) || hex == 0) {
                    g_c19b = hex;
                    break;
                }
                break;
            case 'F':
                if ((hex >= 0x300 && hex <= 0x3ff) || hex == 0) {
                    g_c189 = hex;
                    break;
                }
                break;
            case 'U':
                if (dec >= 0 && dec <= 15) {
                    g_c1a3 = dec;
                    break;
                }
                break;
            case 'Q':
                if (dec >= 0 && dec <= 15) {
                    g_c1a1 = dec;
                    break;
                }
                break;
            case 'K':
                if (hex >= 0 && hex <= 7) {
                    g_c1ab = hex;
                    break;
                }
                break;
            case 'Y':
                if (dec >= 2 && dec <= 15) {
                    g_c1a5 = dec;
                    break;
                }
                break;
            default:
                break;
            }
        }
        tok = strtok(0, " ");
        n++;
    }
    free(dup);
    if (n < 1)
        return 0;
    return 1;
}

/* read the ANSI identifier string (tag 0x82) of card csn into buf */
int f_4425(unsigned char csn, unsigned char *buf, int type)
{
    int done;
    int ok = 1;
    unsigned char tag;
    unsigned len;
    if (csn > 0 && buf) {
        if (type == 11) {
            f_3c59(2, 2);
            delay(1);
            f_3c21();
        }
        f_3c59(3, csn);
        delay(1);
        if (f_4b76(0, 9)) {
            tag = 0;
            len = 0;
            done = 0;
            *buf = 0;
            while (!done) {
                if (f_4bfc(&tag, &len)) {
                    if (tag == 0x82) {
                        if (f_4b76(buf, len))
                            buf[len] = 0;
                        else
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
        f_3c59(3, 0);
    } else
        ok = 0;
    return ok;
}

/* walk the resource data of card csn: read its name and map logical devices to functions */
int f_44fa(unsigned char csn, char *name, int *ld, int type)
{
    int ok = csn > 0 && name != 0;
    unsigned char tag = 0;
    unsigned len = 0;
    int done = 0;
    int n = 0;
    int rev10 = 0;
    int ad = 0;
    int ew = 0;
    int ma = 0;
    if (type == 11) {
        f_3c59(2, 2);
        delay(1);
        f_3c21();
    }
    f_3c59(3, csn);
    delay(1);
    if (ok)
        ok = f_4b76(0, 9);
    if (ok)
        *name = 0;
    while (ok && !done) {
        unsigned char id[7] = { 0 };
        delay(1);
        ok = f_4bfc(&tag, &len);
        delay(1);
        if (!ok)
            break;
        switch (tag) {
        case 2:
            n++;
            ok = f_4b76(id, len);
            if (n == 1) {
                ((unsigned char *)ld)[0x15] = id[3];
                if (type == 6 && id[3] == 0x68)
                    type = 13;
                if (type == 2 && name[3] == '1' && name[4] == '0')
                    rev10 = 3;
                if (type == 8 && memcmp(name, "AD-", 3) == 0)
                    ad = 1;
                if (type == 11 && memcmp(name + 12, "EW", 2) == 0)
                    ew = 4;
                if (type == 3 && memcmp(name, "Ma", 2) == 0)
                    ma = 1;
            }
            if (type == 2) {
                if (n == 1) ld[0] = 0;
                if (n == 2) ld[1] = 1;
                if (n == 3) ld[3] = 2;
                if (n == 4) ld[2] = 3;
                if (n == 5) ld[5] = 4;
            }
            if (type == 3) {
                if (n == 1) ld[6] = 0;
                if (n == 2) ld[0] = 1;
                if (n == 3) ld[3] = 2;
                if (ma == 1) {
                    if (n == 4) ld[2] = 3;
                } else {
                    if (n == 4) ld[5] = 3;
                }
            }
            if (type == 4) {
                if (n == 1) ld[0] = 0;
                if (n == 2) ld[3] = 1;
                if (n == 3) ld[7] = 2;
                if (n == 4) ld[5] = 3;
            }
            if (type == 5) {
                if (n == 1) ld[6] = 0;
                if (n == 2) ld[0] = 1;
                if (n == 3) ld[2] = 2;
                if (n == 6) ld[3] = 5;
            }
            if (type == 6) {
                if (n == 1) ld[0] = 0;
                if (n == 2) ld[1] = 1;
                if (n == 3) ld[3] = 2;
                if (n == 4) ld[2] = 3;
                if (n == 5) ld[5] = 4;
            }
            if (type == 7 || type == 12) {
                if (n == 2) ld[0] = 1;
                if (n == 3) ld[3] = 2;
                if (n == 4) ld[2] = 3;
                ld[5] = 0;
            }
            if (type == 16) {
                if (n == 1) ld[0] = 0;
                if (n == 2) ld[3] = 1;
                if (n == 3) ld[2] = 2;
                if (n == 4) ld[5] = 3;
                if (n == 6) ld[6] = 5;
            }
            if (type == 8) {
                if (ad == 0) {
                    if (n == 1) ld[1] = 0;
                    if (n == 2) ld[2] = 1;
                    if (n == 3) ld[3] = 2;
                    if (n == 4) ld[0] = 3;
                } else if (ad == 1) {
                    if (n == 1) ld[0] = 0;
                    if (n == 2) ld[2] = 1;
                    if (n == 3) ld[3] = 2;
                    if (n == 4) ld[1] = 3;
                    if (n == 5) ld[6] = 4;
                }
            }
            if (type == 9) {
                if (n == 1) ld[0] = 0;
                if (n == 2) ld[2] = 1;
                if (n == 3) ld[3] = 2;
            }
            if (type == 19) {
                if (n == 1) ld[1] = 0;
                if (n == 2) ld[0] = 1;
                if (n == 3) ld[8] = 2;
                if (n == 4) ld[2] = 3;
                if (n == 5) ld[3] = 4;
                if (n == 6) ld[7] = 5;
            }
            if (type == 64) {
                if (n == 1) ld[0] = 0;
                if (n == 2) ld[5] = 1;
                if (n == 3) ld[3] = 2;
                if (n == 4) ld[1] = 3;
                if (n == 5) ld[2] = 4;
                if (n == 6 || g_c1ec == 0) ld[6] = 5;
            }
            if (type == 10) {
                if (n == 1) ld[5] = 0;
                if (n == 2) ld[0] = 1;
                ld[2] = 2;
                ld[3] = 3;
                ld[7] = 4;
                ld[8] = 5;
            }
            if (type == 11) {
                if (n == 1) ld[0] = 0;
                if (n == 2) ld[3] = 1;
                if (n == 3) ld[6] = 2;
                if (n == 4 || g_c1ea == 1) ld[2] = 3;
                if (ew == 4) {
                    if (n == 5) ld[1] = 4;
                    if (n == 6) ld[8] = 5;
                } else {
                    if (n == 5) ld[5] = 4;
                }
            }
            if (type == 13) {
                if (n == 1) ld[0] = 0;
                if (n == 2) ld[3] = 1;
            }
            if (type == 14) {
                if (n == 1) ld[6] = 0;
                if (n == 2) ld[0] = 1;
                if (n == 3) ld[3] = 2;
                if (n == 4) ld[5] = 3;
            }
            if (type == 44) {
                if (n == 1) ld[2] = 0;
            }
            if (type == 55) {
                if (n == 1) ld[6] = 0;
                if (n == 2) ld[3] = 1;
            }
            if (type == 59) {
                if (n == 2) ld[2] = 1;
                ld[3] = 2;
            }
            if (type == 2 && rev10 == 3 && n == 4 && ld[2] == 3) {
                done = 1;
                ld[5] = 4;
                break;
            }
            break;
        case 0xf:
            ok = f_4b76(0, len);
            done = 1;
            break;
        case 0x82:
            if (*name == 0) {
                ok = f_4b76(name, len);
                if (ok)
                    name[len] = 0;
            } else {
                char str[64] = { 0 };
                ok = f_4b76(str, len);
                if (ok) {
                    if (str[0] == 'A' && str[1] == 'u' && str[2] == 'd')
                        ld[0] = n - 1;
                    else if (str[0] == 'G' && str[1] == 'a' && str[2] == 'm')
                        ld[3] = n - 1;
                    else if (str[0] == 'W' && str[1] == 'a' && str[2] == 'v')
                        ld[4] = n - 1;
                    else if (str[0] == 'I' && str[1] == 'D' && str[2] == 'E')
                        ld[5] = n - 1;
                }
            }
            break;
        default:
            if (len > 0)
                ok = f_4b76(0, len);
        }
    }
    f_3c59(3, 0);
    delay(1);
    return ok;
}
#undef g_c1de
#undef g_c187
#undef g_c189
#undef g_c18b
#undef g_c18d
#undef g_c18f
#undef g_c191
#undef g_c193
#undef g_c199
#undef g_c19b
#undef g_c19d
#undef g_c1ad
#undef g_c1af
#undef g_c1bd

/* ======== chunk 4 ======== */
#define g_c19f (*(unsigned char *)&g_c19f)
#define g_c1a7 (*(unsigned char *)&g_c1a7)
#define g_c1a9 (*(unsigned char *)&g_c1a9)
#define g_c1bd (*(unsigned *)&g_c1bd)
/* read n bytes of PnP resource data (waits on status bit) into buf (may be NULL) */
int f_4b76(unsigned char *buf, int n)
{
    int i = 0;
    unsigned char st = 0;
    unsigned char d = 0;
    unsigned char cnt = 15;
    while (i < n) {
        cnt = 15;
        outportb(g_c1b9, 5);
        while (cnt--) {
            st = inportb(g_c1bd) & 1;
            if (!st)
                delay(1);
            else
                break;
        }
        outportb(g_c1b9, 4);
        if (st != 0) {
            d = inportb(g_c1bd);
            if (buf)
                buf[i] = d;
            i++;
        } else
            return 0;
    }
    return 1;
}

/* read a PnP resource tag header: tag in *tag, length in *len */
int f_4bfc(unsigned char *tag, int *len)
{
    int r = 0;
    unsigned char b = 0;
    unsigned char w[2] = { 0, 0 };
    r = f_4b76(&b, 1);
    if (r) {
        if (b & 0x80) {
            *tag = b;
            r = f_4b76(w, 2);
            if (r)
                *len = (w[1] << 8) | w[0];
        } else {
            *tag = (b >> 1 >> 1 >> 1) & 0xf;
            *len = b & 7;
        }
        r = (r && *tag != 0xff && *len != -1) ? 1 : 0;
    }
    return r;
}

/* detect and initialize the AWE EMU8000 at port */
void f_4c96(unsigned port)
{
    unsigned char ver = 2;
    unsigned char a;
    unsigned char b;
    unsigned char st;
    char *msg = "AWE EMU8000 chip not found. WaveTable and/or FM may not be available\nInitialization done.\n";
    outportb(port + 0x802, 0x3d);
    outportb(port + 0x400, 0x59);
    outportb(port + 0x802, 0x3e);
    outportb(port + 0x400, 0x20);
    outportb(port + 0x802, 0x3f);
    outportb(port + 0x400, 0);
    outportb(port + 0x802, 0x3d);
    st = inportb(port + 0x400);
    if ((st & 0x7e) != 0x58) {
        printf(msg);
        return;
    }
    outportb(port + 0x802, 0x3e);
    st = inportb(port + 0x400);
    if ((st & 3) != 3) {
        printf(msg);
        return;
    }
    delay(1);
    outportb(port - 0x400 + 4, 0xfe);
    delay(1);
    a = inportb(port - 0x400 + 5);
    outportb(port - 0x400 + 4, 0xfd);
    delay(1);
    b = inportb(port - 0x400 + 5);
    if (a == 0xff && b == 0xff)
        ver = 1;
    printf("AWE [%Xh] EMU8000 v%d Synth chip found. ", port, ver);
    f_721c(0x3f, port + 0x400, 0);
    printf("\nInitializing EMU8K... ");
    printf("AWE RAM detected: [%dK] ", f_72a5(ver));
    if (g_c1e0 > 0 || g_c1e2 > 0)
        printf("\nEffects [Reverb:%d Chorus:%d] ", g_c1e2, g_c1e0);
    printf("\nInitialization done. ");
}

/* print the SB DSP version at port */
void f_4dec(int port)
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

/* detect and configure a non-PnP SB16/AWE card */
int f_4e94(void)
{
    int port = 0x220;
    unsigned char irq = g_c19f;
    unsigned char dma = g_c1a7;
    unsigned char hdma = g_c1a9;
    unsigned mpu = 0;
    int awe = g_c18b;
    unsigned char i;
    unsigned char k;
    unsigned char maj = 0;
    unsigned char min = 0;
    unsigned char t = 0;
    unsigned char irqr = 2;
    unsigned char dmar = 2;
    unsigned char mr = 0;
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
                t = inportb(port + 0xa);
                delay(1);
                if (t == 0x43) {
                    for (k = 0; k < 100; k++) {
                        t = inportb(port + 0xa);
                        delay(1);
                        if (t == 0)
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
        blaster = f_4192();
        if (blaster != 0) {
            irq = g_c19f;
            dma = g_c1a7;
            hdma = g_c1a9;
            awe = g_c18b;
        }
        printf("\n%s detected at: [%Xh] DSP: v%d.%02d\n", name, port, maj, min);
        f_5175(port, irq, dma, hdma);
        outportb(port + 4, 0x84);
        delay(1);
        mr = inportb(port + 5);
        delay(1);
        if (mr > 0x7f && mr < 0xf0) {
            if (g_c18d == 0)
                outportb(port + 5, 0xe2);
            else if (g_c18d == 0x300)
                outportb(port + 5, 0xe4);
            else
                outportb(port + 5, 0xe0);
        }
        delay(20);
        irqr = f_5270(port);
        delay(20);
        dmar = f_52d1(port);
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
        if (f_5378(port)) {
            awe = port + 0x400;
            printf(" AWE:%X\n", awe);
            f_4c96(awe);
            printf("\n");
        } else
            printf("\n");
        f_556d();
        f_5419("SB", -1, g_c1fa);
        return 1;
    }
    if (g_c1da == 1)
        puts("No SB16 or AWE non-PNP card detected.");
    return 0;
}

/* program SB16 mixer IRQ (0x80) and DMA (0x81) select registers */
void f_5175(int port, unsigned char irq, unsigned char dma, unsigned char hdma)
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

/* read SB16 IRQ setting from mixer register 0x80 */
unsigned char f_5270(int port)
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

/* read SB16 DMA setting from mixer register 0x81 (HDMA<<4 | DMA) */
unsigned char f_52d1(int port)
{
    unsigned char lo;
    unsigned char hi;
    unsigned char r = 0;
    unsigned char v;
    outportb(port + 4, 0x81);
    delay(1);
    v = inportb(port + 5);
    lo = v & 0xb;
    hi = (v >> 4) & 0xe;
    switch (hi) {
    case 2: r = 5; break;
    case 4: r = 6; break;
    case 8: r = 7; break;
    default: r = 0;
    }
    r = r << 4;
    switch (lo) {
    case 1: r += 0; break;
    case 2: r += 1; break;
    case 8: r += 3; break;
    default: r += 0;
    }
    return r;
}

/* check for an EMU8000 at port+0x400 */
unsigned char f_5378(int port)
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

/* print mixer settings line */
void f_5419(char *name, int d3, int gain)
{
    printf("%s Mixer [VOL:%d WAV:%d FM:%d LIN:%d CD:%d MIC:%d", name,
           g_c1c0, g_c1c6, g_c1c3, g_c1c9, g_c1cc, g_c1cf);
    if (memcmp(name, "SB", 2) == 0)
        printf(" PC:%d TRB:%d BAS:%d", g_c1d2, g_c1d5, g_c1d8);
    if (memcmp(name, "Ya", 2) == 0)
        printf(" TREB:%d BASS:%d", g_c1d5, g_c1d8);
    if (memcmp(name, "ES", 2) == 0 && g_c1d8 > -1)
        printf(" AUXB:%d", g_c1d8);
    if (gain > -1)
        printf(" GAIN:%d", gain);
    if (d3 > -1)
        printf(" 3D:%d", d3);
    puts("]");
}

/* write OPTi SB-compatible mixer registers */
void f_54e9(void)
{
    unsigned char mic;
    mic = g_c1ce = g_c1cf * 7 / 97;
    f_3cfd(g_c187, 0x22, g_c1bf);
    f_3cfd(g_c187, 4, g_c1c5);
    f_3cfd(g_c187, 0x26, g_c1c2);
    f_3cfd(g_c187, 0x2e, g_c1c8);
    f_3cfd(g_c187, 0x28, g_c1cb);
    f_3cfd(g_c187, 0xa, mic);
}

/* write SB16 mixer registers */
void f_556d(void)
{
    int idx = g_c187 + 4;
    int dat = g_c187 + 5;
    int master = g_c1bf & 0xf0;
    int voice = g_c1c2 & 0xf0;
    int fm = g_c1c5 & 0xf0;
    int line = g_c1cb & 0xf0;
    unsigned char mic = g_c1cf * 255 / 97;
    int gain = 0x40;
    int agc = g_c1f8 > 0 ? 1 : 0;
    switch (g_c1fa) {
    case 1: gain = 0; break;
    case 4: gain = 0x80; break;
    default: gain = 0x40; g_c1fa = 2;
    }
    if (g_c1d5 < 0)
        g_c1d5 = 0x32;
    if (g_c1d8 < 0)
        g_c1d8 = 0x32;
    f_5715();
    outportb(idx, 0x34); outportb(dat, voice);
    outportb(idx, 0x35); outportb(dat, voice);
    outportb(idx, 0x32); outportb(dat, fm);
    outportb(idx, 0x33); outportb(dat, fm);
    outportb(idx, 0x36); outportb(dat, line);
    outportb(idx, 0x37); outportb(dat, line);
    outportb(idx, 0x2e); outportb(dat, g_c1c8);
    outportb(idx, 0x38); outportb(dat, g_c1c8);
    outportb(idx, 0x39); outportb(dat, g_c1c8);
    outportb(idx, 0x0a); outportb(dat, g_c1ce);
    outportb(idx, 0x3a); outportb(dat, mic);
    outportb(idx, 0x41); outportb(dat, gain);
    outportb(idx, 0x42); outportb(dat, gain);
    outportb(idx, 0x3b); outportb(dat, g_c1d1);
    outportb(idx, 0x44); outportb(dat, g_c1d4);
    outportb(idx, 0x45); outportb(dat, g_c1d4);
    outportb(idx, 0x46); outportb(dat, g_c1d7);
    outportb(idx, 0x47); outportb(dat, g_c1d7);
    outportb(idx, 0x30); outportb(dat, master);
    outportb(idx, 0x31); outportb(dat, master);
    outportb(idx, 0x22); outportb(dat, g_c1bf);
    outportb(idx, 0x90); outportb(dat, agc);
}

/* update SB16 mixer output switches (reg 0x3c) */
void f_5715(void)
{
    unsigned char v = 0;
    outportb(g_c187 + 4, 0x3c);
    v = inportb(g_c187 + 5);
    if (g_c1ce > 0)
        v |= 1;
    else
        v &= 0xfe;
    if (g_c1cb > 0)
        v |= 6;
    else
        v &= 0xf9;
    if (g_c1c8 > 0)
        v |= 0x18;
    else
        v &= 0xe7;
    f_3cfd(g_c187 + 5, 0x3c, v);
}

/* initialize OPTi 82C93x card in given mode */
void f_577f(int mode)
{
    unsigned char c = 0xf0;
    unsigned char d = 0x87;
    int e = 0;
    int f = 0;
    int g = 0;
    g_c1cb = (100 - g_c1cc) * 31 / 97;
    g_c1ce = 0xc0 | ((100 - g_c1cf) * 31 / 97) << 1;
    g_c1c8 = (100 - g_c1c9) * 31 / 97;
    g_c1c5 = (100 - g_c1c6) * 31 / 97;
    g_c1c2 = (100 - g_c1c3) * 31 / 97;
    if (g_c1c3 > g_c1c0)
        g_c1c2 = (100 - g_c1c0) * 31 / 97;
    if (g_c1cc < 1)
        g_c1cb = 0x88;
    if (g_c1cf < 1)
        g_c1ce = 0x10;
    if (g_c1c9 < 1)
        g_c1c8 = 0x88;
    if (g_c1c6 < 1)
        g_c1c5 = 0x38;
    if (g_c1c2 < 9)
        g_c1c2 = 9;
    if (g_c187 == 0x240)
        c = 0xf4;
    if (g_c19d > 0)
        f |= 8;
    if (g_c1f2 > 0)
        f |= 0x80;
    switch (g_c18d) {
    case 0x330: e = 4; break;
    case 0x320: e = 5; break;
    case 0x310: e = 6; break;
    case 0x300: e = 7; break;
    }
    d = 7 | e << 5;
    g = f_3d6c(g_c197, 7) & 0x40;
    f_3daa(g_c197, 1, 0x40);
    f_3daa(g_c197, 1, 0);
    delay(120);
    if (mode == 5)
        f_3daa(g_c197, 0xd, 2);
    outportb(g_c191 + 6, 0);
    delay(250);
    f_3daa(g_c197, 1, f);
    f_3daa(g_c197, 3, c);
    f_3daa(g_c197, 4, 0xa2);
    f_3daa(g_c197, 6, d);
    f_3daa(g_c197, 7, g_c1c5 | g);
    f_3daa(g_c197, 2, 0x23);
    f_3daa(g_c197, 2, 3);
    f_3daa(g_c197, 5, 0x9b);
    outportb(g_c191, 0);
    f_3d40(g_c191, 0x48, 7);
    delay(50);
    f_3d40(g_c191, 0x49, 8);
    delay(50);
    f_3d40(g_c191, 0x49, 0);
    delay(50);
    f_3d40(g_c191, 0x2a, 2);
    f_3d40(g_c191, 0x2b, 0);
    delay(50);
    f_3d40(g_c191, 0x20, 0xc0);
    f_3d40(g_c191, 0x21, 0);
    f_3daa(g_c197, 5, 0xab);
    f_3daa(g_c197, 5, 0xbb);
    f_3d40(g_c191, 0x48, 3);
    delay(50);
    f_3d40(g_c191, 0x2a, 2);
    f_3d40(g_c191, 0x2b, 0);
    f_3d40(g_c191, 0x29, 0);
    f_3daa(g_c197, 5, 0xab);
    f_3daa(g_c197, 5, 0x9b);
    f_3d40(g_c191, 0x24, g_c1c2);
    f_3d40(g_c191, 0x25, g_c1c2);
    f_3d40(g_c191, 0x22, g_c1cb);
    f_3d40(g_c191, 0x23, g_c1cb);
    f_3d40(g_c191, 0x2c, 0xca);
    if (mode == 5) {
        f_3d40(g_c191, 0x10, 0x3f & g_c1ce);
        f_3d40(g_c191, 0x11, g_c1ce);
    }
    f_3d40(g_c191, 0x12, g_c1c8);
    f_3d40(g_c191, 0x13, g_c1c8);
    f_3d40(g_c191, 0x2c, 0x8a);
    f_3daa(g_c197, 5, 0xab);
    f_3d40(g_c187, 0x22, g_c1bf);
    outportb(g_c187 + 6, 1);
    delay(5);
    outportb(g_c187 + 6, 0);
    delay(5);
    outportb(g_c187 + 0xc, 0xf2);
    delay(5);
    delay(50);
    f_3daa(g_c197, 5, mode == 5 ? 0xa5 : 0xaf);
    f_5419("OPTi", -1, -1);
    printf("Card mode: ");
    puts(g_c1f2 > 0 ? "[WSS]" : "[SBPro]");
}
#undef g_c19f
#undef g_c1a7
#undef g_c1a9
#undef g_c1bd

/* ======== chunk 5 ======== */
#define g_c191 (*(unsigned *)&g_c191)
#define g_c197 (*(unsigned *)&g_c197)
/* initialise the codec/mixer registers to defaults */
void f_5c48(void)
{
    register unsigned codec = g_c191 + -4;
    register unsigned mix = g_c197 + 2;
    unsigned char b = g_c1f8 > 0 ? 0x60 : 0;

    f_3def(mix, 0x14, 0);
    f_3def(mix, 4, 0x16);
    f_3def(mix, 5, 0x7f);
    f_3def(mix, 6, 0x83);
    f_3def(mix, 10, 1);
    f_3def(mix, 0x13, 0);
    f_3def(mix, 0x14, 0);
    f_3def(mix, 0x12, 0x89);
    f_3def(mix, 0x15, 0x80);
    f_3def(mix, 0x1a, 1);
    delay(20);
    outportb(codec, 0);
    delay(32);
    f_3d40(codec, 0x29, 0);
    f_3d40(codec, 0x2a, 2);
    f_3d40(codec, 0x2b, 0);
    f_3d40(codec, 0x48, 3);
    delay(100);
    f_3d40(codec, 0x30, (unsigned char)((100 - g_c1c3) * 15 / 97) << 1);
    f_3d40(codec, 0x31, (unsigned char)((100 - g_c1c3) * 15 / 97) << 1 | b);
    f_3d40(codec, 0x20, 0xac);
    f_3d40(codec, 0x21, 0xac);
    f_3d40(codec, 0x36, 5);
    f_3d40(codec, 0x37, 5);
    f_3d40(codec, 0x26, 0x84);
    f_3d40(codec, 0x27, 0x84);
    delay(80);
    f_54e9();
}

/* upload SAM9407.BIN firmware to the SAM9407 at port (needs -d: both "Bad" blocks get cross-jumped; matches only under TC 3.0) */
unsigned g_c322[256] = {
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
unsigned char g_c522[37] = {
    0x09, 0x00, 0x02, 0x3f, 0xc0, 0x00, 0xc1, 0x00, 0xc2, 0x00, 0xc3, 0x00,
    0xc4, 0x00, 0xc5, 0x00, 0xc6, 0x00, 0xc7, 0x00, 0xc8, 0x00, 0xc9, 0x00,
    0xca, 0x00, 0xcb, 0x00, 0xcc, 0x00, 0xcd, 0x00, 0xce, 0x00, 0xcf, 0x00,
    0xff };  /* SAM9407 init byte sequence */

void f_5de3(unsigned base)
{
    register unsigned port = base;
    unsigned w = 0;
    unsigned long size;
    register FILE *f;
    int i;
    unsigned p;

    f = fopen("SAM9407.BIN", "rb");
    if (f == NULL) {
        puts("SAM9407.BIN firmware file not found. Sound will be silent.");
        return;
    }
    /* TC 3.0 pushes long 0 as xor ax,ax/push ax/push ax; BC 3.1 emits xor ax,ax/xor dx,dx */
    fseek(f, 0L, SEEK_END);
    size = ftell(f);
    rewind(f);
    if (size > 0x20000L || size < 0x1000L) {
        puts("Bad SAM9407 firmware file.");
        fclose(f);
        return;
    }
    fseek(f, 0x400L, SEEK_SET);
    w = fgetc(f);
    w = (fgetc(f) << 8) | w;
    if (w != 0xd0ce) {
        puts("Bad SAM9407 firmware file.");
        fclose(f);
        return;
    }
    fseek(f, 0L, SEEK_SET);     /* same long-0 difference as above */
    outportb(port + 1, 0x3f);
    outportb(port + 1, 0x48);
    outportb(port, 0);
    outportb(port + 1, 0x70);
    outportb(port, 0x11);
    delay(20);
    for (i = 0; i < 0x100; i++)
        outpw(port + 2, g_c322[i]);
    delay(20);
    outportb(port + 1, 0x0b);
    outportb(port + 1, 0);
    outportb(port + 1, 2);
    outportb(port + 1, 0);
    outportb(port + 1, 0);
    outportb(port + 1, 0x93);
    outportb(port + 1, 0x66);
    delay(20);
    fseek(f, 0x400L, SEEK_SET);
    i = 0;
    printf("Uploading SAM9407 firmware");
    while (!feof(f)) {
        w = fgetc(f);
        w = (fgetc(f) << 8) | w;
        outpw(port + 2, w);
        i++;
        if (i % 5000 == 0)
            printf(".");
    }
    puts(" Done.");
    fclose(f);
    delay(100);
    for (i = 0; i < 0x25; i++) {
        if (i < 4 || i == 0x24)
            p = port + 1;
        else
            p = port;
        outportb(p, g_c522[i]);
    }
}

/* read a byte from the card's ROM/RAM at 24-bit address addr */
unsigned char f_5fc9(unsigned long addr)
{
    outportb(g_c187 + 0x103, 0x43);
    outpw(g_c187 + 0x104, (unsigned)addr);
    outportb(g_c187 + 0x103, 0x44);
    outportb(g_c187 + 0x105, (unsigned char)(addr >> 16));
    return inportb(g_c187 + 0x107);
}

/* write a byte to the card's RAM at 24-bit address addr */
void f_6008(unsigned long addr, unsigned char val)
{
    outportb(g_c187 + 0x103, 0x43);
    outpw(g_c187 + 0x104, (unsigned)addr);
    outportb(g_c187 + 0x103, 0x44);
    outportb(g_c187 + 0x105, (unsigned char)(addr >> 16));
    outportb(g_c187 + 0x107, val);
}

/* scan the 4 ROM banks for "INTR" sample ROM headers and list them */
int f_604a(void)
{
    unsigned char old = 0;
    unsigned char sum = 0;
    unsigned char mask = 0;
    unsigned w = 0;
    unsigned long size = 0;
    unsigned long addr = 0;
    char sig[5] = { 0 };
    char name[16] = { 0 };
    register int i = 0;
    register int n = 0;

    outportb(g_c187 + 0x103, 0x99);
    old = inportb(g_c187 + 0x105);
    outportb(g_c187 + 0x103, 0x19);
    outportb(g_c187 + 0x105, old | 1);
    outportb(g_c187 + 0x103, 0x53);
    outportb(g_c187 + 0x105, 3);
    outportb(g_c187 + 0x103, 0x52);
    w = inpw(g_c187 + 0x104) & 0xff10;
    outpw(g_c187 + 0x104, w | 0x4c);
    for (addr = 0; addr < 0x1000000L; addr += 0x400000L) {
        for (i = 0; i < 4; i++)
            sig[i] = f_5fc9(addr + i);
        if (strncmp(sig, "INTR", 4) != 0)
            continue;
        sum = 0;
        for (i = 0; i < 0x200; i++)
            sum += f_5fc9(addr + i);
        if (sum != 0)
            continue;
        n++;
        mask |= 1 << (addr >> 22);
        for (i = 0; i < 16; i++)
            name[i] = f_5fc9(addr + i + 10);
        size = ((unsigned long)f_5fc9(addr + 0x2b) << 24) +
               ((unsigned long)f_5fc9(addr + 0x2a) << 16) +
               ((unsigned long)f_5fc9(addr + 0x29) << 8) +
               (unsigned)f_5fc9(addr + 0x28);
        if (n > 0)
            printf("ROM Bank #%d: [%s] Size: %dK \n", n, name, size >> 10);
    }
    if (n < 1)
        puts("ROM: Not found");
    outportb(g_c187 + 0x103, 0x53);
    outportb(g_c187 + 0x105, 0);
    outportb(g_c187 + 0x103, 0x19);
    outportb(g_c187 + 0x105, old);
    return 0;
}

/* detect on-board RAM size per bank, program the memory config register; returns total KB */
/* TC 3.0 scales the long index with two single-bit shifts; BC 3.1 emits mov cl,2, so imitate it there */
#ifdef __BORLANDC__
#define SZ(i) (*(unsigned long *)((char *)sizes + ((i) << 1 << 1)))
#else
#define SZ(i) sizes[i]
#endif

int f_62c1(unsigned long *dst)
{
    register unsigned long *out = dst;
    unsigned long sizes[4] = { 0 };
    unsigned long a = 0;
    unsigned long base = 0;
    unsigned long off = 0;
    unsigned char bank = 0;
    unsigned char cfg = 0;
    unsigned char old = 0;
    unsigned char found = 0;
    register unsigned w = 0;
    int total = 0;

    outportb(g_c187 + 0x103, 0x99);
    old = inportb(g_c187 + 0x105);
    outportb(g_c187 + 0x103, 0x19);
    outportb(g_c187 + 0x105, old | 1);
    outportb(g_c187 + 0x103, 0x53);
    outportb(g_c187 + 0x105, 1);
    outportb(g_c187 + 0x103, 0x52);
    w = inpw(g_c187 + 0x104) & 0xfff0;
    outpw(g_c187 + 0x104, w | 0xc);
    for (; a < 0x1000000L; a += 0x10000L)
        f_6008(a, 0);
    for (bank = 0; bank < 4; bank++) {
        f_6008(base, 0xaa);
        f_6008(base + 1, 0x55);
        if (f_5fc9(base) == 0xaa && f_5fc9(base + 1) == 0x55)
            found = 1;
        if (found) {
            while (off < 0x400000L) {
                SZ(bank) += 0x10000L;
                off += 0x10000L;
                a = base + off;
                if (f_5fc9(a) == 0xaa)
                    break;
            }
        }
        if (out)
            *out++ = SZ(bank);
        SZ(bank) >>= 10;
        base += 0x400000L;
        off = 0;
        found = 0;
    }
    for (a = 0; a < 0x200000L; a += 0x400) {
        f_6008(a, 0);
        f_6008(a + 1, 0);
    }
    outportb(g_c187 + 0x103, 0x52);
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
    if (g_c1ee == 1)
        cfg = 1;
    cfg |= 0x80;
    outpw(g_c187 + 0x104, w | cfg);
    outportb(g_c187 + 0x103, 0x19);
    outportb(g_c187 + 0x105, old);
    total = sizes[0] + sizes[1] + sizes[2] + sizes[3];
    return total;
}
#undef g_c191
#undef g_c197

/* ======== chunk 6 ======== */
#define g_c18b (*(unsigned *)&g_c18b)
/* InterWave register write: reg = type<<16 | index<<8 | width */
void f_6895(unsigned long reg, unsigned val)
{
    unsigned char sel;
    unsigned char t2;
    unsigned char t3;
    unsigned type;
    unsigned width;

    width = (unsigned char)reg;
    type = (unsigned)(reg >> 16);
    sel = reg >> 8;
    if (type >= 1 && type <= 0x19) {
        if (type <= 0xe) {
            outp(g_c187 + width, (unsigned char)val);
            return;
        }
        if (type == 0x19)
            return;
        {
            outp(g_c187 + 0x103, 0x5b);
            t3 = inp(g_c187 + 0x105);
            outp(g_c187 + 0x105, (unsigned char)(t3 | 9));
            t2 = inp(g_c187 + 0xf);
            t2 = (t2 & 0xf8) | sel;
            outp(g_c187 + 0xf, t2);
            if (reg == 0xf000bL || reg == 0x10000bL) {
                t2 = inp(g_c187);
                outp(g_c187, (unsigned char)(reg == 0xf000bL ? t2 & 0xbf : t2 | 0x40));
            }
            outp(g_c187 + 0xb, (unsigned char)val);
            outp(g_c187 + 0x103, 0x5b);
            outp(g_c187 + 0x105, t3);
        }
    } else if (type >= 0x1a && type <= 0x5c) {
        if (type == 0x5c) {
            outp(g_c187 + 0x107, (unsigned char)val);
        } else if (type != 0x1b) {
            if (type >= 0x1a && type <= 0x21) {
                if (width == 4)
                    outpw(g_c187 + 0x104, val);
                else
                    outp(g_c187 + 0x100 + width, (unsigned char)val);
            } else {
                outp(g_c187 + 0x103, sel);
                if (width == 4)
                    outpw(g_c187 + 0x104, val);
                else
                    outp(g_c187 + 0x105, (unsigned char)val);
            }
        }
    } else if (type >= 0x5c && type <= 0x81) {
        if (type <= 0x61) {
            outp(g_c187 + 0x10c + width, (unsigned char)val);
        } else {
            t3 = inp(g_c187 + 0x10c);
            t3 = (t3 & 0xe0) + sel;
            outp(g_c187 + 0x10c, t3);
            outp(g_c187 + 0x10d, (unsigned char)val);
        }
    }
    return;
}

/* InterWave init for GUS mode: mixer, DRAM size detection, levels */
void f_6a91(void)
{
    int i;
    unsigned ram = 0;

    f_6895(0x5b6005L, 0);
    f_6895(0x565b05L, 0x15);
    f_6895(0x10000L, g_c1cf > 0 ? 0xc : 8);
    f_6895(0x555a05L, 0x7f);
    f_6895(0x454505L, 0);
    f_6895(0x545905L, 0x1f);
    f_6895(0x565b05L, 0x15);
    f_6895(0x4a4c05L, 0);
    delay(2);
    f_6895(0x4a4c05L, 1);
    f_6895(0x220005L, 2);
    f_6895(0x2f0d05L, 2);
    for (i = 0; i < 0x20; i++) {
        f_6895(0x1e0002L, i);
        f_6895(0x3d1b04L, 2);
        f_6895(0x2e0c04L, 2);
        f_6895(0x371505L, 2);
        f_6895(0x220005L, 2);
        f_6895(0x2f0d05L, 2);
    }
    f_6895(0x4a4c05L, 7);
    delay(2);
    f_6895(0x555a05L, 0x7f);
    f_6895(0x6c0a01L, 0);
    f_6895(0x6e0c01L, 0x6c);
    f_6895(0x6b0901L, 0);
    f_6895(0x721001L, 0x81);
    f_6895(0x731101L, 0xc2);
    f_6895(0x6c0a01L, 0);
    delay(10);
    f_604a();
    delay(10);
    ram = f_62c1(0);
    delay(10);
    printf("RAM detected: %d KB", ram);
    delay(2);
    if (ram < 1)
        printf(" - You need some RAM to get GUS mode working!!");
    else if (g_c1ee == 1)
        printf(" (Forced to 512 KB)");
    printf("\n");
    f_6ddd(0, g_c1c2);
    f_6d8d(0, g_c1c5);
    f_6d3d(0, g_c1bf);
    f_6e2d(g_c1c9 <= 0, g_c1c8);
    f_6e7d(g_c1cc <= 0, g_c1cb);
    f_6ecd(g_c1cf <= 0, g_c1ce);
}

/* set codec mixer pair 0x19/0x1b (bit 7 = mute) */
void f_6d3d(int flag, unsigned char v)
{
    f_6895(0x7b1901L, v | (flag ? 0x80 : 0));
    f_6895(0x7d1b01L, v | (flag ? 0x80 : 0));
}

/* set codec mixer pair 0x06/0x07 (bit 7 = mute) */
void f_6d8d(int flag, unsigned char v)
{
    f_6895(0x680601L, v | (flag ? 0x80 : 0));
    f_6895(0x690701L, v | (flag ? 0x80 : 0));
}

/* set codec mixer pair 0x02/0x03 (bit 7 = mute) */
void f_6ddd(int flag, unsigned char v)
{
    f_6895(0x640201L, v | (flag ? 0x80 : 0));
    f_6895(0x650301L, v | (flag ? 0x80 : 0));
}

/* set codec mixer pair 0x12/0x13 (bit 7 = mute) */
void f_6e2d(int flag, unsigned char v)
{
    f_6895(0x741201L, v | (flag ? 0x80 : 0));
    f_6895(0x751301L, v | (flag ? 0x80 : 0));
}

/* set codec mixer pair 0x04/0x05 (bit 7 = mute) */
void f_6e7d(int flag, unsigned char v)
{
    f_6895(0x660401L, v | (flag ? 0x80 : 0));
    f_6895(0x670501L, v | (flag ? 0x80 : 0));
}

/* set codec mixer pair 0x16/0x17 (bit 7 = mute) */
void f_6ecd(int flag, unsigned char v)
{
    f_6895(0x781601L, v | (flag ? 0x80 : 0));
    f_6895(0x791701L, v | (flag ? 0x80 : 0));
}

/* EMU8000: wait until DRAM read (SMALR) is not busy */
void f_6f1d(void)
{
    int i = 0;
    while (f_725e(0x34, g_c18b + 0x400) & 0x80000000L) {
        i++;
        if (i > 0x834)
            break;
    }
}

/* EMU8000: wait until DRAM write (SMALW) is not busy */
void f_6f4a(void)
{
    int i = 0;
    while (f_725e(0x36, g_c18b + 0x400) & 0x80000000L) {
        i++;
        if (i > 0x834)
            break;
    }
}

/* EMU8000: detect onboard DRAM size, returns KB */
int f_6f77(void)
{
    int size = 0;

    f_70e5(1);
    f_6f4a();
    f_71ec(0x36, g_c18b + 0x400, 0x200000L);
    f_721c(0x3a, g_c18b + 0x400, 0xa5b6);
    f_6f4a();
    f_6f1d();
    f_71ec(0x34, g_c18b + 0x400, 0x200000L);
    f_723b(0x3a, g_c18b + 0x400);
    if (f_723b(0x3a, g_c18b + 0x400) == 0xa5b6)
        size += 0x20;
    f_6f1d();
    while (size < 0x7000) {
        f_71ec(0x36, g_c18b + 0x400, ((long)size << 9) + 0x200000L);
        f_721c(0x3a, g_c18b + 0x400, 0x9de3);
        f_6f4a();
        f_71ec(0x34, g_c18b + 0x400, ((long)size << 9) + 0x200000L);
        f_723b(0x3a, g_c18b + 0x400);
        if (f_723b(0x3a, g_c18b + 0x400) != 0x9de3)
            break;
        f_6f1d();
        f_71ec(0x34, g_c18b + 0x400, 0x200000L);
        f_723b(0x3a, g_c18b + 0x400);
        if (f_723b(0x3a, g_c18b + 0x400) != 0xa5b6)
            break;
        f_6f1d();
        size += 0x20;
    }
    f_71b2();
    if (size == 0x7000)
        size--;
    if (size == 0x20)
        size = 0;
    return size;
}

/* EMU8000: set up the 30 DRAM DMA channels (odd ones read if a != 0) */
void f_70e5(int a)
{
    unsigned i;
    for (i = 0; i < 0x1e; i++) {
        f_721c(i + 0xa0, g_c18b + 0x400, 0x80);
        f_71ec(i + 0x60, g_c18b, 0L);
        f_71ec(i + 0x40, g_c18b, 0L);
        f_71ec(i + 0x20, g_c18b, 0x40000000L);
        f_71ec(i, g_c18b, 0x40000000L);
        f_71ec(i + 0xc0, g_c18b, 0L);
        f_71ec(i + 0xe0, g_c18b, 0L);
        f_71ec(i, g_c18b + 0x400, (i & 1) && a ? 0x04000000L : 0x06000000L);
    }
}

/* EMU8000: release the DRAM DMA channels */
void f_71b2(void)
{
    unsigned i;
    for (i = 0; i < 0x1e; i++) {
        f_71ec(i, g_c18b + 0x400, 0L);
        f_721c(i + 0xa0, g_c18b + 0x400, 0x807f);
    }
}

/* EMU8000: write dword register */
void f_71ec(unsigned reg, unsigned port, unsigned long val)
{
    outpw(g_c18b + 0x802, reg);
    outpw(port, (unsigned)val);
    outpw(port + 2, (unsigned)(val >> 16));
}

/* EMU8000: write word register */
void f_721c(unsigned reg, unsigned port, unsigned val)
{
    outpw(g_c18b + 0x802, reg);
    outpw(port, val);
}

/* EMU8000: read word register */
unsigned f_723b(unsigned reg, unsigned port)
{
    unsigned v;
    outpw(g_c18b + 0x802, reg);
    v = inpw(port);
    return v;
}

/* EMU8000: read dword register */
unsigned long f_725e(unsigned reg, unsigned port)
{
    unsigned long lo;
    unsigned long hi;
    outpw(g_c18b + 0x802, reg);
    lo = inpw(port);
    hi = inpw(port + 2);
    lo |= hi << 16;
    return lo;
}

/* EMU8000 (AWE32) initialisation, returns DRAM size in KB */
int f_72a5(int a)
{
    int i = 0;
    int j = 1;
    unsigned v1 = 0x3ff;
    unsigned v2 = 0x30;
    unsigned v3 = 0x8030;
    unsigned cnt = 0;
    unsigned t = 0;
    int mem = 0;
    long rev = g_c1e0 * 0xff / 0x63;
    int cho = (g_c1e2 * 0xff / 0x63) << 8;

    f_721c(0x3d, g_c18b + 0x400, 0x59);
    f_721c(0x3e, g_c18b + 0x400, 0x20);
    f_721c(0x3f, g_c18b + 0x400, 0);
    for (i = 0; i < 0x20; i++) {
        f_721c(i + 0xa0, g_c18b + 0x400, 0x80);
        f_721c(i + 0xc0, g_c18b + 0x402, 0);
        f_721c(i + 0xe0, g_c18b + 0x400, 0);
        f_721c(i, g_c18b + 0x800, 0xd000);
        f_721c(i + 0x20, g_c18b + 0x800, 0xff00);
        f_721c(i + 0x40, g_c18b + 0x800, 0);
        f_721c(i + 0x60, g_c18b + 0x800, 0);
        f_721c(i + 0x80, g_c18b + 0x800, 0x18);
        f_721c(i + 0xa0, g_c18b + 0x800, 0x18);
        f_721c(i + 0xc0, g_c18b + 0x800, 0);
        f_721c(i + 0xe0, g_c18b + 0x402, 0);
        f_721c(i + 0xa0, g_c18b + 0x402, 0);
        f_721c(i + 0x80, g_c18b + 0x402, 0);
        f_721c(i + 0x80, g_c18b + 0x400, 0);
        f_721c(i + 0xc0, g_c18b + 0x400, 0);
    }
    f_723b(0x3b, g_c18b + 0x402);
    f_723b(0x3b, g_c18b + 0x402);
    f_723b(0x3b, g_c18b + 0x402);
    for (i = 0; i < 0x20; i++) {
        f_71ec(i + 0x20, g_c18b, 0L);
        f_71ec(i + 0x60, g_c18b, 0xffffL);
        f_71ec(i + 0xc0, g_c18b, 0L);
        f_71ec(i + 0xe0, g_c18b, 0L);
        f_71ec(i, g_c18b, 0L);
        f_71ec(i + 0x40, g_c18b, 0xffffL);
        f_71ec(i, g_c18b + 0x400, 0L);
        f_71ec(i + 0xa0, g_c18b, 0L);
        f_71ec(i + 0x80, g_c18b, 0L);
        f_721c(i + 0xe0, g_c18b + 0x400, 0x807f);
        f_721c(i + 0xa0, g_c18b + 0x400, 0x807f);
    }
    f_71ec(0x34, g_c18b + 0x400, 0L);
    f_71ec(0x35, g_c18b + 0x400, 0L);
    f_71ec(0x36, g_c18b + 0x400, 0L);
    f_71ec(0x35, g_c18b + 0x400, 0L);

    j = 1;
    for (i = 0; i < 0x20; i += 2) {
        f_721c(i + 0x40, g_c18b + 0x400, v1);
        f_721c(j + 0x40, g_c18b + 0x400, v2);
        v1 += 0x400;
        v2 += 0x100;
        j += 2;
    }
    j = 1;
    v2 = 0x30;
    for (i = 0; i < 0x20; i += 2) {
        f_721c(i + 0x40, g_c18b + 0x402, v1);
        f_721c(j + 0x40, g_c18b + 0x402, v2);
        v1 += 0x400;
        v2 += 0x100;
        j += 2;
    }
    j = 1;
    v2 = 0x30;
    for (i = 0; i < 0x20; i += 2) {
        f_721c(i + 0x60, g_c18b + 0x400, v1);
        f_721c(j + 0x60, g_c18b + 0x400, v2);
        v1 += 0x400;
        v2 += 0x100;
        j += 2;
    }
    j = 1;
    v2 = 0x30;
    for (i = 0; i < 0x20; i += 2) {
        f_721c(i + 0x60, g_c18b + 0x402, v1);
        f_721c(j + 0x60, g_c18b + 0x402, v2);
        v1 += 0x400;
        v2 += 0x100;
        j += 2;
    }
    for (i = 0; i < 0x14; i++)
        f_723b(0x3b, g_c18b + 0x402);

    cnt = 0;
    outpw(g_c18b + 0x802, 0x3b);
    t = inpw(g_c18b + 0x402);
    t += 0x400;
    if (t - 0x400 < t) {
        while (inpw(g_c18b + 0x402) > t && cnt < 0xfff0)
            cnt++;
    }
    cnt = 0;
    while (inpw(g_c18b + 0x402) < t && cnt < 0xfff0)
        cnt++;

    j = 1;
    v1 = 0x3ff;
    v3 = 0x8030;
    for (i = 0; i < 0x20; i += 2) {
        f_721c(i + 0x40, g_c18b + 0x400, v1);
        f_721c(j + 0x40, g_c18b + 0x400, v3);
        v1 += 0x400;
        v3 += 0x100;
        j += 2;
    }
    j = 1;
    v3 = 0x8030;
    for (i = 0; i < 0x20; i += 2) {
        f_721c(i + 0x40, g_c18b + 0x402, v1);
        f_721c(j + 0x40, g_c18b + 0x402, v3);
        v1 += 0x400;
        v3 += 0x100;
        j += 2;
    }
    j = 1;
    v3 = 0x8030;
    for (i = 0; i < 0x20; i += 2) {
        f_721c(i + 0x60, g_c18b + 0x400, v1);
        f_721c(j + 0x60, g_c18b + 0x400, v3);
        v1 += 0x400;
        v3 += 0x100;
        j += 2;
    }
    j = 1;
    v3 = 0x8030;
    for (i = 0; i < 0x20; i += 2) {
        f_721c(i + 0x60, g_c18b + 0x402, v1);
        f_721c(j + 0x60, g_c18b + 0x402, v3);
        v1 += 0x400;
        v3 += 0x100;
        j += 2;
    }
    for (i = 0; i < 0x14; i++)
        f_71ec(i + 0x20, g_c18b + 0x400, 0L);

    for (i = 0; i < 0x20; i++)
        f_721c(i + 0x40, g_c18b + 0x400, g_b3f2[i]);
    for (i = 0; i < 0x20; i++)
        f_721c(i + 0x40, g_c18b + 0x402, g_b432[i]);
    for (i = 0; i < 0x20; i++)
        f_721c(i + 0x60, g_c18b + 0x400, g_b472[i]);
    for (i = 0; i < 0x20; i++)
        f_721c(i + 0x60, g_c18b + 0x402, g_b4b2[i]);

    f_71ec(0x29, g_c18b + 0x400, 0L);
    f_71ec(0x2a, g_c18b + 0x400, 0x83L);
    f_71ec(0x2d, g_c18b + 0x400, 0x8000L);
    f_71ec(0x2e, g_c18b + 0x400, 0L);

    g_b3f2[13] = 0xf7c;
    g_b3f2[15] = 0x7254;
    g_b3f2[21] = 0xbaa;
    g_b3f2[23] = 0x7234;
    g_b3f2[29] = 0x6e7;
    g_b3f2[31] = 0x7224;
    g_b432[5] = 0x7f6;
    g_b432[7] = 0x7254;
    g_b432[13] = 0xf02;
    g_b432[15] = 0x7264;
    g_b432[21] = 0xfa9;
    g_b432[23] = 0x7294;
    g_b432[29] = 0x44c3;
    g_b432[31] = 0x45c3;
    g_b472[5] = 0x671;
    g_b472[7] = 0x287;
    g_b472[11] = 0xc7b;
    g_b472[13] = 0x7e6;
    g_b472[21] = 0x21f;
    g_b472[23] = 0x386;
    g_b472[25] = 0xc03;
    g_b472[27] = 0x31e;
    g_b472[31] = 0xc55;
    g_b4b2[5] = 0xead;
    g_b4b2[11] = 0x129;
    g_b4b2[15] = 0x31f;
    g_b4b2[19] = 0x327f;
    g_b4b2[21] = 0x265;
    g_b4b2[27] = 0x337f;

    for (i = 0; i < 0x20; i++)
        f_721c(i + 0x40, g_c18b + 0x400, g_b3f2[i]);
    for (i = 0; i < 0x20; i++)
        f_721c(i + 0x40, g_c18b + 0x402, g_b432[i]);
    for (i = 0; i < 0x20; i++)
        f_721c(i + 0x60, g_c18b + 0x400, g_b472[i]);
    for (i = 0; i < 0x20; i++)
        f_721c(i + 0x60, g_c18b + 0x402, g_b4b2[i]);

    rev = rev << 24;
    f_721c(0xbe, g_c18b + 0x400, 0x80);
    f_71ec(0xde, g_c18b, 0xffffffe0L);
    f_71ec(0xfe, g_c18b, 0xffffe8L | rev);
    f_71ec(0x3e, g_c18b, (long)cho);
    f_71ec(0x1e, g_c18b, 0L);
    f_71ec(0x1e, g_c18b + 0x400, 0xffffe3L);
    f_721c(0xbf, g_c18b + 0x400, 0x80);
    f_71ec(0xdf, g_c18b, 0xfffff0L);
    f_71ec(0xff, g_c18b, 0xfffff8L | rev);
    f_71ec(0x3f, g_c18b, (long)(0xff | cho));
    f_71ec(0x1f, g_c18b, 0x8000L);
    f_71ec(0x1f, g_c18b + 0x400, 0xfffff3L);
    f_71ec(0x3e, g_c18b, (unsigned long)(a == 1 ? 0x4828u : 0x4800u) << 16);
    outpw(g_c18b + 0x802, 0x3c);
    outpw(g_c18b + 0x400, 0);
    f_71ec(0x7e, g_c18b, 0xffffffffL);
    f_71ec(0x7f, g_c18b, 0xffffffffL);
    f_721c(0x3e, g_c18b + 0x400, 0x20);
    f_721c(0x3f, g_c18b + 0x400, 0);
    mem = f_6f77();
    f_721c(0x3f, g_c18b + 0x400, 4);
    outpw(g_c18b + 0x802, 0x3e);
    outpw(g_c18b + 0x802, 0xa0);
    outpw(g_c18b + 0x802, 0x3e);
    outpw(g_c18b + 0x802, 0xfe);
    return mem;
}
#undef g_c18b

/* ======== chunk 7 ======== */
#define g_c191 (*(unsigned *)&g_c191)
#define g_c195 (*(unsigned *)&g_c195)
/* switch PnP to alternate address port (Crystal key, then use g_c199) */
void f_7d59(void)
{
    f_3c59(2, 2);
    delay(2);
    f_7d81();
    g_c1b9 = g_c199;
    g_c1bb = g_c199 + 1;
}

/* send Crystal key sequence + alternate address 4 times */
void f_7d81(void)
{
    unsigned char key[32] = {
        0x66, 0xa1, 0xc2, 0xf1, 0xea, 0xe7, 0x71, 0xaa, 0xc7, 0x63, 0x33, 0x1b,
        0x0d, 0x96, 0xdb, 0x6d, 0xa4, 0x50, 0x28, 0x16, 0x9b, 0x4d, 0xb6, 0xc9,
        0xf4, 0x78, 0x3e, 0x8d, 0xd6, 0xfb, 0x7f, 0x3d };
    int i = 0;
    int j = 0;
    for (j = 0; j < 4; j++) {
        for (i = 0; i < 32; i++)
            outportb(g_c1b9, key[i] ^ j);
        delay(1);
        outportb(g_c1b9, g_c199 & 0xff);
        outportb(g_c1b9, (g_c199 >> 8) & 0xff);
    }
}

/* upload CS4232 firmware */
void f_7de9(void)
{
    int i = 0;
    printf("Loading CS4232 Firmware");
    f_3cfd(g_c191, 0x0c, 0xc0);
    putchar('.');
    for (i = 0; i < 8; i++)
        outportb(g_c195 + 5, g_b4f2[i]);
    outportb(g_c195 + 6, 0);
    delay(2);
    putchar('.');
    for (i = 0; i < 0xc8d; i++)
        outportb(g_c195 + 5, g_b4fa[i]);
    outportb(g_c195 + 6, 0);
    delay(2);
    putchar('.');
    f_3cfd(g_c191, 0x0c, 0);
    puts(" done.");
}

/* apply Orpheus/special card settings (FM, S/PDIF, volumes) */
void f_7ed9(int card)
{
    unsigned char ctl = 0;
    unsigned char vol1 = 0;
    unsigned char vol2 = 0;
    int surr;
    int val;
    int i;

    surr = (g_c1f8 * 15 / 9) | (~(g_c1f8 * 15 / 9) << 4);
    if (g_c1f8 < 1)
        surr = 0;
    vol1 = (100 - g_c1c0) * 63 / 97;
    vol2 = (100 - g_c1c6) * 63 / 97;
    if (vol1 > 63)
        vol1 = 63;
    if (vol2 > 63)
        vol2 = 63;
    if (card == 2)
        printf("Orpheus");
    else {
        printf("Special");
        if (g_c1e6 == -1)
            g_c1e6 = 1;
    }
    printf(" settings: ");
    f_3d19(g_c191, 0x0c, 0xea);
    f_3d19(g_c191, 0x17, 0x48);
    delay(5);
    ctl = inportb(g_c191 + 1);
    if (card == 2 || card == 7) {
        if (g_c1e6 == 1) {
            f_3d19(g_c191, 0x17, 0x48);
            outportb(g_c191 + 1, ctl | 0x10);
            printf("FM: [CrystalFM]  ");
        } else {
            f_3d19(g_c191, 0x17, 0x48);
            outportb(g_c191 + 1, ctl & 0xef);
            delay(5);
            if (card == 2)
                f_849f(8, 8);
            printf("FM: [Yamaha OPL3]  ");
        }
    }
    delay(10);
    if (card == 2 || card == 3) {
        if (g_c1e8 == 1) {
            f_849f(8, 4);
            f_3d19(g_c191, 0x4c, 0x60);
            f_3d19(g_c191, 0x10, 0x02);
            delay(10);
            f_3d19(g_c191, 0x51, 0x02);
            delay(5);
            f_3d19(g_c191, 0x50, 0x82);
            delay(10);
            f_3d19(g_c191, 0x5c, 0x50);
            f_3d19(g_c191, 0x48, 0x5b);
            delay(5);
            f_3d19(g_c191, 0x49, 0x00);
            f_3d19(g_c191, 0x4a, 0x06);
            f_3d19(g_c191, 0x4d, 0x01);
            f_3d19(g_c191, 0x40, 0x00);
            f_3d19(g_c191, 0x41, 0x00);
            delay(5);
            f_8474(4, 0x40);
            f_8474(3, 0x20);
            f_8474(6, 0x00);
            f_8474(5, 0x04);
            f_8474(4, 0x80);
            f_8474(4, 0xc0);
            f_8474(2, 0x77);
            f_8474(3, 0xa0);
            delay(20);
            f_3d19(g_c191, 0x1a, 0xc2);
            delay(60);
            f_3d19(g_c191, 0x0f, 0xaf);
            f_3d19(g_c191, 0x0e, 0x02);
            f_3d19(g_c191, 0x48, 0x5b);
            delay(5);
            f_3d19(g_c191, 0x49, 0x00);
            f_3d19(g_c191, 0x4a, 0x06);
            delay(5);
            f_84e1(0x0b, 0xe0);
            f_84e1(0x0d, 0x60);
            delay(10);
            f_8474(2, surr);
            printf("S/PDIF: [ON]");
        } else {
            f_3d19(g_c191, 0x10, 0x00);
            delay(10);
            f_84e1(0x0b, 0xc0);
            f_84e1(0x0d, 0x18);
            f_84e1(0x0a, 0x3f);
            f_3d19(g_c191, 0x5c, 0x00);
            f_3d19(g_c191, 0x4d, 0x00);
            f_3d19(g_c191, 0x50, 0x00);
            delay(20);
            f_3d19(g_c191, 0x51, 0x00);
            f_8474(2, surr);
            f_8474(3, g_c1f8 > 0 ? 0x80 : 0);
            f_8474(4, 0x00);
            printf("S/PDIF: [OFF]");
        }
    }
    delay(10);
    f_3d19(g_c191, 0x06, vol2);
    f_3d19(g_c191, 0x07, vol2);
    f_84e1(0x0e, vol1);
    f_84e1(0x0f, vol1);
    if ((card == 2 || card == 3) && g_c1e8 == 1) {
        val = 0;
        i = 0;
        if (g_c1c9 > 0) {
            for (i = 2; i < 4; i++) {
                outportb(g_c191, i);
                val = inportb(g_c191 + 1);
                outportb(g_c191 + 1, (val & 0x1f) | 0xa0);
            }
        }
        if (g_c1cc > 0) {
            for (i = 4; i < 6; i++) {
                outportb(g_c191, i);
                val = inportb(g_c191 + 1);
                outportb(g_c191 + 1, (val & 0x1f) | 0xa0);
            }
        }
        if (g_c1cf > 0) {
            for (i = 2; i < 4; i++) {
                f_3d19(g_c191, 0x17, (i << 4) | 8);
                val = inportb(g_c191 + 1);
                outportb(g_c191 + 1, (val & 0x1f) | 0xa0);
            }
        }
        if (g_c1c3 > 0) {
            for (i = 0x12; i < 0x14; i++) {
                outportb(g_c191, i);
                val = inportb(g_c191 + 1);
                outportb(g_c191 + 1, (val & 0x1f) | 0xa0);
            }
        }
        if (g_c1cc > 0)
            f_3cfd(g_c187, 0x0c, 3);
        if (g_c1c9 > 0)
            f_3cfd(g_c187, 0x0c, 7);
    }
    puts("");
}
#undef g_c191
#undef g_c195

/* ======== chunk 8 ======== */
#define g_c187 (*(unsigned *)&g_c187)
#define g_c191 (*(unsigned *)&g_c191)
#define g_c195 (*(unsigned *)&g_c195)
#define g_c197 (*(unsigned *)&g_c197)
#define g_c1ad (*(unsigned *)&g_c1ad)
/* indexed config register write via g_c195 */
void f_8474(unsigned char reg, unsigned char val)
{
    outportb(g_c195 + 3, reg);
    delay(2);
    outportb(g_c195 + 4, val);
    delay(5);
}

/* indexed config register: set bits (read-modify-write) */
void f_849f(unsigned char reg, unsigned char bits)
{
    unsigned char v = 0;
    outportb(g_c195 + 3, reg);
    v = inportb(g_c195 + 4);
    delay(2);
    outportb(g_c195 + 4, v | bits);
    delay(5);
}

/* select logical unit via reg 0x17 then write data port */
void f_84e1(unsigned char unit, unsigned char val)
{
    f_3d19(g_c191, 0x17, (unit << 4) | 8);
    outportb(g_c191 + 1, val);
    delay(5);
}

/* OPL (AdLib) timer detection at 0x388 */
int f_850e(void)
{
    int s1 = 0;
    int s2 = 0;
    int found = 0;
    f_85ba(4, 0x60);
    f_85ba(4, 0x80);
    delay(10);
    s1 = inportb(0x388);
    f_85ba(2, 0xff);
    f_85ba(4, 0x21);
    delay(10);
    s2 = inportb(0x388);
    f_85ba(4, 0x60);
    f_85ba(4, 0x80);
    s1 = s1 & 0xe0;
    s2 = s2 & 0xe0;
    if (s1 == 0 && s2 == 0xc0)
        found = 1;
    else
        found = 0;
    return found;
}

/* OPL register write */
void f_85ba(unsigned char reg, unsigned char val)
{
    outportb(0x388, reg);
    delay(6);
    outportb(0x389, val);
    delay(8);
}

/* send alternate PnP key and isolate card at given read port */
void f_85dd(int port)
{
    unsigned char key[34] = {
        0x00, 0x00, 0xda, 0x6d, 0x36, 0x1b, 0x8d, 0x46, 0x23, 0x91, 0x48, 0xa4,
        0xd2, 0x69, 0x34, 0x9a, 0x4d, 0x26, 0x13, 0x89, 0x44, 0xa2, 0x51, 0x28,
        0x94, 0xca, 0x65, 0x32, 0x19, 0x0c, 0x86, 0x43, 0xa1, 0x50 };
    int i = 0;
    unsigned char buf[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    f_3c59(2, 2);
    delay(2);
    for (i = 0; i < 34; i++)
        outportb(g_c1b9, key[i]);
    delay(1);
    f_3c59(3, 0);
    delay(2);
    f_3c59(0, (port >> 1) >> 1);
    delay(1);
    f_3e51(port, buf);
    f_3c59(6, 0x81);
    delay(2);
    f_3c59(0x30, 0x81);
    delay(2);
    f_3c59(2, 2);
    delay(2);
}

/* serial bit-bang: write a register (start, address, data, stop) */
void f_86aa(unsigned char reg, unsigned char val)
{
    f_8745(1);
    f_8745(0);
    f_8745(2);
    f_86f1(0x80);
    f_86f1(reg);
    f_86f1(val);
    f_8745(1);
    f_8745(3);
    delay(2);
}

/* serial bit-bang: shift out one byte MSB first */
void f_86f1(unsigned char v)
{
    int i = 0;
    unsigned char b = 0;
    for (i = 7; i >= 0; i--) {
        b = ((v >> i) & 1) << 1;
        f_8745(1 | b);
        f_8745(b);
    }
    f_8745(3);
    f_8745(2);
}

/* serial bit-bang: output a line state (3 times for timing) */
void f_8745(unsigned char v)
{
    int i = 0;
    for (i = 0; i < 3; i++)
        outportb(g_c1ad, v);
    /* no code: marks BX/CX busy so BC 3.1 keeps i in SI as the original
       compiler did (BC 3.1 otherwise uses BX for register vars in leaf functions) */
    _BX; _CX;
}

/* OPTi 82C928 read: unlock password then read port */
unsigned char f_8760(unsigned port)
{
    delay(30);
    outportb(0xf8f, 0xe2);
    return inportb(port);
}

/* OPTi 82C928 write: unlock password then write port */
void f_8777(unsigned port, unsigned char val)
{
    outportb(0xf8f, 0xe2);
    outportb(port, val);
}

/* detect and configure OAK OTI601 / OPTi 82C928 */
int f_8789(void)
{
    unsigned char r2 = 0;
    unsigned char r1 = 0;
    unsigned char cfg1 = 0x80;
    unsigned char cfg2 = 0;
    unsigned char st = 0;
    r1 = f_8760(0xf8d);
    r2 = f_8760(0xf8f);
    if (r1 < 0xff && r2 < 0xff) {
        puts("Card detected: OAK OTI601 / OPTi 82C928");
        puts(f_4192() ? "Loading BLASTER values..." : "Loading default values...");
        if (g_c187 == 0x240 || g_c191 == 0xf40) {
            g_c191 = 0xf40;
            cfg1 |= 0x20;
        } else {
            g_c187 = 0x220;
            g_c191 = 0x530;
            cfg1 &= 0xcf;
        }
        if (g_c18f < 1)
            cfg1 |= 1;
        else {
            cfg1 &= 0xfe;
            g_c18f = 0x200;
        }
        switch (g_c19f) {
        case 11: cfg2 |= 0x40; break;
        case 7: break;
        default: g_c19f = 5; cfg2 |= 0x80;
        }
        switch (g_c1a7) {
        case 3: cfg2 |= 0x20; break;
        case 0: cfg2 |= 0x10; break;
        default: g_c1a7 = 1;
        }
        f_8777(0xf8d, 0x40);
        delay(120);
        f_8777(0xf8d, 0x80);
        delay(140);
        f_8777(0xf8e, 3);
        delay(50);
        f_8777(0xf8d, cfg1);
        delay(50);
        f_8777(0xf8f, cfg2);
        f_8777(0xf90, 2);
        delay(40);
        outportb(g_c191 + 4, 0x40);
        f_3d40(g_c191, 0x44, 0xf);
        f_3d40(g_c191, 0x45, 0xf);
        f_3d40(g_c191, 0x49, 0x40);
        delay(70);
        f_3d40(g_c191, 0x4a, 2);
        f_3d40(g_c191, 0x4d, 0);
        f_3d40(g_c191, 0x4c, 0x8a);
        f_3d40(g_c191, 0x48, 3);
        delay(60);
        f_3d40(g_c191, 0x4e, 0xff);
        g_c1c2 = g_c1c3 > 0 ? (100 - g_c1c3) * 31 / 97 : 0x80;
        g_c1c5 = g_c1c6 > 0 ? (100 - g_c1c6) * 63 / 97 : 0x80;
        g_c1c8 = g_c1c9 > 0 ? (100 - g_c1c9) * 31 / 97 : 0x80;
        f_8777(0xf91, 0x1a);
        outportb(g_c191 + 4, 0x2c);
        st = inportb(g_c191 + 5);
        f_3d40(g_c191, 0x24, g_c1c2);
        f_3d40(g_c191, 0x25, g_c1c2);
        f_3d40(g_c191, 0x26, g_c1c5);
        f_3d40(g_c191, 0x27, g_c1c5);
        f_3d40(g_c191, 0x22, g_c1c8);
        f_3d40(g_c191, 0x23, g_c1c8);
        if (g_c1f2 == 0)
            f_8777(0xf8d, f_8760(0xf8d) & 0x7f);
        delay(50);
        f_8777(0xf91, (st & 0xfd) == 2 ? 0x2a : 0x20);
        printf("Settings: ADD:%X IRQ:%d DMA:%d JOY:%X", g_c187, g_c19f, g_c1a7, g_c18f);
        if (g_c1f2 > 0)
            printf(" [WSS mode: %X]", g_c191);
        else {
            printf(" [SB Mode]");
            delay(20);
            outportb(g_c187 + 6, 1);
            delay(4);
            outportb(g_c187 + 6, 0);
            delay(20);
            outportb(g_c187 + 12, 0x20);
            printf("\nMixer [WAV:%d FM:%d LIN/CD:%d]\n", g_c1c6, g_c1c3, g_c1c9);
        }
        return 1;
    }
    return 0;
}

/* probe OPTi controller at 0xf8c and set subtype */
void f_8b2d(void)
{
    if (f_3d6c(0xf8c, 1) < 0xff && f_3d6c(0xf8c, 3) < 0xff) {
        f_3daa(0xf8c, 7, 0x40);
        g_c1fc = 2;
    }
}

/* detect and configure OPTi 83C930 */
int f_8b68(void)
{
    unsigned char cfg1 = 0;
    unsigned char cfg2 = 0;
    unsigned char cfg3 = 0x83;
    if (g_c1a3 < 0)
        g_c1a3 = 9;
    g_c197 = 0xe0c;
    g_c1e4 = 0xf8f;
    if (f_3e1c(0xe0e, 1) < 0xff && f_3e1c(0xe0e, 3) < 0xff && f_3e1c(0xe0e, 0x12) < 8) {
        puts("Card detected: OPTi 83C930");
        puts(f_4192() ? "Loading BLASTER values..." : "Loading default values...");
        if (g_c187 == 0x240)
            cfg1 |= 0x80;
        else {
            g_c187 = 0x220;
            cfg1 &= 0x7f;
        }
        switch (g_c191) {
        case 0xe80: cfg1 |= 0x10; break;
        case 0x604: cfg1 |= 0x30; break;
        default: g_c191 = 0x530; cfg1 &= 0xcf;
        }
        if (g_c18f < 1) {
            cfg1 |= 1;
            g_c18f = 0;
        } else {
            cfg1 &= 0xfe;
            g_c18f = 0x200;
        }
        switch (g_c19f) {
        case 11: cfg2 |= 0x20; break;
        case 10: cfg2 |= 0x18; break;
        case 9: cfg2 |= 0x10; break;
        case 7: cfg2 |= 8; break;
        default: g_c19f = 5; cfg2 |= 0x28;
        }
        switch (g_c1a7) {
        case 3: cfg2 |= 3; break;
        case 0: cfg2 |= 1; break;
        default: g_c1a7 = 1; cfg2 |= 2;
        }
        switch (g_c18d) {
        case 0: cfg3 &= 0xef; break;
        case 0x300: cfg3 |= 0x60; break;
        case 0x310: cfg3 |= 0x40; break;
        case 0x320: cfg3 |= 0x20; break;
        default: g_c18d = 0x330;
        }
        switch (g_c1a3) {
        case 2: case 7: cfg3 |= 0x18; break;
        case 5: cfg3 |= 0x10; break;
        case 10: cfg3 |= 8; break;
        default: g_c1a3 = 9;
        }
        delay(40);
        f_3def(g_c197 + 2, 2, 0);
        f_3def(g_c197 + 2, 1, cfg1);
        f_3def(g_c197 + 2, 3, cfg2);
        f_3def(g_c197 + 2, 4, 0x52);
        f_3def(g_c197 + 2, 5, 0x3d);
        f_3def(g_c197 + 2, 6, cfg3);
        f_3def(g_c197 + 2, 0xa, 0x10);
        delay(20);
        f_3d40(g_c191, 0x29, 0);
        f_3d40(g_c191, 0x2a, 2);
        f_3d40(g_c191, 0x2b, 0);
        f_3d40(g_c191, 0x48, 3);
        delay(100);
        f_3def(g_c197 + 2, 3, cfg2);
        f_3d40(g_c191, 0x30, ((100 - g_c1c3) * 15 / 97) << 1);
        f_3d40(g_c191, 0x31, ((100 - g_c1c3) * 15 / 97) << 1);
        f_3d40(g_c191, 0x20, 0x89);
        f_3d40(g_c191, 0x21, 0x89);
        f_3d40(g_c191, 0x36, 7);
        f_3d40(g_c191, 0x37, 7);
        delay(10);
        f_3d40(g_c191, 0x26, 0x84);
        f_3d40(g_c191, 0x27, 0x84);
        delay(80);
        f_54e9();
        printf("Settings: ADD:%X IRQ:%d DMA:%d WSS:%X", g_c187, g_c19f, g_c1a7, g_c191);
        printf(" MPU:%X/I%d JOY:%X", g_c18d, g_c1a3, g_c18f);
        puts("");
        f_5419("OPTi", -1, -1);
        return 1;
    }
    return 0;
}

/* detect and configure ESS ES668 / ES1688 */
int f_8f2f(void)
{
    int port = 0x220;
    int irqv = 0x16;
    int dmav = 0x1a;
    int mpu = 0;
    unsigned char ver = 0;
    for (port = 0x220; port <= 0x260; port += 0x10) {
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
        outportb(port + 0xc, 0xc6);
        outportb(port + 4, 0x40);
        if (inportb(port + 5) != 0x18 || inportb(port + 5) < 0x68) {
            printf("Card detected: ES");
            printf(ver < 0x88 ? "668" : "1688");
            printf(" at [%xh] \n", port);
            puts(f_4192() ? "Loading BLASTER values..." : "Loading default values...");
            g_c187 = port;
            delay(50);
            switch (g_c19f) {
            case 2: case 9: irqv -= 4; break;
            case 7: irqv += 4; break;
            case 10: irqv += 8; break;
            default: g_c19f = 5;
            }
            f_3d40(g_c187 + 8, 0xb1, irqv);
            switch (g_c1a7) {
            case 0: dmav -= 4; break;
            case 3: dmav += 4; break;
            default: g_c1a7 = 1;
            }
            f_3d40(g_c187 + 8, 0xb2, dmav);
            if (ver >= 0x88) {
                switch (g_c1a3) {
                case 2: case 9: mpu |= 0x80; break;
                case 5: mpu |= 0xa0; break;
                case 7: mpu |= 0xc0; break;
                case 10: mpu |= 0xe0; break;
                default: g_c1a3 = g_c19f; mpu |= 0x40;
                }
                if (g_c18f > 0) {
                    mpu |= 2;
                    g_c18f = 0x201;
                } else
                    g_c18f = 0;
                mpu |= ((g_c18d & 0xf0) >> 1) | 1;
                f_3d40(g_c187, 0x40, mpu);
            }
            printf("Settings: ADD:%X IRQ:%d DMA:%d", g_c187, g_c19f, g_c1a7);
            if (ver >= 0x88)
                printf(" MPU:%X/I%d JOY:%X", g_c18d, g_c1a3, g_c18f);
            puts("");
            f_54e9();
            f_5419("ESS", -1, -1);
            return 1;
        }
    }
    return 0;
}
#undef g_c187
#undef g_c191
#undef g_c195
#undef g_c197
#undef g_c1ad
