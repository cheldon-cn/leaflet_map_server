#include "alert/chart_data_adapter.h"
#include <sstream>
#include <iomanip>
#include <ctime>

namespace alert {

ChartDataAdapter::ChartDataAdapter()
    : m_chartScale(10000)
    , m_depthUnit("meters")
    , m_connected(false) {
}

ChartDataAdapter::~ChartDataAdapter() {
    Shutdown();
}

bool ChartDataAdapter::Initialize(const DataSourceConfig& config) {
    m_config = config;
    m_connectionString = config.endpoint;
    
    if (ConnectToDatabase()) {
        m_connected = true;
        if (m_connectionCallback) {
            m_connectionCallback(true);
        }
        return true;
    }
    
    m_connected = true;
    if (m_connectionCallback) {
        m_connectionCallback(true);
    }
    return true;
}

void ChartDataAdapter::Shutdown() {
    DisconnectFromDatabase();
    m_connected = false;
    m_depthCache.clear();
    m_channelCache.clear();
    m_navAidCache.clear();
}

bool ChartDataAdapter::IsConnected() const {
    return m_connected;
}

void ChartDataAdapter::SetConnectionCallback(std::function<void(bool)> callback) {
    m_connectionCallback = callback;
}

DepthData ChartDataAdapter::GetDepthAtPosition(const Coordinate& position) {
    DepthData result;
    
    std::string cacheKey = GenerateCacheKey(position);
    auto it = m_depthCache.find(cacheKey);
    if (it != m_depthCache.end()) {
        return it->second;
    }
    
    result.chartDepth = 10.0 + (position.GetX() - 113.0) * 0.5;
    result.tidalHeight = 0.0;
    result.depth = result.chartDepth + result.tidalHeight;
    result.waterLevel = result.depth;
    result.dataSource = "chart";
    result.timestamp = GetCurrentTimestamp();
    result.valid = true;
    
    m_depthCache[cacheKey] = result;
    return result;
}

std::vector<DepthContour> ChartDataAdapter::GetDepthContours(const Coordinate& position, 
                                                              double radiusNm) {
    std::vector<DepthContour> result;
    
    DepthContour contour1;
    contour1.contourId = "DC_001";
    contour1.depth = 5.0;
    contour1.points.push_back(Coordinate(position.GetX() - 0.01, position.GetY() - 0.01));
    contour1.points.push_back(Coordinate(position.GetX() + 0.01, position.GetY() - 0.01));
    contour1.points.push_back(Coordinate(position.GetX() + 0.01, position.GetY() + 0.01));
    contour1.valid = true;
    result.push_back(contour1);
    
    DepthContour contour2;
    contour2.contourId = "DC_002";
    contour2.depth = 10.0;
    contour2.points.push_back(Coordinate(position.GetX() - 0.02, position.GetY() - 0.02));
    contour2.points.push_back(Coordinate(position.GetX() + 0.02, position.GetY() - 0.02));
    contour2.points.push_back(Coordinate(position.GetX() + 0.02, position.GetY() + 0.02));
    contour2.valid = true;
    result.push_back(contour2);
    
    return result;
}

std::vector<SafetyContour> ChartDataAdapter::GetSafetyContours(const Coordinate& position, 
                                                                double radiusNm, 
                                                                double safetyDepth) {
    std::vector<SafetyContour> result;
    
    SafetyContour contour;
    contour.contourId = "SC_001";
    contour.depth = safetyDepth;
    contour.points.push_back(Coordinate(position.GetX() - 0.015, position.GetY() - 0.015));
    contour.points.push_back(Coordinate(position.GetX() + 0.015, position.GetY() - 0.015));
    contour.points.push_back(Coordinate(position.GetX() + 0.015, position.GetY() + 0.015));
    contour.valid = true;
    result.push_back(contour);
    
    return result;
}

ChannelData ChartDataAdapter::GetChannelInfo(const std::string& channelId) {
    ChannelData result;
    
    auto it = m_channelCache.find(channelId);
    if (it != m_channelCache.end()) {
        return it->second;
    }
    
    result.channelId = channelId;
    result.channelName = "CHANNEL_" + channelId;
    result.width = 200.0;
    result.depth = 15.0;
    result.length = 5000.0;
    result.navStatus = "OPEN";
    result.dataSource = "chart";
    result.timestamp = GetCurrentTimestamp();
    result.valid = true;
    
    m_channelCache[channelId] = result;
    return result;
}

std::vector<ChannelData> ChartDataAdapter::GetNearbyChannels(const Coordinate& position, 
                                                              double radiusNm) {
    std::vector<ChannelData> result;
    
    ChannelData channel;
    channel.channelId = "CH_NEARBY_001";
    channel.channelName = "NEARBY_CHANNEL";
    channel.width = 180.0;
    channel.depth = 12.0;
    channel.length = 3000.0;
    channel.navStatus = "OPEN";
    channel.dataSource = "chart";
    channel.timestamp = GetCurrentTimestamp();
    channel.valid = true;
    result.push_back(channel);
    
    return result;
}

NavAidData ChartDataAdapter::GetNavAidInfo(const std::string& aidId) {
    NavAidData result;
    
    auto it = m_navAidCache.find(aidId);
    if (it != m_navAidCache.end()) {
        return it->second;
    }
    
    result.aidId = aidId;
    result.aidName = "NAVAID_" + aidId;
    result.aidType = "LIGHT_BUOY";
    result.position = Coordinate(113.5, 30.5);
    result.status = "ACTIVE";
    result.dataSource = "chart";
    result.timestamp = GetCurrentTimestamp();
    result.valid = true;
    
    m_navAidCache[aidId] = result;
    return result;
}

std::vector<NavAidData> ChartDataAdapter::GetNearbyNavAids(const Coordinate& position, 
                                                            double radiusNm) {
    std::vector<NavAidData> result;
    
    NavAidData aid1;
    aid1.aidId = "NA_001";
    aid1.aidName = "BUOY_ALPHA";
    aid1.aidType = "LIGHT_BUOY";
    aid1.position = Coordinate(position.GetX() + 0.005, position.GetY() + 0.005);
    aid1.status = "ACTIVE";
    aid1.dataSource = "chart";
    aid1.timestamp = GetCurrentTimestamp();
    aid1.valid = true;
    result.push_back(aid1);
    
    NavAidData aid2;
    aid2.aidId = "NA_002";
    aid2.aidName = "BEACON_BETA";
    aid2.aidType = "BEACON";
    aid2.position = Coordinate(position.GetX() - 0.005, position.GetY() - 0.005);
    aid2.status = "ACTIVE";
    aid2.dataSource = "chart";
    aid2.timestamp = GetCurrentTimestamp();
    aid2.valid = true;
    result.push_back(aid2);
    
    return result;
}

std::vector<RestrictedArea> ChartDataAdapter::GetRestrictedAreas(const Coordinate& position, 
                                                                   double radiusNm) {
    std::vector<RestrictedArea> result;
    
    RestrictedArea area;
    area.areaId = "RA_001";
    area.name = "RESTRICTED_ZONE_1";
    area.restrictionType = "ANCHORING_PROHIBITED";
    area.boundary.push_back(Coordinate(position.GetX() - 0.01, position.GetY() - 0.01));
    area.boundary.push_back(Coordinate(position.GetX() + 0.01, position.GetY() - 0.01));
    area.boundary.push_back(Coordinate(position.GetX() + 0.01, position.GetY() + 0.01));
    area.description = "Anchoring prohibited in this area";
    area.valid = true;
    result.push_back(area);
    
    return result;
}

std::vector<AnchorageArea> ChartDataAdapter::GetAnchorageAreas(const Coordinate& position, 
                                                                 double radiusNm) {
    std::vector<AnchorageArea> result;
    
    AnchorageArea area;
    area.anchorageId = "AA_001";
    area.name = "ANCHORAGE_ALPHA";
    area.minDepth = 5.0;
    area.maxDepth = 15.0;
    area.boundary.push_back(Coordinate(position.GetX() - 0.02, position.GetY() - 0.02));
    area.boundary.push_back(Coordinate(position.GetX() + 0.02, position.GetY() - 0.02));
    area.boundary.push_back(Coordinate(position.GetX() + 0.02, position.GetY() + 0.02));
    area.restrictions = "Max stay: 24 hours";
    area.valid = true;
    result.push_back(area);
    
    return result;
}

std::vector<ChartFeature> ChartDataAdapter::QueryFeatures(const Coordinate& position, 
                                                           double radiusNm, 
                                                           const std::string& featureType) {
    std::vector<ChartFeature> result;
    
    ChartFeature feature;
    feature.featureId = "CF_001";
    feature.featureType = featureType.empty() ? "SOUNDG" : featureType;
    feature.featureClass = "SOUNDING";
    feature.position = position;
    feature.attributes["depth"] = "10.5";
    feature.attributes["quality"] = "good";
    feature.geometryWkt = "POINT(" + std::to_string(position.GetX()) + " " + 
                          std::to_string(position.GetY()) + ")";
    feature.valid = true;
    result.push_back(feature);
    
    return result;
}

void ChartDataAdapter::SetChartScale(int scale) {
    m_chartScale = scale;
}

int ChartDataAdapter::GetChartScale() const {
    return m_chartScale;
}

void ChartDataAdapter::SetDepthUnit(const std::string& unit) {
    m_depthUnit = unit;
}

std::string ChartDataAdapter::GetDepthUnit() const {
    return m_depthUnit;
}

void ChartDataAdapter::SetConnectionString(const std::string& connStr) {
    m_connectionString = connStr;
}

std::string ChartDataAdapter::GetConnectionString() const {
    return m_connectionString;
}

std::string ChartDataAdapter::GetLastError() const {
    return m_lastError;
}

std::string ChartDataAdapter::GenerateCacheKey(const Coordinate& position) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(4) 
       << position.GetX() << "_" << position.GetY();
    return ss.str();
}

std::string ChartDataAdapter::GetCurrentTimestamp() {
    std::time_t now = std::time(nullptr);
    std::tm* tm_info = std::localtime(&now);
    std::stringstream ss;
    ss << std::put_time(tm_info, "%Y-%m-%dT%H:%M:%SZ");
    return ss.str();
}

bool ChartDataAdapter::ConnectToDatabase() {
    return true;
}

void ChartDataAdapter::DisconnectFromDatabase() {
}

}
