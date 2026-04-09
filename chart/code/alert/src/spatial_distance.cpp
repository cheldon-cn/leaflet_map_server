#include "ogc/alert/spatial_distance.h"
#include "ogc/geom/geometry.h"
#include "ogc/geom/point.h"
#include <cmath>
#include <algorithm>
#include <limits>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace ogc {
namespace alert {

namespace {
const double kEarthRadiusKm = 6371.0;
const double kEarthRadiusM = 6371000.0;
const double kFlattening = 1.0 / 298.257223563;
const double kSemiMajorAxis = 6378137.0;

double ToRadians(double degrees) {
    return degrees * M_PI / 180.0;
}

double ToDegrees(double radians) {
    return radians * 180.0 / M_PI;
}

ogc::Coordinate ToOgcCoordinate(const Coordinate& coord) {
    return ogc::Coordinate(coord.longitude, coord.latitude, coord.altitude);
}

}

SpatialDistance::SpatialDistance() {
}

SpatialDistance::~SpatialDistance() {
}

double SpatialDistance::PointToPoint(const Coordinate& from, const Coordinate& to) {
    return HaversineDistance(from, to);
}

double SpatialDistance::PointToPointMeters(const Coordinate& from, const Coordinate& to) {
    return HaversineDistance(from, to) * 1000.0;
}

PointToLineResult SpatialDistance::PointToLine(const Coordinate& point,
                                                const Coordinate& line_start,
                                                const Coordinate& line_end) {
    return PointToLineInternal(point, line_start, line_end);
}

PointToLineResult SpatialDistance::PointToLineInternal(const Coordinate& point,
                                                        const Coordinate& line_start,
                                                        const Coordinate& line_end) const {
    PointToLineResult result;
    result.distance = 0.0;
    result.along_distance = 0.0;
    result.cross_distance = 0.0;
    result.bearing_to_nearest = 0.0;
    
    double d13 = HaversineDistance(line_start, point);
    double brng13 = ToRadians(BearingInternal(line_start, point));
    double brng12 = ToRadians(BearingInternal(line_start, line_end));
    double d12 = HaversineDistance(line_start, line_end);
    
    double dxt = std::asin(std::sin(d13 / kEarthRadiusKm) * std::sin(brng13 - brng12));
    result.cross_distance = std::abs(dxt * kEarthRadiusKm);
    
    double dat = std::acos(std::cos(d13 / kEarthRadiusKm) / std::cos(dxt));
    result.along_distance = dat * kEarthRadiusKm;
    
    if (result.along_distance < 0 || result.along_distance > d12) {
        double dist_to_start = HaversineDistance(point, line_start);
        double dist_to_end = HaversineDistance(point, line_end);
        
        if (dist_to_start < dist_to_end) {
            result.distance = dist_to_start;
            result.nearest_point = line_start;
            result.along_distance = 0;
        } else {
            result.distance = dist_to_end;
            result.nearest_point = line_end;
            result.along_distance = d12;
        }
    } else {
        result.distance = result.cross_distance;
        result.nearest_point = DestinationInternal(line_start, ToDegrees(brng12), result.along_distance);
    }
    
    result.bearing_to_nearest = BearingInternal(point, result.nearest_point);
    
    return result;
}

PointToPolygonResult SpatialDistance::PointToPolygon(const Coordinate& point,
                                                      const std::vector<Coordinate>& polygon) {
    PointToPolygonResult result;
    result.distance = std::numeric_limits<double>::max();
    result.is_inside = false;
    result.nearest_edge_index = 0;
    
    if (polygon.size() < 3) {
        return result;
    }
    
    result.is_inside = IsPointInPolygon(point, polygon);
    
    if (result.is_inside) {
        result.distance = 0.0;
        result.nearest_point = point;
        return result;
    }
    
    for (size_t i = 0; i < polygon.size(); ++i) {
        size_t next_i = (i + 1) % polygon.size();
        
        Coordinate nearest;
        double dist = DistanceToSegment(point, polygon[i], polygon[next_i], nearest);
        
        if (dist < result.distance) {
            result.distance = dist;
            result.nearest_point = nearest;
            result.nearest_edge_index = static_cast<int>(i);
        }
    }
    
    return result;
}

double SpatialDistance::PointToGeometry(const Coordinate& point, const Geometry& geometry) {
    PointPtr pt = Point::Create(ToOgcCoordinate(point));
    return geometry.Distance(pt.get()) / 1000.0;
}

double SpatialDistance::Bearing(const Coordinate& from, const Coordinate& to) {
    return BearingInternal(from, to);
}

double SpatialDistance::BearingInternal(const Coordinate& from, const Coordinate& to) const {
    double lat1 = ToRadians(from.latitude);
    double lat2 = ToRadians(to.latitude);
    double dLon = ToRadians(to.longitude - from.longitude);
    
    double x = std::sin(dLon) * std::cos(lat2);
    double y = std::cos(lat1) * std::sin(lat2) -
               std::sin(lat1) * std::cos(lat2) * std::cos(dLon);
    
    double bearing = ToDegrees(std::atan2(x, y));
    return NormalizeAngle(bearing);
}

Coordinate SpatialDistance::Destination(const Coordinate& start, double bearing, double distance) {
    return DestinationInternal(start, bearing, distance);
}

Coordinate SpatialDistance::DestinationInternal(const Coordinate& start, double bearing, double distance) const {
    double brng = ToRadians(bearing);
    double d = distance / kEarthRadiusKm;
    double lat1 = ToRadians(start.latitude);
    double lon1 = ToRadians(start.longitude);
    
    double lat2 = std::asin(std::sin(lat1) * std::cos(d) +
                            std::cos(lat1) * std::sin(d) * std::cos(brng));
    double lon2 = lon1 + std::atan2(std::sin(brng) * std::sin(d) * std::cos(lat1),
                                     std::cos(d) - std::sin(lat1) * std::sin(lat2));
    
    return Coordinate(ToDegrees(lon2), ToDegrees(lat2));
}

Coordinate SpatialDistance::Midpoint(const Coordinate& p1, const Coordinate& p2) {
    double lat1 = ToRadians(p1.latitude);
    double lon1 = ToRadians(p1.longitude);
    double lat2 = ToRadians(p2.latitude);
    double dLon = ToRadians(p2.longitude - p1.longitude);
    
    double bx = std::cos(lat2) * std::cos(dLon);
    double by = std::cos(lat2) * std::sin(dLon);
    
    double lat3 = std::atan2(std::sin(lat1) + std::sin(lat2),
                              std::sqrt((std::cos(lat1) + bx) * (std::cos(lat1) + bx) + by * by));
    double lon3 = lon1 + std::atan2(by, std::cos(lat1) + bx);
    
    return Coordinate(ToDegrees(lon3), ToDegrees(lat3));
}

double SpatialDistance::HaversineDistance(const Coordinate& from, const Coordinate& to) const {
    double lat1 = ToRadians(from.latitude);
    double lat2 = ToRadians(to.latitude);
    double dLat = ToRadians(to.latitude - from.latitude);
    double dLon = ToRadians(to.longitude - from.longitude);
    
    double a = std::sin(dLat / 2) * std::sin(dLat / 2) +
               std::cos(lat1) * std::cos(lat2) *
               std::sin(dLon / 2) * std::sin(dLon / 2);
    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));
    
    return kEarthRadiusKm * c;
}

double SpatialDistance::NormalizeAngle(double angle) const {
    while (angle < 0) angle += 360;
    while (angle >= 360) angle -= 360;
    return angle;
}

Coordinate SpatialDistance::ProjectPointOnLine(const Coordinate& point,
                                                const Coordinate& line_start,
                                                const Coordinate& line_end) const {
    double d13 = HaversineDistance(line_start, point);
    double brng13 = ToRadians(BearingInternal(line_start, point));
    double brng12 = ToRadians(BearingInternal(line_start, line_end));
    double d12 = HaversineDistance(line_start, line_end);
    
    double dxt = std::asin(std::sin(d13 / kEarthRadiusKm) * std::sin(brng13 - brng12));
    double dat = std::acos(std::cos(d13 / kEarthRadiusKm) / std::cos(dxt));
    
    if (dat < 0 || dat * kEarthRadiusKm > d12) {
        double dist_to_start = HaversineDistance(point, line_start);
        double dist_to_end = HaversineDistance(point, line_end);
        
        return dist_to_start < dist_to_end ? line_start : line_end;
    }
    
    return DestinationInternal(line_start, ToDegrees(brng12), dat * kEarthRadiusKm);
}

bool SpatialDistance::IsPointInPolygon(const Coordinate& point, 
                                        const std::vector<Coordinate>& polygon) const {
    bool inside = false;
    size_t n = polygon.size();
    
    for (size_t i = 0, j = n - 1; i < n; j = i++) {
        if (((polygon[i].latitude > point.latitude) != (polygon[j].latitude > point.latitude)) &&
            (point.longitude < (polygon[j].longitude - polygon[i].longitude) * (point.latitude - polygon[i].latitude) / 
                       (polygon[j].latitude - polygon[i].latitude) + polygon[i].longitude)) {
            inside = !inside;
        }
    }
    
    return inside;
}

double SpatialDistance::DistanceToSegment(const Coordinate& point,
                                           const Coordinate& seg_start,
                                           const Coordinate& seg_end,
                                           Coordinate& nearest) const {
    PointToLineResult result = PointToLineInternal(point, seg_start, seg_end);
    nearest = result.nearest_point;
    return result.distance;
}

std::unique_ptr<ISpatialDistance> ISpatialDistance::Create() {
    return std::unique_ptr<ISpatialDistance>(new SpatialDistance());
}

double GreatCircleCalculator::Distance(const Coordinate& from, const Coordinate& to) {
    double lat1 = ToRadians(from.latitude);
    double lat2 = ToRadians(to.latitude);
    double dLat = ToRadians(to.latitude - from.latitude);
    double dLon = ToRadians(to.longitude - from.longitude);
    
    double a = std::sin(dLat / 2) * std::sin(dLat / 2) +
               std::cos(lat1) * std::cos(lat2) *
               std::sin(dLon / 2) * std::sin(dLon / 2);
    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));
    
    return kEarthRadiusKm * c;
}

double GreatCircleCalculator::Bearing(const Coordinate& from, const Coordinate& to) {
    double lat1 = ToRadians(from.latitude);
    double lat2 = ToRadians(to.latitude);
    double dLon = ToRadians(to.longitude - from.longitude);
    
    double x = std::sin(dLon) * std::cos(lat2);
    double y = std::cos(lat1) * std::sin(lat2) -
               std::sin(lat1) * std::cos(lat2) * std::cos(dLon);
    
    double bearing = ToDegrees(std::atan2(x, y));
    while (bearing < 0) bearing += 360;
    return bearing;
}

Coordinate GreatCircleCalculator::Destination(const Coordinate& start, 
                                               double bearing, double distance_km) {
    double brng = ToRadians(bearing);
    double d = distance_km / kEarthRadiusKm;
    double lat1 = ToRadians(start.latitude);
    double lon1 = ToRadians(start.longitude);
    
    double lat2 = std::asin(std::sin(lat1) * std::cos(d) +
                            std::cos(lat1) * std::sin(d) * std::cos(brng));
    double lon2 = lon1 + std::atan2(std::sin(brng) * std::sin(d) * std::cos(lat1),
                                     std::cos(d) - std::sin(lat1) * std::sin(lat2));
    
    return Coordinate(ToDegrees(lon2), ToDegrees(lat2));
}

Coordinate GreatCircleCalculator::Midpoint(const Coordinate& p1, const Coordinate& p2) {
    double lat1 = ToRadians(p1.latitude);
    double lon1 = ToRadians(p1.longitude);
    double lat2 = ToRadians(p2.latitude);
    double dLon = ToRadians(p2.longitude - p1.longitude);
    
    double bx = std::cos(lat2) * std::cos(dLon);
    double by = std::cos(lat2) * std::sin(dLon);
    
    double lat3 = std::atan2(std::sin(lat1) + std::sin(lat2),
                              std::sqrt((std::cos(lat1) + bx) * (std::cos(lat1) + bx) + by * by));
    double lon3 = lon1 + std::atan2(by, std::cos(lat1) + bx);
    
    return Coordinate(ToDegrees(lon3), ToDegrees(lat3));
}

std::vector<Coordinate> GreatCircleCalculator::Interpolate(const Coordinate& from, 
                                                            const Coordinate& to, 
                                                            int num_points) {
    std::vector<Coordinate> points;
    if (num_points < 2) {
        points.push_back(from);
        points.push_back(to);
        return points;
    }
    
    double total_distance = Distance(from, to);
    double bearing = Bearing(from, to);
    
    for (int i = 0; i <= num_points; ++i) {
        double fraction = static_cast<double>(i) / num_points;
        double dist = total_distance * fraction;
        points.push_back(Destination(from, bearing, dist));
    }
    
    return points;
}

double RhumbLineCalculator::Distance(const Coordinate& from, const Coordinate& to) {
    double lat1 = ToRadians(from.latitude);
    double lat2 = ToRadians(to.latitude);
    double dLat = lat2 - lat1;
    double dLon = ToRadians(std::abs(to.longitude - from.longitude));
    
    if (dLon > M_PI) dLon = 2 * M_PI - dLon;
    
    double dPhi = std::log(std::tan(M_PI / 4 + lat2 / 2) / std::tan(M_PI / 4 + lat1 / 2));
    double q = std::abs(dPhi) > 1e-10 ? dLat / dPhi : std::cos(lat1);
    
    return kEarthRadiusKm * std::sqrt(dLat * dLat + q * q * dLon * dLon);
}

double RhumbLineCalculator::Bearing(const Coordinate& from, const Coordinate& to) {
    double lat1 = ToRadians(from.latitude);
    double lat2 = ToRadians(to.latitude);
    double dLon = ToRadians(to.longitude - from.longitude);
    
    double dPhi = std::log(std::tan(M_PI / 4 + lat2 / 2) / std::tan(M_PI / 4 + lat1 / 2));
    
    double bearing = ToDegrees(std::atan2(dLon, dPhi));
    while (bearing < 0) bearing += 360;
    return bearing;
}

Coordinate RhumbLineCalculator::Destination(const Coordinate& start, 
                                             double bearing, double distance_km) {
    double brng = ToRadians(bearing);
    double d = distance_km / kEarthRadiusKm;
    double lat1 = ToRadians(start.latitude);
    double lon1 = ToRadians(start.longitude);
    
    double dPhi = d * std::cos(brng);
    double lat2 = lat1 + dPhi;
    
    double dPsi = std::log(std::tan(M_PI / 4 + lat2 / 2) / std::tan(M_PI / 4 + lat1 / 2));
    double q = std::abs(dPsi) > 1e-10 ? dPhi / dPsi : std::cos(lat1);
    
    double dLon = d * std::sin(brng) / q;
    double lon2 = lon1 + dLon;
    
    return Coordinate(ToDegrees(lon2), ToDegrees(lat2));
}

double GeodesicCalculator::Distance(const Coordinate& from, const Coordinate& to) {
    return DistanceVincenty(from, to);
}

double GeodesicCalculator::DistanceVincenty(const Coordinate& from, const Coordinate& to) {
    double a = kSemiMajorAxis;
    double f = kFlattening;
    double b = a * (1 - f);
    
    double U1 = std::atan((1 - f) * std::tan(ToRadians(from.latitude)));
    double U2 = std::atan((1 - f) * std::tan(ToRadians(to.latitude)));
    double L = ToRadians(to.longitude - from.longitude);
    
    double lambda = L;
    double lambda_prev;
    double sinU1 = std::sin(U1), cosU1 = std::cos(U1);
    double sinU2 = std::sin(U2), cosU2 = std::cos(U2);
    
    double sinSigma, cosSigma, sigma, sinAlpha, cosSqAlpha, cos2SigmaM;
    
    int iterations = 0;
    const int max_iterations = 100;
    const double tolerance = 1e-12;
    
    do {
        double sinLambda = std::sin(lambda);
        double cosLambda = std::cos(lambda);
        
        sinSigma = std::sqrt((cosU2 * sinLambda) * (cosU2 * sinLambda) +
                             (cosU1 * sinU2 - sinU1 * cosU2 * cosLambda) *
                             (cosU1 * sinU2 - sinU1 * cosU2 * cosLambda));
        
        if (sinSigma == 0) return 0.0;
        
        cosSigma = sinU1 * sinU2 + cosU1 * cosU2 * cosLambda;
        sigma = std::atan2(sinSigma, cosSigma);
        
        sinAlpha = cosU1 * cosU2 * sinLambda / sinSigma;
        cosSqAlpha = 1 - sinAlpha * sinAlpha;
        
        cos2SigmaM = (cosSqAlpha != 0) ? cosSigma - 2 * sinU1 * sinU2 / cosSqAlpha : 0;
        
        double C = f / 16 * cosSqAlpha * (4 + f * (4 - 3 * cosSqAlpha));
        
        lambda_prev = lambda;
        lambda = L + (1 - C) * f * sinAlpha *
                 (sigma + C * sinSigma * (cos2SigmaM + C * cosSigma * (-1 + 2 * cos2SigmaM * cos2SigmaM)));
        
        iterations++;
    } while (std::abs(lambda - lambda_prev) > tolerance && iterations < max_iterations);
    
    double uSq = cosSqAlpha * (a * a - b * b) / (b * b);
    double A = 1 + uSq / 16384 * (4096 + uSq * (-768 + uSq * (320 - 175 * uSq)));
    double B = uSq / 1024 * (256 + uSq * (-128 + uSq * (74 - 47 * uSq)));
    
    double deltaSigma = B * sinSigma * (cos2SigmaM + B / 4 *
                        (cosSigma * (-1 + 2 * cos2SigmaM * cos2SigmaM) -
                         B / 6 * cos2SigmaM * (-3 + 4 * sinSigma * sinSigma) *
                         (-3 + 4 * cos2SigmaM * cos2SigmaM)));
    
    double s = b * A * (sigma - deltaSigma);
    
    return s / 1000.0;
}

}
}
