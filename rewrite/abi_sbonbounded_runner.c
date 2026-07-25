#include <stdio.h>
#include <string.h>

static unsigned char globals[0x200];

void iplay_sb_on_bounded(unsigned char *globals, const char *symbol);

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static void print_bytes(const unsigned char *p, unsigned n) {
    unsigned i;
    for (i = 0; i < n; ++i) printf("%02x", p[i]);
}

static void set_started(void) {
    globals[0x006e] = 0x00;
    globals[0x006f] = 0x10;
    globals[0x00ce] = 0x01;
    globals[0x00cf] = 0x58;
}

void sb_on(void);
#pragma aux sb_on __parm __caller [] __modify __exact [__ax __cx __dx __si]
void sb_on(void) {
    iplay_sb_on_bounded(globals, "sb_on");
}

void sb16_on(void);
#pragma aux sb16_on __parm __caller [] __modify __exact [__ax __cx __dx __si]
void sb16_on(void) {
    iplay_sb_on_bounded(globals, "sb16_on");
}

int main(int argc, char **argv) {
    if (argc != 3) return 2;
    if (!streq(argv[1], "abisbonbounded")) return 2;

    memset(globals, 0, sizeof(globals));
    globals[0x00be] = 0x22;
    globals[0x00bf] = 0x56;
    globals[0x00b2] = 0x20;
    globals[0x00b3] = 0x02;
    globals[0x00b9] = 7;
    globals[0x00ba] = 0x55;
    globals[0x00b8] = 1;
    globals[0x0083] = streq(argv[2], "sb16_on") ? 1 : 0;
    globals[0x0084] = 8;

    if (streq(argv[2], "sb_on")) {
        _asm { call sb_on }
    } else if (streq(argv[2], "sb16_on")) {
        _asm { call sb16_on }
    } else {
        return 2;
    }

    printf("data=");
    print_bytes(globals + 0x006e, 2);
    print_bytes(globals + 0x00ce, 2);
    printf("\n");
    return 0;
}
