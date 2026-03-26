#pragma once

#include "export.h"
#include "ogc/feature/feature.h"
#include "ogc/feature/feature_collection.h"
#include <memory>

namespace ogc {

class OGC_FEATURE_API CNFeatureIterator {
public:
    explicit CNFeatureIterator(const CNFeatureCollection* collection);
    ~CNFeatureIterator();

    CNFeatureIterator(const CNFeatureIterator& other);
    CNFeatureIterator& operator=(const CNFeatureIterator& other);

    void Reset();

    bool HasNext() const;
    CNFeature* Next();

    CNFeature* operator*() const;
    CNFeature* operator->() const;

    CNFeatureIterator& operator++();
    CNFeatureIterator operator++(int);

    size_t GetPosition() const { return position_; }
    size_t GetTotalCount() const { return total_count_; }

private:
    const CNFeatureCollection* collection_;
    size_t position_;
    size_t total_count_;
};

using FeatureIterator = CNFeatureIterator;

}
