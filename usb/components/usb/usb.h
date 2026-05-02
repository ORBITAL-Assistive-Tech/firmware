#pragma once
#include <cstdint>
#include <string>
#include "freertos/FreeRTOS.h"
#include "tinyusb_cdc_acm.h"

// constexpr const char *TAG = "example";
constexpr size_t TINYUSB_CDC_RX_BUFSIZE = 1024;
typedef struct {
    uint8_t buf[TINYUSB_CDC_RX_BUFSIZE + 1];     // Data buffer
    size_t buf_len;                                     // Number of bytes received
    uint8_t itf;                                        // Index of CDC device interface
} app_message_t;

/**
 * Handler to manage a shared queue 
 */
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

void tinyusb_cdc_rx_callback(int itf, cdcacm_event_t *event);
void tinyusb_cdc_line_state_changed_callback(int itf, cdcacm_event_t *event);
std::string* check_data_received(QueueHandle_t queue);
