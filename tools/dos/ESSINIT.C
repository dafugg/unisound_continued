/* ESSINIT [/OLD]: set up a non-PnP ESS ES688/ES1688-family chip (ES688, ES1688, ES1788,
   ES1887/ES1888) for the IRQ and DMA in BLASTER (A, I, D) and check that it took them,
   printing each step. It does what ESS's ESSCFG does:
   1. If the DSP at A doesn't answer a reset, send the ES1688 enable reads (229h three times,
      22Bh, 229h, 22Bh, 229h twice, 22Bh, 229h, then A itself, which picks the base) with
      interrupts off, and reset again.
   2. Reset (3 then 0 to 2x6h), wait for AAh, flush, identify (E7h answers 68h and 8xh), and
      turn on extended mode (C6h).
   3. Read B1h (IRQ) and B2h (DMA) with C0h, and write (old & F3h) | 50h | select to each: the
      register number and the value both to 2xCh, waiting for 2xCh bit 7 to clear before each
      byte. Select bits 3-2: IRQ 2/9, 5, 7, 10 = 0, 4, 8, C; DMA 0, 1, 3 = 4, 8, C.
   4. Read B1h and B2h back, then trigger an interrupt with F2h (up to three times) with
      handlers on IRQ 3, 5, 7, 9 and 10, and report which one fired.
   /OLD writes B1h/B2h as UNISOUND 0.82a does instead: the register number to 2xCh and the
   value (12h/16h + select, no bit 6) to 2xDh, with no busy wait, for comparison.
   An ES1887/ES1888 whose BIOS routes the interrupt to a pin in mixer 7Fh ignores B1h's IRQ.
   Exit code 0 when the chip took the IRQ and DMA and the interrupt came on the BLASTER IRQ,
   1 when something didn't work, 2 when no ESS chip was found.
   Build: tcc -mt -lt -f- ESSINIT.C (gives ESSINIT.COM) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dos.h>

unsigned base = 0x220;
int irq = 5, dma = 1;
volatile int hit[16];
unsigned char mask1, mask2;
void interrupt (*old[16])(void);
static int irqs[5] = {3, 5, 7, 9, 10};

int vec(int n) { return n < 8 ? 8 + n : 0x70 + n - 8; }

/* Count the interrupt and acknowledge it at the DSP (read 2xEh). If the IRQ was already
   unmasked, another driver owns it: pass it on to that handler, which ends it at the PIC. */
void ack(int n)
{
    hit[n]++;
    inportb(base + 0xe);
    if (!((n < 8 ? mask1 >> n : mask2 >> (n - 8)) & 1)) {
        (*old[n])();
        return;
    }
    if (n >= 8)
        outportb(0xa0, 0x20);
    outportb(0x20, 0x20);
}
void interrupt isr3(void) { ack(3); }
void interrupt isr5(void) { ack(5); }
void interrupt isr7(void) { ack(7); }
void interrupt isr9(void) { ack(9); }
void interrupt isr10(void) { ack(10); }

/* Point IRQ n's vector at the handler at offset off in this segment. Taking a handler's far
   address directly needs a segment fixup, which a .COM file can't have. */
void hook(int n, unsigned off)
{
    setvect(vec(n), (void interrupt (*)(void))MK_FP(_CS, off));
}

/* Wait up to 100 ms for 2xCh bit 7 (write buffer busy) to clear, then write v to port.
   Returns the number of ms waited, or -1 on timeout (the byte is written anyway). */
int dsp_out(unsigned port, unsigned char v)
{
    int i;
    for (i = 0; i < 100 && (inportb(base + 0xc) & 0x80); i++)
        delay(1);
    outportb(port, v);
    return i < 100 ? i : -1;
}

/* Read a DSP byte once 2xEh bit 7 says one is there, waiting up to 100 ms; -1 on timeout. */
int dsp_in(void)
{
    int i;
    for (i = 0; i < 100 && !(inportb(base + 0xe) & 0x80); i++)
        delay(1);
    return i < 100 ? inportb(base + 0xa) : -1;
}

/* Reset the DSP and wait up to 100 ms for AAh. Prints the result; returns 1 if AAh came. */
int dsp_reset(void)
{
    int i, b, n = 0;
    outportb(base + 6, 3);
    delay(1);
    outportb(base + 6, 0);
    for (i = 0; i < 100 && !(inportb(base + 0xe) & 0x80); i++)
        delay(1);
    b = inportb(base + 0xa);
    printf("Reset: 2xAh = %02X after %d ms", b, i);
    while (b == 0xaa && n < 16 && (inportb(base + 0xe) & 0x80)) {
        inportb(base + 0xa);
        n++;
    }
    printf(", %d more byte(s) flushed\n", n);
    return b == 0xaa;
}

int ext_read(unsigned char reg)
{
    dsp_out(base + 0xc, 0xc0);
    dsp_out(base + 0xc, reg);
    return dsp_in();
}

/* Write extended register reg. New way: both bytes to 2xCh after a busy wait. /OLD: the
   value to 2xDh without waiting, with UNISOUND 0.82a's delays. */
void ext_write(unsigned char reg, unsigned char v, int oldway)
{
    int w1, w2;
    if (oldway) {
        outportb(base + 0xc, reg);
        delay(17);
        outportb(base + 0xd, v);
        delay(16);
        printf("Write %02Xh = %02X: %02X to %Xh, %02X to %Xh (old way)\n", reg, v, reg,
               base + 0xc, v, base + 0xd);
        return;
    }
    w1 = dsp_out(base + 0xc, reg);
    w2 = dsp_out(base + 0xc, v);
    printf("Write %02Xh = %02X: both bytes to %Xh, busy wait %d and %d ms\n", reg, v,
           base + 0xc, w1, w2);
}

/* Parse BLASTER's A, I and D fields. Returns 1 if BLASTER is set. */
int parse_blaster(void)
{
    char *e = getenv("BLASTER");
    if (e == 0)
        return 0;
    printf("BLASTER=%s\n", e);
    while (*e) {
        char c = toupper(*e);
        if (c == 'A')
            base = (unsigned)strtoul(e + 1, 0, 16);
        else if (c == 'I')
            irq = atoi(e + 1);
        else if (c == 'D')
            dma = atoi(e + 1);
        while (*e && *e != ' ')
            e++;
        while (*e == ' ')
            e++;
    }
    return 1;
}

/* Fire F2h up to three times with handlers on IRQ 3, 5, 7, 9 and 10; returns the IRQ that
   fired, 0 if none did. */
int irq_test(void)
{
    int i, t, k, got = 0;
    mask1 = inportb(0x21);
    mask2 = inportb(0xa1);
    for (i = 0; i < 5; i++)
        old[irqs[i]] = getvect(vec(irqs[i]));
    hook(3, FP_OFF(isr3));
    hook(5, FP_OFF(isr5));
    hook(7, FP_OFF(isr7));
    hook(9, FP_OFF(isr9));
    hook(10, FP_OFF(isr10));
    disable();
    outportb(0x21, mask1 & ~0xac);          /* IRQ 2 (cascade), 3, 5, 7 */
    outportb(0xa1, mask2 & ~0x06);          /* IRQ 9, 10 */
    enable();
    for (t = 1; t <= 3 && !got; t++) {
        dsp_out(base + 0xc, 0xf2);
        for (k = 0; k < 50 && !got; k++) {
            delay(1);
            for (i = 0; i < 5; i++)
                if (hit[irqs[i]])
                    got = irqs[i];
        }
        printf("F2h try %d:", t);
        for (i = 0; i < 5; i++)
            printf(" IRQ%d=%d", irqs[i], hit[irqs[i]]);
        puts("");
    }
    disable();
    outportb(0x21, mask1);
    outportb(0xa1, mask2);
    for (i = 0; i < 5; i++)
        setvect(vec(irqs[i]), old[irqs[i]]);
    enable();
    return got;
}

int main(int argc, char *argv[])
{
    int i, e7, ver, isel, dsel, b1, b2, n1, n2, got, ok = 1, oldway = 0;
    static unsigned char key[10] = {9, 9, 9, 0xb, 9, 0xb, 9, 9, 0xb, 9};
    for (i = 1; i < argc; i++) {
        if (stricmp(argv[i], "/OLD") == 0)
            oldway = 1;
        else {
            puts("ESSINIT [/OLD]: set up an ESS ES688/ES1688-family chip for BLASTER A, I, D.");
            puts("  /OLD  write B1h/B2h as UNISOUND 0.82a does (value to 2xDh, no bit 6)");
            puts("Exit code 0 = OK, 1 = a step failed, 2 = no ESS chip found.");
            return 1;
        }
    }
    if (!parse_blaster())
        puts("BLASTER not set");
    switch (irq) {
    case 2: case 9: isel = 0; irq = 9; break;
    case 5: isel = 4; break;
    case 7: isel = 8; break;
    case 10: isel = 0xc; break;
    default: printf("IRQ %d not possible on these chips, using 5\n", irq); irq = 5; isel = 4;
    }
    switch (dma) {
    case 0: dsel = 4; break;
    case 1: dsel = 8; break;
    case 3: dsel = 0xc; break;
    default: printf("DMA %d not possible on these chips, using 1\n", dma); dma = 1; dsel = 8;
    }
    printf("Base %Xh, IRQ %d, DMA %d%s\n", base, irq, dma, oldway ? ", old write" : "");
    printf("Before: 2xEh = %02X, 2xAh = %02X\n", inportb(base + 0xe), inportb(base + 0xa));
    if (!dsp_reset()) {
        disable();
        for (i = 0; i < 10; i++)
            inportb(0x220 + key[i]);
        inportb(base);
        enable();
        puts("No answer: enable reads sent (229h/22Bh, then the base)");
        if (!dsp_reset()) {
            puts("No DSP at this base.");
            return 2;
        }
    }
    dsp_out(base + 0xc, 0xe7);
    e7 = dsp_in();
    ver = dsp_in();
    printf("E7h: %02X %02X\n", e7 & 0xff, ver & 0xff);
    if (e7 != 0x68 || (ver & 0xf0) != 0x80) {
        puts("Not an ES688/ES1688-family chip.");
        return 2;
    }
    printf("Chip: %s\n", ver < 0x88 ? "ES688" : "ES1688 or later");
    dsp_out(base + 0xc, 0xc6);
    outportb(base + 4, 0x40);
    printf("Mixer 40h = %02X\n", inportb(base + 5));
    b1 = ext_read(0xb1);
    b2 = ext_read(0xb2);
    printf("Before: B1h = %02X, B2h = %02X\n", b1 & 0xff, b2 & 0xff);
    if (oldway) {
        ext_write(0xb1, 0x12 + isel, 1);
        ext_write(0xb2, 0x12 + dsel, 1);
    } else {
        ext_write(0xb1, (b1 & 0xf3) | 0x50 | isel, 0);
        ext_write(0xb2, (b2 & 0xf3) | 0x50 | dsel, 0);
    }
    n1 = ext_read(0xb1);
    n2 = ext_read(0xb2);
    printf("After:  B1h = %02X, B2h = %02X\n", n1 & 0xff, n2 & 0xff);
    if ((n1 & 0x0c) != isel || n1 < 0) {
        printf("B1h doesn't select IRQ %d\n", irq);
        ok = 0;
    }
    if ((n2 & 0x0c) != dsel || n2 < 0) {
        printf("B2h doesn't select DMA %d\n", dma);
        ok = 0;
    }
    got = irq_test();
    if (got == irq)
        printf("Interrupt on IRQ %d: OK\n", got);
    else {
        if (got)
            printf("Interrupt on IRQ %d, expected %d\n", got, irq);
        else
            puts("No interrupt");
        ok = 0;
    }
    puts(ok ? "RESULT: OK" : "RESULT: FAILED");
    return ok ? 0 : 1;
}
