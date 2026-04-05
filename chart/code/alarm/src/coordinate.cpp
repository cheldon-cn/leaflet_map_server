#include "alert/coordinate.h"
#include "alert/alert_types.h"
#include <sstream>
#include <iomanip>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace alert {

Coordinate::Coordinate()
    : m_x(0), m_y(0), m_z(0) {}

Coordinate::Coordinate(double x, double y)
    : m_x(x), m_y(y), m_z(0) {}

Coordinate::Coordinate(double x, double y, double z)
    : m_x(x), m_y(y), m_z(z) {}

bool Coordinate::IsNull() const {
    return m_x == 0 && m_y == 0 && m_z == 0;
}

bool Coordinate::IsValid() const {
    return m_x >= -180 && m_x <= 180 && m_y >= -90 && m_y <= 90;
}

double Coordinate::DistanceTo(const Coordinate& other) const {
    return CalculateDistance(m_x, m_y, other.m_x, other.m_y);
}

double Coordinate::BearingTo(const Coordinate& other) const {
    return CalculateBearing(m_x, m_y, other.m_x, other.m_y);
}

Coordinate Coordinate::Destination(double distance, double bearing) const {
    double distRad = distance / AlertConstants::kEarthRadiusMeters;
    double bearingRad = bearing * M_PI / 180.0;
    double lat1Rad = m_y * M_PI / 180.0;
    double lon1Rad = m_x * M_PI / 180.0;
    
    double lat2Rad = std::asin(std::sin(lat1Rad) * std::cos(distRad) +
                               std::cos(lat1Rad) * std::sin(distRad) * std::cos(bearingRad));
    double lon2Rad = lon1Rad + std::atan2(std::sin(bearingRad) * std::sin(distRad) * std::cos(lat1Rad),
                                          std::cos(distRad) - std::sin(lat1Rad) * std::sin(lat2Rad));
    
    double lon2 = lon2Rad * 180.0 / M_PI;
    double lat2 = lat2Rad * 180.0 / M_PI;
    
    if (lon2 > 180) lon2 -= 360;
    if (lon2 < -180) lon2 += 360;
    
    return Coordinate(lon2, lat2, m_z);
}

bool Coordinate::operator==(const Coordinate& other) const {
    return std::abs(m_x - other.m_x) < 1e-9 &&
           std::abs(m_y - other.m_y) < 1e-9 &&
           std::abs(m_z - other.m_z) < 1e-9;
}

bool Coordinate::operator!=(const Coordinate& other) const {
    return !(*this == other);
}

std::string Coordinate::ToString() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6);
    oss << "(" << m_x << ", " << m_y;
    if (m_z != 0) {
        oss << ", " << m_z;
    }
    oss << ")";
    return oss.str();
}

double Coordinate::CalculateDistance(double lon1, double lat1, double lon2, double lat2) {
    double lat1Rad = lat1 * M_PI / 180.0;
    double lat2Rad = lat2 * M_PI / 180.0;
    double deltaLat = (lat2 - lat1) * M_PI / 180.0;
    double deltaLon = (lon2 - lon1) * M_PI / 180.0;
    
    double a = std::sin(deltaLat / 2) * std::sin(deltaLat / 2) +
               std::cos(lat1Rad) * std::cos(lat2Rad) *
               std::sin(deltaLon / 2) * std::sin(deltaLon / 2);
    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));
    
    return AlertConstants::kEarthRadiusMeters * c;
}

double Coordinate::CalculateBearing(double lon1, double lat1, double lon2, double lat2) {
    double lat1Rad = lat1 * M_PI / 180.0;
    double lat2Rad = lat2 * M_PI / 180.0;
    double deltaLon = (lon2 - lon1) * M_PI / 180.0;
    
    double y = std::sin(deltaLon) * std::cos(lat2Rad);
    double x = std::cos(lat1Rad) * std::sin(lat2Rad) -
               std::sin(lat1Rad) * std::cos(lat2Rad) * std::cos(deltaLon);
    
    double bearing = std::atan2(y, x) * 180.0 / M_PI;
    return std::fmod(bearing + 360.0, 360.0);
}

}
