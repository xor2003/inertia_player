#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char db;
typedef unsigned short dw;
typedef unsigned long dd;

void iplay_sub_13177(db *channel, dw period, dd dword_245bc, dd dword_245c0, db shift);

static db channel[0x42];
static dw input_period;
static dd input_dword_245bc;
static dd input_dword_245c0;
static db input_shift;

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

void sub_13177(void);
#pragma aux sub_13177 __parm __caller [] __modify __exact []
void sub_13177(void) {
    iplay_sub_13177(channel, input_period, input_dword_245bc, input_dword_245c0, input_shift);
}

int main(int argc, char **argv) {
    if (argc != 7) return 2;
    if (!streq(argv[1], "abisub13177")) return 2;

    memset(channel, 0, sizeof(channel));
    input_period = (dw)strtoul(argv[2], 0, 0);
    input_dword_245bc = strtoul(argv[3], 0, 0);
    input_dword_245c0 = strtoul(argv[4], 0, 0);
    input_shift = (db)strtoul(argv[5], 0, 0);
    channel[0x3d] = (db)strtoul(argv[6], 0, 0);

    _asm { call sub_13177 }

    printf("data=%02x%02x%02x%02x%02x%02x%02x\n",
           (unsigned)channel[0x1e],
           (unsigned)channel[0x1f],
           (unsigned)channel[0x20],
           (unsigned)channel[0x21],
           (unsigned)channel[0x3d],
           (unsigned)channel[0x3e],
           (unsigned)channel[0x3f]);
    return 0;
}
