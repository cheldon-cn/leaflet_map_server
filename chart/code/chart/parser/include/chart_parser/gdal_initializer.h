#ifndef GDAL_INITIALIZER_H
#define GDAL_INITIALIZER_H

#include <string>

namespace chart {
namespace parser {

class GDALInitializer {
public:
    static GDALInitializer& Instance();
    
    bool Initialize();
    void Shutdown();
    
    bool IsInitialized() const { return m_initialized; }
    
    bool RegisterDriver(const std::string& driverName);
    bool IsDriverRegistered(const std::string& driverName) const;
    
private:
    GDALInitializer();
    ~GDALInitializer();
    
    GDALInitializer(const GDALInitializer&) = delete;
    GDALInitializer& operator=(const GDALInitializer&) = delete;
    
    bool m_initialized;
    int m_registerCount;
};

} // namespace parser
} // namespace chart

#endif // GDAL_INITIALIZER_H
