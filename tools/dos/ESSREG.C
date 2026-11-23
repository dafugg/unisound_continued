/* ESSREG BASE [C6]: read ESS extended registers B1h/B2h with DSP command C0h (no DSP reset, so
   the state UNISOUND left is kept) and the SB Pro / ESS mixer registers. With "C6" it first
   sends command C6h (enable extended mode). Numbers are hex. A DSP that does not answer C0h
   (not an ESS) reads as FF and the mixer is still dumped; with no DSP at BASE at all it prints
   "no DSP" and exits 1.
   Build: tcc ESSREG.C */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>

unsigned base;

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

/* Returns the register, -1 if the DSP did not answer C0h (a non-ESS DSP; printed as FF as
   before) or -2 if the read-status bit never cleared: nothing is at BASE (open bus reads FFh),
   and an unbounded drain loop would hang with no DOS call for Ctrl-C to break it. */
int ext_read(unsigned char reg)
{
    long i;
    for (i = 0; i < 100000L && (inportb(base + 0xe) & 0x80); i++) {
        inportb(base + 0xa);
    }
    if (i >= 100000L) {
        return -2;
    }
    dsp_write(0xc0);
    dsp_write(reg);
    return dsp_read();
}

unsigned char mix(unsigned char reg)
{
    outportb(base + 4, reg);
    return inportb(base + 5);
}

int main(int argc, char *argv[])
{
    static unsigned char mr[] = {0x22, 0x04, 0x26, 0x2e, 0x28, 0x0a, 0x0c, 0x0e,
                                 0x14, 0x32, 0x36, 0x38, 0x3a, 0x3c, 0x3e, 0x40};
    int i, b1, b2;
    if (argc < 2) {
        puts("usage: ESSREG BASE [C6]");
        return 1;
    }
    base = (unsigned)strtoul(argv[1], 0, 16);
    if (argc > 2 && !stricmp(argv[2], "C6")) {
        dsp_write(0xc6);
    }
    b1 = ext_read(0xb1);
    b2 = ext_read(0xb2);
    if (b1 == -2 || b2 == -2) {
        printf("no DSP at %X (read status stays set)\n", base);
        return 1;
    }
    printf("ext B1=%02X B2=%02X\n", b1 & 0xff, b2 & 0xff);
    printf("mixer");
    for (i = 0; i < sizeof mr; i++) {
        printf(" %02X=%02X", mr[i], mix(mr[i]));
    }
    puts("");
    return 0;
}
