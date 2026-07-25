#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned char sndcard_type;
static unsigned short snd_base_port;
static unsigned char irq_number;
static unsigned char dma_channel;
static unsigned char freq_code;
static unsigned char byte_246d8;
static unsigned char byte_246d9;
static unsigned short snd_output_frq;
static unsigned short freq2;
static unsigned short config_word;
static unsigned char sndflags;

typedef struct IplayRegs {
    unsigned long eax;
    unsigned long ebx;
    unsigned long ecx;
    unsigned long edx;
    unsigned long ebp;
    unsigned long esi;
    unsigned long edi;
} IplayRegs;

static unsigned short ret_ax;
static unsigned short ret_bx;
static unsigned short ret_cx;
static unsigned short ret_dx;
static unsigned short ret_bp;
static unsigned short ret_si;

void iplay_read_sndsettings(
    IplayRegs *r,
    unsigned char sndcard_type,
    unsigned short snd_base_port,
    unsigned char irq_number,
    unsigned char dma_channel,
    unsigned char freq_code,
    unsigned char byte_246d8,
    unsigned char byte_246d9,
    unsigned short snd_output_frq,
    unsigned short freq2,
    unsigned short config_word,
    unsigned char sndflags);

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

void __far read_sndsettings(void);
#pragma aux read_sndsettings __parm __caller [] __modify __exact [__ax __bx __cx __dx __bp __si]
void __far read_sndsettings(void) {
    IplayRegs r;

    r.eax = 0;
    r.ebx = 0;
    r.ecx = 0;
    r.edx = 0;
    r.ebp = 0;
    r.esi = 0;
    r.edi = 0;
    iplay_read_sndsettings(&r, sndcard_type, snd_base_port, irq_number,
                           dma_channel, freq_code, byte_246d8, byte_246d9,
                           snd_output_frq, freq2, config_word, sndflags);
    ret_ax = (unsigned short)r.eax;
    ret_bx = (unsigned short)r.ebx;
    ret_cx = (unsigned short)r.ecx;
    ret_dx = (unsigned short)r.edx;
    ret_bp = (unsigned short)r.ebp;
    ret_si = (unsigned short)r.esi;
    _asm {
        mov ax, ret_ax
        mov bx, ret_bx
        mov cx, ret_cx
        mov dx, ret_dx
        mov si, ret_si
    }
}

int main(int argc, char **argv) {
    unsigned ax_after;
    unsigned bx_after;
    unsigned cx_after;
    unsigned dx_after;
    unsigned bp_after;
    unsigned si_after;

    if (argc != 13) return 2;
    if (!streq(argv[1], "abireadsndsettings")) return 2;

    sndcard_type = (unsigned char)strtoul(argv[2], 0, 0);
    snd_base_port = (unsigned short)strtoul(argv[3], 0, 0);
    irq_number = (unsigned char)strtoul(argv[4], 0, 0);
    dma_channel = (unsigned char)strtoul(argv[5], 0, 0);
    freq_code = (unsigned char)strtoul(argv[6], 0, 0);
    byte_246d8 = (unsigned char)strtoul(argv[7], 0, 0);
    byte_246d9 = (unsigned char)strtoul(argv[8], 0, 0);
    snd_output_frq = (unsigned short)strtoul(argv[9], 0, 0);
    freq2 = (unsigned short)strtoul(argv[10], 0, 0);
    config_word = (unsigned short)strtoul(argv[11], 0, 0);
    sndflags = (unsigned char)strtoul(argv[12], 0, 0);

    _asm {
        call read_sndsettings
        mov ax_after, ax
        mov bx_after, bx
        mov cx_after, cx
        mov dx_after, dx
        mov si_after, si
    }
    bp_after = ret_bp;

    printf("ax=%04x bx=%04x cx=%04x dx=%04x bp=%04x si=%04x\n",
           ax_after, bx_after, cx_after, dx_after, bp_after, si_after);
    return 0;
}
