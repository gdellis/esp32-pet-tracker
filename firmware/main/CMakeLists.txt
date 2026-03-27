idf_component_register(
    SRCS "geofence.cpp" "config.cpp" "state_machine.cpp" "accelerometer.cpp" "main.cpp"
    "gpio_driver.cpp"
    "led_driver.cpp"
    "button_handler.cpp"
    "gps.cpp"
    "ble.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lora/sx1262.cpp"
    INCLUDE_DIRS "."
)
