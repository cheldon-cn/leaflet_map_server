#pragma once

#include "ogc/mokrender/interfaces.h"
#include "ogc/mokrender/common.h"
#include <memory>
#include <string>
#include <vector>

namespace ogc {
namespace mokrender {

class OGC_MOKRENDER_API DatabaseManager {
public:
    DatabaseManager();
    ~DatabaseManager();
    
    MokRenderResult Initialize(const std::string& dbPath);
    MokRenderResult CreateTable(const std::string& tableName, 
                                const std::string& geometryType);
    MokRenderResult InsertFeature(const std::string& tableName, void* feature);
    MokRenderResult InsertFeatures(const std::string& tableName, 
                                   const std::vector<void*>& features);
    void Close();
    
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}
}
