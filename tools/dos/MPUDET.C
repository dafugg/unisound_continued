/* MPUDET BASE [BASE...]: MPU-401 detection at each base (hex): wait for the status port
   (BASE+1) to accept a command, send reset (FFh), and wait for the ACK (FEh) at BASE. Then put
   it in UART mode (3Fh, ACK again) and back with another reset. Found if the reset gets an ACK.
   Build: tcc MPUDET.C */
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>

/* Wait for status bit 6 (output ready, active low) to clear; 1 if it did. */
int wait_out(unsigned base)
{
    long i;
    for (i = 0; i < 30000L; i++) {
        if ((inportb(base + 1) & 0x40) == 0) {
            return 1;
        }
    }
    return 0;
}

/* Wait for an ACK (FEh) at base, reading bytes while bit 7 (input ready, active low) is clear.
   Returns the last byte read, or -1 if nothing came. */
int wait_ack(unsigned base)
{
    long i;
    int last = -1;
    for (i = 0; i < 30000L; i++) {
        if ((inportb(base + 1) & 0x80) == 0) {
            last = inportb(base);
            if (last == 0xfe) {
                return last;
            }
        }
    }
    return last;
}

int main(int argc, char *argv[])
{
    int a, r1, r2, r3;
    unsigned base;
    if (argc < 2) {
        puts("MPUDET BASE [BASE...]");
        return 2;
    }
    for (a = 1; a < argc; a++) {
        base = (unsigned)strtoul(argv[a], 0, 16);
        r1 = r2 = r3 = -1;
        if (wait_out(base)) {
            outportb(base + 1, 0xff);
            r1 = wait_ack(base);
        }
        if (r1 == 0xfe && wait_out(base)) {
            outportb(base + 1, 0x3f);
            r2 = wait_ack(base);
            if (wait_out(base)) {
                outportb(base + 1, 0xff);
                r3 = wait_ack(base);
            }
        }
        printf("MPU %X: status %02X reset %d uart %d reset %d %s\n", base, inportb(base + 1), r1,
               r2, r3, r1 == 0xfe ? "FOUND" : "none");
    }
    return 0;
}
