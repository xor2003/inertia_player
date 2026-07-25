#include <stdio.h>
#include <string.h>

static unsigned char globals[0x2000];

void iplay_sub_13017_bounded(unsigned char *globals, unsigned char *samples, unsigned short sample_count);

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static void print_bytes(const unsigned char *p, unsigned n) {
    unsigned i;
    for (i = 0; i < n; ++i) printf("%02x", p[i]);
}

static void init_fixture(void) {
    unsigned char *samples;

    memset(globals, 0, sizeof(globals));
    samples = globals + 0x1d68;
    globals[0x0032] = 2;
    samples[0x24] = 0xaa;
    samples[0x25] = 0xaa;
    samples[0x26] = 0xaa;
    samples[0x27] = 0xaa;
    samples[0x2c] = 0x11;
    samples[0x2d] = 0x11;
    samples[0x2e] = 0x11;
    samples[0x2f] = 0x11;
    samples[0x3c] = 0;
    samples[0x40 + 0x24] = 0x22;
    samples[0x40 + 0x25] = 0x22;
    samples[0x40 + 0x26] = 0x22;
    samples[0x40 + 0x27] = 0x22;
    samples[0x40 + 0x2c] = 0x33;
    samples[0x40 + 0x2d] = 0x33;
    samples[0x40 + 0x2e] = 0x33;
    samples[0x40 + 0x2f] = 0x33;
    samples[0x40 + 0x3c] = 8;
}

static void fill_dma_prefill(void) {
    unsigned char *samples = globals + 0x1d68;

    iplay_sub_13017_bounded(globals, samples, 2);
}

void sub_13017(void);
#pragma aux sub_13017 __parm __caller [] __modify __exact [__ax __cx __di]
void sub_13017(void) {
    fill_dma_prefill();
}

void configure_timer(void);
#pragma aux configure_timer __parm __caller [] __modify __exact [__ax __cx __dx __di]
void configure_timer(void) {
    fill_dma_prefill();
    _asm {
        xor ax, ax
        mov dx, 09b0h
    }
}

int main(int argc, char **argv) {
    unsigned char *samples;

    if (argc != 3) return 2;
    if (!streq(argv[1], "abitimerbounded")) return 2;

    init_fixture();
    if (streq(argv[2], "sub_13017")) {
        _asm { call sub_13017 }
    } else if (streq(argv[2], "configure_timer")) {
        _asm { call configure_timer }
    } else {
        return 2;
    }

    samples = globals + 0x1d68;
    printf("data=");
    print_bytes(samples + 0x24, 4);
    print_bytes(samples + 0x40 + 0x24, 4);
    print_bytes(globals + 0x0060, 2);
    printf("\n");
    return 0;
}
