/* SD card and FAT filesystem example.
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

// This example uses SDMMC peripheral to communicate with SD card.

#include "esp_log.h"
#include "sd_card_methods.h"
#include "shift_register.h"
#include "utils.h"
#include <cstdint>
#include <string>

static const char *tag = "main";

extern "C" void app_main(void) {
  // Return value - error code
  esp_err_t ret;

  // Mount SD card
  ret = SD::mount();

  if (ret != ESP_OK) {
    return;
  }
  ESP_LOGI(tag, "Filesystem mounted");

  // Print SD card important data
  SD::print_info();

  // Create file to look for, read it and put it into braille data
  std::string file = SD::to_path("/hello.brf");
  std::uint8_t braille_data[MAX_LINE_SIZE];
  ret = SD::read_file(file, braille_data, MAX_LINE_SIZE);
  if (ret != ESP_OK) {
    return;
  }

  // unmount
  SD::unmount();
  // HELL06 -> hello!

  // Configure shift register and turn on the outputs
  ShiftRegister::configure();
  ShiftRegister::turn_on();

  // Push to register every 10 seconds
  while (true) {
    ret = ShiftRegister::push(braille_data, MAX_LINE_SIZE - 1);
    if (ret != ESP_OK) {
      return;
    }
    sleep_ms(10000);
  }
}
