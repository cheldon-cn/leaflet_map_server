#include "ogc/draw/mapbox_style_parser.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cstdlib>

namespace ogc {
namespace draw {

MapboxStyleParser::MapboxStyleParser()
    : m_strictMode(false)
    , m_currentPos(0)
{
}

MapboxStyleParserPtr MapboxStyleParser::Create()
{
    return std::make_shared<MapboxStyleParser>();
}

MapboxStyleResult MapboxStyleParser::Parse(const std::string& jsonContent)
{
    MapboxStyleResult result;
    ClearError();
    
    m_content = jsonContent;
    m_currentPos = 0;
    m_style = MapboxStyle();
    
    if (jsonContent.empty()) {
        SetError("JSON content is empty");
        result.errorMessage = m_lastError;
        return result;
    }
    
    SkipWhitespace();
    
    if (m_currentPos >= m_content.length() || m_content[m_currentPos] != '{') {
        SetError("Invalid JSON: expected '{' at start");
        result.errorMessage = m_lastError;
        return result;
    }
    
    if (!ParseStyleObject()) {
        result.errorMessage = m_lastError;
        return result;
    }
    
    result.success = true;
    result.rules = GetRules();
    return result;
}

MapboxStyleResult MapboxStyleParser::ParseFile(const std::string& filePath)
{
    MapboxStyleResult result;
    
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

std::vector<SymbolizerRulePtr> MapboxStyleParser::GetRules() const
{
    std::vector<SymbolizerRulePtr> rules;
    
    for (const auto& layer : m_style.layers) {
        SymbolizerRulePtr rule = ConvertLayerToRule(layer);
        if (rule) {
            rules.push_back(rule);
        }
    }
    
    return rules;
}

std::vector<SymbolizerRulePtr> MapboxStyleParser::ParseStyle(const std::string& jsonContent)
{
    MapboxStyleParser parser;
    auto result = parser.Parse(jsonContent);
    return result.rules;
}

std::vector<SymbolizerRulePtr> MapboxStyleParser::ParseStyleFile(const std::string& filePath)
{
    MapboxStyleParser parser;
    auto result = parser.ParseFile(filePath);
    return result.rules;
}

bool MapboxStyleParser::ParseStyleObject()
{
    if (m_content[m_currentPos] != '{') {
        SetError("Expected '{'");
        return false;
    }
    
    m_currentPos++;
    SkipWhitespace();
    
    while (m_currentPos < m_content.length() && m_content[m_currentPos] != '}') {
        SkipWhitespace();
        
        std::string key = ReadString();
        if (key.empty()) {
            if (m_strictMode) {
                SetError("Empty key in style object");
                return false;
            }
            m_currentPos++;
            continue;
        }
        
        SkipWhitespace();
        if (m_currentPos >= m_content.length() || m_content[m_currentPos] != ':') {
            SetError("Expected ':' after key");
            return false;
        }
        m_currentPos++;
        SkipWhitespace();
        
        if (key == "version") {
            m_style.version = ReadString();
        } else if (key == "name") {
            m_style.name = ReadString();
        } else if (key == "sources") {
            if (!ParseSources()) {
                if (m_strictMode) {
                    return false;
                }
            }
        } else if (key == "layers") {
            if (!ParseLayers()) {
                if (m_strictMode) {
                    return false;
                }
            }
        } else if (key == "sprite") {
            std::string sprite = ReadString();
            m_style.sprites["default"] = sprite;
        } else if (key == "glyphs") {
            std::string glyphs = ReadString();
            m_style.glyphs["default"] = glyphs;
        } else {
            SkipWhitespace();
            if (m_currentPos < m_content.length()) {
                if (m_content[m_currentPos] == '{') {
                    int depth = 1;
                    m_currentPos++;
                    while (m_currentPos < m_content.length() && depth > 0) {
                        if (m_content[m_currentPos] == '{') depth++;
                        else if (m_content[m_currentPos] == '}') depth--;
                        m_currentPos++;
                    }
                } else if (m_content[m_currentPos] == '[') {
                    int depth = 1;
                    m_currentPos++;
                    while (m_currentPos < m_content.length() && depth > 0) {
                        if (m_content[m_currentPos] == '[') depth++;
                        else if (m_content[m_currentPos] == ']') depth--;
                        m_currentPos++;
                    }
                } else if (m_content[m_currentPos] == '"') {
                    ReadString();
                } else {
                    while (m_currentPos < m_content.length() && 
                           m_content[m_currentPos] != ',' && 
                           m_content[m_currentPos] != '}') {
                        m_currentPos++;
                    }
                }
            }
        }
        
        SkipWhitespace();
        if (m_currentPos < m_content.length() && m_content[m_currentPos] == ',') {
            m_currentPos++;
        }
    }
    
    if (m_currentPos >= m_content.length() || m_content[m_currentPos] != '}') {
        SetError("Expected '}' at end of style object");
        return false;
    }
    
    m_currentPos++;
    return true;
}

bool MapboxStyleParser::ParseSources()
{
    if (m_content[m_currentPos] != '{') {
        SetError("Expected '{' for sources");
        return false;
    }
    
    m_currentPos++;
    SkipWhitespace();
    
    while (m_currentPos < m_content.length() && m_content[m_currentPos] != '}') {
        SkipWhitespace();
        
        std::string sourceName = ReadString();
        if (sourceName.empty()) {
            break;
        }
        
        SkipWhitespace();
        if (m_currentPos >= m_content.length() || m_content[m_currentPos] != ':') {
            break;
        }
        m_currentPos++;
        SkipWhitespace();
        
        MapboxSource source;
        
        if (m_content[m_currentPos] == '{') {
            m_currentPos++;
            SkipWhitespace();
            
            while (m_currentPos < m_content.length() && m_content[m_currentPos] != '}') {
                SkipWhitespace();
                std::string key = ReadString();
                
                SkipWhitespace();
                if (m_currentPos < m_content.length() && m_content[m_currentPos] == ':') {
                    m_currentPos++;
                    SkipWhitespace();
                    
                    if (key == "type") {
                        source.type = ReadString();
                    } else if (key == "url") {
                        source.url = ReadString();
                    } else if (key == "minzoom") {
                        source.minZoom = static_cast<int>(ReadNumber());
                    } else if (key == "maxzoom") {
                        source.maxZoom = static_cast<int>(ReadNumber());
                    } else if (key == "tiles") {
                        if (m_content[m_currentPos] == '[') {
                            m_currentPos++;
                            SkipWhitespace();
                            while (m_currentPos < m_content.length() && m_content[m_currentPos] != ']') {
                                std::string tile = ReadString();
                                if (!tile.empty()) {
                                    source.tiles[tile] = tile;
                                }
                                SkipWhitespace();
                                if (m_currentPos < m_content.length() && m_content[m_currentPos] == ',') {
                                    m_currentPos++;
                                }
                            }
                            if (m_currentPos < m_content.length() && m_content[m_currentPos] == ']') {
                                m_currentPos++;
                            }
                        }
                    } else {
                        if (m_content[m_currentPos] == '{') {
                            int depth = 1;
                            m_currentPos++;
                            while (m_currentPos < m_content.length() && depth > 0) {
                                if (m_content[m_currentPos] == '{') depth++;
                                else if (m_content[m_currentPos] == '}') depth--;
                                m_currentPos++;
                            }
                        } else if (m_content[m_currentPos] == '[') {
                            int depth = 1;
                            m_currentPos++;
                            while (m_currentPos < m_content.length() && depth > 0) {
                                if (m_content[m_currentPos] == '[') depth++;
                                else if (m_content[m_currentPos] == ']') depth--;
                                m_currentPos++;
                            }
                        } else if (m_content[m_currentPos] == '"') {
                            ReadString();
                        } else {
                            while (m_currentPos < m_content.length() && 
                                   m_content[m_currentPos] != ',' && 
                                   m_content[m_currentPos] != '}') {
                                m_currentPos++;
                            }
                        }
                    }
                }
                
                SkipWhitespace();
                if (m_currentPos < m_content.length() && m_content[m_currentPos] == ',') {
                    m_currentPos++;
                }
            }
            
            if (m_currentPos < m_content.length() && m_content[m_currentPos] == '}') {
                m_currentPos++;
            }
        }
        
        m_style.sources[sourceName] = source;
        
        SkipWhitespace();
        if (m_currentPos < m_content.length() && m_content[m_currentPos] == ',') {
            m_currentPos++;
        }
    }
    
    if (m_currentPos < m_content.length() && m_content[m_currentPos] == '}') {
        m_currentPos++;
    }
    
    return true;
}

bool MapboxStyleParser::ParseLayers()
{
    if (m_content[m_currentPos] != '[') {
        SetError("Expected '[' for layers");
        return false;
    }
    
    m_currentPos++;
    SkipWhitespace();
    
    while (m_currentPos < m_content.length() && m_content[m_currentPos] != ']') {
        SkipWhitespace();
        
        MapboxLayer layer;
        if (ParseLayer(layer)) {
            m_style.layers.push_back(layer);
        }
        
        SkipWhitespace();
        if (m_currentPos < m_content.length() && m_content[m_currentPos] == ',') {
            m_currentPos++;
        }
    }
    
    if (m_currentPos < m_content.length() && m_content[m_currentPos] == ']') {
        m_currentPos++;
    }
    
    return true;
}

bool MapboxStyleParser::ParseLayer(MapboxLayer& layer)
{
    if (m_content[m_currentPos] != '{') {
        return false;
    }
    
    m_currentPos++;
    SkipWhitespace();
    
    while (m_currentPos < m_content.length() && m_content[m_currentPos] != '}') {
        SkipWhitespace();
        std::string key = ReadString();
        
        SkipWhitespace();
        if (m_currentPos >= m_content.length() || m_content[m_currentPos] != ':') {
            break;
        }
        m_currentPos++;
        SkipWhitespace();
        
        if (key == "id") {
            layer.id = ReadString();
        } else if (key == "type") {
            layer.type = ReadString();
        } else if (key == "source") {
            layer.source = ReadString();
        } else if (key == "source-layer") {
            layer.sourceLayer = ReadString();
        } else if (key == "minzoom") {
            layer.minZoom = ReadNumber();
        } else if (key == "maxzoom") {
            layer.maxZoom = ReadNumber();
        } else if (key == "visibility") {
            std::string vis = ReadString();
            layer.visible = (vis != "none");
        } else if (key == "paint") {
            ParsePaint(layer.paint);
        } else if (key == "layout") {
            ParseLayout(layer.layout);
        } else if (key == "filter") {
            layer.filter = ParseFilter();
        } else {
            if (m_content[m_currentPos] == '{') {
                int depth = 1;
                m_currentPos++;
                while (m_currentPos < m_content.length() && depth > 0) {
                    if (m_content[m_currentPos] == '{') depth++;
                    else if (m_content[m_currentPos] == '}') depth--;
                    m_currentPos++;
                }
            } else if (m_content[m_currentPos] == '[') {
                int depth = 1;
                m_currentPos++;
                while (m_currentPos < m_content.length() && depth > 0) {
                    if (m_content[m_currentPos] == '[') depth++;
                    else if (m_content[m_currentPos] == ']') depth--;
                    m_currentPos++;
                }
            } else if (m_content[m_currentPos] == '"') {
                ReadString();
            } else {
                while (m_currentPos < m_content.length() && 
                       m_content[m_currentPos] != ',' && 
                       m_content[m_currentPos] != '}') {
                    m_currentPos++;
                }
            }
        }
        
        SkipWhitespace();
        if (m_currentPos < m_content.length() && m_content[m_currentPos] == ',') {
            m_currentPos++;
        }
    }
    
    if (m_currentPos < m_content.length() && m_content[m_currentPos] == '}') {
        m_currentPos++;
    }
    
    return true;
}

bool MapboxStyleParser::ParsePaint(std::map<std::string, std::string>& paint)
{
    if (m_content[m_currentPos] != '{') {
        return false;
    }
    
    m_currentPos++;
    SkipWhitespace();
    
    while (m_currentPos < m_content.length() && m_content[m_currentPos] != '}') {
        SkipWhitespace();
        std::string key = ReadString();
        
        SkipWhitespace();
        if (m_currentPos >= m_content.length() || m_content[m_currentPos] != ':') {
            break;
        }
        m_currentPos++;
        SkipWhitespace();
        
        std::string value;
        if (m_content[m_currentPos] == '"') {
            value = ReadString();
        } else if (m_content[m_currentPos] == '[') {
            size_t start = m_currentPos;
            int depth = 1;
            m_currentPos++;
            while (m_currentPos < m_content.length() && depth > 0) {
                if (m_content[m_currentPos] == '[') depth++;
                else if (m_content[m_currentPos] == ']') depth--;
                m_currentPos++;
            }
            value = m_content.substr(start, m_currentPos - start);
        } else if (m_content[m_currentPos] == '{') {
            size_t start = m_currentPos;
            int depth = 1;
            m_currentPos++;
            while (m_currentPos < m_content.length() && depth > 0) {
                if (m_content[m_currentPos] == '{') depth++;
                else if (m_content[m_currentPos] == '}') depth--;
                m_currentPos++;
            }
            value = m_content.substr(start, m_currentPos - start);
        } else {
            size_t start = m_currentPos;
            while (m_currentPos < m_content.length() && 
                   m_content[m_currentPos] != ',' && 
                   m_content[m_currentPos] != '}') {
                m_currentPos++;
            }
            value = Trim(m_content.substr(start, m_currentPos - start));
        }
        
        paint[key] = value;
        
        SkipWhitespace();
        if (m_currentPos < m_content.length() && m_content[m_currentPos] == ',') {
            m_currentPos++;
        }
    }
    
    if (m_currentPos < m_content.length() && m_content[m_currentPos] == '}') {
        m_currentPos++;
    }
    
    return true;
}

bool MapboxStyleParser::ParseLayout(std::map<std::string, std::string>& layout)
{
    return ParsePaint(layout);
}

std::string MapboxStyleParser::ParseFilter()
{
    if (m_content[m_currentPos] != '[') {
        return "";
    }
    
    size_t start = m_currentPos;
    int depth = 1;
    m_currentPos++;
    
    while (m_currentPos < m_content.length() && depth > 0) {
        if (m_content[m_currentPos] == '[') depth++;
        else if (m_content[m_currentPos] == ']') depth--;
        m_currentPos++;
    }
    
    return m_content.substr(start, m_currentPos - start);
}

SymbolizerRulePtr MapboxStyleParser::ConvertLayerToRule(const MapboxLayer& layer) const
{
    SymbolizerRulePtr rule = SymbolizerRule::Create(layer.id);
    rule->SetTitle(layer.id);
    
    double minScale = 1.0;
    double maxScale = 1.0;
    
    if (layer.minZoom > 0) {
        minScale = 559082264.0 / std::pow(2.0, layer.minZoom);
    }
    if (layer.maxZoom < 22) {
        maxScale = 559082264.0 / std::pow(2.0, layer.maxZoom);
    }
    
    rule->SetMinScaleDenominator(minScale);
    rule->SetMaxScaleDenominator(maxScale);
    
    SymbolizerPtr symbolizer = CreateSymbolizer(layer);
    if (symbolizer) {
        rule->AddSymbolizer(symbolizer);
    }
    
    if (!layer.filter.empty()) {
        FilterPtr filter = ConvertFilter(layer.filter);
        if (filter) {
            rule->SetFilter(filter);
        }
    }
    
    return rule;
}

SymbolizerPtr MapboxStyleParser::CreateSymbolizer(const MapboxLayer& layer) const
{
    std::string type = ToLower(layer.type);
    
    if (type == "circle" || type == "symbol") {
        return std::static_pointer_cast<Symbolizer>(CreatePointSymbolizer(layer));
    } else if (type == "line") {
        return std::static_pointer_cast<Symbolizer>(CreateLineSymbolizer(layer));
    } else if (type == "fill") {
        return std::static_pointer_cast<Symbolizer>(CreatePolygonSymbolizer(layer));
    } else if (type == "fill-extrusion") {
        return std::static_pointer_cast<Symbolizer>(CreatePolygonSymbolizer(layer));
    } else if (type == "text") {
        return std::static_pointer_cast<Symbolizer>(CreateTextSymbolizer(layer));
    } else if (type == "raster") {
        return std::static_pointer_cast<Symbolizer>(CreateRasterSymbolizer(layer));
    }
    
    return nullptr;
}

PointSymbolizerPtr MapboxStyleParser::CreatePointSymbolizer(const MapboxLayer& layer) const
{
    PointSymbolizerPtr symbolizer = PointSymbolizer::Create();
    
    auto it = layer.paint.find("circle-radius");
    if (it != layer.paint.end()) {
        symbolizer->SetSize(ParseDouble(it->second, 5.0));
    }
    
    it = layer.paint.find("circle-color");
    if (it != layer.paint.end()) {
        std::string colorStr = it->second;
        if (colorStr.find('"') != std::string::npos) {
            colorStr = Trim(colorStr);
            colorStr = colorStr.substr(1, colorStr.length() - 2);
        }
        symbolizer->SetColor(ParseColor(colorStr));
    }
    
    it = layer.paint.find("circle-stroke-color");
    if (it != layer.paint.end()) {
        std::string colorStr = it->second;
        if (colorStr.find('"') != std::string::npos) {
            colorStr = Trim(colorStr);
            colorStr = colorStr.substr(1, colorStr.length() - 2);
        }
        symbolizer->SetStrokeColor(ParseColor(colorStr));
    }
    
    it = layer.paint.find("circle-stroke-width");
    if (it != layer.paint.end()) {
        symbolizer->SetStrokeWidth(ParseDouble(it->second, 1.0));
    }
    
    it = layer.paint.find("circle-opacity");
    if (it != layer.paint.end()) {
        double opacity = ParseDouble(it->second, 1.0);
        symbolizer->SetOpacity(opacity);
    }
    
    return symbolizer;
}

LineSymbolizerPtr MapboxStyleParser::CreateLineSymbolizer(const MapboxLayer& layer) const
{
    LineSymbolizerPtr symbolizer = LineSymbolizer::Create();
    
    auto it = layer.paint.find("line-color");
    if (it != layer.paint.end()) {
        std::string colorStr = it->second;
        if (colorStr.find('"') != std::string::npos) {
            colorStr = Trim(colorStr);
            colorStr = colorStr.substr(1, colorStr.length() - 2);
        }
        symbolizer->SetColor(ParseColor(colorStr));
    }
    
    it = layer.paint.find("line-width");
    if (it != layer.paint.end()) {
        symbolizer->SetWidth(ParseDouble(it->second, 1.0));
    }
    
    it = layer.paint.find("line-opacity");
    if (it != layer.paint.end()) {
        symbolizer->SetOpacity(ParseDouble(it->second, 1.0));
    }
    
    it = layer.paint.find("line-dasharray");
    if (it != layer.paint.end()) {
        std::string dashStr = it->second;
        std::vector<double> pattern;
        
        size_t start = dashStr.find('[');
        size_t end = dashStr.find(']');
        if (start != std::string::npos && end != std::string::npos) {
            dashStr = dashStr.substr(start + 1, end - start - 1);
            std::stringstream ss(dashStr);
            std::string token;
            while (std::getline(ss, token, ',')) {
                pattern.push_back(ParseDouble(token, 0.0));
            }
        }
        
        if (!pattern.empty()) {
            symbolizer->SetDashPattern(pattern);
            symbolizer->SetDashStyle(DashStyle::kCustom);
        }
    }
    
    it = layer.layout.find("line-cap");
    if (it != layer.layout.end()) {
        std::string cap = ToLower(it->second);
        if (cap.find("butt") != std::string::npos) {
            symbolizer->SetCapStyle(LineCapStyle::kButt);
        } else if (cap.find("round") != std::string::npos) {
            symbolizer->SetCapStyle(LineCapStyle::kRound);
        } else if (cap.find("square") != std::string::npos) {
            symbolizer->SetCapStyle(LineCapStyle::kSquare);
        }
    }
    
    it = layer.layout.find("line-join");
    if (it != layer.layout.end()) {
        std::string join = ToLower(it->second);
        if (join.find("miter") != std::string::npos) {
            symbolizer->SetJoinStyle(LineJoinStyle::kMiter);
        } else if (join.find("round") != std::string::npos) {
            symbolizer->SetJoinStyle(LineJoinStyle::kRound);
        } else if (join.find("bevel") != std::string::npos) {
            symbolizer->SetJoinStyle(LineJoinStyle::kBevel);
        }
    }
    
    return symbolizer;
}

PolygonSymbolizerPtr MapboxStyleParser::CreatePolygonSymbolizer(const MapboxLayer& layer) const
{
    PolygonSymbolizerPtr symbolizer = PolygonSymbolizer::Create();
    
    auto it = layer.paint.find("fill-color");
    if (it != layer.paint.end()) {
        std::string colorStr = it->second;
        if (colorStr.find('"') != std::string::npos) {
            colorStr = Trim(colorStr);
            colorStr = colorStr.substr(1, colorStr.length() - 2);
        }
        symbolizer->SetFillColor(ParseColor(colorStr));
    }
    
    it = layer.paint.find("fill-opacity");
    if (it != layer.paint.end()) {
        symbolizer->SetFillOpacity(ParseDouble(it->second, 1.0));
    }
    
    it = layer.paint.find("fill-outline-color");
    if (it != layer.paint.end()) {
        std::string colorStr = it->second;
        if (colorStr.find('"') != std::string::npos) {
            colorStr = Trim(colorStr);
            colorStr = colorStr.substr(1, colorStr.length() - 2);
        }
        symbolizer->SetStrokeColor(ParseColor(colorStr));
        symbolizer->SetStrokeWidth(1.0);
    }
    
    return symbolizer;
}

TextSymbolizerPtr MapboxStyleParser::CreateTextSymbolizer(const MapboxLayer& layer) const
{
    TextSymbolizerPtr symbolizer = TextSymbolizer::Create();
    
    auto it = layer.paint.find("text-color");
    if (it != layer.paint.end()) {
        std::string colorStr = it->second;
        if (colorStr.find('"') != std::string::npos) {
            colorStr = Trim(colorStr);
            colorStr = colorStr.substr(1, colorStr.length() - 2);
        }
        symbolizer->SetColor(ParseColor(colorStr));
    }
    
    it = layer.paint.find("text-opacity");
    if (it != layer.paint.end()) {
        symbolizer->SetOpacity(ParseDouble(it->second, 1.0));
    }
    
    it = layer.layout.find("text-field");
    if (it != layer.layout.end()) {
        std::string field = it->second;
        if (field.find('"') != std::string::npos) {
            field = Trim(field);
            field = field.substr(1, field.length() - 2);
        }
        symbolizer->SetLabelProperty(field);
    }
    
    it = layer.layout.find("text-size");
    if (it != layer.layout.end()) {
        Font font;
        font.SetSize(static_cast<int>(ParseDouble(it->second, 12.0)));
        symbolizer->SetFont(font);
    }
    
    it = layer.layout.find("text-font");
    if (it != layer.layout.end()) {
        Font font = symbolizer->GetFont();
        std::string fontStr = it->second;
        if (fontStr.find('"') != std::string::npos) {
            fontStr = Trim(fontStr);
            fontStr = fontStr.substr(1, fontStr.length() - 2);
        }
        font.SetFamily(fontStr);
        symbolizer->SetFont(font);
    }
    
    return symbolizer;
}

RasterSymbolizerPtr MapboxStyleParser::CreateRasterSymbolizer(const MapboxLayer& layer) const
{
    RasterSymbolizerPtr symbolizer = RasterSymbolizer::Create();
    
    auto it = layer.paint.find("raster-opacity");
    if (it != layer.paint.end()) {
        symbolizer->SetOpacity(ParseDouble(it->second, 1.0));
    }
    
    return symbolizer;
}

FilterPtr MapboxStyleParser::ConvertFilter(const std::string& filterExpr) const
{
    std::string expr = Trim(filterExpr);
    
    if (expr.empty() || expr[0] != '[') {
        return nullptr;
    }
    
    size_t spacePos = expr.find(' ');
    if (spacePos == std::string::npos) {
        return nullptr;
    }
    
    std::string op = expr.substr(1, spacePos - 1);
    op = Trim(op);
    if (op.find('"') != std::string::npos) {
        op = op.substr(1, op.length() - 2);
    }
    
    std::string rest = expr.substr(spacePos + 1);
    rest = Trim(rest);
    
    if (rest.length() > 0 && rest[rest.length() - 1] == ']') {
        rest = rest.substr(0, rest.length() - 1);
    }
    
    if (op == "==" || op == "!=" || op == "<" || op == ">" || op == "<=" || op == ">=") {
        size_t commaPos = rest.find(',');
        if (commaPos == std::string::npos) {
            return nullptr;
        }
        
        std::string prop = rest.substr(0, commaPos);
        std::string value = rest.substr(commaPos + 1);
        
        prop = Trim(prop);
        value = Trim(value);
        
        if (prop.find('"') != std::string::npos) {
            prop = prop.substr(1, prop.length() - 2);
        }
        if (value.find('"') != std::string::npos) {
            value = value.substr(1, value.length() - 2);
        }
        
        ComparisonOperator compOp = ComparisonOperator::kEqual;
        if (op == "!=") compOp = ComparisonOperator::kNotEqual;
        else if (op == "<") compOp = ComparisonOperator::kLessThan;
        else if (op == ">") compOp = ComparisonOperator::kGreaterThan;
        else if (op == "<=") compOp = ComparisonOperator::kLessThanOrEqual;
        else if (op == ">=") compOp = ComparisonOperator::kGreaterThanOrEqual;
        
        return std::make_shared<ComparisonFilter>(compOp, prop, value);
    }
    
    if (op == "all" || op == "any" || op == "none") {
        LogicalOperator logOp = LogicalOperator::kAnd;
        if (op == "any") logOp = LogicalOperator::kOr;
        else if (op == "none") logOp = LogicalOperator::kNot;
        
        LogicalFilterPtr logicalFilter = std::make_shared<LogicalFilter>(logOp);
        
        // Parse sub-filters
        // Simplified: just create a basic filter
        return logicalFilter;
    }
    
    return nullptr;
}

void MapboxStyleParser::SkipWhitespace()
{
    while (m_currentPos < m_content.length() && 
           (m_content[m_currentPos] == ' ' || 
            m_content[m_currentPos] == '\t' || 
            m_content[m_currentPos] == '\n' || 
            m_content[m_currentPos] == '\r')) {
        m_currentPos++;
    }
}

std::string MapboxStyleParser::ReadString()
{
    SkipWhitespace();
    
    if (m_currentPos >= m_content.length() || m_content[m_currentPos] != '"') {
        return "";
    }
    
    m_currentPos++;
    std::string result;
    
    while (m_currentPos < m_content.length() && m_content[m_currentPos] != '"') {
        if (m_content[m_currentPos] == '\\' && m_currentPos + 1 < m_content.length()) {
            m_currentPos++;
            switch (m_content[m_currentPos]) {
                case 'n': result += '\n'; break;
                case 't': result += '\t'; break;
                case 'r': result += '\r'; break;
                case '"': result += '"'; break;
                case '\\': result += '\\'; break;
                default: result += m_content[m_currentPos]; break;
            }
        } else {
            result += m_content[m_currentPos];
        }
        m_currentPos++;
    }
    
    if (m_currentPos < m_content.length() && m_content[m_currentPos] == '"') {
        m_currentPos++;
    }
    
    return result;
}

double MapboxStyleParser::ReadNumber()
{
    SkipWhitespace();
    
    size_t start = m_currentPos;
    
    if (m_currentPos < m_content.length() && m_content[m_currentPos] == '-') {
        m_currentPos++;
    }
    
    while (m_currentPos < m_content.length() && 
           (std::isdigit(m_content[m_currentPos]) || m_content[m_currentPos] == '.')) {
        m_currentPos++;
    }
    
    std::string numStr = m_content.substr(start, m_currentPos - start);
    return ParseDouble(numStr, 0.0);
}

bool MapboxStyleParser::ReadBool()
{
    SkipWhitespace();
    
    if (m_currentPos + 4 <= m_content.length() && 
        m_content.substr(m_currentPos, 4) == "true") {
        m_currentPos += 4;
        return true;
    }
    
    if (m_currentPos + 5 <= m_content.length() && 
        m_content.substr(m_currentPos, 5) == "false") {
        m_currentPos += 5;
        return false;
    }
    
    return false;
}

std::string MapboxStyleParser::PeekToken() const
{
    size_t pos = m_currentPos;
    
    while (pos < m_content.length() && 
           (m_content[pos] == ' ' || m_content[pos] == '\t' || 
            m_content[pos] == '\n' || m_content[pos] == '\r')) {
        pos++;
    }
    
    if (pos >= m_content.length()) {
        return "";
    }
    
    if (m_content[pos] == '"') {
        pos++;
        std::string result;
        while (pos < m_content.length() && m_content[pos] != '"') {
            result += m_content[pos];
            pos++;
        }
        return result;
    }
    
    std::string result;
    while (pos < m_content.length() && 
           m_content[pos] != ' ' && m_content[pos] != '\t' && 
           m_content[pos] != '\n' && m_content[pos] != '\r' &&
           m_content[pos] != ',' && m_content[pos] != '}' &&
           m_content[pos] != ']' && m_content[pos] != ':') {
        result += m_content[pos];
        pos++;
    }
    
    return result;
}

std::string MapboxStyleParser::Trim(const std::string& str) const
{
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        return "";
    }
    
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string MapboxStyleParser::ToLower(const std::string& str) const
{
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

uint32_t MapboxStyleParser::ParseColor(const std::string& colorStr) const
{
    std::string color = Trim(colorStr);
    
    if (color.empty()) {
        return 0xFF000000;
    }
    
    if (color[0] == '#') {
        color = color.substr(1);
    } else if (color.find("rgb") == 0 || color.find("hsl") == 0) {
        return 0xFF000000;
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

double MapboxStyleParser::ParseDouble(const std::string& str, double defaultVal) const
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

int MapboxStyleParser::ParseInt(const std::string& str, int defaultVal) const
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

std::string MapboxStyleParser::ColorToString(uint32_t color)
{
    char buf[16];
    snprintf(buf, sizeof(buf), "#%06X", color & 0x00FFFFFF);
    return std::string(buf);
}

std::string MapboxStyleParser::DoubleToString(double val)
{
    std::ostringstream oss;
    oss << val;
    return oss.str();
}

void MapboxStyleParser::SetError(const std::string& message)
{
    m_lastError = message;
}

void MapboxStyleParser::ClearError()
{
    m_lastError.clear();
}

std::string MapboxStyleParser::GenerateIndent(int level)
{
    return std::string(level * 2, ' ');
}

std::string MapboxStyleParser::EscapeJson(const std::string& str)
{
    std::string result;
    for (char c : str) {
        switch (c) {
            case '"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default: result += c; break;
        }
    }
    return result;
}

std::string MapboxStyleParser::GenerateStyle(const std::vector<SymbolizerRulePtr>& rules)
{
    std::ostringstream oss;
    
    oss << "{\n";
    oss << "  \"version\": 8,\n";
    oss << "  \"name\": \"Generated Style\",\n";
    oss << "  \"sources\": {},\n";
    oss << "  \"layers\": [\n";
    
    for (size_t i = 0; i < rules.size(); ++i) {
        oss << GenerateStyle(rules[i]);
        if (i < rules.size() - 1) {
            oss << ",";
        }
        oss << "\n";
    }
    
    oss << "  ]\n";
    oss << "}\n";
    
    return oss.str();
}

std::string MapboxStyleParser::GenerateStyle(const SymbolizerRulePtr& rule)
{
    std::ostringstream oss;
    
    oss << "    {\n";
    oss << "      \"id\": \"" << EscapeJson(rule->GetName()) << "\",\n";
    
    if (!rule->GetSymbolizers().empty()) {
        SymbolizerPtr sym = rule->GetSymbolizers()[0];
        switch (sym->GetType()) {
            case SymbolizerType::kPoint:
                oss << "      \"type\": \"circle\",\n";
                break;
            case SymbolizerType::kLine:
                oss << "      \"type\": \"line\",\n";
                break;
            case SymbolizerType::kPolygon:
                oss << "      \"type\": \"fill\",\n";
                break;
            case SymbolizerType::kText:
                oss << "      \"type\": \"symbol\",\n";
                break;
            case SymbolizerType::kRaster:
                oss << "      \"type\": \"raster\",\n";
                break;
            default:
                oss << "      \"type\": \"circle\",\n";
                break;
        }
    }
    
    oss << "      \"paint\": {}\n";
    oss << "    }";
    
    return oss.str();
}

}
}
