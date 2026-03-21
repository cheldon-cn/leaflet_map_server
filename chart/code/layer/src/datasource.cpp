#include "ogc/layer/datasource.h"
#include "ogc/layer/driver_manager.h"

namespace ogc {

std::unique_ptr<CNDataSource> CNDataSource::Open(
    const std::string& path,
    bool update) {
    return CNDriverManager::Instance().Open(path, update);
}

std::unique_ptr<CNDataSource> CNDataSource::Create(
    const std::string& path,
    const std::string& driver_name) {
    auto driver = CNDriverManager::Instance().GetDriver(driver_name);
    if (!driver) {
        return nullptr;
    }
    return driver->Create(path);
}

} // namespace ogc
