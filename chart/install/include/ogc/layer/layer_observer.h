#pragma once

#include "ogc/layer/export.h"
#include "ogc/layer/layer.h"

#include <memory>
#include <mutex>
#include <set>
#include <string>

namespace ogc {

enum class CNLayerEventType {
    kFeatureAdded,
    kFeatureDeleted,
    kFeatureModified,
    kSchemaChanged,
    kSpatialFilterChanged,
    kAttributeFilterChanged,
    kCleared,
    kTransactionStarted,
    kTransactionCommitted,
    kTransactionRolledBack
};

struct CNLayerEventArgs {
    CNLayerEventType type;
    int64_t fid = -1;
    int field_index = -1;
    std::string message;
    const CNFeature* feature = nullptr;
};

class CNLayerObserver {
public:
    virtual ~CNLayerObserver() = default;

    virtual void OnLayerChanged(CNLayer* layer,
                                 const CNLayerEventArgs& args) = 0;

    virtual bool OnLayerChanging(CNLayer* layer,
                                  const CNLayerEventArgs& args) {
        (void)layer;
        (void)args;
        return true;
    }
};

class OGC_LAYER_API CNObservableLayer : public CNLayer {
public:
    void AddObserver(CNLayerObserver* observer);
    void RemoveObserver(CNLayerObserver* observer);
    void ClearObservers();

protected:
    void NotifyObservers(const CNLayerEventArgs& args);
    bool NotifyChanging(const CNLayerEventArgs& args);

private:
    mutable std::mutex observers_mutex_;
    std::set<CNLayerObserver*> observers_;
};

} // namespace ogc
