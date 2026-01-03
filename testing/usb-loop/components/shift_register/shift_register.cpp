#include "shift_register.h"

#include "esp_err.h"
#include "esp_log.h"

#include <cstdint>

static const char *tag = "shift_register";

bool ShiftRegister::is_on = false;

esp_err_t ShiftRegister::push(std::uint8_t data[], std::size_t data_size) {
  if (ShiftRegister::is_on) {
    for (int i = MAX_BYTES / SIZEOF_BYTE - 1; i >= 0; i--) {
      for (std::size_t j = 0; j < MAX_BYTES / SIZEOF_BYTE; j++) {
        if (i < data_size) {
          ShiftRegister::set_ser((((data[i] >> j) & 1) == 1) ? HIGH : LOW);
        } else {
          ShiftRegister::set_ser(LOW);
        }
        srclk();
      }
    }
    rclk();
    return ESP_OK;
  } else {
    ESP_LOGE(tag, "Configure your shift register before you use it!");
    return ESP_FAIL;
  }
}