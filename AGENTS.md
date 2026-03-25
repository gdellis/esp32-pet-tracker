# AGENTS.md

ESP32-C6 pet tracker firmware built with C++ and ESP-IDF framework.

## Project Overview

- **Type**: Embedded C++ project (ESP32-C6)
- **Stack**: C++, ESP-IDF v6.0, FreeRTOS, LoRa SX1262, GPS, BLE
- **Target**: ESP32-C6 (RISC-V)
- **Reference**: https://docs.espressif.com/projects/esp-idf/en/stable/esp32c6/

## Agent Guidelines

For detailed guidelines, see:

- [.agents/guides/](.agents/guides/) - Project-specific guides
- [.agents/rules/](.agents/rules/) - Coding and workflow rules
  - [git.md](.agents/rules/git.md) - Git workflow
  - [github.md](.agents/rules/github.md) - GitHub workflow
  - [markdown.md](.agents/rules/markdown.md) - Markdown guidelines

## Project Structure

```
firmware/
├── CMakeLists.txt       # ESP-IDF root CMake
├── sdkconfig.defaults   # ESP-IDF configuration
├── build.sh            # Docker-based build script
└── main/
    ├── CMakeLists.txt   # Main component
    └── main.cpp         # Application entry
```

## Build Commands

### Using Docker (Recommended)

```bash
cd firmware
./build.sh
```

### Flash to Device

```bash
docker run --rm -v $(pwd):/workspace -w /workspace espressif/idf:v6.0 \
  sh -c ". /opt/esp/idf/export.sh && idf.py -p /dev/ttyACM0 flash monitor"
```

### Manual Build (with ESP-IDF installed)

```bash
cd firmware
idf.py build
idf.py -p /dev/ttyACM0 flash monitor
```

## Code Style Guidelines

### C++ Conventions

- **Formatting**: Follow ESP-IDF style (GNU brace style)
- **Naming**:
  - Types/Classes: `PascalCase` (e.g., `GpioDriver`, `LedDriver`)
  - Functions/Variables: `snake_case` (e.g., `get_pin()`, `led_state`)
  - Constants: `SCREAMING_SNAKE_CASE` (e.g., `LED_PIN`, `MAX_RETRIES`)
- **Line length**: Max 100 characters
- **Error handling**: Use `ESP_ERROR_CHECK()` for ESP-IDF errors
- **C++ features**: Use classes for driver abstraction, avoid raw pointers

### ESP-IDF Specific

- **Panics**: In embedded context, use `ESP_LOGW` or `ESP_LOGE` for errors
- **Memory**: Avoid dynamic allocation; prefer stack or static buffers
- **Concurrency**: Use FreeRTOS tasks and queues, not threads

### Module Structure (Future)

```
main/
├── main.cpp           # Application entry point
├── gpio_driver.cpp    # GPIO abstraction
├── led_driver.cpp     # LED driver
├── button_handler.cpp # Button debounce
├── ble.cpp            # Bluetooth Low Energy
├── gps.cpp            # GPS parsing
├── lora.cpp           # LoRa SX1262 driver
└── config.h           # Configuration constants
```

## Hardware Configuration

| Pin | Function | Notes |
|-----|----------|-------|
| GPIO8 | LED | Active high |
| GPIO9 | Button | Active low (pulled up) |
| GPIO4 | LoRa TX | |
| GPIO5 | LoRa RX | |
| GPIO6 | LoRa Reset | |
| GPIO7 | GPS TX | |
| GPIO15 | GPS RX | |

## Testing

```bash
# Build and run tests
idf.py build
idf.py test
```

## Common Issues

- **Docker build**: Ensure Docker is running and you have permissions
- **Port access**: Add user to `docker` group or use `sudo`
- **Flash**: Ensure device is in download mode (hold BOOT, press RESET)
- **Monitor**: Use `idf.py monitor` to view serial output

## Dependencies (ESP-IDF Components)

- `driver` - GPIO, UART, SPI, I2C
- `freertos` - Real-time OS
- `esp_timer` - High-resolution timer
- `esp_log` - Logging
- `nvs_flash` - Non-volatile storage
- `esp_bt` - Bluetooth
- `esp_wifi` - WiFi
