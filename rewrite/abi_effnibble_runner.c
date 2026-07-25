#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char db;
typedef unsigned short dw;

void iplay_eff_13bc0(db *channel, db value);
void iplay_eff_13c34(db *channel, db value);

static db channel[0x10];
static db input_value;
static dw ret_ax;

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

void eff_13BC0(void);
#pragma aux eff_13BC0 __parm __caller [] __modify __exact [__ax]
void eff_13BC0(void) {
    iplay_eff_13bc0(channel, input_value);
    ret_ax = input_value;
    _asm {
        mov ax, ret_ax
    }
}

void eff_13C34(void);
#pragma aux eff_13C34 __parm __caller [] __modify __exact [__ax]
void eff_13C34(void) {
    iplay_eff_13c34(channel, input_value);
    ret_ax = (dw)((input_value & 0x0fu) << 4);
    _asm {
        mov ax, ret_ax
    }
}

int main(int argc, char **argv) {
    unsigned ax_after;

    if (argc != 4) return 2;
    if (!streq(argv[1], "abieff13bc0") && !streq(argv[1], "abieff13c34")) return 2;

    memset(channel, 0, sizeof(channel));
    channel[0x09] = (db)strtoul(argv[2], 0, 0);
    input_value = (db)strtoul(argv[3], 0, 0);

    if (streq(argv[1], "abieff13bc0")) {
        _asm {
            xor ax, ax
            mov al, input_value
            call eff_13BC0
            mov ax_after, ax
        }
    } else {
        _asm {
            xor ax, ax
            mov al, input_value
            call eff_13C34
            mov ax_after, ax
        }
    }

    printf("ax=%04x data=%02x\n", ax_after, (unsigned)channel[0x09]);
    return 0;
}
