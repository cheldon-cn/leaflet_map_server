#ifndef ALERT_CHART_DATA_ADAPTER_H
#define ALERT_CHART_DATA_ADAPTER_H

#include "alert/external_data_gateway.h"
#include "alert/coordinate.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace alert {

struct ChartFeature {
    std::string featureId;
    std::string featureType;
    std::string featureClass;
    Coordinate position;
    std::map<std::string, std::string> attributes;
    std::string geometryWkt;
    bool valid;
    
    ChartFeature()
        : valid(false) {}
};

struct DepthContour {
    std::string contourId;
    double depth;
    std::vector<Coordinate> points;
    bool valid;
    
    DepthContour()
        : depth(0), valid(false) {}
};

struct SafetyContour {
    std::string contourId;
    double depth;
    std::vector<Coordinate> points;
    bool valid;
    
    SafetyContour()
        : depth(0), valid(false) {}
};

struct RestrictedArea {
    std::string areaId;
    std::string name;
    std::string restrictionType;
    std::vector<Coordinate> boundary;
    std::string description;
    bool valid;
    
    RestrictedArea()
        : valid(false) {}
};

struct AnchorageArea {
    std::string anchorageId;
    std::string name;
    double minDepth;
    double maxDepth;
    std::vector<Coordinate> boundary;
    std::string restrictions;
    bool valid;
    
    AnchorageArea()
        : minDepth(0), maxDepth(0), valid(false) {}
};

class IChartDataAdapter : public IDataProvider {
public:
    virtual ~IChartDataAdapter() {}
    
    virtual DepthData GetDepthAtPosition(const Coordinate& position) = 0;
    virtual std::vector<DepthContour> GetDepthContours(const Coordinate& position, double radiusNm) = 0;
    virtual std::vector<SafetyContour> GetSafetyContours(const Coordinate& position, double radiusNm, double safetyDepth) = 0;
    
    virtual ChannelData GetChannelInfo(const std::string& channelId) = 0;
    virtual std::vector<ChannelData> GetNearbyChannels(const Coordinate& position, double radiusNm) = 0;
    
    virtual NavAidData GetNavAidInfo(const std::string& aidId) = 0;
    virtual std::vector<NavAidData> GetNearbyNavAids(const Coordinate& position, double radiusNm) = 0;
    
    virtual std::vector<RestrictedArea> GetRestrictedAreas(const Coordinate& position, double radiusNm) = 0;
    virtual std::vector<AnchorageArea> GetAnchorageAreas(const Coordinate& position, double radiusNm) = 0;
    
    virtual std::vector<ChartFeature> QueryFeatures(const Coordinate& position, double radiusNm, const std::string& featureType) = 0;
    
    virtual void SetChartScale(int scale) = 0;
    virtual int GetChartScale() const = 0;
    
    virtual void SetDepthUnit(const std::string& unit) = 0;
    virtual std::string GetDepthUnit() const = 0;
};

class ChartDataAdapter : public IChartDataAdapter {
public:
    ChartDataAdapter();
    virtual ~ChartDataAdapter();
    
    std::string GetProviderId() const override { return "chart"; }
    std::string GetProviderType() const override { return "chart"; }
    
    bool Initialize(const DataSourceConfig& config) override;
    void Shutdown() override;
    bool IsConnected() const override;
    void SetConnectionCallback(std::function<void(bool)> callback) override;
    
    DepthData GetDepthAtPosition(const Coordinate& position) override;
    std::vector<DepthContour> GetDepthContours(const Coordinate& position, double radiusNm) override;
    std::vector<SafetyContour> GetSafetyContours(const Coordinate& position, double radiusNm, double safetyDepth) override;
    
    ChannelData GetChannelInfo(const std::string& channelId) override;
    std::vector<ChannelData> GetNearbyChannels(const Coordinate& position, double radiusNm) override;
    
    NavAidData GetNavAidInfo(const std::string& aidId) override;
    std::vector<NavAidData> GetNearbyNavAids(const Coordinate& position, double radiusNm) override;
    
    std::vector<RestrictedArea> GetRestrictedAreas(const Coordinate& position, double radiusNm) override;
    std::vector<AnchorageArea> GetAnchorageAreas(const Coordinate& position, double radiusNm) override;
    
    std::vector<ChartFeature> QueryFeatures(const Coordinate& position, double radiusNm, const std::string& featureType) override;
    
    void SetChartScale(int scale) override;
    int GetChartScale() const override;
    
    void SetDepthUnit(const std::string& unit) override;
    std::string GetDepthUnit() const override;
    
    void SetConnectionString(const std::string& connStr);
    std::string GetConnectionString() const;
    
    std::string GetLastError() const;
    
private:
    DataSourceConfig m_config;
    std::string m_connectionString;
    std::function<void(bool)> m_connectionCallback;
    int m_chartScale;
    std::string m_depthUnit;
    bool m_connected;
    std::string m_lastError;
    
    std::map<std::string, DepthData> m_depthCache;
    std::map<std::string, ChannelData> m_channelCache;
    std::map<std::string, NavAidData> m_navAidCache;
    
    std::string GenerateCacheKey(const Coordinate& position);
    std::string GetCurrentTimestamp();
    bool ConnectToDatabase();
    void DisconnectFromDatabase();
};

}

#endif
