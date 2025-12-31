idf_component_register(SRCS "sd_card_methods.cpp"
                       INCLUDE_DIRS "include"
                       REQUIRES "utils" fatfs esp_adc 
                       WHOLE_ARCHIVE)