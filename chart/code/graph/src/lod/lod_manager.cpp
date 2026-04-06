#include "ogc/graph/lod/lod_manager.h"
#include <algorithm>
#include <cmath>

namespace ogc {
namespace graph {

namespace {
    const double WEB_MERCATOR_RESOLUTION_0 = 156543.03392804062;
    const double INCHES_PER_METER = 39.37;
    const double DPI_DEFAULT = 96.0;
}

LODManager::LODManager()
    : m_strategy(LODStrategy::CreateDefault())
    , m_minLOD(0)
    , m_maxLOD(18)
    , m_transitionFactor(1.5)
    , m_enableTransitions(true) {
}

LODManager::LODManager(LODStrategyPtr strategy)
    : m_strategy(strategy ? strategy : LODStrategy::CreateDefault())
    , m_minLOD(0)
    , m_maxLOD(18)
    , m_transitionFactor(1.5)
    , m_enableTransitions(true) {
}

void LODManager::SetStrategy(LODStrategyPtr strategy) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_strategy = strategy;
}

LODStrategyPtr LODManager::GetStrategy() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_strategy;
}

void LODManager::AddLODLevel(LODLevelPtr level) {
    if (!level) return;
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = std::find_if(m_levels.begin(), m_levels.end(),
        [&level](const LODLevelPtr& l) {
            return l && l->GetLevel() == level->GetLevel();
        });
    
    if (it != m_levels.end()) {
        *it = level;
    } else {
        m_levels.push_back(level);
        std::sort(m_levels.begin(), m_levels.end(),
            [](const LODLevelPtr& a, const LODLevelPtr& b) {
                return a->GetLevel() < b->GetLevel();
            });
    }
}

void LODManager::RemoveLODLevel(int level) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_levels.erase(
        std::remove_if(m_levels.begin(), m_levels.end(),
            [level](const LODLevelPtr& l) {
                return l && l->GetLevel() == level;
            }),
        m_levels.end());
}

void LODManager::ClearLODLevels() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_levels.clear();
}

LODLevelPtr LODManager::GetLODLevel(int level) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = std::find_if(m_levels.begin(), m_levels.end(),
        [level](const LODLevelPtr& l) {
            return l && l->GetLevel() == level;
        });
    
    return it != m_levels.end() ? *it : nullptr;
}

LODLevelPtr LODManager::GetLODLevelInternal(int level) const {
    auto it = std::find_if(m_levels.begin(), m_levels.end(),
        [level](const LODLevelPtr& l) {
            return l && l->GetLevel() == level;
        });
    
    return it != m_levels.end() ? *it : nullptr;
}

std::vector<LODLevelPtr> LODManager::GetLODLevels() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_levels;
}

size_t LODManager::GetLODCount() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_levels.size();
}

int LODManager::SelectLOD(double scale) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_strategy || m_levels.empty()) {
        return -1;
    }
    
    int lod = m_strategy->SelectLOD(scale, m_levels);
    return IsLODInRange(lod) ? lod : -1;
}

int LODManager::SelectLODByResolution(double resolution) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_strategy || m_levels.empty()) {
        return -1;
    }
    
    int lod = m_strategy->SelectLODByResolution(resolution, m_levels);
    return IsLODInRange(lod) ? lod : -1;
}

int LODManager::SelectLODByExtent(const Envelope& extent, int pixelWidth, int pixelHeight) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_strategy || m_levels.empty()) {
        return -1;
    }
    
    int lod = m_strategy->SelectLODByExtent(extent, pixelWidth, pixelHeight, m_levels);
    return IsLODInRange(lod) ? lod : -1;
}

std::vector<int> LODManager::GetVisibleLODs(double scale) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_strategy) {
        return {};
    }
    
    auto visible = m_strategy->GetVisibleLODs(scale, m_levels);
    
    visible.erase(
        std::remove_if(visible.begin(), visible.end(),
            [this](int lod) { return !IsLODInRange(lod); }),
        visible.end());
    
    return visible;
}

void LODManager::SetMinLOD(int minLOD) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_minLOD = minLOD;
}

int LODManager::GetMinLOD() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_minLOD;
}

void LODManager::SetMaxLOD(int maxLOD) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_maxLOD = maxLOD;
}

int LODManager::GetMaxLOD() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_maxLOD;
}

void LODManager::SetLODRange(int minLOD, int maxLOD) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_minLOD = minLOD;
    m_maxLOD = maxLOD;
}

bool LODManager::IsLODVisible(int lod) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return IsLODInRange(lod);
}

void LODManager::SetTransitionFactor(double factor) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_transitionFactor = factor;
}

double LODManager::GetTransitionFactor() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_transitionFactor;
}

void LODManager::SetEnableTransitions(bool enable) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_enableTransitions = enable;
}

bool LODManager::IsTransitionsEnabled() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_enableTransitions;
}

void LODManager::CreateDefaultLODs(int minLevel, int maxLevel, double baseResolution) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_levels.clear();
    m_minLOD = minLevel;
    m_maxLOD = maxLevel;
    
    for (int i = minLevel; i <= maxLevel; ++i) {
        double resolution = baseResolution / std::pow(2.0, i - minLevel);
        auto level = LODLevel::Create(i, 0.0, 0.0);
        level->SetResolution(resolution);
        level->SetName("Level " + std::to_string(i));
        m_levels.push_back(level);
    }
}

void LODManager::CreateWebMercatorLODs(int minLevel, int maxLevel) {
    CreateDefaultLODs(minLevel, maxLevel, WEB_MERCATOR_RESOLUTION_0);
}

double LODManager::GetResolutionForLevel(int level) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto lodLevel = GetLODLevelInternal(level);
    if (lodLevel) {
        return lodLevel->GetResolution();
    }
    
    return WEB_MERCATOR_RESOLUTION_0 / std::pow(2.0, level);
}

int LODManager::GetLevelForResolution(double resolution) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    for (const auto& level : m_levels) {
        if (level) {
            double levelRes = level->GetResolution();
            if (levelRes > 0.0 && resolution >= levelRes / m_transitionFactor) {
                return level->GetLevel();
            }
        }
    }
    
    return m_levels.empty() ? 0 : m_levels.back()->GetLevel();
}

double LODManager::GetScaleForLevel(int level, double dpi) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto lodLevel = GetLODLevelInternal(level);
    double resolution = 0.0;
    if (lodLevel) {
        resolution = lodLevel->GetResolution();
    } else {
        resolution = WEB_MERCATOR_RESOLUTION_0 / std::pow(2.0, level);
    }
    
    return resolution * dpi * INCHES_PER_METER;
}

int LODManager::GetLevelForScale(double scale, double dpi) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    double resolution = scale / (dpi * INCHES_PER_METER);
    
    for (const auto& level : m_levels) {
        if (level) {
            double levelRes = level->GetResolution();
            if (levelRes > 0.0 && resolution >= levelRes / m_transitionFactor) {
                return level->GetLevel();
            }
        }
    }
    
    return m_levels.empty() ? 0 : m_levels.back()->GetLevel();
}

bool LODManager::IsLODInRange(int lod) const {
    return lod >= m_minLOD && lod <= m_maxLOD;
}

LODManagerPtr LODManager::Create() {
    return std::make_shared<LODManager>();
}

LODManagerPtr LODManager::Create(LODStrategyPtr strategy) {
    return std::make_shared<LODManager>(strategy);
}

LODManagerPtr LODManager::CreateWebMercator(int minLevel, int maxLevel) {
    auto manager = std::make_shared<LODManager>();
    manager->CreateWebMercatorLODs(minLevel, maxLevel);
    return manager;
}

}
}
