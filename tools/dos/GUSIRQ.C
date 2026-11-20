/* GUSIRQ: check a GUS's IRQ and DMA latches after setup, from ULTRASND (port,dma,dma2,irq,irq2).
   1. DMA: 1 KB from PC memory to GF1 DRAM 1000h on DMA 1 (8-bit channels 1 and 3 only), with
      the DMA terminal count IRQ enabled (GF1 register 41h = 21h). Reports which IRQ fired
      (2/9, 3, 5, 7, 11, 12, 15 hooked), the 8237 TC bit, and whether the DRAM got the data.
   2. MIDI: reset the MIDI ACIA at +100h and enable its transmit IRQ (TX empty fires at once);
      reports which IRQ fired (the second ULTRASND IRQ, or the first if combined).
   Does not touch the mix control or latches.
   Build: tcc GUSIRQ.C */
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>

#define LEN 1024
unsigned base;
volatile int hit[16];
volatile unsigned char st6, st41;
unsigned char buf[2 * LEN + 16];
static int irqs[7] = {3, 5, 7, 9, 11, 12, 15};
static unsigned pagep[4] = {0x87, 0x83, 0x81, 0x82};
void interrupt (*old[16])(void);

void ack(int irq)
{
    hit[irq]++;
    st6 = inportb(base + 6);
    outportb(base + 0x103, 0x41);
    st41 = inportb(base + 0x105);
    outportb(base + 0x100, 3); /* MIDI reset: stops the TX IRQ */
    if (irq >= 8)
        outportb(0xa0, 0x20);
    outportb(0x20, 0x20);
}
void interrupt isr3(void) { ack(3); }
void interrupt isr5(void) { ack(5); }
void interrupt isr7(void) { ack(7); }
void interrupt isr9(void) { ack(9); }
void interrupt isr11(void) { ack(11); }
void interrupt isr12(void) { ack(12); }
void interrupt isr15(void) { ack(15); }
void interrupt (*isrs[7])(void) = {isr3, isr5, isr7, isr9, isr11, isr12, isr15};

int vec(int irq) { return irq < 8 ? 8 + irq : 0x70 + irq - 8; }

void w8(unsigned char r, unsigned char v)
{
    outportb(base + 0x103, r);
    outportb(base + 0x105, v);
}
unsigned char r8(unsigned char r)
{
    outportb(base + 0x103, r);
    return inportb(base + 0x105);
}
unsigned char dpeek(unsigned long a)
{
    outportb(base + 0x103, 0x43);
    outport(base + 0x104, (unsigned)a);
    w8(0x44, (unsigned char)(a >> 16));
    return inportb(base + 0x107);
}

void clear(void)
{
    int i;
    for (i = 0; i < 16; i++)
        hit[i] = 0;
}
int wait_hit(void)
{
    unsigned long t0 = biostime(0, 0L);
    int i;
    while (biostime(0, 0L) - t0 < 18) {
        for (i = 0; i < 7; i++)
            if (hit[irqs[i]])
                return irqs[i];
    }
    return -1;
}
void show(char *what)
{
    int i;
    printf("%s irq:", what);
    for (i = 0; i < 7; i++)
        printf(" %d=%d", irqs[i], hit[irqs[i]]);
    puts("");
}

int main(void)
{
    char *env = getenv("ULTRASND");
    int dma = -1, dma2 = -1, irq = -1, irq2 = -1, i, got, bad = 0, c;
    unsigned char m1, m2, st;
    unsigned char *p;
    unsigned long a;
    unsigned left;
    if (env == 0 || sscanf(env, "%x,%d,%d,%d,%d", &base, &dma, &dma2, &irq, &irq2) < 4) {
        puts("GUSIRQ: set ULTRASND=port,dma,dma2,irq,irq2");
        return 2;
    }
    printf("ULTRASND=%s\n", env);
    c = dma;
    a = ((unsigned long)FP_SEG(buf) << 4) + FP_OFF(buf);
    p = buf;
    if ((a & 0xffff) + LEN > 0x10000L) {
        p += LEN;
        a += LEN;
    }
    for (i = 0; i < LEN; i++)
        p[i] = (unsigned char)(i * 7 + 3);
    for (i = 0; i < 7; i++) {
        old[irqs[i]] = getvect(vec(irqs[i]));
        setvect(vec(irqs[i]), isrs[i]);
    }
    m1 = inportb(0x21);
    m2 = inportb(0xa1);
    outportb(0x21, m1 & ~((1 << 3) | (1 << 5) | (1 << 7) | (1 << 2)));
    outportb(0xa1, m2 & ~((1 << 1) | (1 << 3) | (1 << 4) | (1 << 7)));
    delay(10);
    for (i = 0; i < 7; i++)
        if (hit[irqs[i]])
            bad = 1;
    if (bad)
        show("stray");
    clear();
    /* 1: DMA to DRAM */
    if (c == 1 || c == 3) {
        w8(0x41, 0);
        r8(0x41);
        inportb(base + 6);
        inportb(0x08);
        outportb(0x0a, 4 | c);
        outportb(0x0c, 0);
        outportb(0x0b, 0x48 | c); /* single, memory to I/O */
        outportb(c * 2, (unsigned char)a);
        outportb(c * 2, (unsigned char)(a >> 8));
        outportb(pagep[c], (unsigned char)(a >> 16));
        outportb(0x0c, 0);
        outportb(c * 2 + 1, (LEN - 1) & 0xff);
        outportb(c * 2 + 1, (LEN - 1) >> 8);
        outportb(0x0a, c);
        outportb(base + 0x103, 0x42);
        outport(base + 0x104, 0x1000 >> 4);
        w8(0x41, 0x21); /* enable, write DRAM, 8-bit, TC IRQ on */
        got = wait_hit();
        st = inportb(0x08);
        outportb(0x0c, 0);
        left = inportb(c * 2 + 1);
        left |= inportb(c * 2 + 1) << 8;
        outportb(0x0a, 4 | c);
        w8(0x41, 0);
        show("dma");
        bad = 0;
        for (i = 0; i < LEN; i++)
            if (dpeek(0x1000L + i) != (unsigned char)(i * 7 + 3))
                bad++;
        printf("isr 2x6=%02X 41h=%02X; 8237 ch%d %s count %04X; DRAM %d/%d bytes wrong\n", st6,
               st41, c, (st >> c) & 1 ? "TC" : "--", left, bad, LEN);
        printf("RESULT: GF1 DMA %d IRQ %d DRAM %s\n", (st >> c) & 1 ? c : -1, got,
               bad == 0 ? "ok" : "BAD");
    } else {
        printf("DMA %d: only 8-bit channels 1 and 3 are tested\n", c);
    }
    clear();
    /* 2: MIDI TX IRQ */
    outportb(base + 0x100, 3);
    delay(1);
    outportb(base + 0x100, 0x20);
    got = wait_hit();
    outportb(base + 0x100, 3);
    show("midi");
    printf("RESULT: MIDI IRQ %d\n", got);
    outportb(0x21, m1);
    outportb(0xa1, m2);
    for (i = 0; i < 7; i++)
        setvect(vec(irqs[i]), old[irqs[i]]);
    return 0;
}
