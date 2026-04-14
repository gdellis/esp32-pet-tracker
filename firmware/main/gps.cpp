#include "gps.hpp"
#include "board_config.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "esp_timer.h"

static const char* TAG = "gps";

Gps::Gps (uart_port_t uart_num) : uart_num_ (uart_num), data_ ({}) {}

Gps::~Gps () {
	uart_driver_delete (uart_num_);
}

bool
Gps::init () {
	gpio_config_t power_config = {
		.pin_bit_mask = (1ULL << BOARD_GPS_POWER_PIN),
		.mode = GPIO_MODE_OUTPUT,
		.pull_up_en = GPIO_PULLUP_DISABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type = GPIO_INTR_DISABLE,
	};
	ESP_ERROR_CHECK (gpio_config (&power_config));
	gpio_set_level (BOARD_GPS_POWER_PIN, 0);

	uart_config_t uart_config = {};
	uart_config.baud_rate = 115200;
	uart_config.data_bits = UART_DATA_8_BITS;
	uart_config.parity = UART_PARITY_DISABLE;
	uart_config.stop_bits = UART_STOP_BITS_1;
	uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;

	ESP_ERROR_CHECK (uart_param_config (uart_num_, &uart_config));
	ESP_ERROR_CHECK (uart_set_pin (uart_num_, BOARD_GPS_TX_PIN, BOARD_GPS_RX_PIN,
								   UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
	ESP_ERROR_CHECK (uart_driver_install (uart_num_, GPS_BUFFER_SIZE * 2, 0, 0, NULL, 0));

	ESP_LOGI (TAG, "GPS initialized on UART%d", uart_num_);
	return true;
}

void
Gps::power_on () {
	gpio_set_level (BOARD_GPS_POWER_PIN, 1);
	ESP_LOGD (TAG, "GPS powered on");
}

void
Gps::power_off () {
	gpio_set_level (BOARD_GPS_POWER_PIN, 0);
	ESP_LOGD (TAG, "GPS powered off");
}

bool
Gps::update () {
	int len = uart_read_bytes (uart_num_, rx_buffer_, GPS_BUFFER_SIZE - 1, 0);
	if (len > 0 && (size_t)len < GPS_BUFFER_SIZE) {
		rx_buffer_[len] = '\0';
		return parse_nmea ((const char*)rx_buffer_, len);
	}
	return false;
}

bool
Gps::parse_nmea (const char* nmea, size_t len) {
	NmeaData nmea_data;
	if (!nmea_parse (nmea, len, nmea_data)) {
		return false;
	}
	return convert_to_gps_data (nmea_data);
}

bool
Gps::convert_to_gps_data (const NmeaData& nmea_data) {
	data_.latitude = nmea_data.latitude;
	data_.longitude = nmea_data.longitude;
	data_.altitude = nmea_data.altitude;
	data_.speed = nmea_data.speed;
	data_.course = nmea_data.course;
	data_.satellites = nmea_data.satellites;

	if (nmea_data.has_fix) {
		data_.fix_status = GpsFixStatus::FIX_3D;
		data_.timestamp = esp_timer_get_time ();
	} else {
		data_.fix_status = GpsFixStatus::NONE;
	}

	return true;
}
