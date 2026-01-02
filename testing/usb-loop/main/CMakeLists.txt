set(srcs "usb_loop_main.cpp")

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "."
                       REQUIRES "utils"
                                fatfs
                                freertos
                                "shift_register"
                                "sd_card_methods"
                       WHOLE_ARCHIVE)

if(NOT CONFIG_SOC_SDMMC_HOST_SUPPORTED)
    fail_at_build_time(sdmmc ""
                             "Only ESP32-S3 targets are supported.")
endif()

set_source_files_properties("usb_loop_main.cpp"
                            PROPERTIES COMPILE_FLAGS "-std=c++23")
