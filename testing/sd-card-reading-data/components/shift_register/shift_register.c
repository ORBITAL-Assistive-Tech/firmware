#include "shift_register.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include <stdio.h>

void configure(void) {
  gpio_reset_pin(SER);
  gpio_reset_pin(UE);
  gpio_reset_pin(RCLK);
  gpio_reset_pin(SRCLK);
  /* Set the GPIO as a push/pull output */
  // gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
  gpio_set_direction(SER, GPIO_MODE_OUTPUT);
  gpio_set_direction(UE, GPIO_MODE_OUTPUT);
  gpio_set_direction(RCLK, GPIO_MODE_OUTPUT);
  gpio_set_direction(SRCLK, GPIO_MODE_OUTPUT);
}

void push_to_shift_register(size_t data_size, uint8_t *data) {
  for (int i = MAX_BYTES / SIZEOF_BYTE - 1; i >= 0; i--) {
    for (int j = 0; j < MAX_BYTES / SIZEOF_BYTE; j++) {
      if (i < data_size) {
        SET_SER((data[i] >> j) & 1);
        ESP_LOGI("push_to_shift_register", "pushing to SER.");
        ESP_LOGI("push_to_shift_register", "%d", (data[i] >> j) & 1);
      } else
        SET_SER(LOW);
      SET_SRCLK(LOW);
      vTaskDelay(CLOCK_DELAY / portTICK_PERIOD_MS);
      SET_SRCLK(HIGH);
      vTaskDelay(CLOCK_DELAY / portTICK_PERIOD_MS);
    }
  }

  SET_RCLK(HIGH);
  vTaskDelay(CLOCK_DELAY / portTICK_PERIOD_MS);
  SET_RCLK(LOW);
}