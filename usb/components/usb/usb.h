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
constexpr uint16_t TINYUSB_CDC_RX_BUFSIZE = 512;
typedef struct {
    uint8_t buf[TINYUSB_CDC_RX_BUFSIZE + 1];     // Data buffer
    size_t buf_len;                                     // Number of bytes received
    uint8_t itf;                                        // Index of CDC device interface
} app_message_t;

void tinyusb_cdc_rx_callback(int itf, cdcacm_event_t *event);
void tinyusb_cdc_line_state_changed_callback(int itf, cdcacm_event_t *event);
void check_data_received(QueueHandle_t queue);

class AppQueueHandler {
    private:
        static QueueHandle_t app_queue;
        static AppQueueHandler* instancePtr;

        AppQueueHandler() {}
    public:
        // Deleting the copy constructor to prevent copies
        AppQueueHandler(const AppQueueHandler& obj) = delete;

        static AppQueueHandler* getInstance();
        static QueueHandle_t getQueue();
};

extern AppQueueHandler* handler;