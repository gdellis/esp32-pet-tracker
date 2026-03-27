# Pet Tracker Implementation Plan

## Overview

This plan outlines the implementation order for the ESP32-C6 pet tracker firmware. The goal is to build incrementally, starting with bare metal hardware access and adding complexity layer by layer.

---

## Phase 1: Project Scaffold

### 1.1 Initialize ESP-IDF C++ Project

```bash
# Create project with ESP-IDF
idf.py create-project pet-tracker
cd pet-tracker

# Configure for ESP32-C6
idf.py set-target esp32c6
idf.py menuconfig  # Configure JTAG, flash, etc.
```

### 1.2 Verify Build Pipeline

- [x] Build for ESP32-C6 target: `idf.py build`
- [x] Flash to device: `idf.py -p /dev/ttyACM0 flash monitor`
- [x] Verify basic "hello world" via `ESP_LOGI` output

**Done**: Build infrastructure validated via Docker (`build.sh`) and host tests.

### 1.3 Create Module Structure

```
firmware/
├── main/
│   ├── main.cpp             # Entry point
│   ├── lib.cpp              # Shared types (optional)
│   ├── gps.cpp              # GPS module
│   ├── nmea_parser.hpp      # Header-only NMEA parsing (testable)
│   ├── lora/
│   │   ├── sx1262.cpp
│   │   └── sx1262.hpp
│   ├── power.cpp            # Sleep/wake
│   ├── accelerometer.cpp    # LIS3DH I2C driver
│   ├── ble.cpp              # BLE GATT server
│   ├── geofence.cpp
│   ├── config.cpp           # NVS
│   └── error.cpp
└── tests/                   # Host-based unit tests with mocks
```

---

## Phase 2: Hardware Drivers

### 2.1 GPIO (Day 1)

**Goal**: Control LEDs, read button

```cpp
// gpio_driver.cpp
class GpioDriver {
public:
    static void init();
    static void set_led(bool state);
    static bool is_button_pressed();
};
```

- [x] **Done** - Implemented in `gpio_driver.cpp`

**Test**: LED blinks, button press detected

### 2.2 Power Management (Day 1–2)

**Goal**: Enter/exit deep sleep, configure RTC wake timer

```cpp
// deep_sleep.hpp
enum class WakeSource {
    Timer(uint32_t duration_ms),
    Button,
};

void enter_deep_sleep(WakeSource source);
WakeSource wake_reason();
uint16_t get_battery_voltage();  // mV via ADC
```

- [x] **Done** - Implemented in `deep_sleep.hpp`, merged PR #1

**Test**: Device sleeps for N seconds, wakes, reports wake reason

### 2.3 LED Driver (Day 1)

**Goal**: LED control with patterns

```cpp
// led_driver.cpp
class LedDriver {
public:
    static void init();
    static void set(bool on);
    static void blink(uint32_t interval_ms);
};
```

- [x] **Done** - Implemented in `led_driver.cpp`

### 2.4 Button Handler (Day 1)

**Goal**: Button debouncing

```cpp
// button_handler.cpp
class ButtonHandler {
public:
    ButtonHandler(gpio_num_t pin);
    void update();  // Call periodically
    bool was_pressed();  // Returns true once per press
};
```

- [x] **Done** - Implemented in `button_handler.cpp`, merged PR #3
- [x] **Bug Fixed**: Debounce bug (first press always failed) resolved by proper initialization to `-INT64_MAX`

**Test**: Button press detected, debounced correctly

### 2.5 Accelerometer LIS3DH (Day 2)

**Goal**: Motion detection via I2C, wake on movement

```cpp
// accelerometer.cpp
class Accelerometer {
public:
    esp_err_t init();
    esp_err_t enable_motion_interrupt(uint16_t threshold_mg);
    esp_err_t clear_interrupt();
    bool has_motion();
};
```

- [ ] **Pending** - Not yet implemented

**Test**: LIS3DH triggers interrupt on movement

### 2.6 GPS Module (Day 2–3)

**Goal**: Parse NMEA sentences, extract lat/lon/altitude

```cpp
// gps.cpp
class GpsDriver {
public:
    GpsDriver(gpio_num_t tx_pin, gpio_num_t rx_pin);
    void power_on();
    void power_off();
    GpsData read_fix(uint32_t timeout_ms);
};

// nmea_parser.hpp (header-only, testable on host)
struct GpsData {
    int32_t latitude;    // degrees * 1e6
    int32_t longitude;   // degrees * 1e6
    int32_t altitude;    // meters * 100 (cm)
    bool valid;
    uint32_t timestamp;
};

bool parse_nmea(const char* nmea, GpsData* out);
```

- [x] **Done** - Implemented in `gps.cpp` and `nmea_parser.hpp`, merged PR #2
- [x] **Updated** - Baud rate changed from 9600 to 115200 for M100-5883 compatibility
- [x] **Bugs Fixed**: NMEA sentence type detection (`nmea + 3`), field index offset, buffer size issues

**Test**:
- GPS powered on, receives NMEA on UART
- Fix extracted (timeout after 60s returns `valid: false`)

### 2.7 LoRa SX1262 (Day 3–5)

**Goal**: Initialize SX1262, send/receive packets

```cpp
// lora/sx1262.hpp
class LoRaDriver {
public:
    LoRaDriver();
    esp_err_t init();
    esp_err_t send(const RadioPacket& packet);
    esp_err_t recv(RadioPacket& packet, uint32_t timeout_ms);
    esp_err_t sleep();
    esp_err_t wake();
};
```

- [ ] **Pending** - Not yet implemented

**Test**:
- TX: Send packet, verify received on base station
- RX: Listen for packet with timeout
- Sleep: Verify low current draw

---

## Phase 3: State Machine

- [ ] **Pending** - Not yet implemented

### 3.1 Basic State Machine (Day 5–6)

Wake sources: RTC timer, LIS3DH motion interrupt, button press

```cpp
enum class State {
    DeepSleep,
    Idle,
    AcquiringGps,
    Transmitting,
};

void run_state_machine() {
    while (true) {
        switch (current_state) {
            case State::DeepSleep:
                wait_for_wake();
                current_state = State::Idle;
                break;
            case State::Idle:
                current_state = State::AcquiringGps;
                break;
            case State::AcquiringGps: {
                GpsData data = gps.read_fix(GPS_TIMEOUT_MS);
                current_state = State::Transmitting;
                break;
            }
            case State::Transmitting: {
                esp_err_t err = lora.send(data);
                if (err == ESP_OK) {
                    enter_deep_sleep(SLEEP_INTERVAL_MS);
                }
                break;
            }
        }
    }
}
```

### 3.2 Motion-Aware Sleep (Day 6)

When LIS3DH detects motion after stationary period, wake immediately.

```cpp
uint32_t determine_sleep_duration(bool is_moving) {
    if (is_moving) {
        return config.sleep_interval_ms;
    } else {
        return config.stationary_interval_ms;
    }
}
```

### 3.3 Retry Logic (Day 6)

- On TX failure: retry up to 3 times with exponential backoff
- On GPS timeout: transmit with `valid=false` flag set
- On TX failure + BLE in range: fallback to BLE notification

---

## Phase 4: Configuration & Storage

- [ ] **Pending** - Not yet implemented

### 4.1 NVS Configuration (Day 7)

```cpp
// config.hpp
struct TrackerConfig {
    uint32_t device_id;
    uint32_t sleep_interval_ms;
    uint32_t stationary_interval_ms;
    uint8_t tx_power;
    uint8_t sf;
};

esp_err_t load_config(TrackerConfig* out);
esp_err_t save_config(const TrackerConfig& config);
```

**Stored keys**:
- `device_id` (u32)
- `sleep_interval_ms` (u32)
- `stationary_interval_ms` (u32)
- `tx_power` (u8)
- `sf` (u8)

### 4.2 Geofence Storage (Day 7–8)

```cpp
struct Geofence {
    std::string name;
    ZoneType zone_type;  // Circle or Polygon
    int32_t center_lat;
    int32_t center_lon;
    uint32_t radius_m;  // for Circle
    std::vector<std::pair<int32_t, int32_t>> vertices;  // for Polygon
};

esp_err_t load_geofences(std::vector<Geofence>* out);
esp_err_t save_geofences(const std::vector<Geofence>& geofences);
```

---

## Phase 5: Geofencing

- [ ] **Pending** - Not yet implemented

### 5.1 Point-in-Circle (Day 8)

```cpp
bool check_geofence_circle(
    int32_t lat, int32_t lon,
    int32_t center_lat, int32_t center_lon,
    uint32_t radius_m
) {
    int64_t distance = haversine_distance(lat, lon, center_lat, center_lon);
    return distance <= radius_m;
}
```

### 5.2 Point-in-Polygon (Day 8–9)

Ray casting algorithm for polygon zones.

---

## Phase 6: Integration & Testing

### 6.1 Host-Based Unit Tests (In Progress)

- [x] NMEA parser tests (43 assertions, 5 test cases)
- [x] Button handler tests (4 assertions)
- [x] LED driver tests (9 assertions, 1 test case)
- [x] LoRa driver tests (14 assertions, 3 test cases)
- [ ] GPS driver tests
- [ ] Expand LoRa tests: packet TX/RX, error conditions, state transitions, timeouts

### 6.2 Target Tests

- [ ] Full cycle: wake → GPS fix → LoRa TX → deep sleep
- [ ] GPS timeout: wake → 60s no fix → TX with invalid flag → deep sleep
- [ ] TX failure: wake → GPS fix → TX fails → retry → success → deep sleep
- [ ] Button wake: wake immediately on button press
- [ ] Motion wake: LIS3DH triggers wake on movement
- [ ] Motion sleep: Device enters longer sleep interval when stationary
- [ ] BLE fallback: TX fails → BLE available → notify via BLE

### 6.3 Power Measurement

- [ ] Measure deep sleep current (target: <10µA including LIS3DH)
- [ ] Measure GPS acquisition current (target: ~25mA)
- [ ] Measure LoRa TX current (target: ~120mA)
- [ ] Measure LIS3DH low-power mode current (~3µA)
- [ ] Verify battery life estimate matches design

### 6.4 Range Test

- [ ] TX from 100m away, verify received
- [ ] TX from 1km away, verify received
- [ ] Find SF/range boundary

---

## Phase 7: Base Station (Python on Raspberry Pi)

### 7.1 Raspberry Pi Setup

```bash
# Install Python dependencies
pip install spidev paho-mqtt

# Configure WiFi and enable SPI
sudo raspi-config
```

### 7.2 LoRa Receiver (Python)

```python
# lora_receiver.py
import spidev
from sx1262 import SX1262

def main():
    lora = SX1262(spi_bus=0, spi_device=0)
    lora.receive_mode()

    while True:
        packet = lora.recv(timeout_ms=1000)
        if packet:
            parsed = parse_packet(packet)
            publish_to_mqtt(parsed)

def parse_packet(data):
    # Unpack 23-byte binary packet
    device_id, lat, lon, alt, battery, flags, ts = struct.unpack('<IiiIHBI', data)
    return {...}

def publish_to_mqtt(data):
    client = mqtt.Client()
    client.connect("hivemq-broker.local")
    client.publish(f"pettracker/{data['device_id']}/location", json.dumps(data))
```

### 7.3 MQTT Integration

- Connect to HiveMQ broker (configurable address)
- Publish location data to topic: `pettracker/{device_id}/location`
- Subscribe to commands: `pettracker/{device_id}/command`

### 7.4 Optional ACK

Send LoRa ACK back to tracker after successful MQTT publish.

### 7.5 Flask Web UI

```python
# app.py
from flask import Flask, jsonify, request, render_template
import sqlite3

app = Flask(__name__)

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/api/devices')
def get_devices():
    # Return list of registered devices

@app.route('/api/devices/<device_id>/history')
def get_history(device_id):
    # Return location history from SQLite

@app.route('/api/config', methods=['GET', 'POST'])
def config():
    # Get/set WiFi, MQTT, LoRa settings

@app.route('/api/geofences', methods=['GET', 'POST', 'DELETE'])
def geofences():
    # Manage geofence zones
```

**Frontend files** (templates/):
- `templates/index.html` — Dashboard with Leaflet map
- `templates/device.html` — Device detail view
- `templates/config.html` — Configuration form
- `static/css/style.css` — Minimal styling
- `static/js/app.js` — Map updates, MQTT subscription

**Day 12**: Flask setup, basic routes, SQLite schema
**Day 13**: Frontend HTML/JS, Leaflet map integration
**Day 14**: MQTT live updates, geofence management UI

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
| 6 | NVS config storage | Pending |
| 7 | Motion-aware sleep | Pending |
| 8 | Geofencing | Pending |
| 9 | Integration testing | Pending |
| 10 | Base station (Pi + Python) | Pending |
| 11 | Flask Web UI | Pending |

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

---

## TODO

- [x] Add Doxygen docstrings to all public APIs (LoRaDriver, Gps, ButtonHandler, LedDriver, etc.)
- [x] Add unit tests for LoRa driver with SPI mock
- [x] Expand LoRa driver tests: packet TX/RX, error conditions, state transitions, timeout scenarios
- [x] Add interrupt-driven DIO handling for LoRa (replace polling with FreeRTOS event group)

---

## Open Questions

All questions have been answered. See "Key Decisions Made" above.
