#pragma once

#include "driver/gpio.h"
#include <stddef.h>
#include <stdint.h>

constexpr gpio_num_t SER = GPIO_NUM_14;   // data in
constexpr gpio_num_t OE = GPIO_NUM_45;    // output enable
constexpr gpio_num_t RCLK = GPIO_NUM_13;  // register clock - toggle on bit push
constexpr gpio_num_t SRCLK = GPIO_NUM_47; // shift register clock - always on
constexpr gpio_num_t NONE = GPIO_NUM_NC;
constexpr uint8_t SD_CARD_BUS_WIDTH = 4;

constexpr size_t MAX_BYTES = 64; // max amount of data available to show
constexpr size_t SIZEOF_BYTE = 8;

typedef enum {
  LOW = 0,
  HIGH = 1,
} level_t;

inline void set_ser(level_t level) { gpio_set_level(SER, level); }

inline void set_srclk(level_t level) { gpio_set_level(SRCLK, level); }

inline void set_rclk(level_t level) { gpio_set_level(RCLK, level); }

inline void set_oe(level_t level) { gpio_set_level(OE, level); }

#define CLOCK_DELAY 1

inline void shift_register_configure() {
  gpio_reset_pin(SER);
  gpio_reset_pin(OE);
  gpio_reset_pin(RCLK);
  gpio_reset_pin(SRCLK);

  gpio_set_direction(SER, GPIO_MODE_OUTPUT);
  gpio_set_direction(OE, GPIO_MODE_OUTPUT);
  gpio_set_direction(RCLK, GPIO_MODE_OUTPUT);
  gpio_set_direction(SRCLK, GPIO_MODE_OUTPUT);
}

void push_to_shift_register(size_t data_size, uint8_t *data);