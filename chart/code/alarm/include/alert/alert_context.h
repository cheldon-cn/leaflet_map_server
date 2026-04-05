#ifndef ALERT_SYSTEM_ALERT_CONTEXT_H
#define ALERT_SYSTEM_ALERT_CONTEXT_H

#include "ship_info.h"
#include "coordinate.h"
#include <string>
#include <map>

namespace alert {

struct EnvironmentData {
    double windSpeed;
    double windDirection;
    double waveHeight;
    double wavePeriod;
    double visibility;
    double currentSpeed;
    double currentDirection;
    double airTemperature;
    double waterTemperature;
    double pressure;
    
    EnvironmentData()
        : windSpeed(0), windDirection(0), waveHeight(0), wavePeriod(0),
          visibility(10), currentSpeed(0), currentDirection(0),
          airTemperature(20), waterTemperature(15), pressure(1013.25) {}
};

class AlertContext {
public:
    AlertContext();
    AlertContext(const ShipInfo& shipInfo, const Coordinate& position);
    
    const ShipInfo& GetShipInfo() const { return m_shipInfo; }
    void SetShipInfo(const ShipInfo& info) { m_shipInfo = info; }
    
    const Coordinate& GetPosition() const { return m_position; }
    void SetPosition(const Coordinate& pos) { m_position = pos; }
    
    double GetSpeed() const { return m_speed; }
    void SetSpeed(double speed) { m_speed = speed; }
    
    double GetCourse() const { return m_course; }
    void SetCourse(double course) { m_course = course; }
    
    double GetHeading() const { return m_heading; }
    void SetHeading(double heading) { m_heading = heading; }
    
    double GetRateOfTurn() const { return m_rateOfTurn; }
    void SetRateOfTurn(double rot) { m_rateOfTurn = rot; }
    
    const std::string& GetTimestamp() const { return m_timestamp; }
    void SetTimestamp(const std::string& ts) { m_timestamp = ts; }
    
    const EnvironmentData& GetEnvironmentData() const { return m_envData; }
    void SetEnvironmentData(const EnvironmentData& data) { m_envData = data; }
    
    void SetParameter(const std::string& key, double value);
    double GetParameter(const std::string& key, double defaultValue = 0.0) const;
    
    bool IsValid() const;
    
private:
    ShipInfo m_shipInfo;
    Coordinate m_position;
    double m_speed;
    double m_course;
    double m_heading;
    double m_rateOfTurn;
    std::string m_timestamp;
    EnvironmentData m_envData;
    std::map<std::string, double> m_parameters;
};

}

#endif
