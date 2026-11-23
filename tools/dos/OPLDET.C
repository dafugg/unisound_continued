/* OPLDET BASE [BASE...]: AdLib timer detection at each base (hex): reset timers, read status,
   start timer 1 (FFh), wait ~1 ms, read status. Found if s1=00 and s2=C0.
   OPLDET T BASE: start a held 440 Hz OPL2 note at BASE.   OPLDET 0 BASE: key it off.
   BASE must be all hex digits, 100-FFFE: anything else (FOO parses as F, T or 0 as 0) is a
   usage error, exit 1, before any port is touched, so no OPL sequence reaches the DMA or PIC.
   Build: tcc OPLDET.C */
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>

unsigned base;

/* Set base from s if it is a whole hex number in 100h-FFFEh (above the motherboard ports, with
   room for base + 1); otherwise print the usage and return 0. */
int get_base(char *s)
{
    char *end;
    unsigned long v = strtoul(s, &end, 16);
    if (end == s || *end || v < 0x100 || v > 0xfffe) {
        printf("OPLDET: bad base '%s' (hex 100-FFFE)\n", s);
        puts("usage: OPLDET BASE [BASE...] | OPLDET T BASE | OPLDET 0 BASE");
        return 0;
    }
    base = (unsigned)v;
    return 1;
}

void w(unsigned char reg, unsigned char val)
{
    int i;
    outportb(base, reg);
    for (i = 0; i < 6; i++) {
        inportb(base);
    }
    outportb(base + 1, val);
    for (i = 0; i < 35; i++) {
        inportb(base);
    }
}

int main(int argc, char *argv[])
{
    int a, i;
    unsigned char s1, s2;
    if (argc > 2 && (argv[1][0] == 'T' || argv[1][0] == 't')) {
        if (!get_base(argv[2])) {
            return 1;
        }
        w(0x20, 0x01);
        w(0x40, 0x10);
        w(0x60, 0xf0);
        w(0x80, 0x77);
        w(0x23, 0x01);
        w(0x43, 0x00);
        w(0x63, 0xf0);
        w(0x83, 0x77);
        w(0xc0, 0x01);
        w(0xa0, 0x41);
        w(0xb0, 0x32);
        printf("tone on at %X\n", base);
        return 0;
    }
    if (argc > 2 && argv[1][0] == '0') {
        if (!get_base(argv[2])) {
            return 1;
        }
        w(0xb0, 0x11);
        printf("tone off at %X\n", base);
        return 0;
    }
    for (a = 1; a < argc; a++) { /* check every base before probing any */
        if (!get_base(argv[a])) {
            return 1;
        }
    }
    for (a = 1; a < argc; a++) {
        get_base(argv[a]);
        w(4, 0x60);
        w(4, 0x80);
        s1 = inportb(base) & 0xe0;
        w(2, 0xff);
        w(4, 0x21);
        for (i = 0; i < 2000; i++) {
            inportb(base);
        }
        delay(2);
        s2 = inportb(base) & 0xe0;
        w(4, 0x60);
        w(4, 0x80);
        printf("OPL %X: s1=%02X s2=%02X %s\n", base, s1, s2,
               (s1 == 0 && s2 == 0xc0) ? "FOUND" : "none");
    }
    return 0;
}
