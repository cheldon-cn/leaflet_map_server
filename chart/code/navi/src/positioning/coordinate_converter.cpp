#include "ogc/navi/positioning/coordinate_converter.h"
#include <cmath>

namespace ogc {
namespace navi {

const EllipsoidParams CoordinateConverter::WGS84(6378137.0, 1.0 / 298.257223563);
const EllipsoidParams CoordinateConverter::CGCS2000(6378137.0, 1.0 / 298.257222101);

namespace {
    const double PI = 3.14159265358979323846;
    const double DEG_TO_RAD = PI / 180.0;
    const double RAD_TO_DEG = 180.0 / PI;
    const double EARTH_RADIUS = 6371000.0;
}

CoordinateConverter& CoordinateConverter::Instance() {
    static CoordinateConverter instance;
    return instance;
}

void CoordinateConverter::LatLonToMercator(double lat, double lon, double& x, double& y) {
    double lat_rad = lat * DEG_TO_RAD;
    double lon_rad = lon * DEG_TO_RAD;
    
    x = WGS84.a * lon_rad;
    
    if (lat >= 90.0) {
        y = WGS84.a * PI / 2.0;
    } else if (lat <= -90.0) {
        y = -WGS84.a * PI / 2.0;
    } else {
        double e = std::sqrt(WGS84.e2);
        double sin_lat = std::sin(lat_rad);
        double ts = std::tan(PI / 4.0 - lat_rad / 2.0) / 
                    std::pow((1.0 - e * sin_lat) / (1.0 + e * sin_lat), e / 2.0);
        y = -WGS84.a * std::log(ts);
    }
}

void CoordinateConverter::MercatorToLatLon(double x, double y, double& lat, double& lon) {
    lon = (x / WGS84.a) * RAD_TO_DEG;
    
    double e = std::sqrt(WGS84.e2);
    double ts = std::exp(-y / WGS84.a);
    
    double lat_rad = PI / 2.0 - 2.0 * std::atan(ts);
    double lat_prev = 0.0;
    
    for (int i = 0; i < 10; ++i) {
        lat_prev = lat_rad;
        double sin_lat = std::sin(lat_rad);
        lat_rad = PI / 2.0 - 2.0 * std::atan(ts * std::pow((1.0 - e * sin_lat) / (1.0 + e * sin_lat), e / 2.0));
        
        if (std::abs(lat_rad - lat_prev) < 1e-10) {
            break;
        }
    }
    
    lat = lat_rad * RAD_TO_DEG;
}

void CoordinateConverter::Wgs84ToCgcs2000(double wgs_lat, double wgs_lon, 
                                         double& cgcs_lat, double& cgcs_lon) {
    double dx = 0.0;
    double dy = 0.0;
    double dz = 0.0;
    
    double lat_rad = wgs_lat * DEG_TO_RAD;
    double sin_lat = std::sin(lat_rad);
    double cos_lat = std::cos(lat_rad);
    double sin_lon = std::sin(wgs_lon * DEG_TO_RAD);
    double cos_lon = std::cos(wgs_lon * DEG_TO_RAD);
    
    double e2 = WGS84.e2;
    double N = WGS84.a / std::sqrt(1.0 - e2 * sin_lat * sin_lat);
    
    double x_wgs = (N + 0.0) * cos_lat * cos_lon;
    double y_wgs = (N + 0.0) * cos_lat * sin_lon;
    double z_wgs = (N * (1.0 - e2) + 0.0) * sin_lat;
    
    double x_cgcs = x_wgs + dx;
    double y_cgcs = y_wgs + dy;
    double z_cgcs = z_wgs + dz;
    
    double p = std::sqrt(x_cgcs * x_cgcs + y_cgcs * y_cgcs);
    double theta = std::atan2(z_cgcs * CGCS2000.a, p * CGCS2000.b);
    
    e2 = CGCS2000.e2;
    double lat_cgcs_rad = std::atan2(z_cgcs + CGCS2000.ep2 * CGCS2000.b * std::pow(std::sin(theta), 3),
                                     p - e2 * CGCS2000.a * std::pow(std::cos(theta), 3));
    
    cgcs_lat = lat_cgcs_rad * RAD_TO_DEG;
    cgcs_lon = std::atan2(y_cgcs, x_cgcs) * RAD_TO_DEG;
}

void CoordinateConverter::GeoToScreen(double lat, double lon, double scale, 
                                      double origin_x, double origin_y,
                                      double& screen_x, double& screen_y) {
    double mercator_x, mercator_y;
    LatLonToMercator(lat, lon, mercator_x, mercator_y);
    
    screen_x = (mercator_x - origin_x) * scale;
    screen_y = (mercator_y - origin_y) * scale;
}

void CoordinateConverter::ScreenToGeo(double screen_x, double screen_y, double scale,
                                      double origin_x, double origin_y,
                                      double& lat, double& lon) {
    double mercator_x = screen_x / scale + origin_x;
    double mercator_y = screen_y / scale + origin_y;
    
    MercatorToLatLon(mercator_x, mercator_y, lat, lon);
}

double CoordinateConverter::CalculateGreatCircleDistance(double lat1, double lon1, 
                                                         double lat2, double lon2) {
    double lat1_rad = lat1 * DEG_TO_RAD;
    double lat2_rad = lat2 * DEG_TO_RAD;
    double delta_lat = (lat2 - lat1) * DEG_TO_RAD;
    double delta_lon = (lon2 - lon1) * DEG_TO_RAD;
    
    double a = std::sin(delta_lat / 2.0) * std::sin(delta_lat / 2.0) +
               std::cos(lat1_rad) * std::cos(lat2_rad) *
               std::sin(delta_lon / 2.0) * std::sin(delta_lon / 2.0);
    
    double c = 2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a));
    
    return EARTH_RADIUS * c;
}

double CoordinateConverter::CalculateBearing(double lat1, double lon1, 
                                             double lat2, double lon2) {
    double lat1_rad = lat1 * DEG_TO_RAD;
    double lat2_rad = lat2 * DEG_TO_RAD;
    double delta_lon = (lon2 - lon1) * DEG_TO_RAD;
    
    double x = std::sin(delta_lon) * std::cos(lat2_rad);
    double y = std::cos(lat1_rad) * std::sin(lat2_rad) -
               std::sin(lat1_rad) * std::cos(lat2_rad) * std::cos(delta_lon);
    
    double bearing = std::atan2(x, y) * RAD_TO_DEG;
    
    return std::fmod(bearing + 360.0, 360.0);
}

void CoordinateConverter::CalculateDestination(double lat1, double lon1, 
                                               double bearing, double distance,
                                               double& lat2, double& lon2) {
    double lat1_rad = lat1 * DEG_TO_RAD;
    double lon1_rad = lon1 * DEG_TO_RAD;
    double bearing_rad = bearing * DEG_TO_RAD;
    
    double angular_distance = distance / EARTH_RADIUS;
    
    double lat2_rad = std::asin(std::sin(lat1_rad) * std::cos(angular_distance) +
                                std::cos(lat1_rad) * std::sin(angular_distance) * std::cos(bearing_rad));
    
    double lon2_rad = lon1_rad + std::atan2(std::sin(bearing_rad) * std::sin(angular_distance) * std::cos(lat1_rad),
                                            std::cos(angular_distance) - std::sin(lat1_rad) * std::sin(lat2_rad));
    
    lat2 = lat2_rad * RAD_TO_DEG;
    lon2 = lon2_rad * RAD_TO_DEG;
    
    lon2 = std::fmod(lon2 + 540.0, 360.0) - 180.0;
}

}
}
