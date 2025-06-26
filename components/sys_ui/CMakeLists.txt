
include($ENV{IDF_PATH}/tools/cmake/version.cmake) # $ENV{IDF_VERSION} was added after v4.3...
idf_component_register(
    SRCS "UIRefresher.cpp"
    INCLUDE_DIRS "."
    REQUIRES nvs_flash esp_wifi wifi esp-brookesia esp32_p4_wifi6_touch_lcd_4b
)