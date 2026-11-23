/* ATAPIID BASE IRQ [SLAVE]: check that an ATAPI CD-ROM drive answers on the IDE channel at BASE
   (170, 1E8 or 168; control port BASE+206h) and interrupts on IRQ, as after UNISOUND sets up a
   sound card's IDE port with BLASTER or UNISOUND_CD C and Y. Resets the channel, prints the
   signature (14 EB for ATAPI), sends IDENTIFY PACKET DEVICE (A1h) to the master, or the slave
   with a third argument, and prints the model string and how many interrupts arrived.
   BASE must be nonzero; IRQ must be 2-7 or 9-15 (IRQ 0 is the timer, 1 the keyboard, 8 the
   RTC, and anything outside 0-15 is not an IRQ at all). IRQ 2 is taken as IRQ 9: on an AT the
   bus's IRQ 2 line is wired to IRQ 9 of the second PIC, IRQ 2 being its cascade. The third
   argument, if given, must be SLAVE. Timeouts print as timeout, not status FF.
   Build: tcc ATAPIID.C */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>

unsigned base, ctl;
int irq;
volatile int hits;
void interrupt (*old)(void);

void interrupt isr(void)
{
    hits++;
    inportb(base + 7); /* reading status clears the drive's interrupt */
    if (irq >= 8) {
        outportb(0xa0, 0x20);
    }
    outportb(0x20, 0x20);
}

int vec(int n)
{
    return n < 8 ? 8 + n : 0x70 + n - 8;
}

/* Wait up to about 2 s for BSY to clear; return the status, or -1 on timeout. */
int wait_ready(void)
{
    int k;
    unsigned char st;
    for (k = 0; k < 2000; k++) {
        st = inportb(base + 7);
        if (!(st & 0x80)) {
            return st;
        }
        delay(1);
    }
    return -1;
}

int main(int argc, char *argv[])
{
    unsigned id[256];
    unsigned char m1, m2;
    int i, st, dev = 0xa0;
    if (argc < 3) {
        puts("usage: ATAPIID BASE IRQ [SLAVE]");
        return 1;
    }
    base = (unsigned)strtoul(argv[1], 0, 16);
    ctl = base + 0x206;
    irq = atoi(argv[2]);
    if (irq == 2) {
        irq = 9; /* the AT bus's IRQ 2 line is IRQ 9 */
    }
    if (base == 0 || irq < 2 || irq > 15 || irq == 8) {
        puts("usage: ATAPIID BASE IRQ [SLAVE]");
        puts("BASE must be nonzero; IRQ must be 2-7 or 9-15 (not 0, 1 or 8; 2 means 9).");
        return 1;
    }
    if (argc >= 4) {
        if (stricmp(argv[3], "SLAVE") == 0) {
            dev = 0xb0;
        } else {
            puts("usage: ATAPIID BASE IRQ [SLAVE]");
            return 1;
        }
    }
    printf("IDE %X ctl %X irq %d %s: status %02X\n", base, ctl, irq,
           dev == 0xa0 ? "master" : "slave", inportb(base + 7));
    outportb(ctl, 0x06); /* SRST, nIEN */
    delay(5);
    outportb(ctl, 0x02);
    delay(50);
    outportb(base + 6, dev);
    delay(5);
    st = wait_ready();
    if (st < 0) {
        puts("after reset: timeout");
    } else {
        printf("after reset: status %02X signature %02X %02X\n", st & 0xff, inportb(base + 4),
               inportb(base + 5));
    }
    old = getvect(vec(irq));
    setvect(vec(irq), isr);
    m1 = inportb(0x21);
    m2 = inportb(0xa1);
    if (irq >= 8) {
        outportb(0x21, m1 & ~4);
        outportb(0xa1, m2 & ~(1 << (irq - 8)));
    } else {
        outportb(0x21, m1 & ~(1 << irq));
    }
    outportb(ctl, 0x00); /* nIEN off: the drive may interrupt */
    delay(10);
    hits = 0;
    outportb(base + 7, 0xa1);
    delay(100);
    st = wait_ready();
    if (st >= 0 && (st & 0x08)) {
        for (i = 0; i < 256; i++) {
            id[i] = inport(base);
        }
        printf("IDENTIFY PACKET: status %02X, model \"", st);
        for (i = 27; i < 47; i++) {
            putchar(id[i] >> 8);
            putchar(id[i] & 0xff);
        }
        printf("\"\n");
    } else if (st < 0) {
        puts("IDENTIFY PACKET: no data (timeout)");
    } else {
        printf("IDENTIFY PACKET: no data (status %02X)\n", st & 0xff);
    }
    delay(10);
    printf("RESULT: irq %d hits %d\n", irq, hits);
    outportb(ctl, 0x02);
    outportb(0x21, m1);
    outportb(0xa1, m2);
    setvect(vec(irq), old);
    return 0;
}
