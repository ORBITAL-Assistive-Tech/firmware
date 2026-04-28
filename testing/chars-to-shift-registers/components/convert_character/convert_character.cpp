#include <stdint.h>
#include <ctype.h>
#include "convert_character.h"

#include "esp_log.h"
static const char *TAG = "convert_character";

using namespace ShiftRegisters;

uint8_t ShiftRegisters::convert_character(uint8_t input) {

    uint8_t converted_input = conversion_table[input-0x20];

    // print converted character in binary
    // for(int i=5; i>=0; i--){
    //     printf("%d", (converted_input>>i) & 1);
    // }

    // printf("\n");



    // ESP_LOGI(TAG, "%d", conversion_table[toupper(input)-32]);

    return ShiftRegisters::conversion_table[toupper(input)-0x20];
}
