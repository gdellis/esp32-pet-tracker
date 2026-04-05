#pragma once

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/timers.h"
#include <functional>
#include <stdint.h>

#define LORA_DEFAULT_SPI_FREQ		  1000000
#define LORA_DEFAULT_TX_POWER		  22
#define LORA_DEFAULT_FREQ			  915000000
#define LORA_MAX_PACKET_SIZE		  255
#define LORA_DEFAULT_SPREADING_FACTOR 7
#define LORA_DEFAULT_PREAMBLE_LENGTH  8

#define LORA_EVENT_TX_DONE_BIT	  (1 << 0)
#define LORA_EVENT_RX_DONE_BIT	  (1 << 1)
#define LORA_EVENT_RX_TIMEOUT_BIT (1 << 2)

/**
 * @brief LoRa transceiver operating modes
 */
enum class LoRaMode {
	SLEEP,	 /**< Low-power sleep mode */
	STANDBY, /**< Standby mode (ready for TX/RX) */
	TX,		 /**< Transmit mode */
	RX,		 /**< Receive mode */
	CAD		 /**< Channel activity detection mode */
};

/**
 * @brief LoRa event types for async callback notification
 */
enum class LoRaEvent {
	TX_DONE,	  /**< Packet transmission completed */
	RX_DONE,	  /**< Packet received successfully */
	RX_TIMEOUT,	  /**< RX timeout expired with no packet */
	CAD_DONE,	  /**< Channel activity detection completed */
	CAD_DETECTED, /**< LoRa signal detected during CAD */
	ERROR		  /**< Generic error event */
};

/**
 * @brief Callback function type for LoRa events
 * @param event The event that occurred
 */
using LoRaEventCallback = std::function<void (LoRaEvent event)>;

/**
 * @brief LoRa packet structure for TX/RX operations
 */
struct LoRaPacket {
	uint8_t data[LORA_MAX_PACKET_SIZE]; /**< Packet payload data */
	size_t length;						/**< Actual packet length in bytes */
	int16_t rssi;						/**< Received signal strength (dBm) */
	int8_t snr;							/**< Signal-to-noise ratio (dB) */
};

/**
 * @brief Driver for SX1262 LoRa transceiver via SPI
 */
class LoRaDriver {
  public:
	/**
	 * @brief Construct a LoRaDriver instance
	 * @param spi_host SPI host device (e.g., SPI2_HOST)
	 * @param mosi GPIO pin for MOSI
	 * @param miso GPIO pin for MISO
	 * @param sclk GPIO pin for SPI clock
	 * @param nss GPIO pin for chip select (active low)
	 * @param reset GPIO pin for reset (active low)
	 * @param busy GPIO pin for busy indicator
	 * @param dio1 GPIO pin for DIO1 interrupt
	 */
	LoRaDriver (spi_host_device_t spi_host, gpio_num_t mosi, gpio_num_t miso, gpio_num_t sclk,
				gpio_num_t nss, gpio_num_t reset, gpio_num_t busy, gpio_num_t dio1);

	/**
	 * @brief Destructor - releases SPI bus resources
	 */
	~LoRaDriver ();

	/**
	 * @brief Initialize SPI bus and configure SX1262 for LoRa mode
	 * @return ESP_OK on success, error code otherwise
	 */
	esp_err_t init ();

	/**
	 * @brief Perform hardware reset of SX1262
	 * @return ESP_OK on success
	 */
	esp_err_t reset ();

	/**
	 * @brief Enter low-power sleep mode
	 * @return ESP_OK on success
	 */
	esp_err_t sleep ();

	/**
	 * @brief Wake from sleep mode (returns to standby)
	 * @return ESP_OK on success
	 */
	esp_err_t wake ();

	/**
	 * @brief Enter standby mode (ready for TX/RX)
	 * @return ESP_OK on success
	 */
	esp_err_t standby ();

	/**
	 * @brief Set carrier frequency
	 * @param freq_hz Frequency in Hz (e.g., 915000000 for 915 MHz)
	 * @return ESP_OK on success, ESP_ERR_INVALID_ARG if out of range
	 */
	esp_err_t set_frequency (uint32_t freq_hz);

	/**
	 * @brief Set transmit output power
	 * @param power_dbm Power in dBm (-9 to 22)
	 * @return ESP_OK on success, value clamped if out of range
	 */
	esp_err_t set_tx_power (int8_t power_dbm);

	/**
	 * @brief Set LoRa spreading factor
	 * @param sf Spreading factor (5-12)
	 * @return ESP_OK on success, ESP_ERR_INVALID_ARG if invalid
	 */
	esp_err_t set_spreading_factor (uint8_t sf);

	/**
	 * @brief Set LoRa bandwidth
	 * @param bw_hz Bandwidth in Hz
	 * @return ESP_OK on success
	 */
	esp_err_t set_bandwidth (uint32_t bw_hz);

	/**
	 * @brief Set LoRa coding rate
	 * @param cr Coding rate denominator (5-8, results in 4/5 to 4/8)
	 * @return ESP_OK on success, ESP_ERR_INVALID_ARG if invalid
	 */
	esp_err_t set_coding_rate (uint8_t cr);

	/**
	 * @brief Set preamble length
	 * @param len Number of preamble symbols
	 * @return ESP_OK on success
	 */
	esp_err_t set_preamble_length (uint16_t len);

	/**
	 * @brief Start asynchronous packet transmission
	 * @param data Pointer to transmit data buffer
	 * @param len Number of bytes to transmit
	 * @return ESP_OK on success, ESP_ERR_INVALID_SIZE if too large
	 */
	esp_err_t send (const uint8_t* data, size_t len);

	/**
	 * @brief Transmit packet and block until complete or timeout
	 * @param data Pointer to transmit data buffer
	 * @param len Number of bytes to transmit
	 * @param timeout_ms Timeout in milliseconds
	 * @return ESP_OK on success, ESP_ERR_TIMEOUT on timeout
	 */
	esp_err_t send_blocking (const uint8_t* data, size_t len, uint32_t timeout_ms);

	/**
	 * @brief Start receive mode and block until packet or timeout
	 * @param data Pointer to receive buffer
	 * @param max_len Maximum buffer capacity
	 * @param actual_len Pointer to store actual bytes received
	 * @param timeout_ms Timeout in milliseconds
	 * @return ESP_OK on success, ESP_ERR_TIMEOUT on timeout
	 */
	esp_err_t receive (uint8_t* data, size_t max_len, size_t* actual_len, uint32_t timeout_ms);

	/**
	 * @brief Start channel activity detection (CAD)
	 * @return ESP_OK on success
	 */
	esp_err_t start_cad ();

	/**
	 * @brief Check if channel is active (must call after start_cad)
	 * @return true if LoRa signal detected, false otherwise
	 */
	bool is_channel_active ();

	/**
	 * @brief Register callback for async event notifications
	 * @param callback Function to call on events
	 */
	void
	set_event_callback (LoRaEventCallback callback) {
		callback_ = callback;
	}

	/**
	 * @brief Get current operating mode
	 * @return Current LoRaMode
	 */
	LoRaMode
	get_mode () const {
		return mode_;
	}

	/**
	 * @brief Check if SX1262 is busy (DIO1 asserted)
	 * @return true if busy, false otherwise
	 */
	bool is_busy () const;

	/**
	 * @brief Convert mode enum to string for logging
	 * @param mode Mode to convert
	 * @return Static string representation
	 */
	static const char* mode_to_string (LoRaMode mode);

	/**
	 * @brief Convert event enum to string for logging
	 * @param event Event to convert
	 * @return Static string representation
	 */
	static const char* event_to_string (LoRaEvent event);

  private:
	esp_err_t write_reg (uint8_t addr, const uint8_t* data, size_t len);
	esp_err_t read_reg (uint8_t addr, uint8_t* data, size_t len);
	uint8_t read_reg (uint8_t addr);
	void write_reg (uint8_t addr, uint8_t value);

	esp_err_t wait_busy (uint32_t timeout_ms);
	esp_err_t set_operating_mode (LoRaMode mode);
	esp_err_t configure_modem ();
	static void dio1_isr_handler (void* arg);
	void handle_irq ();

	spi_host_device_t spi_host_;
	spi_device_handle_t spi_;
	gpio_num_t mosi_, miso_, sclk_, nss_, reset_, busy_, dio1_;

	EventGroupHandle_t event_group_;

	LoRaMode mode_;
	uint32_t frequency_;
	int8_t tx_power_;
	uint8_t spreading_factor_;
	LoRaEventCallback callback_;
};
