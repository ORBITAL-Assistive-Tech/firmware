#include <stdint.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "display_string.h"

static const char *TAG = "main";

extern "C" void app_main(void) {
    ShiftRegisters::display_string("HELLO!");
}

// while (1) {
//     ESP_LOGI(TAG, "Turning the LED %s!", s_led_state == true ? "ON" : "OFF");
//     blink_led();
//     /* Toggle the LED state */
//     s_led_state = !s_led_state;
//     // vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
//     vTaskDelay(pdMS_TO_TICKS(200));
// }
