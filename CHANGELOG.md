# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/).

## [Unreleased]

### Added

- `.clang-format` configuration for GNU-style C++ formatting (ESP-IDF conventions)
- `.clangd/config.yaml` for clangd LSP configuration
- `.agents/guides/ide-setup.md` - Comprehensive IDE setup guide
- Pre-commit hooks for clang-format and markdownlint
- `PROGRESS.md` - Consolidated implementation status tracking

### Changed

- Markdown line length limit: 200 → 120 characters
- Updated `.pre-commit-config.yaml` with clang-format hook

### Fixed

- Removed duplicate `app_main()` causing linker errors
- Fixed ESP32-S3 pin conflict (LED and LoRa NSS both on GPIO_8)
- Fixed ESP32C6 pin conflict (ACCEL SDA and LoRa RESET both on GPIO_1)
- Added missing `gpio_install_isr_service()` initialization
- Added missing `i2c_driver_install()` for accelerometer
- Fixed event group memory leak in LoRaDriver destructor
- Fixed duplicate SX1262 command definitions
- Fixed GPS buffer overflow check
- Fixed integer overflow in accelerometer threshold calculation
- Fixed geofence test expectation for small radius zones
- Added missing mocks: `vEventGroupDelete()`, `gpio_install_isr_service()`
- Phase 3: `TrackerStateMachine::init()` now propagates errors from `Config::init()` and `Config::load()`
- Phase 4: ButtonHandler integrated into state machine
- Phase 4: Button wake now uses shorter sleep duration
- Phase 4: `last_wake` now assigned and used for sleep duration decisions

## [0.2.0] - 2025-04-05

### Added

- State machine for tracker lifecycle management with motion-aware sleep
- Pin configuration documentation in `hardware/tracker/PIN_MAPPING.md`
- Programmatic PCB builder script
- ESP-IDF v6.0 support for ESP32-S3 and ESP32-C6

### Changed

- Refactored main.cpp to use state machine pattern
- Updated board pin mappings for both ESP32-S3 and ESP32-C6 targets

## [0.1.0] - 2025-03-25

### Added

- Initial project structure
- GPS driver with NMEA parsing
- LoRa driver (SX1262) with basic transmission
- LED and button drivers
- Accelerometer driver (LIS3DH)
- Geofencing module with circle zone support
- BLE fallback support
- Deep sleep with timer and GPIO wakeup
- Host-based unit tests (NMEA parser, button handler, LED driver, LoRa driver, geofence)
- KiCad PCB design files for tracker and base station
- Flask web UI for base station
- CI/CD workflows

[Unreleased]: https://github.com/glennmcewan/esp32-tracker/compare/v0.2.0...HEAD
[0.2.0]: https://github.com/glennmcewan/esp32-tracker/compare/v0.1.0...v0.2.0
[0.1.0]: https://github.com/glennmcewan/esp32-tracker/releases/tag/v0.1.0
