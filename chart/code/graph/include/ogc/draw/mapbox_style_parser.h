#ifndef OGC_DRAW_MAPBOX_STYLE_PARSER_H
#define OGC_DRAW_MAPBOX_STYLE_PARSER_H

#include "ogc/draw/export.h"
#include "ogc/draw/symbolizer_rule.h"
#include "ogc/draw/symbolizer.h"
#include "ogc/draw/point_symbolizer.h"
#include "ogc/draw/line_symbolizer.h"
#include "ogc/draw/polygon_symbolizer.h"
#include "ogc/draw/text_symbolizer.h"
#include "ogc/draw/raster_symbolizer.h"
#include "ogc/draw/filter.h"
#include "ogc/draw/comparison_filter.h"
#include "ogc/draw/logical_filter.h"
#include "ogc/draw/spatial_filter.h"
#include <ogc/draw/color.h>
#include <memory>
#include <string>
#include <vector>
#include <map>

namespace ogc {
namespace draw {

class MapboxStyleParser;
typedef std::shared_ptr<MapboxStyleParser> MapboxStyleParserPtr;

struct MapboxStyleResult {
    bool success;
    std::string errorMessage;
    int errorLine;
    int errorColumn;
    std::vector<SymbolizerRulePtr> rules;
    
    MapboxStyleResult() : success(false), errorLine(0), errorColumn(0) {}
};

struct MapboxSource {
    std::string type;
    std::string url;
    std::map<std::string, std::string> tiles;
    int minZoom;
    int maxZoom;
    
    MapboxSource() : minZoom(0), maxZoom(22) {}
};

struct MapboxLayer {
    std::string id;
    std::string type;
    std::string source;
    std::string sourceLayer;
    double minZoom;
    double maxZoom;
    bool visible;
    double opacity;
    std::map<std::string, std::string> paint;
    std::map<std::string, std::string> layout;
    std::string filter;
    
    MapboxLayer() : minZoom(0), maxZoom(22), visible(true), opacity(1.0) {}
};

struct MapboxStyle {
    std::string version;
    std::string name;
    std::map<std::string, MapboxSource> sources;
    std::vector<MapboxLayer> layers;
    std::map<std::string, std::string> sprites;
    std::map<std::string, std::string> glyphs;
    
    MapboxStyle() = default;
};

class OGC_GRAPH_API MapboxStyleParser {
public:
    MapboxStyleParser();
    ~MapboxStyleParser() = default;
    
    MapboxStyleResult Parse(const std::string& jsonContent);
    MapboxStyleResult ParseFile(const std::string& filePath);
    
    MapboxStyle GetStyle() const { return m_style; }
    std::vector<SymbolizerRulePtr> GetRules() const;
    
    void SetStrictMode(bool strict) { m_strictMode = strict; }
    bool IsStrictMode() const { return m_strictMode; }
    
    bool HasError() const { return !m_lastError.empty(); }
    std::string GetLastError() const { return m_lastError; }
    
    static MapboxStyleParserPtr Create();
    
    static std::vector<SymbolizerRulePtr> ParseStyle(const std::string& jsonContent);
    static std::vector<SymbolizerRulePtr> ParseStyleFile(const std::string& filePath);
    
    static std::string GenerateStyle(const std::vector<SymbolizerRulePtr>& rules);
    static std::string GenerateStyle(const SymbolizerRulePtr& rule);
    
private:
    MapboxStyle m_style;
    std::string m_lastError;
    bool m_strictMode;
    size_t m_currentPos;
    std::string m_content;
    
    bool ParseStyleObject();
    bool ParseSources();
    bool ParseLayers();
    bool ParseLayer(MapboxLayer& layer);
    bool ParsePaint(std::map<std::string, std::string>& paint);
    bool ParseLayout(std::map<std::string, std::string>& layout);
    std::string ParseFilter();
    
    SymbolizerRulePtr ConvertLayerToRule(const MapboxLayer& layer) const;
    SymbolizerPtr CreateSymbolizer(const MapboxLayer& layer) const;
    PointSymbolizerPtr CreatePointSymbolizer(const MapboxLayer& layer) const;
    LineSymbolizerPtr CreateLineSymbolizer(const MapboxLayer& layer) const;
    PolygonSymbolizerPtr CreatePolygonSymbolizer(const MapboxLayer& layer) const;
    TextSymbolizerPtr CreateTextSymbolizer(const MapboxLayer& layer) const;
    RasterSymbolizerPtr CreateRasterSymbolizer(const MapboxLayer& layer) const;
    FilterPtr ConvertFilter(const std::string& filterExpr) const;
    
    std::string ReadToken();
    std::string ReadString();
    double ReadNumber();
    bool ReadBool();
    void SkipWhitespace();
    std::string PeekToken() const;
    
    std::string Trim(const std::string& str) const;
    std::string ToLower(const std::string& str) const;
    
    uint32_t ParseColor(const std::string& colorStr) const;
    double ParseDouble(const std::string& str, double defaultVal = 0.0) const;
    int ParseInt(const std::string& str, int defaultVal = 0) const;
    
    static std::string ColorToString(uint32_t color);
    static std::string DoubleToString(double val);
    
    void SetError(const std::string& message);
    void ClearError();
    
    static std::string GenerateIndent(int level);
    static std::string EscapeJson(const std::string& str);
};

}
}

#endif
