#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include <stdio.h>

#define SER 19   // Data In
#define UE 20    // ENABLE
#define RCLK 21  // Main Clock - Toggle when all the bits have been pushed
#define SRCLK 47 // Second Clock - Always on

#define SET_SER(level) gpio_set_level(SER, level)
#define SET_SRCLK(level) gpio_set_level(SRCLK, level)
#define SET_RCLK(level) gpio_set_level(RCLK, level)

#define CLOCK_DELAY 0.5

#define HIGH 1
#define LOW 0

#define MAX_DATA 64

static void configure(void) {
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

void app_main(void) {
  configure();
  gpio_set_level(UE, LOW);

  char data[8] = {0b0,        0b11110000, 0b11110000, 0b11110000,
                  0b11110000, 0b11110000, 0b11110000, 0b11110000};

  while (1) {
    for (int i = 0; i < MAX_DATA / 8; i++) {
      for (int j = 0; j < MAX_DATA / 8; j++) {
        SET_SER((data[i] >> j) & 1);
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
}
