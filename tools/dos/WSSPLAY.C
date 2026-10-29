/* WSSPLAY BASE: play one block of 8-bit mono 8 kHz audio through the AD1848-compatible codec at
   BASE+4 (WSS base, e.g. 530) and report which IRQ (5, 7, 9, 10, 11) fired and which 8-bit DMA
   channel (0, 1, 3) reached terminal count. All three channels are programmed with the same
   buffer; only the one the codec drives reaches TC. Also prints the WSS config byte at BASE.
   Build: tcc WSSPLAY.C */
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>

#define LEN 2000
unsigned base, cdc;
volatile int hit[16];
unsigned char buf[2 * LEN + 16];
unsigned long phys;
static int irqs[5] = {5, 7, 9, 10, 11};
static int chans[3] = {0, 1, 3};
static unsigned pagep[4] = {0x87, 0x83, 0x81, 0x82};
void interrupt (*old[16])(void);

void ack(int irq)
{
    hit[irq]++;
    outportb(cdc + 2, 0);
    if (irq >= 8)
        outportb(0xa0, 0x20);
    outportb(0x20, 0x20);
}
void interrupt isr5(void) { ack(5); }
void interrupt isr7(void) { ack(7); }
void interrupt isr9(void) { ack(9); }
void interrupt isr10(void) { ack(10); }
void interrupt isr11(void) { ack(11); }

int vec(int irq) { return irq < 8 ? 8 + irq : 0x70 + irq - 8; }

void wait_init(void)
{
    int k;
    for (k = 0; k < 1000 && (inportb(cdc) & 0x80); k++)
        delay(1);
}

void cw(unsigned char idx, unsigned char v)
{
    wait_init();
    outportb(cdc, idx);
    outportb(cdc + 1, v);
}

unsigned char cr(unsigned char idx)
{
    wait_init();
    outportb(cdc, idx);
    return inportb(cdc + 1);
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

int main(int argc, char *argv[])
{
    int i, irq = -1, dma = -1;
    unsigned char m1, m2, st;
    unsigned char *p;
    unsigned long a, t0;
    if (argc < 2) {
        puts("usage: WSSPLAY BASE");
        return 1;
    }
    base = (unsigned)strtoul(argv[1], 0, 16);
    cdc = base + 4;
    a = ((unsigned long)FP_SEG(buf) << 4) + FP_OFF(buf);
    p = buf;
    if ((a & 0xffff) + LEN > 0x10000L) {
        p += LEN;
        a += LEN;
    }
    phys = a;
    for (i = 0; i < LEN; i++)
        p[i] = (i / 8) & 1 ? 0xc0 : 0x40;
    printf("WSS cfg[%X]=%02X codec idx=%02X I12=%02X\n", base, inportb(base), inportb(cdc), cr(12));
    old[5] = getvect(vec(5)); setvect(vec(5), isr5);
    old[7] = getvect(vec(7)); setvect(vec(7), isr7);
    old[9] = getvect(vec(9)); setvect(vec(9), isr9);
    old[10] = getvect(vec(10)); setvect(vec(10), isr10);
    old[11] = getvect(vec(11)); setvect(vec(11), isr11);
    m1 = inportb(0x21);
    m2 = inportb(0xa1);
    outportb(0x21, m1 & ~((1 << 5) | (1 << 7) | (1 << 2)));
    outportb(0xa1, m2 & ~((1 << 1) | (1 << 2) | (1 << 3)));
    inportb(0x08);
    cw(0x48, 0x00);           /* MCE + I8: 8 kHz, 8-bit unsigned mono */
    cw(0x49, 0x04);           /* MCE + I9: SDC, playback off */
    cw(0x09, 0x04);           /* leave MCE */
    delay(50);
    wait_init();
    cw(0x06, 0x00);           /* DAC L/R unmuted */
    cw(0x07, 0x00);
    cw(0x0a, 0x02);           /* IEN */
    cw(0x0f, (LEN - 1) & 0xff);
    cw(0x0e, (LEN - 1) >> 8);
    outportb(cdc + 2, 0);
    for (i = 0; i < 3; i++)
        dma_prog(chans[i]);
    cw(0x09, 0x05);           /* PEN + SDC */
    t0 = biostime(0, 0L);
    while (biostime(0, 0L) - t0 < 18) {
        int n = 0;
        for (i = 0; i < 5; i++)
            n += hit[irqs[i]];
        if (n)
            break;
    }
    st = inportb(0x08);
    cw(0x09, 0x04);
    printf("irq:");
    for (i = 0; i < 5; i++) {
        printf(" %d=%d", irqs[i], hit[irqs[i]]);
        if (hit[irqs[i]])
            irq = irqs[i];
    }
    printf("\ndma:");
    for (i = 0; i < 3; i++) {
        int c = chans[i];
        unsigned left = dma_count(c);
        printf(" %d=%s/%04X", c, (st >> c) & 1 ? "TC" : "--", left);
        if ((st >> c) & 1)
            dma = c;
    }
    printf("\nRESULT: WSS IRQ %d DMA %d (-1 = none)\n", irq, dma);
    for (i = 0; i < 3; i++)
        outportb(0x0a, 4 | chans[i]);
    outportb(0x21, m1);
    outportb(0xa1, m2);
    setvect(vec(5), old[5]);
    setvect(vec(7), old[7]);
    setvect(vec(9), old[9]);
    setvect(vec(10), old[10]);
    setvect(vec(11), old[11]);
    return 0;
}
