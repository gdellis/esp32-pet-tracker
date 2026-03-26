# AGENTS.md

ESP32 pet tracker firmware built with C++ and ESP-IDF framework.

## Project Overview

- **Type**: Embedded C++ project (ESP32)
- **Stack**: C++, ESP-IDF v6.0, FreeRTOS, LoRa SX1262, GPS, BLE
- **Primary Target**: ESP32-S3 (Tensilica LX7)
- **Alternate Target**: ESP32-C6 (RISC-V)
- **Reference**: https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/

## Multi-Board Support

The firmware supports both ESP32S3 and ESP32C6. Pin configurations are defined in `firmware/main/board_config.h`.

| Pin | ESP32S3 | ESP32C6 | Function |
|-----|---------|---------|----------|
| LED | GPIO8 | GPIO8 | Status LED |
| Button | GPIO9 | GPIO9 | Push button |
| GPS TX | GPIO7 | GPIO4 | GPS UART TX |
| GPS RX | GPIO15 | GPIO5 | GPS UART RX |
| LoRa TX | GPIO4 | GPIO6 | LoRa UART TX |
| LoRa RX | GPIO5 | GPIO7 | LoRa UART RX |
| LoRa Reset | GPIO6 | GPIO10 | LoRa reset |

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
├── CMakeLists.txt           # ESP-IDF root CMake
├── sdkconfig.defaults      # ESP-IDF configuration
├── build.sh                # Docker-based build script
├── main/
│   ├── CMakeLists.txt      # Main component
│   ├── main.cpp            # Application entry
│   ├── board_config.h      # Multi-board pin configuration
│   ├── gpio_driver.cpp     # GPIO abstraction
│   ├── led_driver.cpp      # LED driver
│   ├── button_handler.cpp  # Button debounce
│   ├── gps.cpp             # GPS NMEA parsing
│   └── nmea_parser.hpp    # Header-only NMEA parser (testable on host)
└── tests/
    ├── CMakeLists.txt      # Host test CMake
    ├── test_nmea_parser.cpp
    ├── test_button_handler.cpp
    ├── test_led_driver.cpp
    ├── include/             # Mock headers for host testing
    │   ├── esp_timer.h
    │   ├── esp_err.h
    │   ├── esp_log.h
    │   └── driver/gpio.h
    └── src/
        ├── esp_timer_mock.cpp
        └── gpio_mock.cpp
```

## Build Commands

### Using Docker (Recommended)

```bash
cd firmware
./build.sh                    # Build for ESP32S3 (default)
./build.sh --board esp32c6    # Build for ESP32C6
./build.sh --flash --monitor  # Build, flash, and monitor
```

### Flash to Device

```bash
docker run --rm -v $(pwd):/workspace -w /workspace espressif/idf:v6.0 \
  sh -c ". /opt/esp/idf/export.sh && idf.py -D IDF_TARGET=esp32s3 -p /dev/ttyACM0 flash monitor"
```

### Manual Build (with ESP-IDF installed)

```bash
cd firmware
idf.py set-target esp32s3    # Set target once
idf.py build                  # Build
idf.py -p /dev/ttyACM0 flash monitor  # Flash and monitor
```

### CI Build Target

The CI builds for **both ESP32S3 and ESP32C6 targets**. When modifying code:
- Always test locally for your target first
- The CI uses `idf.py build` which reads from `sdkconfig`
- If CI fails with "Target esp32c6 in sdkconfig does not match IDF_TARGET esp32s3", run `rm -rf build sdkconfig` locally and rebuild
- When adding ESP-IDF APIs, verify the API exists in ESP-IDF v6.0 for both architectures (x86_64 and RISC-V compilers may have different behavior)

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

### Documentation

- **Doxygen docstrings**: Add Doxygen-style comments to all public APIs:

  ```cpp
  /**
   * @brief Initialize the GPS module
   * @param uart_num UART peripheral number
   * @return ESP_OK on success, error code otherwise
   */
  esp_err_t init(uart_port_t uart_num);
  ```

- **File headers**: Add brief description at top of each source file
- **Inline comments**: Explain *why*, not *what* - code should be self-documenting for *what*
- **TODO format**: Use `// TODO(username): description` for future work

### ESP-IDF Specific

- **Panics**: In embedded context, use `ESP_LOGW` or `ESP_LOGE` for errors
- **Memory**: Avoid dynamic allocation; prefer stack or static buffers
- **Concurrency**: Use FreeRTOS tasks and queues, not threads

### Module Structure

```
main/
├── main.cpp           # Application entry point
├── gpio_driver.cpp    # GPIO abstraction
├── led_driver.cpp     # LED driver
├── button_handler.cpp # Button debounce
├── nmea_parser.hpp   # Header-only NMEA parser
├── gps.cpp           # GPS UART driver
├── deep_sleep.hpp    # Deep sleep utility
└── config.h          # Configuration constants
```

## Hardware Configuration

Pin configurations for each board are defined in `board_config.h`. Default configuration is ESP32S3.

| Pin | ESP32S3 | ESP32C6 | Function |
|-----|---------|---------|----------|
| LED | GPIO8 | GPIO8 | Status LED |
| Button | GPIO9 | GPIO9 | Push button |
| GPS TX | GPIO7 | GPIO4 | GPS UART TX |
| GPS RX | GPIO15 | GPIO5 | GPS UART RX |
| LoRa TX | GPIO4 | GPIO6 | LoRa UART TX |
| LoRa RX | GPIO5 | GPIO7 | LoRa UART RX |
| LoRa Reset | GPIO6 | GPIO10 | LoRa reset |

## Testing

### Host-Based Unit Tests (Recommended for Development)

Host tests run on Linux without hardware, using mocked ESP-IDF headers.

```bash
# Build and run host tests
cd firmware/tests
mkdir -p build && cd build
cmake .. && make -j$(nproc)
./test_nmea_parser    # NMEA parsing tests (43 assertions)
./test_button_handler # Button debounce tests (4 assertions)
./test_led_driver     # LED driver tests (9 assertions)
```

### Target Tests (Requires Hardware)

```bash
cd firmware
idf.py build
idf.py test
```

### Adding New Host Tests

1. **Create mock headers** in `tests/include/` if new ESP-IDF dependencies are needed
2. **Implement mock functions** in `tests/src/` following the existing pattern
3. **Add test file** following Catch2 TEST_CASE/SECTION format
4. **Update CMakeLists.txt** to include new test executable

### Mock Maintenance

When modifying ESP-IDF dependencies:

| If you add... | Update these files |
|--------------|-------------------|
| `esp_timer_get_time()` | `tests/include/esp_timer.h`, `tests/src/esp_timer_mock.cpp` |
| `gpio_get_level()` | `tests/include/driver/gpio.h`, `tests/src/gpio_mock.cpp` |
| `ESP_ERROR_CHECK()` | `tests/include/esp_err.h` |
| `ESP_LOG*` macros | `tests/include/esp_log.h` |

**Key principle**: Keep mocks minimal - only mock what the code actually calls. Prefer thin wrappers around source files rather than comprehensive mocks.

## Common Issues

- **Docker build**: Ensure Docker is running and you have permissions
- **Port access**: Add user to `docker` group or use `sudo`
- **Flash**: Ensure device is in download mode (hold BOOT, press RESET)
- **Monitor**: Use `idf.py monitor` to view serial output
- **sdkconfig target mismatch**: If CI fails with target mismatch, `rm -rf build sdkconfig` and rebuild
- **FreeRTOS API signatures**: When adding FreeRTOS calls, verify exact signatures in ESP-IDF v6.0 headers - some APIs differ between versions (e.g., `xEventGroupClearBits` takes `const EventBits_t`)

## Dependencies (ESP-IDF Components)

- `driver` - GPIO, UART, SPI, I2C
- `freertos` - Real-time OS
- `esp_timer` - High-resolution timer
- `esp_log` - Logging
- `nvs_flash` - Non-volatile storage
- `esp_bt` - Bluetooth
- `esp_wifi` - WiFi

## Documentation Maintenance

### Keeping PLAN.md Updated

After completing any feature, bug fix, or significant change:

1. **Mark completed items** in `docs/hardware/PLAN.md`:
   - Phase sections (2.1, 2.2, etc.) - Add `- [x] **Done**` with PR reference
   - Unit test sections - Mark tests as done when passing
   - Update the Implementation Order Summary table

2. **Update relevant sections**:
   - Phase subsections with completion status and PR numbers
   - Bug fixes noted in appropriate modules
   - New tests added to host-based unit tests list

3. **Example updates**:

   ```markdown
   ### 2.4 Button Handler (Day 1)
   ...
   - [x] **Done** - Implemented in `button_handler.cpp`, merged PR #3
   - [x] **Bug Fixed**: Debounce bug (first press always failed) resolved by proper initialization to `-INT64_MAX`
   ```

   ```markdown
   | Phase | Module | Status |
   |-------|--------|--------|
   | 2 | Button handler with debounce | ✅ Done (bug fixed, PR #3) |
   ```

4. **After merging a PR**: Update PLAN.md in the same commit or immediately after

### Keeping DESIGN.md Updated

The design document captures the current implementation architecture. Update it when:

1. **Hardware changes**: New components, pin reassignments, or power architecture
2. **Interface changes**: Modified class/function signatures in public APIs
3. **Protocol changes**: Altered packet formats, state machine transitions
4. **New features**: Adding modules not previously documented

**What to update**:
- Module Structure - if new files are added
- Key Interfaces - if public APIs change
- Pin mappings in Hardware Configuration
- Packet structure in LoRa Protocol section
- TODO checklist at the end

**Example updates**:

```markdown
### Module Structure
...
- [x] ~~Implement BLE GATT server~~ → `ble.cpp` added in PR #5
```

```markdown
| Phase | Module | Status |
|-------|--------|--------|
| 4 | BLE GATT server | ✅ Done (PR #5) |
```
