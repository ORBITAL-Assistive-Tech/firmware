#pragma once

#include <concepts>
#include <cstdint>

template <std::integral T> inline T braille_offset(const T index) {
  return index - 0x20;
}

extern const std::uint8_t ascii_braille_conversion[64];
