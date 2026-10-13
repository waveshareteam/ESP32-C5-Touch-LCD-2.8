idf_component_register(
                         SRCS 
                              "./main.c" 
                              "./CH32V003/ch32v003.c"
                              
                         REQUIRES esp_driver_i2c esp_driver_gpio
                         INCLUDE_DIRS 
                              "./CH32V003"
                              "."

                       )
