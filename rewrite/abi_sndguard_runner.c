#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned char globals[0x200];
static unsigned short snd_ax_in;
static unsigned short ret_ax;

typedef struct IplayRegs {
    unsigned long eax;
    unsigned long ebx;
    unsigned long ecx;
    unsigned long edx;
    unsigned long ebp;
    unsigned long esi;
    unsigned long edi;
} IplayRegs;

void iplay_snd_guard(IplayRegs *r, unsigned char *globals, unsigned op);

static void call_snd_guard(unsigned op) {
    IplayRegs r;

    r.eax = snd_ax_in;
    r.ebx = 0;
    r.ecx = 0;
    r.edx = 0;
    r.ebp = 0;
    r.esi = 0;
    r.edi = 0;
    iplay_snd_guard(&r, globals, op);
    ret_ax = snd_ax_in;
    _asm {
        mov ax, ret_ax
    }
}

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static void print_bytes(const unsigned char *p, unsigned n) {
    unsigned i;
    for (i = 0; i < n; ++i) printf("%02x", p[i]);
}

void snd_initialze(void);
#pragma aux snd_initialze __parm __caller [] __modify __exact [__bx]
void snd_initialze(void) {
    _asm {
        mov snd_ax_in, ax
    }
    call_snd_guard(0);
}

void snd_on(void);
#pragma aux snd_on __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void snd_on(void) {
    _asm {
        mov snd_ax_in, ax
    }
    call_snd_guard(1);
}

void snd_off(void);
#pragma aux snd_off __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void snd_off(void) {
    _asm {
        mov snd_ax_in, ax
    }
    call_snd_guard(2);
}

void snd_deinit(void);
#pragma aux snd_deinit __parm __caller [] __modify __exact [__bx]
void snd_deinit(void) {
    _asm {
        mov snd_ax_in, ax
    }
    call_snd_guard(3);
}

void __far snd_offx(void);
#pragma aux snd_offx __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void __far snd_offx(void) {
    _asm {
        mov snd_ax_in, ax
    }
    call_snd_guard(4);
}

int main(int argc, char **argv) {
    unsigned ax_after;

    if (argc != 6) return 2;
    if (!streq(argv[1], "abisndguard")) return 2;

    memset(globals, 0, sizeof(globals));
    globals[0x00e0] = (unsigned char)strtoul(argv[3], 0, 0);
    globals[0x00e1] = (unsigned char)strtoul(argv[4], 0, 0);
    globals[0x010c] = (unsigned char)strtoul(argv[5], 0, 0);

    if (streq(argv[2], "snd_initialze")) {
        _asm {
            mov ax, 156ah
            call snd_initialze
            mov ax_after, ax
        }
    } else if (streq(argv[2], "snd_on")) {
        _asm {
            mov ax, 156ah
            call snd_on
            mov ax_after, ax
        }
    } else if (streq(argv[2], "snd_off")) {
        _asm {
            mov ax, 156ah
            call snd_off
            mov ax_after, ax
        }
    } else if (streq(argv[2], "snd_deinit")) {
        _asm {
            mov ax, 156ah
            call snd_deinit
            mov ax_after, ax
        }
    } else if (streq(argv[2], "snd_offx")) {
        _asm {
            mov ax, 156ah
            call snd_offx
            mov ax_after, ax
        }
    } else {
        return 2;
    }

    printf("ax=%04x data=", ax_after);
    print_bytes(globals + 0x00e0, 3);
    printf("\n");
    return 0;
}
