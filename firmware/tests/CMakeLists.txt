cmake_minimum_required(VERSION 3.16)
project(tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.4.0
    FIND_PACKAGE_ARGS
)

set(CATCH_CONFIG_FALLBACK_STRINGIFIER ON)
FetchContent_MakeAvailable(catch2)

add_executable(test_nmea_parser test_nmea_parser.cpp)
target_include_directories(test_nmea_parser PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/../main
)
target_link_libraries(test_nmea_parser PRIVATE Catch2::Catch2WithMain)

add_executable(test_button_handler test_button_handler.cpp
    src/esp_timer_mock.cpp
    src/gpio_mock.cpp
    ${CMAKE_SOURCE_DIR}/../main/button_handler.cpp
)
target_include_directories(test_button_handler PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/../main
)
target_link_libraries(test_button_handler PRIVATE Catch2::Catch2WithMain)

add_executable(test_led_driver test_led_driver.cpp
    src/gpio_mock.cpp
    ${CMAKE_SOURCE_DIR}/../main/gpio_driver.cpp
    ${CMAKE_SOURCE_DIR}/../main/led_driver.cpp
)
target_include_directories(test_led_driver PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/../main
)
target_link_libraries(test_led_driver PRIVATE Catch2::Catch2WithMain)

add_executable(test_lora_driver test_lora_driver.cpp
    src/esp_timer_mock.cpp
    src/gpio_mock.cpp
    src/spi_mock.cpp
    src/event_groups_mock.cpp
    ${CMAKE_SOURCE_DIR}/../main/lora/sx1262.cpp
)
target_include_directories(test_lora_driver PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/../main
)
target_link_libraries(test_lora_driver PRIVATE Catch2::Catch2WithMain)

add_executable(test_ble test_ble.cpp)
target_include_directories(test_ble PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(test_ble PRIVATE Catch2::Catch2WithMain)

add_executable(test_geofence test_geofence.cpp
    ${CMAKE_SOURCE_DIR}/../main/geofence.cpp
)
target_include_directories(test_geofence PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/../main
)
target_link_libraries(test_geofence PRIVATE Catch2::Catch2WithMain m)
