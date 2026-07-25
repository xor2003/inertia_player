#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char db;
typedef unsigned short dw;
typedef unsigned long dd;

static db channel_kind[32];
static db globals[0x1800];
static db channels[0x50 * 32];
static db byte_1de79;
static db byte_1de7a;
static db byte_1de81;
static dw x_storage[32];

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static void put_word(db *p, dw value) {
    p[0] = (db)value;
    p[1] = (db)(value >> 8);
}

void iplay_video_prp_mtr_positn(db *globals, const db *channels, dw count);

void video_prp_mtr_positn(void);
#pragma aux video_prp_mtr_positn __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void video_prp_mtr_positn(void) {
    unsigned i;
    unsigned count = 3;

    memset(globals, 0, sizeof(globals));
    memset(channels, 0, sizeof(channels));
    memset(x_storage, 0, sizeof(x_storage));
    for (i = 0; i < count; ++i) {
        channels[i * 0x50u + 0x3au] = channel_kind[i];
    }
    iplay_video_prp_mtr_positn(globals, channels, (dw)count);
    byte_1de79 = globals[0x1689];
    byte_1de7a = globals[0x168a];
    byte_1de81 = globals[0x1691];
    for (i = 0; i < count; ++i) {
        x_storage[i] = (dw)globals[0x16ac + i * 2u] |
                       ((dw)globals[0x16ad + i * 2u] << 8);
    }
}

int main(int argc, char **argv) {
    db out[9];

    if (argc != 5) return 2;
    if (!streq(argv[1], "abivideoprp")) return 2;

    channel_kind[0] = (db)strtoul(argv[2], 0, 16);
    channel_kind[1] = (db)strtoul(argv[3], 0, 16);
    channel_kind[2] = (db)strtoul(argv[4], 0, 16);
    video_prp_mtr_positn();

    out[0] = byte_1de79;
    out[1] = byte_1de7a;
    out[2] = byte_1de81;
    put_word(out + 3, x_storage[0]);
    put_word(out + 5, x_storage[1]);
    put_word(out + 7, x_storage[2]);

    printf("data=");
    for (argc = 0; argc < 9; ++argc) printf("%02x", (unsigned)out[argc]);
    printf("\n");
    return 0;
}
