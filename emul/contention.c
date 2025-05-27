#include <stdint.h>
#include "env.h"

#define FRAME_TSTATES 69888
#define LINE_TSTATES 224
#define FIRST_DISPLAY_LINE 64
#define LAST_DISPLAY_LINE 255
#define DISPLAY_START_TSTATE (FIRST_DISPLAY_LINE * LINE_TSTATES)
#define DISPLAY_END_TSTATE ((LAST_DISPLAY_LINE + 1) * LINE_TSTATES)

// Contention table for the first 128 T-states of a contended line
static const uint8_t contention_table[128] = {
    6,5,4,3,2,1,0,0, 6,5,4,3,2,1,0,0,
    6,5,4,3,2,1,0,0, 6,5,4,3,2,1,0,0,
    6,5,4,3,2,1,0,0, 6,5,4,3,2,1,0,0,
    6,5,4,3,2,1,0,0, 6,5,4,3,2,1,0,0,
    6,5,4,3,2,1,0,0, 6,5,4,3,2,1,0,0,
    6,5,4,3,2,1,0,0, 6,5,4,3,2,1,0,0,
    6,5,4,3,2,1,0,0, 6,5,4,3,2,1,0,0,
    6,5,4,3,2,1,0,0, 6,5,4,3,2,1,0,0
};

int is_contended_address(uint16_t addr) {
    return (addr >= 0x4000 && addr <= 0x7FFF);
}

void contention_delay(uint32_t tstate, uint16_t address) {
    if (!is_contended_address(address)) {
        return; // No contention for uncontended RAM
    }

    if (tstate < DISPLAY_START_TSTATE || tstate >= DISPLAY_END_TSTATE) {
        return; // Outside active display area
    }

    uint32_t line_tstate = (tstate - DISPLAY_START_TSTATE) % LINE_TSTATES;

    if (line_tstate < 128) {
        T(contention_table[line_tstate]);
    } else {
        return; // No contention in border part of line
    }
}

