#include "ogc/symbology/style/s52_symbol_renderer.h"
#include "ogc/symbology/symbolizer/point_symbolizer.h"
#include "ogc/symbology/symbolizer/line_symbolizer.h"
#include "ogc/symbology/symbolizer/polygon_symbolizer.h"
#include "ogc/symbology/symbolizer/icon_symbolizer.h"
#include "ogc/point.h"
#include "ogc/linestring.h"
#include "ogc/polygon.h"
#include <algorithm>

using ogc::draw::DrawResult;
using ogc::draw::DrawContextPtr;
using ogc::draw::DrawContext;

namespace ogc {
namespace symbology {

S52SymbolRenderer& S52SymbolRenderer::Instance()
{
    static S52SymbolRenderer instance;
    return instance;
}

bool S52SymbolRenderer::Initialize()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_initialized) {
        return true;
    }
    
    m_defaultSymbolSize = 10.0;
    m_defaultLineWidth = 1.0;
    m_initialized = true;
    return true;
}

DrawResult S52SymbolRenderer::RenderSymbol(DrawContextPtr context, 
                                           const std::string& symbolId,
                                           const Geometry* geometry,
                                           const StyleRule* rule)
{
    if (!geometry) {
        return DrawResult::kInvalidParameter;
    }
    
    const SymbolDefinition* symbolDef = SymbolLibrary::Instance().GetSymbol(symbolId);
    
    GeomType geomType = geometry->GetGeometryType();
    
    switch (geomType) {
        case GeomType::kPoint:
        case GeomType::kMultiPoint:
            return RenderPointSymbol(context, symbolId, 0, 0, rule);
        case GeomType::kLineString:
        case GeomType::kMultiLineString:
            return RenderLineSymbol(context, symbolId, 
                                   dynamic_cast<const LineString*>(geometry), rule);
        case GeomType::kPolygon:
        case GeomType::kMultiPolygon:
            return RenderPolygonSymbol(context, symbolId,
                                      dynamic_cast<const Polygon*>(geometry), rule);
        default:
            return DrawResult::kUnsupportedOperation;
    }
}

DrawResult S52SymbolRenderer::RenderPointSymbol(DrawContextPtr context,
                                                 const std::string& symbolId,
                                                 double x, double y,
                                                 const StyleRule* rule)
{
    const SymbolDefinition* symbolDef = SymbolLibrary::Instance().GetSymbol(symbolId);
    
    if (!symbolDef) {
        SymbolizerPtr symbolizer = CreatePointSymbolizer(nullptr, rule);
        if (symbolizer) {
            auto point = ogc::Point::Create(x, y);
            return symbolizer->Symbolize(context, point.get());
        }
        return DrawResult::kFileNotFound;
    }
    
    switch (symbolDef->type) {
        case SymbolType::kVector:
            return RenderVectorSymbol(context, symbolDef, x, y, rule);
        case SymbolType::kRaster:
            return RenderRasterSymbol(context, symbolDef, x, y, rule);
        case SymbolType::kComposite:
            return RenderCompositeSymbol(context, symbolDef, x, y, rule);
        default:
            return DrawResult::kUnsupportedOperation;
    }
}

DrawResult S52SymbolRenderer::RenderLineSymbol(DrawContextPtr context,
                                                const std::string& symbolId,
                                                const LineString* lineString,
                                                const StyleRule* rule)
{
    if (!lineString) {
        return DrawResult::kInvalidParameter;
    }
    
    const SymbolDefinition* symbolDef = SymbolLibrary::Instance().GetSymbol(symbolId);
    
    SymbolizerPtr symbolizer = CreateLineSymbolizer(symbolDef, rule);
    if (symbolizer) {
        return symbolizer->Symbolize(context, lineString);
    }
    
    return DrawResult::kFailed;
}

DrawResult S52SymbolRenderer::RenderPolygonSymbol(DrawContextPtr context,
                                                   const std::string& symbolId,
                                                   const Polygon* polygon,
                                                   const StyleRule* rule)
{
    if (!polygon) {
        return DrawResult::kInvalidParameter;
    }
    
    const SymbolDefinition* symbolDef = SymbolLibrary::Instance().GetSymbol(symbolId);
    
    SymbolizerPtr symbolizer = CreatePolygonSymbolizer(symbolDef, rule);
    if (symbolizer) {
        return symbolizer->Symbolize(context, polygon);
    }
    
    return DrawResult::kFailed;
}

SymbolizerPtr S52SymbolRenderer::CreateSymbolizerFromDefinition(const SymbolDefinition* symbolDef,
                                                                  const StyleRule* rule)
{
    if (!symbolDef) {
        return nullptr;
    }
    
    switch (symbolDef->type) {
        case SymbolType::kVector:
        case SymbolType::kRaster:
            return CreatePointSymbolizer(symbolDef, rule);
        case SymbolType::kComposite:
            return CreatePointSymbolizer(symbolDef, rule);
        default:
            return nullptr;
    }
}

SymbolizerPtr S52SymbolRenderer::CreatePointSymbolizer(const SymbolDefinition* symbolDef,
                                                        const StyleRule* rule)
{
    PointSymbolizerPtr symbolizer = PointSymbolizer::Create();
    
    double size = m_defaultSymbolSize;
    uint32_t color = 0x000000;
    
    if (symbolDef) {
        size = std::max(symbolDef->width, symbolDef->height);
        if (size <= 0) {
            size = m_defaultSymbolSize;
        }
    }
    
    if (rule) {
        color = rule->lineColor.GetRGBA();
    }
    
    symbolizer->SetSize(size);
    symbolizer->SetColor(color);
    symbolizer->SetSymbolType(PointSymbolType::kCircle);
    
    return symbolizer;
}

SymbolizerPtr S52SymbolRenderer::CreateLineSymbolizer(const SymbolDefinition* symbolDef,
                                                       const StyleRule* rule)
{
    LineSymbolizerPtr symbolizer = LineSymbolizer::Create();
    
    double width = m_defaultLineWidth;
    uint32_t color = 0x000000;
    
    if (rule) {
        width = rule->lineWidth;
        color = rule->lineColor.GetRGBA();
    }
    
    symbolizer->SetWidth(width);
    symbolizer->SetColor(color);
    
    return symbolizer;
}

SymbolizerPtr S52SymbolRenderer::CreatePolygonSymbolizer(const SymbolDefinition* symbolDef,
                                                          const StyleRule* rule)
{
    PolygonSymbolizerPtr symbolizer = PolygonSymbolizer::Create();
    
    uint32_t fillColor = 0xFFFFFF;
    uint32_t strokeColor = 0x000000;
    double strokeWidth = m_defaultLineWidth;
    
    if (rule) {
        fillColor = rule->fillColor.GetRGBA();
        strokeColor = rule->lineColor.GetRGBA();
        strokeWidth = rule->lineWidth;
    }
    
    symbolizer->SetFillColor(fillColor);
    symbolizer->SetStrokeColor(strokeColor);
    symbolizer->SetStrokeWidth(strokeWidth);
    
    return symbolizer;
}

void S52SymbolRenderer::SetDefaultSymbolSize(double size)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_defaultSymbolSize = size;
}

double S52SymbolRenderer::GetDefaultSymbolSize() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_defaultSymbolSize;
}

void S52SymbolRenderer::SetDefaultLineWidth(double width)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_defaultLineWidth = width;
}

double S52SymbolRenderer::GetDefaultLineWidth() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_defaultLineWidth;
}

void S52SymbolRenderer::CacheSymbolizer(const std::string& key, SymbolizerPtr symbolizer)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_symbolizerCache[key] = symbolizer;
}

SymbolizerPtr S52SymbolRenderer::GetCachedSymbolizer(const std::string& key) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_symbolizerCache.find(key);
    if (it != m_symbolizerCache.end()) {
        return it->second;
    }
    return nullptr;
}

void S52SymbolRenderer::ClearCache()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_symbolizerCache.clear();
}

DrawResult S52SymbolRenderer::RenderVectorSymbol(DrawContextPtr context,
                                                  const SymbolDefinition* symbolDef,
                                                  double x, double y,
                                                  const StyleRule* rule)
{
    if (!symbolDef) {
        return DrawResult::kInvalidParameter;
    }
    
    SymbolizerPtr symbolizer = CreatePointSymbolizer(symbolDef, rule);
    if (symbolizer) {
        auto point = ogc::Point::Create(x, y);
        return symbolizer->Symbolize(context, point.get());
    }
    
    return DrawResult::kFailed;
}

DrawResult S52SymbolRenderer::RenderRasterSymbol(DrawContextPtr context,
                                                  const SymbolDefinition* symbolDef,
                                                  double x, double y,
                                                  const StyleRule* rule)
{
    if (!symbolDef) {
        return DrawResult::kInvalidParameter;
    }
    
    if (!symbolDef->rasterImagePath.empty()) {
        IconSymbolizerPtr symbolizer = IconSymbolizer::Create(symbolDef->rasterImagePath);
        if (symbolizer) {
            symbolizer->SetSize(symbolDef->width, symbolDef->height);
            auto point = ogc::Point::Create(x, y);
            return symbolizer->Symbolize(context, point.get());
        }
    }
    
    return DrawResult::kFailed;
}

DrawResult S52SymbolRenderer::RenderCompositeSymbol(DrawContextPtr context,
                                                     const SymbolDefinition* symbolDef,
                                                     double x, double y,
                                                     const StyleRule* rule)
{
    if (!symbolDef) {
        return DrawResult::kInvalidParameter;
    }
    
    for (const auto& componentId : symbolDef->componentSymbolIds) {
        const SymbolDefinition* componentDef = SymbolLibrary::Instance().GetSymbol(componentId);
        if (componentDef) {
            RenderVectorSymbol(context, componentDef, x, y, rule);
        }
    }
    
    return DrawResult::kSuccess;
}

void S52SymbolRenderer::ExecuteVectorCommand(DrawContextPtr context,
                                              const VectorCommand& cmd,
                                              double x, double y,
                                              double scale)
{
}

std::string S52SymbolRenderer::GenerateCacheKey(const std::string& symbolId, 
                                                  const StyleRule* rule) const
{
    std::string key = symbolId;
    if (rule) {
        key += "_" + rule->ruleId;
    }
    return key;
}

S52SymbolRenderer::S52SymbolRenderer()
    : m_initialized(false)
    , m_defaultSymbolSize(10.0)
    , m_defaultLineWidth(1.0)
{
}

S52SymbolRenderer::~S52SymbolRenderer()
{
}

}
}
