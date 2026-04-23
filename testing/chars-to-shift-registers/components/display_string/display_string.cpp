#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "display_string.h"
#include "initialize_board.h"
#include "convert_character.h"
#include "driver/gpio.h"
#include "esp_rom_sys.h"
#include "esp_log.h"

using namespace ShiftRegisters;

static const char *TAG = "display_string";

void ShiftRegisters::display_character_set(uint8_t* characters, size_t num_characters) {

    int total_cells = 8;

    uint8_t inputs[total_cells];

    for(int i=0; i<num_characters; i++){
        inputs[i] = convert_character(characters[i]);
    }

    if(num_characters<total_cells){
        for(int i=num_characters; i<total_cells; i++){
            inputs[i] = 0b00000000;
        }
    }

    initialize_board();

    for(int i=0; i<8; i++){

        uint8_t input = inputs[i];
        // rclk off
        gpio_set_level(GPIO_NUM_13, 0);

        for(int j=0; j<8; j++){
            // srclk off
            gpio_set_level(GPIO_NUM_47, 0);

            // pass the jth bit into shift register
            gpio_set_level(GPIO_NUM_14, (input>>j) & 1);

            esp_rom_delay_us(500);

            // srclk on
            gpio_set_level(GPIO_NUM_47, 1);

            // delay
            esp_rom_delay_us(500);

            // data pin 0?
            gpio_set_level(GPIO_NUM_14, 0);

            // ESP_LOGI(TAG, "%d: %d", i*8+j, (input>>(i*8+j)) & 1);
        }

        // rclk on
        gpio_set_level(GPIO_NUM_13, 1);
        // delay
        esp_rom_delay_us(500);
        
    }
    // // rclk on
    // gpio_set_level(GPIO_NUM_13, 1);
    // // delay
    // esp_rom_delay_us(500);
    // // rclk off
    // gpio_set_level(GPIO_NUM_13, 0);


}

void ShiftRegisters::display_string(char* input) {

    size_t num_characters = strlen(input);
    uint8_t characters[num_characters];

    for(int i=0; i<num_characters; i++){
        characters[i] = (uint8_t)input[i];
    }

    int i=0;
    while(1){
        if(i+8 > num_characters){
            ShiftRegisters::display_character_set(characters + i, num_characters - i);
            i = 0;
        } else {
            ShiftRegisters::display_character_set(characters + i, 8);
            i += 8;
        }
        // esp_rom_delay_us(1000000);
        ESP_LOGI(TAG, "");
        vTaskDelay(pdMS_TO_TICKS(1000));

        uint8_t zeros[] = {32, 32, 32, 32, 32, 32, 32, 32};

        ShiftRegisters::display_character_set(zeros, 8);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
