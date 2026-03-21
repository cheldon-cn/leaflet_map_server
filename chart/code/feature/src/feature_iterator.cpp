#include "ogc/feature/feature_iterator.h"

namespace ogc {

CNFeatureIterator::CNFeatureIterator(const CNFeatureCollection* collection)
    : collection_(collection), position_(0), total_count_(0) {
    if (collection_) {
        total_count_ = collection_->GetFeatureCount();
    }
}

CNFeatureIterator::~CNFeatureIterator() {
}

CNFeatureIterator::CNFeatureIterator(const CNFeatureIterator& other)
    : collection_(other.collection_), position_(other.position_), total_count_(other.total_count_) {
}

CNFeatureIterator& CNFeatureIterator::operator=(const CNFeatureIterator& other) {
    if (this != &other) {
        collection_ = other.collection_;
        position_ = other.position_;
        total_count_ = other.total_count_;
    }
    return *this;
}

void CNFeatureIterator::Reset() {
    position_ = 0;
}

bool CNFeatureIterator::HasNext() const {
    return collection_ && position_ < total_count_;
}

CNFeature* CNFeatureIterator::Next() {
    if (!HasNext()) {
        return nullptr;
    }
    CNFeature* feature = collection_->GetFeature(position_);
    position_++;
    return feature;
}

CNFeature* CNFeatureIterator::operator*() const {
    if (!collection_ || position_ >= total_count_) {
        return nullptr;
    }
    return collection_->GetFeature(position_);
}

CNFeature* CNFeatureIterator::operator->() const {
    return operator*();
}

CNFeatureIterator& CNFeatureIterator::operator++() {
    if (position_ < total_count_) {
        position_++;
    }
    return *this;
}

CNFeatureIterator CNFeatureIterator::operator++(int) {
    CNFeatureIterator temp(*this);
    if (position_ < total_count_) {
        position_++;
    }
    return temp;
}

}
