#pragma once

#include "driver/gpio.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <cstdint>

constexpr gpio_num_t SER = GPIO_NUM_14;   // data in
constexpr gpio_num_t OE = GPIO_NUM_45;    // output enable
constexpr gpio_num_t RCLK = GPIO_NUM_13;  // register clock - toggle on bit push
constexpr gpio_num_t SRCLK = GPIO_NUM_47; // shift register clock - always on

constexpr std::size_t MAX_BYTES = 64; // max amount of data available to show
constexpr std::size_t SIZEOF_BYTE = 8;

typedef enum {
  LOW = static_cast<std::uint32_t>(0),
  HIGH = static_cast<std::uint32_t>(1),
} level_t;

class ShiftRegister {
public:
  static inline void configure() {
    gpio_reset_pin(SER);
    gpio_reset_pin(OE);
    gpio_reset_pin(RCLK);
    gpio_reset_pin(SRCLK);

    gpio_set_direction(SER, GPIO_MODE_OUTPUT);
    gpio_set_direction(OE, GPIO_MODE_OUTPUT);
    gpio_set_direction(RCLK, GPIO_MODE_OUTPUT);
    gpio_set_direction(SRCLK, GPIO_MODE_OUTPUT);
  }

  static esp_err_t push(std::uint8_t data[], std::size_t data_size);

  static inline void turn_on() {
    gpio_set_level(OE, HIGH);
    is_on = true;
  }

  static inline void turn_off() {
    gpio_set_level(OE, LOW);
    is_on = false;
  }

private:
  static bool is_on;

  static inline void set_ser(level_t level) { gpio_set_level(SER, level); }

  static inline void set_srclk(level_t level) { gpio_set_level(SRCLK, level); }

  static inline void set_rclk(level_t level) { gpio_set_level(RCLK, level); }

  static inline void clock() {
    vTaskDelay(static_cast<TickType_t>(1) / portTICK_PERIOD_MS);
  }

  static inline void rclk() {
    set_rclk(HIGH);
    clock();
    set_rclk(LOW);
  }

  static inline void srclk() {
    set_srclk(LOW);
    clock();
    set_srclk(HIGH);
    clock();
  }
};
