#include "ogc/draw/sld_parser.h"
#include "ogc/draw/point_symbolizer.h"
#include "ogc/draw/line_symbolizer.h"
#include "ogc/draw/polygon_symbolizer.h"
#include "ogc/draw/text_symbolizer.h"
#include "ogc/draw/raster_symbolizer.h"
#include "ogc/draw/comparison_filter.h"
#include "ogc/draw/logical_filter.h"
#include "ogc/draw/spatial_filter.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cstdlib>

namespace ogc {
namespace draw {

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
                    // Handle fill opacity
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
            if (params.find("fill-opacity") != params.end()) {
                symbolizer->SetOpacity(ParseDouble(params["fill-opacity"], 1.0));
            }
        }
    }
    
    std::string haloContent = ReadElementContent("Halo");
    if (!haloContent.empty()) {
        std::string radius = ReadElementContent("Radius");
        if (!radius.empty()) {
            // symbolizer->SetHaloRadius(ParseDouble(radius, 1.0));
        }
    }
    
    std::string placement = ReadElementContent("LabelPlacement");
    if (!placement.empty()) {
        std::string pointPlacement = ReadElementContent("PointPlacement");
        std::string linePlacement = ReadElementContent("LinePlacement");
        
        if (!pointPlacement.empty()) {
            symbolizer->SetPlacement(TextPlacement::kPoint);
        } else if (!linePlacement.empty()) {
            symbolizer->SetPlacement(TextPlacement::kLine);
            symbolizer->SetFollowLine(true);
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
    
    std::string channelSelection = ReadElementContent("ChannelSelection");
    if (!channelSelection.empty()) {
        symbolizer->SetChannelSelection(RasterChannelSelection::kRGB);
    }
    
    std::string contrastEnhancement = ReadElementContent("ContrastEnhancement");
    if (!contrastEnhancement.empty()) {
        symbolizer->SetContrastEnhancement(true);
        
        std::string gamma = ReadElementContent("GammaValue");
        if (!gamma.empty()) {
            symbolizer->SetGammaValue(ParseDouble(gamma, 1.0));
        }
    }
    
    m_currentPos = symEnd + 19;
    return true;
}

bool SldParser::ParseFilter(FilterPtr& filter)
{
    size_t filterEnd = m_content.find("</Filter>", m_currentPos);
    if (filterEnd == std::string::npos) {
        return false;
    }
    
    size_t filterStart = m_content.find("<Filter", m_currentPos);
    if (filterStart == std::string::npos) {
        return false;
    }
    
    size_t contentStart = m_content.find(">", filterStart) + 1;
    m_currentPos = contentStart;
    
    if (m_content.find("<PropertyIsEqualTo", m_currentPos) < filterEnd ||
        m_content.find("<PropertyIsNotEqualTo", m_currentPos) < filterEnd ||
        m_content.find("<PropertyIsLessThan", m_currentPos) < filterEnd ||
        m_content.find("<PropertyIsGreaterThan", m_currentPos) < filterEnd ||
        m_content.find("<PropertyIsLessThanOrEqualTo", m_currentPos) < filterEnd ||
        m_content.find("<PropertyIsGreaterThanOrEqualTo", m_currentPos) < filterEnd) {
        return ParseComparisonFilter(filter);
    }
    
    if (m_content.find("<PropertyIsBetween", m_currentPos) < filterEnd) {
        return ParsePropertyIsBetween(filter);
    }
    
    if (m_content.find("<PropertyIsLike", m_currentPos) < filterEnd) {
        return ParsePropertyIsLike(filter);
    }
    
    if (m_content.find("<PropertyIsNull", m_currentPos) < filterEnd) {
        return ParsePropertyIsNull(filter);
    }
    
    if (m_content.find("<And", m_currentPos) < filterEnd ||
        m_content.find("<Or", m_currentPos) < filterEnd ||
        m_content.find("<Not", m_currentPos) < filterEnd) {
        return ParseLogicalFilter(filter);
    }
    
    if (m_content.find("<BBOX", m_currentPos) < filterEnd ||
        m_content.find("<Intersects", m_currentPos) < filterEnd ||
        m_content.find("<Within", m_currentPos) < filterEnd ||
        m_content.find("<Contains", m_currentPos) < filterEnd) {
        return ParseSpatialFilter(filter);
    }
    
    m_currentPos = filterEnd + 9;
    return false;
}

bool SldParser::ParseComparisonFilter(FilterPtr& filter)
{
    std::string opName;
    ComparisonOperator op = ComparisonOperator::kEqual;
    
    size_t opStart = std::string::npos;
    
    if ((opStart = m_content.find("<PropertyIsEqualTo", m_currentPos)) != std::string::npos) {
        op = ComparisonOperator::kEqual;
        opName = "PropertyIsEqualTo";
    } else if ((opStart = m_content.find("<PropertyIsNotEqualTo", m_currentPos)) != std::string::npos) {
        op = ComparisonOperator::kNotEqual;
        opName = "PropertyIsNotEqualTo";
    } else if ((opStart = m_content.find("<PropertyIsLessThan", m_currentPos)) != std::string::npos) {
        op = ComparisonOperator::kLessThan;
        opName = "PropertyIsLessThan";
    } else if ((opStart = m_content.find("<PropertyIsGreaterThan", m_currentPos)) != std::string::npos) {
        op = ComparisonOperator::kGreaterThan;
        opName = "PropertyIsGreaterThan";
    } else if ((opStart = m_content.find("<PropertyIsLessThanOrEqualTo", m_currentPos)) != std::string::npos) {
        op = ComparisonOperator::kLessThanOrEqual;
        opName = "PropertyIsLessThanOrEqualTo";
    } else if ((opStart = m_content.find("<PropertyIsGreaterThanOrEqualTo", m_currentPos)) != std::string::npos) {
        op = ComparisonOperator::kGreaterThanOrEqual;
        opName = "PropertyIsGreaterThanOrEqualTo";
    }
    
    if (opStart == std::string::npos) {
        return false;
    }
    
    m_currentPos = opStart;
    
    std::string propertyName = ReadElementContent("PropertyName");
    std::string literal = ReadElementContent("Literal");
    
    filter = std::make_shared<ComparisonFilter>(op, propertyName, literal);
    
    SkipToEndElement(opName);
    return true;
}

bool SldParser::ParseLogicalFilter(FilterPtr& filter)
{
    size_t andPos = m_content.find("<And", m_currentPos);
    size_t orPos = m_content.find("<Or", m_currentPos);
    size_t notPos = m_content.find("<Not", m_currentPos);
    
    LogicalOperator op;
    std::string tagName;
    size_t opPos = std::string::npos;
    
    if (andPos != std::string::npos && (orPos == std::string::npos || andPos < orPos) &&
        (notPos == std::string::npos || andPos < notPos)) {
        op = LogicalOperator::kAnd;
        tagName = "And";
        opPos = andPos;
    } else if (orPos != std::string::npos && (notPos == std::string::npos || orPos < notPos)) {
        op = LogicalOperator::kOr;
        tagName = "Or";
        opPos = orPos;
    } else if (notPos != std::string::npos) {
        op = LogicalOperator::kNot;
        tagName = "Not";
        opPos = notPos;
    }
    
    if (opPos == std::string::npos) {
        return false;
    }
    
    m_currentPos = opPos;
    
    LogicalFilterPtr logicalFilter = std::make_shared<LogicalFilter>(op);
    
    size_t endTag = m_content.find("</" + tagName + ">", m_currentPos);
    if (endTag == std::string::npos) {
        return false;
    }
    
    while (m_currentPos < endTag) {
        FilterPtr subFilter;
        if (ParseFilter(subFilter)) {
            logicalFilter->AddFilter(subFilter);
        } else {
            break;
        }
    }
    
    filter = logicalFilter;
    m_currentPos = endTag + tagName.length() + 3;
    return true;
}

bool SldParser::ParseSpatialFilter(FilterPtr& filter)
{
    SpatialOperator op = SpatialOperator::kBbox;
    std::string tagName;
    size_t opPos = std::string::npos;
    
    if ((opPos = m_content.find("<BBOX", m_currentPos)) != std::string::npos) {
        op = SpatialOperator::kBbox;
        tagName = "BBOX";
    } else if ((opPos = m_content.find("<Intersects", m_currentPos)) != std::string::npos) {
        op = SpatialOperator::kIntersects;
        tagName = "Intersects";
    } else if ((opPos = m_content.find("<Within", m_currentPos)) != std::string::npos) {
        op = SpatialOperator::kWithin;
        tagName = "Within";
    } else if ((opPos = m_content.find("<Contains", m_currentPos)) != std::string::npos) {
        op = SpatialOperator::kContains;
        tagName = "Contains";
    }
    
    if (opPos == std::string::npos) {
        return false;
    }
    
    m_currentPos = opPos;
    
    std::string propertyName = ReadElementContent("PropertyName");
    std::string envelopeContent = ReadElementContent("Envelope");
    
    if (!envelopeContent.empty()) {
        std::string lowerCorner = ReadElementContent("lowerCorner");
        std::string upperCorner = ReadElementContent("upperCorner");
        
        double minX = 0, minY = 0, maxX = 0, maxY = 0;
        
        std::stringstream ss1(lowerCorner);
        ss1 >> minX >> minY;
        
        std::stringstream ss2(upperCorner);
        ss2 >> maxX >> maxY;
        
        Envelope env(minX, minY, maxX, maxY);
        filter = std::make_shared<SpatialFilter>(op, env);
    }
    
    SkipToEndElement(tagName);
    return filter != nullptr;
}

bool SldParser::ParsePropertyIsBetween(FilterPtr& filter)
{
    size_t startPos = m_content.find("<PropertyIsBetween", m_currentPos);
    if (startPos == std::string::npos) {
        return false;
    }
    
    m_currentPos = startPos;
    
    std::string propertyName = ReadElementContent("PropertyName");
    std::string lowerBoundary = ReadElementContent("LowerBoundary");
    std::string upperBoundary = ReadElementContent("UpperBoundary");
    
    std::string lowerLiteral = ReadElementContent("Literal");
    std::string upperLiteral = ReadElementContent("Literal");
    
    filter = std::make_shared<ComparisonFilter>(ComparisonOperator::kBetween, 
                                                  propertyName, lowerLiteral, upperLiteral);
    
    SkipToEndElement("PropertyIsBetween");
    return true;
}

bool SldParser::ParsePropertyIsLike(FilterPtr& filter)
{
    size_t startPos = m_content.find("<PropertyIsLike", m_currentPos);
    if (startPos == std::string::npos) {
        return false;
    }
    
    m_currentPos = startPos;
    
    std::string propertyName = ReadElementContent("PropertyName");
    std::string literal = ReadElementContent("Literal");
    
    filter = std::make_shared<ComparisonFilter>(ComparisonOperator::kLike, propertyName, literal);
    
    SkipToEndElement("PropertyIsLike");
    return true;
}

bool SldParser::ParsePropertyIsNull(FilterPtr& filter)
{
    size_t startPos = m_content.find("<PropertyIsNull", m_currentPos);
    if (startPos == std::string::npos) {
        return false;
    }
    
    m_currentPos = startPos;
    
    std::string propertyName = ReadElementContent("PropertyName");
    
    filter = std::make_shared<ComparisonFilter>(ComparisonOperator::kIsNull, propertyName, "");
    
    SkipToEndElement("PropertyIsNull");
    return true;
}

bool SldParser::ParseParameter(std::map<std::string, std::string>& params, const std::string& tagName)
{
    size_t searchPos = m_currentPos;
    
    while (true) {
        size_t paramStart = m_content.find("<" + tagName, searchPos);
        if (paramStart == std::string::npos) {
            break;
        }
        
        size_t paramEnd = m_content.find("</" + tagName + ">", paramStart);
        if (paramEnd == std::string::npos) {
            break;
        }
        
        size_t nameAttrStart = m_content.find("name=\"", paramStart);
        if (nameAttrStart != std::string::npos && nameAttrStart < paramEnd) {
            nameAttrStart += 6;
            size_t nameAttrEnd = m_content.find("\"", nameAttrStart);
            std::string name = m_content.substr(nameAttrStart, nameAttrEnd - nameAttrStart);
            
            size_t contentStart = m_content.find(">", paramStart) + 1;
            std::string value = m_content.substr(contentStart, paramEnd - contentStart);
            
            params[Trim(name)] = Trim(value);
        }
        
        searchPos = paramEnd + tagName.length() + 3;
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
    contentStart++;
    
    size_t end = m_content.find("</" + tagName + ">", contentStart);
    if (end == std::string::npos) {
        return "";
    }
    
    return m_content.substr(contentStart, end - contentStart);
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
    size_t start = m_content.find("<" + tagName, m_currentPos);
    if (start == std::string::npos) {
        return "";
    }
    
    size_t end = m_content.find(">", start);
    if (end == std::string::npos) {
        return "";
    }
    
    std::string tagContent = m_content.substr(start, end - start);
    
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
    size_t colonPos = fullName.find(':');
    if (colonPos != std::string::npos) {
        return fullName.substr(colonPos + 1);
    }
    return fullName;
}

uint32_t SldParser::ParseColor(const std::string& colorStr)
{
    std::string color = Trim(colorStr);
    
    if (color.empty()) {
        return 0xFF000000;
    }
    
    if (color[0] == '#') {
        color = color.substr(1);
    }
    
    if (color.length() == 6) {
        uint32_t rgb = static_cast<uint32_t>(std::strtoul(color.c_str(), nullptr, 16));
        return 0xFF000000 | rgb;
    } else if (color.length() == 8) {
        uint32_t argb = static_cast<uint32_t>(std::strtoul(color.c_str(), nullptr, 16));
        return argb;
    }
    
    return 0xFF000000;
}

double SldParser::ParseDouble(const std::string& str, double defaultVal)
{
    std::string trimmed = Trim(str);
    if (trimmed.empty()) {
        return defaultVal;
    }
    
    try {
        return std::stod(trimmed);
    } catch (...) {
        return defaultVal;
    }
}

int SldParser::ParseInt(const std::string& str, int defaultVal)
{
    std::string trimmed = Trim(str);
    if (trimmed.empty()) {
        return defaultVal;
    }
    
    try {
        return std::stoi(trimmed);
    } catch (...) {
        return defaultVal;
    }
}

std::string SldParser::ColorToString(uint32_t color)
{
    char buf[16];
    snprintf(buf, sizeof(buf), "#%06X", color & 0x00FFFFFF);
    return std::string(buf);
}

std::string SldParser::DoubleToString(double val)
{
    std::ostringstream oss;
    oss << val;
    return oss.str();
}

void SldParser::SetError(const std::string& message)
{
    m_lastError = message;
}

void SldParser::ClearError()
{
    m_lastError.clear();
}

std::string SldParser::GenerateIndent(int level)
{
    return std::string(level * 2, ' ');
}

std::string SldParser::GenerateSld(const std::vector<SymbolizerRulePtr>& rules)
{
    std::ostringstream oss;
    
    oss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    oss << "<StyledLayerDescriptor version=\"1.0.0\" \n";
    oss << "    xsi:schemaLocation=\"http://www.opengis.net/sld StyledLayerDescriptor.xsd\" \n";
    oss << "    xmlns=\"http://www.opengis.net/sld\" \n";
    oss << "    xmlns:ogc=\"http://www.opengis.net/ogc\" \n";
    oss << "    xmlns:xlink=\"http://www.w3.org/1999/xlink\" \n";
    oss << "    xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n";
    
    oss << "  <NamedLayer>\n";
    oss << "    <Name>DefaultLayer</Name>\n";
    oss << "    <UserStyle>\n";
    oss << "      <FeatureTypeStyle>\n";
    
    for (const auto& rule : rules) {
        oss << GenerateSld(rule);
    }
    
    oss << "      </FeatureTypeStyle>\n";
    oss << "    </UserStyle>\n";
    oss << "  </NamedLayer>\n";
    oss << "</StyledLayerDescriptor>\n";
    
    return oss.str();
}

std::string SldParser::GenerateSld(const SymbolizerRulePtr& rule)
{
    std::ostringstream oss;
    
    oss << "        <Rule>\n";
    
    if (!rule->GetName().empty()) {
        oss << "          <Name>" << rule->GetName() << "</Name>\n";
    }
    
    if (!rule->GetTitle().empty()) {
        oss << "          <Title>" << rule->GetTitle() << "</Title>\n";
    }
    
    if (rule->GetMinScaleDenominator() > 0) {
        oss << "          <MinScaleDenominator>" << rule->GetMinScaleDenominator() 
            << "</MinScaleDenominator>\n";
    }
    
    if (rule->GetMaxScaleDenominator() < std::numeric_limits<double>::max()) {
        oss << "          <MaxScaleDenominator>" << rule->GetMaxScaleDenominator() 
            << "</MaxScaleDenominator>\n";
    }
    
    if (rule->HasFilter()) {
        oss << GenerateFilter(rule->GetFilter(), 10);
    }
    
    for (const auto& symbolizer : rule->GetSymbolizers()) {
        switch (symbolizer->GetType()) {
            case SymbolizerType::kPoint:
                oss << GeneratePointSymbolizer(
                    std::dynamic_pointer_cast<PointSymbolizer>(symbolizer), 10);
                break;
            case SymbolizerType::kLine:
                oss << GenerateLineSymbolizer(
                    std::dynamic_pointer_cast<LineSymbolizer>(symbolizer), 10);
                break;
            case SymbolizerType::kPolygon:
                oss << GeneratePolygonSymbolizer(
                    std::dynamic_pointer_cast<PolygonSymbolizer>(symbolizer), 10);
                break;
            case SymbolizerType::kText:
                oss << GenerateTextSymbolizer(
                    std::dynamic_pointer_cast<TextSymbolizer>(symbolizer), 10);
                break;
            case SymbolizerType::kRaster:
                oss << GenerateRasterSymbolizer(
                    std::dynamic_pointer_cast<RasterSymbolizer>(symbolizer), 10);
                break;
            default:
                break;
        }
    }
    
    oss << "        </Rule>\n";
    
    return oss.str();
}

std::string SldParser::GeneratePointSymbolizer(const PointSymbolizerPtr& symbolizer, int indent)
{
    std::ostringstream oss;
    std::string ind = GenerateIndent(indent);
    
    oss << ind << "<PointSymbolizer>\n";
    oss << ind << "  <Graphic>\n";
    oss << ind << "    <Mark>\n";
    oss << ind << "      <WellKnownName>circle</WellKnownName>\n";
    oss << ind << "      <Fill>\n";
    oss << ind << "        <CssParameter name=\"fill\">" 
        << ColorToString(symbolizer->GetColor()) << "</CssParameter>\n";
    oss << ind << "      </Fill>\n";
    oss << ind << "      <Stroke>\n";
    oss << ind << "        <CssParameter name=\"stroke\">" 
        << ColorToString(symbolizer->GetStrokeColor()) << "</CssParameter>\n";
    oss << ind << "        <CssParameter name=\"stroke-width\">" 
        << symbolizer->GetStrokeWidth() << "</CssParameter>\n";
    oss << ind << "      </Stroke>\n";
    oss << ind << "    </Mark>\n";
    oss << ind << "    <Size>" << symbolizer->GetSize() << "</Size>\n";
    oss << ind << "  </Graphic>\n";
    oss << ind << "</PointSymbolizer>\n";
    
    return oss.str();
}

std::string SldParser::GenerateLineSymbolizer(const LineSymbolizerPtr& symbolizer, int indent)
{
    std::ostringstream oss;
    std::string ind = GenerateIndent(indent);
    
    oss << ind << "<LineSymbolizer>\n";
    oss << ind << "  <Stroke>\n";
    oss << ind << "    <CssParameter name=\"stroke\">" 
        << ColorToString(symbolizer->GetColor()) << "</CssParameter>\n";
    oss << ind << "    <CssParameter name=\"stroke-width\">" 
        << symbolizer->GetWidth() << "</CssParameter>\n";
    if (symbolizer->GetOpacity() < 1.0) {
        oss << ind << "    <CssParameter name=\"stroke-opacity\">" 
            << symbolizer->GetOpacity() << "</CssParameter>\n";
    }
    oss << ind << "  </Stroke>\n";
    oss << ind << "</LineSymbolizer>\n";
    
    return oss.str();
}

std::string SldParser::GeneratePolygonSymbolizer(const PolygonSymbolizerPtr& symbolizer, int indent)
{
    std::ostringstream oss;
    std::string ind = GenerateIndent(indent);
    
    oss << ind << "<PolygonSymbolizer>\n";
    oss << ind << "  <Fill>\n";
    oss << ind << "    <CssParameter name=\"fill\">" 
        << ColorToString(symbolizer->GetFillColor()) << "</CssParameter>\n";
    if (symbolizer->GetFillOpacity() < 1.0) {
        oss << ind << "    <CssParameter name=\"fill-opacity\">" 
            << symbolizer->GetFillOpacity() << "</CssParameter>\n";
    }
    oss << ind << "  </Fill>\n";
    oss << ind << "  <Stroke>\n";
    oss << ind << "    <CssParameter name=\"stroke\">" 
        << ColorToString(symbolizer->GetStrokeColor()) << "</CssParameter>\n";
    oss << ind << "    <CssParameter name=\"stroke-width\">" 
        << symbolizer->GetStrokeWidth() << "</CssParameter>\n";
    oss << ind << "  </Stroke>\n";
    oss << ind << "</PolygonSymbolizer>\n";
    
    return oss.str();
}

std::string SldParser::GenerateTextSymbolizer(const TextSymbolizerPtr& symbolizer, int indent)
{
    std::ostringstream oss;
    std::string ind = GenerateIndent(indent);
    
    oss << ind << "<TextSymbolizer>\n";
    
    if (!symbolizer->GetLabelProperty().empty()) {
        oss << ind << "  <Label>\n";
        oss << ind << "    <ogc:PropertyName>" << symbolizer->GetLabelProperty() 
            << "</ogc:PropertyName>\n";
        oss << ind << "  </Label>\n";
    } else if (!symbolizer->GetLabel().empty()) {
        oss << ind << "  <Label>" << symbolizer->GetLabel() << "</Label>\n";
    }
    
    Font font = symbolizer->GetFont();
    oss << ind << "  <Font>\n";
    oss << ind << "    <CssParameter name=\"font-family\">" << font.GetFamily() 
        << "</CssParameter>\n";
    oss << ind << "    <CssParameter name=\"font-size\">" << font.GetSize() 
        << "</CssParameter>\n";
    if (font.IsBold()) {
        oss << ind << "    <CssParameter name=\"font-weight\">bold</CssParameter>\n";
    }
    if (font.IsItalic()) {
        oss << ind << "    <CssParameter name=\"font-style\">italic</CssParameter>\n";
    }
    oss << ind << "  </Font>\n";
    
    oss << ind << "  <Fill>\n";
    oss << ind << "    <CssParameter name=\"fill\">" 
        << ColorToString(symbolizer->GetColor()) << "</CssParameter>\n";
    oss << ind << "  </Fill>\n";
    
    oss << ind << "</TextSymbolizer>\n";
    
    return oss.str();
}

std::string SldParser::GenerateRasterSymbolizer(const RasterSymbolizerPtr& symbolizer, int indent)
{
    std::ostringstream oss;
    std::string ind = GenerateIndent(indent);
    
    oss << ind << "<RasterSymbolizer>\n";
    oss << ind << "  <Opacity>" << symbolizer->GetOpacity() << "</Opacity>\n";
    oss << ind << "</RasterSymbolizer>\n";
    
    return oss.str();
}

std::string SldParser::GenerateFilter(const FilterPtr& filter, int indent)
{
    if (!filter) {
        return "";
    }
    
    switch (filter->GetType()) {
        case FilterType::kComparison:
            return GenerateComparisonFilter(
                std::dynamic_pointer_cast<ComparisonFilter>(filter), indent);
        case FilterType::kLogical:
            return GenerateLogicalFilter(
                std::dynamic_pointer_cast<LogicalFilter>(filter), indent);
        case FilterType::kSpatial:
            return GenerateSpatialFilter(
                std::dynamic_pointer_cast<SpatialFilter>(filter), indent);
        default:
            return "";
    }
}

std::string SldParser::GenerateComparisonFilter(const ComparisonFilterPtr& filter, int indent)
{
    std::ostringstream oss;
    std::string ind = GenerateIndent(indent);
    
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
        case ComparisonOperator::kLike:
            tagName = "PropertyIsLike";
            break;
        case ComparisonOperator::kIsNull:
            tagName = "PropertyIsNull";
            break;
        case ComparisonOperator::kBetween:
            tagName = "PropertyIsBetween";
            break;
        default:
            tagName = "PropertyIsEqualTo";
            break;
    }
    
    oss << ind << "<ogc:Filter>\n";
    oss << ind << "  <ogc:" << tagName << ">\n";
    oss << ind << "    <ogc:PropertyName>" << filter->GetPropertyName() 
        << "</ogc:PropertyName>\n";
    
    if (filter->GetOperator() != ComparisonOperator::kIsNull) {
        oss << ind << "    <ogc:Literal>" << filter->GetLiteral() 
            << "</ogc:Literal>\n";
    }
    
    oss << ind << "  </ogc:" << tagName << ">\n";
    oss << ind << "</ogc:Filter>\n";
    
    return oss.str();
}

std::string SldParser::GenerateLogicalFilter(const LogicalFilterPtr& filter, int indent)
{
    std::ostringstream oss;
    std::string ind = GenerateIndent(indent);
    
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
            tagName = "And";
            break;
    }
    
    oss << ind << "<ogc:Filter>\n";
    oss << ind << "  <ogc:" << tagName << ">\n";
    
    for (const auto& subFilter : filter->GetFilters()) {
        std::string subFilterStr = GenerateFilter(subFilter, indent + 4);
        // Remove outer Filter tags for nested filters
        size_t start = subFilterStr.find("<ogc:Filter>");
        size_t end = subFilterStr.find("</ogc:Filter>");
        if (start != std::string::npos && end != std::string::npos) {
            start += 12;
            oss << subFilterStr.substr(start, end - start);
        } else {
            oss << subFilterStr;
        }
    }
    
    oss << ind << "  </ogc:" << tagName << ">\n";
    oss << ind << "</ogc:Filter>\n";
    
    return oss.str();
}

std::string SldParser::GenerateSpatialFilter(const SpatialFilterPtr& filter, int indent)
{
    std::ostringstream oss;
    std::string ind = GenerateIndent(indent);
    
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
            tagName = "BBOX";
            break;
    }
    
    oss << ind << "<ogc:Filter>\n";
    oss << ind << "  <ogc:" << tagName << ">\n";
    
    if (filter->HasEnvelope()) {
        const Envelope& env = filter->GetEnvelope();
        oss << ind << "    <ogc:PropertyName>geometry</ogc:PropertyName>\n";
        oss << ind << "    <gml:Envelope>\n";
        oss << ind << "      <gml:lowerCorner>" << env.GetMinX() << " " << env.GetMinY() 
            << "</gml:lowerCorner>\n";
        oss << ind << "      <gml:upperCorner>" << env.GetMaxX() << " " << env.GetMaxY() 
            << "</gml:upperCorner>\n";
        oss << ind << "    </gml:Envelope>\n";
    }
    
    oss << ind << "  </ogc:" << tagName << ">\n";
    oss << ind << "</ogc:Filter>\n";
    
    return oss.str();
}

}
}
