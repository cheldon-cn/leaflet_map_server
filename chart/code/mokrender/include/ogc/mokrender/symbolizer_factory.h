#pragma once

#include "ogc/mokrender/interfaces.h"
#include "ogc/mokrender/common.h"
#include <memory>
#include <string>

namespace ogc {
namespace mokrender {

class OGC_MOKRENDER_API SymbolizerFactory {
public:
    SymbolizerFactory();
    ~SymbolizerFactory();
    
    MokRenderResult Initialize();
    void* CreatePointSymbolizer();
    void* CreateLineSymbolizer();
    void* CreatePolygonSymbolizer();
    void* CreateTextSymbolizer();
    void* CreateRasterSymbolizer();
    
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}
}
