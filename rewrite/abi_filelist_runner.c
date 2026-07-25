#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char db;
typedef unsigned short dw;
typedef unsigned long dd;

static db row[160];
static db entry_type;
static db flags_value;
static dw time_word;
static dw date_word;
static dd file_size;
static char entry_name[32];

void iplay_filelist_row(db *row, db entry_type, db flags, dw time_word, dw date_word, dd size, const char *name);

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static void print_bytes(const unsigned char *p, unsigned n) {
    unsigned i;
    for (i = 0; i < n; ++i) printf("%02x", p[i]);
}

static void put_row_cell(unsigned index, db ch, db attr) {
    row[index * 2u] = ch;
    row[index * 2u + 1u] = attr;
}

static unsigned put_row_text(unsigned index, const char *text, db attr) {
    while (*text) put_row_cell(index++, (db)*text++, attr);
    return index;
}

void filelist_198B8(void);
#pragma aux filelist_198B8 __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di __bp __es]
void filelist_198B8(void) {
    iplay_filelist_row(row, entry_type, flags_value, time_word, date_word, file_size, entry_name);
}

int main(int argc, char **argv) {
    if (argc != 8) return 2;
    if (!streq(argv[1], "abifilelist")) return 2;

    entry_type = (db)strtoul(argv[2], 0, 0);
    flags_value = (db)strtoul(argv[3], 0, 0);
    time_word = (dw)strtoul(argv[4], 0, 0);
    date_word = (dw)strtoul(argv[5], 0, 0);
    file_size = (dd)strtoul(argv[6], 0, 0);
    strncpy(entry_name, argv[7], sizeof(entry_name) - 1u);
    entry_name[sizeof(entry_name) - 1u] = 0;
    memset(row, 0xcc, sizeof(row));

    _asm { call filelist_198B8 }

    printf("data=");
    print_bytes(row, sizeof(row));
    printf("\n");
    return 0;
}
