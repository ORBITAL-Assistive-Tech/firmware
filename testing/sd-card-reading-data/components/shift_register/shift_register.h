#pragma once

#include "driver/gpio.h"
#include "sdkconfig.h"
#include <stddef.h>
#include <stdint.h>

#define SER 19   // Data In
#define UE 20    // ENABLE
#define RCLK 21  // Main Clock - Toggle when all the bits have been pushed
#define SRCLK 47 // Second Clock - Always on

#define MAX_BYTES 64  // max amount of bits that the shift register can hold
#define SIZEOF_BYTE 8 // bits in a byte

#define SET_SER(level) gpio_set_level(SER, level)
#define SET_SRCLK(level) gpio_set_level(SRCLK, level)
#define SET_RCLK(level) gpio_set_level(RCLK, level)
#define SET_UE(level) gpio_set_level(UE, level)

#define CLOCK_DELAY 1

#define HIGH 1
#define LOW 0

void configure(void);
void push_to_shift_register(size_t data_size, uint8_t *data);