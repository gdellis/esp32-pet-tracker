#pragma once

/**
 * @brief Board pin configuration for ESP32-S3 and ESP32-C6
 *
 * Pin assignments for:
 * - Status LED
 * - Push button
 * - GPS UART (TX/RX)
 * - LoRa SX1262 SPI (MOSI/MISO/SCK/NSS/RESET/BUSY/DIO1)
 */

#include <sdkconfig.h>

/** @brief Task watchdog timeout in milliseconds */
#define BOARD_WDT_TIMEOUT_MS 5000

#if defined(CONFIG_IDF_TARGET_ESP32S3)
#include <driver/gpio.h>

/** @brief Status LED GPIO */
#define BOARD_LED_PIN GPIO_NUM_40
/** @brief Push button GPIO (active-low, pulled high internally) */
#define BOARD_BUTTON_PIN GPIO_NUM_9
/** @brief GPS UART TX GPIO */
#define BOARD_GPS_TX_PIN GPIO_NUM_7
/** @brief GPS UART RX GPIO */
#define BOARD_GPS_RX_PIN GPIO_NUM_15
/** @brief GPS power control GPIO (active-high MOSFET) */
#define BOARD_GPS_POWER_PIN GPIO_NUM_21
/** @brief LoRa MOSI GPIO */
#define BOARD_LORA_MOSI_PIN GPIO_NUM_4
/** @brief LoRa MISO GPIO */
#define BOARD_LORA_MISO_PIN GPIO_NUM_5
/** @brief LoRa SCK GPIO */
#define BOARD_LORA_SCK_PIN GPIO_NUM_6
/** @brief LoRa NSS (chip select) GPIO */
#define BOARD_LORA_NSS_PIN GPIO_NUM_8
/** @brief LoRa reset GPIO */
#define BOARD_LORA_RESET_PIN GPIO_NUM_1
/** @brief LoRa busy indicator GPIO */
#define BOARD_LORA_BUSY_PIN GPIO_NUM_2
/** @brief LoRa DIO1 interrupt GPIO */
#define BOARD_LORA_DIO1_PIN GPIO_NUM_3
/** @brief LIS3DH interrupt GPIO */
#define BOARD_ACCEL_INT_PIN GPIO_NUM_0
/** @brief LIS3DH I2C SDA GPIO */
#define BOARD_ACCEL_SDA_PIN GPIO_NUM_11
/** @brief LIS3DH I2C SCL GPIO */
#define BOARD_ACCEL_SCL_PIN GPIO_NUM_12
/** @brief Battery voltage ADC GPIO (100k/100k divider) */
#define BOARD_BATTERY_ADC_PIN GPIO_NUM_10

#elif defined(CONFIG_IDF_TARGET_ESP32C6)
#include <driver/gpio.h>

/** @brief Status LED GPIO */
#define BOARD_LED_PIN		  GPIO_NUM_8
/** @brief Push button GPIO (active-low, pulled high internally) */
#define BOARD_BUTTON_PIN	  GPIO_NUM_9
/** @brief GPS UART TX GPIO */
#define BOARD_GPS_TX_PIN	  GPIO_NUM_4
/** @brief GPS UART RX GPIO */
#define BOARD_GPS_RX_PIN	  GPIO_NUM_5
/** @brief GPS power control GPIO (active-high MOSFET) */
#define BOARD_GPS_POWER_PIN	  GPIO_NUM_18
/** @brief LoRa MOSI GPIO */
#define BOARD_LORA_MOSI_PIN	  GPIO_NUM_6
/** @brief LoRa MISO GPIO */
#define BOARD_LORA_MISO_PIN	  GPIO_NUM_7
/** @brief LoRa SCK GPIO */
#define BOARD_LORA_SCK_PIN	  GPIO_NUM_8
/** @brief LoRa NSS (chip select) GPIO */
#define BOARD_LORA_NSS_PIN	  GPIO_NUM_10
/** @brief LoRa reset GPIO */
#define BOARD_LORA_RESET_PIN  GPIO_NUM_11
/** @brief LoRa busy indicator GPIO */
#define BOARD_LORA_BUSY_PIN	  GPIO_NUM_3
/** @brief LoRa DIO1 interrupt GPIO */
#define BOARD_LORA_DIO1_PIN	  GPIO_NUM_1
/** @brief LIS3DH interrupt GPIO */
#define BOARD_ACCEL_INT_PIN	  GPIO_NUM_12
/** @brief LIS3DH I2C SDA GPIO */
#define BOARD_ACCEL_SDA_PIN	  GPIO_NUM_13
/** @brief LIS3DH I2C SCL GPIO */
#define BOARD_ACCEL_SCL_PIN	  GPIO_NUM_14
/** @brief Battery voltage ADC GPIO (100k/100k divider) */
#define BOARD_BATTERY_ADC_PIN GPIO_NUM_6

#else
#error "No target board specified. Set CONFIG_IDF_TARGET_ESP32S3 or CONFIG_IDF_TARGET_ESP32C6"
#endif
