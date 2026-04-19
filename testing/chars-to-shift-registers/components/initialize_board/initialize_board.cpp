#include "driver/gpio.h"
#include "initialize_board.h"

void initialize_board(void) {
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
