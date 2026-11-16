/* GUSDET BASE [BASE...]: Gravis UltraSound GF1 detection at each base (hex, 2x0): reset the
   GF1 (register 4Ch at BASE+103h/BASE+105h) and take it out of reset, then write two bytes to
   DRAM addresses 0 and 1 (registers 43h/44h, data at BASE+107h) and read them back, as the GUS
   SDK's probe does. Found if both bytes read back. With ULTRASND set the same probe works for
   the GUS PnP's GF1-compatible (InterWave) mode.
   Build: tcc GUSDET.C */
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>

unsigned base;

void gf1_w8(unsigned char reg, unsigned char val)
{
    outportb(base + 0x103, reg);
    outportb(base + 0x105, val);
}

void dram_poke(unsigned long addr, unsigned char val)
{
    outportb(base + 0x103, 0x43);
    outport(base + 0x104, (unsigned)(addr & 0xffff));
    outportb(base + 0x103, 0x44);
    outportb(base + 0x105, (unsigned char)(addr >> 16));
    outportb(base + 0x107, val);
}

unsigned char dram_peek(unsigned long addr)
{
    outportb(base + 0x103, 0x43);
    outport(base + 0x104, (unsigned)(addr & 0xffff));
    outportb(base + 0x103, 0x44);
    outportb(base + 0x105, (unsigned char)(addr >> 16));
    return inportb(base + 0x107);
}

int main(int argc, char *argv[])
{
    int a;
    unsigned char b0, b1;
    if (argc < 2) {
        puts("GUSDET BASE [BASE...]");
        return 2;
    }
    for (a = 1; a < argc; a++) {
        base = (unsigned)strtoul(argv[a], 0, 16);
        gf1_w8(0x4c, 0);
        delay(10);
        gf1_w8(0x4c, 1);
        delay(10);
        dram_poke(0, 0xaa);
        dram_poke(1, 0x55);
        b0 = dram_peek(0);
        b1 = dram_peek(1);
        printf("GUS %X: DRAM 0 %02X 1 %02X %s\n", base, b0, b1,
               (b0 == 0xaa && b1 == 0x55) ? "FOUND" : "none");
    }
    return 0;
}
