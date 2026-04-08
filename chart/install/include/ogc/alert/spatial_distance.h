#ifndef OGC_ALERT_SPATIAL_DISTANCE_H
#define OGC_ALERT_SPATIAL_DISTANCE_H

#include "export.h"
#include "types.h"
#include <vector>
#include <memory>

namespace ogc {
class Geometry;

namespace alert {

struct DistanceResult {
    double distance;
    double bearing;
    Coordinate nearest_point;
    int nearest_segment_index;
    bool is_inside;
};

struct PointToPolygonResult {
    double distance;
    bool is_inside;
    Coordinate nearest_point;
    int nearest_edge_index;
};

struct PointToLineResult {
    double distance;
    double along_distance;
    double cross_distance;
    Coordinate nearest_point;
    double bearing_to_nearest;
};

class OGC_ALERT_API ISpatialDistance {
public:
    virtual ~ISpatialDistance() = default;
    
    virtual double PointToPoint(const Coordinate& from, const Coordinate& to) = 0;
    virtual double PointToPointMeters(const Coordinate& from, const Coordinate& to) = 0;
    
    virtual PointToLineResult PointToLine(const Coordinate& point,
                                           const Coordinate& line_start,
                                           const Coordinate& line_end) = 0;
    
    virtual PointToPolygonResult PointToPolygon(const Coordinate& point,
                                                 const std::vector<Coordinate>& polygon) = 0;
    
    virtual double Bearing(const Coordinate& from, const Coordinate& to) = 0;
    
    virtual Coordinate Destination(const Coordinate& start, double bearing, double distance) = 0;
    
    virtual Coordinate Midpoint(const Coordinate& p1, const Coordinate& p2) = 0;
    
    static std::unique_ptr<ISpatialDistance> Create();
};

class OGC_ALERT_API SpatialDistance : public ISpatialDistance {
public:
    SpatialDistance();
    ~SpatialDistance() override;
    
    double PointToPoint(const Coordinate& from, const Coordinate& to) override;
    double PointToPointMeters(const Coordinate& from, const Coordinate& to) override;
    
    PointToLineResult PointToLine(const Coordinate& point,
                                   const Coordinate& line_start,
                                   const Coordinate& line_end) override;
    
    PointToPolygonResult PointToPolygon(const Coordinate& point,
                                         const std::vector<Coordinate>& polygon) override;
    
    double Bearing(const Coordinate& from, const Coordinate& to) override;
    
    Coordinate Destination(const Coordinate& start, double bearing, double distance) override;
    
    Coordinate Midpoint(const Coordinate& p1, const Coordinate& p2) override;
    
    static double PointToGeometry(const Coordinate& point, const Geometry& geometry);

private:
    double HaversineDistance(const Coordinate& from, const Coordinate& to) const;
    double NormalizeAngle(double angle) const;
    Coordinate ProjectPointOnLine(const Coordinate& point,
                                   const Coordinate& line_start,
                                   const Coordinate& line_end) const;
    bool IsPointInPolygon(const Coordinate& point, const std::vector<Coordinate>& polygon) const;
    double DistanceToSegment(const Coordinate& point,
                              const Coordinate& seg_start,
                              const Coordinate& seg_end,
                              Coordinate& nearest) const;
    
    double BearingInternal(const Coordinate& from, const Coordinate& to) const;
    Coordinate DestinationInternal(const Coordinate& start, double bearing, double distance) const;
    PointToLineResult PointToLineInternal(const Coordinate& point,
                                           const Coordinate& line_start,
                                           const Coordinate& line_end) const;
};

class OGC_ALERT_API GreatCircleCalculator {
public:
    static double Distance(const Coordinate& from, const Coordinate& to);
    static double Bearing(const Coordinate& from, const Coordinate& to);
    static Coordinate Destination(const Coordinate& start, double bearing, double distance_km);
    static Coordinate Midpoint(const Coordinate& p1, const Coordinate& p2);
    static std::vector<Coordinate> Interpolate(const Coordinate& from, 
                                                const Coordinate& to, 
                                                int num_points);
};

class OGC_ALERT_API RhumbLineCalculator {
public:
    static double Distance(const Coordinate& from, const Coordinate& to);
    static double Bearing(const Coordinate& from, const Coordinate& to);
    static Coordinate Destination(const Coordinate& start, double bearing, double distance_km);
};

class OGC_ALERT_API GeodesicCalculator {
public:
    static double Distance(const Coordinate& from, const Coordinate& to);
    static double DistanceVincenty(const Coordinate& from, const Coordinate& to);
};

}
}

#endif
