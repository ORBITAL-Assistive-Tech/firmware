idf_component_register(SRCS "sd_card_methods.c"
                       INCLUDE_DIRS "."
                       REQUIRES fatfs esp_adc "utils"
                       WHOLE_ARCHIVE)