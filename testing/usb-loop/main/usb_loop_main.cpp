/* SD card and FAT filesystem example.
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

// This example uses SDMMC peripheral to communicate with SD card.

#include "driver/sdmmc_host.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sd_card_methods.h"
#include "sdmmc_cmd.h"
#include "shift_register.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/unistd.h>

static const char *tag = "main";

extern "C" void app_main(void) {
  esp_err_t ret;

  // Options for mounting the filesystem.
  // If format_if_mount_failed is set to true, SD card will be partitioned and
  // formatted in case when mounting fails.
  esp_vfs_fat_sdmmc_mount_config_t mount_config = {
      .format_if_mount_failed = false,
      .max_files = 5,
      .allocation_unit_size = 16 * 1024,
      .disk_status_check_enable = false,
      .use_one_fat = false};

  sdmmc_card_t *card;
  const char mount_point[] = MOUNT_POINT;

  // By default, SD card frequency is initialized to SDMMC_FREQ_DEFAULT (20MHz)
  // For setting a specific frequency, use host.max_freq_khz (range 400kHz -
  // 40MHz for SDMMC) Example: for fixed frequency of 10MHz, use
  // host.max_freq_khz = 10000;
  sdmmc_host_t host = SDMMC_HOST_DEFAULT();

  sdmmc_slot_config_t slot_config = {
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
      .cd = SDMMC_SLOT_NO_CD,
      .wp = SDMMC_SLOT_NO_WP,
      .width = SD_CARD_BUS_WIDTH,
      .flags = 0,
  };

  ret = esp_vfs_fat_sdmmc_mount(mount_point, &host, &slot_config, &mount_config,
                                &card);

  if (ret != ESP_OK) {
    if (ret == ESP_FAIL) {
      ESP_LOGE(tag, "Failed to mount filesystem. "
                    "If you want the card to be formatted, set the "
                    "EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
    } else {
      ESP_LOGE(tag,
               "Failed to initialize the card (%s). "
               "Make sure SD card lines have pull-up resistors in place.",
               esp_err_to_name(ret));
    }
    return;
  }
  ESP_LOGI(tag, "Filesystem mounted");

  // Card has been initialized, print its properties
  sdmmc_card_print_info(stdout, card);

  const char *file = MOUNT_POINT "/hello.brf";
  uint8_t braille_data[MAX_LINE_SIZE];
  ret = sd_read_file(file, braille_data, MAX_LINE_SIZE);

  // read text data and convert it to braille format, then take the uint8_t
  // array and push it to shift register
  if (ret != ESP_OK) {
    ESP_LOGE(tag, "Data read did not work. Check sd_read_file.");
    return;
  }
  // All done, unmount partition and disable SDMMC peripheral
  esp_vfs_fat_sdcard_unmount(mount_point, card);
  ESP_LOGI(tag, "Unmounted");
  // HELL06 -> hello!

  // Configure shift register and turn on the outputs
  shift_register_configure();
  set_oe(LOW);

  // Push to register every 10 seconds
  while (1) {
    push_to_shift_register(MAX_LINE_SIZE - 1, braille_data);
    vTaskDelay(pdMS_TO_TICKS(10000));
  }
}
