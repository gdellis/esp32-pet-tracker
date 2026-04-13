#define CATCH_CONFIG_MAIN
#include "catch2/catch_all.hpp"
#include "geofence.hpp"

TEST_CASE ("GPS coordinate integration with geofence", "[gps_geofence]") {
	CircleZone zone = { .name = "Home", .center = { 40000000, -74000000 }, .radius_m = 1000 };

	SECTION ("Coordinate scaling - lat/lon to GeoPoint") {
		double test_lat = 40.123456;
		double test_lon = -74.654321;
		int32_t scaled_lat = (int32_t)(test_lat * COORD_SCALE);
		int32_t scaled_lon = (int32_t)(test_lon * COORD_SCALE);

		REQUIRE (scaled_lat == 40123456);
		REQUIRE (scaled_lon == -74654321);

		GeoPoint point = { scaled_lat, scaled_lon };
		REQUIRE (point_in_circle (point, zone) == false);
	}

	SECTION ("Inside zone - point at zone center") {
		GeoPoint point = { 40000000, -74000000 };
		REQUIRE (point_in_circle (point, zone) == true);
	}

	SECTION ("Inside zone - small offset from center (within 1km)") {
		GeoPoint point = { 40000500, -74000500 };
		REQUIRE (point_in_circle (point, zone) == true);
	}

	SECTION ("Outside zone - larger offset (beyond 1km radius)") {
		GeoPoint point = { 40100000, -74100000 };
		REQUIRE (point_in_circle (point, zone) == false);
	}

	SECTION ("Edge case - exactly at zone boundary") {
		CircleZone edge_zone
			= { .name = "Edge", .center = { 40000000, -74000000 }, .radius_m = 1000 };
		double radius_deg = 1000.0 / 111320.0;
		GeoPoint at_edge
			= { (int32_t)((40.0 + radius_deg) * COORD_SCALE), (int32_t)(-74.0 * COORD_SCALE) };
		REQUIRE (point_in_circle (at_edge, edge_zone) == true);

		GeoPoint just_outside = { (int32_t)((40.0 + radius_deg * 1.1) * COORD_SCALE),
								  (int32_t)(-74.0 * COORD_SCALE) };
		REQUIRE (point_in_circle (just_outside, edge_zone) == false);
	}

	SECTION ("Realistic GPS coordinates - NYC to Boston breach") {
		CircleZone home = { .name = "Home", .center = { 40700000, -74000000 }, .radius_m = 5000 };

		GeoPoint at_home = { 40700000, -74000000 };
		GeoPoint near_home = { 40702500, -74000000 };
		GeoPoint in_boston = { 42350000, -71000000 };

		REQUIRE (point_in_circle (at_home, home) == true);
		REQUIRE (point_in_circle (near_home, home) == true);
		REQUIRE (point_in_circle (in_boston, home) == false);
	}

	SECTION ("Multiple zones checked in order") {
		CircleZone zones[2]
			= { { .name = "Home", .center = { 40000000, -74000000 }, .radius_m = 1000 },
				{ .name = "Work", .center = { 40010000, -74010000 }, .radius_m = 500 } };

		GeoPoint at_home = { 40000000, -74000000 };
		GeoPoint at_work = { 40010000, -74010000 };
		GeoPoint neither = { 40100000, -74100000 };

		REQUIRE (point_in_circle (at_home, zones[0]) == true);
		REQUIRE (point_in_circle (at_work, zones[1]) == true);
		REQUIRE (point_in_circle (neither, zones[0]) == false);
		REQUIRE (point_in_circle (neither, zones[1]) == false);
	}

	SECTION ("Large zone - neighborhood size (1km radius)") {
		CircleZone neighborhood
			= { .name = "Neighborhood", .center = { 40000000, -74000000 }, .radius_m = 1000 };

		GeoPoint at_center = { 40000000, -74000000 };
		GeoPoint quarter_km = { 40002250, -74000000 };
		GeoPoint half_km = { 40004500, -74000000 };
		GeoPoint one_km = { 40009000, -74000000 };
		GeoPoint two_km = { 40018000, -74000000 };

		REQUIRE (point_in_circle (at_center, neighborhood) == true);
		REQUIRE (point_in_circle (quarter_km, neighborhood) == true);
		REQUIRE (point_in_circle (half_km, neighborhood) == true);
		REQUIRE (point_in_circle (one_km, neighborhood) == true);
		REQUIRE (point_in_circle (two_km, neighborhood) == false);
	}

	SECTION ("Small zone - pet collar range (10m radius)") {
		CircleZone collar = { .name = "Collar", .center = { 40000000, -74000000 }, .radius_m = 10 };

		GeoPoint at_center = { 40000000, -74000000 };
		GeoPoint very_close = { 40000001, -74000000 };
		GeoPoint far_away = { 40000100, -74000000 };

		REQUIRE (point_in_circle (at_center, collar) == true);
		REQUIRE (point_in_circle (very_close, collar) == true);
		REQUIRE (point_in_circle (far_away, collar) == false);
	}

	SECTION ("Alert data preparation on breach") {
		double breach_lat = 41.0;
		double breach_lon = -74.0;
		double breach_alt = 150.5;
		int32_t scaled_lat = (int32_t)(breach_lat * COORD_SCALE);
		int32_t scaled_lon = (int32_t)(breach_lon * COORD_SCALE);
		int32_t scaled_alt = (int32_t)(breach_alt * 100);

		REQUIRE (scaled_lat == 41000000);
		REQUIRE (scaled_lon == -74000000);
		REQUIRE (scaled_alt == 15050);

		GeoPoint breach_point = { scaled_lat, scaled_lon };
		REQUIRE (point_in_circle (breach_point, zone) == false);
	}
}
