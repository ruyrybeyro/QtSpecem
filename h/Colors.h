#ifndef COLORS_H
#define COLORS_H

#include <cstdint>
#include <memory>
#include <QImage>

#define BORDER_HORIZONTAL 32
#define BORDER_VERTICAL 16

// External global variables that need to be shared
extern uint8_t border_colors[192+(2*BORDER_VERTICAL)];
extern uint8_t border_color;
extern uint16_t border_ptr;

// Color definitions
extern unsigned char rgbvals[256][3];

// Functions that need to be accessible from multiple files
void complete_border();

#endif // COLORS_H

