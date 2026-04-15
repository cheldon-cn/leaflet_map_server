#ifndef OGC_GRAPH_LOD_MANAGER_H
#define OGC_GRAPH_LOD_MANAGER_H

#include "ogc/graph/export.h"
#include "ogc/graph/lod/lod.h"
#include "ogc/geom/envelope.h"
#include <memory>
#include <string>
#include <vector>
#include <mutex>

namespace ogc {
namespace graph {

class LODManager;
typedef std::shared_ptr<LODManager> LODManagerPtr;

class OGC_GRAPH_API LODManager {
public:
    LODManager();
    explicit LODManager(LODStrategyPtr strategy);
    ~LODManager();
    
    void SetStrategy(LODStrategyPtr strategy);
    LODStrategyPtr GetStrategy() const;
    
    void AddLODLevel(LODLevelPtr level);
    void RemoveLODLevel(int level);
    void ClearLODLevels();
    
    LODLevelPtr GetLODLevel(int level) const;
    std::vector<LODLevelPtr> GetLODLevels() const;
    size_t GetLODCount() const;
    
    int SelectLOD(double scale) const;
    int SelectLODByResolution(double resolution) const;
    int SelectLODByExtent(const Envelope& extent, int pixelWidth, int pixelHeight) const;
    
    std::vector<int> GetVisibleLODs(double scale) const;
    
    void SetMinLOD(int minLOD);
    int GetMinLOD() const;
    
    void SetMaxLOD(int maxLOD);
    int GetMaxLOD() const;
    
    void SetLODRange(int minLOD, int maxLOD);
    
    bool IsLODVisible(int lod) const;
    
    void SetTransitionFactor(double factor);
    double GetTransitionFactor() const;
    
    void SetEnableTransitions(bool enable);
    bool IsTransitionsEnabled() const;
    
    void CreateDefaultLODs(int minLevel, int maxLevel, double baseResolution);
    
    void CreateWebMercatorLODs(int minLevel = 0, int maxLevel = 18);
    
    double GetResolutionForLevel(int level) const;
    int GetLevelForResolution(double resolution) const;
    
    double GetScaleForLevel(int level, double dpi = 96.0) const;
    int GetLevelForScale(double scale, double dpi = 96.0) const;
    
    static LODManagerPtr Create();
    static LODManagerPtr Create(LODStrategyPtr strategy);
    static LODManagerPtr CreateWebMercator(int minLevel = 0, int maxLevel = 18);

private:
    bool IsLODInRange(int lod) const;
    LODLevelPtr GetLODLevelInternal(int level) const;
    
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}
}

#endif
