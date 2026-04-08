#ifndef OGC_SYMBOLOGY_SLD_PARSER_H
#define OGC_SYMBOLOGY_SLD_PARSER_H

#include "ogc/symbology/filter/symbolizer_rule.h"
#include "ogc/symbology/symbolizer/symbolizer.h"
#include "ogc/symbology/symbolizer/point_symbolizer.h"
#include "ogc/symbology/symbolizer/line_symbolizer.h"
#include "ogc/symbology/symbolizer/polygon_symbolizer.h"
#include "ogc/symbology/symbolizer/text_symbolizer.h"
#include "ogc/symbology/symbolizer/raster_symbolizer.h"
#include "ogc/symbology/filter/filter.h"
#include "ogc/symbology/filter/comparison_filter.h"
#include "ogc/symbology/filter/logical_filter.h"
#include "ogc/symbology/filter/spatial_filter.h"
#include <ogc/draw/color.h>
#include "ogc/symbology/export.h"
#include <memory>
#include <string>
#include <vector>
#include <map>

namespace ogc {
namespace symbology {

class SldParser;
typedef std::shared_ptr<SldParser> SldParserPtr;

struct SldParseResult {
    bool success;
    std::string errorMessage;
    int errorLine;
    int errorColumn;
    std::vector<SymbolizerRulePtr> rules;
    
    SldParseResult() : success(false), errorLine(0), errorColumn(0) {}
};

struct SldFeatureTypeStyle {
    std::string name;
    std::string title;
    std::string abstract;
    std::vector<SymbolizerRulePtr> rules;
    
    SldFeatureTypeStyle() = default;
};

struct SldNamedLayer {
    std::string name;
    std::vector<SldFeatureTypeStyle> featureTypeStyles;
    
    SldNamedLayer() = default;
};

struct SldStyledLayerDescriptor {
    std::string name;
    std::string title;
    std::string abstract;
    std::string version;
    std::vector<SldNamedLayer> namedLayers;
    
    SldStyledLayerDescriptor() = default;
};

class OGC_SYMBOLOGY_API SldParser {
public:
    SldParser();
    ~SldParser() = default;
    
    SldParseResult Parse(const std::string& sldContent);
    SldParseResult ParseFile(const std::string& filePath);
    
    SldStyledLayerDescriptor GetSld() const { return m_sld; }
    std::vector<SymbolizerRulePtr> GetRules() const;
    
    void SetStrictMode(bool strict) { m_strictMode = strict; }
    bool IsStrictMode() const { return m_strictMode; }
    
    void SetDefaultVersion(const std::string& version) { m_defaultVersion = version; }
    std::string GetDefaultVersion() const { return m_defaultVersion; }
    
    bool HasError() const { return !m_lastError.empty(); }
    std::string GetLastError() const { return m_lastError; }
    
    static SldParserPtr Create();
    
    static std::vector<SymbolizerRulePtr> ParseSld(const std::string& sldContent);
    static std::vector<SymbolizerRulePtr> ParseSldFile(const std::string& filePath);
    
    static std::string GenerateSld(const std::vector<SymbolizerRulePtr>& rules);
    static std::string GenerateSld(const SymbolizerRulePtr& rule);
    
private:
    SldStyledLayerDescriptor m_sld;
    std::string m_lastError;
    bool m_strictMode;
    std::string m_defaultVersion;
    size_t m_currentPos;
    std::string m_content;
    
    bool ParseSldElement();
    bool ParseNamedLayer();
    bool ParseUserLayer();
    bool ParseFeatureTypeStyle(SldFeatureTypeStyle& style);
    bool ParseRule(SymbolizerRulePtr rule);
    bool ParseSymbolizer(SymbolizerPtr& symbolizer);
    bool ParsePointSymbolizer(PointSymbolizerPtr& symbolizer);
    bool ParseLineSymbolizer(LineSymbolizerPtr& symbolizer);
    bool ParsePolygonSymbolizer(PolygonSymbolizerPtr& symbolizer);
    bool ParseTextSymbolizer(TextSymbolizerPtr& symbolizer);
    bool ParseRasterSymbolizer(RasterSymbolizerPtr& symbolizer);
    
    bool ParseFilter(FilterPtr& filter);
    bool ParseComparisonFilter(FilterPtr& filter);
    bool ParseLogicalFilter(FilterPtr& filter);
    bool ParseSpatialFilter(FilterPtr& filter);
    bool ParseBinaryComparisonFilter(FilterPtr& filter);
    bool ParsePropertyIsBetween(FilterPtr& filter);
    bool ParsePropertyIsLike(FilterPtr& filter);
    bool ParsePropertyIsNull(FilterPtr& filter);
    
    bool ParseParameter(std::map<std::string, std::string>& params, const std::string& tagName);
    
    std::string ReadElement(const std::string& tagName);
    std::string ReadElementContent(const std::string& tagName);
    bool SkipToElement(const std::string& tagName);
    bool SkipToEndElement(const std::string& tagName);
    std::string ReadAttribute(const std::string& tagName, const std::string& attrName);
    
    std::string Trim(const std::string& str) const;
    std::string ToLower(const std::string& str) const;
    std::string GetElementName(const std::string& fullName) const;
    
    uint32_t ParseColor(const std::string& colorStr);
    double ParseDouble(const std::string& str, double defaultVal = 0.0);
    int ParseInt(const std::string& str, int defaultVal = 0);
    
    static std::string ColorToString(uint32_t color);
    static std::string DoubleToString(double val);
    
    void SetError(const std::string& message);
    void ClearError();
    
    static std::string GenerateIndent(int level);
    static std::string GeneratePointSymbolizer(const PointSymbolizerPtr& symbolizer, int indent);
    static std::string GenerateLineSymbolizer(const LineSymbolizerPtr& symbolizer, int indent);
    static std::string GeneratePolygonSymbolizer(const PolygonSymbolizerPtr& symbolizer, int indent);
    static std::string GenerateTextSymbolizer(const TextSymbolizerPtr& symbolizer, int indent);
    static std::string GenerateRasterSymbolizer(const RasterSymbolizerPtr& symbolizer, int indent);
    static std::string GenerateFilter(const FilterPtr& filter, int indent);
    static std::string GenerateComparisonFilter(const ComparisonFilterPtr& filter, int indent);
    static std::string GenerateLogicalFilter(const LogicalFilterPtr& filter, int indent);
    static std::string GenerateSpatialFilter(const SpatialFilterPtr& filter, int indent);
};

}
}

#endif
