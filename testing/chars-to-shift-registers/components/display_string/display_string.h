#include <stdint.h>

namespace ShiftRegisters{
    constexpr int total_cells = 8;

    void display_character_set(uint8_t* characters, size_t num_characters);
    void display_string(char* input);
}
