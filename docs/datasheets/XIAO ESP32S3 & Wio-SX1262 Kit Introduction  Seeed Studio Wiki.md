# XIAO ESP32S3 & Wio-SX1262 Kit Introduction

![XIAO ESP32S3 and Wio-SX1262 Kit](https://files.seeedstudio.com/wiki/XIAO_ESP32S3_for_Meshtastic_LoRa/2.png)

## Introduction

This thumb-sized LoRa development kit, which consists of the XIAO ESP32S3 and
the Wio-SX1262 LoRa module, is an ideal starter kit for Meshtastic and
LoRa/LoRaWAN projects. Supporting WiFi, BLE, and LoRa, it includes a built-in
power management chip and can be extended via IIC, UART, and other GPIO
interfaces, and is compatible with Arduino development.

The XIAO ESP32S3 & Wio-SX1262 Kit is a single-channel LoRaWAN LoRa module with
a low price and high cost performance. It is suitable for application
scenarios with long distance, low power consumption, and small data volume. It
supports more than 30 node devices (depending on upload interval, data
payload, etc.).

Seeed provides 3 applications for this XIAO ESP32S3 & Wio-SX1262 Kit.

- Using Meshtastic for network communication
- Configuring as LoRaWAN node
- Configuring as LoRaWAN gateway

## Features

- **Meshtactic works out of the box**: Pre-flashed with Meshtastic firmware, it is
  ready to work once powered on.
- **Outstanding RF performance**: Supports LoRa(862-930MHz) 2.4GHz Wi-Fi and BLE
  5.0 dual wireless communication, support 2~5km(LoRa) and 100m+(Wi-Fi/BLE)
  remote communication when connected with U.FL antenna.
- **Thumb-sized Compact Design**: 21 x 18mm, adopting the classic form factor of XIAO, suitable for space-limited projects like wearable devices.
- **Powerful MCU Board**: Incorporate the ESP32S3 32-bit, dual-core, Xtensa processor running at up to 240MHz, mounted multiple development ports, Arduino / MicroPython supported.
- **Elaborate Power Design**: Includes a Type-C USB interface and lithium battery charge management.

## Specification

| Feature | Details |
|---------|---------|
| ESP32S3 | ESP32-S3R8, Xtensa LX7 dual-core, 32-bit processor running up to 240 MHz |
| LoRa | Based on Semtech SX1262, supports 862-930MHz |
| BLE | Bluetooth 5.0, Bluetooth mesh |
| Wi-Fi | Complete 2.4GHz Wi-Fi subsystem |
| Interface (ESP32S3) | 1x UART, 1x IIC, 1x IIS, 1x SPI(connect to SX1262),11x GPIO(PWM), 9x ADC, 1x User LED, 1x Charge LED, 1x Reset button, 1x Boot button |
| Interface (Wio-SX1262) | 1x User button, 1x SPI(connect to ESP32S3) |
| Power | Input voltage (Type-C): 5V, Input voltage (BAT): 4.2V, Charging battery current: 100mA |
| Working Temperature | -40°C ~ 65°C |
| Dimensions | 21 x 17.8mm |

## Hardware Preparation

### Part List

| Item | Quantity |
|------|----------|
| XIAO ESP32S3 | x1 |
| Wio-SX1262 extension board | x1 |
| Wi-Fi antenna | x1 |
| LoRa antenna | x1 |

### Recommended accessories

Compatible with XIAO Expansion Board for adding screens and sensors, and add Grove sensors via the Grove expansion Board.

We also supply two kinds of antennas for this kit to support multifunctional applications.

![Antenna options](https://files.seeedstudio.com/wiki/XIAO_ESP32S3_for_Meshtastic_LoRa/37.png)

note

Original FPC Antenna: Only for **testing**. For reliable requirments, please purchase Antenna 2 additionally.

## Hardware Overview

| Wio-SX1262 extension board diagram |
|-------------------------------------|
| ![Wio-SX1262 diagram](https://files.seeedstudio.com/wiki/XIAO_ESP32S3_for_Meshtastic_LoRa/10.png) |
| XIAO ESP32S3 front indication diagram |
| ![Front indication](https://files.seeedstudio.com/wiki/SeeedStudio-XIAO-ESP32S3/img/front-indication.png) |
| XIAO ESP32S3 back indication diagram |
| ![Back indication](https://files.seeedstudio.com/wiki/SeeedStudio-XIAO-ESP32S3/img/back-indication.png) |
| XIAO ESP32S3 Pin List |
| ![Pin list diagram](https://files.seeedstudio.com/wiki/XIAO_ESP32S3_for_Meshtastic_LoRa/13.png) |

## Video Instruction

[![Watch the video](https://img.youtube.com/vi/AGPZ9xnX3gs/0.jpg)](https://www.youtube.com/watch?v=AGPZ9xnX3gs)

## Resource

- [Wio-SX1262 Module Datasheet](https://files.seeedstudio.com/products/SenseCAP/Wio_SX1262/Wio-SX1262_Module_Datasheet.pdf)
- [Wio-SX1262 Schemetic Diagram](https://files.seeedstudio.com/products/SenseCAP/Wio_SX1262/Schematic_Diagram_Wio-SX1262_for_XIAO.pdf)
- [Wio-SX1262 for XIAO 3D file](https://files.seeedstudio.com/products/SenseCAP/Wio_SX1262/Wio-SX1262_for_XIAO_3D_file.step)

## Tech Support & Product Discussion

Thank you for choosing our products! We are here to provide you with different
support to ensure that your experience with our products is as smooth as
possible. We offer several communication channels to cater to different
preferences and needs.
