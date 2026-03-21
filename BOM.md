# Bill of Materials (BOM)

## Tracker BOM

### Main Components

| Qty | Component | Part Number | Est. Cost | Notes |
|-----|-----------|-------------|-----------|-------|
| 1 | Seeed Studio XIAO ESP32S3 | 113991162 | $10.99 | |
| 1 | Wio-SX1262 for XIAO (915 MHz) | 113991554 | $14.90 | |
| 1 | u-blox NEO-6M GPS Module | Various | $8–15 | |
| 1 | LIS3DH Accelerometer | ADA1897 | $1.95 | |
| 1 | LiPo Battery 503035 500mAh | Various | $5–8 | |

### Power Components

| Qty | Component | Part Number | Est. Cost | Notes |
|-----|-----------|-------------|-----------|-------|
| 1 | TP4056 USB-C Charger Module | Various | $0.50 | |
| 1 | MCP1700-3302E LDO (3.3V) | MCP1700-3302E | $0.35 | |
| 1 | BSS138 N-Channel MOSFET | BSS138 | $0.10 | GPS power switch |
| 2 | 10kΩ Resistor (0805) | CRCW080510K0FKEA | $0.02 | Voltage divider |
| 1 | RGB LED 5050 | Various | $0.20 | |

### Hardware & Misc

| Qty | Component | Part Number | Est. Cost | Notes |
|-----|-----------|-------------|-----------|-------|
| 1 | Pin Headers (2.54mm) | PRT-00116 | $0.50 | For stacking |
| 1 | JST-PH 2-Pin Connector | PRT-08612 | $0.20 | Battery connector |
| 1 | Tactile Button (6x6mm) | Various | $0.05 | Reset button |
| 1 | 0.1µF Capacitor (0805) | C0805C104K5RACAU | $0.02 | |
| 1 | 10µF Capacitor (0805) | C0805C106K9PACTU | $0.05 | |

**Estimated Tracker Total: $40–55** (excluding shipping)

---

## Base Station BOM

| Qty | Component | Part Number | Est. Cost | Notes |
|-----|-----------|-------------|-----------|-------|
| 1 | Raspberry Pi Zero 2 W | RASPBERRY PI ZERO 2 W | $15.00 | |
| 1 | SX1262 LoRa HAT for Pi | 102991329 | $15.90 | |
| 1 | USB-C Power Supply 5V 3A | 119–1106 | $8.00 | |
| 1 | SanDisk 32GB microSD | SDSQUA4-32G-GN6MA | $7.00 | |
| 1 | 3D Printed Enclosure | DIY | $5–10 | PLA/PETG |

**Estimated Base Station Total: $50–60** (excluding shipping)

---

## Where to Buy

| Vendor | Best For | Notes |
|--------|----------|-------|
| [Seeed Studio](https://www.seeedstudio.com) | XIAO, Wio-SX1262, LoRa HAT | Good prices, combined shipping |
| [DigiKey](https://www.digikey.com) | Passives, connectors, ICs | Fast shipping, good for small qty |
| [Mouser](https://www.mouserr.com) | Passives, connectors, ICs | Alternative to DigiKey |
| [Adafruit](https://www.adafruit.com) | LIS3DH, Raspberry Pi | Quality products |
| [Amazon](https://www.amazon.com) | NEO-6M, batteries, cables | Fast prime shipping |
| [Pi Shop](https://www.pishop.us) | Raspberry Pi | US-based, good prices |

---

## Alternative Parts

### GPS Modules

| Part | Pros | Cons | Est. Cost |
|------|------|------|-----------|
| NEO-6M (default) | Cheap, reliable, well-documented | Higher power (~25mA), older platform | $8–15 |
| MAX-M10S | Lower power (~15mA), faster fix, M10 platform | More expensive | $15–20 |
| L76B | Integrated, small | Higher power, less common | $8–12 |

### Microcontroller

| Part | Pros | Cons | Est. Cost |
|------|------|------|-----------|
| XIAO ESP32S3 (default) | FPU, BLE, more RAM | Slightly more expensive | $10.99 |
| XIAO ESP32C3 | RISC-V, WiFi/BLE 5.0 | No FPU, less RAM | $7.99 |

### LoRa Modules

| Part | Pros | Cons | Est. Cost |
|------|------|------|-----------|
| Wio-SX1262 (default) | Low power, SPI control | More firmware code | $14.90 |
| LoRa-E5 | AT commands, simpler firmware | Higher sleep current | $12.90 |

---

## Mechanical Parts (3D Printed)

### Enclosure Design Files

Design files to be created in:
- `enclosure/tracker_case_bottom.step`
- `enclosure/tracker_case_top.step`
- `enclosure/gasket_profile.step`

**Print Settings**:
- Material: PETG or ABS
- Layer height: 0.2mm
- Infill: 15–20%
- Walls: 3 perimeters
- No supports needed (bridge acceptable)

**Suggested Dimensions** (verify against actual components):
- Length: ~60mm
- Width: ~40mm  
- Height: ~25mm
- Wall thickness: ~3mm
- Gasket groove: 2mm x 2mm channel

---

## Order List (Copy to Vendor Carts)

### Seeed Studio

- [ ] XIAO ESP32S3 × 1
- [ ] Wio-SX1262 for XIAO (915 MHz) × 1
- [ ] Grove 2mm 2-Pin JST-PH Cable × 1

### DigiKey

- [ ] MCP1700-3302E/TT-ND × 1
- [ ] BSS138LT3GOSDKR-ND × 1
- [ ] CRCW080510K0FKEAHP100DKR-ND × 2
- [ ] 497-1563-1-ND × 1 (0.1µF)
- [ ] 493-12819-1-ND × 1 (10µF)

### Amazon / Other

- [ ] NEO-6M GPS Module with antenna
- [ ] LIS3DH Breakout Board
- [ ] TP4056 USB-C Charger Module
- [ ] LiPo 503035 500mAh
- [ ] 3mm x 10mm silicone gasket strip (1m)

---

## Revision History

| Rev | Date | Changes |
|-----|------|---------|
| 1.0 | 2026-03-20 | Initial BOM |
