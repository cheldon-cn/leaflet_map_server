#ifndef OGC_DRAW_LOD_H
#define OGC_DRAW_LOD_H

#include "ogc/draw/export.h"
#include "ogc/envelope.h"
#include <memory>
#include <string>
#include <vector>

namespace ogc {
namespace draw {

class LODLevel;
typedef std::shared_ptr<LODLevel> LODLevelPtr;

class LODLevel {
public:
    LODLevel();
    LODLevel(int level, double minScale, double maxScale);
    ~LODLevel() = default;
    
    void SetLevel(int level);
    int GetLevel() const;
    
    void SetMinScale(double scale);
    double GetMinScale() const;
    
    void SetMaxScale(double scale);
    double GetMaxScale() const;
    
    void SetResolution(double resolution);
    double GetResolution() const;
    
    void SetExtent(const Envelope& extent);
    Envelope GetExtent() const;
    
    void SetName(const std::string& name);
    std::string GetName() const;
    
    void SetDescription(const std::string& desc);
    std::string GetDescription() const;
    
    bool IsVisibleAtScale(double scale) const;
    bool IsVisibleAtResolution(double resolution) const;
    
    bool IsValid() const;
    
    static LODLevelPtr Create();
    static LODLevelPtr Create(int level, double minScale, double maxScale);

private:
    int m_level;
    double m_minScale;
    double m_maxScale;
    double m_resolution;
    Envelope m_extent;
    std::string m_name;
    std::string m_description;
};

class LODStrategy;
typedef std::shared_ptr<LODStrategy> LODStrategyPtr;

class OGC_GRAPH_API LODStrategy {
public:
    virtual ~LODStrategy() = default;
    
    virtual int SelectLOD(double scale, const std::vector<LODLevelPtr>& levels) const = 0;
    
    virtual int SelectLODByResolution(double resolution, const std::vector<LODLevelPtr>& levels) const = 0;
    
    virtual int SelectLODByExtent(const Envelope& extent, int pixelWidth, int pixelHeight, 
                                   const std::vector<LODLevelPtr>& levels) const = 0;
    
    virtual std::vector<int> GetVisibleLODs(double scale, const std::vector<LODLevelPtr>& levels) const = 0;
    
    virtual std::string GetName() const = 0;
    virtual std::string GetDescription() const = 0;
    
    virtual LODStrategyPtr Clone() const = 0;
    
    static LODStrategyPtr CreateDefault();
};

}
}

#endif
