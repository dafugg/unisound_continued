/* ESSBYP CTL SB [1|4] [S] [W]: prototype of ES1868/ES1869 detection and setup through the ESS bypass
   key only. Nothing is written to the PnP write-data port 0xA79 and the read-data port is never
   used, so it works where the LPC bridge does not forward 0xA79 or the chip is not on standard
   isolation.
   0. (S only) Wait for Key, then the PnP initiation key, which moves the chip from Wait for Key
      to Sleep. UNISOUND sends the initiation key before /XESSB, so with S the bypass key below
      reaches the chip in Sleep, as it does from UNISOUND; the ES1869 data sheet (p. 28, "Bypass
      Key") says the chip must be in Wait for Key, so S is the experiment for that difference.
   1. (W only) Wait for Key through 0x279/0xA79, as ESSCFG does before the key.
   2. Bypass key to 0x279: the 32-byte ESS key, then CTL low and high byte. "1" sends one round
      (key ^ 0), "4" (default) four rounds with key ^ 0..3 as UNISOUND's send_alt_pnp_key does.
   3. Through the configuration device at CTL (index CTL+0, data CTL+1): dump LDN 0 and 1 regs,
      then program LDN 1: SB base SB, OPL 388, MPU 330, IRQ 5 (70h and 72h), DMA 1 (74h),
      no second DMA (75h = 4), activate. LDN 1 is only programmed once the chip has answered
      as an ESS (step 4's check runs first, with just the SB base and activate written, as the
      driver's ess_bypass_id does); if it does not answer, that LDN 1 is deactivated again,
      "RESULT: no ES1868/ES1869 answered" is printed and the exit code is 1.
   4. Identify through the chip itself: DSP reset at SB, DSP command E7h (68h 8xh), then four
      reads of mixer register 40h (ES1868/ES1869: 18h, 68h/69h, CTL high, CTL low).
   Options other than 1, 4, S and W are a usage error (exit 1). Numbers are hex.
   Build: tcc ESSBYP.C */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>

static unsigned char initkey[32] = {
    0x6a, 0xb5, 0xda, 0xed, 0xf6, 0xfb, 0x7d, 0xbe, 0xdf, 0x6f, 0x37, 0x1b, 0x0d, 0x86, 0xc3, 0x61,
    0xb0, 0x58, 0x2c, 0x16, 0x8b, 0x45, 0xa2, 0xd1, 0xe8, 0x74, 0x3a, 0x9d, 0xce, 0xe7, 0x73, 0x39};
static unsigned char key[32] = {0x66, 0xa1, 0xc2, 0xf1, 0xea, 0xe7, 0x71, 0xaa, 0xc7, 0x63, 0x33,
                                0x1b, 0x0d, 0x96, 0xdb, 0x6d, 0xa4, 0x50, 0x28, 0x16, 0x9b, 0x4d,
                                0xb6, 0xc9, 0xf4, 0x78, 0x3e, 0x8d, 0xd6, 0xfb, 0x7f, 0x3d};
unsigned ctl, sb;

void cw(unsigned char r, unsigned char v)
{
    outportb(ctl, r);
    outportb(ctl + 1, v);
}
unsigned char cr(unsigned char r)
{
    outportb(ctl, r);
    return inportb(ctl + 1);
}

void dump(unsigned char ldn)
{
    cw(7, ldn);
    printf("LDN %X: act %02X io %02X%02X %02X%02X %02X%02X irq %02X/%02X %02X/%02X dma %02X %02X\n",
           ldn, cr(0x30), cr(0x60), cr(0x61), cr(0x62), cr(0x63), cr(0x64), cr(0x65), cr(0x70),
           cr(0x71), cr(0x72), cr(0x73), cr(0x74), cr(0x75));
}

int dsp_read(void)
{
    long i;
    for (i = 0; i < 30000L; i++) {
        if (inportb(sb + 0xe) & 0x80) {
            return inportb(sb + 0xa);
        }
    }
    return -1;
}

/* Returns 1 if the byte was written, 0 if the DSP stayed busy past the timeout. */
int dsp_write(unsigned char v)
{
    long i;
    for (i = 0; i < 30000L && (inportb(sb + 0xc) & 0x80); i++) {
    }
    if (i >= 30000L) {
        return 0;
    }
    outportb(sb + 0xc, v);
    return 1;
}

int main(int argc, char *argv[])
{
    int i, j, rounds = 4, waitkey = 0, sleep = 0, r, v1, v2;
    unsigned char m[4];
    if (argc < 3) {
        puts("usage: ESSBYP CTL SB [1|4] [S] [W]");
        return 1;
    }
    ctl = (unsigned)strtoul(argv[1], 0, 16);
    sb = (unsigned)strtoul(argv[2], 0, 16);
    for (i = 3; i < argc; i++) {
        if (argv[i][0] == '1') {
            rounds = 1;
        } else if (argv[i][0] == '4') {
            rounds = 4;
        } else if (argv[i][0] == 'W' || argv[i][0] == 'w') {
            waitkey = 1;
        } else if (argv[i][0] == 'S' || argv[i][0] == 's') {
            sleep = 1;
        } else {
            puts("usage: ESSBYP CTL SB [1|4] [S] [W]");
            return 1;
        }
    }
    printf("before: ctl+0..7:");
    for (i = 0; i < 8; i++) {
        printf(" %02X", inportb(ctl + i));
    }
    puts("");
    if (sleep) {
        outportb(0x279, 2);
        outportb(0xa79, 2);
        delay(2);
        outportb(0x279, 0);
        outportb(0x279, 0);
        for (i = 0; i < 32; i++) {
            outportb(0x279, initkey[i]);
        }
        delay(1);
        puts("initiation key sent: the chip is in Sleep");
    }
    if (waitkey) {
        outportb(0x279, 2);
        outportb(0xa79, 2);
        delay(2);
    }
    printf("bypass key sent from %s\n", waitkey ? "Wait for Key"
                                        : sleep ? "Sleep"
                                                : "the state after boot (Wait for Key)");
    for (j = 0; j < rounds; j++) {
        for (i = 0; i < 32; i++) {
            outportb(0x279, key[i] ^ j);
        }
        delay(1);
        outportb(0x279, ctl & 0xff);
        outportb(0x279, ctl >> 8);
    }
    delay(2);
    printf("after key (%d round%s): ctl+0..7:", rounds, rounds > 1 ? "s" : "");
    for (i = 0; i < 8; i++) {
        printf(" %02X", inportb(ctl + i));
    }
    puts("");
    printf("card regs: 06=%02X 07=%02X 20..26:", cr(6), cr(7));
    for (i = 0x20; i <= 0x26; i++) {
        printf(" %02X", cr(i));
    }
    puts("");
    dump(0);
    dump(1);
    /* SB base + activate only, as the driver's ess_bypass_id() does: the rest of LDN 1 is
       programmed only once the chip has answered as an ESS, so a non-ESS port is left alone. */
    cw(7, 1);
    cw(0x30, 0);
    cw(0x60, sb >> 8);
    cw(0x61, sb & 0xff);
    cw(0x30, 1);
    delay(2);
    outportb(sb + 6, 3);
    delay(1);
    outportb(sb + 6, 0);
    r = dsp_read();
    if (!dsp_write(0xe7)) {
        v1 = -1;
        v2 = -1;
    } else {
        v1 = dsp_read();
        v2 = dsp_read();
    }
    outportb(sb + 4, 0x40); /* index once, then consecutive data reads (as Linux es18xx) */
    for (i = 0; i < 4; i++) {
        m[i] = inportb(sb + 5);
    }
    printf("DSP %X: reset=%02X E7=%02X %02X  mixer40h: %02X %02X %02X %02X\n", sb, r & 0xff,
           v1 & 0xff, v2 & 0xff, m[0], m[1], m[2], m[3]);
    if (r != 0xaa || v1 != 0x68 || m[0] != 0x18 || (m[1] != 0x68 && m[1] != 0x69)) {
        puts("RESULT: no ES1868/ES1869 answered");
        cw(7, 1);
        cw(0x30, 0); /* deactivate the LDN 1 we just turned on */
        return 1;
    }
    cw(7, 1);
    cw(0x62, 0x03);
    cw(0x63, 0x88);
    cw(0x64, 0x03);
    cw(0x65, 0x30);
    cw(0x70, 5);
    cw(0x71, 2);
    cw(0x72, 5);
    cw(0x73, 2);
    cw(0x74, 1);
    cw(0x75, 4);
    cw(0x30, 1);
    delay(2);
    dump(1);
    if (((m[2] << 8) | m[3]) != ctl) {
        printf("RESULT: ES18%02X answers, but its control port is %X, not %X: the bypass key was\n"
               "not taken (a PnP BIOS or a driver set the chip up)\n",
               m[1], (m[2] << 8) | m[3], ctl);
    } else {
        printf("RESULT: ES18%02X found, control port reported %X (asked %X): the key was taken\n",
               m[1], (m[2] << 8) | m[3], ctl);
    }
    return 0;
}
