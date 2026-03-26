#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_rom_gpio.h"

namespace mock_spi {

static Transaction last_transaction_ = {};
static bool initialized_ = false;

void reset_spi_mock() {
    last_transaction_ = {};
    initialized_ = false;
}

Transaction get_last_transaction() {
    return last_transaction_;
}

}

using namespace mock_spi;

extern "C" {

int spi_bus_initialize(spi_host_device_t host, const spi_bus_config_t* config, int dma) {
    (void)host;
    (void)config;
    (void)dma;
    initialized_ = true;
    return ESP_OK;
}

int spi_bus_add_device(spi_host_device_t host, const spi_device_interface_config_t* config, spi_device_handle_t* dev) {
    (void)host;
    (void)config;
    (void)dev;
    return ESP_OK;
}

int spi_bus_remove_device(spi_device_handle_t dev) {
    (void)dev;
    return ESP_OK;
}

int spi_bus_free(spi_host_device_t host) {
    (void)host;
    return ESP_OK;
}

int spi_device_transmit(spi_device_handle_t dev, spi_transaction_t* trans) {
    (void)dev;
    
    last_transaction_.addr = trans->addr;
    last_transaction_.flags = trans->flags;
    last_transaction_.length = trans->length;
    last_transaction_.tx_buffer = trans->tx_buffer;
    last_transaction_.rx_buffer = trans->rx_buffer;
    
    if (trans->rx_buffer != nullptr && trans->rxlength > 0) {
        uint8_t* rx = static_cast<uint8_t*>(trans->rx_buffer);
        const uint8_t* tx = static_cast<const uint8_t*>(trans->tx_buffer);
        for (size_t i = 0; i < trans->rxlength / 8 && i < 16; i++) {
            rx[i] = tx ? tx[i] : 0;
        }
    }
    
    return ESP_OK;
}

}

extern "C" {

void esp_rom_gpio_connect_spi_signal(int gpio, int signal, bool invert, uint8_t mode) {
    (void)gpio;
    (void)signal;
    (void)invert;
    (void)mode;
}

void esp_rom_gpio_pad_select_gpio(uint8_t gpio) {
    (void)gpio;
}

void esp_rom_delay_us(uint32_t us) {
    (void)us;
}

}
