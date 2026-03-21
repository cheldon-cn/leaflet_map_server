#include "ogc/layer/layer_observer.h"

namespace ogc {

void CNObservableLayer::AddObserver(CNLayerObserver* observer) {
    if (!observer) {
        return;
    }
    std::lock_guard<std::mutex> lock(observers_mutex_);
    observers_.insert(observer);
}

void CNObservableLayer::RemoveObserver(CNLayerObserver* observer) {
    if (!observer) {
        return;
    }
    std::lock_guard<std::mutex> lock(observers_mutex_);
    observers_.erase(observer);
}

void CNObservableLayer::ClearObservers() {
    std::lock_guard<std::mutex> lock(observers_mutex_);
    observers_.clear();
}

void CNObservableLayer::NotifyObservers(const CNLayerEventArgs& args) {
    std::set<CNLayerObserver*> observers_copy;
    {
        std::lock_guard<std::mutex> lock(observers_mutex_);
        observers_copy = observers_;
    }

    for (auto* observer : observers_copy) {
        if (observer) {
            observer->OnLayerChanged(this, args);
        }
    }
}

bool CNObservableLayer::NotifyChanging(const CNLayerEventArgs& args) {
    std::set<CNLayerObserver*> observers_copy;
    {
        std::lock_guard<std::mutex> lock(observers_mutex_);
        observers_copy = observers_;
    }

    for (auto* observer : observers_copy) {
        if (observer && !observer->OnLayerChanging(this, args)) {
            return false;
        }
    }
    return true;
}

} // namespace ogc
