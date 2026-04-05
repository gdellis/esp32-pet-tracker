#include "sx1262.hpp"
#include "esp_log.h"
#include "esp_rom_gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/timers.h"
#include <string.h>

static const char* TAG = "sx1262";

static constexpr uint8_t SX1262_REG_LORA_MODEM_CONFIG1 = 0x51;
static constexpr uint8_t SX1262_REG_LORA_MODEM_CONFIG2 = 0x52;
static constexpr uint8_t SX1262_REG_LORA_PREAMBLE_MSB = 0x54;
static constexpr uint8_t SX1262_REG_LORA_PREAMBLE_LSB = 0x55;

static constexpr uint8_t SX1262_CMD_SET_SLEEP = 0x84;
static constexpr uint8_t SX1262_CMD_SET_STANDBY = 0x80;
static constexpr uint8_t SX1262_CMD_SET_FS = 0xC1;
static constexpr uint8_t SX1262_CMD_SET_TX = 0x83;
static constexpr uint8_t SX1262_CMD_SET_RX = 0x82;
static constexpr uint8_t SX1262_CMD_SET_CAD = 0xC5;
static constexpr uint8_t SX1262_CMD_SET_PKT_TYPE = 0x8A;
static constexpr uint8_t SX1262_CMD_GET_PKT_TYPE = 0x11;
static constexpr uint8_t SX1262_CMD_GET_STATUS = 0x12;
static constexpr uint8_t SX1262_CMD_GET_RX_BUFFER_STATUS = 0x13;
static constexpr uint8_t SX1262_CMD_GET_PKT_CONFIG = 0x14;
static constexpr uint8_t SX1262_CMD_GET_IRQ_STATUS = 0x15;
static constexpr uint8_t SX1262_CMD_CLR_IRQ_STATUS = 0x02;
static constexpr uint8_t SX1262_CMD_SET_DIO_IRQ = 0x08;
static constexpr uint8_t SX1262_CMD_SET_PKT_PARAM = 0x8C;
static constexpr uint8_t SX1262_CMD_SET_RF_FREQ = 0x86;
static constexpr uint8_t SX1262_CMD_SET_TX_PARAMS = 0x8E;
static constexpr uint8_t SX1262_CMD_SET_MODULATION = 0x90;

static constexpr uint8_t SX1262_PKT_TYPE_LORA = 0x01;
static constexpr uint8_t SX1262_PKT_TYPE_FSK = 0x02;

static constexpr uint8_t SX1262_IRQ_TX_DONE = 0x01;
static constexpr uint8_t SX1262_IRQ_RX_DONE = 0x02;
static constexpr uint8_t SX1262_IRQ_CAD_DONE = 0x04;
static constexpr uint8_t SX1262_IRQ_CAD_DETECTED = 0x08;
static constexpr uint8_t SX1262_IRQ_TIMEOUT = 0x40;
static constexpr uint8_t SX1262_IRQ_ALL = 0xFF;

static constexpr uint8_t SX1262_MODEM_CONFIG1_DEFAULT = 0x70;
static constexpr uint8_t SX1262_MODEM_CONFIG2_DEFAULT = 0x74;
static constexpr uint8_t SX1262_MODEM_CONFIG2_CRC_ON = 0x04;

static constexpr uint8_t SX1262_SF_5 = 0x50;
static constexpr uint8_t SX1262_SF_6 = 0x60;
static constexpr uint8_t SX1262_SF_7 = 0x70;
static constexpr uint8_t SX1262_SF_8 = 0x80;
static constexpr uint8_t SX1262_SF_9 = 0x90;
static constexpr uint8_t SX1262_SF_10 = 0xA0;
static constexpr uint8_t SX1262_SF_11 = 0xB0;
static constexpr uint8_t SX1262_SF_12 = 0xC0;

const char*
LoRaDriver::mode_to_string (LoRaMode mode) {
	switch (mode) {
	case LoRaMode::SLEEP:
		return "SLEEP";
	case LoRaMode::STANDBY:
		return "STANDBY";
	case LoRaMode::TX:
		return "TX";
	case LoRaMode::RX:
		return "RX";
	case LoRaMode::CAD:
		return "CAD";
	default:
		return "UNKNOWN";
	}
}

const char*
LoRaDriver::event_to_string (LoRaEvent event) {
	switch (event) {
	case LoRaEvent::TX_DONE:
		return "TX_DONE";
	case LoRaEvent::RX_DONE:
		return "RX_DONE";
	case LoRaEvent::RX_TIMEOUT:
		return "RX_TIMEOUT";
	case LoRaEvent::CAD_DONE:
		return "CAD_DONE";
	case LoRaEvent::CAD_DETECTED:
		return "CAD_DETECTED";
	case LoRaEvent::ERROR:
		return "ERROR";
	default:
		return "UNKNOWN";
	}
}

LoRaDriver::LoRaDriver (spi_host_device_t spi_host, gpio_num_t mosi, gpio_num_t miso,
						gpio_num_t sclk, gpio_num_t nss, gpio_num_t reset, gpio_num_t busy,
						gpio_num_t dio1)
	: spi_host_ (spi_host), spi_ (nullptr), mosi_ (mosi), miso_ (miso), sclk_ (sclk), nss_ (nss),
	  reset_ (reset), busy_ (busy), dio1_ (dio1), event_group_ (xEventGroupCreate ()),
	  mode_ (LoRaMode::SLEEP), frequency_ (LORA_DEFAULT_FREQ), tx_power_ (LORA_DEFAULT_TX_POWER),
	  spreading_factor_ (7), callback_ (nullptr) {}

LoRaDriver::~LoRaDriver () {
	if (spi_ != nullptr) {
		spi_bus_remove_device (spi_);
	}
	spi_bus_free (spi_host_);
	if (event_group_ != nullptr) {
		vEventGroupDelete (event_group_);
	}
}

esp_err_t
LoRaDriver::init () {
	ESP_LOGI (TAG, "Initializing SX1262 LoRa driver");

	spi_bus_config_t bus_config = {};
	bus_config.mosi_io_num = mosi_;
	bus_config.miso_io_num = miso_;
	bus_config.sclk_io_num = sclk_;
	bus_config.quadwp_io_num = -1;
	bus_config.quadhd_io_num = -1;
	bus_config.max_transfer_sz = 256;

	ESP_ERROR_CHECK (spi_bus_initialize (spi_host_, &bus_config, SPI_DMA_CH_AUTO));

	spi_device_interface_config_t dev_config = {};
	dev_config.command_bits = 0;
	dev_config.address_bits = 8;
	dev_config.dummy_bits = 0;
	dev_config.mode = 0;
	dev_config.cs_ena_pretrans = 0;
	dev_config.cs_ena_posttrans = 0;
	dev_config.clock_speed_hz = LORA_DEFAULT_SPI_FREQ;
	dev_config.spics_io_num = nss_;
	dev_config.queue_size = 1;
	dev_config.flags = 0;

	ESP_ERROR_CHECK (spi_bus_add_device (spi_host_, &dev_config, &spi_));

	gpio_reset_pin (reset_);
	gpio_set_direction (reset_, GPIO_MODE_OUTPUT);
	gpio_reset_pin (busy_);
	gpio_set_direction (busy_, GPIO_MODE_INPUT);
	gpio_reset_pin (dio1_);
	gpio_set_direction (dio1_, GPIO_MODE_INPUT);
	gpio_set_intr_type (dio1_, GPIO_INTR_POSEDGE);
	gpio_install_isr_service (0);
	gpio_isr_handler_add (dio1_, dio1_isr_handler, this);

	ESP_ERROR_CHECK (reset ());
	ESP_ERROR_CHECK (standby ());
	ESP_ERROR_CHECK (configure_modem ());
	ESP_ERROR_CHECK (set_frequency (frequency_));
	ESP_ERROR_CHECK (set_tx_power (tx_power_));

	ESP_LOGI (TAG, "SX1262 initialized successfully on %s", mode_to_string (mode_));
	return ESP_OK;
}

esp_err_t
LoRaDriver::configure_modem () {
	uint8_t pkt_type = SX1262_PKT_TYPE_LORA;
	ESP_ERROR_CHECK (write_reg (SX1262_CMD_SET_PKT_TYPE, &pkt_type, 1));

	write_reg (SX1262_REG_LORA_MODEM_CONFIG1, SX1262_MODEM_CONFIG1_DEFAULT);
	write_reg (SX1262_REG_LORA_MODEM_CONFIG2,
			   SX1262_MODEM_CONFIG2_DEFAULT | SX1262_MODEM_CONFIG2_CRC_ON);

	uint16_t preamble = LORA_DEFAULT_PREAMBLE_LENGTH;
	write_reg (SX1262_REG_LORA_PREAMBLE_MSB, (preamble >> 8) & 0xFF);
	write_reg (SX1262_REG_LORA_PREAMBLE_LSB, preamble & 0xFF);

	return ESP_OK;
}

esp_err_t
LoRaDriver::reset () {
	gpio_set_level (reset_, 0);
	esp_rom_delay_us (150);
	gpio_set_level (reset_, 1);
	esp_rom_delay_us (100);
	ESP_LOGD (TAG, "SX1262 reset complete");
	return ESP_OK;
}

esp_err_t
LoRaDriver::sleep () {
	uint8_t params = 0x00;
	ESP_ERROR_CHECK (write_reg (SX1262_CMD_SET_SLEEP, &params, 1));
	mode_ = LoRaMode::SLEEP;
	esp_rom_delay_us (500);
	ESP_LOGD (TAG, "SX1262 entered sleep mode");
	return ESP_OK;
}

esp_err_t
LoRaDriver::standby () {
	ESP_ERROR_CHECK (write_reg (SX1262_CMD_SET_STANDBY, nullptr, 0));
	mode_ = LoRaMode::STANDBY;
	ESP_LOGD (TAG, "SX1262 entered standby mode");
	return ESP_OK;
}

esp_err_t
LoRaDriver::set_frequency (uint32_t freq_hz) {
	uint64_t freq_reg = ((uint64_t)freq_hz << 25) / 1000000;

	uint8_t params[3] = { (uint8_t)((freq_reg >> 16) & 0xFF), (uint8_t)((freq_reg >> 8) & 0xFF),
						  (uint8_t)(freq_reg & 0xFF) };

	ESP_ERROR_CHECK (write_reg (SX1262_CMD_SET_RF_FREQ, params, sizeof (params)));
	frequency_ = freq_hz;
	ESP_LOGD (TAG, "Frequency set to %lu Hz", freq_hz);
	return ESP_OK;
}

esp_err_t
LoRaDriver::set_tx_power (int8_t power_dbm) {
	if (power_dbm < -9 || power_dbm > 22) {
		ESP_LOGW (TAG, "TX power %d dBm out of range (-9 to 22)", power_dbm);
		power_dbm = (power_dbm < -9) ? -9 : 22;
	}

	uint8_t params[2] = { (uint8_t)power_dbm, 0x01 };
	ESP_ERROR_CHECK (write_reg (SX1262_CMD_SET_TX_PARAMS, params, sizeof (params)));
	tx_power_ = power_dbm;
	ESP_LOGD (TAG, "TX power set to %d dBm", power_dbm);
	return ESP_OK;
}

esp_err_t
LoRaDriver::set_spreading_factor (uint8_t sf) {
	if (sf < 5 || sf > 12) {
		return ESP_ERR_INVALID_ARG;
	}

	uint8_t modcfg1 = 0x00;
	switch (sf) {
	case 5:
		modcfg1 = SX1262_SF_5;
		break;
	case 6:
		modcfg1 = SX1262_SF_6;
		break;
	case 7:
		modcfg1 = SX1262_SF_7;
		break;
	case 8:
		modcfg1 = SX1262_SF_8;
		break;
	case 9:
		modcfg1 = SX1262_SF_9;
		break;
	case 10:
		modcfg1 = SX1262_SF_10;
		break;
	case 11:
		modcfg1 = SX1262_SF_11;
		break;
	case 12:
		modcfg1 = SX1262_SF_12;
		break;
	}

	uint8_t reg = read_reg (SX1262_REG_LORA_MODEM_CONFIG1);
	write_reg (SX1262_REG_LORA_MODEM_CONFIG1, (reg & 0x0F) | modcfg1);

	spreading_factor_ = sf;
	ESP_LOGD (TAG, "Spreading factor set to %d", sf);
	return ESP_OK;
}

esp_err_t
LoRaDriver::set_bandwidth (uint32_t bw_hz) {
	uint8_t bw_reg = 0x00;
	if (bw_hz <= 7800)
		bw_reg = 0x00;
	else if (bw_hz <= 15600)
		bw_reg = 0x10;
	else if (bw_hz <= 31200)
		bw_reg = 0x20;
	else if (bw_hz <= 62500)
		bw_reg = 0x30;
	else if (bw_hz <= 125000)
		bw_reg = 0x40;
	else if (bw_hz <= 250000)
		bw_reg = 0x50;
	else
		bw_reg = 0x60;

	uint8_t reg = read_reg (SX1262_REG_LORA_MODEM_CONFIG1);
	write_reg (SX1262_REG_LORA_MODEM_CONFIG1, (reg & 0x0F) | bw_reg);

	ESP_LOGD (TAG, "Bandwidth set to %lu Hz", bw_hz);
	return ESP_OK;
}

esp_err_t
LoRaDriver::set_coding_rate (uint8_t cr) {
	if (cr < 5 || cr > 8) {
		return ESP_ERR_INVALID_ARG;
	}

	uint8_t cr_reg = (cr - 4) << 1;
	uint8_t reg = read_reg (SX1262_REG_LORA_MODEM_CONFIG1);
	write_reg (SX1262_REG_LORA_MODEM_CONFIG1, (reg & 0xF1) | cr_reg);

	ESP_LOGD (TAG, "Coding rate set to 4/%d", cr);
	return ESP_OK;
}

esp_err_t
LoRaDriver::set_preamble_length (uint16_t len) {
	write_reg (SX1262_REG_LORA_PREAMBLE_MSB, (len >> 8) & 0xFF);
	write_reg (SX1262_REG_LORA_PREAMBLE_LSB, len & 0xFF);
	ESP_LOGD (TAG, "Preamble length set to %d", len);
	return ESP_OK;
}

esp_err_t
LoRaDriver::send (const uint8_t* data, size_t len) {
	if (len > LORA_MAX_PACKET_SIZE) {
		return ESP_ERR_INVALID_SIZE;
	}

	ESP_ERROR_CHECK (standby ());

	uint8_t pkt_params[6]
		= { 0x00, (uint8_t)((len >> 8) & 0xFF), (uint8_t)(len & 0xFF), 0x00, 0x00, 0x00 };
	ESP_ERROR_CHECK (write_reg (SX1262_CMD_SET_PKT_PARAM, pkt_params, sizeof (pkt_params)));

	uint8_t cmd = 0x40;
	ESP_ERROR_CHECK (write_reg (cmd, data, len));

	uint8_t irq_mask = SX1262_IRQ_TX_DONE;
	write_reg (SX1262_CMD_SET_DIO_IRQ, &irq_mask, 1);

	uint8_t timeout[3] = { 0x00, 0x00, 0xFF };
	ESP_ERROR_CHECK (write_reg (SX1262_CMD_SET_TX, timeout, sizeof (timeout)));

	mode_ = LoRaMode::TX;
	ESP_LOGD (TAG, "TX started, %d bytes", len);

	return ESP_OK;
}

esp_err_t
LoRaDriver::send_blocking (const uint8_t* data, size_t len, uint32_t timeout_ms) {
	ESP_ERROR_CHECK (send (data, len));

	xEventGroupClearBits (event_group_, LORA_EVENT_TX_DONE_BIT);

	EventBits_t bits = xEventGroupWaitBits (event_group_, LORA_EVENT_TX_DONE_BIT, pdTRUE, pdFALSE,
											pdMS_TO_TICKS (timeout_ms));

	if (bits & LORA_EVENT_TX_DONE_BIT) {
		ESP_LOGD (TAG, "TX complete");
		mode_ = LoRaMode::STANDBY;
		return ESP_OK;
	}

	ESP_LOGW (TAG, "TX timeout");
	mode_ = LoRaMode::STANDBY;
	return ESP_ERR_TIMEOUT;
}

esp_err_t
LoRaDriver::receive (uint8_t* data, size_t max_len, size_t* actual_len, uint32_t timeout_ms) {
	if (mode_ == LoRaMode::CAD) {
		ESP_LOGW (TAG, "Cannot start RX while in CAD mode");
		return ESP_ERR_INVALID_STATE;
	}

	ESP_ERROR_CHECK (standby ());

	uint8_t pkt_type = SX1262_PKT_TYPE_LORA;
	ESP_ERROR_CHECK (write_reg (SX1262_CMD_SET_PKT_TYPE, &pkt_type, 1));

	uint8_t rx_params[4] = { 0x00, 0x00, 0x00, 0x01 };
	ESP_ERROR_CHECK (write_reg (0x8D, rx_params, sizeof (rx_params)));

	uint8_t irq_mask = SX1262_IRQ_RX_DONE | SX1262_IRQ_TIMEOUT;
	write_reg (SX1262_CMD_SET_DIO_IRQ, &irq_mask, 1);

	uint32_t timeout_ticks = (timeout_ms * 8) / 125;
	uint8_t timeout[3]
		= { (uint8_t)((timeout_ticks >> 16) & 0xFF), (uint8_t)((timeout_ticks >> 8) & 0xFF),
			(uint8_t)(timeout_ticks & 0xFF) };
	ESP_ERROR_CHECK (write_reg (SX1262_CMD_SET_RX, timeout, sizeof (timeout)));

	mode_ = LoRaMode::RX;
	ESP_LOGD (TAG, "RX started, timeout=%lu ms", timeout_ms);

	xEventGroupClearBits (event_group_, LORA_EVENT_RX_DONE_BIT | LORA_EVENT_RX_TIMEOUT_BIT);

	EventBits_t bits
		= xEventGroupWaitBits (event_group_, LORA_EVENT_RX_DONE_BIT | LORA_EVENT_RX_TIMEOUT_BIT,
							   pdTRUE, pdFALSE, pdMS_TO_TICKS (timeout_ms));

	if (bits & LORA_EVENT_RX_TIMEOUT_BIT) {
		ESP_LOGW (TAG, "RX timeout");
		mode_ = LoRaMode::STANDBY;
		return ESP_ERR_TIMEOUT;
	}

	if (bits & LORA_EVENT_RX_DONE_BIT) {
		uint8_t rx_buf_status[2] = { 0 };
		read_reg (SX1262_CMD_GET_RX_BUFFER_STATUS, rx_buf_status, sizeof (rx_buf_status));

		uint8_t rx_start = rx_buf_status[1];
		*actual_len = rx_buf_status[0];

		if (*actual_len > max_len) {
			ESP_LOGW (TAG, "RX buffer too small");
			*actual_len = max_len;
		}

		uint8_t cmd = 0x1F;
		uint8_t rx_data[*actual_len + 1];
		read_reg (cmd, rx_data, *actual_len + 1);
		memcpy (data, rx_data + rx_start, *actual_len);

		int8_t snr = (int8_t)read_reg (0x1D);
		int8_t rssi = (int8_t)read_reg (0x1E);

		ESP_LOGD (TAG, "RX complete: %d bytes, SNR=%d, RSSI=%d", *actual_len, snr, rssi);

		mode_ = LoRaMode::STANDBY;
		return ESP_OK;
	}

	mode_ = LoRaMode::STANDBY;
	return ESP_ERR_TIMEOUT;
}

esp_err_t
LoRaDriver::start_cad () {
	ESP_ERROR_CHECK (standby ());

	uint8_t cmd = SX1262_CMD_SET_CAD;
	ESP_ERROR_CHECK (write_reg (cmd, nullptr, 0));

	uint8_t irq_mask = SX1262_IRQ_CAD_DONE | SX1262_IRQ_CAD_DETECTED;
	write_reg (SX1262_CMD_SET_DIO_IRQ, &irq_mask, 1);

	mode_ = LoRaMode::CAD;
	ESP_LOGD (TAG, "CAD started");
	return ESP_OK;
}

bool
LoRaDriver::is_channel_active () {
	if (mode_ != LoRaMode::CAD) {
		return false;
	}

	uint8_t irq_status = read_reg (0x0C);
	bool detected = (irq_status & SX1262_IRQ_CAD_DETECTED) != 0;

	ESP_LOGD (TAG, "CAD result: %s", detected ? "channel active" : "channel clear");

	if (callback_) {
		callback_ (detected ? LoRaEvent::CAD_DETECTED : LoRaEvent::CAD_DONE);
	}

	return detected;
}

bool
LoRaDriver::is_busy () const {
	return gpio_get_level (busy_) == 1;
}

esp_err_t
LoRaDriver::wait_busy (uint32_t timeout_ms) {
	uint32_t elapsed = 0;
	while (is_busy () && elapsed < timeout_ms * 1000) {
		esp_rom_delay_us (100);
		elapsed += 100;
	}

	if (elapsed >= timeout_ms * 1000) {
		return ESP_ERR_TIMEOUT;
	}
	return ESP_OK;
}

void
LoRaDriver::dio1_isr_handler (void* arg) {
	auto* self = static_cast<LoRaDriver*> (arg);
	BaseType_t higher_priority_task_woken = pdFALSE;

	uint8_t irq_status = self->read_reg (0x0C);

	if (irq_status & SX1262_IRQ_TX_DONE) {
		xEventGroupSetBitsFromISR (self->event_group_, LORA_EVENT_TX_DONE_BIT,
								   &higher_priority_task_woken);
	}
	if (irq_status & SX1262_IRQ_RX_DONE) {
		xEventGroupSetBitsFromISR (self->event_group_, LORA_EVENT_RX_DONE_BIT,
								   &higher_priority_task_woken);
	}
	if (irq_status & SX1262_IRQ_TIMEOUT) {
		xEventGroupSetBitsFromISR (self->event_group_, LORA_EVENT_RX_TIMEOUT_BIT,
								   &higher_priority_task_woken);
	}

	if (irq_status & SX1262_IRQ_TX_DONE) {
		self->mode_ = LoRaMode::STANDBY;
		if (self->callback_) {
			self->callback_ (LoRaEvent::TX_DONE);
		}
	}
	if (irq_status & SX1262_IRQ_RX_DONE) {
		self->mode_ = LoRaMode::STANDBY;
		if (self->callback_) {
			self->callback_ (LoRaEvent::RX_DONE);
		}
	}
	if (irq_status & SX1262_IRQ_TIMEOUT) {
		self->mode_ = LoRaMode::STANDBY;
		if (self->callback_) {
			self->callback_ (LoRaEvent::RX_TIMEOUT);
		}
	}

	if (higher_priority_task_woken == pdTRUE) {
		portYIELD_FROM_ISR ();
	}
}

esp_err_t
LoRaDriver::write_reg (uint8_t addr, const uint8_t* data, size_t len) {
	if (data == nullptr && len > 0) {
		return ESP_ERR_INVALID_ARG;
	}

	spi_transaction_t trans = {};
	trans.addr = addr;
	trans.length = len * 8;
	trans.rxlength = 0;
	trans.tx_buffer = data;

	if (len == 0) {
		trans.flags = SPI_TRANS_USE_TXDATA;
	}

	ESP_ERROR_CHECK (spi_device_transmit (spi_, &trans));
	return ESP_OK;
}

esp_err_t
LoRaDriver::read_reg (uint8_t addr, uint8_t* data, size_t len) {
	spi_transaction_t trans = {};
	trans.addr = 0x80 | addr;
	trans.length = len * 8;
	trans.rxlength = len * 8;
	trans.rx_buffer = data;

	ESP_ERROR_CHECK (spi_device_transmit (spi_, &trans));
	return ESP_OK;
}

uint8_t
LoRaDriver::read_reg (uint8_t addr) {
	uint8_t data;
	read_reg (addr, &data, 1);
	return data;
}

void
LoRaDriver::write_reg (uint8_t addr, uint8_t value) {
	write_reg (addr, &value, 1);
}
