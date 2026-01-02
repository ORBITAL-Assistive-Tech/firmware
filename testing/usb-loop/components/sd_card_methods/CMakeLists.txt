idf_component_register(SRCS "sd_card_methods.cpp"
                       INCLUDE_DIRS "include"
                       REQUIRES "utils" fatfs esp_adc 
                       WHOLE_ARCHIVE)

set_source_files_properties("sd_card_methods.cpp"
                            PROPERTIES COMPILE_FLAGS "-std=c++23")