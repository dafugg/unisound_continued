/* UNIDUMP [options] > DUMP.TXT: one report of a machine's sound hardware and the system around it,
   for testers to send with a problem. By default it only reads, or writes what detection needs
   (DSP resets, the OPL timer test, codec index writes, the ISA PnP key and Wake), and changes no
   card's settings. Sections, in order (names for /SKIP and /ONLY):
     ENV      DOS version, boot drive, BLASTER, ULTRASND, ULTRA16, UNISOUND_CD
     CPU      processor class, CPUID vendor, family/model/stepping, name, features; FPU; V86 mode
     MEM      conventional and extended memory (INT 12h, INT 15h E801h/88h)
     BIOS     ROM date and model bytes, BIOS ID strings, SMBIOS/DMI: BIOS, system, board,
              processor and memory devices
     PCI      every PCI function (PCI BIOS): vendor, device, class, sound devices marked
     DOS      EMS, XMS, Windows, memory blocks with their owners (as MEM /C), device drivers,
              CONFIG.SYS and AUTOEXEC.BAT (not with /NOFILES)
     IRQ      PIC mask, request and in-service registers; IRQ vectors and who owns them;
              COM and LPT ports from the BIOS data area
     PNPBIOS  the PnP BIOS: header, every system device node with its current, possible and
              compatible resources decoded (sound devices marked), ISA PnP configuration, ESCD
     PNP      ISA PnP cards at the CSNs the BIOS assigned (no isolation, read port left alone):
              serial ID, resource data in hex and decoded, each logical device's settings
     SB OPL MPU WSS PSS   non-PnP probes: SB DSPs at 220h-280h (reset, version, ESS ID and
              B1h/B2h), OPL timers at 388h, MPU-401 status at 330h/300h, WSS codecs at 530h,
              604h, E80h and F40h, an Echo PSS ID at 220h-250h
   Options:
     /SKIP:A,B   leave sections out (e.g. /SKIP:PCI,WSS if a probe hangs the machine);
                 repeatable, lists combine
     /ONLY:A,B   run only these sections (repeatable, lists combine)
     /ESCD       also print the ESCD in hex (PnP BIOS function 42h, or read from memory)
     /ISOLATE    isolate ISA PnP cards that have no CSN, as UNISOUND does, and report them too.
                 It assigns CSNs; without a PnP BIOS (or with no CSNs) it resets them first
     /DEEP       also the OPTi 82C924/925, 82C928/929 and 82C930 password probes at F8Ch-F8Fh
                 and E0Eh, which write to those ports (a card decoding 10 address bits sees F8Fh
                 as 38Fh, next to the FM ports)
     /NOFILES    leave CONFIG.SYS and AUTOEXEC.BAT out of the DOS section, for anyone who'd
                 rather not share them; the rest of the section is still reported
   Before each step it prints "UNIDUMP: step" on the screen (stderr), and the report is flushed
   after each section, so if the machine hangs the screen shows where and DUMP.TXT holds the rest.
   The PnP BIOS calls and register layouts are from the Plug and Play BIOS Specification 1.0A and
   the Plug and Play ISA Specification 1.0a, SMBIOS from the DMTF SMBIOS specification.
   Build: tcc UNIDUMP.C */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dos.h>

#define ADDR    0x279
#define WDATA   0xa79
#define RES_MAX 2048

typedef int(far *PNPFN)(int, ...);

PNPFN pnp_entry;
unsigned pnp_ds;
unsigned rdport;
int pnp_bios_csns = -1; /* CSNs the PnP BIOS reported; -1 = no PnP BIOS or no function 40h */
int cpu = 0;            /* processor class from cpu_class: 0 8086, 2 286, 3 386, 4 486, 5 CPUID */
unsigned char res[RES_MAX];
unsigned char node_buf[1024];
char only[80], skip[80];
int opt_escd, opt_isolate, opt_deep, opt_nofiles;

/* 16-bit routines that need 386/486 instructions, which Turbo C can't emit without an assembler.
   Assembled with GNU as (.code16), all far cdecl:
   0000 cpu_class: pushf; FLAGS bits 12-15 stuck at 1: 8086; stuck at 0: 286; EFLAGS AC (bit 18)
        not settable: 386; ID (bit 21) not settable: 486 without CPUID; else CPUID. popf; retf.
   007E cpuid4(unsigned long leaf, unsigned long far *out): push bp/si/ds/bx; mov eax,[bp+6];
        xor ecx,ecx; cpuid; lds si,[bp+10]; store eax, ebx, ecx, edx; pop; retf.
   00A4 msw: smsw ax; retf.
   00A8 xms(void far *entry, unsigned ax): push bp/bx/si/di; mov ax,[bp+10]; call far [bp+6];
        pop; retf (DX:AX is the result). */
unsigned char blob[] = {
    0x9c, 0x9c, 0x58, 0x89, 0xc1, 0x25, 0xff, 0x0f, 0x50, 0x9d, 0x9c, 0x58, 0x25, 0x00, 0xf0, 0x3d,
    0x00, 0xf0, 0x74, 0x57, 0x89, 0xc8, 0x0d, 0x00, 0xf0, 0x50, 0x9d, 0x9c, 0x58, 0x25, 0x00, 0xf0,
    0x74, 0x4d, 0x66, 0x9c, 0x66, 0x58, 0x66, 0x89, 0xc1, 0x66, 0x35, 0x00, 0x00, 0x04, 0x00, 0x66,
    0x50, 0x66, 0x9d, 0x66, 0x9c, 0x66, 0x58, 0x66, 0x51, 0x66, 0x9d, 0x66, 0x31, 0xc8, 0x66, 0xa9,
    0x00, 0x00, 0x04, 0x00, 0x74, 0x2e, 0x66, 0x89, 0xc8, 0x66, 0x35, 0x00, 0x00, 0x20, 0x00, 0x66,
    0x50, 0x66, 0x9d, 0x66, 0x9c, 0x66, 0x58, 0x66, 0x51, 0x66, 0x9d, 0x66, 0x31, 0xc8, 0x66, 0xa9,
    0x00, 0x00, 0x20, 0x00, 0x74, 0x13, 0xb8, 0x05, 0x00, 0xeb, 0x11, 0x31, 0xc0, 0xeb, 0x0d, 0xb8,
    0x02, 0x00, 0xeb, 0x08, 0xb8, 0x03, 0x00, 0xeb, 0x03, 0xb8, 0x04, 0x00, 0x9d, 0xcb, 0x55, 0x89,
    0xe5, 0x56, 0x1e, 0x53, 0x66, 0x8b, 0x46, 0x06, 0x66, 0x31, 0xc9, 0x0f, 0xa2, 0xc5, 0x76, 0x0a,
    0x66, 0x89, 0x04, 0x66, 0x89, 0x5c, 0x04, 0x66, 0x89, 0x4c, 0x08, 0x66, 0x89, 0x54, 0x0c, 0x5b,
    0x1f, 0x5e, 0x5d, 0xcb, 0x0f, 0x01, 0xe0, 0xcb, 0x55, 0x89, 0xe5, 0x53, 0x56, 0x57, 0x8b, 0x46,
    0x0a, 0xff, 0x5e, 0x06, 0x5f, 0x5e, 0x5b, 0x5d, 0xcb};

int cpu_class(void)
{
    return ((int(far *)(void))MK_FP(_DS, (unsigned)blob))();
}

void cpuid4(unsigned long leaf, unsigned long *out)
{
    ((void(far *)(unsigned long, unsigned long far *))MK_FP(_DS, (unsigned)blob + 0x7e))(
        leaf, (unsigned long far *)out);
}

unsigned msw(void)
{
    return ((unsigned(far *)(void))MK_FP(_DS, (unsigned)blob + 0xa4))();
}

unsigned long xms(void far *entry, unsigned ax)
{
    return ((unsigned long(far *)(void far *, unsigned))MK_FP(_DS, (unsigned)blob + 0xa8))(entry,
                                                                                           ax);
}

/* --- sections and progress ------------------------------------------------------------------- */

int in_list(char *list, char *name)
{
    char *p = list;
    int n = strlen(name);
    while (*p) {
        if (strnicmp(p, name, n) == 0 && (p[n] == 0 || p[n] == ',')) {
            return 1;
        }
        while (*p && *p != ',') {
            p++;
        }
        if (*p == ',') {
            p++;
        }
    }
    return 0;
}

int want(char *name)
{
    if (only[0]) {
        return in_list(only, name);
    }
    return !in_list(skip, name);
}

/* Append src to a comma-separated list, so repeated /SKIP: and /ONLY: combine instead of
   overwriting each other. */
void add_list(char *dst, unsigned size, char *src)
{
    unsigned n = strlen(dst);
    if (n > 0) {
        if (n + 1 >= size) {
            return;
        }
        dst[n++] = ',';
        dst[n] = 0;
    }
    strncat(dst, src, size - n - 1);
}

void step(char *what)
{
    fflush(stdout);
    fprintf(stderr, "UNIDUMP: %s\n", what);
}

int section(char *name, char *title)
{
    if (!want(name)) {
        return 0;
    }
    step(name);
    printf("\n== %s (%s)\n", title, name);
    return 1;
}

/* --- memory above 1 MB ----------------------------------------------------------------------- */

/* Copy len bytes (even, at most 64K) from physical address phys to buf: directly below 1 MB,
   else through INT 15h AH=87h (also under EMM386, which emulates it). Returns 1 on success. */
int phys_read(unsigned long phys, void *buf, unsigned len)
{
    static unsigned char gdt[48];
    unsigned long dst;
    union REGS r;
    struct SREGS s;
    if (phys + len <= 0x100000L) {
        movedata((unsigned)(phys >> 4), (unsigned)(phys & 15), _DS, (unsigned)buf, len);
        return 1;
    }
    if (cpu < 2) {
        return 0;
    }
    memset(gdt, 0, sizeof(gdt));
    dst = ((unsigned long)_DS << 4) + (unsigned)buf;
    *(unsigned *)(gdt + 16) = len - 1;
    *(unsigned *)(gdt + 18) = (unsigned)phys;
    gdt[20] = (unsigned char)(phys >> 16);
    gdt[21] = 0x93;
    gdt[23] = (unsigned char)(phys >> 24);
    *(unsigned *)(gdt + 24) = len - 1;
    *(unsigned *)(gdt + 26) = (unsigned)dst;
    gdt[28] = (unsigned char)(dst >> 16);
    gdt[29] = 0x93;
    r.h.ah = 0x87;
    r.x.cx = (len + 1) / 2;
    r.x.si = (unsigned)gdt;
    segread(&s);
    s.es = _DS;
    int86x(0x15, &r, &r, &s);
    return !r.x.cflag && r.h.ah == 0;
}

/* --- resource data --------------------------------------------------------------------------- */

/* Compressed EISA ID (bytes as stored, e.g. 41 D0 00 00) to text, e.g. "PNP0000". */
char *eisa_id(unsigned char far *b, char *s)
{
    static char hex[] = "0123456789ABCDEF";
    s[0] = '@' + ((b[0] >> 2) & 0x1f);
    s[1] = '@' + (((b[0] & 3) << 3) | (b[1] >> 5));
    s[2] = '@' + (b[1] & 0x1f);
    s[3] = hex[b[2] >> 4];
    s[4] = hex[b[2] & 0xf];
    s[5] = hex[b[3] >> 4];
    s[6] = hex[b[3] & 0xf];
    s[7] = 0;
    return s;
}

/* Vendors of sound chips that UNISOUND knows, or that a BIOS may describe as on-board devices. */
int sound_vendor(char *id)
{
    static char *v[] = {"CSC", "ESS", "YMH", "OPT", "CTL", "AZT", "ALS", "CMI", "GRV",
                        "ENS", "BVJ", "RWB", "ADS", "DXP", "MIR", "RTL", 0};
    int i;
    for (i = 0; v[i]; i++) {
        if (memcmp(id, v[i], 3) == 0) {
            return 1;
        }
    }
    return 0;
}

void hexdump(unsigned char far *p, unsigned n, char *indent)
{
    unsigned i;
    for (i = 0; i < n; i++) {
        if (i % 16 == 0) {
            printf("%s%04X:", indent, i);
        }
        printf(" %02X", p[i]);
        if (i % 16 == 15 || i + 1 == n) {
            puts("");
        }
    }
}

void print_mask(char *what, unsigned mask, int max)
{
    int i, any = 0;
    printf("%s", what);
    for (i = 0; i <= max; i++) {
        if (mask & (1u << i)) {
            printf(" %d", i);
            any = 1;
        }
    }
    puts(any ? "" : " none");
}

/* Decode resource data p[0..n) up to and including the end tag; returns the bytes used, or n if
   there is no end tag. Every tag gets one line, unknown ones in hex. */
unsigned decode_res(unsigned char far *p, unsigned n, char *indent)
{
    unsigned i = 0, len, k;
    unsigned char t;
    char id[8];
    while (i < n) {
        t = p[i];
        if (t & 0x80) {
            if (i + 3 > n) {
                break;
            }
            len = p[i + 1] | (p[i + 2] << 8);
            i += 3;
        } else {
            len = t & 7;
            t = (t >> 3) & 0xf;
            i++;
        }
        if (i + len > n) {
            printf("%s(tag %02X runs past the end)\n", indent, t);
            return n;
        }
        printf("%s", indent);
        switch (t) {
            case 0x1:
                printf("PnP version %d.%d, vendor %02X\n", p[i] >> 4, p[i] & 0xf,
                       len > 1 ? p[i + 1] : 0);
                break;
            case 0x2:
                printf("Logical device %s", eisa_id(p + i, id));
                printf(len > 4 ? ", flags %02X\n" : "\n", p[i + 4]);
                break;
            case 0x3:
                printf("Compatible ID %s\n", eisa_id(p + i, id));
                break;
            case 0x4:
                print_mask("  IRQ", p[i] | (p[i + 1] << 8), 15);
                break;
            case 0x5:
                printf("  DMA");
                print_mask("", p[i], 7);
                break;
            case 0x6:
                printf("Dependent function%s\n", len == 0    ? ""
                                                 : p[i] == 0 ? " (good)"
                                                 : p[i] == 1 ? " (acceptable)"
                                                             : " (sub-optimal)");
                break;
            case 0x7:
                puts("End of dependent functions");
                break;
            case 0x8:
                printf("  I/O %03X-%03X align %X len %X, %s decode\n", p[i + 1] | (p[i + 2] << 8),
                       p[i + 3] | (p[i + 4] << 8), p[i + 5], p[i + 6],
                       p[i] & 1 ? "16-bit" : "10-bit");
                break;
            case 0x9:
                printf("  Fixed I/O %03X len %X\n", (p[i] | (p[i + 1] << 8)) & 0x3ff, p[i + 2]);
                break;
            case 0xf:
                puts("End");
                return i + len;
            case 0x82:
                printf("\"");
                for (k = 0; k < len; k++) {
                    putchar(p[i + k] >= ' ' ? p[i + k] : '.');
                }
                puts("\"");
                break;
            case 0x81:
                printf("  Memory %02X%02X00-%02X%02X00 len %02X%02X00\n", p[i + 2], p[i + 1],
                       p[i + 4], p[i + 3], p[i + 8], p[i + 7]);
                break;
            case 0x85:
                printf("  32-bit memory %08lX-%08lX align %08lX len %08lX\n",
                       *(unsigned long far *)(p + i + 1), *(unsigned long far *)(p + i + 5),
                       *(unsigned long far *)(p + i + 9), *(unsigned long far *)(p + i + 13));
                break;
            case 0x86:
                printf("  Fixed memory %08lX len %08lX\n", *(unsigned long far *)(p + i + 1),
                       *(unsigned long far *)(p + i + 5));
                break;
            default:
                printf("Tag %02X:", t);
                for (k = 0; k < len && k < 16; k++) {
                    printf(" %02X", p[i + k]);
                }
                puts(len > 16 ? " ..." : "");
                break;
        }
        i += len;
    }
    return n;
}

/* --- ENV ------------------------------------------------------------------------------------- */

void report_env(void)
{
    static char *vars[] = {"BLASTER", "ULTRASND", "ULTRA16", "UNISOUND_CD", "SOUND", "MIDI", 0};
    int i;
    union REGS r;
    r.x.ax = 0x3000;
    intdos(&r, &r);
    printf("DOS %d.%02d", r.h.al, r.h.ah);
    r.x.ax = 0x3306;
    r.x.bx = 0;
    intdos(&r, &r);
    if (r.h.bl >= 5) {
        printf(" (true version %d.%02d, revision %d, %s)", r.h.bl, r.h.bh, r.h.dl & 7,
               r.h.dh & 0x10 ? "in HMA" : "low");
    }
    r.x.ax = 0x3305;
    intdos(&r, &r);
    printf(", boot drive %c:\n", r.h.dl ? 'A' + r.h.dl - 1 : '?');
    for (i = 0; vars[i]; i++) {
        char *v = getenv(vars[i]);
        printf("%s=%s\n", vars[i], v ? v : "(not set)");
    }
}

/* --- CPU ------------------------------------------------------------------------------------- */

void report_cpu(void)
{
    static char *cls[] = {"8086/8088", "?", "80286", "80386", "80486 (no CPUID)", "CPUID"};
    static char *feat[] = {"FPU",  "VME",   "DE",   "PSE",   "TSC",  "MSR", "PAE",  "MCE",
                           "CX8",  "APIC",  "",     "SEP",   "MTRR", "PGE", "MCA",  "CMOV",
                           "PAT",  "PSE36", "PSN",  "CLFSH", "",     "DS",  "ACPI", "MMX",
                           "FXSR", "SSE",   "SSE2", "SS",    "HTT",  "TM",  "",     "PBE"};
    unsigned long r[4], max, ext;
    char name[52];
    union REGS g;
    int i;
    cpu = cpu_class();
    printf("Class: %s\n", cls[cpu]);
    int86(0x11, &g, &g);
    printf("FPU (INT 11h): %s\n", g.x.ax & 2 ? "yes" : "no");
    if (cpu >= 2) {
        printf("Mode: %s\n", msw() & 1 ? "virtual 8086 (EMM386, QEMM or Windows)" : "real");
    }
    if (cpu < 5) {
        return;
    }
    cpuid4(0, r);
    max = r[0];
    memcpy(name, &r[1], 4);
    memcpy(name + 4, &r[3], 4);
    memcpy(name + 8, &r[2], 4);
    name[12] = 0;
    printf("CPUID: vendor %s, max leaf %lu\n", name, max);
    if (max >= 1) {
        cpuid4(1, r);
        printf("Family %lu, model %lu, stepping %lu (signature %08lX)\nFeatures:",
               ((r[0] >> 8) & 15) + (((r[0] >> 8) & 15) == 15 ? ((r[0] >> 20) & 0xff) : 0),
               ((r[0] >> 4) & 15) | (((r[0] >> 8) & 15) >= 6 ? ((r[0] >> 12) & 0xf0) : 0),
               r[0] & 15, r[0]);
        for (i = 0; i < 32; i++) {
            if ((r[3] >> i) & 1 && feat[i][0]) {
                printf(" %s", feat[i]);
            }
        }
        puts("");
    }
    cpuid4(0x80000000L, r);
    ext = r[0];
    if (ext >= 0x80000004L && ext < 0x8000ffffL) {
        for (i = 0; i < 3; i++) {
            cpuid4(0x80000002L + i, (unsigned long *)(name + 16 * i));
        }
        name[48] = 0;
        printf("Name: %s\n", name);
    }
}

/* --- MEM ------------------------------------------------------------------------------------- */

/* CMOS byte idx, keeping the NMI enabled (bit 7 of the index clear). */
unsigned cmos(unsigned char idx)
{
    unsigned char v;
    disable();
    outportb(0x70, idx);
    v = inportb(0x71);
    enable();
    return v;
}

void report_mem(void)
{
    union REGS r;
    int86(0x12, &r, &r);
    printf("Conventional: %u KB\n", r.x.ax);
    r.x.ax = 0xe801;
    r.x.bx = r.x.cx = r.x.dx = 0;
    int86(0x15, &r, &r);
    if (!r.x.cflag && (r.x.ax || r.x.cx)) {
        unsigned lo = r.x.cx ? r.x.cx : r.x.ax, hi = r.x.cx ? r.x.dx : r.x.bx;
        printf("Extended (E801h): %u KB below 16 MB, %u x 64 KB above: %lu KB in all\n", lo, hi,
               lo + 64UL * hi);
    } else {
        r.x.ax = 0x8800;
        int86(0x15, &r, &r);
        if (r.x.cflag) {
            printf("Extended (88h): no answer (AH %02X)\n", r.h.ah);
        } else {
            printf("Extended (88h): %u KB (an XMS driver may report 0)\n", r.x.ax);
        }
    }
    printf("Extended (CMOS): %u KB set up, %u KB found by POST%s\n", cmos(0x17) | (cmos(0x18) << 8),
           cmos(0x30) | (cmos(0x31) << 8),
           (cmos(0x30) | (cmos(0x31) << 8)) == 0xffff ? " (the most CMOS can hold: 64 MB or more)"
                                                      : "");
}

/* --- BIOS ------------------------------------------------------------------------------------ */

/* Does p[0..n) contain word? */
int has(unsigned char far *p, unsigned n, char *word)
{
    unsigned k, w = strlen(word);
    for (k = 0; k + w <= n; k++) {
        if (_fmemcmp(p + k, word, w) == 0) {
            return 1;
        }
    }
    return 0;
}

/* Print the readable runs of the ROM at seg:0000-FFFF that name a BIOS maker or a copyright. */
void bios_strings(unsigned seg)
{
    unsigned char far *p = MK_FP(seg, 0);
    unsigned i = 0, j, n = 0, k;
    while (i < 0xfff0 && n < 10) {
        for (j = i; j < 0xffff && p[j] >= ' ' && p[j] < 0x7f; j++) {
        }
        if (j - i >= 12) {
            static char *w[] = {"Award",
                                "AMI",
                                "American Megatrends",
                                "Phoenix",
                                "Copyright",
                                "(C)",
                                "(c)",
                                "MR BIOS",
                                "SystemSoft",
                                "Quadtel",
                                "Insyde",
                                "Compaq",
                                "IBM",
                                "Dell",
                                "Gateway",
                                "Intel",
                                0};
            int hit = 0, x;
            for (x = 0; w[x] && !hit; x++) {
                hit = has(p + i, j - i, w[x]);
            }
            if (hit) {
                printf("  %04X:%04X \"", seg, i);
                for (k = i; k < j && k < i + 70; k++) {
                    putchar(p[k]);
                }
                puts(j - i > 70 ? "...\"" : "\"");
                n++;
            }
        }
        i = j + 1;
    }
}

/* String number n (1-based) of the string area s[0..len). Every scan is bounded by len, so a
   malformed table can only yield "?" or "(blank)", never a read past the table. */
char *smb_str(unsigned char *s, int n, unsigned len)
{
    unsigned char *p = s, *q, *end = s + len;
    unsigned k;
    if (n == 0) {
        return "";
    }
    while (--n > 0 && p < end) {
        for (k = 0; p + k < end && p[k]; k++) {
        }
        if (p + k >= end) {
            return "?"; /* unterminated string: no NUL before the end of the area */
        }
        p += k + 1;
        if (p >= end || *p == 0) {
            return "?"; /* past the last string: index out of range */
        }
    }
    if (p >= end) {
        return "?";
    }
    /* The string must end with a NUL inside the area; without one printf %s would read on. */
    for (k = 0; p + k < end && p[k]; k++) {
    }
    if (p + k >= end) {
        return "?";
    }
    for (q = p; *q == ' '; q++) {
    }
    return *q ? (char *)p : "(blank)";
}

/* An MPU-401's status port reads bit 6 clear when it can take a command, bit 7 set when it has
   no data; FFh is no device. */
void probe_mpu(unsigned base)
{
    unsigned char st = inportb(base + 1);
    printf("MPU %X: status %02X, %s\n", base, st,
           st == 0xff  ? "none"
           : st & 0x40 ? "busy or none"
           : st & 0x80 ? "ready"
                       : "ready, data waiting");
}

/* SMBIOS (_SM_) or legacy DMI (_DMI_) entry point in F0000h-FFFFFh, and its table. */
void report_smbios(void)
{
    unsigned off, tlen = 0, count = 0, i;
    unsigned long taddr = 0;
    unsigned char far *e = 0, *t, *p, *end;
    for (off = 0; off < 0xfff0; off += 16) {
        unsigned char far *q = MK_FP(0xf000, off);
        if (q[0] == '_' && q[1] == 'S' && q[2] == 'M' && q[3] == '_') {
            e = q;
            printf("SMBIOS %d.%d at F000:%04X", q[6], q[7], off);
            tlen = *(unsigned far *)(q + 0x16);
            taddr = *(unsigned long far *)(q + 0x18);
            count = *(unsigned far *)(q + 0x1c);
            break;
        }
        if (q[0] == '_' && q[1] == 'D' && q[2] == 'M' && q[3] == 'I' && q[4] == '_') {
            e = q;
            printf("DMI %d.%d at F000:%04X", q[14] >> 4, q[14] & 15, off);
            tlen = *(unsigned far *)(q + 6);
            taddr = *(unsigned long far *)(q + 8);
            count = *(unsigned far *)(q + 12);
            break;
        }
    }
    if (e == 0) {
        puts("No SMBIOS or DMI table");
        return;
    }
    printf(": %u structures, %u bytes at %08lX\n", count, tlen, taddr);
    if (tlen == 0 || tlen > 8192 || (t = malloc(tlen + 2)) == 0) {
        puts("  table not read (too big or no memory)");
        return;
    }
    if (!phys_read(taddr, t, (tlen + 1) & ~1)) {
        puts("  table not read (INT 15h AH=87h failed)");
        free(t);
        return;
    }
    p = t;
    end = t + tlen;
    for (i = 0; i < count && p + 4 <= end; i++) {
        unsigned char ty = p[0], ln = p[1];
        unsigned char *s, *n;
        if (ln < 4 || p + ln > end) {
            /* A length past the end of the table would make the string scan below read past
               the malloc'd buffer. */
            printf("  type %u: (truncated structure, len %u)\n", ty, ln);
            break;
        }
        s = p + ln;
        n = s;
        while (n + 1 < end && (n[0] || n[1])) {
            n++;
        }
        n += 2;
        if (n > end) {
            n = end;
        }
        switch (ty) {
            case 0:
                if (ln >= 9) {
                    printf("  BIOS: %s, version %s, date %s\n", smb_str(s, p[4], n - s),
                           smb_str(s, p[5], n - s), smb_str(s, p[8], n - s));
                } else {
                    printf("  BIOS: (short structure, len %u)\n", ln);
                }
                break;
            case 1:
                if (ln >= 7) {
                    printf("  System: %s %s, version %s\n", smb_str(s, p[4], n - s),
                           smb_str(s, p[5], n - s), smb_str(s, p[6], n - s));
                } else {
                    printf("  System: (short structure, len %u)\n", ln);
                }
                break;
            case 2:
                if (ln >= 7) {
                    printf("  Board: %s %s, version %s\n", smb_str(s, p[4], n - s),
                           smb_str(s, p[5], n - s), smb_str(s, p[6], n - s));
                } else {
                    printf("  Board: (short structure, len %u)\n", ln);
                }
                break;
            case 4:
                if (ln >= 0x18) {
                    printf("  Processor: %s, %s %s, %u MHz (max %u)\n", smb_str(s, p[4], n - s),
                           smb_str(s, p[7], n - s), smb_str(s, p[0x10], n - s),
                           *(unsigned *)(p + 0x16), *(unsigned *)(p + 0x14));
                } else {
                    printf("  Processor: (short structure, len %u)\n", ln);
                }
                break;
            case 6:
                if (ln >= 10) {
                    printf("  Memory module %s: size code %02X\n", smb_str(s, p[4], n - s), p[9]);
                } else {
                    printf("  Memory module: (short structure, len %u)\n", ln);
                }
                break;
            case 17:
                if (ln >= 0x11) {
                    unsigned sz = *(unsigned *)(p + 0x0c);
                    printf("  Memory device %s: ", smb_str(s, p[0x10], n - s));
                    if (sz == 0) {
                        puts("empty");
                    } else if (sz == 0xffff) {
                        puts("size unknown");
                    } else {
                        printf("%u %s\n", sz & 0x7fff, sz & 0x8000 ? "KB" : "MB");
                    }
                }
                break;
            case 127:
                i = count;
                break;
        }
        p = n;
    }
    free(t);
}

void report_bios(void)
{
    char date[9];
    union REGS r;
    struct SREGS s;
    movedata(0xf000, 0xfff5, _DS, (unsigned)date, 8);
    date[8] = 0;
    printf("ROM date %s, model byte %02X\n", date, *(unsigned char far *)MK_FP(0xf000, 0xfffe));
    r.h.ah = 0xc0;
    segread(&s);
    int86x(0x15, &r, &r, &s);
    if (!r.x.cflag && r.h.ah == 0) {
        unsigned char far *c = MK_FP(s.es, r.x.bx);
        printf("INT 15h C0h: model %02X, submodel %02X, revision %02X, features %02X\n", c[2], c[3],
               c[4], c[5]);
    }
    puts("ID strings:");
    bios_strings(0xf000);
    report_smbios();
}

/* --- PCI ------------------------------------------------------------------------------------- */

unsigned pci_w(unsigned char bus, unsigned char df, unsigned reg)
{
    union REGS r;
    r.x.ax = 0xb109;
    r.h.bh = bus;
    r.h.bl = df;
    r.x.di = reg;
    int86(0x1a, &r, &r);
    return r.x.cflag ? 0xffff : r.x.cx;
}

unsigned char pci_b(unsigned char bus, unsigned char df, unsigned reg)
{
    union REGS r;
    r.x.ax = 0xb108;
    r.h.bh = bus;
    r.h.bl = df;
    r.x.di = reg;
    int86(0x1a, &r, &r);
    return r.x.cflag ? 0xff : r.h.cl;
}

char *pci_vendor(unsigned v)
{
    static struct {
        unsigned id;
        char *name;
    } t[] = {{0x8086, "Intel"},
             {0x1106, "VIA"},
             {0x10b9, "ALi"},
             {0x1039, "SiS"},
             {0x1022, "AMD"},
             {0x1045, "OPTi"},
             {0x1060, "UMC"},
             {0x1078, "Cyrix"},
             {0x1274, "Ensoniq"},
             {0x1102, "Creative"},
             {0x125d, "ESS"},
             {0x13f6, "C-Media"},
             {0x1073, "Yamaha"},
             {0x1013, "Cirrus"},
             {0x10de, "NVIDIA"},
             {0x1002, "ATI"},
             {0x5333, "S3"},
             {0x1023, "Trident"},
             {0x102b, "Matrox"},
             {0x121a, "3dfx"},
             {0x104c, "TI"},
             {0x10ec, "Realtek"},
             {0x1011, "DEC"},
             {0x10b7, "3Com"},
             {0x9004, "Adaptec"},
             {0x1000, "LSI/Symbios"},
             {0x14f1, "Conexant"},
             {0x11d4, "Analog Devices"},
             {0x1319, "ForteMedia"},
             {0x1412, "VIA/ICEnsemble"},
             {0x5053, "Voyetra/Aureal"},
             {0x12eb, "Aureal"},
             {0, 0}};
    int i;
    for (i = 0; t[i].name; i++) {
        if (t[i].id == v) {
            return t[i].name;
        }
    }
    return "";
}

void report_pci(void)
{
    union REGS r;
    unsigned bus, dev, fn, last, ven, did;
    unsigned char df, ht, c0, c1, c2;
    r.x.ax = 0xb101;
    r.x.di = 0;
    int86(0x1a, &r, &r);
    if (r.x.cflag || r.h.ah != 0) {
        puts("No PCI BIOS");
        return;
    }
    last = r.h.cl;
    printf("PCI BIOS %X.%02X, mechanism %s, last bus %u\n", r.h.bh, r.h.bl,
           r.h.al & 1   ? "1"
           : r.h.al & 2 ? "2"
                        : "?",
           last);
    for (bus = 0; bus <= last; bus++) {
        for (dev = 0; dev < 32; dev++) {
            for (fn = 0; fn < 8; fn++) {
                df = (unsigned char)((dev << 3) | fn);
                ven = pci_w(bus, df, 0);
                if (ven == 0xffff || ven == 0) {
                    if (fn == 0) {
                        break;
                    }
                    continue;
                }
                did = pci_w(bus, df, 2);
                c0 = pci_b(bus, df, 0x0b);
                c1 = pci_b(bus, df, 0x0a);
                c2 = pci_b(bus, df, 0x09);
                ht = pci_b(bus, df, 0x0e);
                printf("  %02X:%02X.%d %04X:%04X rev %02X class %02X %02X %02X", bus, dev, fn, ven,
                       did, pci_b(bus, df, 8), c0, c1, c2);
                if ((ht & 0x7f) == 0) {
                    printf(" sub %04X:%04X", pci_w(bus, df, 0x2c), pci_w(bus, df, 0x2e));
                    if (pci_b(bus, df, 0x3d)) {
                        printf(" irq %d", pci_b(bus, df, 0x3c));
                    }
                }
                printf(" %s%s\n", pci_vendor(ven),
                       c0 == 4 && (c1 == 1 || c1 == 3) ? "   <-- sound" : "");
                if (fn == 0 && !(ht & 0x80)) {
                    break;
                }
            }
        }
    }
}

/* --- DOS ------------------------------------------------------------------------------------- */

unsigned first_mcb(void)
{
    union REGS r;
    struct SREGS s;
    r.h.ah = 0x52;
    segread(&s);
    intdosx(&r, &r, &s);
    return *(unsigned far *)MK_FP(s.es, r.x.bx - 2);
}

/* Name of the program owning the memory block at seg (from its MCB), or "" if none is found. */
char *owner_name(unsigned seg)
{
    static char name[12];
    unsigned m = first_mcb(), n;
    int k;
    if (seg < m) {
        return "DOS kernel";
    }
    for (k = 0; k < 400; k++) {
        unsigned char far *b = MK_FP(m, 0);
        unsigned size = *(unsigned far *)(b + 3), own = *(unsigned far *)(b + 1);
        if (b[0] != 'M' && b[0] != 'Z') {
            break;
        }
        if (seg > m && seg <= m + size) {
            if (own == 0) {
                return "free";
            }
            if (own == 8) {
                return "DOS";
            }
            for (n = 0; n < 8; n++) {
                unsigned char c = *(unsigned char far *)MK_FP(own - 1, 8 + n);
                name[n] = c >= ' ' && c < 0x7f ? c : 0;
                if (c == 0) {
                    break;
                }
            }
            name[n] = 0;
            return name;
        }
        if (b[0] == 'Z') {
            break;
        }
        m += size + 1;
    }
    return "";
}

void print_file(char *path)
{
    FILE *f = fopen(path, "r");
    char line[200];
    int n = 0;
    if (f == 0) {
        printf("%s: not found\n", path);
        return;
    }
    printf("%s:\n", path);
    while (fgets(line, sizeof(line), f) && n++ < 150) {
        printf("  %s", line);
        if (line[0] && line[strlen(line) - 1] != '\n') {
            puts("");
        }
    }
    fclose(f);
}

void report_dos(void)
{
    union REGS r;
    struct SREGS s;
    unsigned m, k, link = 0, haslink;
    void far *entry;
    char path[16];
    char far *emm = MK_FP(*(unsigned far *)MK_FP(0, 0x67 * 4 + 2), 10);
    if (_fmemcmp(emm, "EMMXXXX0", 8) == 0 || _fmemcmp(emm, "EMMQXXX0", 8) == 0) {
        r.h.ah = 0x46;
        int86(0x67, &r, &r);
        printf("EMS: %.8Fs, version %d.%d", emm, r.h.al >> 4, r.h.al & 15);
        r.h.ah = 0x42;
        int86(0x67, &r, &r);
        printf(", %u of %u pages free\n", r.x.bx, r.x.dx);
    } else {
        puts("EMS: none");
    }
    r.x.ax = 0x4300;
    int86(0x2f, &r, &r);
    if (r.h.al == 0x80) {
        unsigned long v, f;
        r.x.ax = 0x4310;
        segread(&s);
        int86x(0x2f, &r, &r, &s);
        entry = MK_FP(s.es, r.x.bx);
        v = xms(entry, 0x0000);
        f = xms(entry, 0x0800);
        printf("XMS: version %X.%02X, HMA %s, %lu KB free (largest block %u KB)\n",
               (unsigned)v >> 8, (unsigned)v & 0xff, (v >> 16) ? "present" : "absent", f >> 16,
               (unsigned)f);
    } else {
        puts("XMS: none");
    }
    r.x.ax = 0x1600;
    int86(0x2f, &r, &r);
    if (r.h.al != 0 && r.h.al != 0x80) {
        printf("Windows: enhanced mode %d.%02d\n", r.h.al == 0xff || r.h.al == 1 ? 2 : r.h.al,
               r.h.al == 0xff || r.h.al == 1 ? 0 : r.h.ah);
    } else {
        puts("Windows: not running in enhanced mode");
    }
    /* include the upper memory blocks: link them in for the walk, then restore the link state */
    r.x.ax = 0x5802;
    intdos(&r, &r);
    haslink = !r.x.cflag;
    if (haslink) {
        link = r.h.al;
        r.x.ax = 0x5803;
        r.x.bx = 1;
        intdos(&r, &r);
    }
    puts("Memory blocks (segment, bytes, owner):");
    m = first_mcb();
    for (k = 0; k < 400; k++) {
        unsigned char far *b = MK_FP(m, 0);
        unsigned size = *(unsigned far *)(b + 3), own = *(unsigned far *)(b + 1);
        if (b[0] != 'M' && b[0] != 'Z') {
            printf("  %04X: chain broken (%02X)\n", m, b[0]);
            break;
        }
        printf("  %04X %7lu %s%s\n", m + 1, 16UL * size,
               own == 0   ? "free"
               : own == 8 ? "DOS"
                          : "",
               own == 0 || own == 8 ? "" : owner_name(m + 1));
        if (b[0] == 'Z') {
            break;
        }
        m += size + 1;
    }
    if (haslink) {
        r.x.ax = 0x5803;
        r.x.bx = link;
        intdos(&r, &r);
    }
    puts("Device drivers:");
    r.h.ah = 0x52;
    segread(&s);
    intdosx(&r, &r, &s);
    {
        unsigned char far *d = MK_FP(s.es, r.x.bx + 0x22);
        for (k = 0; k < 100; k++) {
            unsigned attr = *(unsigned far *)(d + 4);
            if (attr & 0x8000) {
                printf("  %Fp %.8Fs\n", d, d + 10);
            } else {
                printf("  %Fp block device, %d units\n", d, d[10]);
            }
            if (*(unsigned far *)d == 0xffff) {
                break;
            }
            d = MK_FP(*(unsigned far *)(d + 2), *(unsigned far *)d);
        }
    }
    if (opt_nofiles) {
        puts("CONFIG.SYS and AUTOEXEC.BAT: not included (/NOFILES)");
        return;
    }
    r.x.ax = 0x3305;
    intdos(&r, &r);
    sprintf(path, "%c:\\CONFIG.SYS", r.h.dl ? 'A' + r.h.dl - 1 : 'C');
    print_file(path);
    sprintf(path, "%c:\\AUTOEXEC.BAT", r.h.dl ? 'A' + r.h.dl - 1 : 'C');
    print_file(path);
}

/* --- IRQ ------------------------------------------------------------------------------------- */

void report_irq(void)
{
    unsigned char imr1, imr2, irr1, irr2, isr1, isr2;
    unsigned far *bda = MK_FP(0x40, 0);
    int i;
    disable();
    imr1 = inportb(0x21);
    imr2 = inportb(0xa1);
    outportb(0x20, 0x0a);
    outportb(0xa0, 0x0a);
    irr1 = inportb(0x20);
    irr2 = inportb(0xa0);
    outportb(0x20, 0x0b);
    outportb(0xa0, 0x0b);
    isr1 = inportb(0x20);
    isr2 = inportb(0xa0);
    outportb(0x20, 0x0a);
    outportb(0xa0, 0x0a);
    enable();
    printf("PIC: mask %02X %02X, requests %02X %02X, in service %02X %02X\n", imr1, imr2, irr1,
           irr2, isr1, isr2);
    puts("IRQ  INT  vector     mask pending  owner");
    for (i = 0; i < 16; i++) {
        int v = i < 8 ? 8 + i : 0x70 + i - 8;
        unsigned off = *(unsigned far *)MK_FP(0, v * 4), seg = *(unsigned far *)MK_FP(0, v * 4 + 2);
        unsigned char mk = i < 8 ? (imr1 >> i) & 1 : (imr2 >> (i - 8)) & 1;
        unsigned char rq = i < 8 ? (irr1 >> i) & 1 : (irr2 >> (i - 8)) & 1;
        char *own = seg >= 0xf000 ? "BIOS" : seg >= 0xc000 ? "ROM/UMB" : owner_name(seg);
        if (seg >= 0xc000 && seg < 0xf000 && owner_name(seg)[0]) {
            own = owner_name(seg);
        }
        printf("%2d   %02Xh  %04X:%04X  %s  %s      %s\n", i, v, seg, off, mk ? "off " : "on  ",
               rq ? "yes" : "-  ", own);
    }
    printf("COM ports: %X %X %X %X\nLPT ports: %X %X %X\n", bda[0], bda[1], bda[2], bda[3], bda[4],
           bda[5], bda[6]);
}

/* --- PNPBIOS --------------------------------------------------------------------------------- */

unsigned char far *find_pnp_bios(void)
{
    unsigned off;
    unsigned char sum;
    int i;
    for (off = 0; off < 0xfff0; off += 16) {
        unsigned char far *h = MK_FP(0xf000, off);
        if (h[0] == '$' && h[1] == 'P' && h[2] == 'n' && h[3] == 'P' && h[5] >= 0x21) {
            sum = 0;
            for (i = 0; i < h[5]; i++) {
                sum += h[i];
            }
            if (sum == 0) {
                return h;
            }
        }
    }
    return 0;
}

void report_pnp_bios(void)
{
    unsigned char far *h = find_pnp_bios();
    unsigned char nnodes = 0, node, cfg[6];
    unsigned nsize = 0, minw = 0, esize = 0, ax;
    unsigned long nvbase = 0;
    unsigned char *buf;
    char id[8];
    int count = 0;
    if (h == 0) {
        puts("No PnP BIOS ($PnP header) found.");
        return;
    }
    pnp_entry = (PNPFN)MK_FP(*(unsigned far *)(h + 0x0f), *(unsigned far *)(h + 0x0d));
    pnp_ds = *(unsigned far *)(h + 0x1b);
    printf("$PnP at F000:%04X: version %d.%d, control %04X, real-mode entry %04X:%04X, data %04X, "
           "OEM ID %08lX\n",
           FP_OFF(h), h[4] >> 4, h[4] & 0xf, *(unsigned far *)(h + 6), *(unsigned far *)(h + 0x0f),
           *(unsigned far *)(h + 0x0d), pnp_ds, *(unsigned long far *)(h + 0x17));
    step("PnP BIOS function 00h");
    ax = pnp_entry(0x00, (unsigned char far *)&nnodes, (unsigned far *)&nsize, pnp_ds);
    printf("Function 00h: %04X, %d device nodes, largest %u bytes\n", ax, nnodes, nsize);
    if (ax == 0 && nsize <= sizeof(node_buf)) {
        node = 0;
        while (node != 0xff && count < 256) {
            unsigned size, used, pos;
            unsigned char this_node = node;
            step("PnP BIOS function 01h");
            ax = pnp_entry(0x01, (unsigned char far *)&node, (unsigned char far *)node_buf, 1,
                           pnp_ds);
            if (ax != 0) {
                printf("Function 01h node %02X: %04X\n", this_node, ax);
                break;
            }
            count++;
            size = node_buf[0] | (node_buf[1] << 8);
            eisa_id(node_buf + 3, id);
            printf("\nNode %02X: %s, type %02X %02X %02X, attributes %04X%s\n", node_buf[2], id,
                   node_buf[7], node_buf[8], node_buf[9], node_buf[10] | (node_buf[11] << 8),
                   (node_buf[7] == 4 && node_buf[8] == 1 && strcmp(id, "PNP0800") != 0) ||
                           sound_vendor(id)
                       ? "   <-- sound"
                       : "");
            if (size > 12 && size <= sizeof(node_buf)) {
                /* three resource blocks, each ending with an end tag: allocated (current),
                   possible, and compatible */
                puts("    current:");
                pos = 12 + decode_res(node_buf + 12, size - 12, "    ");
                if (pos < size && node_buf[pos] != 0x79) {
                    puts("    possible:");
                    used = decode_res(node_buf + pos, size - pos, "    ");
                    pos += used;
                } else {
                    pos += 2;
                }
                if (pos < size && node_buf[pos] != 0x79) {
                    puts("    compatible:");
                    decode_res(node_buf + pos, size - pos, "    ");
                }
            }
        }
    }
    step("PnP BIOS function 40h");
    ax = pnp_entry(0x40, (unsigned char far *)cfg, pnp_ds);
    if (ax == 0) {
        rdport = cfg[2] | (cfg[3] << 8);
        pnp_bios_csns = cfg[1];
        printf("\nFunction 40h: ISA PnP revision %d, %d CSNs, read port %X\n", cfg[0], cfg[1],
               rdport);
    } else {
        printf("\nFunction 40h: %04X (no ISA PnP configuration)\n", ax);
    }
    step("PnP BIOS function 41h");
    ax = pnp_entry(0x41, (unsigned far *)&minw, (unsigned far *)&esize,
                   (unsigned long far *)&nvbase, pnp_ds);
    if (ax != 0) {
        printf("Function 41h: %04X (no ESCD)\n", ax);
        return;
    }
    printf("Function 41h: ESCD %u bytes, minimum write %u, at %08lX\n", esize, minw, nvbase);
    if (!opt_escd) {
        return;
    }
    if (esize == 0 || esize > 0x4000 || (buf = malloc(esize + 1)) == 0) {
        puts("ESCD not read (empty, over 16K, or no memory)");
        return;
    }
    if (nvbase < 0x100000L) {
        step("PnP BIOS function 42h");
        ax = pnp_entry(0x42, (unsigned char far *)buf, (unsigned)(nvbase >> 4), pnp_ds);
        printf("Function 42h: %04X\n", ax);
    } else {
        step("ESCD from memory");
        ax = phys_read(nvbase, buf, (esize + 1) & ~1) ? 0 : 1;
        printf("ESCD read from %08lX with INT 15h AH=87h: %s\n", nvbase, ax ? "failed" : "ok");
    }
    if (ax == 0) {
        if (esize >= 6 && memcmp(buf + 2, "ACFG", 4) != 0) {
            puts("  (no ACFG signature at offset 2: this may not be the ESCD)");
        }
        hexdump(buf, esize, "  ");
    }
    free(buf);
}

/* --- PNP ------------------------------------------------------------------------------------- */

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
    return inportb(rdport);
}

/* Next resource data byte, or -1 if the status bit doesn't come up in 5000 polls (a few ms). */
int res_byte(void)
{
    int k;
    for (k = 0; k < 5000; k++) {
        if (rd(0x05) & 1) {
            return rd(0x04);
        }
    }
    return -1;
}

/* Read the woken card's serial ID and resource data into res, up to the end tag and its
   checksum byte. Returns the number of bytes, or 0 on a timeout before the end tag. */
unsigned read_card(void)
{
    unsigned n = 0, len;
    int b, t;
    for (n = 0; n < 9; n++) {
        if ((b = res_byte()) < 0) {
            return 0;
        }
        res[n] = b;
    }
    while (n + 3 < RES_MAX) {
        if ((t = res_byte()) < 0) {
            return 0;
        }
        res[n++] = t;
        if (t & 0x80) {
            int lo = res_byte(), hi = res_byte();
            if (lo < 0 || hi < 0) {
                return 0;
            }
            res[n++] = lo;
            res[n++] = hi;
            len = lo | (hi << 8);
        } else {
            len = t & 7;
        }
        while (len-- > 0 && n < RES_MAX) {
            if ((b = res_byte()) < 0) {
                return 0;
            }
            res[n++] = b;
        }
        if ((t & 0xf8) == 0x78) {
            return n;
        }
    }
    return n;
}

/* Wake card csn, print its serial ID, resource data and logical devices' settings, and return
   it to Wait for Key. */
void report_card(int csn)
{
    unsigned n, k, nld;
    int ld, i;
    char id[8];
    key();
    wr(0x03, csn);
    n = read_card();
    printf("\nCSN %d:", csn);
    if (n < 9) {
        puts(" no answer");
        wr(0x02, 0x02);
        return;
    }
    printf(" %s serial %02X%02X%02X%02X, %u bytes%s\n", eisa_id(res, id), res[7], res[6], res[5],
           res[4], n, n < 10 ? " (no resource data)" : "");
    hexdump(res, n, "  ");
    decode_res(res + 9, n - 9, "    ");
    for (nld = 0, k = 9; k < n;) {
        unsigned char t = res[k];
        if (t & 0x80) {
            k += 3 + (res[k + 1] | (res[k + 2] << 8));
        } else {
            if (((t >> 3) & 0xf) == 2) {
                nld++;
            }
            k += 1 + (t & 7);
        }
    }
    for (ld = 0; ld < (int)nld; ld++) {
        wr(0x07, ld);
        printf("  LD %d: act %02X io", ld, rd(0x30));
        for (i = 0; i < 8; i++) {
            printf(" %02X%02X", rd(0x60 + 2 * i), rd(0x61 + 2 * i));
        }
        printf(" irq %02X/%02X %02X/%02X dma %02X %02X\n", rd(0x70), rd(0x71), rd(0x72), rd(0x73),
               rd(0x74), rd(0x75));
    }
    wr(0x02, 0x02);
    fflush(stdout);
}

/* One isolation pass among the cards in Isolation state: read the 72-bit serial ID bit by bit
   (55h then AAh means 1). Returns 1 if a card won with a good checksum, 0 if no card answered,
   -1 on a bad checksum. */
int isolate_one(void)
{
    unsigned char id[9], c = 0x6a;
    int bit, any = 0, b;
    memset(id, 0, sizeof(id));
    outportb(ADDR, 0x01);
    delay(1);
    for (bit = 0; bit < 72; bit++) {
        unsigned char a1 = inportb(rdport), a2;
        delay(1);
        a2 = inportb(rdport);
        delay(1);
        b = a1 == 0x55 && a2 == 0xaa;
        any |= b;
        if (b) {
            id[bit / 8] |= 1 << (bit % 8);
        }
        if (bit < 64) {
            c = (c >> 1) | (((c ^ (c >> 1) ^ b) & 1) << 7);
        }
    }
    if (!any) {
        return 0;
    }
    return c == id[8] ? 1 : -1;
}

void isolate(void)
{
    int csn, r, bad = 0;
    int reset = pnp_bios_csns <= 0;
    if (rdport < 0x203 || rdport > 0x3ff) {
        rdport = 0x20b;
    }
    printf("\nIsolation on read port %X, %s:\n", rdport,
           reset ? "CSNs reset first" : "only cards without a CSN");
    step("ISA PnP isolation");
    key();
    if (reset) {
        wr(0x02, 0x04);
        delay(2);
    }
    csn = reset ? 1 : pnp_bios_csns + 1;
    wr(0x03, 0x00);
    wr(0x00, rdport >> 2);
    while (csn < 255) {
        r = isolate_one();
        if (r == 0) {
            break;
        }
        if (r < 0) {
            if (++bad > 3) {
                puts("  checksum errors, giving up");
                break;
            }
            wr(0x03, 0x00);
            continue;
        }
        wr(0x06, csn);
        printf("  card assigned CSN %d\n", csn);
        csn++;
        wr(0x03, 0x00);
    }
    wr(0x02, 0x02);
    if (csn == (reset ? 1 : pnp_bios_csns + 1)) {
        puts("  no card answered");
    }
    for (r = reset ? 1 : pnp_bios_csns + 1; r < csn; r++) {
        report_card(r);
    }
}

void report_isa_cards(void)
{
    int csn;
    if (pnp_bios_csns < 0 && !opt_isolate) {
        puts("No ISA PnP configuration from a PnP BIOS: UNIDUMP /ISOLATE or UNISOUND /LOG "
             "isolates the cards.");
        return;
    }
    if (pnp_bios_csns == 0 && !opt_isolate) {
        puts("The BIOS assigned no CSNs: UNIDUMP /ISOLATE or UNISOUND /LOG isolates the cards.");
    }
    for (csn = 1; csn <= pnp_bios_csns; csn++) {
        step("ISA PnP card");
        report_card(csn);
    }
    if (opt_isolate) {
        isolate();
    }
}

/* --- non-PnP probes -------------------------------------------------------------------------- */

int dsp_write(unsigned base, unsigned char v)
{
    long i;
    for (i = 0; i < 20000L; i++) {
        if (!(inportb(base + 0xc) & 0x80)) {
            outportb(base + 0xc, v);
            return 1;
        }
    }
    return 0;
}

int dsp_read(unsigned base)
{
    long i;
    for (i = 0; i < 20000L; i++) {
        if (inportb(base + 0xe) & 0x80) {
            return inportb(base + 0xa);
        }
    }
    return -1;
}

void probe_sb(void)
{
    unsigned base;
    int k, r, hi, lo, e1, e2;
    for (base = 0x220; base <= 0x280; base += 0x20) {
        step("SB");
        outportb(base + 6, 1);
        delay(1);
        outportb(base + 6, 0);
        for (k = 0; k < 100 && !(inportb(base + 0xe) & 0x80); k++) {
            delay(1);
        }
        r = inportb(base + 0xa);
        if (r != 0xaa) {
            printf("SB %X: reset %02X, none\n", base, r);
            continue;
        }
        hi = lo = -1;
        if (dsp_write(base, 0xe1)) {
            hi = dsp_read(base);
            lo = dsp_read(base);
        }
        if (hi < 0 || lo < 0) {
            printf("SB %X: reset AA, no version", base);
        } else {
            printf("SB %X: reset AA, DSP version %d.%02d", base, hi, lo);
        }
        if (hi == 3 && dsp_write(base, 0xe7)) {
            e1 = dsp_read(base);
            e2 = dsp_read(base);
            if (e1 == 0x68 && (e2 & 0xf0) == 0x80) {
                printf(", ESS ID %02X %02X", e1, e2);
                if (dsp_write(base, 0xc0) && dsp_write(base, 0xb1)) {
                    printf(", B1h %02X", dsp_read(base));
                }
                if (dsp_write(base, 0xc0) && dsp_write(base, 0xb2)) {
                    printf(" B2h %02X", dsp_read(base));
                }
            }
        }
        puts("");
    }
}

void opl_w(unsigned base, unsigned char reg, unsigned char val)
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

void probe_opl(unsigned base)
{
    unsigned char s1, s2;
    int i;
    opl_w(base, 4, 0x60);
    opl_w(base, 4, 0x80);
    s1 = inportb(base) & 0xe0;
    opl_w(base, 2, 0xff);
    opl_w(base, 4, 0x21);
    for (i = 0; i < 2000; i++) {
        inportb(base);
    }
    delay(2);
    s2 = inportb(base) & 0xe0;
    opl_w(base, 4, 0x60);
    opl_w(base, 4, 0x80);
    printf("OPL %X: s1=%02X s2=%02X, %s\n", base, s1, s2, s1 == 0 && s2 == 0xc0 ? "found" : "none");
}

void probe_wss(void)
{
    static unsigned bases[4] = {0x530, 0x604, 0xe80, 0xf40};
    int i;
    unsigned char cfg, wid, idx, i12;
    for (i = 0; i < 4; i++) {
        unsigned b = bases[i];
        step("WSS");
        cfg = inportb(b);
        wid = inportb(b + 3);
        idx = inportb(b + 4);
        if (idx == 0xff) {
            printf("WSS %X: config %02X, ID %02X, codec index FF, none\n", b, cfg, wid);
            continue;
        }
        outportb(b + 4, (idx & 0xe0) | 12);
        i12 = inportb(b + 5);
        outportb(b + 4, idx);
        printf("WSS %X: config %02X, ID %02X, codec index %02X, I12 %02X\n", b, cfg, wid, idx, i12);
    }
}

void probe_pss(void)
{
    unsigned base, id;
    int i, any = 0;
    for (base = 0x220; base <= 0x250; base += 0x10) {
        id = inport(base + 4);
        if ((id >> 8) != 'E') {
            continue;
        }
        any = 1;
        printf("PSS %X: ID %04X (rev %d), status %04X, config", base, id, id & 0xff,
               inport(base + 2));
        for (i = 0; i < 5; i++) {
            printf(" %04X", inport(base + 0x10 + 2 * i));
        }
        puts(" (PSS WSS SB CD MIDI)");
    }
    if (!any) {
        puts("PSS: none at 220-250");
    }
}

/* /DEEP: the OPTi password probes UNISOUND's /NOPNP uses, reads only after each password. */
void probe_deep(void)
{
    unsigned p;
    unsigned char a, b, c;
    printf("F88-F8F without a password:");
    for (p = 0xf88; p <= 0xf8f; p++) {
        printf(" %02X", inportb(p));
    }
    puts("");
    step("OPTi 82C924/925");
    outportb(0xf8f, 0xe5);
    a = inportb(0xf8d);
    outportb(0xf8f, 0xe5);
    b = inportb(0xf8f);
    printf("82C924/925 (E5h to F8Fh): register 1 %02X, register 3 %02X\n", a, b);
    step("OPTi 82C928/929");
    for (c = 0xe2; c <= 0xe3; c++) {
        delay(30);
        outportb(0xf8f, c);
        a = inportb(0xf8d);
        b = inportb(0xf8d);
        delay(30);
        outportb(0xf8f, c);
        printf("82C92%c (%02Xh to F8Fh): F8D %02X, again without %02X, F8F %02X\n",
               c == 0xe2 ? '8' : '9', c, a, b, inportb(0xf8f));
    }
    step("OPTi 82C930");
    printf("82C930 (E4h to F8Fh, E0E/E0F):");
    for (c = 1; c <= 0x12; c++) {
        if (c == 7) {
            c = 0x12;
        }
        outportb(0xf8f, 0xe4);
        outportb(0xe0e, c);
        outportb(0xf8f, 0xe4);
        printf(" r%X %02X", c, inportb(0xe0f));
        delay(5);
    }
    puts("");
}

int main(int argc, char *argv[])
{
    int i;
    for (i = 1; i < argc; i++) {
        if (strnicmp(argv[i], "/SKIP:", 6) == 0) {
            add_list(skip, sizeof(skip), argv[i] + 6);
        } else if (strnicmp(argv[i], "/ONLY:", 6) == 0) {
            add_list(only, sizeof(only), argv[i] + 6);
        } else if (stricmp(argv[i], "/ESCD") == 0) {
            opt_escd = 1;
        } else if (stricmp(argv[i], "/ISOLATE") == 0) {
            opt_isolate = 1;
        } else if (stricmp(argv[i], "/DEEP") == 0) {
            opt_deep = 1;
        } else if (stricmp(argv[i], "/NOFILES") == 0) {
            opt_nofiles = 1;
        } else {
            puts("usage: UNIDUMP [/SKIP:A,B] [/ONLY:A,B] [/ESCD] [/ISOLATE] [/DEEP] [/NOFILES]"
                 " > DUMP.TXT\n"
                 "sections: ENV CPU MEM BIOS PCI DOS IRQ PNPBIOS PNP SB OPL MPU WSS PSS");
            return 1;
        }
    }
    printf("UNIDUMP 1 (UNISOUND test tool)");
    for (i = 1; i < argc; i++) {
        printf(" %s", argv[i]);
    }
    puts("");
    if (section("ENV", "Environment")) {
        report_env();
    }
    cpu = cpu_class();
    if (section("CPU", "Processor")) {
        report_cpu();
    }
    if (section("MEM", "Memory")) {
        report_mem();
    }
    if (section("BIOS", "BIOS and board")) {
        report_bios();
    }
    if (section("PCI", "PCI")) {
        report_pci();
    }
    if (section("DOS", "DOS, drivers and memory")) {
        report_dos();
    }
    if (section("IRQ", "IRQs and ports")) {
        report_irq();
    }
    if (section("PNPBIOS", "PnP BIOS")) {
        report_pnp_bios();
    } else if (want("PNP") && find_pnp_bios()) {
        /* the cards section needs the BIOS's read port and CSN count */
        unsigned char far *h = find_pnp_bios(), cfg[6];
        pnp_entry = (PNPFN)MK_FP(*(unsigned far *)(h + 0x0f), *(unsigned far *)(h + 0x0d));
        pnp_ds = *(unsigned far *)(h + 0x1b);
        if (pnp_entry(0x40, (unsigned char far *)cfg, pnp_ds) == 0) {
            rdport = cfg[2] | (cfg[3] << 8);
            pnp_bios_csns = cfg[1];
        }
    }
    if (section("PNP", "ISA PnP cards")) {
        report_isa_cards();
    }
    if (want("SB") || want("OPL") || want("MPU") || want("WSS") || want("PSS") || opt_deep) {
        step("non-PnP probes");
        puts("\n== Non-PnP probes");
        if (want("SB")) {
            probe_sb();
        }
        if (want("OPL")) {
            step("OPL");
            probe_opl(0x388);
        }
        if (want("MPU")) {
            step("MPU");
            probe_mpu(0x330);
            probe_mpu(0x300);
        }
        if (want("WSS")) {
            probe_wss();
        }
        if (want("PSS")) {
            step("PSS");
            probe_pss();
        }
        if (opt_deep) {
            probe_deep();
        }
    }
    fflush(stdout);
    fprintf(stderr, "UNIDUMP: done\n");
    return 0;
}
