#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/time.h>

#include "iplay_masm_.h"

db vgaPalette[256 * 3];
dd selectorsPointer;
dd selectors[NB_SELECTORS];
int maxy = 30;
dd heapPointer;
db vgaRamPaddingBefore[VGARAM_SIZE];
db vgaRam[VGARAM_SIZE];
db vgaRamPaddingAfter[VGARAM_SIZE];
db *diskTransferAddr = nullptr;
bool isLittle = true;
bool jumpToBackGround = false;
char *path = nullptr;
bool executionFinished = false;
db exitCode = 0;
FILE *logDebug = nullptr;
chtype vga_to_curses[256];
bool iplay_test_loadcfg_file = false;
int iplay_test_loadcfg_read_index = 0;

extern "C" {
void log_error(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
}

void log_debug(const char *fmt, ...) {
    (void)fmt;
}

void log_info(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stdout, fmt, ap);
    va_end(ap);
}

void log_debug2(const char *fmt, ...) {
    (void)fmt;
}

void hexDump(void *addr, int len) {
    (void)addr;
    (void)len;
}

void stackDump(_STATE *_state) {
    (void)_state;
}

void asm2C_OUT(int16_t address, int data) {
    (void)address;
    (void)data;
}

int8_t asm2C_IN(int16_t address) {
    (void)address;
    return 0;
}

uint16_t asm2C_INW(uint16_t address) {
    (void)address;
    return 0;
}

void asm2C_INT(_STATE *_state, int a) {
    static uint16_t interrupt_vector_offsets[256];
    static uint16_t interrupt_vector_segments[256];
    if (a == 0x1a) {
        uint8_t ah = (uint8_t)((_state->eax >> 8) & 0xff);
        if (ah == 0x02) {
            _state->ecx = (_state->ecx & 0xffff0000u) | 0x1234u;
            _state->edx = (_state->edx & 0xffff00ffu) | 0x5600u;
            _state->CF = false;
            return;
        }
    }
    if (a == 0x21) {
        uint8_t ah = (uint8_t)((_state->eax >> 8) & 0xff);
        uint8_t al = (uint8_t)(_state->eax & 0xff);
        if (ah == 0x25) {
            interrupt_vector_offsets[al] = (uint16_t)(_state->edx & 0xffff);
            interrupt_vector_segments[al] = _state->ds;
            return;
        }
        if (ah == 0x35) {
            _state->ebx = (_state->ebx & 0xffff0000u) | interrupt_vector_offsets[al];
            _state->es = interrupt_vector_segments[al];
            return;
        }
        if (ah == 0x19) {
            _state->eax = (_state->eax & 0xffffff00u) | 0x04u;
            return;
        }
        if (ah == 0x47) {
            db *dst = raddr(_state->ds, _state->esi & 0xffff);
            dst[0] = 0;
            _state->eax = (_state->eax & 0xffff0000u) | 0x0100u;
            return;
        }
        if (ah == 0x0e) {
            _state->eax &= 0xffffff00u;
            return;
        }
        if (ah == 0x3b) {
            return;
        }
        if (ah == 0x3d && iplay_test_loadcfg_file) {
            _state->eax = (_state->eax & 0xffff0000u) | 0x0042u;
            _state->CF = false;
            iplay_test_loadcfg_read_index = 0;
            return;
        }
        if (ah == 0x3f) {
            if ((_state->ebx & 0xffff) == 0xffffu) {
                _state->eax = (_state->eax & 0xffff0000u) | 0x0006u;
                _state->CF = true;
                return;
            }
            if (iplay_test_loadcfg_file && (_state->ebx & 0xffff) == 0x0042u) {
                static const db config_bytes[16] = {
                    0x49, 0x4e, 0x52, 0x10,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                };
                uint16_t count = (uint16_t)(_state->ecx & 0xffff);
                db *dst = raddr(_state->ds, _state->edx & 0xffff);
                for (uint16_t i = 0; i < count && iplay_test_loadcfg_read_index < 16; ++i) {
                    dst[i] = config_bytes[iplay_test_loadcfg_read_index++];
                }
                _state->eax = (_state->eax & 0xffff0000u) | count;
                _state->CF = false;
                return;
            }
            _state->eax &= 0xffff0000u;
            _state->CF = false;
            return;
        }
        if (ah == 0x3e && iplay_test_loadcfg_file) {
            _state->CF = false;
            return;
        }
        if (ah == 0x1a) {
            return;
        }
        if (ah == 0x4f) {
            _state->eax = (_state->eax & 0xffff0000u) | 0x0012u;
            _state->CF = true;
            return;
        }
        if (ah == 0x49) {
            _state->eax = (_state->eax & 0xffff0000u) | 0x0007u;
            _state->CF = true;
            return;
        }
    }
    (void)a;
}

double realElapsedTime(void) {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
}

void asm2C_init(void) {}

void mcb_init(uint16_t, uint16_t, uint8_t) {}
uint16_t mem_alloc(uint16_t) { return 0; }
uint16_t mem_realloc(uint16_t, uint16_t) { return 0; }
uint16_t mem_free(uint16_t) { return 0; }
uint16_t mem_get_alloc_strategy(void) { return 0; }
uint16_t mem_set_alloc_strategy(uint16_t) { return 0; }
}
