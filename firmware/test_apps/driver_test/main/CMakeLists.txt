set(srcs "test_app_main.cpp" "test_driver.cpp")

idf_component_register(SRCS ${srcs}
                     PRIV_REQUIRES unity esp_driver_gpio esp_timer
                     WHOLE_ARCHIVE)
