#ifndef OGC_DRAW_COORD_SYSTEM_PRESET_H
#define OGC_DRAW_COORD_SYSTEM_PRESET_H

#include "ogc/draw/export.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace ogc {
namespace draw {

struct CoordSystemInfo {
    std::string name;
    std::string epsg;
    std::string wkt;
    std::string projString;
    std::string description;
    std::string area;
    double accuracy;
    
    CoordSystemInfo() : accuracy(0.0) {}
    
    CoordSystemInfo(const std::string& n, const std::string& e, 
                   const std::string& desc = "", const std::string& a = "",
                   double acc = 0.0)
        : name(n), epsg(e), description(desc), area(a), accuracy(acc) {}
};

class OGC_GRAPH_API CoordSystemPreset {
public:
    static CoordSystemPreset& Instance();
    
    CoordSystemInfo GetCoordSystem(const std::string& name) const;
    CoordSystemInfo GetCoordSystemByEpsg(const std::string& epsg) const;
    
    std::vector<std::string> GetCoordSystemNames() const;
    std::vector<CoordSystemInfo> GetAllCoordSystems() const;
    
    void RegisterCoordSystem(const CoordSystemInfo& info);
    bool RemoveCoordSystem(const std::string& name);
    
    bool HasCoordSystem(const std::string& name) const;
    
    static std::string WGS84() { return "EPSG:4326"; }
    static std::string WGS84_UTM(int zone, bool north = true);
    static std::string WebMercator() { return "EPSG:3857"; }
    static std::string CGCS2000() { return "EPSG:4490"; }
    static std::string CGCS2000_UTM(int zone, bool north = true);
    static std::string Beijing54() { return "EPSG:4214"; }
    static std::string Xian80() { return "EPSG:4610"; }
    static std::string GCJ02() { return "GCJ02"; }
    static std::string BD09() { return "BD09"; }
    
    static bool IsChineseCoordSystem(const std::string& crs);
    static bool IsGeographicCRS(const std::string& crs);
    static bool IsProjectedCRS(const std::string& crs);
    
private:
    CoordSystemPreset();
    ~CoordSystemPreset();
    
    void InitializeDefaultSystems();
    void AddChineseCoordSystems();
    
    std::map<std::string, CoordSystemInfo> m_systems;
};

}
}

#endif
