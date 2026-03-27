# ESP32 Pet Tracker

ESP32 pet tracker firmware built with C++ and ESP-IDF framework.

## Project Overview

- **Stack**: C++, ESP-IDF v6.0, FreeRTOS, LoRa SX1262, GPS, BLE
- **Targets**: ESP32-S3 (primary), ESP32-C6 (alternate)
- **Reference**: https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/

## Guides

See the `.agents/` folder for detailed guidelines:

### Workflow & Rules

- [.agents/rules/git.md](.agents/rules/git.md) - Git workflow
- [.agents/rules/github.md](.agents/rules/github.md) - GitHub workflow
- [.agents/rules/markdown.md](.agents/rules/markdown.md) - Markdown guidelines

### Project Guides

- [.agents/guides/build.md](.agents/guides/build.md) - Build commands and CI
- [.agents/guides/testing.md](.agents/guides/testing.md) - Unit testing
- [.agents/guides/documentation.md](.agents/guides/documentation.md) - Documentation standards

## Quick Reference

### Pin Configuration

See `firmware/main/board_config.h` for ESP32S3/ESP32C6 pin mappings.

### Dependencies

- `driver` - GPIO, UART, SPI, I2C
- `freertos` - Real-time OS
- `esp_timer` - High-resolution timer
- `nvs_flash` - Non-volatile storage
- `esp_bt` - Bluetooth

### Common Issues

- **sdkconfig target mismatch**: `rm -rf build sdkconfig` and rebuild
- **FreeRTOS API signatures**: Verify in ESP-IDF v6.0 headers
- **Port access**: Add user to `docker` group or use `sudo`
