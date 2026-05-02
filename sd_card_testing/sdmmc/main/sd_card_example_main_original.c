/* SD card and FAT filesystem example.
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/unistd.h>

#include "driver/sdmmc_host.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "sd_card_config.h"
#include "sd_test_io.h"
#include "sdmmc_cmd.h"

#define EXAMPLE_MAX_CHAR_SIZE 64
#define EXAMPLE_IS_UHS1 (CONFIG_EXAMPLE_SDMMC_SPEED_UHS_I_SDR50 || CONFIG_EXAMPLE_SDMMC_SPEED_UHS_I_DDR50)

static const char *TAG = "example";

#ifdef CONFIG_EXAMPLE_DEBUG_PIN_CONNECTIONS
static const char *names[] = {"CLK", "CMD", "D0"
#ifdef CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4
    , "D1", "D2", "D3"
#endif
};

static const int pins[] = {
    CONFIG_EXAMPLE_PIN_CLK,
    CONFIG_EXAMPLE_PIN_CMD,
    CONFIG_EXAMPLE_PIN_D0,
#ifdef CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4
    CONFIG_EXAMPLE_PIN_D1,
    CONFIG_EXAMPLE_PIN_D2,
    CONFIG_EXAMPLE_PIN_D3,
#endif
};

static const int pin_count = sizeof(pins) / sizeof(pins[0]);

#if CONFIG_EXAMPLE_ENABLE_ADC_FEATURE
static const int adc_channels[] = {
    CONFIG_EXAMPLE_ADC_PIN_CLK,
    CONFIG_EXAMPLE_ADC_PIN_CMD,
    CONFIG_EXAMPLE_ADC_PIN_D0,
#ifdef CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4
    CONFIG_EXAMPLE_ADC_PIN_D1,
    CONFIG_EXAMPLE_ADC_PIN_D2,
    CONFIG_EXAMPLE_ADC_PIN_D3,
#endif
};
#endif

static pin_configuration_t pin_config = {
    .names = names,
    .pins = pins,
#if CONFIG_EXAMPLE_ENABLE_ADC_FEATURE
    .adc_channels = adc_channels,
#endif
};
#endif

static esp_err_t example_write_file(const char *path, const char *data)
{
    ESP_LOGI(TAG, "Opening file %s", path);
    FILE *f = fopen(path, "w");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return ESP_FAIL;
    }

    fprintf(f, "%s", data);
    fclose(f);
    ESP_LOGI(TAG, "File written");
    return ESP_OK;
}

static esp_err_t example_read_file(const char *path)
{
    ESP_LOGI(TAG, "Reading file %s", path);
    FILE *f = fopen(path, "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return ESP_FAIL;
    }

    char line[EXAMPLE_MAX_CHAR_SIZE];
    if (fgets(line, sizeof(line), f) == NULL) {
        fclose(f);
        ESP_LOGE(TAG, "Failed to read file");
        return ESP_FAIL;
    }
    fclose(f);

    char *pos = strchr(line, '\n');
    if (pos != NULL) {
        *pos = '\0';
    }
    ESP_LOGI(TAG, "Read from file: '%s'", line);
    return ESP_OK;
}

void app_main(void)
{
    esp_err_t ret;
    sdmmc_card_t *card;
    const char mount_point[] = MOUNT_POINT;

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
#ifdef CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED
        .format_if_mount_failed = true,
#else
        .format_if_mount_failed = false,
#endif
        .max_files = 5,
        .allocation_unit_size = 16 * 1024,
    };

    ESP_LOGI(TAG, "Initializing SD card");
    ESP_LOGI(TAG, "Using SDMMC peripheral");

    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
#if CONFIG_EXAMPLE_SDMMC_SPEED_HS
    host.max_freq_khz = SDMMC_FREQ_HIGHSPEED;
#elif CONFIG_EXAMPLE_SDMMC_SPEED_UHS_I_SDR50
    host.slot = SDMMC_HOST_SLOT_0;
    host.max_freq_khz = SDMMC_FREQ_SDR50;
    host.flags &= ~SDMMC_HOST_FLAG_DDR;
#elif CONFIG_EXAMPLE_SDMMC_SPEED_UHS_I_DDR50
    host.slot = SDMMC_HOST_SLOT_0;
    host.max_freq_khz = SDMMC_FREQ_DDR50;
#endif

    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

#if EXAMPLE_IS_UHS1
    slot_config.flags |= SDMMC_SLOT_FLAG_UHS1;
#endif

#ifdef CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4
    slot_config.width = 4;
#else
    slot_config.width = 1;
#endif

#ifdef CONFIG_SOC_SDMMC_USE_GPIO_MATRIX
    slot_config.clk = CONFIG_EXAMPLE_PIN_CLK;
    slot_config.cmd = CONFIG_EXAMPLE_PIN_CMD;
    slot_config.d0 = CONFIG_EXAMPLE_PIN_D0;
#ifdef CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4
    slot_config.d1 = CONFIG_EXAMPLE_PIN_D1;
    slot_config.d2 = CONFIG_EXAMPLE_PIN_D2;
    slot_config.d3 = CONFIG_EXAMPLE_PIN_D3;
#endif
#endif

    slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;

    ESP_LOGI(TAG, "Mounting filesystem");
    ret = esp_vfs_fat_sdmmc_mount(mount_point, &host, &slot_config, &mount_config, &card);
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                     "If you want the card to be formatted, set the EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                     "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
#ifdef CONFIG_EXAMPLE_DEBUG_PIN_CONNECTIONS
            check_sd_card_pins(&pin_config, pin_count);
#endif
        }
        return;
    }
    ESP_LOGI(TAG, "Filesystem mounted");

    sdmmc_card_print_info(stdout, card);

    const char *file_hello = MOUNT_POINT "/hello.txt";
    char data[EXAMPLE_MAX_CHAR_SIZE];
    snprintf(data, sizeof(data), "Hello %s!\n", card->cid.name);
    ret = example_write_file(file_hello, data);
    if (ret != ESP_OK) {
        esp_vfs_fat_sdcard_unmount(mount_point, card);
        return;
    }

    const char *file_foo = MOUNT_POINT "/foo.txt";
    struct stat st;
    if (stat(file_foo, &st) == 0) {
        unlink(file_foo);
    }

    ESP_LOGI(TAG, "Renaming file %s to %s", file_hello, file_foo);
    if (rename(file_hello, file_foo) != 0) {
        ESP_LOGE(TAG, "Rename failed");
        esp_vfs_fat_sdcard_unmount(mount_point, card);
        return;
    }

    ret = example_read_file(file_foo);
    if (ret != ESP_OK) {
        esp_vfs_fat_sdcard_unmount(mount_point, card);
        return;
    }

#ifdef CONFIG_EXAMPLE_FORMAT_SD_CARD
    ret = esp_vfs_fat_sdcard_format(mount_point, card);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to format FATFS (%s)", esp_err_to_name(ret));
        esp_vfs_fat_sdcard_unmount(mount_point, card);
        return;
    }

    if (stat(file_foo, &st) == 0) {
        ESP_LOGI(TAG, "file still exists");
        esp_vfs_fat_sdcard_unmount(mount_point, card);
        return;
    }

    ESP_LOGI(TAG, "file doesn't exist, formatting done");
#endif

    const char *file_nihao = MOUNT_POINT "/nihao.txt";
    snprintf(data, sizeof(data), "Nihao %s!\n", card->cid.name);
    ret = example_write_file(file_nihao, data);
    if (ret != ESP_OK) {
        esp_vfs_fat_sdcard_unmount(mount_point, card);
        return;
    }

    ret = example_read_file(file_nihao);
    if (ret != ESP_OK) {
        esp_vfs_fat_sdcard_unmount(mount_point, card);
        return;
    }

    esp_vfs_fat_sdcard_unmount(mount_point, card);
    ESP_LOGI(TAG, "Card unmounted");
}
