idf_component_register(
    SRCS "wifi_manager.c" "wifi_credentials.c" "portal_dns.c"
    INCLUDE_DIRS "."
    REQUIRES esp_wifi esp_event esp_netif nvs_flash esp_http_server esp_timer json lwip esp32_c5_touch_lcd_2_8
    EMBED_FILES "root.html"
)
