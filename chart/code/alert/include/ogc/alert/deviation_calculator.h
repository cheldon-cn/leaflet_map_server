#ifndef OGC_ALERT_DEVIATION_CALCULATOR_H
#define OGC_ALERT_DEVIATION_CALCULATOR_H

#include "export.h"
#include "types.h"
#include <vector>
#include <memory>

namespace ogc {
namespace alert {

struct DeviationResult {
    double deviation_distance;
    double cross_track_distance;
    double along_track_distance;
    Coordinate nearest_point;
    int nearest_segment_index;
    double bearing_to_nearest;
    double distance_to_end;
    bool is_left_of_route;
};

struct RouteSegment {
    Coordinate start;
    Coordinate end;
    double length;
    double bearing;
};

class OGC_ALERT_API IDeviationCalculator {
public:
    virtual ~IDeviationCalculator() = default;
    
    virtual DeviationResult Calculate(const Coordinate& position,
                                       const std::vector<Coordinate>& route) = 0;
    
    virtual double CalculateCrossTrack(const Coordinate& position,
                                        const Coordinate& line_start,
                                        const Coordinate& line_end) = 0;
    
    virtual double CalculateAlongTrack(const Coordinate& position,
                                        const Coordinate& line_start,
                                        const Coordinate& line_end) = 0;
    
    virtual Coordinate FindNearestPoint(const Coordinate& position,
                                         const std::vector<Coordinate>& route) = 0;
    
    static std::unique_ptr<IDeviationCalculator> Create();
};

class OGC_ALERT_API DeviationCalculator : public IDeviationCalculator {
public:
    DeviationCalculator();
    ~DeviationCalculator() override;
    
    DeviationResult Calculate(const Coordinate& position,
                               const std::vector<Coordinate>& route) override;
    
    double CalculateCrossTrack(const Coordinate& position,
                                const Coordinate& line_start,
                                const Coordinate& line_end) override;
    
    double CalculateAlongTrack(const Coordinate& position,
                                const Coordinate& line_start,
                                const Coordinate& line_end) override;
    
    Coordinate FindNearestPoint(const Coordinate& position,
                                 const std::vector<Coordinate>& route) override;

private:
    double HaversineDistance(const Coordinate& from, const Coordinate& to) const;
    double CalculateBearing(const Coordinate& from, const Coordinate& to) const;
    Coordinate CalculateDestination(const Coordinate& start, 
                                     double bearing, double distance) const;
    double NormalizeAngle(double angle) const;
    RouteSegment CreateSegment(const Coordinate& start, const Coordinate& end) const;
    Coordinate ProjectPointOnSegment(const Coordinate& point,
                                      const Coordinate& line_start,
                                      const Coordinate& line_end,
                                      double& distance) const;
};

class OGC_ALERT_API RouteAnalyzer {
public:
    static double CalculateTotalLength(const std::vector<Coordinate>& route);
    static std::vector<RouteSegment> CreateSegments(const std::vector<Coordinate>& route);
    static int FindNearestSegment(const Coordinate& position,
                                   const std::vector<RouteSegment>& segments);
    static double CalculateProgress(const Coordinate& position,
                                     const std::vector<Coordinate>& route);
};

class OGC_ALERT_API CrossTrackCalculator {
public:
    static double Calculate(const Coordinate& position,
                            const Coordinate& line_start,
                            const Coordinate& line_end);
    
    static double CalculateSigned(const Coordinate& position,
                                   const Coordinate& line_start,
                                   const Coordinate& line_end);
};

}
}

#endif
