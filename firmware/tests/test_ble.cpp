#define CATCH_CONFIG_MAIN
#include "catch2/catch_all.hpp"
#include <cstdint>
#include <cstring>

constexpr uint16_t BLE_SERVICE_UUID = 0x1810;
constexpr uint16_t BLE_CHAR_LOCATION_UUID = 0x2A67;
constexpr uint16_t BLE_CHAR_NAME_UUID = 0x2A99;
constexpr uint16_t BLE_DEVICE_NAME_MAX = 32;

struct BleLocationData {
	int32_t latitude;
	int32_t longitude;
	int32_t altitude;
	uint32_t timestamp;
	uint8_t valid;
};

TEST_CASE ("BleLocationData structure size", "[ble]") {
	BleLocationData loc = {};
	size_t expected_min_size = sizeof (int32_t) * 3 + sizeof (uint32_t) + sizeof (uint8_t);
	REQUIRE (sizeof (loc) >= expected_min_size);
}

TEST_CASE ("BleLocationData initialization", "[ble]") {
	BleLocationData loc = { .latitude = 40000000,
							.longitude = -74000000,
							.altitude = 10000,
							.timestamp = 1234567890,
							.valid = 1 };

	REQUIRE (loc.latitude == 40000000);
	REQUIRE (loc.longitude == -74000000);
	REQUIRE (loc.altitude == 10000);
	REQUIRE (loc.timestamp == 1234567890);
	REQUIRE (loc.valid == 1);
}

TEST_CASE ("BleServer device name constants", "[ble]") {
	REQUIRE (BLE_DEVICE_NAME_MAX == 32);
	REQUIRE (BLE_SERVICE_UUID == 0x1810);
	REQUIRE (BLE_CHAR_LOCATION_UUID == 0x2A67);
	REQUIRE (BLE_CHAR_NAME_UUID == 0x2A99);
}

TEST_CASE ("BleLocationData byte layout", "[ble]") {
	BleLocationData loc = { .latitude = 0x00000001,
							.longitude = 0x00000002,
							.altitude = 0x00000003,
							.timestamp = 0x00000004,
							.valid = 1 };

	uint8_t* bytes = reinterpret_cast<uint8_t*> (&loc);
	REQUIRE (bytes[0] == 0x01);
	REQUIRE (bytes[4] == 0x02);
	REQUIRE (bytes[8] == 0x03);
	REQUIRE (bytes[12] == 0x04);
	REQUIRE (bytes[16] == 0x01);
}
