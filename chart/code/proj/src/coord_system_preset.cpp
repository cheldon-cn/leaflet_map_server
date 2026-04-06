#include "ogc/proj/coord_system_preset.h"
#include <algorithm>

namespace ogc {
namespace proj {

CoordSystemPreset& CoordSystemPreset::Instance()
{
    static CoordSystemPreset instance;
    return instance;
}

CoordSystemInfo CoordSystemPreset::GetCoordSystem(const std::string& name) const
{
    auto it = m_systems.find(name);
    if (it != m_systems.end()) {
        return it->second;
    }
    return CoordSystemInfo();
}

CoordSystemInfo CoordSystemPreset::GetCoordSystemByEpsg(const std::string& epsg) const
{
    for (const auto& pair : m_systems) {
        if (pair.second.epsg == epsg) {
            return pair.second;
        }
    }
    return CoordSystemInfo();
}

std::vector<std::string> CoordSystemPreset::GetCoordSystemNames() const
{
    std::vector<std::string> names;
    names.reserve(m_systems.size());
    for (const auto& pair : m_systems) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<CoordSystemInfo> CoordSystemPreset::GetAllCoordSystems() const
{
    std::vector<CoordSystemInfo> systems;
    systems.reserve(m_systems.size());
    for (const auto& pair : m_systems) {
        systems.push_back(pair.second);
    }
    return systems;
}

void CoordSystemPreset::RegisterCoordSystem(const CoordSystemInfo& info)
{
    m_systems[info.name] = info;
}

bool CoordSystemPreset::RemoveCoordSystem(const std::string& name)
{
    return m_systems.erase(name) > 0;
}

bool CoordSystemPreset::HasCoordSystem(const std::string& name) const
{
    return m_systems.find(name) != m_systems.end();
}

std::string CoordSystemPreset::WGS84_UTM(int zone, bool north)
{
    if (zone < 1 || zone > 60) {
        return "";
    }
    int epsg = north ? (32600 + zone) : (32700 + zone);
    return "EPSG:" + std::to_string(epsg);
}

std::string CoordSystemPreset::CGCS2000_UTM(int zone, bool north)
{
    if (zone < 1 || zone > 60) {
        return "";
    }
    int epsg = north ? (4513 + zone - 1) : (4555 + zone - 1);
    return "EPSG:" + std::to_string(epsg);
}

bool CoordSystemPreset::IsChineseCoordSystem(const std::string& crs)
{
    std::string upper = crs;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    
    return upper.find("CGCS2000") != std::string::npos ||
           upper.find("BEIJING54") != std::string::npos ||
           upper.find("XIAN80") != std::string::npos ||
           upper.find("GCJ02") != std::string::npos ||
           upper.find("BD09") != std::string::npos ||
           upper.find("EPSG:4490") != std::string::npos ||
           upper.find("EPSG:4214") != std::string::npos ||
           upper.find("EPSG:4610") != std::string::npos;
}

bool CoordSystemPreset::IsGeographicCRS(const std::string& crs)
{
    std::string upper = crs;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    
    if (upper.find("EPSG:4326") != std::string::npos ||
        upper.find("WGS84") != std::string::npos ||
        upper.find("WGS 84") != std::string::npos ||
        upper.find("EPSG:4490") != std::string::npos ||
        upper.find("CGCS2000") != std::string::npos ||
        upper.find("EPSG:4214") != std::string::npos ||
        upper.find("BEIJING54") != std::string::npos ||
        upper.find("EPSG:4610") != std::string::npos ||
        upper.find("XIAN80") != std::string::npos) {
        return true;
    }
    
    return false;
}

bool CoordSystemPreset::IsProjectedCRS(const std::string& crs)
{
    std::string upper = crs;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    
    if (upper.find("EPSG:3857") != std::string::npos ||
        upper.find("WEBMERCATOR") != std::string::npos ||
        upper.find("WEB MERCATOR") != std::string::npos ||
        upper.find("MERCATOR") != std::string::npos ||
        upper.find("UTM") != std::string::npos) {
        return true;
    }
    
    return false;
}

CoordSystemPreset::CoordSystemPreset()
{
    InitializeDefaultSystems();
    AddChineseCoordSystems();
}

CoordSystemPreset::~CoordSystemPreset()
{
}

void CoordSystemPreset::InitializeDefaultSystems()
{
    CoordSystemInfo wgs84;
    wgs84.name = "WGS84";
    wgs84.epsg = "EPSG:4326";
    wgs84.description = "World Geodetic System 1984";
    wgs84.area = "Global";
    wgs84.accuracy = 0.0;
    m_systems[wgs84.name] = wgs84;
    
    CoordSystemInfo webMercator;
    webMercator.name = "WebMercator";
    webMercator.epsg = "EPSG:3857";
    webMercator.description = "Web Mercator (Pseudo-Mercator)";
    webMercator.area = "Global";
    webMercator.accuracy = 0.0;
    m_systems[webMercator.name] = webMercator;
    
    for (int zone = 1; zone <= 60; ++zone) {
        CoordSystemInfo utmNorth;
        utmNorth.name = "WGS84_UTM_N" + std::to_string(zone);
        utmNorth.epsg = WGS84_UTM(zone, true);
        utmNorth.description = "WGS 84 / UTM zone " + std::to_string(zone) + "N";
        utmNorth.area = "Northern hemisphere, zone " + std::to_string(zone);
        utmNorth.accuracy = 1.0;
        m_systems[utmNorth.name] = utmNorth;
        
        CoordSystemInfo utmSouth;
        utmSouth.name = "WGS84_UTM_S" + std::to_string(zone);
        utmSouth.epsg = WGS84_UTM(zone, false);
        utmSouth.description = "WGS 84 / UTM zone " + std::to_string(zone) + "S";
        utmSouth.area = "Southern hemisphere, zone " + std::to_string(zone);
        utmSouth.accuracy = 1.0;
        m_systems[utmSouth.name] = utmSouth;
    }
}

void CoordSystemPreset::AddChineseCoordSystems()
{
    CoordSystemInfo cgcs2000;
    cgcs2000.name = "CGCS2000";
    cgcs2000.epsg = "EPSG:4490";
    cgcs2000.description = "China Geodetic Coordinate System 2000";
    cgcs2000.area = "China";
    cgcs2000.accuracy = 0.0;
    m_systems[cgcs2000.name] = cgcs2000;
    
    CoordSystemInfo beijing54;
    beijing54.name = "Beijing54";
    beijing54.epsg = "EPSG:4214";
    beijing54.description = "Beijing 1954";
    beijing54.area = "China";
    beijing54.accuracy = 0.5;
    m_systems[beijing54.name] = beijing54;
    
    CoordSystemInfo xian80;
    xian80.name = "Xian80";
    xian80.epsg = "EPSG:4610";
    xian80.description = "Xian 1980";
    xian80.area = "China";
    xian80.accuracy = 0.3;
    m_systems[xian80.name] = xian80;
    
    CoordSystemInfo gcj02;
    gcj02.name = "GCJ02";
    gcj02.epsg = "GCJ02";
    gcj02.description = "China GCJ-02 (Mars Coordinate)";
    gcj02.area = "China";
    gcj02.accuracy = 0.1;
    m_systems[gcj02.name] = gcj02;
    
    CoordSystemInfo bd09;
    bd09.name = "BD09";
    bd09.epsg = "BD09";
    bd09.description = "Baidu BD-09 Coordinate";
    bd09.area = "China";
    bd09.accuracy = 0.1;
    m_systems[bd09.name] = bd09;
    
    for (int zone = 1; zone <= 60; ++zone) {
        CoordSystemInfo cgcsUtmNorth;
        cgcsUtmNorth.name = "CGCS2000_UTM_N" + std::to_string(zone);
        cgcsUtmNorth.epsg = CGCS2000_UTM(zone, true);
        cgcsUtmNorth.description = "CGCS2000 / UTM zone " + std::to_string(zone) + "N";
        cgcsUtmNorth.area = "China, zone " + std::to_string(zone);
        cgcsUtmNorth.accuracy = 1.0;
        m_systems[cgcsUtmNorth.name] = cgcsUtmNorth;
        
        CoordSystemInfo cgcsUtmSouth;
        cgcsUtmSouth.name = "CGCS2000_UTM_S" + std::to_string(zone);
        cgcsUtmSouth.epsg = CGCS2000_UTM(zone, false);
        cgcsUtmSouth.description = "CGCS2000 / UTM zone " + std::to_string(zone) + "S";
        cgcsUtmSouth.area = "China, zone " + std::to_string(zone);
        cgcsUtmSouth.accuracy = 1.0;
        m_systems[cgcsUtmSouth.name] = cgcsUtmSouth;
    }
}

}
}
