# LoRaPaws32 Tracker Pinout Diagram

## Overview

Complete pinout reference for the ESP32-S3 / ESP32-C6 based pet tracker with SX1262 LoRa radio, GPS module, and LIS3DH accelerometer.

---

## ESP32-S3 Pinout (Primary)

### Physical Pin Layout

```
                    ┌─────────────────────────┐
                    │   ESP32-S3-WROOM-1      │
                    │  (Top view, pin side)   │
                    │                         │
    GND   3V3       │  ┌─────────────────┐    │
     1     2        │  │                 │    │
    IO46  IO45      │ ┌─────────────────┐│    │
     3     4        │ │ ESP32-S3 SoC    ││    │
    IO44  IO43      │ │ (chip side)     ││    │
     5     6        │ │                 ││    │
    IO42  IO41      │ └─────────────────┘│    │
     7     8        │                     │    │
    IO40  IO39      │  ┌─────────────────┐    │
     9    10        │  │   Flash/PSRAM   │    │
    IO38  IO37      │  │   8MB each      │    │
    11    12        │  └─────────────────┘    │
    IO36  IO35      │                         │
    13    14        │ ┌─────────────────────┐ │
    IO34  IO33      │ │  Antenna area      │ │
    15    16        │ │  (keep clear)      │ │
    IO32  IO31      │ └─────────────────────┘ │
    17    18        │                         │
    IO30  GND       │                         │
    19    20        │                         │
    IO29  IO28      │ ┌─────────────────────┐ │
    21    22        │ │  USB connector area │ │
    IO27  IO26      │ │  (optional)         │ │
    23    24        │ └─────────────────────┘ │
    IO25  IO24      │                         │
    25    26        │                         │
    IO23  IO22      │                         │
    27    28        │                         │
    IO21  IO20      │                         │
    29    30        │                         │
    IO19  IO18      │                         │
    31    32        │                         │
    IO17  IO16      │                         │
    33    34        │                         │
    IO15  IO14      │                         │
    35    36        │                         │
    IO13  IO12      │                         │
    37    38        │                         │
    IO11  IO10      │                         │
    39    40        │                         │
     IO9   IO8      │                         │
    41    42        │                         │
     IO7   IO6      │                         │
    43    44        │                         │
     IO5   IO4      │                         │
    45    46        │                         │
     IO3   IO2      │                         │
    47    48        │                         │
     IO1   IO0      │                         │
    49    50        │                         │
    5V    GND       │                         │
    51    52        │                         │
                    └─────────────────────────┘
```

### ESP32-S3 Pin Assignments

| GPIO | Function | Peripheral | Connection | Notes |
|------|----------|------------|-----------|-------|
| **GPIO0** | Boot/Download | Button | Reset (active low) | Pulled up internally |
| **GPIO1** | LoRa Reset | SX1262 | Reset line | Active high |
| **GPIO2** | LoRa BUSY | SX1262 | Busy indicator | Read-only |
| **GPIO3** | LoRa DIO1 | SX1262 | TX/RX done interrupt | Edge-triggered |
| **GPIO4** | LoRa MOSI | SPI1 (LoRa) | SPI data out | 3.3V |
| **GPIO5** | LoRa MISO | SPI1 (LoRa) | SPI data in | 3.3V |
| **GPIO6** | LoRa CLK | SPI1 (LoRa) | SPI clock | 3.3V |
| **GPIO7** | GPS RX | UART1 | GPS data in | 3.3V, 115200 baud |
| **GPIO8** | LoRa NSS | SPI1 (LoRa) | Chip select (active low) | 3.3V |
| **GPIO9** | Button | GPIO | Push button wake | Active low, pulled up |
| **GPIO10** | Unused | GPIO | Available | 3.3V |
| **GPIO15** | GPS TX | UART1 | GPS data out | 3.3V, 115200 baud |
| **GPIO16** | Unused | GPIO | Available | 3.3V |
| **GPIO17** | Unused | GPIO | Available | 3.3V |
| **GPIO18** | Unused | GPIO | Available | 3.3V |
| **GPIO19** | Unused | GPIO | Available | 3.3V |
| **GPIO20** | Unused | GPIO | Available | 3.3V |
| **GPIO21** | I2C SDA | I2C0 | LIS3DH + others | Pull-ups required (4.7kΩ) |
| **GPIO22** | I2C SCL | I2C0 | LIS3DH + others | Pull-ups required (4.7kΩ) |
| **GPIO23** | LED (Status) | GPIO | Indicator LED | Active high, 20mA source |
| **GPIO24** | GPS Power | GPIO | MOSFET gate | Active high (powers GPS) |
| **GPIO25** | Unused | GPIO | Available | 3.3V |
| **GPIO26** | Unused | GPIO | Available | 3.3V |
| **GPIO27** | Unused | GPIO | Available | 3.3V |
| **GPIO28** | Unused | GPIO | Available | 3.3V |
| **GPIO29** | Unused | GPIO | Available | 3.3V |
| **GPIO30** | Unused | GPIO | Available | 3.3V |
| **GPIO31** | Unused | GPIO | Available | 3.3V |
| **GPIO32** | Unused | GPIO | Available | 3.3V |
| **GPIO33** | Unused | GPIO | Available | 3.3V |
| **GPIO34** | Unused | GPIO | Available | 3.3V |
| **GPIO35** | Battery Voltage | ADC1 | Battery monitor | Via 1/2 divider |
| **GPIO36** | Unused | ADC1 | Available | 3.3V |
| **GPIO37** | Unused | GPIO | Available | 3.3V |
| **GPIO38** | Unused | GPIO | Available | 3.3V |
| **GPIO39** | Unused | GPIO | Available | 3.3V |
| **GPIO40** | Unused | GPIO | Available | 3.3V |
| **GPIO41** | Unused | GPIO | Available | 3.3V |
| **GPIO42** | Unused | GPIO | Available | 3.3V |
| **GPIO43** | Unused | GPIO | Available | 3.3V |
| **GPIO44** | Unused | GPIO | Available | 3.3V |
| **GPIO45** | Unused | GPIO | Available | 3.3V |
| **GPIO46** | Unused | GPIO | Available | 3.3V |
| **3V3** | Power | Power | 3.3V rail | 500mA typical |
| **GND** | Ground | Power | Ground plane | Multiple pins |

---

## ESP32-C6 Pinout (Alternate)

### Physical Pin Layout

```
                    ┌─────────────────────────┐
                    │   ESP32-C6-WROOM-1C     │
                    │  (Top view, pin side)   │
                    │                         │
    GND   3V3       │  ┌─────────────────┐    │
     1     2        │  │                 │    │
    IO20  IO19      │ ┌─────────────────┐│    │
     3     4        │ │ ESP32-C6 SoC    ││    │
    IO18  IO17      │ │ (RISC-V, 160MHz)││    │
     5     6        │ │                 ││    │
    IO16  IO15      │ └─────────────────┘│    │
     7     8        │                     │    │
    IO14  IO13      │  ┌─────────────────┐    │
     9    10        │  │   Flash/PSRAM   │    │
    IO12  IO11      │  │   4-8MB each    │    │
    11    12        │  └─────────────────┘    │
    IO10   IO9      │                         │
    13    14        │ ┌─────────────────────┐ │
     IO8   IO7      │ │  Antenna area      │ │
    15    16        │ │  (keep clear)      │ │
     IO6   IO5      │ └─────────────────────┘ │
    17    18        │                         │
     IO4   IO3      │                         │
    19    20        │                         │
     IO2   IO1      │ ┌─────────────────────┐ │
    21    22        │ │  USB connector area │ │
     IO0  GND       │ │  (optional)         │ │
    23    24        │ └─────────────────────┘ │
    5V    GND       │                         │
    25    26        │                         │
                    └─────────────────────────┘
```

### ESP32-C6 Pin Assignments

| GPIO | Function | Peripheral | Connection | Notes |
|------|----------|------------|-----------|-------|
| **GPIO0** | Boot/Download | Button | Reset (active low) | Pulled up internally |
| **GPIO1** | LoRa DIO1 | SX1262 | TX/RX done interrupt | Edge-triggered |
| **GPIO2** | LED (Status) | GPIO | Indicator LED | Active high, 20mA source |
| **GPIO3** | LoRa BUSY | SX1262 | Busy indicator | Read-only |
| **GPIO4** | GPS RX | UART0 | GPS data in | 3.3V, 115200 baud |
| **GPIO5** | GPS TX | UART0 | GPS data out | 3.3V, 115200 baud |
| **GPIO6** | LoRa MOSI | SPI2 (LoRa) | SPI data out | 3.3V |
| **GPIO7** | LoRa MISO | SPI2 (LoRa) | SPI data in | 3.3V |
| **GPIO8** | LoRa CLK | SPI2 (LoRa) | SPI clock | 3.3V |
| **GPIO9** | Unused | GPIO | Available | 3.3V |
| **GPIO10** | LoRa NSS | SPI2 (LoRa) | Chip select (active low) | 3.3V |
| **GPIO11** | LoRa Reset | SX1262 | Reset line | Active high |
| **GPIO12** | Unused | GPIO | Available | 3.3V |
| **GPIO13** | Unused | GPIO | Available | 3.3V |
| **GPIO14** | Unused | GPIO | Available | 3.3V |
| **GPIO15** | Unused | GPIO | Available | 3.3V |
| **GPIO16** | Unused | GPIO | Available | 3.3V |
| **GPIO17** | Unused | GPIO | Available | 3.3V |
| **GPIO18** | Unused | GPIO | Available | 3.3V |
| **GPIO19** | Unused | GPIO | Available | 3.3V |
| **GPIO20** | Unused | GPIO | Available | 3.3V |
| **GPIO21** | I2C SDA | I2C0 | LIS3DH + others | Pull-ups required (4.7kΩ) |
| **GPIO22** | I2C SCL | I2C0 | LIS3DH + others | Pull-ups required (4.7kΩ) |

**Note**: GPIO21 and GPIO22 for I2C are not physically available on ESP32-C6 standard pinout. Use GPIO18/GPIO19 or GPIO6/GPIO7 for I2C instead. Update GPIO assignments accordingly.

---

## SX1262 LoRa Radio Pinout

### Physical Package

SX1262 is typically mounted on a breakout board with standard SPI + control pins:

```
                    ┌──────────────────┐
                    │   SX1262 Module  │
                    │   (Top view)     │
      GND (1)  ────── ┌─────────────┐  │
      DCDC (2) ────── │ SX1262 IC   │  │
      GND (3)  ────── │             │  │
      VCC (4)  ────── │   (Exposed  │  │
      GND (5)  ────── │    Pad)     │  │
      ANT (6)  ────── └─────────────┘  │
      GND (7)  │                        │
      MOSI (8) ├─ SPI Interface         │
      MISO (9) ├─ SPI Interface         │
      SCK (10) ├─ SPI Interface         │
      NSS (11) ├─ Chip Select           │
      RESET (12)─ Active High           │
      BUSY (13)─ Status (Read-only)     │
      DIO1 (14)─ Interrupt/Done         │
      DIO2 (15)─ TX/RX Done (alt)       │
      DIO3 (16)─ Unused                 │
      GND (17) │                        │
      GND (18) │                        │
                    └──────────────────┘
```

### SX1262 Pin Connections

| SX1262 Pin | Signal | ESP32-S3 GPIO | ESP32-C6 GPIO | Function | Notes |
|-----------|--------|---------------|---------------|----------|-------|
| 1, 3, 5, 7, 17, 18 | GND | GND | GND | Ground plane | Multi-point |
| 2 | DCDC | VCC (via 0.1µF cap) | VCC (via 0.1µF cap) | Power decoupling | 4.7µF bulk + 0.1µF ceramic |
| 4 | VCC | 3V3 | 3V3 | Power supply | From LDO (3.3V, 500mA) |
| 6 | ANT | U.FL antenna | U.FL antenna | RF antenna | SMA or U.FL connector |
| 8 | MOSI | GPIO4 | GPIO6 | SPI data out | 3.3V, 10MHz typical |
| 9 | MISO | GPIO5 | GPIO7 | SPI data in | 3.3V, 10MHz typical |
| 10 | SCK | GPIO6 | GPIO8 | SPI clock | 3.3V, 10MHz typical |
| 11 | NSS | GPIO8 | GPIO10 | Chip Select | Active low |
| 12 | RESET | GPIO1 | GPIO11 | Reset control | Active high, pull-down |
| 13 | BUSY | GPIO2 | GPIO3 | Status indicator | Read-only, high during TX |
| 14 | DIO1 | GPIO3 | GPIO1 | TX/RX Done | Rising edge interrupt |
| 15 | DIO2 | Unused (alt) | Unused (alt) | Alt interrupt (optional) | Leave unconnected |
| 16 | DIO3 | Unused | Unused | Unused | Leave unconnected |

---

## GPS Module Pinout (HGLRC M100-5883)

### Physical Connector

```
              ┌─────────────────────┐
              │  M100-5883 GPS      │
              │  (4-pin interface)  │
              │                     │
        ┌─────┴─────────────────────┴─────┐
        │  VCC  TX  RX  GND              │
        │  (1)  (2) (3) (4)              │
        └───────────────────────────────┘
```

### GPS Pin Connections

| GPS Pin | Signal | ESP32-S3 GPIO | ESP32-C6 GPIO | Function | Voltage | Notes |
|---------|--------|---------------|---------------|----------|---------|-------|
| 1 | VCC | GPIO24 (via MOSFET) | GPIO24 (via MOSFET) | Power control | 3.3V | Power switch (GPIO-controlled) |
| 2 | TX | GPIO15 | GPIO5 | GPS data output | 3.3V | RX on ESP32 (UART) |
| 3 | RX | GPIO7 | GPIO4 | GPS data input | 3.3V | TX on ESP32 (UART) |
| 4 | GND | GND | GND | Ground | 0V | Ground plane |

### GPS UART Configuration

- **Baud rate**: 115200 (standard M100 default)
- **Data bits**: 8
- **Stop bits**: 1
- **Parity**: None (8N1)
- **Flow control**: None
- **Protocol**: NMEA 0183

**NMEA Sentences received:**
- `$GPRMC` - Recommended Minimum Navigation Information
- `$GPGGA` - Global Positioning System Fix Data
- `$GPGSA` - GPS DOP and Active Satellites
- `$GPGSV` - GPS Satellites in View

---

## LIS3DH Accelerometer Pinout

### Physical Package

LIS3DH is typically mounted on breakout board with I2C interface:

```
                    ┌──────────────────┐
                    │  LIS3DH Accel    │
                    │  (SMD LGA-14)    │
                    │                  │
        ┌───────────┼──────────────────┼──────────┐
        │           │ (Top view)       │          │
        │  VCC      │                  │  GND     │
        │  SCL      │                  │  SDA     │
        │  INT1     │  [LIS3DH IC]     │  INT2    │
        │  CS (I2C) │  (GND on bottom) │  SD0     │
        └───────────┴──────────────────┴──────────┘
```

### LIS3DH Pin Connections

| LIS3DH Pin | Signal | ESP32-S3 GPIO | ESP32-C6 GPIO | Function | Voltage | Notes |
|-----------|--------|---------------|---------------|----------|---------|-------|
| VCC | Power | 3V3 | 3V3 | Power supply | 3.3V | Via 0.1µF cap |
| GND | Ground | GND | GND | Ground plane | 0V | Multi-point |
| SCL | I2C clock | GPIO22 | GPIO22 | I2C clock | 3.3V | Pull-up 4.7kΩ |
| SDA | I2C data | GPIO21 | GPIO21 | I2C data | 3.3V | Pull-up 4.7kΩ |
| INT1 | Interrupt 1 | GPIO18 (optional) | GPIO18 (optional) | Motion detected | 3.3V | Edge-triggered, active high |
| INT2 | Interrupt 2 | GPIO19 (optional) | GPIO19 (optional) | Alternative interrupt | 3.3V | Unused (optional) |
| CS | Chip Select | VCC (pulled high) | VCC (pulled high) | I2C mode select | 3.3V | Tie to VCC for I2C |
| SD0 | I2C Addr LSB | GND (or VCC) | GND (or VCC) | Address select | 0V or 3.3V | 0x18 (GND) or 0x19 (VCC) |

### I2C Configuration

- **Address (SD0 → GND)**: 0x18
- **Address (SD0 → VCC)**: 0x19
- **I2C clock speed**: 400 kHz (standard)
- **Default address**: 0x18 (SD0 pulled to GND)

---

## Power Distribution Schematic

```
                    ┌─────────────────────┐
                    │   LiPo Battery      │
                    │   3.7V nominal      │
                    │   500-1000 mAh      │
                    └──────────┬──────────┘
                               │
                    ┌──────────┴──────────┐
                    │  TP4056 Charger     │
                    │  (USB or 5V input)  │
                    └──────────┬──────────┘
                               │ (Vbat)
                    ┌──────────┴──────────┐
                    │  Power Distribution │
                    │                     │
        ┌───────────┼───────────┬─────────┴────────┐
        │           │           │                  │
        │    ┌──────▼─────┐    │         ┌────────▼────────┐
        │    │  MCP1700   │    │         │ Voltage Divider │
        │    │  3.3V LDO  │    │         │  (for ADC mon)  │
        │    └──────┬─────┘    │         └────────┬────────┘
        │           │          │                  │
   ┌────▼──────┬────▼───┬──────▼───────┬─────────▼──────┐
   │            │        │              │                │
ESP32-S3/C6  SX1262   LIS3DH       LED + Button     GPS (via MOSFET)
  3V3 rail   Power    Power         Power            Via GPIO24 switch
  500mA      ~120mA   ~3mA          ~5mA             ~25mA (active)
```

### Power Budget

| Component | State | Current | Duration | Notes |
|-----------|-------|---------|----------|-------|
| ESP32-S3/C6 | Deep sleep | 10-20 µA | ~55 min | CPU off, RTC running |
| SX1262 | Sleep | ~1 µA | idle | Very low standby |
| LIS3DH | Low-power mode | ~3 µA | idle | Motion detection enabled |
| LED | Off | 0 µA | - | No current |
| GPS | Off | 0 µA | - | Powered off via MOSFET |
| **Total deep sleep** | | **~30 µA** | - | **Very efficient** |
| GPS | Acquiring fix | 25 mA | 30 s | Active acquisition |
| SX1262 | Transmitting | 120 mA | 2 s | TX @ +17 dBm |
| **Per cycle** | 60 s | ~0.28 mAh | - | ~401 mAh/day @ 1Hz |

With 500 mAh battery and 5-minute idle intervals: ~5 days typical battery life

---

## Board-Level Connections

### SPI Bus (LoRa)

```
ESP32        SX1262
─────        ──────
GPIO4 (MOSI) ──→ MOSI (pin 8)
GPIO5 (MISO) ←── MISO (pin 9)
GPIO6 (SCK)  ──→ SCK (pin 10)
GPIO8 (NSS)  ──→ NSS (pin 11, active low)
GPIO1 (Reset)──→ RESET (pin 12)
GPIO2 (BUSY) ←── BUSY (pin 13)
GPIO3 (DIO1) ←── DIO1 (pin 14)
GND          ──→ GND (pins 1, 3, 5, 7, 17, 18)
```

### UART Bus (GPS)

```
ESP32-S3     GPS Module
────────     ──────────
GPIO7 (RX)   ←── TX (pin 2)
GPIO15 (TX)  ──→ RX (pin 3)
GND          ──→ GND (pin 4)
GPIO24 (PWR) ──→ MOSFET gate (controls VCC to pin 1)
```

### I2C Bus (LIS3DH + future sensors)

```
ESP32-S3/C6      LIS3DH
─────────────    ──────
GPIO22 (SCL)  ──→ SCL (pin, via 4.7kΩ pull-up)
GPIO21 (SDA)  ──→ SDA (pin, via 4.7kΩ pull-up)
GND           ──→ GND
3V3           ──→ VCC
              ──→ CS (tied to VCC for I2C mode)
              ──→ SD0 (GND for addr 0x18, VCC for 0x19)
```

---

## Troubleshooting

### LoRa SPI Issues

- **No response from SX1262**: Check NSS (chip select) toggling during SPI
- **DIO1 not triggering**: Verify GPIO3 is connected, edge interrupt configured
- **TX fails silently**: Check BUSY line, ensure RESET pulse sent on init

### GPS No Fix

- **No NMEA sentences**: Check UART baud rate (115200), GPIO7/15 connections
- **GPS timeout**: Increase GPS_TIMEOUT_MS if in urban canyon
- **No power**: Check GPIO24 MOSFET is switching, verify battery voltage

### I2C Accelerometer

- **Address mismatch**: Verify SD0 pin connection (0x18 vs 0x19)
- **No INT1 interrupt**: Check GPIO18 connection, INT1 enable bit in LIS3DH config
- **Wrong data**: Check endianness in firmware (16-bit reads)

---

## Files Referenced

- `board_config.h` - Pin definitions for both ESP32-S3 and ESP32-C6
- `gpio_driver.cpp` - GPIO initialization and control
- `sx1262.cpp` - SX1262 SPI driver and pin handling
- `gps.cpp` - GPS UART configuration and polling
