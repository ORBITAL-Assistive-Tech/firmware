#include "usb.h"
#include "esp_log.h"

static const char *TAG = "usb";

static uint8_t rx_buf[TINYUSB_CDC_RX_BUFSIZE + 1];

/**
 * @brief Application Queue
 */
QueueHandle_t AppQueueHandler::app_queue = xQueueCreate(5, sizeof(app_message_t));

/**
 * @brief Get the AppQueueHandler instance
 */
AppQueueHandler* AppQueueHandler::getInstance() {
    if (instancePtr == nullptr) {
        instancePtr = new AppQueueHandler();
        AppQueueHandler::app_queue = xQueueCreate(5, sizeof(app_message_t));
    }
    return instancePtr;
}

/**
 * @brief Ge the queue attached to the handler.
 */
QueueHandle_t AppQueueHandler::getQueue() {
    AppQueueHandler* instance = AppQueueHandler::getInstance();
    ESP_LOGI(TAG, "USB initialization");
    return instance->app_queue;
}

AppQueueHandler* AppQueueHandler::instancePtr = nullptr;

AppQueueHandler* handler = AppQueueHandler::getInstance();
QueueHandle_t app_queue = handler->getQueue();

/**
 * @brief CDC device RX callback
 *
 * CDC device signals, that new data were received
 *
 * @param[in] itf   CDC device index
 * @param[in] event CDC event type
 */
void tinyusb_cdc_rx_callback(int itf, cdcacm_event_t *event)
{
    /* initialization */
    size_t rx_size = 0;

    /* read */
    esp_err_t ret = tinyusb_cdcacm_read(static_cast<tinyusb_cdcacm_itf_t>(itf), rx_buf, TINYUSB_CDC_RX_BUFSIZE, &rx_size);
    if (ret == ESP_OK) {

        app_message_t tx_msg = {
            .buf_len = rx_size,
            .itf = itf,
        };

        memcpy(tx_msg.buf, rx_buf, rx_size);
        xQueueSend(app_queue, &tx_msg, 0);
    } else {
        ESP_LOGE(TAG, "Read Error");
    }
}

/**
 * @brief CDC device line change callback
 *
 * CDC device signals, that the DTR, RTS states changed
 *
 * @param[in] itf   CDC device index
 * @param[in] event CDC event type
 */
void tinyusb_cdc_line_state_changed_callback(int itf, cdcacm_event_t *event)
{
    int dtr = event->line_state_changed_data.dtr;
    int rts = event->line_state_changed_data.rts;
    ESP_LOGI(TAG, "Line state changed on channel %d: DTR:%d, RTS:%d", itf, dtr, rts);
}

/**
 * @brief check a queue for data
 * 
 * Logs received data from USB if data has been received.
 * 
 * @param queue Queue to check
 */
std::string* check_data_received(QueueHandle_t queue) {
    app_message_t msg;
    if (xQueueReceive(queue, &msg, portMAX_DELAY)) {
        if (msg.buf_len) {

            /* Print received data*/
            ESP_LOGI(TAG, "Data from channel %d:", msg.itf);
            ESP_LOGI(TAG, "%d", msg.buf);
            char arr[msg.buf_len + 1];
            for (size_t i = 0; i < msg.buf_len; i++) {
                arr[i] = (char) msg.buf[i];
            }
            arr[msg.buf_len] = '\0';
            
            std::string* received_string = new std::string(arr);

            // ESP_LOG_BUFFER_HEXDUMP(TAG, msg.buf, msg.buf_len, ESP_LOG_INFO);

            /* write back */
            tinyusb_cdcacm_write_queue(static_cast<tinyusb_cdcacm_itf_t>(msg.itf), msg.buf, msg.buf_len);
            esp_err_t err = tinyusb_cdcacm_write_flush(static_cast<tinyusb_cdcacm_itf_t>(msg.itf), 0);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "CDC ACM write flush error: %s", esp_err_to_name(err));
            }
            return received_string;
        }
    }
}