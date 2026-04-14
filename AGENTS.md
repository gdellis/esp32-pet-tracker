# LoRaPaws32

**⚠️ IMPORTANT: Never merge PRs unless explicitly instructed by the user.**

LoRa-based pet tracker firmware built with C++ and ESP-IDF framework.

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
- [.agents/rules/workflow.md](.agents/rules/workflow.md) - Agent workflow rules

### Project Guides

- [.agents/guides/build.md](.agents/guides/build.md) - Build commands and CI
- [.agents/guides/testing.md](.agents/guides/testing.md) - Unit testing
- [.agents/guides/documentation.md](.agents/guides/documentation.md) - Documentation standards

## Current Progress

See [PROGRESS.md](PROGRESS.md) for detailed implementation status.

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

### ESP-IDF Installation

ESP-IDF v6.0 is installed at `~/.espressif/v6.0/esp-idf`. To activate the environment:

```bash
. ~/.espressif/v6.0/esp-idf/export.sh
```

Or add to shell profile (`~/.bashrc` or `~/.zshrc`):
```bash
alias ef='. ~/.espressif/v6.0/esp-idf/export.sh'
```

**Build commands:**
```bash
. ~/.espressif/v6.0/esp-idf/export.sh  # Activate ESP-IDF
idf.py build                           # Build firmware
idf.py -p /dev/ttyACM0 flash monitor  # Flash and monitor
```

## IDE Setup

### LSP Warnings (Expected)

When using clangd or other GCC-based language servers, you may see warnings about Xtensa-specific
compiler flags (`-mlongcalls`, `-fno-shrink-wrap`, `-fstrict-volatile-bitfields`).

**These are cosmetic and safe to ignore:**

- ESP-IDF compiles with `xtensa-esp-elf-gcc` which accepts these flags
- Host-based LSP servers use standard GCC which doesn't recognize them
- Actual builds via `idf.py build` or `./build.sh` work correctly

See [.agents/guides/ide-setup.md](.agents/guides/ide-setup.md) for detailed configuration.

### Code Formatting

- **C++**: `.clang-format` configured for GNU style (ESP-IDF conventions)
- **Markdown**: 120 character line length
- **Pre-commit**: Auto-formats C++ and lints Markdown before commit

See [.agents/guides/build.md](.agents/guides/build.md) for formatting details.
