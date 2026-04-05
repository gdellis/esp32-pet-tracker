#ifndef GPS_HPP
#define GPS_HPP

#include "board_config.h"
#include "driver/uart.h"
#include "esp_event.h"
#include "nmea_parser.hpp"
#include <stdint.h>

#define GPS_UART_NUM	UART_NUM_1
#define GPS_BUFFER_SIZE 256

/**
 * @brief GPS fix status
 */
enum class GpsFixStatus {
	NONE,	/**< No fix */
	FIX_2D, /**< 2D fix */
	FIX_3D	/**< 3D fix */
};

/**
 * @brief GPS location data
 */
struct GpsData {
	double latitude;		 /**< Latitude in degrees */
	double longitude;		 /**< Longitude in degrees */
	double altitude;		 /**< Altitude in meters */
	double speed;			 /**< Speed in m/s */
	double course;			 /**< Course in degrees */
	uint8_t satellites;		 /**< Number of satellites */
	GpsFixStatus fix_status; /**< Current fix status */
	uint64_t timestamp;		 /**< Timestamp of last update */
};

/**
 * @brief GPS driver for NEO-6M module via UART
 */
class Gps {
  public:
	/**
	 * @brief Construct a Gps driver
	 * @param uart_num UART port number (default GPS_UART_NUM)
	 */
	Gps (uart_port_t uart_num = GPS_UART_NUM);

	/**
	 * @brief Destructor - releases UART resources
	 */
	~Gps ();

	/**
	 * @brief Initialize UART and GPS module
	 * @return true on success, false otherwise
	 */
	bool init ();

	/**
	 * @brief Update GPS data by reading from UART
	 * @return true if new data was parsed, false otherwise
	 */
	bool update ();

	/**
	 * @brief Check if GPS has a valid fix
	 * @return true if fix is available
	 */
	bool
	has_fix () const {
		return data_.fix_status != GpsFixStatus::NONE;
	}

	/**
	 * @brief Get current GPS data
	 * @return Const reference to GpsData
	 */
	const GpsData&
	get_data () const {
		return data_;
	}

  private:
	bool parse_nmea (const char* nmea, size_t len);
	bool convert_to_gps_data (const NmeaData& nmea_data);

	uart_port_t uart_num_;
	GpsData data_;
	uint8_t rx_buffer_[GPS_BUFFER_SIZE];
};

#endif
