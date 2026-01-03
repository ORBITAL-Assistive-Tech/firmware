#pragma once

#include "driver/gpio.h"
#include "driver/sdmmc_default_configs.h"
#include "driver/sdmmc_host.h"
#include "esp_err.h"
#include "esp_vfs_fat.h"
#include <cstdint>
#include <string>

#define MOUNT_POINT "/sdcard"

constexpr gpio_num_t DATA_IN = GPIO_NUM_35;     // CMD
constexpr gpio_num_t SPI_CLK = GPIO_NUM_36;     // CLK
constexpr gpio_num_t DATA_OUT = GPIO_NUM_37;    // D0
constexpr gpio_num_t DATA_1 = GPIO_NUM_38;      // D1
constexpr gpio_num_t CS = GPIO_NUM_39;          // D3
constexpr gpio_num_t DATA_2 = GPIO_NUM_40;      // D2
constexpr gpio_num_t CARD_DETECT = GPIO_NUM_17; // DET
constexpr gpio_num_t NONE = GPIO_NUM_NC;
constexpr std::uint8_t SD_CARD_BUS_WIDTH = 4;

constexpr std::size_t MAX_DATA = 64;
constexpr std::size_t MAX_LINE_SIZE = 8;

class SDInfo {
public:
  static constexpr const char *mount_point = "/sdcard";

  esp_vfs_fat_sdmmc_mount_config_t mount_config;

  sdmmc_host_t host;

  sdmmc_slot_config_t slot_config;

  SDInfo()
      : mount_config{.format_if_mount_failed = false,
                     .max_files = 5,
                     .allocation_unit_size = 16 * 1024,
                     .disk_status_check_enable = false,
                     .use_one_fat = false},
        host SDMMC_HOST_DEFAULT(), slot_config{
                                       .clk = SPI_CLK,
                                       .cmd = DATA_IN,
                                       .d0 = DATA_OUT,
                                       .d1 = DATA_1,
                                       .d2 = DATA_2,
                                       .d3 = CS,
                                       .d4 = NONE,
                                       .d5 = NONE,
                                       .d6 = NONE,
                                       .d7 = NONE,
                                       .cd = /* CARD_DETECT */ SDMMC_SLOT_NO_CD,
                                       .wp = SDMMC_SLOT_NO_WP,
                                       .width = SD_CARD_BUS_WIDTH,
                                       .flags = 0,
                                   } {};
};

class SD {
public:
  static sdmmc_card_t *card;

  static const SDInfo sd_info;

  static bool is_mounted;

  static esp_err_t write_file(const std::string &path, std::uint8_t data[],
                              std::size_t data_len);

  static esp_err_t read_file(const std::string &path, std::uint8_t dst[],
                             std::size_t dst_len);

  static esp_err_t mount();

  static void unmount();

  static void print_info();

  static inline std::string to_path(const char *file_path) {
    return std::string(SDInfo::mount_point).append(file_path);
  }
};
