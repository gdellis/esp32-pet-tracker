#define CATCH_CONFIG_MAIN
#include "catch2/catch_all.hpp"
#include "geofence.hpp"

TEST_CASE ("Haversine distance calculation", "[geofence]") {
	SECTION ("Same point returns zero") {
		int32_t lat = 40000000;
		int32_t lon = -74000000;
		REQUIRE (haversine_distance (lat, lon, lat, lon) == 0);
	}

	SECTION ("Known distance NYC to LA is approximately 3940km") {
		int32_t nyc_lat = 40760000;
		int32_t nyc_lon = -73940000;
		int32_t la_lat = 34030000;
		int32_t la_lon = -118150000;
		int64_t dist = haversine_distance (nyc_lat, nyc_lon, la_lat, la_lon);
		REQUIRE (dist > 3900000);
		REQUIRE (dist < 4000000);
	}

	SECTION ("Equator points 1 degree apart is approximately 111km") {
		int32_t lat = 0;
		int32_t lon1 = 0;
		int32_t lon2 = 1000000;
		int64_t dist = haversine_distance (lat, lon1, lat, lon2);
		REQUIRE (dist > 110000);
		REQUIRE (dist < 112000);
	}

	SECTION ("Antimeridian crossing - Tokyo to San Francisco") {
		int32_t tokyo_lat = 35652000;
		int32_t tokyo_lon = 139745000;
		int32_t sf_lat = 37774700;
		int32_t sf_lon = -122420000;
		int64_t dist = haversine_distance (tokyo_lat, tokyo_lon, sf_lat, sf_lon);
		REQUIRE (dist > 8200000);
		REQUIRE (dist < 8400000);
	}

	SECTION ("North pole to equator") {
		int32_t pole_lat = 90000000;
		int32_t eq_lat = 0;
		int32_t lon = 0;
		int64_t dist = haversine_distance (pole_lat, lon, eq_lat, lon);
		REQUIRE (dist > 10000000);
		REQUIRE (dist < 10050000);
	}
}

TEST_CASE ("Coordinate validation", "[geofence]") {
	SECTION ("Valid coordinates pass") {
		REQUIRE (coordinates_valid (40000000, -74000000) == true);
		REQUIRE (coordinates_valid (0, 0) == true);
		REQUIRE (coordinates_valid (-60000000, 180000000) == true);
	}

	SECTION ("Invalid latitude fails") {
		REQUIRE (coordinates_valid (95000000, 0) == false);
		REQUIRE (coordinates_valid (-95000000, 0) == false);
	}

	SECTION ("Invalid longitude fails") {
		REQUIRE (coordinates_valid (0, 185000000) == false);
		REQUIRE (coordinates_valid (0, -185000000) == false);
	}
}

TEST_CASE ("Zone name safety", "[geofence]") {
	SECTION ("Short name fits") {
		CircleZone zone = { { 0 } };
		zone_set_name (zone, "Home");
		REQUIRE (strcmp (zone.name, "Home") == 0);
	}

	SECTION ("Long name truncated safely") {
		CircleZone zone = { { 0 } };
		zone_set_name (zone, "This is a very long zone name that exceeds limit");
		REQUIRE (strlen (zone.name) < 32);
		REQUIRE (zone.name[30] == '\0');
	}
}

TEST_CASE ("Point in circle detection", "[geofence]") {
	CircleZone zone = { .name = "TestZone", .center = { 40000000, -74000000 }, .radius_m = 1000 };

	SECTION ("Point at center is inside") {
		GeoPoint point = { 40000000, -74000000 };
		REQUIRE (point_in_circle (point, zone) == true);
	}

	SECTION ("Point well within radius is inside") {
		GeoPoint point = { 40000500, -74000500 };
		REQUIRE (point_in_circle (point, zone) == true);
	}

	SECTION ("Point outside radius is outside") {
		GeoPoint point = { 40010000, -74010000 };
		REQUIRE (point_in_circle (point, zone) == false);
	}

	SECTION ("Invalid point coordinates returns false") {
		GeoPoint bad_point = { 95000000, 0 };
		REQUIRE (point_in_circle (bad_point, zone) == false);
	}

	SECTION ("Very small radius zone") {
		CircleZone tiny_zone
			= { .name = "TinyZone", .center = { 40000000, -74000000 }, .radius_m = 1 };
		GeoPoint center_point = { 40000000, -74000000 };
		GeoPoint near_point = { 40000001, -74000000 };
		GeoPoint far_point = { 40000100, -74000000 };
		REQUIRE (point_in_circle (center_point, tiny_zone) == true);
		REQUIRE (point_in_circle (near_point, tiny_zone) == true);
		REQUIRE (point_in_circle (far_point, tiny_zone) == false);
	}
}

TEST_CASE ("Polygon vertex management", "[geofence]") {
	SECTION ("Add vertices to polygon") {
		PolygonZone zone = {};
		REQUIRE (zone.vertex_count == 0);
		REQUIRE (polygon_add_vertex (zone, 40000000, -74000000) == true);
		REQUIRE (zone.vertex_count == 1);
		REQUIRE (polygon_add_vertex (zone, 40010000, -74000000) == true);
		REQUIRE (zone.vertex_count == 2);
	}

	SECTION ("Polygon rejects vertices when full") {
		PolygonZone zone = {};
		for (uint8_t i = 0; i < 16; i++) {
			REQUIRE (polygon_add_vertex (zone, 40000000 + i, -74000000) == true);
		}
		REQUIRE (zone.vertex_count == 16);
		REQUIRE (polygon_add_vertex (zone, 50000000, -74000000) == false);
	}
}

TEST_CASE ("Point in polygon detection", "[geofence]") {
	SECTION ("Triangle zone - point inside") {
		PolygonZone zone = {};
		polygon_add_vertex (zone, 40000000, -74000000);
		polygon_add_vertex (zone, 40000000, -73900000);
		polygon_add_vertex (zone, 40100000, -74000000);

		GeoPoint inside = { 40033333, -73966666 };
		REQUIRE (point_in_polygon (inside, zone) == true);
	}

	SECTION ("Triangle zone - point outside") {
		PolygonZone zone = {};
		polygon_add_vertex (zone, 40000000, -74000000);
		polygon_add_vertex (zone, 40000000, -73900000);
		polygon_add_vertex (zone, 40100000, -74000000);

		GeoPoint outside = { 39000000, -74000000 };
		REQUIRE (point_in_polygon (outside, zone) == false);
	}

	SECTION ("Square zone") {
		PolygonZone zone = {};
		polygon_add_vertex (zone, 40000000, -74000000);
		polygon_add_vertex (zone, 40000000, -73900000);
		polygon_add_vertex (zone, 40100000, -73900000);
		polygon_add_vertex (zone, 40100000, -74000000);

		GeoPoint inside = { 40050000, -73950000 };
		GeoPoint outside = { 39000000, -74000000 };
		GeoPoint on_edge = { 40000000, -73950000 };

		REQUIRE (point_in_polygon (inside, zone) == true);
		REQUIRE (point_in_polygon (outside, zone) == false);
		REQUIRE (point_in_polygon (on_edge, zone) == true);
	}

	SECTION ("Complex polygon - pentagon") {
		PolygonZone zone = {};
		polygon_add_vertex (zone, 40000000, -74000000);
		polygon_add_vertex (zone, 40100000, -74000000);
		polygon_add_vertex (zone, 40150000, -73950000);
		polygon_add_vertex (zone, 40050000, -73850000);
		polygon_add_vertex (zone, 39950000, -73950000);

		GeoPoint inside = { 40050000, -73950000 };
		GeoPoint outside = { 39000000, -74000000 };

		REQUIRE (point_in_polygon (inside, zone) == true);
		REQUIRE (point_in_polygon (outside, zone) == false);
	}

	SECTION ("Invalid coordinates returns false") {
		PolygonZone zone = {};
		polygon_add_vertex (zone, 40000000, -74000000);
		polygon_add_vertex (zone, 40000000, -73900000);
		polygon_add_vertex (zone, 40100000, -74000000);

		GeoPoint bad_point = { 95000000, 0 };
		REQUIRE (point_in_polygon (bad_point, zone) == false);
	}

	SECTION ("Less than 3 vertices returns false") {
		PolygonZone zone = {};
		polygon_add_vertex (zone, 40000000, -74000000);
		polygon_add_vertex (zone, 40000000, -73900000);

		GeoPoint point = { 40050000, -73950000 };
		REQUIRE (point_in_polygon (point, zone) == false);
	}
}
