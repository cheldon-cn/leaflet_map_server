#include "ogc/draw/lod.h"
#include <algorithm>
#include <cmath>

namespace ogc {
namespace draw {

LODLevel::LODLevel()
    : m_level(0)
    , m_minScale(0.0)
    , m_maxScale(0.0)
    , m_resolution(0.0) {
}

LODLevel::LODLevel(int level, double minScale, double maxScale)
    : m_level(level)
    , m_minScale(minScale)
    , m_maxScale(maxScale)
    , m_resolution(0.0) {
}

void LODLevel::SetLevel(int level) {
    m_level = level;
}

int LODLevel::GetLevel() const {
    return m_level;
}

void LODLevel::SetMinScale(double scale) {
    m_minScale = scale;
}

double LODLevel::GetMinScale() const {
    return m_minScale;
}

void LODLevel::SetMaxScale(double scale) {
    m_maxScale = scale;
}

double LODLevel::GetMaxScale() const {
    return m_maxScale;
}

void LODLevel::SetResolution(double resolution) {
    m_resolution = resolution;
}

double LODLevel::GetResolution() const {
    return m_resolution;
}

void LODLevel::SetExtent(const Envelope& extent) {
    m_extent = extent;
}

Envelope LODLevel::GetExtent() const {
    return m_extent;
}

void LODLevel::SetName(const std::string& name) {
    m_name = name;
}

std::string LODLevel::GetName() const {
    return m_name;
}

void LODLevel::SetDescription(const std::string& desc) {
    m_description = desc;
}

std::string LODLevel::GetDescription() const {
    return m_description;
}

bool LODLevel::IsVisibleAtScale(double scale) const {
    if (m_minScale <= 0.0 && m_maxScale <= 0.0) {
        return true;
    }
    
    if (m_minScale <= 0.0) {
        return scale <= m_maxScale;
    }
    
    if (m_maxScale <= 0.0) {
        return scale >= m_minScale;
    }
    
    return scale >= m_minScale && scale <= m_maxScale;
}

bool LODLevel::IsVisibleAtResolution(double resolution) const {
    if (m_resolution <= 0.0) {
        return true;
    }
    
    double tolerance = m_resolution * 0.1;
    return std::abs(resolution - m_resolution) <= tolerance;
}

bool LODLevel::IsValid() const {
    return m_level >= 0;
}

LODLevelPtr LODLevel::Create() {
    return std::make_shared<LODLevel>();
}

LODLevelPtr LODLevel::Create(int level, double minScale, double maxScale) {
    return std::make_shared<LODLevel>(level, minScale, maxScale);
}

class DefaultLODStrategy : public LODStrategy {
public:
    int SelectLOD(double scale, const std::vector<LODLevelPtr>& levels) const override {
        if (levels.empty()) {
            return -1;
        }
        
        for (const auto& level : levels) {
            if (level && level->IsVisibleAtScale(scale)) {
                return level->GetLevel();
            }
        }
        
        if (scale < levels[0]->GetMinScale()) {
            return levels[0]->GetLevel();
        }
        
        return levels.back()->GetLevel();
    }
    
    int SelectLODByResolution(double resolution, const std::vector<LODLevelPtr>& levels) const override {
        if (levels.empty()) {
            return -1;
        }
        
        int bestLevel = -1;
        double minDiff = std::numeric_limits<double>::max();
        
        for (const auto& level : levels) {
            if (!level) continue;
            
            double levelRes = level->GetResolution();
            if (levelRes <= 0.0) continue;
            
            double diff = std::abs(resolution - levelRes);
            if (diff < minDiff) {
                minDiff = diff;
                bestLevel = level->GetLevel();
            }
        }
        
        return bestLevel >= 0 ? bestLevel : levels[0]->GetLevel();
    }
    
    int SelectLODByExtent(const Envelope& extent, int pixelWidth, int pixelHeight,
                          const std::vector<LODLevelPtr>& levels) const override {
        if (levels.empty() || pixelWidth <= 0 || pixelHeight <= 0) {
            return -1;
        }
        
        double extentWidth = extent.GetWidth();
        double extentHeight = extent.GetHeight();
        
        double resolutionX = extentWidth / pixelWidth;
        double resolutionY = extentHeight / pixelHeight;
        double resolution = std::max(resolutionX, resolutionY);
        
        return SelectLODByResolution(resolution, levels);
    }
    
    std::vector<int> GetVisibleLODs(double scale, const std::vector<LODLevelPtr>& levels) const override {
        std::vector<int> visible;
        
        for (const auto& level : levels) {
            if (level && level->IsVisibleAtScale(scale)) {
                visible.push_back(level->GetLevel());
            }
        }
        
        return visible;
    }
    
    std::string GetName() const override {
        return "DefaultLODStrategy";
    }
    
    std::string GetDescription() const override {
        return "Default LOD selection strategy based on scale and resolution";
    }
    
    LODStrategyPtr Clone() const override {
        return std::make_shared<DefaultLODStrategy>(*this);
    }
};

LODStrategyPtr LODStrategy::CreateDefault() {
    return std::make_shared<DefaultLODStrategy>();
}

}
}
