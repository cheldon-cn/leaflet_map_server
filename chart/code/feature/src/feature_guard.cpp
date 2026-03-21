#include "ogc/feature/feature_guard.h"

namespace ogc {

CNFeatureGuard::CNFeatureGuard(CNFeature* feature)
    : feature_(feature) {
}

CNFeatureGuard::~CNFeatureGuard() {
    if (feature_) {
        delete feature_;
        feature_ = nullptr;
    }
}

CNFeatureGuard::CNFeatureGuard(CNFeatureGuard&& other) noexcept
    : feature_(other.feature_) {
    other.feature_ = nullptr;
}

CNFeatureGuard& CNFeatureGuard::operator=(CNFeatureGuard&& other) noexcept {
    if (this != &other) {
        if (feature_) {
            delete feature_;
        }
        feature_ = other.feature_;
        other.feature_ = nullptr;
    }
    return *this;
}

CNFeature* CNFeatureGuard::Release() {
    CNFeature* temp = feature_;
    feature_ = nullptr;
    return temp;
}

void CNFeatureGuard::Reset(CNFeature* feature) {
    if (feature_ && feature_ != feature) {
        delete feature_;
    }
    feature_ = feature;
}

}
