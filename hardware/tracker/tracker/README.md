# Pet Tracker KiCad Project

KiCad 8 project for the pet tracker hardware.

## Project Files

```
tracker/
├── tracker.kicad_pro   # Project file (open this in KiCad)
├── tracker.kicad_sch  # Schematic (placeholder - see Setup)
├── tracker.kicad_pcb  # PCB layout (to be created)
└── sym/               # Custom symbol library (to be created)
    └── tracker.kicad_sym
```

## Components

| Ref   | Component         | Footprint                       | Description                    |
|-------|-------------------|---------------------------------|--------------------------------|
| U1    | XIAO ESP32S3      | SeeedStudio:XIAO-ESP32S3       | Main MCU                       |
| U2    | Wio-SX1262        | SeeedStudio:Wio-SX1262          | LoRa radio module (stacks on U1) |
| U3    | NEO-6M GPS        | GPS:GPS-NEO-6M                 | GPS module                     |
| U4    | LIS3DH            | Package_LGA:LIS3DH             | Accelerometer (I2C)            |
| U5    | MCP1700-3302E     | Package_TO_SOT_SMD:SOT-23-3    | 3.3V LDO regulator             |
| U6    | TP4056            | Package_SO:TSSOP-8             | LiPo battery charger           |
| Q1    | BSS138            | Package_TO_SOT_SMD:SOT-23      | N-channel MOSFET (GPS power)   |
| BT1   | LiPo 500mAh       | JST_PH_B2B-PH-K               | Battery connector              |
| R1,R2 | 10kΩ              | Resistor_SMD:R_0603_1608Metric | Voltage divider for ADC        |
| R3    | 470Ω              | Resistor_SMD:R_0603_1608Metric  | LED current limit              |
| C1    | 10µF              | Capacitor_SMD:C_0603_1608Metric | Decoupling                     |
| D1    | Status LED        | Device:LED_0603                | Power/status indicator         |
| J1    | USB-C             | Connector_USB:USB_C_Receptacle  | Charging port                  |

## Pin Connections

### XIAO ESP32S3 → GPS (UART)

| XIAO Pin     | GPS Module | Signal        |
|--------------|------------|---------------|
| D4 (GPIO20)  | TX        | GPS data out  |
| D5 (GPIO21)  | RX        | GPS data in   |
| 3V3          | VCC       | Power         |
| GND          | GND       | Ground        |

### XIAO ESP32S3 → LIS3DH (I2C)

| XIAO Pin     | LIS3DH    | Signal        |
|--------------|-----------|---------------|
| D0 (GPIO0)   | INT1      | Motion interrupt |
| D1 (GPIO1)   | SDA       | I2C data      |
| D2 (GPIO2)   | SCL       | I2C clock     |
| 3V3          | VCC       | Power         |
| GND          | GND       | Ground        |

### XIAO ESP32S3 → LoRa (SPI via stacking connector)

The Wio-SX1262 stacks directly on the XIAO via board-to-board connector. No wiring needed.

### Power Circuit

```
LiPo (3.7V) → TP4056 (USB-C charging) → MCP1700 (3.3V LDO) → XIAO + LoRa
                                                            ↓
                                          GPIO-controlled MOSFET → GPS module
                                                            ↓
                                          Voltage Divider (10k+10k) → ADC
```

## Setup

1. Initialize submodules (OPL library for Seeed Studio components):

   ```bash
   git submodule update --init --recursive
   ```

2. Open `tracker.kicad_pro` in KiCad 8+

3. Configure library paths if not already set:
   - **Symbol Libraries**: Add `../../third_party/OPL_Kicad_Library/Seeed Studio XIAO ESP32S3/lib/`
   - **Footprint Libraries**: Add `../../third_party/OPL_Kicad_Library/Seeed Studio XIAO ESP32S3.pretty/`

4. Add components to schematic from libraries

5. Wire up connections per pin table above

6. Run ERC (Electrical Rules Check)

7. Switch to PCB Editor, import netlist, place components

8. Route traces and run DRC

## Power Budget

| State          | Current | Duration | Charge (500mAh) |
|----------------|---------|----------|-----------------|
| Deep sleep     | ~10µA   | ~55 min  | 0.0006 mAh     |
| GPS acquire    | ~25mA   | 30 s     | 0.208 mAh       |
| LoRa TX        | ~120mA  | 2 s      | 0.067 mAh       |
| **Per cycle**  |         | ~60s     | **0.276 mAh**   |

**Estimated battery life**: ~1.25 days at 1-minute intervals with motion-triggered wake extending to ~5 days.

## Design Files Reference

- Seeed Studio XIAO ESP32S3: https://wiki.seeedstudio.com/XIAO-ESP32S3/
- Wio-SX1262 for XIAO: https://wiki.seeedstudio.com/Wio-SX1262/
- Schematic diagram: Generated via Mermaid in DESIGN.md

## Notes

- The Wio-SX1262 stacks on XIAO - no direct wiring for LoRa connections
- GPS is powered via GPIO-controlled MOSFET for deep sleep current optimization
- LIS3DH INT1 connected to GPIO0 for motion wake detection
- Battery voltage monitoring via ADC through 10kΩ+10kΩ voltage divider
