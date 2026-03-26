#pragma once

#include <stdint.h>
#include <functional>
#include "esp_err.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

#define LORA_DEFAULT_SPI_FREQ 1000000
#define LORA_DEFAULT_TX_POWER 22
#define LORA_DEFAULT_FREQ 915000000
#define LORA_MAX_PACKET_SIZE 255

enum class LoRaMode {
    SLEEP,
    STANDBY,
    TX,
    RX,
    CAD
};

enum class LoRaEvent {
    TX_DONE,
    RX_DONE,
    RX_TIMEOUT,
    CAD_DONE,
    CAD_DETECTED,
    ERROR
};

using LoRaEventCallback = std::function<void(LoRaEvent event)>;

struct LoRaPacket {
    uint8_t data[LORA_MAX_PACKET_SIZE];
    size_t length;
    int16_t rssi;
    int8_t snr;
};

class LoRaDriver {
public:
    LoRaDriver(spi_host_device_t spi_host, gpio_num_t mosi, gpio_num_t miso,
               gpio_num_t sclk, gpio_num_t nss, gpio_num_t reset,
               gpio_num_t busy, gpio_num_t dio1);
    ~LoRaDriver();

    esp_err_t init();
    esp_err_t reset();
    esp_err_t sleep();
    esp_err_t standby();

    esp_err_t set_frequency(uint32_t freq_hz);
    esp_err_t set_tx_power(int8_t power_dbm);
    esp_err_t set_spreading_factor(uint8_t sf);
    esp_err_t set_bandwidth(uint32_t bw_hz);
    esp_err_t set_coding_rate(uint8_t cr);
    esp_err_t set_preamble_length(uint16_t len);

    esp_err_t send(const uint8_t* data, size_t len);
    esp_err_t send_blocking(const uint8_t* data, size_t len, uint32_t timeout_ms);
    esp_err_t receive(uint8_t* data, size_t max_len, size_t* actual_len, uint32_t timeout_ms);

    esp_err_t start_cad();
    bool is_channel_active();

    void set_event_callback(LoRaEventCallback callback) { callback_ = callback; }

    LoRaMode get_mode() const { return mode_; }
    bool is_busy() const;

    static const char* mode_to_string(LoRaMode mode);
    static const char* event_to_string(LoRaEvent event);

private:
    esp_err_t write_reg(uint8_t addr, const uint8_t* data, size_t len);
    esp_err_t read_reg(uint8_t addr, uint8_t* data, size_t len);
    uint8_t read_reg(uint8_t addr);
    void write_reg(uint8_t addr, uint8_t value);

    esp_err_t wait_busy(uint32_t timeout_ms);
    esp_err_t set_operating_mode(LoRaMode mode);
    esp_err_t configure_modem();
    void handle_irq();

    spi_host_device_t spi_host_;
    spi_device_handle_t spi_;
    gpio_num_t mosi_, miso_, sclk_, nss_, reset_, busy_, dio1_;

    LoRaMode mode_;
    uint32_t frequency_;
    int8_t tx_power_;
    uint8_t spreading_factor_;
    LoRaEventCallback callback_;
};
