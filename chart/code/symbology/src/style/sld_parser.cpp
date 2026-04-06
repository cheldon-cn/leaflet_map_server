#include "ogc/symbology/style/sld_parser.h"
#include "ogc/symbology/symbolizer/point_symbolizer.h"
#include "ogc/symbology/symbolizer/line_symbolizer.h"
#include "ogc/symbology/symbolizer/polygon_symbolizer.h"
#include "ogc/symbology/symbolizer/text_symbolizer.h"
#include "ogc/symbology/symbolizer/raster_symbolizer.h"
#include "ogc/symbology/filter/comparison_filter.h"
#include "ogc/symbology/filter/logical_filter.h"
#include "ogc/symbology/filter/spatial_filter.h"
#include <ogc/draw/font.h>
#include <ogc/draw/draw_style.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cstdlib>

namespace ogc {
namespace symbology {

using ogc::draw::Font;
using ogc::draw::LineCap;
using ogc::draw::LineJoin;

SldParser::SldParser()
    : m_strictMode(false)
    , m_defaultVersion("1.0.0")
    , m_currentPos(0)
{
}

SldParserPtr SldParser::Create()
{
    return std::make_shared<SldParser>();
}

SldParseResult SldParser::Parse(const std::string& sldContent)
{
    SldParseResult result;
    ClearError();
    
    m_content = sldContent;
    m_currentPos = 0;
    m_sld = SldStyledLayerDescriptor();
    
    if (sldContent.empty()) {
        SetError("SLD content is empty");
        result.errorMessage = m_lastError;
        return result;
    }
    
    if (!ParseSldElement()) {
        result.errorMessage = m_lastError;
        return result;
    }
    
    result.success = true;
    result.rules = GetRules();
    return result;
}

SldParseResult SldParser::ParseFile(const std::string& filePath)
{
    SldParseResult result;
    
    std::ifstream file(filePath);
    if (!file.is_open()) {
        SetError("Cannot open file: " + filePath);
        result.errorMessage = m_lastError;
        return result;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    return Parse(buffer.str());
}

std::vector<SymbolizerRulePtr> SldParser::GetRules() const
{
    std::vector<SymbolizerRulePtr> allRules;
    
    for (const auto& namedLayer : m_sld.namedLayers) {
        for (const auto& fts : namedLayer.featureTypeStyles) {
            for (const auto& rule : fts.rules) {
                allRules.push_back(rule);
            }
        }
    }
    
    return allRules;
}

std::vector<SymbolizerRulePtr> SldParser::ParseSld(const std::string& sldContent)
{
    SldParser parser;
    auto result = parser.Parse(sldContent);
    return result.rules;
}

std::vector<SymbolizerRulePtr> SldParser::ParseSldFile(const std::string& filePath)
{
    SldParser parser;
    auto result = parser.ParseFile(filePath);
    return result.rules;
}

bool SldParser::ParseSldElement()
{
    if (!SkipToElement("StyledLayerDescriptor")) {
        SetError("Cannot find StyledLayerDescriptor element");
        return false;
    }
    
    std::string version = ReadAttribute("StyledLayerDescriptor", "version");
    if (!version.empty()) {
        m_sld.version = version;
    } else {
        m_sld.version = m_defaultVersion;
    }
    
    size_t sldStart = m_content.find("<StyledLayerDescriptor");
    if (sldStart == std::string::npos) {
        return false;
    }
    
    size_t sldEnd = m_content.find("</StyledLayerDescriptor>");
    if (sldEnd == std::string::npos) {
        if (m_strictMode) {
            SetError("Missing closing StyledLayerDescriptor tag");
            return false;
        }
        sldEnd = m_content.length();
    }
    
    std::string sldContent = m_content.substr(sldStart, sldEnd - sldStart + 24);
    
    while (true) {
        size_t namedLayerPos = sldContent.find("<NamedLayer", m_currentPos);
        size_t userLayerPos = sldContent.find("<UserLayer", m_currentPos);
        
        if (namedLayerPos == std::string::npos && userLayerPos == std::string::npos) {
            break;
        }
        
        size_t nextPos = std::min(namedLayerPos, userLayerPos);
        m_currentPos = nextPos;
        
        if (nextPos == namedLayerPos && namedLayerPos != std::string::npos) {
            if (!ParseNamedLayer()) {
                if (m_strictMode) {
                    return false;
                }
            }
        } else if (nextPos == userLayerPos && userLayerPos != std::string::npos) {
            if (!ParseUserLayer()) {
                if (m_strictMode) {
                    return false;
                }
            }
        }
    }
    
    return true;
}

bool SldParser::ParseNamedLayer()
{
    SldNamedLayer namedLayer;
    
    namedLayer.name = ReadElementContent("Name");
    
    while (true) {
        size_t ftsPos = m_content.find("<FeatureTypeStyle", m_currentPos);
        size_t endNamedLayerPos = m_content.find("</NamedLayer>", m_currentPos);
        
        if (ftsPos == std::string::npos || ftsPos > endNamedLayerPos) {
            break;
        }
        
        m_currentPos = ftsPos;
        
        SldFeatureTypeStyle fts;
        if (ParseFeatureTypeStyle(fts)) {
            namedLayer.featureTypeStyles.push_back(fts);
        }
    }
    
    m_sld.namedLayers.push_back(namedLayer);
    SkipToEndElement("NamedLayer");
    return true;
}

bool SldParser::ParseUserLayer()
{
    SldNamedLayer userLayer;
    
    userLayer.name = ReadElementContent("Name");
    
    while (true) {
        size_t ftsPos = m_content.find("<FeatureTypeStyle", m_currentPos);
        size_t endUserLayerPos = m_content.find("</UserLayer>", m_currentPos);
        
        if (ftsPos == std::string::npos || ftsPos > endUserLayerPos) {
            break;
        }
        
        m_currentPos = ftsPos;
        
        SldFeatureTypeStyle fts;
        if (ParseFeatureTypeStyle(fts)) {
            userLayer.featureTypeStyles.push_back(fts);
        }
    }
    
    m_sld.namedLayers.push_back(userLayer);
    SkipToEndElement("UserLayer");
    return true;
}

bool SldParser::ParseFeatureTypeStyle(SldFeatureTypeStyle& style)
{
    size_t ftsStart = m_content.find("<FeatureTypeStyle", m_currentPos);
    if (ftsStart == std::string::npos) {
        return false;
    }
    
    size_t ftsEnd = m_content.find("</FeatureTypeStyle>", ftsStart);
    if (ftsEnd == std::string::npos) {
        if (m_strictMode) {
            SetError("Missing closing FeatureTypeStyle tag");
            return false;
        }
        ftsEnd = m_content.length();
    }
    
    std::string ftsContent = m_content.substr(ftsStart, ftsEnd - ftsStart + 19);
    m_currentPos = ftsStart;
    
    style.name = ReadElementContent("Name");
    style.title = ReadElementContent("Title");
    style.abstract = ReadElementContent("Abstract");
    
    while (true) {
        size_t rulePos = ftsContent.find("<Rule", m_currentPos - ftsStart);
        if (rulePos == std::string::npos || rulePos > ftsEnd - ftsStart) {
            break;
        }
        
        m_currentPos = ftsStart + rulePos;
        
        SymbolizerRulePtr rule = SymbolizerRule::Create();
        if (ParseRule(rule)) {
            style.rules.push_back(rule);
        }
    }
    
    m_currentPos = ftsEnd + 19;
    return true;
}

bool SldParser::ParseRule(SymbolizerRulePtr rule)
{
    size_t ruleStart = m_content.find("<Rule", m_currentPos);
    if (ruleStart == std::string::npos) {
        return false;
    }
    
    size_t ruleEnd = m_content.find("</Rule>", ruleStart);
    if (ruleEnd == std::string::npos) {
        if (m_strictMode) {
            SetError("Missing closing Rule tag");
            return false;
        }
        ruleEnd = m_content.length();
    }
    
    m_currentPos = ruleStart;
    
    std::string name = ReadElementContent("Name");
    if (!name.empty()) {
        rule->SetName(name);
    }
    
    std::string title = ReadElementContent("Title");
    if (!title.empty()) {
        rule->SetTitle(title);
    }
    
    std::string abstract = ReadElementContent("Abstract");
    if (!abstract.empty()) {
        rule->SetAbstract(abstract);
    }
    
    std::string minScale = ReadElementContent("MinScaleDenominator");
    if (!minScale.empty()) {
        rule->SetMinScaleDenominator(ParseDouble(minScale, 0.0));
    }
    
    std::string maxScale = ReadElementContent("MaxScaleDenominator");
    if (!maxScale.empty()) {
        rule->SetMaxScaleDenominator(ParseDouble(maxScale, std::numeric_limits<double>::max()));
    }
    
    size_t filterPos = m_content.find("<Filter", m_currentPos);
    size_t elseFilterPos = m_content.find("<ElseFilter", m_currentPos);
    
    if (filterPos != std::string::npos && filterPos < ruleEnd) {
        m_currentPos = filterPos;
        FilterPtr filter;
        if (ParseFilter(filter)) {
            rule->SetFilter(filter);
        }
    } else if (elseFilterPos != std::string::npos && elseFilterPos < ruleEnd) {
        rule->SetElseFilter(true);
        SkipToEndElement("ElseFilter");
    }
    
    while (m_currentPos < ruleEnd) {
        SymbolizerPtr symbolizer;
        if (ParseSymbolizer(symbolizer)) {
            rule->AddSymbolizer(symbolizer);
        } else {
            break;
        }
    }
    
    m_currentPos = ruleEnd + 7;
    return true;
}

bool SldParser::ParseSymbolizer(SymbolizerPtr& symbolizer)
{
    size_t pos = m_currentPos;
    
    if (m_content.find("<PointSymbolizer", pos) != std::string::npos &&
        m_content.find("<PointSymbolizer", pos) < m_content.find("</Rule>", pos)) {
        m_currentPos = m_content.find("<PointSymbolizer", pos);
        PointSymbolizerPtr pointSym;
        if (ParsePointSymbolizer(pointSym)) {
            symbolizer = std::static_pointer_cast<Symbolizer>(pointSym);
            return true;
        }
    }
    
    if (m_content.find("<LineSymbolizer", pos) != std::string::npos &&
        m_content.find("<LineSymbolizer", pos) < m_content.find("</Rule>", pos)) {
        m_currentPos = m_content.find("<LineSymbolizer", pos);
        LineSymbolizerPtr lineSym;
        if (ParseLineSymbolizer(lineSym)) {
            symbolizer = std::static_pointer_cast<Symbolizer>(lineSym);
            return true;
        }
    }
    
    if (m_content.find("<PolygonSymbolizer", pos) != std::string::npos &&
        m_content.find("<PolygonSymbolizer", pos) < m_content.find("</Rule>", pos)) {
        m_currentPos = m_content.find("<PolygonSymbolizer", pos);
        PolygonSymbolizerPtr polySym;
        if (ParsePolygonSymbolizer(polySym)) {
            symbolizer = std::static_pointer_cast<Symbolizer>(polySym);
            return true;
        }
    }
    
    if (m_content.find("<TextSymbolizer", pos) != std::string::npos &&
        m_content.find("<TextSymbolizer", pos) < m_content.find("</Rule>", pos)) {
        m_currentPos = m_content.find("<TextSymbolizer", pos);
        TextSymbolizerPtr textSym;
        if (ParseTextSymbolizer(textSym)) {
            symbolizer = std::static_pointer_cast<Symbolizer>(textSym);
            return true;
        }
    }
    
    if (m_content.find("<RasterSymbolizer", pos) != std::string::npos &&
        m_content.find("<RasterSymbolizer", pos) < m_content.find("</Rule>", pos)) {
        m_currentPos = m_content.find("<RasterSymbolizer", pos);
        RasterSymbolizerPtr rasterSym;
        if (ParseRasterSymbolizer(rasterSym)) {
            symbolizer = std::static_pointer_cast<Symbolizer>(rasterSym);
            return true;
        }
    }
    
    return false;
}

bool SldParser::ParsePointSymbolizer(PointSymbolizerPtr& symbolizer)
{
    symbolizer = PointSymbolizer::Create();
    
    size_t symEnd = m_content.find("</PointSymbolizer>", m_currentPos);
    if (symEnd == std::string::npos) {
        return false;
    }
    
    std::string graphicContent = ReadElementContent("Graphic");
    if (!graphicContent.empty()) {
        std::string sizeStr = ReadElementContent("Size");
        if (!sizeStr.empty()) {
            symbolizer->SetSize(ParseDouble(sizeStr, 6.0));
        }
        
        std::string rotationStr = ReadElementContent("Rotation");
        if (!rotationStr.empty()) {
            symbolizer->SetRotation(ParseDouble(rotationStr, 0.0));
        }
    }
    
    std::string markContent = ReadElementContent("Mark");
    if (!markContent.empty()) {
        std::string wellKnownName = ReadElementContent("WellKnownName");
        if (!wellKnownName.empty()) {
            std::string lowerName = ToLower(wellKnownName);
            if (lowerName == "circle") {
                symbolizer->SetSymbolType(PointSymbolType::kCircle);
            } else if (lowerName == "square") {
                symbolizer->SetSymbolType(PointSymbolType::kSquare);
            } else if (lowerName == "triangle") {
                symbolizer->SetSymbolType(PointSymbolType::kTriangle);
            } else if (lowerName == "star") {
                symbolizer->SetSymbolType(PointSymbolType::kStar);
            } else if (lowerName == "cross") {
                symbolizer->SetSymbolType(PointSymbolType::kCross);
            } else if (lowerName == "diamond" || lowerName == "x") {
                symbolizer->SetSymbolType(PointSymbolType::kDiamond);
            }
        }
        
        std::string fillContent = ReadElementContent("Fill");
        if (!fillContent.empty()) {
            std::map<std::string, std::string> params;
            if (ParseParameter(params, "CssParameter")) {
                if (params.find("fill") != params.end()) {
                    symbolizer->SetColor(ParseColor(params["fill"]));
                }
                if (params.find("fill-opacity") != params.end()) {
                }
            }
        }
        
        std::string strokeContent = ReadElementContent("Stroke");
        if (!strokeContent.empty()) {
            std::map<std::string, std::string> params;
            if (ParseParameter(params, "CssParameter")) {
                if (params.find("stroke") != params.end()) {
                    symbolizer->SetStrokeColor(ParseColor(params["stroke"]));
                }
                if (params.find("stroke-width") != params.end()) {
                    symbolizer->SetStrokeWidth(ParseDouble(params["stroke-width"], 1.0));
                }
            }
        }
    }
    
    m_currentPos = symEnd + 18;
    return true;
}

bool SldParser::ParseLineSymbolizer(LineSymbolizerPtr& symbolizer)
{
    symbolizer = LineSymbolizer::Create();
    
    size_t symEnd = m_content.find("</LineSymbolizer>", m_currentPos);
    if (symEnd == std::string::npos) {
        return false;
    }
    
    std::string strokeContent = ReadElementContent("Stroke");
    if (!strokeContent.empty()) {
        std::map<std::string, std::string> params;
        if (ParseParameter(params, "CssParameter")) {
            if (params.find("stroke") != params.end()) {
                symbolizer->SetColor(ParseColor(params["stroke"]));
            }
            if (params.find("stroke-width") != params.end()) {
                symbolizer->SetWidth(ParseDouble(params["stroke-width"], 1.0));
            }
            if (params.find("stroke-opacity") != params.end()) {
                symbolizer->SetOpacity(ParseDouble(params["stroke-opacity"], 1.0));
            }
            if (params.find("stroke-linecap") != params.end()) {
                std::string cap = ToLower(params["stroke-linecap"]);
                if (cap == "butt") {
                    symbolizer->SetCapStyle(LineCap::kFlat);
                } else if (cap == "round") {
                    symbolizer->SetCapStyle(LineCap::kRound);
                } else if (cap == "square") {
                    symbolizer->SetCapStyle(LineCap::kSquare);
                }
            }
            if (params.find("stroke-linejoin") != params.end()) {
                std::string join = ToLower(params["stroke-linejoin"]);
                if (join == "miter") {
                    symbolizer->SetJoinStyle(LineJoin::kMiter);
                } else if (join == "round") {
                    symbolizer->SetJoinStyle(LineJoin::kRound);
                } else if (join == "bevel") {
                    symbolizer->SetJoinStyle(LineJoin::kBevel);
                }
            }
            if (params.find("stroke-dasharray") != params.end()) {
                std::string dashArray = params["stroke-dasharray"];
                std::vector<double> pattern;
                std::stringstream ss(dashArray);
                std::string token;
                while (std::getline(ss, token, ' ')) {
                    if (!token.empty()) {
                        pattern.push_back(ParseDouble(token, 0.0));
                    }
                }
                if (!pattern.empty()) {
                    symbolizer->SetDashPattern(pattern);
                    symbolizer->SetDashStyle(DashStyle::kCustom);
                }
            }
        }
    }
    
    std::string perpOffset = ReadElementContent("PerpendicularOffset");
    if (!perpOffset.empty()) {
        symbolizer->SetPerpendicularOffset(ParseDouble(perpOffset, 0.0));
    }
    
    m_currentPos = symEnd + 17;
    return true;
}

bool SldParser::ParsePolygonSymbolizer(PolygonSymbolizerPtr& symbolizer)
{
    symbolizer = PolygonSymbolizer::Create();
    
    size_t symEnd = m_content.find("</PolygonSymbolizer>", m_currentPos);
    if (symEnd == std::string::npos) {
        return false;
    }
    
    std::string fillContent = ReadElementContent("Fill");
    if (!fillContent.empty()) {
        std::map<std::string, std::string> params;
        if (ParseParameter(params, "CssParameter")) {
            if (params.find("fill") != params.end()) {
                symbolizer->SetFillColor(ParseColor(params["fill"]));
            }
            if (params.find("fill-opacity") != params.end()) {
                symbolizer->SetFillOpacity(ParseDouble(params["fill-opacity"], 1.0));
            }
        }
    }
    
    std::string strokeContent = ReadElementContent("Stroke");
    if (!strokeContent.empty()) {
        std::map<std::string, std::string> params;
        if (ParseParameter(params, "CssParameter")) {
            if (params.find("stroke") != params.end()) {
                symbolizer->SetStrokeColor(ParseColor(params["stroke"]));
            }
            if (params.find("stroke-width") != params.end()) {
                symbolizer->SetStrokeWidth(ParseDouble(params["stroke-width"], 1.0));
            }
            if (params.find("stroke-opacity") != params.end()) {
                symbolizer->SetStrokeOpacity(ParseDouble(params["stroke-opacity"], 1.0));
            }
        }
    }
    
    std::string displacement = ReadElementContent("Displacement");
    if (!displacement.empty()) {
        std::string dx = ReadElementContent("DisplacementX");
        std::string dy = ReadElementContent("DisplacementY");
        if (!dx.empty() || !dy.empty()) {
            symbolizer->SetDisplacement(ParseDouble(dx, 0.0), ParseDouble(dy, 0.0));
        }
    }
    
    m_currentPos = symEnd + 20;
    return true;
}

bool SldParser::ParseTextSymbolizer(TextSymbolizerPtr& symbolizer)
{
    symbolizer = TextSymbolizer::Create();
    
    size_t symEnd = m_content.find("</TextSymbolizer>", m_currentPos);
    if (symEnd == std::string::npos) {
        return false;
    }
    
    std::string label = ReadElementContent("Label");
    if (!label.empty()) {
        if (label.find("<ogc:PropertyName") != std::string::npos ||
            label.find("<PropertyName") != std::string::npos) {
            size_t start = label.find(">");
            size_t end = label.find("</");
            if (start != std::string::npos && end != std::string::npos) {
                std::string propName = label.substr(start + 1, end - start - 1);
                symbolizer->SetLabelProperty(Trim(propName));
            }
        } else {
            symbolizer->SetLabel(Trim(label));
        }
    }
    
    std::string fontContent = ReadElementContent("Font");
    if (!fontContent.empty()) {
        std::map<std::string, std::string> params;
        if (ParseParameter(params, "CssParameter")) {
            Font font;
            if (params.find("font-family") != params.end()) {
                font.SetFamily(params["font-family"]);
            }
            if (params.find("font-size") != params.end()) {
                font.SetSize(static_cast<int>(ParseDouble(params["font-size"], 12.0)));
            }
            if (params.find("font-style") != params.end()) {
                std::string style = ToLower(params["font-style"]);
                font.SetItalic(style == "italic" || style == "oblique");
            }
            if (params.find("font-weight") != params.end()) {
                std::string weight = ToLower(params["font-weight"]);
                font.SetBold(weight == "bold");
            }
            symbolizer->SetFont(font);
        }
    }
    
    std::string fillContent = ReadElementContent("Fill");
    if (!fillContent.empty()) {
        std::map<std::string, std::string> params;
        if (ParseParameter(params, "CssParameter")) {
            if (params.find("fill") != params.end()) {
                symbolizer->SetColor(ParseColor(params["fill"]));
            }
        }
    }
    
    std::string haloContent = ReadElementContent("Halo");
    if (!haloContent.empty()) {
        std::string radius = ReadElementContent("Radius");
        if (!radius.empty()) {
            symbolizer->SetHaloRadius(ParseDouble(radius, 1.0));
        }
        
        std::string haloFill = ReadElementContent("Fill");
        if (!haloFill.empty()) {
            std::map<std::string, std::string> params;
            if (ParseParameter(params, "CssParameter")) {
                if (params.find("fill") != params.end()) {
                    symbolizer->SetHaloColor(ParseColor(params["fill"]));
                }
            }
        }
    }
    
    std::string placement = ReadElementContent("LabelPlacement");
    if (!placement.empty()) {
        std::string pointPlacement = ReadElementContent("PointPlacement");
        if (!pointPlacement.empty()) {
            std::string anchor = ReadElementContent("AnchorPoint");
            if (!anchor.empty()) {
                std::string ax = ReadElementContent("AnchorPointX");
                std::string ay = ReadElementContent("AnchorPointY");
                if (!ax.empty() || !ay.empty()) {
                    symbolizer->SetAnchorPoint(ParseDouble(ax, 0.5), ParseDouble(ay, 0.5));
                }
            }
            
            std::string disp = ReadElementContent("Displacement");
            if (!disp.empty()) {
                std::string dx = ReadElementContent("DisplacementX");
                std::string dy = ReadElementContent("DisplacementY");
                if (!dx.empty() || !dy.empty()) {
                    symbolizer->SetDisplacement(ParseDouble(dx, 0.0), ParseDouble(dy, 0.0));
                }
            }
            
            std::string rotation = ReadElementContent("Rotation");
            if (!rotation.empty()) {
                symbolizer->SetRotation(ParseDouble(rotation, 0.0));
            }
        }
        
        std::string linePlacement = ReadElementContent("LinePlacement");
        if (!linePlacement.empty()) {
            std::string offset = ReadElementContent("PerpendicularOffset");
            if (!offset.empty()) {
                symbolizer->SetPerpendicularOffset(ParseDouble(offset, 0.0));
            }
        }
    }
    
    m_currentPos = symEnd + 17;
    return true;
}

bool SldParser::ParseRasterSymbolizer(RasterSymbolizerPtr& symbolizer)
{
    symbolizer = RasterSymbolizer::Create();
    
    size_t symEnd = m_content.find("</RasterSymbolizer>", m_currentPos);
    if (symEnd == std::string::npos) {
        return false;
    }
    
    std::string opacity = ReadElementContent("Opacity");
    if (!opacity.empty()) {
        symbolizer->SetOpacity(ParseDouble(opacity, 1.0));
    }
    
    std::string colorMap = ReadElementContent("ColorMap");
    if (!colorMap.empty()) {
    }
    
    m_currentPos = symEnd + 19;
    return true;
}

bool SldParser::ParseFilter(FilterPtr& filter)
{
    size_t filterStart = m_content.find("<Filter", m_currentPos);
    if (filterStart == std::string::npos) {
        return false;
    }
    
    size_t filterEnd = m_content.find("</Filter>", filterStart);
    if (filterEnd == std::string::npos) {
        return false;
    }
    
    m_currentPos = filterStart;
    
    if (m_content.find("<PropertyIsEqualTo", m_currentPos) != std::string::npos &&
        m_content.find("<PropertyIsEqualTo", m_currentPos) < filterEnd) {
        return ParseComparisonFilter(filter);
    }
    
    if (m_content.find("<PropertyIsNotEqualTo", m_currentPos) != std::string::npos &&
        m_content.find("<PropertyIsNotEqualTo", m_currentPos) < filterEnd) {
        return ParseComparisonFilter(filter);
    }
    
    if (m_content.find("<PropertyIsLessThan", m_currentPos) != std::string::npos &&
        m_content.find("<PropertyIsLessThan", m_currentPos) < filterEnd) {
        return ParseComparisonFilter(filter);
    }
    
    if (m_content.find("<PropertyIsGreaterThan", m_currentPos) != std::string::npos &&
        m_content.find("<PropertyIsGreaterThan", m_currentPos) < filterEnd) {
        return ParseComparisonFilter(filter);
    }
    
    if (m_content.find("<PropertyIsLessThanOrEqualTo", m_currentPos) != std::string::npos &&
        m_content.find("<PropertyIsLessThanOrEqualTo", m_currentPos) < filterEnd) {
        return ParseComparisonFilter(filter);
    }
    
    if (m_content.find("<PropertyIsGreaterThanOrEqualTo", m_currentPos) != std::string::npos &&
        m_content.find("<PropertyIsGreaterThanOrEqualTo", m_currentPos) < filterEnd) {
        return ParseComparisonFilter(filter);
    }
    
    if (m_content.find("<PropertyIsBetween", m_currentPos) != std::string::npos &&
        m_content.find("<PropertyIsBetween", m_currentPos) < filterEnd) {
        return ParsePropertyIsBetween(filter);
    }
    
    if (m_content.find("<PropertyIsLike", m_currentPos) != std::string::npos &&
        m_content.find("<PropertyIsLike", m_currentPos) < filterEnd) {
        return ParsePropertyIsLike(filter);
    }
    
    if (m_content.find("<PropertyIsNull", m_currentPos) != std::string::npos &&
        m_content.find("<PropertyIsNull", m_currentPos) < filterEnd) {
        return ParsePropertyIsNull(filter);
    }
    
    if (m_content.find("<And", m_currentPos) != std::string::npos &&
        m_content.find("<And", m_currentPos) < filterEnd) {
        return ParseLogicalFilter(filter);
    }
    
    if (m_content.find("<Or", m_currentPos) != std::string::npos &&
        m_content.find("<Or", m_currentPos) < filterEnd) {
        return ParseLogicalFilter(filter);
    }
    
    if (m_content.find("<Not", m_currentPos) != std::string::npos &&
        m_content.find("<Not", m_currentPos) < filterEnd) {
        return ParseLogicalFilter(filter);
    }
    
    if (m_content.find("<BBOX", m_currentPos) != std::string::npos &&
        m_content.find("<BBOX", m_currentPos) < filterEnd) {
        return ParseSpatialFilter(filter);
    }
    
    if (m_content.find("<Intersects", m_currentPos) != std::string::npos &&
        m_content.find("<Intersects", m_currentPos) < filterEnd) {
        return ParseSpatialFilter(filter);
    }
    
    if (m_content.find("<Within", m_currentPos) != std::string::npos &&
        m_content.find("<Within", m_currentPos) < filterEnd) {
        return ParseSpatialFilter(filter);
    }
    
    if (m_content.find("<Contains", m_currentPos) != std::string::npos &&
        m_content.find("<Contains", m_currentPos) < filterEnd) {
        return ParseSpatialFilter(filter);
    }
    
    m_currentPos = filterEnd + 9;
    return false;
}

bool SldParser::ParseComparisonFilter(FilterPtr& filter)
{
    ComparisonOperator op = ComparisonOperator::kEqual;
    std::string tagName;
    
    if (m_content.find("<PropertyIsEqualTo", m_currentPos) != std::string::npos) {
        op = ComparisonOperator::kEqual;
        tagName = "PropertyIsEqualTo";
    } else if (m_content.find("<PropertyIsNotEqualTo", m_currentPos) != std::string::npos) {
        op = ComparisonOperator::kNotEqual;
        tagName = "PropertyIsNotEqualTo";
    } else if (m_content.find("<PropertyIsLessThan", m_currentPos) != std::string::npos) {
        op = ComparisonOperator::kLessThan;
        tagName = "PropertyIsLessThan";
    } else if (m_content.find("<PropertyIsGreaterThan", m_currentPos) != std::string::npos) {
        op = ComparisonOperator::kGreaterThan;
        tagName = "PropertyIsGreaterThan";
    } else if (m_content.find("<PropertyIsLessThanOrEqualTo", m_currentPos) != std::string::npos) {
        op = ComparisonOperator::kLessThanOrEqual;
        tagName = "PropertyIsLessThanOrEqualTo";
    } else if (m_content.find("<PropertyIsGreaterThanOrEqualTo", m_currentPos) != std::string::npos) {
        op = ComparisonOperator::kGreaterThanOrEqual;
        tagName = "PropertyIsGreaterThanOrEqualTo";
    } else {
        return false;
    }
    
    std::string propertyName = ReadElementContent("PropertyName");
    std::string literal = ReadElementContent("Literal");
    
    filter = std::make_shared<ComparisonFilter>(op, propertyName, literal);
    
    return true;
}

bool SldParser::ParseLogicalFilter(FilterPtr& filter)
{
    LogicalOperator op = LogicalOperator::kAnd;
    std::string tagName;
    
    if (m_content.find("<And", m_currentPos) != std::string::npos) {
        op = LogicalOperator::kAnd;
        tagName = "And";
    } else if (m_content.find("<Or", m_currentPos) != std::string::npos) {
        op = LogicalOperator::kOr;
        tagName = "Or";
    } else if (m_content.find("<Not", m_currentPos) != std::string::npos) {
        op = LogicalOperator::kNot;
        tagName = "Not";
    } else {
        return false;
    }
    
    LogicalFilterPtr logicalFilter = std::make_shared<LogicalFilter>(op);
    
    size_t tagStart = m_content.find("<" + tagName, m_currentPos);
    size_t tagEnd = m_content.find("</" + tagName + ">", tagStart);
    
    if (tagStart == std::string::npos || tagEnd == std::string::npos) {
        return false;
    }
    
    size_t savedPos = m_currentPos;
    m_currentPos = tagStart + tagName.length() + 1;
    
    while (m_currentPos < tagEnd) {
        FilterPtr childFilter;
        if (ParseFilter(childFilter)) {
            logicalFilter->AddFilter(childFilter);
        } else {
            break;
        }
    }
    
    m_currentPos = tagEnd + tagName.length() + 3;
    filter = logicalFilter;
    return true;
}

bool SldParser::ParseSpatialFilter(FilterPtr& filter)
{
    SpatialOperator op = SpatialOperator::kBbox;
    std::string tagName;
    
    if (m_content.find("<BBOX", m_currentPos) != std::string::npos) {
        op = SpatialOperator::kBbox;
        tagName = "BBOX";
    } else if (m_content.find("<Intersects", m_currentPos) != std::string::npos) {
        op = SpatialOperator::kIntersects;
        tagName = "Intersects";
    } else if (m_content.find("<Within", m_currentPos) != std::string::npos) {
        op = SpatialOperator::kWithin;
        tagName = "Within";
    } else if (m_content.find("<Contains", m_currentPos) != std::string::npos) {
        op = SpatialOperator::kContains;
        tagName = "Contains";
    } else {
        return false;
    }
    
    std::string propertyName = ReadElementContent("PropertyName");
    
    SpatialFilterPtr spatialFilter = std::make_shared<SpatialFilter>(op, static_cast<const Geometry*>(nullptr));
    spatialFilter->SetPropertyName(propertyName);
    filter = spatialFilter;
    
    return true;
}

bool SldParser::ParseBinaryComparisonFilter(FilterPtr& filter)
{
    return ParseComparisonFilter(filter);
}

bool SldParser::ParsePropertyIsBetween(FilterPtr& filter)
{
    std::string propertyName = ReadElementContent("PropertyName");
    std::string lower = ReadElementContent("LowerBoundary");
    std::string upper = ReadElementContent("UpperBoundary");
    
    filter = std::make_shared<ComparisonFilter>(ComparisonOperator::kBetween, propertyName, lower, upper);
    
    return true;
}

bool SldParser::ParsePropertyIsLike(FilterPtr& filter)
{
    std::string propertyName = ReadElementContent("PropertyName");
    std::string literal = ReadElementContent("Literal");
    
    filter = std::make_shared<ComparisonFilter>(ComparisonOperator::kLike, propertyName, literal);
    
    return true;
}

bool SldParser::ParsePropertyIsNull(FilterPtr& filter)
{
    std::string propertyName = ReadElementContent("PropertyName");
    
    filter = std::make_shared<ComparisonFilter>(ComparisonOperator::kIsNull, propertyName, "");
    
    return true;
}

bool SldParser::ParseParameter(std::map<std::string, std::string>& params, const std::string& tagName)
{
    size_t pos = m_currentPos;
    
    while (true) {
        size_t paramStart = m_content.find("<" + tagName, pos);
        if (paramStart == std::string::npos) {
            break;
        }
        
        size_t paramEnd = m_content.find("</" + tagName + ">", paramStart);
        if (paramEnd == std::string::npos) {
            break;
        }
        
        std::string nameAttr = ReadAttribute(tagName, "name");
        
        size_t contentStart = m_content.find(">", paramStart);
        if (contentStart == std::string::npos || contentStart > paramEnd) {
            break;
        }
        
        std::string content = m_content.substr(contentStart + 1, paramEnd - contentStart - 1);
        params[nameAttr] = Trim(content);
        
        pos = paramEnd + tagName.length() + 3;
    }
    
    return !params.empty();
}

std::string SldParser::ReadElement(const std::string& tagName)
{
    size_t start = m_content.find("<" + tagName, m_currentPos);
    if (start == std::string::npos) {
        return "";
    }
    
    size_t end = m_content.find("</" + tagName + ">", start);
    if (end == std::string::npos) {
        return "";
    }
    
    return m_content.substr(start, end - start + tagName.length() + 3);
}

std::string SldParser::ReadElementContent(const std::string& tagName)
{
    size_t start = m_content.find("<" + tagName, m_currentPos);
    if (start == std::string::npos) {
        return "";
    }
    
    size_t contentStart = m_content.find(">", start);
    if (contentStart == std::string::npos) {
        return "";
    }
    
    if (m_content[contentStart - 1] == '/') {
        return "";
    }
    
    size_t end = m_content.find("</" + tagName + ">", contentStart);
    if (end == std::string::npos) {
        return "";
    }
    
    return m_content.substr(contentStart + 1, end - contentStart - 1);
}

bool SldParser::SkipToElement(const std::string& tagName)
{
    size_t pos = m_content.find("<" + tagName, m_currentPos);
    if (pos == std::string::npos) {
        return false;
    }
    
    m_currentPos = pos;
    return true;
}

bool SldParser::SkipToEndElement(const std::string& tagName)
{
    size_t pos = m_content.find("</" + tagName + ">", m_currentPos);
    if (pos == std::string::npos) {
        return false;
    }
    
    m_currentPos = pos + tagName.length() + 3;
    return true;
}

std::string SldParser::ReadAttribute(const std::string& tagName, const std::string& attrName)
{
    size_t tagStart = m_content.find("<" + tagName, m_currentPos);
    if (tagStart == std::string::npos) {
        return "";
    }
    
    size_t tagEnd = m_content.find(">", tagStart);
    if (tagEnd == std::string::npos) {
        return "";
    }
    
    std::string tagContent = m_content.substr(tagStart, tagEnd - tagStart);
    
    std::string attrPattern = attrName + "=\"";
    size_t attrStart = tagContent.find(attrPattern);
    if (attrStart == std::string::npos) {
        return "";
    }
    
    attrStart += attrPattern.length();
    size_t attrEnd = tagContent.find("\"", attrStart);
    if (attrEnd == std::string::npos) {
        return "";
    }
    
    return tagContent.substr(attrStart, attrEnd - attrStart);
}

std::string SldParser::Trim(const std::string& str) const
{
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        return "";
    }
    
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string SldParser::ToLower(const std::string& str) const
{
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string SldParser::GetElementName(const std::string& fullName) const
{
    size_t colonPos = fullName.find(":");
    if (colonPos != std::string::npos) {
        return fullName.substr(colonPos + 1);
    }
    return fullName;
}

uint32_t SldParser::ParseColor(const std::string& colorStr)
{
    std::string str = Trim(colorStr);
    
    if (str.empty()) {
        return 0x000000FF;
    }
    
    if (str[0] == '#') {
        std::string hex = str.substr(1);
        if (hex.length() == 6) {
            uint32_t r = std::stoul(hex.substr(0, 2), nullptr, 16);
            uint32_t g = std::stoul(hex.substr(2, 2), nullptr, 16);
            uint32_t b = std::stoul(hex.substr(4, 2), nullptr, 16);
            return (r << 24) | (g << 16) | (b << 8) | 0xFF;
        } else if (hex.length() == 8) {
            uint32_t r = std::stoul(hex.substr(0, 2), nullptr, 16);
            uint32_t g = std::stoul(hex.substr(2, 2), nullptr, 16);
            uint32_t b = std::stoul(hex.substr(4, 2), nullptr, 16);
            uint32_t a = std::stoul(hex.substr(6, 2), nullptr, 16);
            return (r << 24) | (g << 16) | (b << 8) | a;
        }
    }
    
    return 0x000000FF;
}

double SldParser::ParseDouble(const std::string& str, double defaultVal)
{
    try {
        return std::stod(Trim(str));
    } catch (...) {
        return defaultVal;
    }
}

int SldParser::ParseInt(const std::string& str, int defaultVal)
{
    try {
        return std::stoi(Trim(str));
    } catch (...) {
        return defaultVal;
    }
}

std::string SldParser::ColorToString(uint32_t color)
{
    char buf[16];
    snprintf(buf, sizeof(buf), "#%02X%02X%02X",
             (color >> 24) & 0xFF,
             (color >> 16) & 0xFF,
             (color >> 8) & 0xFF);
    return std::string(buf);
}

std::string SldParser::DoubleToString(double val)
{
    char buf[32];
    snprintf(buf, sizeof(buf), "%.6f", val);
    return std::string(buf);
}

void SldParser::SetError(const std::string& message)
{
    m_lastError = message;
}

void SldParser::ClearError()
{
    m_lastError.clear();
}

std::string SldParser::GenerateSld(const std::vector<SymbolizerRulePtr>& rules)
{
    std::string sld = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    sld += "<StyledLayerDescriptor version=\"1.0.0\" xmlns=\"http://www.opengis.net/sld\">\n";
    sld += "  <NamedLayer>\n";
    sld += "    <Name>Default</Name>\n";
    sld += "    <UserStyle>\n";
    sld += "      <FeatureTypeStyle>\n";
    
    for (const auto& rule : rules) {
        sld += GenerateSld(rule);
    }
    
    sld += "      </FeatureTypeStyle>\n";
    sld += "    </UserStyle>\n";
    sld += "  </NamedLayer>\n";
    sld += "</StyledLayerDescriptor>\n";
    
    return sld;
}

std::string SldParser::GenerateSld(const SymbolizerRulePtr& rule)
{
    std::string sld = GenerateIndent(3) + "<Rule>\n";
    
    if (!rule->GetName().empty()) {
        sld += GenerateIndent(4) + "<Name>" + rule->GetName() + "</Name>\n";
    }
    
    if (!rule->GetTitle().empty()) {
        sld += GenerateIndent(4) + "<Title>" + rule->GetTitle() + "</Title>\n";
    }
    
    if (rule->GetMinScaleDenominator() > 0) {
        sld += GenerateIndent(4) + "<MinScaleDenominator>" + 
               DoubleToString(rule->GetMinScaleDenominator()) + "</MinScaleDenominator>\n";
    }
    
    if (rule->GetMaxScaleDenominator() < std::numeric_limits<double>::max()) {
        sld += GenerateIndent(4) + "<MaxScaleDenominator>" + 
               DoubleToString(rule->GetMaxScaleDenominator()) + "</MaxScaleDenominator>\n";
    }
    
    if (rule->HasFilter()) {
        sld += GenerateFilter(rule->GetFilter(), 4);
    }
    
    for (const auto& symbolizer : rule->GetSymbolizers()) {
        SymbolizerType type = symbolizer->GetType();
        switch (type) {
            case SymbolizerType::kPoint:
                sld += GeneratePointSymbolizer(
                    std::static_pointer_cast<PointSymbolizer>(symbolizer), 4);
                break;
            case SymbolizerType::kLine:
                sld += GenerateLineSymbolizer(
                    std::static_pointer_cast<LineSymbolizer>(symbolizer), 4);
                break;
            case SymbolizerType::kPolygon:
                sld += GeneratePolygonSymbolizer(
                    std::static_pointer_cast<PolygonSymbolizer>(symbolizer), 4);
                break;
            case SymbolizerType::kText:
                sld += GenerateTextSymbolizer(
                    std::static_pointer_cast<TextSymbolizer>(symbolizer), 4);
                break;
            case SymbolizerType::kRaster:
                sld += GenerateRasterSymbolizer(
                    std::static_pointer_cast<RasterSymbolizer>(symbolizer), 4);
                break;
            default:
                break;
        }
    }
    
    sld += GenerateIndent(3) + "</Rule>\n";
    return sld;
}

std::string SldParser::GenerateIndent(int level)
{
    return std::string(level * 2, ' ');
}

std::string SldParser::GeneratePointSymbolizer(const PointSymbolizerPtr& symbolizer, int indent)
{
    std::string sld = GenerateIndent(indent) + "<PointSymbolizer>\n";
    sld += GenerateIndent(indent + 1) + "<Graphic>\n";
    sld += GenerateIndent(indent + 2) + "<Mark>\n";
    sld += GenerateIndent(indent + 3) + "<WellKnownName>circle</WellKnownName>\n";
    sld += GenerateIndent(indent + 3) + "<Fill>\n";
    sld += GenerateIndent(indent + 4) + "<CssParameter name=\"fill\">" + 
           ColorToString(symbolizer->GetColor()) + "</CssParameter>\n";
    sld += GenerateIndent(indent + 3) + "</Fill>\n";
    sld += GenerateIndent(indent + 2) + "</Mark>\n";
    sld += GenerateIndent(indent + 2) + "<Size>" + DoubleToString(symbolizer->GetSize()) + "</Size>\n";
    sld += GenerateIndent(indent + 1) + "</Graphic>\n";
    sld += GenerateIndent(indent) + "</PointSymbolizer>\n";
    return sld;
}

std::string SldParser::GenerateLineSymbolizer(const LineSymbolizerPtr& symbolizer, int indent)
{
    std::string sld = GenerateIndent(indent) + "<LineSymbolizer>\n";
    sld += GenerateIndent(indent + 1) + "<Stroke>\n";
    sld += GenerateIndent(indent + 2) + "<CssParameter name=\"stroke\">" + 
           ColorToString(symbolizer->GetColor()) + "</CssParameter>\n";
    sld += GenerateIndent(indent + 2) + "<CssParameter name=\"stroke-width\">" + 
           DoubleToString(symbolizer->GetWidth()) + "</CssParameter>\n";
    sld += GenerateIndent(indent + 1) + "</Stroke>\n";
    sld += GenerateIndent(indent) + "</LineSymbolizer>\n";
    return sld;
}

std::string SldParser::GeneratePolygonSymbolizer(const PolygonSymbolizerPtr& symbolizer, int indent)
{
    std::string sld = GenerateIndent(indent) + "<PolygonSymbolizer>\n";
    sld += GenerateIndent(indent + 1) + "<Fill>\n";
    sld += GenerateIndent(indent + 2) + "<CssParameter name=\"fill\">" + 
           ColorToString(symbolizer->GetFillColor()) + "</CssParameter>\n";
    sld += GenerateIndent(indent + 1) + "</Fill>\n";
    sld += GenerateIndent(indent + 1) + "<Stroke>\n";
    sld += GenerateIndent(indent + 2) + "<CssParameter name=\"stroke\">" + 
           ColorToString(symbolizer->GetStrokeColor()) + "</CssParameter>\n";
    sld += GenerateIndent(indent + 2) + "<CssParameter name=\"stroke-width\">" + 
           DoubleToString(symbolizer->GetStrokeWidth()) + "</CssParameter>\n";
    sld += GenerateIndent(indent + 1) + "</Stroke>\n";
    sld += GenerateIndent(indent) + "</PolygonSymbolizer>\n";
    return sld;
}

std::string SldParser::GenerateTextSymbolizer(const TextSymbolizerPtr& symbolizer, int indent)
{
    std::string sld = GenerateIndent(indent) + "<TextSymbolizer>\n";
    sld += GenerateIndent(indent + 1) + "<Label>" + symbolizer->GetLabel() + "</Label>\n";
    sld += GenerateIndent(indent + 1) + "<Font>\n";
    sld += GenerateIndent(indent + 2) + "<CssParameter name=\"font-family\">" + 
           symbolizer->GetFont().GetFamily() + "</CssParameter>\n";
    sld += GenerateIndent(indent + 2) + "<CssParameter name=\"font-size\">" + 
           std::to_string(symbolizer->GetFont().GetSize()) + "</CssParameter>\n";
    sld += GenerateIndent(indent + 1) + "</Font>\n";
    sld += GenerateIndent(indent + 1) + "<Fill>\n";
    sld += GenerateIndent(indent + 2) + "<CssParameter name=\"fill\">" + 
           ColorToString(symbolizer->GetColor()) + "</CssParameter>\n";
    sld += GenerateIndent(indent + 1) + "</Fill>\n";
    sld += GenerateIndent(indent) + "</TextSymbolizer>\n";
    return sld;
}

std::string SldParser::GenerateRasterSymbolizer(const RasterSymbolizerPtr& symbolizer, int indent)
{
    std::string sld = GenerateIndent(indent) + "<RasterSymbolizer>\n";
    sld += GenerateIndent(indent + 1) + "<Opacity>" + 
           DoubleToString(symbolizer->GetOpacity()) + "</Opacity>\n";
    sld += GenerateIndent(indent) + "</RasterSymbolizer>\n";
    return sld;
}

std::string SldParser::GenerateFilter(const FilterPtr& filter, int indent)
{
    if (!filter) {
        return "";
    }
    
    FilterType type = filter->GetType();
    switch (type) {
        case FilterType::kComparison:
            return GenerateComparisonFilter(
                std::static_pointer_cast<ComparisonFilter>(filter), indent);
        case FilterType::kLogical:
            return GenerateLogicalFilter(
                std::static_pointer_cast<LogicalFilter>(filter), indent);
        case FilterType::kSpatial:
            return GenerateSpatialFilter(
                std::static_pointer_cast<SpatialFilter>(filter), indent);
        default:
            return "";
    }
}

std::string SldParser::GenerateComparisonFilter(const ComparisonFilterPtr& filter, int indent)
{
    std::string tagName;
    switch (filter->GetOperator()) {
        case ComparisonOperator::kEqual:
            tagName = "PropertyIsEqualTo";
            break;
        case ComparisonOperator::kNotEqual:
            tagName = "PropertyIsNotEqualTo";
            break;
        case ComparisonOperator::kLessThan:
            tagName = "PropertyIsLessThan";
            break;
        case ComparisonOperator::kGreaterThan:
            tagName = "PropertyIsGreaterThan";
            break;
        case ComparisonOperator::kLessThanOrEqual:
            tagName = "PropertyIsLessThanOrEqualTo";
            break;
        case ComparisonOperator::kGreaterThanOrEqual:
            tagName = "PropertyIsGreaterThanOrEqualTo";
            break;
        default:
            return "";
    }
    
    std::string sld = GenerateIndent(indent) + "<ogc:" + tagName + ">\n";
    sld += GenerateIndent(indent + 1) + "<ogc:PropertyName>" + 
           filter->GetPropertyName() + "</ogc:PropertyName>\n";
    sld += GenerateIndent(indent + 1) + "<ogc:Literal>" + 
           filter->GetLiteral() + "</ogc:Literal>\n";
    sld += GenerateIndent(indent) + "</ogc:" + tagName + ">\n";
    return sld;
}

std::string SldParser::GenerateLogicalFilter(const LogicalFilterPtr& filter, int indent)
{
    std::string tagName;
    switch (filter->GetOperator()) {
        case LogicalOperator::kAnd:
            tagName = "And";
            break;
        case LogicalOperator::kOr:
            tagName = "Or";
            break;
        case LogicalOperator::kNot:
            tagName = "Not";
            break;
        default:
            return "";
    }
    
    std::string sld = GenerateIndent(indent) + "<ogc:" + tagName + ">\n";
    
    for (const auto& child : filter->GetFilters()) {
        sld += GenerateFilter(child, indent + 1);
    }
    
    sld += GenerateIndent(indent) + "</ogc:" + tagName + ">\n";
    return sld;
}

std::string SldParser::GenerateSpatialFilter(const SpatialFilterPtr& filter, int indent)
{
    std::string tagName;
    switch (filter->GetOperator()) {
        case SpatialOperator::kBbox:
            tagName = "BBOX";
            break;
        case SpatialOperator::kIntersects:
            tagName = "Intersects";
            break;
        case SpatialOperator::kWithin:
            tagName = "Within";
            break;
        case SpatialOperator::kContains:
            tagName = "Contains";
            break;
        default:
            return "";
    }
    
    std::string sld = GenerateIndent(indent) + "<ogc:" + tagName + ">\n";
    sld += GenerateIndent(indent + 1) + "<ogc:PropertyName>" + 
           filter->GetPropertyName() + "</ogc:PropertyName>\n";
    sld += GenerateIndent(indent) + "</ogc:" + tagName + ">\n";
    return sld;
}

}
}
