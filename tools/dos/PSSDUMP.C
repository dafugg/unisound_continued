/* PSSDUMP [BASE]: look for an Echo PSS (ESC614) card at BASE, or at 220, 230, 240 and 250, and
   print its ID and five configuration registers, decoded as Linux's pss.c writes them: base
   (bits 15-6, the address with bits 1-0 clear), IRQ (bits 5-3) and DMA (bits 2-0). It only reads:
   the ID at +4 and the 16-bit registers at +10h (PSS), +12h (WSS), +14h (SB), +16h (CD-ROM) and
   +18h (MIDI); the DSP data port at +0 is left alone. Run it after ADI_PSS.SYS to see what the
   driver set. Build: tcc PSSDUMP.C */
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>

static int irqs[8] = {0, 3, 5, 7, 9, 10, 11, 12};
static int dmas[8] = {-1, 0, 1, 3, -1, 5, 6, 7};
static char *names[5] = {"PSS", "WSS", "SB", "CD", "MIDI"};

void conf(unsigned base, int i)
{
    unsigned v = inport(base + 0x10 + 2 * i);
    printf("  %-4s +%02X = %04X: base %03X", names[i], 0x10 + 2 * i, v, (v >> 4) & 0xffc);
    printf(" irq %d dma %d", irqs[(v >> 3) & 7], dmas[v & 7]);
    if (i == 0) {
        printf(" joystick %s", v & 0x400 ? "on" : "off");
    }
    puts("");
}

int dump(unsigned base)
{
    unsigned id = inport(base + 4);
    int i;
    printf("%X: id %04X status %04X", base, id, inport(base + 2));
    if ((id >> 8) != 'E') {
        puts(" (no PSS)");
        return 0;
    }
    printf(" (PSS rev %d)\n", id & 0xff);
    for (i = 0; i < 5; i++) {
        conf(base, i);
    }
    return 1;
}

int main(int argc, char *argv[])
{
    static unsigned bases[4] = {0x220, 0x230, 0x240, 0x250};
    int i, n = 0;
    if (argc >= 2) {
        n = dump((unsigned)strtoul(argv[1], 0, 16));
    } else {
        for (i = 0; i < 4; i++) {
            n += dump(bases[i]);
        }
    }
    puts(n ? "" : "no PSS card found");
    return n ? 0 : 1;
}
