#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char db;
typedef unsigned short dw;

void iplay_sub_131ef(db *channel, db value, dw volume, db max_volume);

static db channel[0x42];
static db input_value;
static dw input_volume;
static db input_max_volume;

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

void sub_131EF(void);
#pragma aux sub_131EF __parm __caller [] __modify __exact []
void sub_131EF(void) {
    iplay_sub_131ef(channel, input_value, input_volume, input_max_volume);
}

int main(int argc, char **argv) {
    if (argc != 7) return 2;
    if (!streq(argv[1], "abisub131ef")) return 2;

    memset(channel, 0, sizeof(channel));
    input_value = (db)strtoul(argv[2], 0, 0);
    input_volume = (dw)strtoul(argv[3], 0, 0);
    input_max_volume = (db)strtoul(argv[4], 0, 0);
    channel[0x23] = (db)strtoul(argv[5], 0, 0);
    channel[0x3d] = (db)strtoul(argv[6], 0, 0);

    _asm { call sub_131EF }

    printf("data=%02x%02x%02x%02x%02x\n",
           (unsigned)channel[0x22],
           (unsigned)channel[0x23],
           (unsigned)channel[0x36],
           (unsigned)channel[0x37],
           (unsigned)channel[0x3d]);
    return 0;
}
