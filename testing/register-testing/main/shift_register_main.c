/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led_strip.h"
#include "sdkconfig.h"
#include <stdio.h>

static const char *TAG = "example";

/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to
   blink, or you can edit the following line and set a number here.
*/
#define BLINK_GPIO CONFIG_BLINK_GPIO
#define SER 26   // Data In
#define UE 27    // ENABLE
#define RCLK 14  // Main Clock - Toggle when all the bits have been pushed
#define SRCLK 12 // Second Clock - Always on

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
  gpio_set_level(UE, HIGH);

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
