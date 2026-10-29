/* DMAPLAY BASE [BLOCKS]: play 8-bit single-cycle DMA audio (DSP command 14h, 11 kHz, square
   wave) through the SB DSP at BASE and find out which IRQ (2/9, 5, 7, 10) and which 8-bit DMA
   channel (0, 1, 3) the card really uses: a handler is installed on all four IRQs and DMA
   channels 0, 1 and 3 are all programmed with the same buffer; only the channel the card
   drives reaches terminal count. With BLOCKS > 1 the found channel/IRQ is used to play that
   many more blocks (about 0.36 s each) for audio capture. Numbers: BASE hex, BLOCKS decimal.
   Build: tcc DMAPLAY.C */
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>

#define LEN 4000
unsigned base;
volatile int hit[16];
unsigned char buf[2 * LEN + 16];
unsigned long phys;
static int irqs[4] = {9, 5, 7, 10};
static int chans[3] = {0, 1, 3};
static unsigned pagep[4] = {0x87, 0x83, 0x81, 0x82};
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

void dma_prog(int c)
{
    outportb(0x0a, 4 | c);
    outportb(0x0c, 0);
    outportb(0x0b, 0x48 | c);
    outportb(c * 2, (unsigned char)phys);
    outportb(c * 2, (unsigned char)(phys >> 8));
    outportb(pagep[c], (unsigned char)(phys >> 16));
    outportb(0x0c, 0);
    outportb(c * 2 + 1, (LEN - 1) & 0xff);
    outportb(c * 2 + 1, (LEN - 1) >> 8);
    outportb(0x0a, c);
}

unsigned dma_count(int c)
{
    unsigned lo;
    outportb(0x0c, 0);
    lo = inportb(c * 2 + 1);
    return lo | (inportb(c * 2 + 1) << 8);
}

void dsp_play(void)
{
    dsp_write(0x14);
    dsp_write((LEN - 1) & 0xff);
    dsp_write((LEN - 1) >> 8);
}

int wait_irq(int before)
{
    long t0 = biostime(0, 0L);
    while (biostime(0, 0L) - t0 < 18) {
        int i, n = 0;
        for (i = 0; i < 4; i++) {
            n += hit[irqs[i]];
        }
        if (n > before) {
            return n;
        }
    }
    return before;
}

int main(int argc, char *argv[])
{
    int i, n, blocks = 1, irq = -1, dma = -1;
    unsigned char m1, m2, st;
    unsigned char *p;
    unsigned long a;
    if (argc < 2) {
        puts("usage: DMAPLAY BASE [BLOCKS]");
        return 1;
    }
    base = (unsigned)strtoul(argv[1], 0, 16);
    if (argc > 2) {
        blocks = atoi(argv[2]);
    }
    a = ((unsigned long)FP_SEG(buf) << 4) + FP_OFF(buf);
    p = buf;
    if ((a & 0xffff) + LEN > 0x10000L) {
        p += LEN;
        a += LEN;
    }
    phys = a;
    for (i = 0; i < LEN; i++) {
        p[i] = (i / 10) & 1 ? 0xc0 : 0x40;
    }
    printf("reset %02X\n", dsp_reset());
    old[5] = getvect(vec(5));
    setvect(vec(5), isr5);
    old[7] = getvect(vec(7));
    setvect(vec(7), isr7);
    old[9] = getvect(vec(9));
    setvect(vec(9), isr9);
    old[10] = getvect(vec(10));
    setvect(vec(10), isr10);
    m1 = inportb(0x21);
    m2 = inportb(0xa1);
    outportb(0x21, m1 & ~((1 << 5) | (1 << 7) | (1 << 2)));
    outportb(0xa1, m2 & ~((1 << 1) | (1 << 2)));
    inportb(0x08);
    for (i = 0; i < 3; i++) {
        dma_prog(chans[i]);
    }
    dsp_write(0xd1);
    dsp_write(0x40);
    dsp_write(0xa5);
    dsp_play();
    n = wait_irq(0);
    st = inportb(0x08);
    printf("irq:");
    for (i = 0; i < 4; i++) {
        printf(" %d=%d", irqs[i], hit[irqs[i]]);
        if (hit[irqs[i]]) {
            irq = irqs[i];
        }
    }
    printf("\ndma:");
    for (i = 0; i < 3; i++) {
        int c = chans[i];
        unsigned left = dma_count(c);
        printf(" %d=%s/%04X", c, (st >> c) & 1 ? "TC" : "--", left);
        if ((st >> c) & 1) {
            dma = c;
        }
    }
    puts("");
    if (irq >= 0 && dma >= 0) {
        for (i = 1; i < blocks; i++) {
            dma_prog(dma);
            dsp_play();
            n = wait_irq(n);
        }
        printf("played %d blocks on IRQ %d DMA %d, irqs %d\n", blocks, irq, dma, n);
    } else {
        printf("RESULT: IRQ %d DMA %d (-1 = none)\n", irq, dma);
    }
    dsp_write(0xd3);
    for (i = 0; i < 3; i++) {
        outportb(0x0a, 4 | chans[i]);
    }
    outportb(0x21, m1);
    outportb(0xa1, m2);
    setvect(vec(5), old[5]);
    setvect(vec(7), old[7]);
    setvect(vec(9), old[9]);
    setvect(vec(10), old[10]);
    return 0;
}
