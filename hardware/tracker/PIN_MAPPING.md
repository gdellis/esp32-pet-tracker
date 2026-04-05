# Tracker PCB Pin Mapping

## ⚠️ FIRMWARE PINS TAKE PRECEDENCE

The KiCad schematic has some discrepancies with `firmware/main/board_config.h`.
**When completing PCB layout, use the pin assignments from this document.**

---

## Main Controller: XIAO ESP32S3

### ESP32S3 Pin Mapping (from board_config.h)

| Pin | GPIO | Signal | Connected To | Notes |
|-----|------|--------|-------------|-------|
| 1 | 3V3 | VCC | VCC_3V3 | |
| 2 | GND | GND | GND | |
| 3 | GPIO0 | ACCEL_INT | LIS3DH INT1 | Motion interrupt |
| 4 | GPIO1 | ACCEL_SDA | LIS3DH SDA | I2C data |
| 5 | GPIO2 | ACCEL_SCL | LIS3DH SCL | I2C clock |
| 6 | GPIO3 | LORA_DIO1 | SX1262 DIO1 | LoRa interrupt |
| 7 | GPIO4 | GPS_TX | GPS RX | UART from GPS |
| 8 | GPIO5 | GPS_RX | GPS TX | UART to GPS |
| 9 | GPIO6 | LORA_SCK | SX1262 SCK | SPI clock |
| 10 | GPIO7 | LORA_MISO | SX1262 MISO | SPI data in |
| 11 | GPIO8 | LED | Status LED (via R3) | |
| 12 | GPIO9 | LORA_NSS | SX1262 NSS | LoRa chip select |
| 13 | GPIO10 | LORA_RESET | SX1262 RESET | LoRa reset |
| 14 | GPIO18 | LORA_MOSI | SX1262 MOSI | SPI data out |
| 15 | GPIO19 | (USB D-) | USB | |
| 16 | GPIO20 | (USB D+) | USB | |
| 17 | GPIO21 | (available) | | |
| 18 | ADC0 | BATT_ADC | R1/R2 divider | Battery monitoring |
| 19 | SWCLK | (debug) | | |
| 20 | SWDIO | (debug) | | |

### Alternative Pin Mapping (from KiCad schematic)

⚠️ **NOTE**: The KiCad schematic has DIFFERENT pins. Update schematic to match firmware or vice versa.

KiCad schematic uses:
- D4/GPIO20 → GPS_TX (should be GPIO4)
- D5/GPIO21 → GPS_RX (should be GPIO5)
- D6/GPIO6 → SX1262_DIO1
- D7/GPIO7 → (available)
- D8/GPIO15 → SX1262_NSS (should be GPIO9)
- D19/GPIO18 → SPI_MISO (should be GPIO7)
- D21/GPIO8 → SPI_MOSI (should be GPIO18)

**Action Required**: Update KiCad schematic to match board_config.h pins

---

## LoRa Module: Wio-SX1262 (stacks on XIAO)

| Pin | Signal | Connected To | Notes |
|-----|--------|-------------|-------|
| 1 | 3V3 | VCC_3V3 | From XIAO |
| 2 | GND | GND | |
| 3 | MISO | GPIO7 (D7) | SPI data in |
| 4 | MOSI | GPIO18 (D19) | SPI data out |
| 5 | SCK | GPIO6 (D6) | SPI clock |
| 6 | NSS | GPIO9 (D8) | Chip select (active low) |
| 7 | DIO1 | GPIO3 (D3) | TX/RX done interrupt |
| 8 | BUSY | (not used) | |
| 9 | ANT | U.FL connector | 915 MHz antenna |

---

## GPS Module: NEO-6M (or compatible)

| Pin | Signal | Connected To | Notes |
|-----|--------|-------------|-------|
| 1 | VCC | VCC_GPS | **Switched via Q1** |
| 2 | GND | GND | |
| 3 | TX | GPIO5 (GPS_RX) | UART to ESP |
| 4 | RX | GPIO4 (GPS_TX) | UART from ESP |
| 5 | PPS | GPIO0 (ACCEL_INT) | 1PPS time pulse |

---

## Accelerometer: LIS3DH

| Pin | Signal | Connected To | Notes |
|-----|--------|-------------|-------|
| 1 | VCC | VCC_3V3 | |
| 2 | GND | GND | |
| 3 | SDA | GPIO1 | I2C data |
| 4 | SCL | GPIO2 | I2C clock |
| 5 | INT1 | GPIO0 | **Motion wake interrupt** |
| 6 | INT2 | (not used) | |

---

## Power Management

### LDO: MCP1700-3302 (3.3V regulator)

| Pin | Signal | Connected To | Notes |
|-----|--------|-------------|-------|
| 1 | VIN | VBAT (3.7V) | From battery |
| 2 | VOUT | VCC_3V3 | 3.3V output |
| 3 | GND | GND | |

### Charger: TP4056 (USB-C LiPo)

| Pin | Signal | Connected To | Notes |
|-----|--------|-------------|-------|
| 1 | TEMP | (optional) | |
| 2 | PROG | R_PROG | Charge current set |
| 3 | GND | GND | |
| 4 | VCC | VBUS (5V USB) | |
| 5 | BAT | VBAT | To battery + |
| 6 | STDBY | (indicator) | Charge complete |
| 7 | CHRG | (indicator) | Charging active |
| 8 | CE | VBUS | Chip enable |

### Power Switch: BSS138 MOSFET

| Pin | Signal | Connected To | Notes |
|-----|--------|-------------|-------|
| 1 | GATE | **GPIO21** (or any GPIO) | GPS power control |
| 2 | SOURCE | GND | |
| 3 | DRAIN | VCC_GPS | To GPS VCC |

---

## Passive Components

| Ref | Value | Purpose | Notes |
|-----|-------|---------|-------|
| R1 | 10kΩ | Battery divider upper | With R2 = 1/2 divider |
| R2 | 10kΩ | Battery divider lower | Vmax at ADC = 3.3V (2x battery) |
| R3 | 470Ω | LED current limit | For status LED |
| C1 | 10µF | LDO bypass | Near MCP1700 VOUT |
| C2 | 0.1µF | Decoupling | Near each IC VCC pin |

---

## Board Outline & Dimensions

Target: **60mm × 40mm** (2-layer PCB)

Suggested outline:

```
    ┌──────────────────────────────┐
    │                              │
    │   XIAO ESP32S3  ←── Wio-SX1262  │
    │   (bottom left)      (stacked) │
    │                              │
    │   LIS3DH          NEO-6M      │
    │   (top right)      (top left)  │
    │                              │
    │   TP4056          MCP1700     │
    │   (bottom right)   (near center) │
    │                              │
    │   [Ant]  [USB-C]  [Battery]  │
    └──────────────────────────────┘
```

---

## TODO: KiCad Schematic Corrections

1. Change GPS_TX from D4/GPIO20 to D4/GPIO4
2. Change GPS_RX from D5/GPIO21 to D5/GPIO5
3. Update LIS3DH I2C pins to match (SDA=GPIO1, SCL=GPIO2)
4. Add GPS_POWER_CTRL net from GPIO21 to MOSFET gate
5. Verify SPI pin assignments match firmware

## Missing Components in Schematic

1. ❌ BSS138 MOSFET for GPS power control
2. ❌ USB-C connector (TP4056 module includes this)
3. ❌ Battery holder/connector
4. ❌ 0.1µF decoupling capacitors (need to add near each IC)
5. ❌ LED current limit resistor R3
6. ❌ I2C pull-up resistors (4.7kΩ) for LIS3DH if not on module
