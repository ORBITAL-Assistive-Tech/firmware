/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <cstdint>
#include <string>
#include "esp_log.h"
#include "tinyusb_default_config.h"
#include "tinyusb_cdc_acm.h"
#include "usb.h"

static const char *TAG = "main";

/**
 * @brief Application Queue
 */
usb::AppQueueHandler* handler_ref = usb::AppQueueHandler::getInstance();
QueueHandle_t app_queue_1;

extern "C" void app_main(void)
{
    // Create FreeRTOS primitives
    app_queue_1 = handler_ref->getQueue();
    assert(app_queue_1);

    ESP_LOGI(TAG, "USB initialization");
    const tinyusb_config_t tusb_cfg = TINYUSB_DEFAULT_CONFIG();
    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));

    tinyusb_config_cdcacm_t acm_cfg = {
        .cdc_port = TINYUSB_CDC_ACM_0,
        .callback_rx = &usb::tinyusb_cdc_rx_callback, // the first way to register a callback
        .callback_rx_wanted_char = NULL,
        .callback_line_state_changed = NULL,
        .callback_line_coding_changed = NULL
    };

    ESP_ERROR_CHECK(tinyusb_cdcacm_init(&acm_cfg));
    /* the second way to register a callback */
    ESP_ERROR_CHECK(tinyusb_cdcacm_register_callback(
                        TINYUSB_CDC_ACM_0,
                        CDC_EVENT_LINE_STATE_CHANGED,
                        &usb::tinyusb_cdc_line_state_changed_callback));

    ESP_LOGI(TAG, "USB initialization DONE");
    while (1) {
        std::string* str = usb::check_data_received(app_queue_1);
        ESP_LOGI(TAG, "%s", str->c_str());
    }
}
