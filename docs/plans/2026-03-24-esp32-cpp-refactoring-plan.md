# ESP32 Pet Tracker: Rust to C++ Refactoring Plan

## Context

The current Rust-based ESP32 firmware project needs to be evaluated for refactoring to C++ with ESP-IDF framework. This exploration serves to:
1. Understand AI agent effectiveness with mature ESP-IDF ecosystem vs newer Rust esp-hal
2. Leverage existing C/C++ expertise for faster personal use development
3. Compare development experience across both approaches

## Current State (Rust Implementation)

### Project Structure

```
esp32-tracker/
├── Cargo.toml          # Rust dependencies
├── .cargo/config.toml  # Build target config
├── rust-toolchain.toml # Toolchain specification
├── build.rs           # Build script
├── src/
│   ├── lib.rs         # Gpio and Led drivers
│   └── main.rs        # Application entry
├── tests/
│   └── lib_test.rs    # Unit tests
└── .github/workflows/
    └── ci.yml         # CI pipeline
```

### Implemented Components

- `Gpio` driver struct (input pin handling)
- `Led` driver struct (output pin control)
- Basic button → LED toggle logic
- ESP32-C6 target (riscv32imac-unknown-none-elf)

### Build Command

```bash
cargo build --release --target riscv32imac-unknown-none-elf
```

## Target State (C++ with ESP-IDF)

### Project Structure

```
esp32-tracker/
├── CMakeLists.txt           # Root CMake file
├── sdkconfig               # ESP-IDF configuration
├── sdkconfig.defaults      # Default config overrides
├── main/
│   ├── CMakeLists.txt      # Main component CMake
│   ├── main.cpp            # Application entry
│   ├── gpio_driver.hpp     # GPIO abstraction
│   ├── gpio_driver.cpp
│   ├── led_driver.hpp      # LED driver
│   ├── led_driver.cpp
│   └── button_handler.cpp  # Button debounce & handling
├── components/             # Optional reusable components
│   └── (future: ble, gps, lorawan)
└── .github/workflows/
    └── ci.yml              # Updated CI pipeline
```

### Build Command

```bash
idf.py build
```

## Refactoring Steps

### Phase 1: Project Setup

| Step | Task | Notes |
|------|------|-------|
| 1.1 | Create ESP-IDF project structure | Use `idf.py create-project` or manual CMake |
| 1.2 | Set target ESP32-C6 | `idf.py set-target esp32c6` |
| 1.3 | Configure GPIO8 (LED), GPIO9 (Button) in sdkconfig | Match current hardware |
| 1.4 | Verify empty project builds | Baseline before adding code |
| 1.5 | Update CI workflow for ESP-IDF | Use docker container with ESP-IDF |

### Phase 2: Core Drivers (Port from Rust)

| Step | Rust Code | C++ Equivalent |
|------|-----------|----------------|
| 2.1 | `Gpio` struct | `GpioDriver` class with `gpio_num_t` |
| 2.2 | `Led` struct | `LedDriver` class wrapping `GpioDriver` |
| 2.3 | `Gpio::new()` | Constructor with `gpio_config()` |
| 2.4 | `Led::toggle()` | `gpio_set_level()` / `gpio_get_level()` |
| 2.5 | `Led::on()` / `off()` | Simple level setters |
| 2.6 | Button input reading | `gpio_get_level()` with polling |
| 2.7 | Basic LED toggle on button press | Main loop logic |

### Phase 3: Testing Infrastructure

| Step | Task |
|------|------|
| 3.1 | Add Unity test framework to CMake |
| 3.2 | Port `tests/lib_test.rs` → C++ unit tests |
| 3.3 | Add CI step for unit tests |

### Phase 4: Enhanced Features (Future)

| Feature | Rust Implementation | C++ Implementation |
|---------|---------------------|-------------------|
| Deep sleep | `esp_sleep_enable_timer_wakeup()` | Same ESP-IDF API |
| BLE advertising | `esp_bt_gap_set_device_name()` | Same API, C++ wrappers |
| GPS data parsing | Manual struct parsing | `nmea_parser` component |
| LoRa SX1262 | `lmkmac` crate | `esp_lora_*.c` drivers |
| Battery monitoring | ADC via `esp_adc` | Same, direct API call |

## Rust vs C++ Comparison for This Project

| Aspect | Rust (esp-hal) | C++ (ESP-IDF) |
|--------|---------------|---------------|
| AI Agent Effectiveness | Lower (less training data) | Higher (extensive examples) |
| Learning Curve | Language + Embedded | ESP-IDF concepts only |
| Type Safety | Stronger (ownership system) | Manual discipline |
| Runtime Overhead | Zero-cost abstractions | Minimal (C++ nature) |
| Ecosystem Maturity | New (~2 years) | Mature (10+ years) |
| Debugging Support | GDB, less documented | OpenOCD, excellent docs |
| Community Examples | Limited | Massive |
| Build Time | Longer (first-time) | Faster (caching) |
| Dependency Management | Cargo | ESP-IDF components + CMake |

## Success Criteria

1. **Functional Equivalence**: C++ implementation produces same LED/button behavior
2. **Build Pipeline**: `idf.py build` works, CI passes
3. **Flash Verification**: Code flashes to ESP32-C6 and runs
4. **Learning Outcome**: Document AI agent effectiveness differences

## Risks & Mitigations

| Risk | Mitigation |
|------|------------|
| ESP-IDF setup complexity | Use official docker image with ESP-IDF pre-installed |
| AI generating incorrect ESP-IDF code | Verify against ESP-IDF documentation |
| Project structure differences | Follow ESP-IDF component model strictly |

## Timeline (Estimated)

| Phase | Time | Focus |
|-------|------|-------|
| Phase 1 | 30 min | Project setup, verify build |
| Phase 2 | 1-2 hrs | Port drivers, basic functionality |
| Phase 3 | 30 min | Tests and CI |
| Phase 4 | Future | Deep sleep, BLE, GPS |

## Next Steps (After This Plan)

1. Create C++ project structure
2. Verify ESP-IDF docker/CI setup
3. Port `gpio_driver.hpp/cpp`
4. Port `led_driver.hpp/cpp`
5. Implement button handler in main loop
6. Add unit tests
7. Flash and verify on hardware
8. Document AI agent experience differences
