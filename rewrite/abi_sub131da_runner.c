#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iplay_rewrite.h"

static db channel[0x40];

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

void sub_131DA(void);
#pragma aux sub_131DA __parm __caller [] __modify __exact []
void sub_131DA(void) {
    iplay_sub_131da(channel);
}

int main(int argc, char **argv) {
    if (argc != 5) return 2;
    if (!streq(argv[1], "abisub131da")) return 2;

    memset(channel, 0, sizeof(channel));
    channel[0x1d] = (db)strtoul(argv[2], 0, 0);
    channel[0x17] = (db)strtoul(argv[3], 0, 0);
    channel[0x35] = (db)strtoul(argv[4], 0, 0);

    _asm { call sub_131DA }

    printf("data=%02x%02x\n", (unsigned)channel[0x17], (unsigned)channel[0x35]);
    return 0;
}
