#define CATCH_CONFIG_MAIN
#include "catch2/catch_all.hpp"
#include "nmea_parser.hpp"
#include <cstring>

TEST_CASE("Checksum validation", "[checksum]") {
    SECTION("Valid checksum passes") {
        const char* sentence = "$GPGGA,123519,4807.038,N,01131.000,W,1,08,0.9,545.4,M,47.0,M,,*5D";
        REQUIRE(nmea_validate_checksum(sentence, strlen(sentence)) == true);
    }

    SECTION("Invalid checksum fails") {
        const char* sentence = "$GPGGA,123519,4807.038,N,01131.000,W,1,08,0.9,545.4,M,47.0,M,,*00";
        REQUIRE(nmea_validate_checksum(sentence, strlen(sentence)) == false);
    }

    SECTION("No checksum star fails") {
        const char* sentence = "$GPGGA,123519,4807.038,N,01131.000,W,1,08,0.9,545.4,M,47.0,M,,";
        REQUIRE(nmea_validate_checksum(sentence, strlen(sentence)) == false);
    }

    SECTION("Truncated checksum fails") {
        const char* sentence = "$GPGGA,123519,4807.038,N,01131.000,W,1,08,0.9,545.4,M,47.0,M,,*";
        REQUIRE(nmea_validate_checksum(sentence, strlen(sentence)) == false);
    }
}

TEST_CASE("GGA sentence parsing", "[gga]") {
    NmeaData data;

    SECTION("No fix status") {
        const char* sentence = "$GPGGA,123519,4807.038,N,01131.000,W,0,08,0.9,545.4,M,47.0,M,,*5C";
        REQUIRE(nmea_parse_gga(sentence, data) == true);
        REQUIRE(data.has_fix == false);
        REQUIRE(data.satellites == 8);
    }

    SECTION("Fix status 1 - 2D fix") {
        const char* sentence = "$GPGGA,123519,4807.038,N,01131.000,W,1,08,0.9,545.4,M,47.0,M,,*5D";
        REQUIRE(nmea_parse_gga(sentence, data) == true);
        REQUIRE(data.has_fix == true);
        REQUIRE(data.satellites == 8);
        REQUIRE(data.altitude == Catch::Approx(545.4));
    }

    SECTION("Fix status 2 - 3D fix") {
        const char* sentence = "$GPGGA,123519,4807.038,N,01131.000,W,2,08,0.9,545.4,M,47.0,M,,*5E";
        REQUIRE(nmea_parse_gga(sentence, data) == true);
        REQUIRE(data.has_fix == true);
    }

    SECTION("Satellite count parsed") {
        const char* sentence = "$GPGGA,123519,4807.038,N,01131.000,W,1,12,0.9,100.0,M,47.0,M,,*57";
        REQUIRE(nmea_parse_gga(sentence, data) == true);
        REQUIRE(data.satellites == 12);
    }

    SECTION("Altitude parsed") {
        const char* sentence = "$GPGGA,123519,4807.038,N,01131.000,W,1,08,0.9,123.456,M,47.0,M,,*5A";
        REQUIRE(nmea_parse_gga(sentence, data) == true);
        REQUIRE(data.altitude == Catch::Approx(123.456));
    }
}

TEST_CASE("RMC sentence parsing", "[rmc]") {
    NmeaData data;

    SECTION("Latitude north positive") {
        const char* sentence = "$GPRMC,123519,A,4807.038,N,01131.000,W,022.4,084.4,230394,003.1,W*78";
        REQUIRE(nmea_parse_rmc(sentence, data) == true);
        REQUIRE(data.latitude == Catch::Approx(4807.038));
    }

    SECTION("Latitude south negative") {
        const char* sentence = "$GPRMC,123519,A,4807.038,S,01131.000,W,022.4,084.4,230394,003.1,W*65";
        REQUIRE(nmea_parse_rmc(sentence, data) == true);
        REQUIRE(data.latitude == Catch::Approx(-4807.038));
    }

    SECTION("Longitude east positive") {
        const char* sentence = "$GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A";
        REQUIRE(nmea_parse_rmc(sentence, data) == true);
        REQUIRE(data.longitude == Catch::Approx(1131.000));
    }

    SECTION("Longitude west negative") {
        const char* sentence = "$GPRMC,123519,A,4807.038,N,01131.000,W,022.4,084.4,230394,003.1,W*78";
        REQUIRE(nmea_parse_rmc(sentence, data) == true);
        REQUIRE(data.longitude == Catch::Approx(-1131.000));
    }

    SECTION("Speed in knots converted to km/h") {
        const char* sentence = "$GPRMC,123519,A,4807.038,N,01131.000,W,010.0,084.4,230394,003.1,W*7D";
        REQUIRE(nmea_parse_rmc(sentence, data) == true);
        REQUIRE(data.speed == Catch::Approx(10.0 * 1.852));
    }

    SECTION("Course parsed") {
        const char* sentence = "$GPRMC,123519,A,4807.038,N,01131.000,W,022.4,084.4,230394,003.1,W*78";
        REQUIRE(nmea_parse_rmc(sentence, data) == true);
        REQUIRE(data.course == Catch::Approx(084.4));
    }
}

TEST_CASE("Field parsing", "[field]") {
    char field[64];

    SECTION("Empty field returns zero") {
        const char* sentence = "$GPGGA,123519,,N,01131.000,W,1,08,0.9,545.4,M,47.0,M,,*43";
        REQUIRE(nmea_parse_field(sentence, 2, field, sizeof(field)) == 0);
    }

    SECTION("Field after first comma") {
        const char* sentence = "$GPGGA,123519,4807.038,N,01131.000,W,1,08,0.9,545.4,M,47.0,M,,*5D";
        REQUIRE(nmea_parse_field(sentence, 1, field, sizeof(field)) == 6);
    }

    SECTION("Last field before checksum empty") {
        const char* sentence = "$GPGGA,123519,4807.038,N,01131.000,W,1,08,0.9,545.4,M,47.0,M,,*5D";
        REQUIRE(nmea_parse_field(sentence, 13, field, sizeof(field)) == 0);
    }

    SECTION("Field with asterisk terminates") {
        const char* sentence = "$GPGGA,123519,4807.038,N,01131.000,W,1,08,0.9,545.4,M,47.0,M,,*";
        REQUIRE(nmea_parse_field(sentence, 13, field, sizeof(field)) == 0);
    }

    SECTION("Buffer limit respected") {
        const char* sentence = "$GPGGA,123519,123456789.123,N,01131.000,W,1,08,0.9,545.4,M,47.0,M,,*5D";
        char small_buf[8];
        REQUIRE(nmea_parse_field(sentence, 2, small_buf, sizeof(small_buf)) == 7);
        REQUIRE(strncmp(small_buf, "1234567", 7) == 0);
    }
}

TEST_CASE("NMEA parse function dispatch", "[nmea]") {
    NmeaData data;

    SECTION("GGA sentence parsed") {
        const char* sentence = "$GPGGA,123519,4807.038,N,01131.000,W,1,08,0.9,545.4,M,47.0,M,,*5D";
        REQUIRE(nmea_parse(sentence, strlen(sentence), data) == true);
        REQUIRE(data.has_fix == true);
    }

    SECTION("RMC sentence parsed") {
        const char* sentence = "$GPRMC,123519,A,4807.038,N,01131.000,W,022.4,084.4,230394,003.1,W*78";
        REQUIRE(nmea_parse(sentence, strlen(sentence), data) == true);
        REQUIRE(data.latitude == Catch::Approx(4807.038));
    }

    SECTION("Unknown sentence type returns false") {
        const char* sentence = "$GPGLL,4807.038,N,01131.000,W,123519,A*37";
        REQUIRE(nmea_parse(sentence, strlen(sentence), data) == false);
    }

    SECTION("Invalid prefix returns false") {
        const char* sentence = "GPGGA,123519,4807.038,N,01131.000,W,1,08,0.9,545.4,M,47.0,M,,*5D";
        REQUIRE(nmea_parse(sentence, strlen(sentence), data) == false);
    }

    SECTION("Too short returns false") {
        const char* sentence = "$GP";
        REQUIRE(nmea_parse(sentence, strlen(sentence), data) == false);
    }

    SECTION("Invalid checksum fails") {
        const char* sentence = "$GPGGA,123519,4807.038,N,01131.000,W,1,08,0.9,545.4,M,47.0,M,,*00";
        REQUIRE(nmea_parse(sentence, strlen(sentence), data) == false);
    }
}
