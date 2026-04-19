#include <stdint.h>
#include <string.h>
#include "display_string.h"

static const char *TAG = "main";

extern "C" void app_main(void) {
    display_string("HELLO!");
}
