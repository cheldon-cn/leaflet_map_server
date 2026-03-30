#ifndef OGC_DRAW_LOD_STRATEGY_H
#define OGC_DRAW_LOD_STRATEGY_H

#include "ogc/draw/export.h"
#include <vector>
#include <functional>

namespace ogc {
namespace draw {

struct LODLevel {
    int level;
    double minDistance;
    double maxDistance;
    double simplification;
    int detailLevel;
};

class OGC_DRAW_API LODStrategy {
public:
    static LODStrategy& Instance();
    
    int CalculateLOD(double distance, double importance = 1.0) const;
    int CalculateLODFromScale(double scale) const;
    
    double GetSimplificationFactor(int lod) const;
    int GetDetailLevel(int lod) const;
    
    void AddLODLevel(const LODLevel& level);
    void ClearLODLevels();
    int GetLODLevelCount() const { return static_cast<int>(m_lodLevels.size()); }
    
    void SetDefaultStrategy(const std::string& strategy);
    std::string GetDefaultStrategy() const { return m_defaultStrategy; }
    
    void SetDistanceScale(double scale);
    double GetDistanceScale() const { return m_distanceScale; }
    
    void SetImportanceFactor(double factor);
    double GetImportanceFactor() const { return m_importanceFactor; }
    
    const std::vector<LODLevel>& GetLODLevels() const { return m_lodLevels; }
    
    void SetCustomLODCalculator(std::function<int(double, double)> calculator);
    
    bool IsValidLOD(int lod) const;
    int ClampLOD(int lod) const;
    
    double EstimateMemoryUsage(int lod) const;
    double EstimateRenderTime(int lod) const;

private:
    LODStrategy();
    ~LODStrategy();
    
    LODStrategy(const LODStrategy&) = delete;
    LODStrategy& operator=(const LODStrategy&) = delete;
    
    void InitializeDefaultLevels();
    
    std::vector<LODLevel> m_lodLevels;
    std::string m_defaultStrategy;
    double m_distanceScale;
    double m_importanceFactor;
    std::function<int(double, double)> m_customCalculator;
};

}
}

#endif
