/* JAZZPLAY BASE: play one 8-bit and one 16-bit single-cycle DMA block through a Media Vision
   Jazz16 (MVD1216) at BASE (hex) and show which IRQ (2/9, 3, 5, 7, 10, 15) and which DMA channel
   each one really uses. The Jazz16 has no SB16 commands: 16-bit mono is DSP command A4h, then the
   SB 2.0 single-cycle command 14h with the length in samples - 1, on the 16-bit DMA channel (Linux
   sound/isa/sb/sb8_main.c, 86Box src/sound/snd_sb_dsp.c). Every channel the card can use (1, 3 for
   8-bit; 1, 3, 5, 7 for 16-bit) is programmed with the same buffer and handlers sit on all six
   IRQs; only the channel the card drives reaches terminal count, and its count of bytes left
   (shown after each channel) moves. 86Box takes the 14h length as bytes when the 16-bit DMA is an
   8-bit channel, so there the 16-bit block stops halfway. An interrupt pending when the IRQs are
   unmasked is shown as "stray irq" and not counted. DSP command A0h (8-bit mono) is sent again at
   the end.
   Build: tcc JAZZPLAY.C */
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>

#define LEN 4000 /* samples per block */
unsigned base;
volatile int hit[16];
unsigned char buf[4 * LEN + 16];
unsigned long phys;
static int irqs[6] = {3, 5, 7, 9, 10, 15};
static unsigned page8[4] = {0x87, 0x83, 0x81, 0x82};
static unsigned page16[4] = {0x8f, 0x8b, 0x89, 0x8a};
void interrupt (*old[16])(void);

void ack(int irq)
{
    hit[irq]++;
    inportb(base + 0xe);
    if (irq >= 8) {
        outportb(0xa0, 0x20);
    }
    outportb(0x20, 0x20);
}
void interrupt isr3(void)
{
    ack(3);
}
void interrupt isr5(void)
{
    ack(5);
}
void interrupt isr7(void)
{
    ack(7);
}
void interrupt isr9(void)
{
    ack(9);
}
void interrupt isr10(void)
{
    ack(10);
}
void interrupt isr15(void)
{
    ack(15);
}

int vec(int irq)
{
    return irq < 8 ? 8 + irq : 0x70 + irq - 8;
}

int dsp_write(unsigned char v)
{
    long i;
    for (i = 0; i < 100000L; i++) {
        if (!(inportb(base + 0xc) & 0x80)) {
            outportb(base + 0xc, v);
            return 1;
        }
    }
    return 0;
}

int dsp_read(void)
{
    long i;
    for (i = 0; i < 100000L; i++) {
        if (inportb(base + 0xe) & 0x80) {
            return inportb(base + 0xa);
        }
    }
    return -1;
}

int dsp_reset(void)
{
    int k;
    outportb(base + 6, 1);
    delay(1);
    outportb(base + 6, 0);
    for (k = 0; k < 100 && !(inportb(base + 0xe) & 0x80); k++) {
        delay(1);
    }
    return inportb(base + 0xa);
}

int nhits(void)
{
    int i, n = 0;
    for (i = 0; i < 6; i++) {
        n += hit[irqs[i]];
    }
    return n;
}

void clear_hits(void)
{
    int i;
    delay(10);
    if (nhits()) {
        printf("stray irq:");
        for (i = 0; i < 6; i++) {
            printf(" %d=%d", irqs[i], hit[irqs[i]]);
        }
        puts("");
    }
    for (i = 0; i < 16; i++) {
        hit[i] = 0;
    }
}

/* Program channel c (0-7) for a single-cycle read of n bytes from phys. */
void dma_prog(int c, unsigned n)
{
    if (c < 4) {
        outportb(0x0a, 4 | c);
        outportb(0x0c, 0);
        outportb(0x0b, 0x48 | c);
        outportb(c * 2, (unsigned char)phys);
        outportb(c * 2, (unsigned char)(phys >> 8));
        outportb(page8[c], (unsigned char)(phys >> 16));
        outportb(c * 2 + 1, (n - 1) & 0xff);
        outportb(c * 2 + 1, (n - 1) >> 8);
        outportb(0x0a, c);
    } else {
        unsigned w = (unsigned)(phys >> 1);
        c -= 4;
        outportb(0xd4, 4 | c);
        outportb(0xd8, 0);
        outportb(0xd6, 0x48 | c);
        outportb(0xc0 + c * 4, w & 0xff);
        outportb(0xc0 + c * 4, w >> 8);
        outportb(page16[c], (unsigned char)(phys >> 16) & 0xfe);
        outportb(0xc2 + c * 4, (n / 2 - 1) & 0xff);
        outportb(0xc2 + c * 4, (n / 2 - 1) >> 8);
        outportb(0xd4, c);
    }
}

/* Bytes channel c (0-7) still has to transfer, minus 1 (FFFFh after terminal count). */
unsigned dma_left(int c)
{
    unsigned lo, n;
    if (c < 4) {
        outportb(0x0c, 0);
        lo = inportb(c * 2 + 1);
        return lo | (inportb(c * 2 + 1) << 8);
    }
    c -= 4;
    outportb(0xd8, 0);
    lo = inportb(0xc2 + c * 4);
    n = lo | (inportb(0xc2 + c * 4) << 8);
    return n == 0xffff ? n : n * 2 + 1;
}

void dma_off(void)
{
    int c;
    for (c = 0; c < 4; c++) {
        outportb(0x0a, 4 | c);
        outportb(0xd4, 4 | c);
    }
}

/* Play one block: 8-bit (A0h, n = LEN bytes) or 16-bit (A4h, LEN words) and report. */
void play(int bits)
{
    static int ch8[2] = {1, 3};
    static int ch16[4] = {1, 3, 5, 7};
    int *ch = bits == 16 ? ch16 : ch8;
    int nch = bits == 16 ? 4 : 2;
    unsigned bytes = bits == 16 ? 2 * LEN : LEN;
    unsigned char st1, st2;
    long t0;
    int i, irq = -1, dma = -1;
    clear_hits();
    inportb(0x08);
    inportb(0xd0);
    for (i = 0; i < nch; i++) {
        dma_prog(ch[i], bytes);
    }
    dsp_write(0xd1);
    dsp_write(0x40);
    dsp_write(0xa5);
    dsp_write(bits == 16 ? 0xa4 : 0xa0);
    dsp_write(0x14);
    dsp_write((LEN - 1) & 0xff);
    dsp_write((LEN - 1) >> 8);
    t0 = biostime(0, 0L);
    while (biostime(0, 0L) - t0 < 18 && nhits() == 0) {
    }
    st1 = inportb(0x08);
    st2 = inportb(0xd0);
    printf("%d-bit irq:", bits);
    for (i = 0; i < 6; i++) {
        printf(" %d=%d", irqs[i], hit[irqs[i]]);
        if (hit[irqs[i]]) {
            irq = irqs[i];
        }
    }
    printf(" dma:");
    for (i = 0; i < nch; i++) {
        int c = ch[i];
        int tc = c < 4 ? (st1 >> c) & 1 : (st2 >> (c - 4)) & 1;
        printf(" %d=%s/%04X", c, tc ? "TC" : "--", dma_left(c));
        if (tc) {
            dma = c;
        }
    }
    puts("");
    printf("RESULT %d-bit: IRQ %d DMA %d (-1 = none)\n", bits, irq, dma);
    dma_off();
    for (i = 0; i < 16; i++) {
        hit[i] = 0;
    }
}

int main(int argc, char *argv[])
{
    int i, n;
    unsigned char m1, m2;
    unsigned char *p;
    unsigned long a;
    if (argc < 2) {
        puts("usage: JAZZPLAY BASE");
        return 1;
    }
    base = (unsigned)strtoul(argv[1], 0, 16);
    /* a buffer of 2 * LEN bytes that crosses no 64K boundary (128K for 16-bit) */
    a = ((unsigned long)FP_SEG(buf) << 4) + FP_OFF(buf);
    p = buf;
    if (a & 1) {
        p++;
        a++;
    }
    if ((a & 0xffff) + 2L * LEN > 0x10000L) {
        n = (int)(0x10000L - (a & 0xffff));
        p += n;
        a += n;
    }
    phys = a;
    for (i = 0; i < 2 * LEN; i++) {
        p[i] = (i / 20) & 1 ? 0x30 : 0xd0;
    }
    printf("reset %02X\n", dsp_reset());
    n = -1;
    i = -1;
    if (dsp_write(0xe1)) {
        n = dsp_read();
        i = dsp_read();
    }
    printf("DSP version %d.%02d", n, i);
    if (dsp_write(0xfa)) {
        printf(", FAh %02X", dsp_read());
    }
    puts("");
    old[3] = getvect(vec(3));
    setvect(vec(3), isr3);
    old[5] = getvect(vec(5));
    setvect(vec(5), isr5);
    old[7] = getvect(vec(7));
    setvect(vec(7), isr7);
    old[9] = getvect(vec(9));
    setvect(vec(9), isr9);
    old[10] = getvect(vec(10));
    setvect(vec(10), isr10);
    old[15] = getvect(vec(15));
    setvect(vec(15), isr15);
    m1 = inportb(0x21);
    m2 = inportb(0xa1);
    outportb(0x21, m1 & ~((1 << 3) | (1 << 5) | (1 << 7) | (1 << 2)));
    outportb(0xa1, m2 & ~((1 << 1) | (1 << 2) | (1 << 7)));
    play(8);
    play(16);
    dsp_write(0xa0);
    dsp_write(0xd3);
    outportb(0x21, m1);
    outportb(0xa1, m2);
    setvect(vec(3), old[3]);
    setvect(vec(5), old[5]);
    setvect(vec(7), old[7]);
    setvect(vec(9), old[9]);
    setvect(vec(10), old[10]);
    setvect(vec(15), old[15]);
    return 0;
}
