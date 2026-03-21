#include "ogc/layer/layer_snapshot.h"

#include "ogc/feature/feature.h"

namespace ogc {

CNLayerSnapshot::CNLayerSnapshot(CNLayer* layer) {
    if (!layer) {
        return;
    }

    layer->ResetReading();
    while (auto feature = layer->GetNextFeature()) {
        features_.push_back(std::move(feature));
    }
}

CNLayerSnapshot::Iterator& CNLayerSnapshot::Iterator::operator++() {
    ++index_;
    return *this;
}

CNLayerSnapshot::Iterator CNLayerSnapshot::Iterator::operator++(int) {
    Iterator temp = *this;
    ++index_;
    return temp;
}

bool CNLayerSnapshot::Iterator::operator==(const Iterator& other) const {
    return index_ == other.index_ && snapshot_ == other.snapshot_;
}

bool CNLayerSnapshot::Iterator::operator!=(const Iterator& other) const {
    return !(*this == other);
}

CNFeature& CNLayerSnapshot::Iterator::operator*() {
    return *snapshot_->features_[index_];
}

CNFeature* CNLayerSnapshot::Iterator::operator->() {
    return snapshot_->features_[index_].get();
}

CNLayerSnapshot::Iterator CNLayerSnapshot::begin() {
    Iterator it;
    it.index_ = 0;
    it.snapshot_ = this;
    return it;
}

CNLayerSnapshot::Iterator CNLayerSnapshot::end() {
    Iterator it;
    it.index_ = features_.size();
    it.snapshot_ = this;
    return it;
}

} // namespace ogc
