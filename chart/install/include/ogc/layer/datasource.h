#pragma once

#include "ogc/layer/export.h"
#include "ogc/layer/layer.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace ogc {

class CNDriver;

class OGC_LAYER_API CNDataSource {
public:
    virtual ~CNDataSource() = default;

    virtual const std::string& GetName() const = 0;

    virtual std::string GetPath() const = 0;

    virtual bool IsReadOnly() const = 0;

    virtual const char* GetDriverName() const = 0;

    virtual int GetLayerCount() const = 0;

    virtual CNLayer* GetLayer(int index) = 0;

    virtual CNLayer* GetLayerByName(const std::string& name) = 0;

    virtual CNLayer* CreateLayer(
        const std::string& name,
        GeomType geom_type,
        void* srs = nullptr) = 0;

    virtual CNStatus DeleteLayer(const std::string& name) {
        return CNStatus::kNotSupported;
    }

    virtual CNStatus StartTransaction() {
        return CNStatus::kNotSupported;
    }

    virtual CNStatus CommitTransaction() {
        return CNStatus::kNotSupported;
    }

    virtual CNStatus RollbackTransaction() {
        return CNStatus::kNotSupported;
    }

    static std::unique_ptr<CNDataSource> Open(
        const std::string& path,
        bool update = false);

    static std::unique_ptr<CNDataSource> Create(
        const std::string& path,
        const std::string& driver);

protected:
    CNDataSource() = default;
};

using CNDataSourcePtr = std::unique_ptr<CNDataSource>;
using CNDataSourceRef = std::shared_ptr<CNDataSource>;

} // namespace ogc
