#include <stdio.h>
#include <string.h>

static unsigned char state[25];
static unsigned short ret_ax;

unsigned char iplay_audio_init_failure(unsigned char *globals, unsigned char *text, const char *symbol);

static const unsigned char covox_text[16] = {
    0x08,0xf1,0x8e,0xda,0xa0,0x00,0xf0,0xba,0x03,0x00,0xee,0xb0,0x20,0xe6,0x20,0x1f
};
static const unsigned char stereo_text[16] = {
    0x08,0xf1,0x8e,0xda,0xba,0x05,0x00,0xb0,0x02,0xee,0x80,0xea,0x02,0xa1,0x00,0xf0
};
static const unsigned char pc_text[16] = {
    0x08,0xf1,0x8e,0xdb,0x32,0xff,0x8a,0x1e,0x00,0xf0,0x2e,0x8a,0x9f,0xe8,0x51,0x8a
};
static const unsigned char adlib_text[16] = {
    0xb8,0x08,0xf1,0x8e,0xd8,0xa0,0x00,0xf0,0xbb,0x6a,0x15,0x8e,0xdb,0xbb,0xf8,0x02
};

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static void print_bytes(const unsigned char *p, unsigned n) {
    unsigned i;
    for (i = 0; i < n; ++i) printf("%02x", p[i]);
}

static void init_state(const unsigned char *text, unsigned char sndflags, unsigned char is_stereo) {
    memcpy(state, text, 16);
    state[16] = sndflags;
    state[17] = is_stereo;
    state[18] = 8;
    state[19] = 0x78;
    state[20] = 0x03;
    state[21] = 0;
    state[22] = 0;
    state[23] = 0;
    state[24] = 0;
}

static void call_audio_init_failure(const char *symbol) {
    unsigned char globals[0x140];
    unsigned char text[16];

    memset(globals, 0, sizeof(globals));
    memset(text, 0, sizeof(text));
    ret_ax = 0x156au;
    (void)iplay_audio_init_failure(globals, text, symbol);
    memcpy(state, text, 16);
    state[16] = globals[0x0082];
    state[17] = globals[0x0083];
    state[18] = globals[0x0084];
    state[19] = globals[0x0132];
    state[20] = globals[0x0133];
    state[21] = globals[0x0018];
    state[22] = globals[0x0019];
    state[23] = globals[0x001a];
    state[24] = globals[0x001b];
}

void covox_init(void);
#pragma aux covox_init __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void covox_init(void) {
    call_audio_init_failure("covox_init");
    _asm { mov ax, ret_ax }
}

void stereo_init(void);
#pragma aux stereo_init __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void stereo_init(void) {
    call_audio_init_failure("stereo_init");
    _asm { mov ax, ret_ax }
}

void pcspeaker_init(void);
#pragma aux pcspeaker_init __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void pcspeaker_init(void) {
    call_audio_init_failure("pcspeaker_init");
    _asm { mov ax, ret_ax }
}

void adlib_init(void);
#pragma aux adlib_init __parm __caller [] __modify __exact [__ax __bx __cx __dx __si __di]
void adlib_init(void) {
    call_audio_init_failure("adlib_init");
    _asm { mov ax, ret_ax }
}

int main(int argc, char **argv) {
    unsigned ax_after;

    if (argc != 3) return 2;
    if (!streq(argv[1], "abiaudioinitfail")) return 2;

    memset(state, 0, sizeof(state));
    if (streq(argv[2], "covox_init")) {
        _asm {
            call covox_init
            mov ax_after, ax
        }
    } else if (streq(argv[2], "stereo_init")) {
        _asm {
            call stereo_init
            mov ax_after, ax
        }
    } else if (streq(argv[2], "pcspeaker_init")) {
        _asm {
            call pcspeaker_init
            mov ax_after, ax
        }
    } else if (streq(argv[2], "adlib_init")) {
        _asm {
            call adlib_init
            mov ax_after, ax
        }
    } else {
        return 2;
    }

    printf("ax=%04x data=", ax_after);
    print_bytes(state, sizeof(state));
    printf("\n");
    return 0;
}
