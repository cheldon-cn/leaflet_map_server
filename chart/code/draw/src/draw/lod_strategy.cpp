#include "ogc/draw/lod_strategy.h"
#include <algorithm>
#include <cmath>

namespace ogc {
namespace draw {

LODStrategy& LODStrategy::Instance() {
    static LODStrategy instance;
    return instance;
}

LODStrategy::LODStrategy()
    : m_defaultStrategy("distance")
    , m_distanceScale(1.0)
    , m_importanceFactor(1.0) {
    InitializeDefaultLevels();
}

LODStrategy::~LODStrategy() {
}

int LODStrategy::CalculateLOD(double distance, double importance) const {
    if (m_customCalculator) {
        return m_customCalculator(distance, importance);
    }
    
    double adjustedDistance = distance * m_distanceScale / (importance * m_importanceFactor);
    
    for (int i = 0; i < static_cast<int>(m_lodLevels.size()); ++i) {
        const auto& level = m_lodLevels[i];
        if (adjustedDistance >= level.minDistance && adjustedDistance < level.maxDistance) {
            return level.level;
        }
    }
    
    if (adjustedDistance < m_lodLevels.front().minDistance) {
        return m_lodLevels.front().level;
    }
    
    return m_lodLevels.back().level;
}

int LODStrategy::CalculateLODFromScale(double scale) const {
    if (scale >= 1.0) {
        return 0;
    } else if (scale >= 0.5) {
        return 1;
    } else if (scale >= 0.25) {
        return 2;
    } else if (scale >= 0.125) {
        return 3;
    } else {
        return 4;
    }
}

double LODStrategy::GetSimplificationFactor(int lod) const {
    for (const auto& level : m_lodLevels) {
        if (level.level == lod) {
            return level.simplification;
        }
    }
    return 1.0;
}

int LODStrategy::GetDetailLevel(int lod) const {
    for (const auto& level : m_lodLevels) {
        if (level.level == lod) {
            return level.detailLevel;
        }
    }
    return 0;
}

void LODStrategy::AddLODLevel(const LODLevel& level) {
    m_lodLevels.push_back(level);
    std::sort(m_lodLevels.begin(), m_lodLevels.end(),
        [](const LODLevel& a, const LODLevel& b) {
            return a.level < b.level;
        });
}

void LODStrategy::ClearLODLevels() {
    m_lodLevels.clear();
}

void LODStrategy::SetDefaultStrategy(const std::string& strategy) {
    m_defaultStrategy = strategy;
}

void LODStrategy::SetDistanceScale(double scale) {
    m_distanceScale = scale > 0 ? scale : 1.0;
}

void LODStrategy::SetImportanceFactor(double factor) {
    m_importanceFactor = factor > 0 ? factor : 1.0;
}

void LODStrategy::SetCustomLODCalculator(std::function<int(double, double)> calculator) {
    m_customCalculator = calculator;
}

bool LODStrategy::IsValidLOD(int lod) const {
    for (const auto& level : m_lodLevels) {
        if (level.level == lod) {
            return true;
        }
    }
    return false;
}

int LODStrategy::ClampLOD(int lod) const {
    if (m_lodLevels.empty()) {
        return 0;
    }
    
    int minLOD = m_lodLevels.front().level;
    int maxLOD = m_lodLevels.back().level;
    
    return std::max(minLOD, std::min(maxLOD, lod));
}

double LODStrategy::EstimateMemoryUsage(int lod) const {
    double baseMemory = 1.0;
    double simplification = GetSimplificationFactor(lod);
    return baseMemory * simplification;
}

double LODStrategy::EstimateRenderTime(int lod) const {
    double baseTime = 1.0;
    double simplification = GetSimplificationFactor(lod);
    return baseTime * simplification;
}

void LODStrategy::InitializeDefaultLevels() {
    m_lodLevels.clear();
    
    LODLevel level0;
    level0.level = 0;
    level0.minDistance = 0.0;
    level0.maxDistance = 100.0;
    level0.simplification = 1.0;
    level0.detailLevel = 100;
    m_lodLevels.push_back(level0);
    
    LODLevel level1;
    level1.level = 1;
    level1.minDistance = 100.0;
    level1.maxDistance = 500.0;
    level1.simplification = 0.75;
    level1.detailLevel = 75;
    m_lodLevels.push_back(level1);
    
    LODLevel level2;
    level2.level = 2;
    level2.minDistance = 500.0;
    level2.maxDistance = 1000.0;
    level2.simplification = 0.5;
    level2.detailLevel = 50;
    m_lodLevels.push_back(level2);
    
    LODLevel level3;
    level3.level = 3;
    level3.minDistance = 1000.0;
    level3.maxDistance = 5000.0;
    level3.simplification = 0.25;
    level3.detailLevel = 25;
    m_lodLevels.push_back(level3);
    
    LODLevel level4;
    level4.level = 4;
    level4.minDistance = 5000.0;
    level4.maxDistance = 100000.0;
    level4.simplification = 0.1;
    level4.detailLevel = 10;
    m_lodLevels.push_back(level4);
}

}
}
