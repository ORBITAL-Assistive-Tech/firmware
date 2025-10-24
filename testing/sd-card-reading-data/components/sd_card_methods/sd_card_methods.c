#include "sd_card_methods.h"
#include "driver/sdmmc_host.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "sdkconfig.h"
#include "sdmmc_cmd.h"
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

esp_err_t sd_write_file(const char *path, uint8_t data[], size_t data_len) {
  ESP_LOGI(TAG, "Opening file %s", path);
  FILE *f = fopen(path, "w");
  if (f == NULL) {
    ESP_LOGE(TAG, "Failed to open file for writing");
    return ESP_FAIL;
  }

  for (size_t i = 0; i < data_len; i++) {
    const uint8_t character = ascii_braille_conversion[BRAILLE_OFFSET(data[i])];
    fprintf(f, "%c", (const char)character);
  }
  fprintf(f, "\n");
  fclose(f);
  ESP_LOGI(TAG, "File written");

  return ESP_OK;
}

esp_err_t sd_read_file(const char *path, uint8_t dst[], size_t dst_len) {
  ESP_LOGI(TAG, "Reading file %s", path);
  FILE *f = fopen(path, "r");
  if (f == NULL) {
    ESP_LOGE(TAG, "Failed to open file for reading");
    return ESP_FAIL;
  }
  char line[EXAMPLE_MAX_CHAR_SIZE];
  fgets(line, sizeof(line), f);
  fclose(f);

  // strip newline
  char *pos = strchr(line, '\n');
  if (pos) {
    *pos = '\0';
  }

  for (size_t i = 0; i < sizeof(line); i++) {
    dst[i] = ascii_braille_conversion[BRAILLE_OFFSET(line[i])]; // (uint8_t)
                                                                // line[i];
  }
  ESP_LOGI(TAG, "Read from file: '%s'", line);

  return ESP_OK;
}