#include <stdio.h>
#include <string.h>

static unsigned char globals[0x2000];

void iplay_snd_on_parnt_bounded(unsigned char *mem);

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static void print_bytes(const unsigned char *p, unsigned n) {
    unsigned i;
    for (i = 0; i < n; ++i) printf("%02x", p[i]);
}

void snd_on_parnt(void);
#pragma aux snd_on_parnt __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void snd_on_parnt(void) {
    iplay_snd_on_parnt_bounded(globals);
}

int main(int argc, char **argv) {
    if (argc != 2) return 2;
    if (!streq(argv[1], "abisndonparntbounded")) return 2;

    memset(globals, 0, sizeof(globals));
    globals[0x00c9] = 0x91;
    globals[0x00ca] = 0x92;
    globals[0x00cb] = 0x93;
    globals[0x00cc] = 0x94;
    globals[0x00cd] = 0x95;
    globals[0x00d1] = 0x96;
    globals[0x00df] = 0x97;
    globals[0x0060] = 0x11;
    globals[0x0061] = 0x11;
    globals[0x0062] = 0x22;
    globals[0x0063] = 0x22;
    globals[0x0080] = 0x33;
    globals[0x0081] = 0x44;
    globals[0x00d9] = 6;
    globals[0x00da] = 125;
    memset(globals + 0x1368, 0xa5, 0x20);

    _asm { call snd_on_parnt }

    printf("data=");
    print_bytes(globals + 0x00c9, 5);
    print_bytes(globals + 0x00d1, 1);
    print_bytes(globals + 0x00df, 1);
    print_bytes(globals + 0x0060, 4);
    print_bytes(globals + 0x0080, 2);
    print_bytes(globals + 0x00c8, 1);
    print_bytes(globals + 0x00db, 2);
    print_bytes(globals + 0x1368, 1);
    print_bytes(globals + 0x1387, 1);
    printf("\n");
    return 0;
}
