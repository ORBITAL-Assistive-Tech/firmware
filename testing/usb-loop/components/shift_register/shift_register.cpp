#include "shift_register.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

static const char *tag = "shift_register";

void push_to_shift_register(size_t data_size, uint8_t *data) {
  for (int i = MAX_BYTES / SIZEOF_BYTE - 1; i >= 0; i--) {
    for (int j = 0; j < MAX_BYTES / SIZEOF_BYTE; j++) {
      if (i < data_size) {
        set_ser((((data[i] >> j) & 1) == 1) ? HIGH : LOW);
        ESP_LOGI(tag, "pushing to SER. %d\n", (data[i] >> j) & 1);
      } else {
        set_ser(LOW);
      }

      set_srclk(LOW);
      vTaskDelay(CLOCK_DELAY / portTICK_PERIOD_MS);
      set_srclk(HIGH);
      vTaskDelay(CLOCK_DELAY / portTICK_PERIOD_MS);
    }
  }

  set_rclk(HIGH);
  vTaskDelay(CLOCK_DELAY / portTICK_PERIOD_MS);
  set_rclk(LOW);
}