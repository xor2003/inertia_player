#include "iplay_rewrite.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    db commands[7];
    db events[27];
    size_t count;
    size_t i;

    if (argc == 8 && strcmp(argv[1], "dma16") == 0) {
        count = iplay_sb16_dma_channel5_events(
            events,
            sizeof(events),
            (dw)strtoul(argv[2], NULL, 0),
            (dw)strtoul(argv[3], NULL, 0),
            (dd)strtoul(argv[4], NULL, 0),
            (dw)strtoul(argv[5], NULL, 0),
            (db)strtoul(argv[6], NULL, 0),
            (dw)strtoul(argv[7], NULL, 0));
        printf("count=%u data=", (unsigned)count);
        for (i = 0; i < count * 3u; ++i) printf("%02x", (unsigned)events[i]);
        printf("\n");
        return 0;
    }
    if (argc != 5) return 2;
    count = iplay_sb16_start_commands(
        commands,
        sizeof(commands),
        (dw)strtoul(argv[1], NULL, 0),
        (db)strtoul(argv[2], NULL, 0),
        (db)strtoul(argv[3], NULL, 0),
        (dw)strtoul(argv[4], NULL, 0));
    printf("count=%u data=", (unsigned)count);
    for (i = 0; i < count; ++i) printf("%02x", (unsigned)commands[i]);
    printf("\n");
    return 0;
}
