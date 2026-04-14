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
  - ✅ Geofence integration - location checked against zones after GPS fix
  - ✅ BLE alert sent on geofence breach

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
| 9 | Integration testing | ✅ Done (PR #24) |
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

### Geofence Integration ✅

- **Status**: Complete
- **Files**: `geofence.cpp`, `geofence.hpp`, `config.hpp`, `state_machine.cpp`
- **Changes**:
  - Added zone storage to `TrackerConfig` (up to 4 zones)
  - Zones persisted to/from NVS via config save/load
  - `check_geofence()` called after GPS fix
  - Breach triggers BLE alert and sets `is_moving = true`
  - Default "Home" zone created if no zones configured

### BLE Notifications ✅

- **Status**: Complete
- **Files**: `ble.hpp`, `ble.cpp`, `state_machine.cpp`
- **Changes**:
  - Added `BleAlertData` struct with alert type, zone index, location, timestamp
  - Added `BLE_CHAR_ALERT_UUID` (0x2A06) for alert characteristic
  - Added `send_alert()` method using `esp_ble_gatts_send_indicate()`
  - Alert sent on geofence breach to connected device

### last_wake Tracking ✅

- **Status**: Fixed
- **Changes**: 
  - `last_wake` now assigned from `get_wake_source()` result
  - `get_sleep_duration()` uses `BUTTON_WAKE_SLEEP_MS` when last_wake was BUTTON

### Battery Reading

- **Status**: ✅ Implemented (PR #25 merged)
- **Files**: `battery.hpp`, `battery.cpp`
- **Features**:
  - ADC voltage reading with voltage divider calculation
  - Percentage calculation (3000mV empty → 4200mV full)
  - Integrated into state machine `try_lora_send()` for transmission
  - GPS power switching via MOSFET

---

## Codebase Analysis (2026-04-12)

### Overall Health: 7/10 - Functional firmware, base station missing

### Code Quality Assessment

| Aspect | Rating | Notes |
|--------|--------|-------|
| Code Organization | 8/10 | Clean separation, follows ESP-IDF patterns |
| Feature Completeness | 7/10 | Core firmware complete, base station pending |
| Error Handling | 8/10 | Proper error returns, BLE bugs fixed |
| Testing | 8/10 | Good host test coverage + integration tests |
| Documentation | 7/10 | Good README/PROGRESS, DESIGN.md updated |
| Build System | 8/10 | Clean Docker setup, good CI |
| Code Quality | 8/10 | BLE bugs fixed, no critical issues |

### Critical Bugs Identified

| Issue | Location | Impact | Status |
|-------|----------|---------|--------|
| BLE characteristic handles never populated | `ble.cpp:373-378` | READ operations fail | ✅ Fixed PR #23 |
| `check_button()` dead code | `state_machine.cpp:235-237` | Never called | ✅ Fixed PR #23 |
| BLE `update_location()` doesn't notify | `ble.cpp:330-340` | Clients don't receive updates | ✅ Fixed PR #23 |
| `(void)ret` ignores errors | `ble.cpp:224` | Silently swallows errors | ✅ Fixed PR #23 |

### Missing/Incomplete

| Item | Phase | Priority |
|------|-------|----------|
| Battery reading | 13 (PCB) | Low (hardware-dependent) |
| Integration tests | 9 | **High** |
| Base station (Python/Flask) | 10 | **High** |
| PCB design | 12 | Medium |
| Enclosure design | 13 | Low |

### Recommended Next Steps

1. ~~Fix critical BLE bugs~~ - ✅ Done (PR #23 merged)
2. ~~Phase 9: Integration testing~~ - ✅ Done (PR #24 merged)
3. ~~GPS power switching~~ - ✅ Done (PR #25 merged)
4. ~~Battery ADC reading~~ - ✅ Done (PR #25 merged)
5. **Phase 10: Base station** - Start Python/Flask receiver (firmware LoRa TX is complete)
6. **PCB design** - Begin KiCad layout to enable battery reading

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
