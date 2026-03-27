# Testing Guide

## Host-Based Unit Tests

Host tests run on Linux without hardware, using mocked ESP-IDF headers.

```bash
cd firmware/tests
mkdir -p build && cd build
cmake .. && make -j$(nproc)
./test_nmea_parser    # NMEA parsing tests
./test_button_handler # Button debounce tests
./test_led_driver    # LED driver tests
./test_lora_driver    # LoRa driver tests
./test_geofence       # Geofence tests
```

## Target Tests

```bash
cd firmware
idf.py build
idf.py test
```

## Adding New Host Tests

1. **Create mock headers** in `tests/include/` if new ESP-IDF dependencies are needed
2. **Implement mock functions** in `tests/src/` following the existing pattern
3. **Add test file** following Catch2 TEST_CASE/SECTION format
4. **Update CMakeLists.txt** to include new test executable

## Mock Maintenance

| If you add... | Update these files |
|--------------|-------------------|
| `esp_timer_get_time()` | `tests/include/esp_timer.h`, `tests/src/esp_timer_mock.cpp` |
| `gpio_get_level()` | `tests/include/driver/gpio.h`, `tests/src/gpio_mock.cpp` |
| `ESP_ERROR_CHECK()` | `tests/include/esp_err.h` |
| `ESP_LOG*` macros | `tests/include/esp_log.h` |

**Key principle**: Keep mocks minimal - only mock what the code actually calls. Prefer thin wrappers around source files rather than comprehensive mocks.
