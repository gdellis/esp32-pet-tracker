#pragma once

#include <stdint.h>
#include <stdbool.h>

constexpr int32_t LATLON_MULTIPLIER = 1000000;

struct GeoPoint {
    int32_t latitude;   // degrees * 1e6
    int32_t longitude;   // degrees * 1e6
};

struct CircleZone {
    char name[32];
    GeoPoint center;
    uint32_t radius_m;   // meters
};

/**
 * @brief Check if a point is within a circular geofence zone
 * @param point The geographic point to check
 * @param zone The circular zone to check against
 * @return true if point is within the zone (including boundary), false otherwise
 */
bool point_in_circle(const GeoPoint& point, const CircleZone& zone);

/**
 * @brief Calculate the Haversine distance between two geographic points
 * @param lat1 Latitude of first point in degrees * 1e6
 * @param lon1 Longitude of first point in degrees * 1e6
 * @param lat2 Latitude of second point in degrees * 1e6
 * @param lon2 Longitude of second point in degrees * 1e6
 * @return Distance between points in meters
 */
int64_t haversine_distance(int32_t lat1, int32_t lon1, int32_t lat2, int32_t lon2);
