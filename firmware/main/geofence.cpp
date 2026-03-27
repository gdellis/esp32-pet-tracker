#include "geofence.hpp"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define EARTH_RADIUS_METERS 6371000

static double to_radians(int32_t degrees_e6) {
    return (degrees_e6 / 1000000.0) * (M_PI / 180.0);
}

int64_t haversine_distance(int32_t lat1, int32_t lon1, int32_t lat2, int32_t lon2) {
    double lat1_rad = to_radians(lat1);
    double lat2_rad = to_radians(lat2);
    double dlat = to_radians(lat2 - lat1);
    double dlon = to_radians(lon2 - lon1);
    
    double a = sin(dlat / 2) * sin(dlat / 2) +
               cos(lat1_rad) * cos(lat2_rad) *
               sin(dlon / 2) * sin(dlon / 2);
    
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    
    return (int64_t)(EARTH_RADIUS_METERS * c);
}

bool point_in_circle(const GeoPoint& point, const CircleZone& zone) {
    int64_t distance = haversine_distance(
        point.latitude, point.longitude,
        zone.center.latitude, zone.center.longitude
    );
    return distance <= zone.radius_m;
}
