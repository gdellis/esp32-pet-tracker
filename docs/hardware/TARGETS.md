# Targets

## Microcontrollers

| MCU      | Architecture | Target Triple           |
| -------- | ----------- | --------------------- |
| ESP32    | Xtensa     | `xtensa-esp32-espidf` |
| ESP32-S2 | Xtensa     | `xtensa-esp32s2-espidf` |
| ESP32-S3 | Xtensa     | `xtensa-esp32s3-espidf` |
| ESP32-C2 | RISC-V     | `riscv32imc-esp-espidf` |
| ESP32-C3 | RISC-V     | `riscv32imc-esp-espidf` |
| ESP32-C6 | RISC-V     | `riscv32imac-esp-espidf` |
| ESP32-H2 | RISC-V     | `riscv32imac-esp-espidf` |

## Current Target

This project targets the **ESP32-C6** with architecture `riscv32imac`.

## Flashing

### Using idf.py (ESP-IDF)

```bash
source ~/.espressif/v6.0/esp-idf/export.sh
idf.py -p /dev/ttyACM0 flash monitor
```

### Using Docker

```bash
docker run --rm -v $(pwd):/workspace -w /workspace espressif/idf:v6.0 \
  sh -c ". /opt/esp/idf/export.sh && idf.py -p /dev/ttyACM0 flash monitor"
```

### Using esptool

```bash
esptool.py --chip esp32c6 -b 460800 --before default-reset \
  --after hard-reset write-flash --flash-mode dio \
  --flash-size 4MB --flash-freq 80m \
  0x0 build/bootloader/bootloader.bin \
  0x8000 build/partition_table/partition-table.bin \
  0x10000 build/esp32-pet-tracker.bin
```
