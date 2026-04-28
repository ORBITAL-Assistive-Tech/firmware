#include <cstdint>
#include <cctype>
#include <cstdio>
#include "convert_character.h"

#include "esp_log.h"

static const char *TAG = "convert_character";

using namespace ShiftRegisters;

std::uint8_t ShiftRegisters::convert_character(std::uint8_t input) {
    return ShiftRegisters::conversion_table[std::toupper(input)-0x20];
}


void ShiftRegisters::print_conversion(std::uint8_t input){
    std::uint8_t converted_input = ShiftRegisters::convert_character(input);
    for(int i=5; i>=0; i--){
        ESP_LOGI(TAG, "%d", (converted_input>>i) & 1);
    }
    std::printf("\n");
}
