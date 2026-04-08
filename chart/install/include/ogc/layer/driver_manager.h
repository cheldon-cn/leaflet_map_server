#pragma once

#include "ogc/layer/export.h"
#include "ogc/layer/driver.h"

#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace ogc {

class OGC_LAYER_API CNDriverManager {
public:
    static CNDriverManager& Instance() {
        static CNDriverManager instance;
        return instance;
    }

    void RegisterDriver(std::unique_ptr<CNDriver> driver) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::string name = driver->GetName();
        drivers_[name] = std::move(driver);
    }

    void DeregisterDriver(const std::string& name) {
        std::lock_guard<std::mutex> lock(mutex_);
        drivers_.erase(name);
    }

    void RegisterBuiltinDrivers();

    CNDriver* GetDriver(const std::string& name) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = drivers_.find(name);
        return it != drivers_.end() ? it->second.get() : nullptr;
    }

    CNDriver* FindDriver(const std::string& path) const {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& pair : drivers_) {
            if (pair.second->CanOpen(path)) {
                return pair.second.get();
            }
        }
        return nullptr;
    }

    std::vector<CNDriver*> GetDrivers() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<CNDriver*> result;
        for (const auto& pair : drivers_) {
            result.push_back(pair.second.get());
        }
        return result;
    }

    std::unique_ptr<CNDataSource> Open(
        const std::string& path,
        bool update = false) const;

private:
    CNDriverManager() {
        RegisterBuiltinDrivers();
    }

    CNDriverManager(const CNDriverManager&) = delete;
    CNDriverManager& operator=(const CNDriverManager&) = delete;

    mutable std::mutex mutex_;
    std::map<std::string, std::unique_ptr<CNDriver>> drivers_;
};

} // namespace ogc
