#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned char out_data[152];
static unsigned char pattern_data[128];
static unsigned char title_data[20];
static unsigned char first_value;
static unsigned char second_value;

void iplay_mod_1021e(unsigned char *out, unsigned char first, unsigned char second,
                     const unsigned char *pattern, const unsigned char *title);

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

static void parse_hex(const char *hex, unsigned char *dst, unsigned max_count) {
    unsigned i;
    for (i = 0; i < max_count && hex[i * 2u] && hex[i * 2u + 1u]; ++i) {
        dst[i] = (unsigned char)((hex_value(hex[i * 2u]) << 4) | hex_value(hex[i * 2u + 1u]));
    }
}

static void print_bytes(const unsigned char *p, unsigned n) {
    unsigned i;
    for (i = 0; i < n; ++i) printf("%02x", p[i]);
}

void mod_1021E(void);
#pragma aux mod_1021E __parm __caller [] __modify __exact [__ax __cx __si __di __es]
void mod_1021E(void) {
    iplay_mod_1021e(out_data, first_value, second_value, pattern_data, title_data);
}

int main(int argc, char **argv) {
    char pattern_hex[300];
    char title_hex[128];
    const char *pattern_arg;
    const char *title_arg;

    if (argc != 6) return 2;
    if (!streq(argv[1], "abimod1021e")) return 2;

    first_value = (unsigned char)strtoul(argv[2], 0, 0);
    second_value = (unsigned char)strtoul(argv[3], 0, 0);
    memset(out_data, 0, sizeof(out_data));
    memset(pattern_data, 0, sizeof(pattern_data));
    memset(title_data, 0, sizeof(title_data));
    pattern_arg = argv[4];
    title_arg = argv[5];
    if (pattern_arg[0] == '@') {
        if (read_text_file_arg(pattern_arg, pattern_hex, sizeof(pattern_hex)) <= 0) return 2;
        pattern_arg = pattern_hex;
    }
    if (title_arg[0] == '@') {
        if (read_text_file_arg(title_arg, title_hex, sizeof(title_hex)) <= 0) return 2;
        title_arg = title_hex;
    }
    parse_hex(pattern_arg, pattern_data, sizeof(pattern_data));
    parse_hex(title_arg, title_data, sizeof(title_data));

    _asm { call mod_1021E }

    printf("data=");
    print_bytes(out_data, sizeof(out_data));
    printf("\n");
    return 0;
}
