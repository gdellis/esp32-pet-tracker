# Targets

## Flashing

In the root of the generated project:

```shell
espflash flash target/<mcu-target>/debug/<your-project-name>
```

| MCU      | Target                    |
| -------- | ------------------------- |
| ESP32    | `xtensa-esp32-espidf`     |
| ESP32-S2 | `xtensa-esp32s2-espidf`   |
| ESP32-S3 | `xtensa-esp32s3-espidf`   |
| ESP32-C2 | `riscv32imc-esp-espidf`   |
| ESP32-C3 | `riscv32imc-esp-espidf`   |
| ESP32-C6 | `riscv32imac-esp-espidf`  |
| ESP32-H2 | `riscv32imac-esp-espidf`  |
| ESP32-P4 | `riscv32imafc-esp-espidf` |
