#define CATCH_CONFIG_MAIN
#include "catch2/catch_all.hpp"
#include "geofence.hpp"

TEST_CASE("Haversine distance calculation", "[geofence]") {
    SECTION("Same point returns zero") {
        int32_t lat = 40000000;
        int32_t lon = -74000000;
        REQUIRE(haversine_distance(lat, lon, lat, lon) == 0);
    }

    SECTION("Known distance NYC to LA is approximately 3940km") {
        int32_t nyc_lat = 40760000;
        int32_t nyc_lon = -73940000;
        int32_t la_lat = 34030000;
        int32_t la_lon = -118150000;
        int64_t dist = haversine_distance(nyc_lat, nyc_lon, la_lat, la_lon);
        REQUIRE(dist > 3900000);
        REQUIRE(dist < 4000000);
    }

    SECTION("Equator points 1 degree apart is approximately 111km") {
        int32_t lat = 0;
        int32_t lon1 = 0;
        int32_t lon2 = 1000000;
        int64_t dist = haversine_distance(lat, lon1, lat, lon2);
        REQUIRE(dist > 110000);
        REQUIRE(dist < 112000);
    }
}

TEST_CASE("Point in circle detection", "[geofence]") {
    CircleZone zone = {
        .name = "TestZone",
        .center = {40000000, -74000000},
        .radius_m = 1000
    };

    SECTION("Point at center is inside") {
        GeoPoint point = {40000000, -74000000};
        REQUIRE(point_in_circle(point, zone) == true);
    }

    SECTION("Point well within radius is inside") {
        GeoPoint point = {40000500, -74000500};
        REQUIRE(point_in_circle(point, zone) == true);
    }

    SECTION("Point outside radius is outside") {
        GeoPoint point = {40010000, -74010000};
        REQUIRE(point_in_circle(point, zone) == false);
    }

    SECTION("Point on boundary is inside") {
        GeoPoint point = zone.center;
        int64_t dist = haversine_distance(point.latitude, point.longitude, 
                                          zone.center.latitude, zone.center.longitude);
        REQUIRE(dist <= zone.radius_m);
    }
}
