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

static const char *tag = "sd_card_methods";

esp_err_t sd_write_file(const char *path, uint8_t data[], size_t data_len) {
  ESP_LOGI(tag, "Opening file %s", path);
  FILE *f = fopen(path, "w");
  if (f == NULL) {
    ESP_LOGE(tag, "Failed to open file for writing");
    return ESP_FAIL;
  }

  for (size_t i = 0; i < data_len; i++) {
    const uint8_t character = ascii_braille_conversion[BRAILLE_OFFSET(data[i])];
    fprintf(f, "%c", static_cast<char>(character));
  }
  fprintf(f, "\n");
  fclose(f);
  ESP_LOGI(tag, "File written");

  return ESP_OK;
}

esp_err_t sd_read_file(const char *path, uint8_t dst[], size_t dst_len) {
  ESP_LOGI(tag, "Reading file %s", path);
  FILE *f = fopen(path, "r");
  if (f == NULL) {
    ESP_LOGE(tag, "Failed to open file for reading");
    return ESP_FAIL;
  }
  char line[MAX_LINE_SIZE];
  fgets(line, sizeof(line), f);
  fclose(f);

  // strip newline
  char *pos = strchr(line, '\n');
  if (pos) {
    *pos = '\0';
  }

  for (size_t i = 0; i < sizeof(line); i++) {
    dst[i] = ascii_braille_conversion[BRAILLE_OFFSET(line[i])] << 2;
  }
  ESP_LOGI(tag, "Read from file: '%s'", line);

  return ESP_OK;
}