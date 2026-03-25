#include <stdint.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "example";

static void configure_led(void)
{
    gpio_reset_pin(GPIO_NUM_13);
    gpio_reset_pin(GPIO_NUM_14);
    gpio_reset_pin(GPIO_NUM_42);
    gpio_reset_pin(GPIO_NUM_47);

    gpio_set_direction(GPIO_NUM_13, GPIO_MODE_OUTPUT); // pin 12, RCLK
    gpio_set_direction(GPIO_NUM_14, GPIO_MODE_OUTPUT); // pin 14, SER (data in)
    gpio_set_direction(GPIO_NUM_42, GPIO_MODE_OUTPUT); // pin 13, OE (output enable)
    gpio_set_direction(GPIO_NUM_47, GPIO_MODE_OUTPUT); // pin 11, SRCLK

    gpio_set_level(GPIO_NUM_42,0);
}

uint8_t convert_character(uint8_t input) {
    return conversion_table[input-32];
}

uint8_t conversion_table[] = {
    0b00000000, 0b00011101, 0b00000010, 0b00001111, 0b00110101, 0b00100101, 0b00111101, 0b00001000,
    0b00111011, 0b00011111, 0b00100001, 0b00001101, 0b00000001, 0b00001001, 0b00000101, 0b00001100,
    0b00001011, 0b00010000, 0b00011000, 0b00010010, 0b00010011, 0b00010001, 0b00011010, 0b00011011,
    0b00011001, 0b00001010, 0b00100011, 0b00000011, 0b00110001, 0b00111111, 0b00001110, 0b00100111,
    0b00000100, 0b00100000, 0b00110000, 0b00100100, 0b00100110, 0b00100010, 0b00110100, 0b00110110,
    0b00110010, 0b00010100, 0b00010110, 0b00101000, 0b00111000, 0b00101100, 0b00101110, 0b00101010,
    0b00111100, 0b00111110, 0b00111010, 0b00011100, 0b00011110, 0b00101001, 0b00111001, 0b00010111,
    0b00101101, 0b00101111, 0b00101011, 0b00010101, 0b00000000, 0b00110111, 0b00000110, 0b00000111
};

/*
    " ": "000000"
    "!": "011101",
    '"': "000010",
    "#": "001111",
    "$": "110101",
    "%": "100101",
    "&": "111101",
    "'": "001000",
    "(": "111011",
    ")": "011111",
    "*": "100001",
    "+": "001101",
    ",": "000001",
    "-": "001001",
    ".": "000101",
    "/": "001100",
    "0": "001011",
    "1": "010000",
    "2": "011000",
    "3": "010010",
    "4": "010011",
    "5": "010001",
    "6": "011010",
    "7": "011011",
    "8": "011001",
    "9": "001010",
    ":": "100011",
    ";": "000011",
    "<": "110001",
    "=": "111111",
    ">": "001110",
    "?": "100111",
    "@": "000100",
    "A": "100000",
    "B": "110000",
    "C": "100100",
    "D": "100110",
    "E": "100010",
    "F": "110100",
    "G": "110110",
    "H": "110010",
    "I": "010100",
    "J": "010110",
    "K": "101000",
    "L": "111000",
    "M": "101100",
    "N": "101110",
    "O": "101010",
    "P": "111100",
    "Q": "111110",
    "R": "111010",
    "S": "011100",
    "T": "011110",
    "U": "101001",
    "V": "111001",
    "W": "010111",
    "X": "101101",
    "Y": "101111",
    "Z": "101011",
    "[": "010101",
    "\": nothing
    "]": "110111",
    "^": "000110",
    "_": "000111",
    
    
*/

void display_string(char* input) {

    size_t num_characters = strlen(input);
    uint8_t characters[num_characters];

    for(int i=0; i<num_characters; i++){
        characters[i] = (uint8_t)input[i];
    }

    int i=0;
    while(1){
        if(i+8 > num_characters){
            display_character_set(characters[i], num_characters - i);
            i = 0;
        } else {
            display_character_set(characters[i], 8);
            i += 8;
        }
    }
}

void display_character_set(uint8_t* characters, size_t num_characters) {

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

    configure_led();

    for(int i=0; i<8; i++){
        uint8_t input = inputs[i];
        for(int j=0; j<8; j++){
            // pass the i*8+jth bit into shift register
            gpio_set_level(GPIO_NUM_14, (input>>j) & 1);

            esp_rom_delay_us(500);
            // srclk on
            gpio_set_level(GPIO_NUM_47, 1);
            // delay
            // vTaskDelay(pdMS_TO_TICKS(.5));
            esp_rom_delay_us(500);
            // srclk off
            gpio_set_level(GPIO_NUM_47, 0);
            // delay
            esp_rom_delay_us(500);

            // ESP_LOGI(TAG, "%d: %d", i*8+j, (input>>(i*8+j)) & 1);
        }
    }
    // rclk on
    gpio_set_level(GPIO_NUM_13, 1);
    // delay
    esp_rom_delay_us(500);
    // rclk off
    gpio_set_level(GPIO_NUM_13, 0);

    esp_rom_delay_us(1000000);

}

void app_main(void) {
    display_string("hello");
}

// while (1) {
//     ESP_LOGI(TAG, "Turning the LED %s!", s_led_state == true ? "ON" : "OFF");
//     blink_led();
//     /* Toggle the LED state */
//     s_led_state = !s_led_state;
//     // vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
//     vTaskDelay(pdMS_TO_TICKS(200));
// }
