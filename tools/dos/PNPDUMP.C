/* PNPDUMP CSN LDN [LDN...]: dump ISA PnP configuration registers of one card.
   Sends the initiation key, sets the read port to 20Bh, wakes the card with the given CSN, and
   for each logical device prints activate (30h), I/O bases (60h-67h), IRQs (70h-73h) and DMAs
   (74h-75h). Values are hex. Leaves the cards in Wait for Key. Numbers are hex.
   For testing in 86Box: setting the read port outside isolation isn't allowed by the spec.
   Build: tcc PNPDUMP.C */
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>

#define ADDR  0x279
#define WDATA 0xa79
#define RDATA 0x20b

void key(void)
{
    unsigned char k = 0x6a;
    int i;
    outportb(ADDR, 0);
    outportb(ADDR, 0);
    for (i = 0; i < 32; i++) {
        outportb(ADDR, k);
        k = (k >> 1) | (((k ^ (k >> 1)) & 1) << 7);
    }
}

void wr(unsigned char reg, unsigned char val)
{
    outportb(ADDR, reg);
    outportb(WDATA, val);
}

unsigned char rd(unsigned char reg)
{
    outportb(ADDR, reg);
    return inportb(RDATA);
}

int main(int argc, char *argv[])
{
    int i;
    if (argc < 3) {
        puts("usage: PNPDUMP CSN LDN [LDN...]");
        return 1;
    }
    key();
    wr(0x00, RDATA >> 2);
    wr(0x03, (unsigned char)strtoul(argv[1], 0, 16));
    for (i = 2; i < argc; i++) {
        unsigned char ldn = (unsigned char)strtoul(argv[i], 0, 16);
        wr(0x07, ldn);
        printf("LDN %X: act %02X io %02X%02X %02X%02X %02X%02X %02X%02X irq %02X/%02X %02X/%02X "
               "dma %02X %02X\n",
               ldn, rd(0x30), rd(0x60), rd(0x61), rd(0x62), rd(0x63), rd(0x64), rd(0x65), rd(0x66),
               rd(0x67), rd(0x70), rd(0x71), rd(0x72), rd(0x73), rd(0x74), rd(0x75));
    }
    wr(0x02, 0x02);
    return 0;
}
