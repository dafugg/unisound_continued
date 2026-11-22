/* PIO - port I/O from the DOS command line, for testing.
   Arguments are processed left to right (numbers in hex):
     o PORT VAL    write byte VAL to PORT
     O PORT VAL    write word VAL to PORT
     i PORT        read a byte from PORT and print it
     w PORT        read a word from PORT and print it
     d MS          wait MS milliseconds (decimal)
     r BASE        reset the SB DSP at BASE and print whether it answered 0xAA
   Build: tcc -mt -lt PIO.C */
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>

int usage(void)
{
    puts("usage: PIO o PORT VAL | O PORT VAL | i PORT | w PORT | d MS | r BASE");
    return 1;
}

int main(int argc, char *argv[])
{
    int i, k;
    unsigned port, val;
    for (i = 1; i < argc; i++) {
        switch (argv[i][0]) {
            case 'o':
                if (i + 2 >= argc) {
                    return usage();
                }
                port = (unsigned)strtoul(argv[++i], 0, 16);
                val = (unsigned)strtoul(argv[++i], 0, 16);
                outportb(port, val);
                printf("o %03X %02X\n", port, val & 0xff);
                break;
            case 'O':
                if (i + 2 >= argc) {
                    return usage();
                }
                port = (unsigned)strtoul(argv[++i], 0, 16);
                val = (unsigned)strtoul(argv[++i], 0, 16);
                outport(port, val);
                printf("O %03X %04X\n", port, val & 0xffff);
                break;
            case 'w':
                if (i + 1 >= argc) {
                    return usage();
                }
                port = (unsigned)strtoul(argv[++i], 0, 16);
                printf("w %03X = %04X\n", port, inport(port));
                break;
            case 'i':
                if (i + 1 >= argc) {
                    return usage();
                }
                port = (unsigned)strtoul(argv[++i], 0, 16);
                printf("i %03X = %02X\n", port, inportb(port));
                break;
            case 'd':
                if (i + 1 >= argc) {
                    return usage();
                }
                delay(atoi(argv[++i]));
                break;
            case 'r':
                if (i + 1 >= argc) {
                    return usage();
                }
                port = (unsigned)strtoul(argv[++i], 0, 16);
                outportb(port + 6, 1);
                delay(1);
                outportb(port + 6, 0);
                for (k = 0; k < 100 && !(inportb(port + 0xe) & 0x80); k++) {
                    delay(1);
                }
                printf("r %03X = %02X\n", port, inportb(port + 0xa));
                break;
            default:
                printf("PIO: unknown opcode '%c'\n", argv[i][0]);
                return usage();
        }
    }
    return 0;
}
