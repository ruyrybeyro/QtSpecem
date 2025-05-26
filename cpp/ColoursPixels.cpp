#include <QImage>
#include <QColor>
#include <QRgb>
#include <memory>
#include <cstdint>

// Type definitions
#ifndef UCHAR
typedef unsigned char UCHAR;
#endif

// Constants
#define BORDER_HORIZONTAL 32
#define BORDER_VERTICAL 16
#define BORDER_THRESHOLD_LOW 4096
#define BORDER_THRESHOLD_HIGH 4096+(65535)

// External variable declarations
extern "C" UCHAR ULAplus;
extern std::unique_ptr<QImage> background;
extern uint8_t border_colors[192+(2*BORDER_VERTICAL)];
extern uint8_t border_color;
extern uint16_t border_ptr;

// Color arrays declarations
extern unsigned char rgbvals[256][3];
extern unsigned char rgbvalsULAplus[256][3];

extern "C" void init_pallete(void) {
    int i;

    if(!ULAplus) {
       for (i = 0; i < 16; i++) {
          background->setColor(i, qRgb(rgbvals[i][0], rgbvals[i][1], rgbvals[i][2]));
       }
    } else {
       for (i = 0; i < 256; i++) {
          background->setColor(i, qRgb(rgbvalsULAplus[i][0], rgbvalsULAplus[i][1], rgbvalsULAplus[i][2]));
       }
    }
}

static uint8_t colour3to8bit(uint8_t c) {
    return (c << 5) | (c << 2) | (c >> 1);
}

extern "C" void set_palette(UCHAR palettePos, UCHAR colour) {
    rgbvalsULAplus[palettePos][0] = colour3to8bit(( colour >> 2 ) & 7);
    rgbvalsULAplus[palettePos][1] = colour3to8bit(colour >> 5);
    if ((rgbvalsULAplus[palettePos][2] = (( colour & 0x3 ) << 1))) {
        rgbvalsULAplus[palettePos][2]++;
    }
    rgbvalsULAplus[palettePos][2] = colour3to8bit(rgbvalsULAplus[palettePos][2]);

    if(ULAplus) {
        background->setColor(palettePos, qRgb(rgbvalsULAplus[palettePos][0],
                                            rgbvalsULAplus[palettePos][1],
                                            rgbvalsULAplus[palettePos][2]));
    }
}

extern "C" void pixel_host(unsigned short x, unsigned short y, unsigned char colour) {
    background->setPixel(x + BORDER_HORIZONTAL, y + BORDER_VERTICAL, colour);
}

void complete_border() {
    while (border_ptr < (BORDER_VERTICAL*2) + 192) {
        border_colors[border_ptr++] = border_color;
    }
}

extern "C" void border_updated(uint8_t color, unsigned long ticks) {
    if (ticks < BORDER_THRESHOLD_LOW || ticks > BORDER_THRESHOLD_HIGH) {
        // Even if we do not draw anything, we need to save the color here.
        border_color = color;
        return;
    }

    // Compute border ptr
    unsigned new_border_ptr = ticks - BORDER_THRESHOLD_LOW;

    // Total number of screen lines including border
    const unsigned lines = sizeof(border_colors) / sizeof(border_colors[0]);

    const unsigned height = 192 + (BORDER_VERTICAL * 2);
    const unsigned delta = BORDER_THRESHOLD_HIGH - BORDER_THRESHOLD_LOW;

    // Remap the range LOW-HIGH into the image vertical area
    new_border_ptr *= height;
    new_border_ptr /= delta;

    // Do not let overflow.
    if (new_border_ptr > lines) {
        new_border_ptr = lines; // This is still 1-past the array size. No problem here.
    }

    while (border_ptr < new_border_ptr) {
        border_colors[border_ptr++] = border_color; // Old color
    }

    // If we already drawn the last line, skip, otherwise place the color there.
    if (border_ptr < lines) {
        border_colors[border_ptr++] = color; // new color
    }
    // Save color.
    border_color = color;
}

// Make sure resize_host is exported with C linkage
extern "C" void resize_host(unsigned x, unsigned y) {
    background = std::make_unique<QImage>(x + (2 * BORDER_HORIZONTAL),
                                       y + (2 * BORDER_VERTICAL),
                                       QImage::Format_Indexed8);
    init_pallete();
}

