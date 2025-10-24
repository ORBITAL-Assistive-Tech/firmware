#pragma once

#include <stdint.h>

#define BRAILLE_OFFSET(index) index - 0x20

extern const uint8_t ascii_braille_conversion[64];
