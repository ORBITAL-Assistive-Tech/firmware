idf_component_register(SRCS "sd_card_methods.c"
                       INCLUDE_DIRS "."
                       REQUIRES "utils" fatfs esp_adc 
                       WHOLE_ARCHIVE)