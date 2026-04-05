#include "geofence.hpp"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define EARTH_RADIUS_METERS 6371000

static double
to_radians (int32_t degrees_e6) {
	return (static_cast<double> (degrees_e6) / 1000000.0) * (M_PI / 180.0);
}

bool
coordinates_valid (int32_t lat, int32_t lon) {
	constexpr int32_t MAX_LAT = 90 * 1000000;
	constexpr int32_t MIN_LAT = -90 * 1000000;
	constexpr int32_t MAX_LON = 180 * 1000000;
	constexpr int32_t MIN_LON = -180 * 1000000;

	return (lat >= MIN_LAT && lat <= MAX_LAT) && (lon >= MIN_LON && lon <= MAX_LON);
}

void
zone_set_name (CircleZone& zone, const char* name) {
	strncpy (zone.name, name, sizeof (zone.name) - 1);
	zone.name[sizeof (zone.name) - 1] = '\0';
}

int64_t
haversine_distance (int32_t lat1, int32_t lon1, int32_t lat2, int32_t lon2) {
	double lat1_rad = to_radians (lat1);
	double lat2_rad = to_radians (lat2);
	double dlat = to_radians (lat2 - lat1);
	double dlon = to_radians (lon2 - lon1);

	double a = sin (dlat / 2) * sin (dlat / 2)
			   + cos (lat1_rad) * cos (lat2_rad) * sin (dlon / 2) * sin (dlon / 2);

	double c = 2 * atan2 (sqrt (a), sqrt (1 - a));

	return (int64_t)(EARTH_RADIUS_METERS * c);
}

bool
point_in_circle (const GeoPoint& point, const CircleZone& zone) {
	if (!coordinates_valid (point.latitude, point.longitude)) {
		return false;
	}
	if (!coordinates_valid (zone.center.latitude, zone.center.longitude)) {
		return false;
	}

	int64_t distance = haversine_distance (point.latitude, point.longitude, zone.center.latitude,
										   zone.center.longitude);
	return distance <= zone.radius_m;
}
