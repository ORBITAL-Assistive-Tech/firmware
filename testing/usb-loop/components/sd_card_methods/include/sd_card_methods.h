#pragma once

#include "driver/gpio.h"
#include <stddef.h>
#include <stdint.h>

#define MOUNT_POINT "/sdcard"

constexpr gpio_num_t DATA_IN = GPIO_NUM_35;     // CMD
constexpr gpio_num_t SPI_CLK = GPIO_NUM_36;     // CLK
constexpr gpio_num_t DATA_OUT = GPIO_NUM_37;    // D0
constexpr gpio_num_t DATA_1 = GPIO_NUM_38;      // D1
constexpr gpio_num_t CS = GPIO_NUM_39;          // D3
constexpr gpio_num_t DATA_2 = GPIO_NUM_40;      // D2
constexpr gpio_num_t CARD_DETECT = GPIO_NUM_17; // DET

constexpr size_t MAX_DATA = 64;
constexpr size_t MAX_LINE_SIZE = 8;

esp_err_t sd_write_file(const char *path, uint8_t data[], size_t data_len);

esp_err_t sd_read_file(const char *path, uint8_t dst[], size_t dst_len);