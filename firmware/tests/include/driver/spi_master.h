#ifndef TEST_MOCKS_DRIVER_SPI_MASTER_H
#define TEST_MOCKS_DRIVER_SPI_MASTER_H

#include <stdint.h>
#include <stddef.h>

typedef enum {
    SPI0_HOST = 0,
    SPI1_HOST = 1,
    SPI2_HOST = 2,
    SPI3_HOST = 3
} spi_host_device_t;

typedef struct spi_transaction_t {
    uint32_t addr;
    uint32_t flags;
    uint32_t length;
    uint32_t rxlength;
    const void* tx_buffer;
    void* rx_buffer;
} spi_transaction_t;

typedef struct spi_bus_config_t {
    int mosi_io_num;
    int miso_io_num;
    int sclk_io_num;
    int quadwp_io_num;
    int quadhd_io_num;
    int max_transfer_sz;
} spi_bus_config_t;

typedef struct spi_device_interface_config_t {
    int command_bits;
    int address_bits;
    int dummy_bits;
    int mode;
    int cs_ena_pretrans;
    int cs_ena_posttrans;
    int clock_speed_hz;
    int spics_io_num;
    int queue_size;
    int flags;
} spi_device_interface_config_t;

typedef struct spi_device_t* spi_device_handle_t;

#define SPI_TRANS_USE_TXDATA (1 << 2)
#define SPI_TRANS_USE_RXDATA (1 << 1)
#define SPI_DMA_CH_AUTO -1
#define ESP_OK 0
#define ESP_ERR_INVALID_ARG 0x102

#ifdef __cplusplus
extern "C" {
#endif

int spi_bus_initialize(spi_host_device_t host, const spi_bus_config_t* config, int dma);
int spi_bus_add_device(spi_host_device_t host, const spi_device_interface_config_t* config, spi_device_handle_t* dev);
int spi_bus_remove_device(spi_device_handle_t dev);
int spi_bus_free(spi_host_device_t host);
int spi_device_transmit(spi_device_handle_t dev, spi_transaction_t* trans);

#ifdef __cplusplus
}
#endif

namespace mock_spi {

struct Transaction {
    uint32_t addr;
    uint32_t flags;
    uint32_t length;
    const void* tx_buffer;
    void* rx_buffer;
};

void reset_spi_mock();
Transaction get_last_transaction();

}

#endif
