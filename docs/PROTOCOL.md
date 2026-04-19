# LoRa Packet Protocol Specification

## Overview

This document describes the binary protocol used for LoRa communication between the LoRaPaws32 tracker and the base station. The protocol is designed for efficient transmission over LoRa radio with minimal overhead.

---

## Packet Format

**Total Size:** 23 bytes

| Offset | Size | Type | Field | Description |
|--------|------|------|-------|-------------|
| 0 | 4 | uint32_t | device_id | Unique device identifier |
| 4 | 4 | int32_t | latitude | Latitude in microdegrees (×10⁶) |
| 8 | 4 | int32_t | longitude | Longitude in microdegrees (×10⁶) |
| 12 | 4 | int32_t | altitude | Altitude in centimeters (×100) |
| 16 | 2 | uint8_t | battery | Battery percentage (0-100, 255 = error) |
| 18 | 1 | uint8_t | flags | Status flags |
| 19 | 4 | uint32_t | timestamp | Seconds since device boot |

**Endianness:** Big-endian (network byte order)

---

## Field Details

### device_id (bytes 0-3)

Unique identifier assigned to each tracker device at manufacturing time.

- **Type:** uint32_t
- **Range:** 0 to 4,294,967,295
- **Note:** Stored in config, read from NVS on startup

### latitude (bytes 4-7)

Latitude in microdegrees (not degrees).

- **Type:** int32_t
- **Scale:** ×10⁶ (divide by 1,000,000 to get degrees)
- **Range:** -90,000,000 to 90,000,000 (representing -90° to +90°)
- **Invalid:** 0 when GPS fix is invalid

**Example:** Value `40877512` represents `40.877512°`

### longitude (bytes 8-11)

Longitude in microdegrees (not degrees).

- **Type:** int32_t
- **Scale:** ×10⁶ (divide by 1,000,000 to get degrees)
- **Range:** -180,000,000 to 180,000,000 (representing -180° to +180°)
- **Invalid:** 0 when GPS fix is invalid

**Example:** Value `-74006667` represents `-74.006667°`

### altitude (bytes 12-15)

Altitude above sea level in centimeters.

- **Type:** int32_t
- **Scale:** ×100 (divide by 100 to get meters)
- **Range:** -500 to 11,000 (representing -5m to 110m)
- **Invalid:** 0 when GPS fix is invalid

**Example:** Value `15423` represents `154.23m`

### battery (bytes 16-17)

Battery charge percentage.

- **Type:** uint8_t
- **Range:** 0-100 (percentage)
- **Error:** 255 (0xFF) indicates ADC read failure

**Note:** The second byte (offset 17) is padding and should be ignored.

### flags (byte 18)

Status flags indicating GPS fix quality.

| Value | Meaning |
|-------|---------|
| 0x00 | Invalid GPS fix |
| 0x01 | Valid GPS fix |

**Bit 0:** Fix valid flag (1 = valid, 0 = invalid)

### timestamp (bytes 19-22)

Time since device boot in seconds.

- **Type:** uint32_t
- **Range:** 0 to 4,294,967,295 (overflows at ~49.7 days)
- **Source:** `esp_timer_get_time() / 1000000`

---

## Coordinate Encoding

Coordinates are scaled to preserve precision:

```python
# Firmware encoding (C++)
lat = latitude * 1000000  # degrees to microdegrees
lon = longitude * 1000000

# Base station decoding (Python)
lat_deg = raw_value / 1000000.0
lon_deg = raw_value / 1000000.0
```

**Precision:**
- Latitude: ~0.000001° (~0.11m at equator)
- Longitude: ~0.000001° (~0.11m at equator, decreases toward poles)

---

## Packet Validation

### Basic Validation

1. **Length check:** Packet must be exactly 23 bytes
2. **Battery range:** byte 16 should be 0-100 or 255
3. **Flags check:** byte 18 must be 0x00 or 0x01

### GPS Fix Validation

When `flags & 0x01 == 0x01` (valid fix):
- Latitude should be in range -90° to +90°
- Longitude should be in range -180° to +180°
- Altitude should be plausible (-500m to 11000m)

When `flags & 0x01 == 0x00` (invalid fix):
- Latitude, longitude, and altitude are typically 0
- Do not use coordinate values

---

## Implementation

### Firmware Source

**File:** `firmware/main/state_machine.cpp`
**Function:** `TrackerStateMachine::try_lora_send()`

```cpp
esp_err_t TrackerStateMachine::try_lora_send (const GpsData& data, bool valid_fix) {
    uint8_t packet[23];
    memset (packet, 0, sizeof (packet));

    uint32_t device_id = config_.device_id;
    int32_t lat = valid_fix ? (int32_t)(data.latitude * 1000000) : 0;
    int32_t lon = valid_fix ? (int32_t)(data.longitude * 1000000) : 0;
    int32_t alt = valid_fix ? (int32_t)(data.altitude * 100) : 0;
    uint8_t battery = battery_.read_percentage ().value_or (255);
    uint8_t flags = valid_fix ? 0x01 : 0x00;
    uint32_t timestamp = (uint32_t)(esp_timer_get_time () / 1000000);

    memcpy (&packet[0], &device_id, 4);
    memcpy (&packet[4], &lat, 4);
    memcpy (&packet[8], &lon, 4);
    memcpy (&packet[12], &alt, 4);
    memcpy (&packet[16], &battery, 2);
    packet[18] = flags;
    memcpy (&packet[19], &timestamp, 4);

    return lora_.send_blocking (packet, sizeof (packet), 10000);
}
```

### Base Station Source

**File:** `base_station/packet_parser.py`
**Class:** `PacketParser` or similar

---

## Change Log

| Version | Date | Change |
|---------|------|--------|
| 1.0.0 | 2026-04-18 | Initial protocol definition |

---

## See Also

- [Pinout Reference](../hardware/PINOUT.md) - Hardware pin assignments
- `firmware/main/state_machine.cpp` - Firmware implementation
- `base_station/packet_parser.py` - Base station implementation
