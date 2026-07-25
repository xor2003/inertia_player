#include <stdio.h>
#include <string.h>

static unsigned char globals[0x200];
static unsigned short ret_ax;
static unsigned short ret_dx;

void iplay_sb_handler_int_bounded(void *r, unsigned char *globals);

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static void print_bytes(const unsigned char *p, unsigned n) {
    unsigned i;
    for (i = 0; i < n; ++i) printf("%02x", p[i]);
}

void sb_handler_int(void);
#pragma aux sb_handler_int __parm __caller [] __modify __exact []
void sb_handler_int(void) {
    _asm {
        mov ret_ax, ax
        mov ret_dx, dx
    }
    iplay_sb_handler_int_bounded(0, globals);
    _asm {
        mov ax, ret_ax
        mov dx, ret_dx
    }
}

int main(int argc, char **argv) {
    unsigned ax_after;
    unsigned dx_after;

    if (argc != 2) return 2;
    if (!streq(argv[1], "abisbhandlerintbounded")) return 2;

    memset(globals, 0, sizeof(globals));
    globals[0x006e] = 0x00;
    globals[0x006f] = 0x10;
    _asm {
        mov ax, 1234h
        mov dx, 022eh
        call sb_handler_int
        mov ax_after, ax
        mov dx_after, dx
    }

    printf("ax=%04x dx=%04x data=", ax_after, dx_after);
    print_bytes(globals + 0x006e, 2);
    printf("\n");
    return 0;
}
