/* CSLAM [SB IRQ DMA [DMA2]]: configure a Crystal CS423x through the Crystal key and SLAM
   ("Sound Logical device Activation Mode"), the non-PnP configuration path that CWDINIT and some
   laptop/OEM BIOSes use when the chip's PnP key is disabled (PKD).
   SB is hex (default 220), IRQ/DMA decimal (default 5, 1); DMA2 = WSS capture DMA (default 4 =
   none). Fixed: WSS codec 534, OPL 388, game port 200, control 120, MPU 330 IRQ 9.
   Sequence written to 279h: the 32-byte Crystal key, then for each logical device
   15h LDN, 47h/48h/42h I/O base 0/1/2 (high, low), 22h IRQ, 2Ah/25h DMA 0/1, 33h activate,
   and finally 79h (activate audio device, leave SLAM). CSN 06h is set to 1 first.
   Build: tcc CSLAM.C */
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>

#define ADDR 0x279

static unsigned char key[32] = {0x96, 0x35, 0x9A, 0xCD, 0xE6, 0xF3, 0x79, 0xBC,
                                0x5E, 0xAF, 0x57, 0x2B, 0x15, 0x8A, 0xC5, 0xE2,
                                0xF1, 0xF8, 0x7C, 0x3E, 0x9F, 0x4F, 0x27, 0x13,
                                0x09, 0x84, 0x42, 0xA1, 0xD0, 0x68, 0x34, 0x1A};

void w(unsigned char v)
{
    outportb(ADDR, v);
    delay(1);
}

void io(unsigned char reg, unsigned base)
{
    w(reg);
    w((unsigned char)(base >> 8));
    w((unsigned char)base);
}

int main(int argc, char *argv[])
{
    unsigned sb = 0x220;
    int irq = 5, dma = 1, dma2 = 4, i;
    if (argc > 1)
        sb = (unsigned)strtoul(argv[1], 0, 16);
    if (argc > 2)
        irq = atoi(argv[2]);
    if (argc > 3)
        dma = atoi(argv[3]);
    if (argc > 4)
        dma2 = atoi(argv[4]);
    for (i = 0; i < 32; i++)
        w(key[i]);
    w(0x06); w(0x01);                   /* CSN 1 */
    w(0x15); w(0x00);                   /* LDN 0: WSS / OPL / SB */
    io(0x47, 0x534);
    io(0x48, 0x388);
    io(0x42, sb);
    w(0x22); w(irq);
    w(0x2a); w(dma);
    w(0x25); w(dma2);
    w(0x33); w(0x01);
    w(0x15); w(0x01);                   /* LDN 1: game port */
    io(0x47, 0x200);
    w(0x33); w(0x01);
    w(0x15); w(0x02);                   /* LDN 2: control */
    io(0x47, 0x120);
    w(0x33); w(0x01);
    w(0x15); w(0x03);                   /* LDN 3: MPU-401 */
    io(0x47, 0x330);
    w(0x22); w(9);
    w(0x33); w(0x01);
    w(0x79);                            /* activate, leave SLAM */
    printf("SLAM: WSS 534 OPL 388 SB %X IRQ %d DMA %d/%d JOY 200 CTRL 120 MPU 330/I9\n", sb, irq, dma, dma2);
    return 0;
}
