
add_library(ui ${SOURCES})


idf_component_register(
    SRCS  "screens/ui_Screen_keytools.c" "keytools_ui.c" "keytools_ui_events.c"
    REQUIRES esp_tinyusb esp_driver_gpio
    INCLUDE_DIRS "."

)
