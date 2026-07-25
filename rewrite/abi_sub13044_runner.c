#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char db;
typedef unsigned short dw;

static db globals[0x200];
static db vlm_table[32];

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static void print_bytes(const db *p, unsigned n) {
    unsigned i;
    for (i = 0; i < n; ++i) printf("%02x", (unsigned)p[i]);
}

static void put_word(unsigned off, dw value) {
    globals[off] = (db)value;
    globals[off + 1u] = (db)(value >> 8);
}

void iplay_sub_13044(db *globals, db *vlm_table);

void sub_13044(void);
#pragma aux sub_13044 __parm __caller [] __modify __exact [__ax __bx __cx __dx __bp __si __di]
void sub_13044(void) {
    iplay_sub_13044(globals, vlm_table);
}

int main(int argc, char **argv) {
    if (argc != 6) return 2;
    if (!streq(argv[1], "abisub13044")) return 2;

    memset(globals, 0, sizeof(globals));
    memset(vlm_table, 0, sizeof(vlm_table));
    globals[0x00de] = (db)strtoul(argv[2], 0, 0);
    put_word(0x0036, (dw)strtoul(argv[3], 0, 0));
    put_word(0x005e, (dw)strtoul(argv[4], 0, 0));
    globals[0x0085] = (db)strtoul(argv[5], 0, 0);

    _asm {
        call sub_13044
    }

    printf("data=");
    print_bytes(globals + 0x008e, 2);
    print_bytes(globals + 0x00b6, 2);
    print_bytes(globals + 0x00dd, 2);
    print_bytes(vlm_table, 32);
    printf("\n");
    return 0;
}
