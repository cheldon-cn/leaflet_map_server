#ifndef ALERT_SYSTEM_COORDINATE_H
#define ALERT_SYSTEM_COORDINATE_H

#include <cmath>
#include <string>

namespace alert {

class Coordinate {
public:
    Coordinate();
    Coordinate(double x, double y);
    Coordinate(double x, double y, double z);
    
    double GetX() const { return m_x; }
    double GetY() const { return m_y; }
    double GetZ() const { return m_z; }
    
    void SetX(double x) { m_x = x; }
    void SetY(double y) { m_y = y; }
    void SetZ(double z) { m_z = z; }
    
    double GetLongitude() const { return m_x; }
    double GetLatitude() const { return m_y; }
    double GetAltitude() const { return m_z; }
    
    void SetLongitude(double lon) { m_x = lon; }
    void SetLatitude(double lat) { m_y = lat; }
    void SetAltitude(double alt) { m_z = alt; }
    
    bool IsNull() const;
    bool IsValid() const;
    
    double DistanceTo(const Coordinate& other) const;
    double BearingTo(const Coordinate& other) const;
    Coordinate Destination(double distance, double bearing) const;
    
    bool operator==(const Coordinate& other) const;
    bool operator!=(const Coordinate& other) const;
    
    std::string ToString() const;
    
    static double CalculateDistance(double lon1, double lat1, double lon2, double lat2);
    static double CalculateBearing(double lon1, double lat1, double lon2, double lat2);
    
private:
    double m_x;
    double m_y;
    double m_z;
};

}

#endif
