#pragma once
#include <stdint.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "tinyusb.h"
#include "tinyusb_default_config.h"
#include "tinyusb_cdc_acm.h"
#include "sdkconfig.h"

// constexpr const char *TAG = "example";
constexpr const uint8_t TINYUSB_CDC_RX_BUFSIZE = 512;
typedef struct {
    uint8_t buf[TINYUSB_CDC_RX_BUFSIZE + 1];     // Data buffer
    size_t buf_len;                                     // Number of bytes received
    uint8_t itf;                                        // Index of CDC device interface
} app_message_t;


void tinyusb_cdc_rx_callback(int itf, cdcacm_event_t *event);
void tinyusb_cdc_line_state_changed_callback(int itf, cdcacm_event_t *event);
constexpr const tinyusb_config_cdcacm_t acm_cfg = {
    .cdc_port = TINYUSB_CDC_ACM_0,
    .callback_rx = &tinyusb_cdc_rx_callback, // the first way to register a callback
    .callback_rx_wanted_char = NULL,
    .callback_line_state_changed = NULL,
    .callback_line_coding_changed = NULL
};
