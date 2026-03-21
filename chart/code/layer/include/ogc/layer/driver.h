#pragma once

#include "ogc/layer/export.h"
#include "ogc/layer/datasource.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace ogc {

class OGC_LAYER_API CNDriver {
public:
    virtual ~CNDriver() = default;

    virtual const char* GetName() const = 0;

    virtual const char* GetDescription() const {
        return GetName();
    }

    virtual std::vector<std::string> GetExtensions() const {
        return {};
    }

    virtual std::vector<std::string> GetMimeTypes() const {
        return {};
    }

    virtual bool CanOpen(const std::string& path) const = 0;

    virtual bool CanCreate(const std::string& path) const {
        return false;
    }

    virtual bool SupportsUpdate() const {
        return false;
    }

    virtual bool SupportsMultipleLayers() const {
        return false;
    }

    virtual bool SupportsTransactions() const {
        return false;
    }

    virtual std::unique_ptr<CNDataSource> Open(
        const std::string& path,
        bool update = false) = 0;

    virtual std::unique_ptr<CNDataSource> Create(
        const std::string& path,
        const std::map<std::string, std::string>& options = {}) {
        (void)path;
        (void)options;
        return nullptr;
    }

    virtual bool Delete(const std::string& path) {
        (void)path;
        return false;
    }
};

using CNDriverPtr = std::unique_ptr<CNDriver>;
using CNDriverRef = std::shared_ptr<CNDriver>;

} // namespace ogc
