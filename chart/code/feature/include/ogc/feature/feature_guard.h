#pragma once

#include "export.h"
#include "ogc/feature/feature.h"
#include <memory>

namespace ogc {

class CNFeatureGuard {
public:
    explicit CNFeatureGuard(CNFeature* feature);
    ~CNFeatureGuard();

    CNFeatureGuard(const CNFeatureGuard& other) = delete;
    CNFeatureGuard& operator=(const CNFeatureGuard& other) = delete;

    CNFeatureGuard(CNFeatureGuard&& other) noexcept;
    CNFeatureGuard& operator=(CNFeatureGuard&& other) noexcept;

    CNFeature* Get() const { return feature_; }
    CNFeature* operator->() const { return feature_; }
    CNFeature& operator*() const { return *feature_; }

    CNFeature* Release();
    void Reset(CNFeature* feature = nullptr);

private:
    CNFeature* feature_;
};

using FeatureGuard = CNFeatureGuard;

}
