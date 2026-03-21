#include "ogc/layer/driver_manager.h"

namespace ogc {

void CNDriverManager::RegisterBuiltinDrivers() {
}

std::unique_ptr<CNDataSource> CNDriverManager::Open(
    const std::string& path,
    bool update) const {
    CNDriver* driver = FindDriver(path);
    if (!driver) {
        return nullptr;
    }
    return driver->Open(path, update);
}

} // namespace ogc
