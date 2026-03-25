#include "gps.hpp"
#include "esp_log.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include <string.h>
#include <stdlib.h>

static const char* TAG = "gps";

Gps::Gps(uart_port_t uart_num) 
    : uart_num_(uart_num), data_({}) {}

Gps::~Gps() {
    uart_driver_delete(uart_num_);
}

bool Gps::init() {
    uart_config_t uart_config = {};
    uart_config.baud_rate = 9600;
    uart_config.data_bits = UART_DATA_8_BITS;
    uart_config.parity = UART_PARITY_DISABLE;
    uart_config.stop_bits = UART_STOP_BITS_1;
    uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    
    ESP_ERROR_CHECK(uart_param_config(uart_num_, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(uart_num_, GPS_TX_PIN, GPS_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(uart_num_, GPS_BUFFER_SIZE * 2, 0, 0, NULL, 0));
    
    ESP_LOGI(TAG, "GPS initialized on UART%d", uart_num_);
    return true;
}

bool Gps::update() {
    int len = uart_read_bytes(uart_num_, rx_buffer_, GPS_BUFFER_SIZE - 1, 0);
    if (len > 0) {
        rx_buffer_[len] = '\0';
        return parse_nmea((const char*)rx_buffer_, len);
    }
    return false;
}

bool Gps::parse_nmea(const char* nmea, size_t len) {
    if (len < 6 || nmea[0] != '$') {
        return false;
    }
    
    if (!validate_checksum(nmea, len)) {
        ESP_LOGW(TAG, "NMEA checksum failed for: %.40s", nmea);
        return false;
    }
    
    if (strncmp(nmea + 2, "GGA", 3) == 0) {
        return parse_gga(nmea);
    } else if (strncmp(nmea + 2, "RMC", 3) == 0) {
        return parse_rmc(nmea);
    }
    return false;
}

bool Gps::validate_checksum(const char* nmea, size_t len) {
    const char* checksum_ptr = (const char*)memchr(nmea, '*', len);
    if (!checksum_ptr) {
        return true;
    }
    
    size_t checksum_pos = checksum_ptr - nmea;
    if (checksum_pos + 3 > len) {
        return false;
    }
    
    uint8_t calculated = 0;
    for (size_t i = 1; i < checksum_pos; i++) {
        calculated ^= nmea[i];
    }
    
    char expected_str[3] = {checksum_ptr[1], checksum_ptr[2], '\0'};
    uint8_t expected = (uint8_t)strtol(expected_str, NULL, 16);
    
    return calculated == expected;
}

bool Gps::parse_gga(const char* nmea) {
    char field[32];
    
    if (parse_field(nmea, 6, field, sizeof(field)) > 0) {
        data_.fix_status = (field[0] == '1') ? GpsFixStatus::FIX_2D : 
                           (field[0] >= '2') ? GpsFixStatus::FIX_3D : 
                           GpsFixStatus::NONE;
        if (data_.fix_status != GpsFixStatus::NONE) {
            data_.timestamp = esp_timer_get_time();
        }
    }
    
    if (parse_field(nmea, 7, field, sizeof(field)) > 0) {
        data_.satellites = (uint8_t)atoi(field);
    }
    
    if (parse_field(nmea, 9, field, sizeof(field)) > 0) {
        data_.altitude = atof(field);
    }
    
    return true;
}

bool Gps::parse_rmc(const char* nmea) {
    char field[32];
    
    if (parse_field(nmea, 1, field, sizeof(field)) > 0) {
        // Time - could parse to timestamp
    }
    
    if (parse_field(nmea, 3, field, sizeof(field)) > 0) {
        double lat = atof(field);
        char dir;
        if (parse_field(nmea, 4, &dir, sizeof(dir)) > 0) {
            data_.latitude = (dir == 'S') ? -lat : lat;
        }
    }
    
    if (parse_field(nmea, 5, field, sizeof(field)) > 0) {
        double lon = atof(field);
        char dir;
        if (parse_field(nmea, 6, &dir, sizeof(dir)) > 0) {
            data_.longitude = (dir == 'W') ? -lon : lon;
        }
    }
    
    if (parse_field(nmea, 7, field, sizeof(field)) > 0) {
        data_.speed = atof(field) * 1.852; // knots to km/h
    }
    
    if (parse_field(nmea, 8, field, sizeof(field)) > 0) {
        data_.course = atof(field);
    }
    
    return true;
}

int Gps::parse_field(const char* nmea, int field_idx, char* out, size_t out_len) {
    int field_count = 0;
    size_t pos = 0;
    size_t field_start = 0;
    size_t nmea_len = strlen(nmea);
    
    while (pos < nmea_len && field_count < field_idx) {
        if (nmea[pos] == ',') {
            field_count++;
            field_start = pos + 1;
        }
        pos++;
    }
    
    if (field_count != field_idx || field_start >= nmea_len || nmea[field_start] == ',' || nmea[field_start] == '*') {
        out[0] = '\0';
        return 0;
    }
    
    size_t out_pos = 0;
    while (out_pos < out_len - 1 && field_start < nmea_len) {
        if (nmea[field_start] == ',' || nmea[field_start] == '*') {
            break;
        }
        out[out_pos++] = nmea[field_start++];
    }
    out[out_pos] = '\0';
    
    return out_pos;
}