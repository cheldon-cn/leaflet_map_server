#ifndef OGC_ALERT_UKC_CALCULATOR_H
#define OGC_ALERT_UKC_CALCULATOR_H

#include "export.h"
#include "types.h"
#include <vector>
#include <memory>

namespace ogc {
namespace alert {

struct UKCInput {
    double chart_depth;
    double tide_height;
    double ship_draft;
    double squat;
    double heel_correction;
    double wave_allowance;
    double safety_margin;
    double water_density;
    double speed_knots;
    double depth_at_position;
};

struct UKCResult {
    double ukc;
    double static_ukc;
    double dynamic_ukc;
    double squat_estimate;
    double effective_depth;
    bool is_safe;
    AlertLevel alert_level;
    std::string warning_message;
};

struct SquatParams {
    double block_coefficient;
    double ship_length;
    double ship_beam;
    double channel_depth;
    double channel_width;
    bool is_confined_water;
};

class OGC_ALERT_API UKCCalculator {
public:
    UKCCalculator();
    ~UKCCalculator();
    
    UKCResult Calculate(const UKCInput& input);
    
    double CalculateSquat(double speed_knots, const SquatParams& params) const;
    
    double CalculateHeelCorrection(double speed_knots, double turn_radius, double ship_draft) const;
    
    double CalculateWaveAllowance(double wave_height) const;
    
    double CalculateDepthCorrection(double water_density) const;
    
    void SetSafetyMargin(double margin);
    double GetSafetyMargin() const;
    
    void SetUKCThresholds(double level1, double level2, double level3, double level4);
    
    static double NauticalMilesToMeters(double nm);
    static double MetersToNauticalMiles(double m);
    static double KnotsToMetersPerSecond(double knots);
    
private:
    double m_safetyMargin;
    double m_level1Threshold;
    double m_level2Threshold;
    double m_level3Threshold;
    double m_level4Threshold;
    
    double CalculateBarrassSquat(double speed_knots, double block_coeff, 
                                  double ship_length, double depth) const;
    double CalculateICorellSquat(double speed_knots, double block_coeff,
                                  double ship_length, double depth, double beam) const;
    AlertLevel DetermineAlertLevel(double ukc) const;
};

class OGC_ALERT_API DepthInterpolator {
public:
    DepthInterpolator();
    ~DepthInterpolator();
    
    void AddDepthPoint(const Coordinate& position, double depth);
    void Clear();
    
    double InterpolateDepth(const Coordinate& position) const;
    
    double GetNearestDepth(const Coordinate& position) const;
    
    bool HasData() const;
    size_t GetPointCount() const;
    
private:
    struct DepthPoint {
        Coordinate position;
        double depth;
    };
    
    std::vector<DepthPoint> m_points;
    
    double CalculateDistance(const Coordinate& a, const Coordinate& b) const;
    double InverseDistanceWeight(const std::vector<std::pair<double, double>>& distances) const;
};

class OGC_ALERT_API TidePredictor {
public:
    TidePredictor();
    ~TidePredictor();
    
    void SetTideStation(const std::string& station_id, const Coordinate& position);
    void SetTideData(const std::string& station_id, double current_height,
                     const DateTime& next_high, const DateTime& next_low);
    
    double PredictTide(const Coordinate& position, const DateTime& time) const;
    
    double GetCurrentTide(const Coordinate& position) const;
    
    bool HasStation(const std::string& station_id) const;
    
private:
    struct TideStation {
        std::string station_id;
        Coordinate position;
        double current_height;
        DateTime next_high_tide;
        DateTime next_low_tide;
    };
    
    std::vector<TideStation> m_stations;
    
    const TideStation* FindNearestStation(const Coordinate& position) const;
    double HarmonicPrediction(const TideStation& station, const DateTime& time) const;
    double CalculateDistance(const Coordinate& a, const Coordinate& b) const;
};

}
}

#endif
