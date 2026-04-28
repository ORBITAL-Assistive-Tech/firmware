#include <cstdint>
#include <cstring>
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

void ShiftRegisters::display_character_set(std::uint8_t* characters, std::size_t num_characters) {

    std::uint8_t inputs[ShiftRegisters::total_cells];

    for(int i=0; i<num_characters; i++){
        inputs[i] = convert_character(characters[i]);
    }

    if(num_characters<ShiftRegisters::total_cells){
        for(int i=num_characters; i<ShiftRegisters::total_cells; i++){
            inputs[i] = 0b00000000;
        }
    }

    initialize_board();

    // rclk off
    gpio_set_level(GPIO_NUM_13, 0);

    for(int i=0; i<8; i++){

        std::uint8_t input = inputs[i];

        for(int j=0; j<8; j++){
            // srclk off
            gpio_set_level(GPIO_NUM_47, 0);
            // rclk off
            gpio_set_level(GPIO_NUM_13, 0);

            // pass the jth bit into shift register
            gpio_set_level(GPIO_NUM_14, (input>>j) & 1);

            esp_rom_delay_us(500);

            // srclk on
            gpio_set_level(GPIO_NUM_47, 1);
            // rclk on
            gpio_set_level(GPIO_NUM_13, 1);

            // delay
            esp_rom_delay_us(500);

            // SER (data in)
            gpio_set_level(GPIO_NUM_14, 0);

        }
    }
    
    // rclk on
    gpio_set_level(GPIO_NUM_13, 1);
    // delay
    esp_rom_delay_us(500);
    // rclk off
    gpio_set_level(GPIO_NUM_13, 0);

}

void ShiftRegisters::display_string(char* input) {

    std::size_t num_characters = std::strlen(input);

    ESP_LOGI(TAG, "%d", num_characters);

    std::uint8_t characters[num_characters];

    for(int i=0; i<num_characters; i++){
        characters[i] = static_cast<std::uint8_t>(input[i]);
    }

    int i=0;
    while(1){ // display each character set until end of string

        if(i+ShiftRegisters::total_cells > num_characters){ // TODO: FIX THIS
            ShiftRegisters::display_character_set(characters + i, num_characters - i);
            i += ShiftRegisters::total_cells;
        } else {
            ShiftRegisters::display_character_set(characters + i, ShiftRegisters::total_cells);
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(1000));

    }
}
