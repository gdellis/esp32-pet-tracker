# Pet Tracker Implementation Plan

## Overview

This plan outlines the implementation order for the ESP32 pet tracker firmware. The goal is to build incrementally, starting with bare metal hardware access and adding complexity layer by layer.

---

## Phase 1: Project Scaffold

### 1.1 Initialize Rust + ESP-IDF Project

```bash
# Create project with ESP-IDF
cargo generate esp-rust/esp-idf-template pet-tracker
cd pet-tracker

# Add dependencies to Cargo.toml
# - esp-idf-svc (WiFi/BLE services)
# - esp-idf-hal (hardware abstractions)
# - lora-phy (LoRa driver - supports SX1262 via sx1262 variant)
# - embedded-nmea0183 (GPS parsing)
# - lis3dh (accelerometer driver)
# - esp-idf-nvs (configuration storage)
```

### 1.2 Verify Build Pipeline

- [ ] Build for ESP32S3 target: `cargo build --release --target xtensa-esp32s3-elf`
- [ ] Flash to device: `espflash flash /dev/ttyUSB0 --monitor`
- [ ] Verify basic "hello world" via `log::info!` output

### 1.3 Create Module Structure

```
src/
├── main.rs           # Entry point
├── lib.rs            # Shared types
├── gps.rs            # GPS module
├── lora/
│   ├── mod.rs
│   └── sx1262.rs
├── power.rs          # Sleep/wake
├── accelerometer.rs  # LIS3DH I2C driver
├── ble.rs            # BLE GATT server
├── geofence.rs
├── config.rs         # NVS
└── error.rs
```

---

## Phase 2: Hardware Drivers

### 2.1 GPIO (Day 1)

**Goal**: Control LEDs, read button

```rust
// gpio.rs
pub fn init() -> Result<()>;
pub fn set_led(state: LedState) -> Result<()>;
pub fn is_button_pressed() -> bool;
```

**Test**: LED blinks, button press detected

### 2.2 Power Management (Day 1–2)

**Goal**: Enter/exit deep sleep, configure RTC wake timer

```rust
// power.rs
pub fn init() -> Result<()>;
pub fn enter_deep_sleep(duration_ms: u32) -> !;
pub fn wake_reason() -> WakeReason;
pub fn get_battery_voltage() -> u16;  // mV via ADC
```

**Test**: Device sleeps for N seconds, wakes, reports wake reason

### 2.3 Accelerometer LIS3DH (Day 2)

**Goal**: Motion detection via I2C, wake on movement

```rust
// accelerometer.rs
pub fn init() -> Result<Lis3dh>;
pub fn enable_motion_interrupt(threshold_mg: u16) -> Result<()>;
pub fn clear_interrupt() -> Result<()>;
pub fn read_motion() -> Result<MotionData>;
```

**Dependencies**: `lis3dh` crate for driver

**Test**: LIS3DH triggers interrupt on movement

### 2.4 GPS Module (Day 2–3)

**Goal**: Parse NMEA sentences, extract lat/lon/altitude

```rust
// gps.rs
pub fn init() -> Result<GpsDevice>;
pub fn power_on(&mut self) -> Result<()>;
pub fn power_off(&mut self) -> Result<()>;
pub fn read_fix(&mut self, timeout_ms: u32) -> Result<GpsData>;
```

**Dependencies**: `embedded-nmea0183` crate for parsing

**Test**:
- GPS powered on, receives NMEA on UART
- Fix extracted (timeout after 60s returns `valid: false`)

### 2.5 LoRa SX1262 (Day 3–5)

**Goal**: Initialize SX1262, send/receive packets

```rust
// lora/sx1262.rs
pub fn init(spi: SPI, pins: GpioPins) -> Result<Sx1262Device>;
pub fn send(&mut self, packet: &[u8]) -> Result<()>;
pub fn recv(&mut self, buffer: &mut [u8], timeout_ms: u32) -> Result<usize>;
pub fn sleep(&mut self) -> Result<()>;
pub fn wake(&mut self) -> Result<()>;
```

**Dependencies**: `lora-phy` crate

**Test**:
- TX: Send packet, verify received on base station
- RX: Listen for packet with timeout
- Sleep: Verify low current draw

### 2.6 BLE GATT Server (Day 5)

**Goal**: Serve location data to phone via BLE

```rust
// ble.rs
pub fn init() -> Result<()>;
pub fn set_location_data(data: &GpsData, battery_mv: u16);
pub fn start_advertising() -> Result<()>;
```

**GATT Service**: Custom service with characteristics for:
- Location (lat, lon, alt, valid)
- Battery voltage
- Device status

**Test**: Phone connects via BLE, reads location characteristic

---

## Phase 3: State Machine

### 3.1 Basic State Machine (Day 5–6)

Wake sources: RTC timer, LIS3DH motion interrupt, button press

```rust
enum State {
    DeepSleep,
    Idle,
    AcquiringGps,
    Transmitting,
    BleAdvertising,  // BLE fallback when in range
}

fn run_state_machine() {
    loop {
        match current_state {
            DeepSleep => {
                let reason = wait_for_wake();
                current_state = handle_wake(reason);
            }
            Idle => {
                // Check if BLE is in range (optional connection from phone)
                if ble::has_pending_connection() {
                    current_state = BleAdvertising;
                } else {
                    current_state = AcquiringGps;
                }
            }
            AcquiringGps => {
                match gps.read_fix(GPS_TIMEOUT_MS) {
                    Ok(data) => current_state = Transmitting(data),
                    Err(GpsTimeout) => current_state = Transmitting(GpsData::invalid()),
                }
            }
            Transmitting(data) => {
                // Try LoRa TX to base station
                let result = lora.send(data);
                match result {
                    Ok(()) => enter_deep_sleep(SLEEP_INTERVAL_MS),
                    Err(LoraTxFailed) => {
                        // Fallback: try BLE if in range
                        if ble::is_connected() {
                            ble::notify_location(data);
                        }
                        enter_deep_sleep(SLEEP_INTERVAL_MS);
                    }
                }
            }
            BleAdvertising => {
                // Phone is connected via BLE, serve location data
                ble::wait_for_disconnect();
                enter_deep_sleep(SLEEP_INTERVAL_MS);
            }
        }
    }
}
```

### 3.2 Motion-Aware Sleep (Day 6)

When LIS3DH detects motion after stationary period, wake immediately.

```rust
fn determine_sleep_duration(is_moving: bool) -> u32 {
    if is_moving {
        config.sleep_interval_ms  // Normal interval when moving
    } else {
        config.stationary_interval_ms  // Longer interval when stationary
    }
}
```

### 3.3 Retry Logic (Day 6)

- On TX failure: retry up to 3 times with exponential backoff
- On GPS timeout: transmit with `valid=false` flag set
- On TX failure + BLE in range: fallback to BLE notification

---

## Phase 4: Configuration & Storage

### 4.1 NVS Configuration (Day 7)

```rust
// config.rs
pub struct TrackerConfig {
    pub device_id: u32,
    pub sleep_interval_ms: u32,
    pub stationary_interval_ms: u32,
    pub tx_power: u8,
    pub sf: u8,
}

pub fn load_config() -> Result<TrackerConfig>;
pub fn save_config(config: &TrackerConfig) -> Result<()>;
```

**Stored keys**:
- `device_id` (u32)
- `sleep_interval_ms` (u32)
- `stationary_interval_ms` (u32)
- `tx_power` (u8)
- `sf` (u8)

### 4.2 Geofence Storage (Day 7–8)

```rust
pub struct Geofence {
    pub name: String,
    pub zone_type: ZoneType,  // Circle or Polygon
    pub center_lat: i32,
    pub center_lon: i32,
    pub radius_m: u32,  // for Circle
    pub vertices: Vec<(i32, i32)>,  // for Polygon
}

pub fn load_geofences() -> Result<Vec<Geofence>>;
pub fn save_geofences(geofences: &[Geofence]) -> Result<()>;
```

---

## Phase 5: Geofencing

### 5.1 Point-in-Circle (Day 8)

```rust
fn check_geofence_circle(
    lat: i32, lon: i32,
    center_lat: i32, center_lon: i32,
    radius_m: u32
) -> bool {
    let distance = haversine_distance(lat, lon, center_lat, center_lon);
    distance <= radius_m
}
```

### 5.2 Point-in-Polygon (Day 8–9)

Ray casting algorithm for polygon zones.

---

## Phase 6: Integration & Testing

### 6.1 Integration Tests

- [ ] Full cycle: wake → GPS fix → LoRa TX → deep sleep
- [ ] GPS timeout: wake → 60s no fix → TX with invalid flag → deep sleep
- [ ] TX failure: wake → GPS fix → TX fails → retry → success → deep sleep
- [ ] Button wake: wake immediately on button press
- [ ] Motion wake: LIS3DH triggers wake on movement
- [ ] Motion sleep: Device enters longer sleep interval when stationary
- [ ] BLE fallback: TX fails → BLE available → notify via BLE

### 6.2 Power Measurement

- [ ] Measure deep sleep current (target: <10µA including LIS3DH)
- [ ] Measure GPS acquisition current (target: ~25mA)
- [ ] Measure LoRa TX current (target: ~120mA)
- [ ] Measure LIS3DH low-power mode current (~3µA)
- [ ] Verify battery life estimate matches design

### 6.3 Range Test

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
    # Unpack 20-byte binary packet
    device_id, lat, lon, alt, battery, flags, ts = struct.unpack('<IiiHHBI', data)
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

| Day | Module | Milestone |
|-----|--------|-----------|
| 1 | Project scaffold, GPIO, LED | Blink LED, button reads |
| 2 | Power management, deep sleep, LIS3DH | Device sleeps/wakes, motion detection |
| 3 | GPS NMEA parsing | GPS coordinates extracted |
| 4 | LoRa SX1262 init | SPI communication working |
| 5 | LoRa TX/RX, BLE GATT | Packets sent/received, BLE serves location |
| 6 | State machine | Full wake→GPS→TX→sleep cycle |
| 7 | NVS config storage | Settings persisted |
| 8 | Motion-aware sleep | Longer intervals when stationary |
| 9 | Geofencing | Zones checked |
| 10 | Integration testing | End-to-end works |
| 11 | Base station (Pi + Python) | LoRa→WiFi→HiveMQ |
| 12 | Flask Web UI | Basic routes, SQLite schema |
| 13 | Frontend + Map | HTML/JS, Leaflet map |
| 14 | MQTT live updates | Real-time pet tracking |

---

## Key Decisions Made

- **ESP-IDF version**: Latest stable
- **LoRa driver**: `lora-phy` crate (generic LoRa PHY, supports SX1262)
- **GPS parsing**: `embedded-nmea0183` crate
- **GPS module**: NEO-6M (25 mA, cheap, reliable)
- **LoRa frequency**: 915 MHz (US region)
- **BLE fallback**: Included for direct phone connectivity when in range
- **Accelerometer**: LIS3DH for motion detection (wake on movement)
- **Base station**: Python on Raspberry Pi with LoRa hat
- **MQTT broker**: HiveMQ (self-hosted)
- **WiFi config**: AP mode on base station for easy configuration
- **Waterproofing**: Silicone gasket between case halves

---

## Open Questions

All questions have been answered. See "Key Decisions Made" above.
