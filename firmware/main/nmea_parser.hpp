#ifndef NMEA_PARSER_HPP
#define NMEA_PARSER_HPP

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief NMEA sentence parsed data
 */
struct NmeaData {
	double latitude = 0;	/**< Degrees, negative for South */
	double longitude = 0;	/**< Degrees, negative for West */
	double altitude = 0;	/**< Meters above sea level */
	double speed = 0;		/**< Kilometers per hour */
	double course = 0;		/**< Degrees from true north */
	uint8_t satellites = 0; /**< Number of satellites used */
	bool has_fix = false;	/**< True if GPS has valid fix */
};

/**
 * @brief Parse a field from an NMEA sentence
 * @param nmea NMEA sentence string
 * @param field_idx Zero-based field index
 * @param out Output buffer for field value
 * @param out_len Size of output buffer
 * @return Number of characters written, 0 on failure
 */
inline int
nmea_parse_field (const char* nmea, int field_idx, char* out, size_t out_len) {
	if (field_idx < 0 || out_len == 0) {
		return 0;
	}

	int field_count = 0;
	size_t pos = 0;
	size_t field_start = 0;
	size_t nmea_len = strlen (nmea);

	while (pos < nmea_len && field_count < field_idx) {
		if (nmea[pos] == ',') {
			field_count++;
			field_start = pos + 1;
		}
		pos++;
	}

	if (field_count != field_idx) {
		out[0] = '\0';
		return 0;
	}

	if (field_start >= nmea_len || nmea[field_start] == ',' || nmea[field_start] == '*') {
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

/**
 * @brief Validate NMEA checksum
 * @param nmea NMEA sentence string
 * @param len Length of sentence
 * @return true if checksum valid, false if invalid or absent
 */
inline bool
nmea_validate_checksum (const char* nmea, size_t len) {
	const char* checksum_ptr = (const char*)memchr (nmea, '*', len);
	if (!checksum_ptr) {
		return false;
	}

	size_t checksum_pos = checksum_ptr - nmea;
	if (checksum_pos + 3 > len) {
		return false;
	}

	uint8_t calculated = 0;
	for (size_t i = 1; i < checksum_pos; i++) {
		calculated ^= nmea[i];
	}

	char expected_str[3] = { checksum_ptr[1], checksum_ptr[2], '\0' };
	char* endptr = NULL;
	unsigned long expected = strtoul (expected_str, &endptr, 16);
	if (endptr == expected_str || *endptr != '\0' || expected > 255) {
		return false;
	}

	return calculated == (uint8_t)expected;
}

/**
 * @brief Parse GGA sentence (fix data)
 * @param nmea NMEA sentence string
 * @param data Output structure
 * @return true on successful parse
 */
inline bool
nmea_parse_gga (const char* nmea, NmeaData& data) {
	char field[32];

	if (nmea_parse_field (nmea, 6, field, sizeof (field)) > 0) {
		data.has_fix = (field[0] >= '1' && field[0] <= '6');
	} else {
		data.has_fix = false;
		return false;
	}

	if (nmea_parse_field (nmea, 7, field, sizeof (field)) > 0) {
		data.satellites = (uint8_t)atoi (field);
	}

	if (nmea_parse_field (nmea, 9, field, sizeof (field)) > 0) {
		data.altitude = atof (field);
	}

	return true;
}

/**
 * @brief Parse RMC sentence (recommended minimum)
 * @param nmea NMEA sentence string
 * @param data Output structure
 * @return true on successful parse
 */
inline bool
nmea_parse_rmc (const char* nmea, NmeaData& data) {
	char field[32];
	char dir[2];

	bool lat_parsed = false;
	if (nmea_parse_field (nmea, 3, field, sizeof (field)) > 0) {
		double lat = atof (field);
		if (nmea_parse_field (nmea, 4, dir, sizeof (dir)) > 0) {
			data.latitude = (dir[0] == 'S') ? -lat : lat;
			lat_parsed = true;
		}
	}

	if (!lat_parsed) {
		return false;
	}

	bool lon_parsed = false;
	if (nmea_parse_field (nmea, 5, field, sizeof (field)) > 0) {
		double lon = atof (field);
		if (nmea_parse_field (nmea, 6, dir, sizeof (dir)) > 0) {
			data.longitude = (dir[0] == 'W') ? -lon : lon;
			lon_parsed = true;
		}
	}

	if (!lon_parsed) {
		return false;
	}

	if (nmea_parse_field (nmea, 7, field, sizeof (field)) > 0) {
		data.speed = atof (field) * 1.852;
	}

	if (nmea_parse_field (nmea, 8, field, sizeof (field)) > 0) {
		data.course = atof (field);
	}

	return true;
}

/**
 * @brief Parse NMEA sentence (GGA or RMC)
 * @param nmea NMEA sentence string
 * @param len Length of sentence
 * @param data Output structure
 * @return true on successful parse
 */
inline bool
nmea_parse (const char* nmea, size_t len, NmeaData& data) {
	if (len < 6 || nmea[0] != '$') {
		return false;
	}

	if (!nmea_validate_checksum (nmea, len)) {
		return false;
	}

	if (strncmp (nmea + 3, "GGA", 3) == 0) {
		return nmea_parse_gga (nmea, data);
	} else if (strncmp (nmea + 3, "RMC", 3) == 0) {
		return nmea_parse_rmc (nmea, data);
	}
	return false;
}

#endif
