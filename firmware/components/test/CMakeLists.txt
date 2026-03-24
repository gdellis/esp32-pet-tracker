idf_component_register(
    SRCS "test_drivers.cpp"
    INCLUDE_DIRS "." "../main"
    REQUIRES unity driver esp_timer
)
