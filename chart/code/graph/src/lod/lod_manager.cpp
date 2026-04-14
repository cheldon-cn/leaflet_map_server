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

struct LODManager::Impl {
    LODStrategyPtr strategy;
    std::vector<LODLevelPtr> levels;
    int minLOD;
    int maxLOD;
    double transitionFactor;
    bool enableTransitions;
    mutable std::mutex mutex;
    
    Impl()
        : strategy(LODStrategy::CreateDefault())
        , minLOD(0)
        , maxLOD(18)
        , transitionFactor(1.5)
        , enableTransitions(true) {
    }
    
    Impl(LODStrategyPtr strat)
        : strategy(strat ? strat : LODStrategy::CreateDefault())
        , minLOD(0)
        , maxLOD(18)
        , transitionFactor(1.5)
        , enableTransitions(true) {
    }
};

LODManager::LODManager()
    : impl_(std::make_unique<Impl>()) {
}

LODManager::LODManager(LODStrategyPtr strategy)
    : impl_(std::make_unique<Impl>(strategy)) {
}

LODManager::~LODManager() = default;

void LODManager::SetStrategy(LODStrategyPtr strategy) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->strategy = strategy;
}

LODStrategyPtr LODManager::GetStrategy() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->strategy;
}

void LODManager::AddLODLevel(LODLevelPtr level) {
    if (!level) return;
    
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    auto it = std::find_if(impl_->levels.begin(), impl_->levels.end(),
        [&level](const LODLevelPtr& l) {
            return l && l->GetLevel() == level->GetLevel();
        });
    
    if (it != impl_->levels.end()) {
        *it = level;
    } else {
        impl_->levels.push_back(level);
        std::sort(impl_->levels.begin(), impl_->levels.end(),
            [](const LODLevelPtr& a, const LODLevelPtr& b) {
                return a->GetLevel() < b->GetLevel();
            });
    }
}

void LODManager::RemoveLODLevel(int level) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    impl_->levels.erase(
        std::remove_if(impl_->levels.begin(), impl_->levels.end(),
            [level](const LODLevelPtr& l) {
                return l && l->GetLevel() == level;
            }),
        impl_->levels.end());
}

void LODManager::ClearLODLevels() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->levels.clear();
}

LODLevelPtr LODManager::GetLODLevel(int level) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    auto it = std::find_if(impl_->levels.begin(), impl_->levels.end(),
        [level](const LODLevelPtr& l) {
            return l && l->GetLevel() == level;
        });
    
    return it != impl_->levels.end() ? *it : nullptr;
}

LODLevelPtr LODManager::GetLODLevelInternal(int level) const {
    auto it = std::find_if(impl_->levels.begin(), impl_->levels.end(),
        [level](const LODLevelPtr& l) {
            return l && l->GetLevel() == level;
        });
    
    return it != impl_->levels.end() ? *it : nullptr;
}

std::vector<LODLevelPtr> LODManager::GetLODLevels() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->levels;
}

size_t LODManager::GetLODCount() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->levels.size();
}

int LODManager::SelectLOD(double scale) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    if (!impl_->strategy || impl_->levels.empty()) {
        return -1;
    }
    
    int lod = impl_->strategy->SelectLOD(scale, impl_->levels);
    return IsLODInRange(lod) ? lod : -1;
}

int LODManager::SelectLODByResolution(double resolution) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    if (!impl_->strategy || impl_->levels.empty()) {
        return -1;
    }
    
    int lod = impl_->strategy->SelectLODByResolution(resolution, impl_->levels);
    return IsLODInRange(lod) ? lod : -1;
}

int LODManager::SelectLODByExtent(const Envelope& extent, int pixelWidth, int pixelHeight) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    if (!impl_->strategy || impl_->levels.empty()) {
        return -1;
    }
    
    int lod = impl_->strategy->SelectLODByExtent(extent, pixelWidth, pixelHeight, impl_->levels);
    return IsLODInRange(lod) ? lod : -1;
}

std::vector<int> LODManager::GetVisibleLODs(double scale) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    if (!impl_->strategy) {
        return {};
    }
    
    auto visible = impl_->strategy->GetVisibleLODs(scale, impl_->levels);
    
    visible.erase(
        std::remove_if(visible.begin(), visible.end(),
            [this](int lod) { return !IsLODInRange(lod); }),
        visible.end());
    
    return visible;
}

void LODManager::SetMinLOD(int minLOD) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->minLOD = minLOD;
}

int LODManager::GetMinLOD() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->minLOD;
}

void LODManager::SetMaxLOD(int maxLOD) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->maxLOD = maxLOD;
}

int LODManager::GetMaxLOD() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->maxLOD;
}

void LODManager::SetLODRange(int minLOD, int maxLOD) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->minLOD = minLOD;
    impl_->maxLOD = maxLOD;
}

bool LODManager::IsLODVisible(int lod) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return IsLODInRange(lod);
}

void LODManager::SetTransitionFactor(double factor) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->transitionFactor = factor;
}

double LODManager::GetTransitionFactor() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->transitionFactor;
}

void LODManager::SetEnableTransitions(bool enable) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->enableTransitions = enable;
}

bool LODManager::IsTransitionsEnabled() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->enableTransitions;
}

void LODManager::CreateDefaultLODs(int minLevel, int maxLevel, double baseResolution) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    impl_->levels.clear();
    impl_->minLOD = minLevel;
    impl_->maxLOD = maxLevel;
    
    for (int i = minLevel; i <= maxLevel; ++i) {
        double resolution = baseResolution / std::pow(2.0, i - minLevel);
        auto level = LODLevel::Create(i, 0.0, 0.0);
        level->SetResolution(resolution);
        level->SetName("Level " + std::to_string(i));
        impl_->levels.push_back(level);
    }
}

void LODManager::CreateWebMercatorLODs(int minLevel, int maxLevel) {
    CreateDefaultLODs(minLevel, maxLevel, WEB_MERCATOR_RESOLUTION_0);
}

double LODManager::GetResolutionForLevel(int level) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    auto lodLevel = GetLODLevelInternal(level);
    if (lodLevel) {
        return lodLevel->GetResolution();
    }
    
    return WEB_MERCATOR_RESOLUTION_0 / std::pow(2.0, level);
}

int LODManager::GetLevelForResolution(double resolution) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    for (const auto& level : impl_->levels) {
        if (level) {
            double levelRes = level->GetResolution();
            if (levelRes > 0.0 && resolution >= levelRes / impl_->transitionFactor) {
                return level->GetLevel();
            }
        }
    }
    
    return impl_->levels.empty() ? 0 : impl_->levels.back()->GetLevel();
}

double LODManager::GetScaleForLevel(int level, double dpi) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
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
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    double resolution = scale / (dpi * INCHES_PER_METER);
    
    for (const auto& level : impl_->levels) {
        if (level) {
            double levelRes = level->GetResolution();
            if (levelRes > 0.0 && resolution >= levelRes / impl_->transitionFactor) {
                return level->GetLevel();
            }
        }
    }
    
    return impl_->levels.empty() ? 0 : impl_->levels.back()->GetLevel();
}

bool LODManager::IsLODInRange(int lod) const {
    return lod >= impl_->minLOD && lod <= impl_->maxLOD;
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
