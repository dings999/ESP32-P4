
include($ENV{IDF_PATH}/tools/cmake/version.cmake) # $ENV{IDF_VERSION} was added after v4.3...
idf_component_register(
    SRCS  "assets.h" "font/font_puhui_20_4.c" "font/font_puhui_16_4.c"
    INCLUDE_DIRS "."
    REQUIRES lvgl__lvgl
)