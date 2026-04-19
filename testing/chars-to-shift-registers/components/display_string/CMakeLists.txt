idf_component_register(SRCS "display_string.cpp"
                  INCLUDE_DIRS "." 
                  PRIV_REQUIRES "initialize_board" "convert_character" freertos driver)
