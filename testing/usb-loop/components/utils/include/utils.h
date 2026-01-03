#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <concepts>
#include <cstdint>

inline void sleep_ms(int millis) { vTaskDelay(pdMS_TO_TICKS(millis)); }

template <std::integral T> inline T braille_offset(const T index) {
  return index - 0x20;
}

extern const std::uint8_t ascii_braille_conversion[64];
