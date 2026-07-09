# ESP-IDF组件构建配置
idf_component_register(
    SRCS "ball_qmi.c"                    
    INCLUDE_DIRS "."                     
    REQUIRES lvgl  qmi8658        
)
