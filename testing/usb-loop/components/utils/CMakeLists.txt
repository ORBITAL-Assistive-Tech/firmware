idf_component_register(SRCS "utils.cpp"
                       INCLUDE_DIRS "include"
                       )

set_source_files_properties("utils.cpp"
                PROPERTIES COMPILE_FLAGS "-std=c++23")