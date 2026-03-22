# Base Station KiCad Project

KiCad 8 project for the pet tracker base station carrier board.

## Project Files

```
base_station/
├── base_station.kicad_pro   # Project file (open this in KiCad)
├── base_station.kicad_sch   # Schematic
├── base_station.kicad_pcb   # PCB layout
└── sym/                     # Custom symbol library
    └── base_station.kicad_sym
```

## Components

| Ref | Component | Footprint |
|-----|-----------|-----------|
| U1 | XIAO ESP32S3 | SeeedStudio:XIAO-ESP32S3 |
| U2 | NEO-6M GPS | GPS:GPS-NEO-6M |
| U3 | MCP1700 LDO | Package_TO_SOT_SMD:SOT-23-3 |
| J1 | USB-C | Connector_USB:USB_C_Receptacle |
| J2 | Battery | Connector_JST:JST_PH_B2B-PH-K |
| D1 | Status LED | Device:LED_0603 |
| R1 | LED Resistor (470R) | Resistor_SMD:R_0603_1608Metric |
| C1 | Decoupling Cap (10µF) | Capacitor_SMD:C_0603_1608Metric |

## Pin Connections

| Signal | XIAO Pin | Connection |
|--------|----------|------------|
| GPS_TX | D7 (GPIO6) | GPS RX |
| GPS_RX | D6 (GPIO7) | GPS TX |
| GPS_PPS | D0 (GPIO0) | GPS PPS (optional) |
| LED_STATUS | D10 (GPIO10) | R1 → LED |

## Setup

1. Initialize the OPL submodule:
   ```bash
   git submodule update --init --recursive
   ```

2. Open `base_station.kicad_pro` in KiCad 8+

3. Configure library paths:
   - **Symbol Libraries**: Add `../../third_party/OPL_Kicad_Library/Seeed Studio XIAO ESP32S3/lib/` and `../../third_party/OPL_Kicad_Library/Seeed Studio Wio SX1262 for XIAO ESP32S3/lib/`
   - **Footprint Libraries**: Same paths with `.pretty` suffix

4. Add components from libraries to schematic

5. Run ERC (Electrical Rules Check) and DRC (Design Rules Check)

## Notes

- This is a starting point — verify all connections against component datasheets
- The Wio-SX1262 stacks on XIAO via board-to-board connector (no wiring needed)
- GPS module is powered from XIAO's 3.3V rail
- Battery (3.7V LiPo) → MCP1700 → 3.3V for XIAO
- Board outline: 100mm x 80mm (adjust as needed)
