#pragma once
#include "driver/sdmmc_host.h"
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#if SOC_SDMMC_IO_POWER_EXTERNAL
#include "sd_pwr_ctrl_by_on_chip_ldo.h"
#endif

#define EXAMPLE_MAX_CHAR_SIZE 8
#define MAX_DATA 64

esp_err_t sd_write_file(const char *path, uint8_t data[], size_t data_len);

esp_err_t sd_read_file(const char *path, uint8_t dst[], size_t dst_len);