#pragma once

#include "ogc/layer/export.h"
#include "ogc/layer/layer.h"

#include <memory>
#include <vector>

namespace ogc {

class OGC_LAYER_API CNLayerSnapshot {
public:
    explicit CNLayerSnapshot(CNLayer* layer);
    
    CNLayerSnapshot(const CNLayerSnapshot&) = delete;
    CNLayerSnapshot& operator=(const CNLayerSnapshot&) = delete;
    CNLayerSnapshot(CNLayerSnapshot&&) = default;
    CNLayerSnapshot& operator=(CNLayerSnapshot&&) = default;

    class OGC_LAYER_API Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = CNFeature;
        using difference_type = std::ptrdiff_t;
        using pointer = CNFeature*;
        using reference = CNFeature&;

        Iterator& operator++();
        Iterator operator++(int);
        bool operator==(const Iterator& other) const;
        bool operator!=(const Iterator& other) const;
        CNFeature& operator*();
        CNFeature* operator->();

    private:
        friend class CNLayerSnapshot;
        size_t index_;
        CNLayerSnapshot* snapshot_;
    };

    Iterator begin();
    Iterator end();

    size_t size() const {
        return features_.size();
    }

private:
    std::vector<std::unique_ptr<CNFeature>> features_;
};

} // namespace ogc
