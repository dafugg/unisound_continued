/* SBST BASE DMA MODE: SB Pro stereo/mono DMA test (8-bit single-cycle, DSP command 14h).
   MODE  S  set mixer 0Eh bit 1 (stereo), play 22050 bytes/s with every EVEN byte (the first of
            each pair) a 550 Hz square wave and every odd byte silence (80h); restore 0Eh after.
         D  as S, but first send one silent byte as its own 1-byte DMA transfer (the Creative
            "dummy byte" recommended before SB Pro stereo playback).
         M  do NOT touch mixer 0Eh (as a mono game would) and play the square wave as mono at
            11025 Hz.
   It prints mixer 0Eh before and after. The IRQ is not used: the program polls the DMA terminal
   count and then acknowledges the DSP by reading BASE+0Eh.
   Build: tcc SBST.C */
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>

#define LEN 22000u

unsigned sb;
int dma;
unsigned char buf[2 * LEN];
static unsigned char pagep[4] = {0x87, 0x83, 0x81, 0x82};

int dsp_w(unsigned char v)
{
    long t = 0;
    while (inportb(sb + 0xc) & 0x80) {
        if (++t > 100000L) {
            return -1;
        }
    }
    outportb(sb + 0xc, v);
    return 0;
}

unsigned char mix_r(unsigned char r)
{
    outportb(sb + 4, r);
    return inportb(sb + 5);
}

void mix_w(unsigned char r, unsigned char v)
{
    outportb(sb + 4, r);
    outportb(sb + 5, v);
}

void play(unsigned long phys, unsigned len, unsigned waitms)
{
    int i;
    outportb(0x0a, dma | 4);
    outportb(0x0c, 0);
    outportb(0x0b, 0x48 | dma);
    outportb(dma * 2, (unsigned char)phys);
    outportb(dma * 2, (unsigned char)(phys >> 8));
    outportb(pagep[dma], (unsigned char)(phys >> 16));
    outportb(dma * 2 + 1, (unsigned char)(len - 1));
    outportb(dma * 2 + 1, (unsigned char)((len - 1) >> 8));
    outportb(0x0a, dma);
    dsp_w(0x14);
    dsp_w((unsigned char)(len - 1));
    dsp_w((unsigned char)((len - 1) >> 8));
    for (i = 0; i < waitms / 10; i++) {
        if (inportb(0x08) & (1 << dma)) {
            break;
        }
        delay(10);
    }
    delay(5);
    inportb(sb + 0xe);
    outportb(0x0a, dma | 4);
}

int main(int argc, char *argv[])
{
    int i, ok = 0;
    char mode;
    unsigned char e0;
    unsigned long phys;
    unsigned char far *p;
    unsigned off;
    if (argc < 4) {
        printf("usage: SBST BASE DMA S|D|M\n");
        return 1;
    }
    sb = (unsigned)strtoul(argv[1], 0, 16);
    dma = atoi(argv[2]);
    mode = argv[3][0] & 0xdf;
    e0 = mix_r(0x0e);
    printf("mixer 0Eh before = %02X\n", e0);
    outportb(sb + 6, 1);
    delay(1);
    outportb(sb + 6, 0);
    for (i = 0; i < 1000; i++) {
        if ((inportb(sb + 0xe) & 0x80) && inportb(sb + 0xa) == 0xaa) {
            ok = 1;
            break;
        }
    }
    printf("DSP reset %s; mixer 0Eh after DSP reset = %02X\n", ok ? "OK" : "FAILED", mix_r(0x0e));
    if (!ok) {
        return 2;
    }
    p = (unsigned char far *)buf;
    phys = ((unsigned long)FP_SEG(p) << 4) + FP_OFF(p);
    if ((phys & 0xffffUL) + LEN > 0x10000UL) {
        p += LEN;
        phys += LEN;
    }
    dsp_w(0xd1);
    if (mode == 'M') {
        for (off = 0; off < LEN; off++) {
            p[off] = (off / 10) & 1 ? 0xc0 : 0x40;
        }
        dsp_w(0x40);
        dsp_w(165); /* 11025 Hz */
        play(phys, LEN, 3000);
    } else {
        p[0] = 0x80;
        mix_w(0x0e, mix_r(0x0e) | 0x02);
        if (mode == 'D') {
            play(phys, 1, 200);
        }
        for (off = 0; off < LEN; off++) {
            p[off] = (off & 1) ? 0x80 : ((off / 40) & 1 ? 0xc0 : 0x40);
        }
        dsp_w(0x40);
        dsp_w(211); /* 1000000/(256-211) = 22222 bytes/s = 11111 frames/s */
        play(phys, LEN, 3000);
        mix_w(0x0e, e0);
    }
    dsp_w(0xd3);
    printf("mode %c done; mixer 0Eh now = %02X\n", mode, mix_r(0x0e));
    return 0;
}
