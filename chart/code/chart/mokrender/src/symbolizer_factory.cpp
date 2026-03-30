#include "ogc/mokrender/symbolizer_factory.h"
#include "ogc/draw/point_symbolizer.h"
#include "ogc/draw/line_symbolizer.h"
#include "ogc/draw/polygon_symbolizer.h"
#include "ogc/draw/text_symbolizer.h"
#include "ogc/draw/raster_symbolizer.h"

namespace ogc {
namespace mokrender {

class SymbolizerFactory::Impl {
public:
    bool initialized;
    draw::PointSymbolizerPtr pointSymbolizer;
    draw::LineSymbolizerPtr lineSymbolizer;
    draw::PolygonSymbolizerPtr polygonSymbolizer;
    draw::TextSymbolizerPtr textSymbolizer;
    draw::RasterSymbolizerPtr rasterSymbolizer;
    
    Impl() : initialized(false) {}
};

SymbolizerFactory::SymbolizerFactory() : m_impl(new Impl()) {
}

SymbolizerFactory::~SymbolizerFactory() {
}

MokRenderResult SymbolizerFactory::Initialize() {
    m_impl->initialized = true;
    return MokRenderResult();
}

void* SymbolizerFactory::CreatePointSymbolizer() {
    if (!m_impl->initialized) {
        return nullptr;
    }
    
    m_impl->pointSymbolizer = draw::PointSymbolizer::Create(5.0, 0xFF0000FF);
    return m_impl->pointSymbolizer.get();
}

void* SymbolizerFactory::CreateLineSymbolizer() {
    if (!m_impl->initialized) {
        return nullptr;
    }
    
    m_impl->lineSymbolizer = draw::LineSymbolizer::Create(2.0, 0xFFFF0000);
    return m_impl->lineSymbolizer.get();
}

void* SymbolizerFactory::CreatePolygonSymbolizer() {
    if (!m_impl->initialized) {
        return nullptr;
    }
    
    m_impl->polygonSymbolizer = draw::PolygonSymbolizer::Create(0x8000FF00, 0xFF008000, 1.0);
    return m_impl->polygonSymbolizer.get();
}

void* SymbolizerFactory::CreateTextSymbolizer() {
    if (!m_impl->initialized) {
        return nullptr;
    }
    
    m_impl->textSymbolizer = draw::TextSymbolizer::Create();
    return m_impl->textSymbolizer.get();
}

void* SymbolizerFactory::CreateRasterSymbolizer() {
    if (!m_impl->initialized) {
        return nullptr;
    }
    
    m_impl->rasterSymbolizer = draw::RasterSymbolizer::Create();
    return m_impl->rasterSymbolizer.get();
}

}
}
