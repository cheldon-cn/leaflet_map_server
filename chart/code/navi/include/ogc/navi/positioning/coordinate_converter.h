#pragma once

#include "ogc/navi/types.h"
#include "ogc/navi/export.h"

namespace ogc {
namespace navi {

struct EllipsoidParams {
    double a;
    double f;
    double b;
    double e2;
    double ep2;
    
    EllipsoidParams()
        : a(0.0), f(0.0), b(0.0), e2(0.0), ep2(0.0)
    {}
    
    EllipsoidParams(double semi_major, double flattening)
        : a(semi_major)
        , f(flattening)
        , b(a * (1.0 - f))
        , e2(2.0 * f - f * f)
        , ep2(e2 / (1.0 - e2))
    {}
};

class OGC_NAVI_API CoordinateConverter {
public:
    static CoordinateConverter& Instance();
    
    void LatLonToMercator(double lat, double lon, double& x, double& y);
    void MercatorToLatLon(double x, double y, double& lat, double& lon);
    
    void Wgs84ToCgcs2000(double wgs_lat, double wgs_lon, 
                         double& cgcs_lat, double& cgcs_lon);
    
    void GeoToScreen(double lat, double lon, double scale, 
                     double origin_x, double origin_y,
                     double& screen_x, double& screen_y);
    void ScreenToGeo(double screen_x, double screen_y, double scale,
                     double origin_x, double origin_y,
                     double& lat, double& lon);
    
    double CalculateGreatCircleDistance(double lat1, double lon1, 
                                        double lat2, double lon2);
    
    double CalculateBearing(double lat1, double lon1, 
                           double lat2, double lon2);
    
    void CalculateDestination(double lat1, double lon1, 
                             double bearing, double distance,
                             double& lat2, double& lon2);
    
    static const EllipsoidParams WGS84;
    static const EllipsoidParams CGCS2000;
    
private:
    CoordinateConverter() = default;
    CoordinateConverter(const CoordinateConverter&) = delete;
    CoordinateConverter& operator=(const CoordinateConverter&) = delete;
};

}
}
