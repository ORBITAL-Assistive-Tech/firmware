#include "sd_card_methods.h"
#include "driver/sdmmc_default_configs.h"
#include "driver/sdmmc_host.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "utils.h"
#include <cstdint>
#include <fstream>
#include <stdio.h>
#include <string>

static const char *tag = "sd_card_methods";

sdmmc_card_t *SD::card = nullptr;
const SDInfo SD::sd_info = SDInfo();

esp_err_t SD::write_file(const std::string &path, std::uint8_t data[],
                         std::size_t data_len) {
  ESP_LOGI(tag, "Opening file %s", path.c_str());
  std::ofstream file;
  file.open(path);
  if (!file.is_open()) {
    ESP_LOGE(tag, "Failed to open file for writing.");
    return ESP_FAIL;
  }

  for (std::size_t i = 0; i < data_len; i++) {
    const std::uint8_t character =
        ascii_braille_conversion[(braille_offset<std::uint8_t>(data[i]))];
    file << static_cast<char>(character);
  }
  file << "\n";
  ESP_LOGI(tag, "File written");

  return ESP_OK;
}

esp_err_t SD::read_file(const std::string &path, std::uint8_t dst[],
                        std::size_t dst_len) {
  ESP_LOGI(tag, "Reading file %s", path.c_str());
  std::ifstream file;
  file.open(path);
  if (!file.is_open()) {
    ESP_LOGE(tag, "Failed to open file for reading");
    return ESP_FAIL;
  }
  std::string line;
  line.reserve(MAX_LINE_SIZE);
  if (!std::getline(file, line)) {
    ESP_LOGE(tag, "Data collection from stream failed! Check the file!");
    return ESP_FAIL;
  }

  ESP_LOGI(tag, "Read from file: '%s'", line.c_str());

  for (std::size_t i = 0; i < line.size(); i++) {
    dst[i] = ascii_braille_conversion[braille_offset<std::uint8_t>(
                 static_cast<std::uint8_t>(line[i]))]
             << 2;
  }

  return ESP_OK;
}

esp_err_t SD::mount() {

  esp_err_t return_value = esp_vfs_fat_sdmmc_mount(
      sd_info.mount_point, &sd_info.host, &sd_info.slot_config,
      &sd_info.mount_config, &card);

  if (return_value != ESP_OK) {
    if (return_value == ESP_FAIL) {
      ESP_LOGE(tag, "Failed to mount filesystem. "
                    "If you want the card to be formatted, set the "
                    "EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
    } else {
      ESP_LOGE(tag,
               "Failed to initialize the card (%s). "
               "Make sure SD card lines have pull-up resistors in place.",
               esp_err_to_name(return_value));
    }
  }

  return return_value;
}

void SD::unmount() {
  esp_vfs_fat_sdcard_unmount(sd_info.mount_point, card);
  ESP_LOGI(tag, "Unmounted");
}

void SD::print_info() { sdmmc_card_print_info(stdout, card); }