#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

constexpr int32_t COORD_SCALE = 1000000;

struct GeoPoint {
	int32_t latitude;  // degrees * 1e6
	int32_t longitude; // degrees * 1e6
};

struct CircleZone {
	char name[31]; // Max 31 chars + null terminator for safety
	GeoPoint center;
	uint32_t radius_m; // meters
};

struct PolygonZone {
	char name[31];
	GeoPoint vertices[16]; // max 16 vertices
	uint8_t vertex_count;
};

/**
 * @brief Validate geographic coordinates
 * @param lat Latitude in degrees * 1e6
 * @param lon Longitude in degrees * 1e6
 * @return true if coordinates are valid, false otherwise
 */
bool coordinates_valid (int32_t lat, int32_t lon);

/**
 * @brief Safely copy zone name with length validation
 * @param zone Target zone
 * @param name Source name string (max 31 chars)
 */
void zone_set_name (CircleZone& zone, const char* name);

/**
 * @brief Check if a point is within a circular geofence zone
 * @param point The geographic point to check
 * @param zone The circular zone to check against
 * @return true if point is within the zone (including boundary), false otherwise
 */
bool point_in_circle (const GeoPoint& point, const CircleZone& zone);

/**
 * @brief Calculate the Haversine distance between two geographic points
 * @param lat1 Latitude of first point in degrees * 1e6
 * @param lon1 Longitude of first point in degrees * 1e6
 * @param lat2 Latitude of second point in degrees * 1e6
 * @param lon2 Longitude of second point in degrees * 1e6
 * @return Distance between points in meters
 */
int64_t haversine_distance (int32_t lat1, int32_t lon1, int32_t lat2, int32_t lon2);

/**
 * @brief Add a vertex to a polygon zone
 * @param zone Target polygon zone
 * @param lat Latitude of vertex in degrees * 1e6
 * @param lon Longitude of vertex in degrees * 1e6
 * @return true if vertex was added, false if zone is full
 */
bool polygon_add_vertex (PolygonZone& zone, int32_t lat, int32_t lon);

/**
 * @brief Check if a point is within a polygon geofence zone using ray casting
 * @param point The geographic point to check
 * @param zone The polygon zone to check against
 * @return true if point is within the zone, false otherwise
 */
bool point_in_polygon (const GeoPoint& point, const PolygonZone& zone);
