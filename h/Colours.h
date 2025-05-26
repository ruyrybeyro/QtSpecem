#ifndef COLOURS_H
#define COLOURS_H

#include <cstdint>
#include <memory>

// Forward declaration
class QImage;

#define BORDER_HORIZONTAL 32
#define BORDER_VERTICAL 16

#define BORDER_THRESHOLD_LOW 4096              /* Border scan start in T-states */
#define BORDER_THRESHOLD_HIGH 4096+(65535)     /* Border scan end in T-states */

// Type definitions
#ifndef UCHAR
typedef unsigned char UCHAR;
#endif

// External global variables that need to be shared
extern uint8_t border_colors[192+(2*BORDER_VERTICAL)];
extern uint8_t border_color;
extern uint16_t border_ptr;

// RGB 'Spectrum' colours
// 16 colours enough for normal palette
// 64 colours for ULAplus
extern unsigned char rgbvals[256][3];
extern unsigned char rgbvalsULAplus[256][3];

// External C variables
extern "C" {
    extern UCHAR ULAplus;
}

// Function declarations
extern "C" {
    void init_pallete(void);
    void set_palette(UCHAR palettePos, UCHAR colour);
    void pixel_host(unsigned short x, unsigned short y, unsigned char colour);
    void border_updated(uint8_t color, unsigned long ticks);
    void resize_host(unsigned x, unsigned y);
}

// C++ functions
void complete_border();

#endif // COLOURS_H

