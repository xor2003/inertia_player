#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iplay_rewrite.h"

static db orders[128];
static dw word_52;

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static int hexval(int ch) {
    if (ch >= '0' && ch <= '9') return ch - '0';
    if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
    if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
    return -1;
}

static void parse_hex_bytes(const char *hex, db *out, size_t max_count) {
    size_t n = 0;
    while (*hex != 0 && n < max_count) {
        int hi;
        int lo;
        while (*hex == ' ' || *hex == '\n' || *hex == '\r' || *hex == '\t') ++hex;
        if (*hex == 0) break;
        hi = hexval((unsigned char)hex[0]);
        lo = hexval((unsigned char)hex[1]);
        if (hi < 0 || lo < 0) break;
        out[n++] = (db)((hi << 4) | lo);
        hex += 2;
    }
}

static int read_text_file_arg(const char *arg, char *buf, size_t buf_size) {
    FILE *fp;
    size_t n;
    if (arg[0] != '@' || buf_size == 0) return 0;
    fp = fopen(arg + 1, "r");
    if (fp == 0) return 0;
    n = fread(buf, 1, buf_size - 1u, fp);
    fclose(fp);
    buf[n] = 0;
    return 1;
}

void mod_102F5(void);
#pragma aux mod_102F5 __parm __caller [] __modify __exact [__ax]
void mod_102F5(void) {
    word_52 = iplay_mod_102f5(orders);
    _asm {
        mov ax, word_52
    }
}

int main(int argc, char **argv) {
    char hexbuf[300];
    const char *hex;
    unsigned short ax_after;

    if (argc != 3) return 2;
    if (!streq(argv[1], "abimod102f5")) return 2;

    hex = argv[2];
    if (hex[0] == '@') {
        if (!read_text_file_arg(hex, hexbuf, sizeof(hexbuf))) return 2;
        hex = hexbuf;
    }

    memset(orders, 0, sizeof(orders));
    parse_hex_bytes(hex, orders, sizeof(orders));

    _asm {
        call mod_102F5
        mov ax_after, ax
    }
    word_52 = ax_after;

    printf("data=%02x%02x\n", (unsigned)(word_52 & 0xffu), (unsigned)(word_52 >> 8));
    return 0;
}
