#include "chart_parser/gdal_initializer.h"
#include "chart_parser/error_handler.h"

#include <ogrsf_frmts.h>
#include <gdal_priv.h>

namespace chart {
namespace parser {

GDALInitializer& GDALInitializer::Instance() {
    static GDALInitializer instance;
    return instance;
}

GDALInitializer::GDALInitializer()
    : m_initialized(false)
    , m_registerCount(0) {
}

GDALInitializer::~GDALInitializer() {
    if (m_initialized) {
        Shutdown();
    }
}

bool GDALInitializer::Initialize() {
    if (m_initialized) {
        return true;
    }
    
    LOG_INFO("Initializing GDAL...");
    
    GDALAllRegister();
    
    if (GDALGetDriverCount() == 0) {
        LOG_ERROR("GDAL driver registration failed");
        return false;
    }
    
    m_initialized = true;
    m_registerCount = GDALGetDriverCount();
    
    LOG_INFO("GDAL initialized successfully. {} drivers registered", m_registerCount);
    
    return true;
}

void GDALInitializer::Shutdown() {
    if (!m_initialized) {
        return;
    }
    
    LOG_INFO("Shutting down GDAL...");
    
    GDALDestroyDriverManager();
    
    m_initialized = false;
    m_registerCount = 0;
    
    LOG_INFO("GDAL shut down successfully");
}

bool GDALInitializer::RegisterDriver(const std::string& driverName) {
    if (!m_initialized) {
        if (!Initialize()) {
            return false;
        }
    }
    
    GDALDriver* driver = GetGDALDriverManager()->GetDriverByName(driverName.c_str());
    if (driver) {
        LOG_DEBUG("Driver '{}' already registered", driverName);
        return true;
    }
    
    LOG_WARN("Driver '{}' not found", driverName);
    return false;
}

bool GDALInitializer::IsDriverRegistered(const std::string& driverName) const {
    if (!m_initialized) {
        return false;
    }
    
    GDALDriver* driver = GetGDALDriverManager()->GetDriverByName(driverName.c_str());
    return driver != nullptr;
}

} // namespace parser
} // namespace chart
