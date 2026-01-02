idf_component_register(SRCS "shift_register.cpp"
                       INCLUDE_DIRS "include"
                       REQUIRES driver freertos
                       )
set_source_files_properties("shift_register.cpp"
                            PROPERTIES COMPILE_FLAGS "-std=c++23")
