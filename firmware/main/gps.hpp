#ifndef GPS_HPP
#define GPS_HPP

#include <stdint.h>
#include "driver/uart.h"
#include "esp_event.h"

#define GPS_UART_NUM UART_NUM_1
#define GPS_TX_PIN GPIO_NUM_7
#define GPS_RX_PIN GPIO_NUM_15
#define GPS_BUFFER_SIZE 256

enum class GpsFixStatus {
    NONE,
    FIX_2D,
    FIX_3D
};

struct GpsData {
    double latitude;
    double longitude;
    double altitude;
    double speed;
    double course;
    uint8_t satellites;
    GpsFixStatus fix_status;
    uint64_t timestamp;
};

class Gps {
public:
    Gps(uart_port_t uart_num = GPS_UART_NUM);
    ~Gps();

    bool init();
    bool update();
    bool has_fix() const { return data_.fix_status != GpsFixStatus::NONE; }
    const GpsData& get_data() const { return data_; }

private:
    bool parse_nmea(const char* nmea, size_t len);
    bool parse_gga(const char* nmea);
    bool parse_rmc(const char* nmea);
    int parse_field(const char* nmea, int field_idx, char* out, size_t out_len);

    uart_port_t uart_num_;
    GpsData data_;
    uint8_t rx_buffer_[GPS_BUFFER_SIZE];
};

#endif