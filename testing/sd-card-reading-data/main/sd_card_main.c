/* SD card and FAT filesystem example.
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

// This example uses SDMMC peripheral to communicate with SD card.

#include "driver/sdmmc_host.h"
#include "esp_vfs_fat.h"
#include "sd_card_methods.h"
#include "sd_test_io.h"
#include "sdkconfig.h"
#include "sdmmc_cmd.h"
#include "shift_register.h"
#include "utils.h"
#include <string.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#if SOC_SDMMC_IO_POWER_EXTERNAL
#include "sd_pwr_ctrl_by_on_chip_ldo.h"
#endif

#define EXAMPLE_MAX_CHAR_SIZE 8
#define MAX_DATA 64

static const char *TAG = "example";

#define MOUNT_POINT "/sdcard"
#define EXAMPLE_IS_UHS1                                                        \
  (CONFIG_EXAMPLE_SDMMC_SPEED_UHS_I_SDR50 ||                                   \
   CONFIG_EXAMPLE_SDMMC_SPEED_UHS_I_DDR50)

void app_main(void) {
  esp_err_t ret;

  // Options for mounting the filesystem.
  // If format_if_mount_failed is set to true, SD card will be partitioned and
  // formatted in case when mounting fails.
  esp_vfs_fat_sdmmc_mount_config_t mount_config = {
      .max_files = 5, .allocation_unit_size = 16 * 1024};
  sdmmc_card_t *card;
  const char mount_point[] = MOUNT_POINT;

  // By default, SD card frequency is initialized to SDMMC_FREQ_DEFAULT (20MHz)
  // For setting a specific frequency, use host.max_freq_khz (range 400kHz -
  // 40MHz for SDMMC) Example: for fixed frequency of 10MHz, use
  // host.max_freq_khz = 10000;
  sdmmc_host_t host = SDMMC_HOST_DEFAULT();

  // This initializes the slot without card detect (CD) and write protect (WP)
  // signals. Modify slot_config.gpio_cd and slot_config.gpio_wp if your board
  // has these signals.
  sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

  // Set bus width to use:
  slot_config.width = 4;

  // On chips where the GPIOs used for SD card can be configured, set them in
  // the slot_config structure:
  slot_config.clk = CONFIG_EXAMPLE_PIN_CLK;
  slot_config.cmd = CONFIG_EXAMPLE_PIN_CMD;
  slot_config.d0 = CONFIG_EXAMPLE_PIN_D0;
  slot_config.d1 = CONFIG_EXAMPLE_PIN_D1;
  slot_config.d2 = CONFIG_EXAMPLE_PIN_D2;
  slot_config.d3 = CONFIG_EXAMPLE_PIN_D3;

  // Enable internal pullups on enabled pins. The internal pullups
  // are insufficient however, please make sure 10k external pullups are
  // connected on the bus. This is for debug / example purpose only.
  slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;

  ret = esp_vfs_fat_sdmmc_mount(mount_point, &host, &slot_config, &mount_config,
                                &card);

  if (ret != ESP_OK) {
    if (ret == ESP_FAIL) {
      ESP_LOGE(TAG, "Failed to mount filesystem. "
                    "If you want the card to be formatted, set the "
                    "EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
    } else {
      ESP_LOGE(TAG,
               "Failed to initialize the card (%s). "
               "Make sure SD card lines have pull-up resistors in place.",
               esp_err_to_name(ret));
    }
    return;
  }
  ESP_LOGI(TAG, "Filesystem mounted");

  // Card has been initialized, print its properties
  sdmmc_card_print_info(stdout, card);

  const char *file = MOUNT_POINT "/hello.brf";
  uint8_t braille_data[EXAMPLE_MAX_CHAR_SIZE - 1];
  ret = sd_read_file(file, braille_data, EXAMPLE_MAX_CHAR_SIZE);

  // read text data and convert it to braille format, then take the uint8_t
  // array and push it to shift register
  if (ret != ESP_OK) {
    ESP_LOGI(TAG, "heyy...this aint working");
    return;
  }
  // All done, unmount partition and disable SDMMC peripheral
  esp_vfs_fat_sdcard_unmount(mount_point, card);
  ESP_LOGI(TAG, "Unmounted");

  configure();
  gpio_set_level(UE, LOW);

  while (1) {
    push_to_shift_register(EXAMPLE_MAX_CHAR_SIZE - 2, braille_data);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}
