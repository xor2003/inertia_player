#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char db;
typedef unsigned short dw;
typedef unsigned long dd;

static db out_data[6 + 8 * 0x40];
static db headers[8 * 30];
static dw sample_count;

void iplay_mod_1024a(db *out, dw sample_count, const db *headers, dw freq);

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static unsigned hex_value(char ch) {
    if (ch >= '0' && ch <= '9') return (unsigned)(ch - '0');
    if (ch >= 'a' && ch <= 'f') return (unsigned)(ch - 'a' + 10);
    if (ch >= 'A' && ch <= 'F') return (unsigned)(ch - 'A' + 10);
    return 0;
}

static int read_text_file_arg(const char *arg, char *dst, unsigned size) {
    FILE *f;
    unsigned n;
    if (arg[0] != '@') return 0;
    f = fopen(arg + 1, "rb");
    if (!f) return -1;
    n = (unsigned)fread(dst, 1, size - 1u, f);
    fclose(f);
    dst[n] = 0;
    return (int)n;
}

static void parse_hex(const char *hex, db *dst, unsigned max_count) {
    unsigned i;
    for (i = 0; i < max_count && hex[i * 2u] && hex[i * 2u + 1u]; ++i) {
        dst[i] = (db)((hex_value(hex[i * 2u]) << 4) | hex_value(hex[i * 2u + 1u]));
    }
}

static void print_bytes(const db *p, unsigned n) {
    unsigned i;
    for (i = 0; i < n; ++i) printf("%02x", p[i]);
}

void mod_1024A(void);
#pragma aux mod_1024A __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __es]
void mod_1024A(void) {
    iplay_mod_1024a(out_data, sample_count, headers, 8363);
}

int main(int argc, char **argv) {
    char header_hex[520];
    const char *header_arg;

    if (argc != 4) return 2;
    if (!streq(argv[1], "abimod1024a")) return 2;

    sample_count = (dw)strtoul(argv[2], 0, 0);
    if (sample_count > 8u) return 2;
    memset(out_data, 0, sizeof(out_data));
    memset(headers, 0, sizeof(headers));
    header_arg = argv[3];
    if (header_arg[0] == '@') {
        if (read_text_file_arg(header_arg, header_hex, sizeof(header_hex)) <= 0) return 2;
        header_arg = header_hex;
    }
    parse_hex(header_arg, headers, (unsigned)sample_count * 30u);

    _asm { call mod_1024A }

    printf("data=");
    print_bytes(out_data, (unsigned)(6u + sample_count * 0x40u));
    printf("\n");
    return 0;
}
