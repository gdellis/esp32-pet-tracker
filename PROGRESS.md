# Project Progress

## Current Status

### Completed

- **Phase 1 & 2**: Integration fixes (PR #16 merged)
  - Added `gps.init()` and `lora.init()` calls
  - Made LED driver static and passed to state machine
  - Added `configure_wakeup_sources()` for GPIO wakeup
  - Fixed `get_wakeup_source()` to distinguish MOTION vs BUTTON
  - Fixed duplicate `esp_sleep_enable_gpio_wakeup()` calls
- **Phase 3**: Error handling (PR #17 merged)
  - `TrackerStateMachine::init()` now checks and propagates `Config::init()` and `Config::load()` errors
  - `Config::load()` returns `ESP_ERR_NOT_FOUND` when NVS is empty (all defaults used)
  - Config load log uses `ESP_LOGW` when all defaults used

### In Progress

- **Phase 4**: Integration (ButtonHandler, geofence, BLE notifications, last_wake tracking)
  - ✅ ButtonHandler integrated into state machine
  - ✅ Button wake now uses shorter sleep duration (`BUTTON_WAKE_SLEEP_MS`)
  - ✅ Button wakeup properly configured for deep sleep
  - ✅ `last_wake` now assigned and used
  - ⏳ Geofence integration (not started)
  - ⏳ BLE notifications on geofence breach (not started)

---

## Implementation Order Summary

| Phase | Module | Status |
|-------|--------|--------|
| 1 | Project scaffold, CMake, Docker build | ✅ Done |
| 2 | Deep sleep, wake sources | ✅ Done (PR #1) |
| 2 | GPS UART driver, NMEA parsing | ✅ Done (PR #2) |
| 2 | Button handler with debounce | ✅ Done (bug fixed) |
| 2 | LED driver | ✅ Done |
| 3 | Unit tests with mocks | ✅ Done (PR #3, PR #7) |
| 4 | LoRa SX1262 driver | ✅ Done (PR #6, PR #7) |
| 4 | BLE GATT server | ✅ Done (PR #9) |
| 4 | LIS3DH accelerometer | ✅ Done (PR #10) |
| 5 | State machine | ✅ Done (PR #11) |
| 6 | NVS config storage | ✅ Done (PR #12) |
| 7 | Motion-aware sleep | ✅ Done (PR #11) |
| 8 | Geofencing | ✅ Done (PR #13, #14) |
| 9 | Integration testing | Pending |
| 10 | Base station (Pi + Python) | Pending |
| 11 | Flask Web UI | Pending |
| 12 | PCB design | Pending |
| 13 | Enclosure design | Pending |

---

## Phase 4: Integration Details

### ButtonHandler Integration ✅

- **Status**: Integrated
- **Files**: `button_handler.cpp`, `button_handler.hpp`, `state_machine.cpp`
- **Changes**:
  - Added `ButtonHandler` member to `TrackerStateMachine`
  - Added `check_button()` method
  - Button now configured as deep sleep wakeup source

### Button Wakeup Configuration ✅

- **Status**: Fixed
- **Files**: `state_machine.cpp:configure_wakeup_sources()`
- **Changes**: Added `esp_sleep_enable_gpio_wakeup()` call
- **Note**: Button uses `gpio_wakeup_enable()` with `GPIO_INTR_LOW_LEVEL`

### Geofence Integration

- **Status**: Not started
- **Files**: `geofence.cpp`, `geofence.hpp`
- **Need**: Check location against geofences after GPS fix, trigger BLE notification if breach detected

### BLE Notifications

- **Status**: Not started
- **Current**: BLE GATT server exists for fallback TX
- **Need**: Send notification to connected device on geofence breach

### last_wake Tracking ✅

- **Status**: Fixed
- **Changes**: 
  - `last_wake` now assigned from `get_wake_source()` result
  - `get_sleep_duration()` uses `BUTTON_WAKE_SLEEP_MS` when last_wake was BUTTON

### Battery Reading

- **Status**: Not implemented
- **Plan**: Add ADC reading for battery voltage (hardware dependent - requires PCB)
- **Note**: Deferred until PCB design complete

---

## Key Decisions Made

- **ESP-IDF version**: v6.0
- **Target MCU**: ESP32-C6 (RISC-V)
- **LoRa driver**: Custom SX1262 driver (SPI interface)
- **GPS parsing**: Custom header-only NMEA parser (`nmea_parser.hpp`)
- **GPS module**: HGLRC M100-5883 (M10, 10Hz, 115200 baud, 72ch, multi-GNSS)
- **LoRa frequency**: 915 MHz (US region)
- **BLE fallback**: Included for direct phone connectivity when in range
- **Accelerometer**: LIS3DH for motion detection (wake on movement)
- **Base station**: Python on Raspberry Pi with LoRa hat
- **MQTT broker**: HiveMQ (self-hosted)
- **WiFi config**: AP mode on base station for easy configuration
- **Waterproofing**: Silicone gasket between case halves
- **Testing**: Host-based unit tests with mocked ESP-IDF headers
