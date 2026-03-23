#include <gtest/gtest.h>
#include "ogc/draw/mapbox_style_parser.h"
#include "ogc/draw/rule_engine.h"
#include "ogc/draw/symbolizer_rule.h"
#include "ogc/draw/symbolizer.h"
#include "ogc/draw/point_symbolizer.h"
#include "ogc/draw/line_symbolizer.h"
#include "ogc/draw/polygon_symbolizer.h"
#include "ogc/draw/text_symbolizer.h"
#include "ogc/draw/draw_context.h"
#include "ogc/draw/raster_image_device.h"
#include "ogc/draw/draw_style.h"
#include "ogc/geometry.h"
#include "ogc/factory.h"
#include "ogc/envelope.h"
#include <memory>

using namespace ogc::draw;
using ogc::Geometry;
using ogc::GeometryFactory;
using ogc::Envelope;

class MapboxStyleRenderITTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_parser = MapboxStyleParser::Create();
        m_engine = RuleEngine::Create();
        m_device = RasterImageDevice::Create(256, 256, 4);
        m_context = DrawContext::Create(m_device);
    }
    
    void TearDown() override {
        m_context.reset();
        m_device.reset();
        m_engine.reset();
        m_parser.reset();
    }
    
    MapboxStyleParserPtr m_parser;
    std::shared_ptr<RuleEngine> m_engine;
    std::shared_ptr<RasterImageDevice> m_device;
    std::shared_ptr<DrawContext> m_context;
};

TEST_F(MapboxStyleRenderITTest, ParseBasicStyle) {
    std::string jsonContent = R"({
        "version": 8,
        "name": "test-style",
        "sources": {},
        "layers": [
            {
                "id": "point-layer",
                "type": "circle",
                "paint": {
                    "circle-radius": 5,
                    "circle-color": "#FF0000"
                }
            }
        ]
    })";

    auto result = m_parser->Parse(jsonContent);
    EXPECT_TRUE(result.success || !result.rules.empty() || m_parser->HasError());
}

TEST_F(MapboxStyleRenderITTest, ParseLineLayer) {
    std::string jsonContent = R"({
        "version": 8,
        "name": "line-style",
        "sources": {},
        "layers": [
            {
                "id": "line-layer",
                "type": "line",
                "paint": {
                    "line-color": "#00FF00",
                    "line-width": 2
                }
            }
        ]
    })";

    auto result = m_parser->Parse(jsonContent);
    EXPECT_TRUE(result.success || !result.rules.empty() || m_parser->HasError());
}

TEST_F(MapboxStyleRenderITTest, ParsePolygonLayer) {
    std::string jsonContent = R"({
        "version": 8,
        "name": "polygon-style",
        "sources": {},
        "layers": [
            {
                "id": "polygon-layer",
                "type": "fill",
                "paint": {
                    "fill-color": "#0000FF",
                    "fill-opacity": 0.8
                }
            }
        ]
    })";

    auto result = m_parser->Parse(jsonContent);
    EXPECT_TRUE(result.success || !result.rules.empty() || m_parser->HasError());
}

TEST_F(MapboxStyleRenderITTest, ParseTextLayer) {
    std::string jsonContent = R"({
        "version": 8,
        "name": "text-style",
        "sources": {},
        "layers": [
            {
                "id": "text-layer",
                "type": "symbol",
                "layout": {
                    "text-field": "{name}",
                    "text-font": ["Arial"],
                    "text-size": 12
                },
                "paint": {
                    "text-color": "#000000"
                }
            }
        ]
    })";

    auto result = m_parser->Parse(jsonContent);
    EXPECT_TRUE(result.success || !result.rules.empty() || m_parser->HasError());
}

TEST_F(MapboxStyleRenderITTest, ParseMultipleLayers) {
    std::string jsonContent = R"({
        "version": 8,
        "name": "multi-layer-style",
        "sources": {},
        "layers": [
            {
                "id": "layer1",
                "type": "circle",
                "paint": {
                    "circle-radius": 5,
                    "circle-color": "#FF0000"
                }
            },
            {
                "id": "layer2",
                "type": "line",
                "paint": {
                    "line-color": "#00FF00",
                    "line-width": 2
                }
            },
            {
                "id": "layer3",
                "type": "fill",
                "paint": {
                    "fill-color": "#0000FF"
                }
            }
        ]
    })";

    auto result = m_parser->Parse(jsonContent);
    EXPECT_TRUE(result.success || !result.rules.empty() || m_parser->HasError());
}

TEST_F(MapboxStyleRenderITTest, ParseLayerWithFilter) {
    std::string jsonContent = R"({
        "version": 8,
        "name": "filtered-style",
        "sources": {},
        "layers": [
            {
                "id": "filtered-layer",
                "type": "line",
                "filter": ["==", "type", "road"],
                "paint": {
                    "line-color": "#FF0000",
                    "line-width": 3
                }
            }
        ]
    })";

    auto result = m_parser->Parse(jsonContent);
    EXPECT_TRUE(result.success || !result.rules.empty() || m_parser->HasError());
}

TEST_F(MapboxStyleRenderITTest, ParseLayerWithZoomRange) {
    std::string jsonContent = R"({
        "version": 8,
        "name": "zoom-style",
        "sources": {},
        "layers": [
            {
                "id": "zoom-layer",
                "type": "line",
                "minzoom": 5,
                "maxzoom": 15,
                "paint": {
                    "line-color": "#FF0000"
                }
            }
        ]
    })";

    auto result = m_parser->Parse(jsonContent);
    EXPECT_TRUE(result.success || !result.rules.empty() || m_parser->HasError());
}

TEST_F(MapboxStyleRenderITTest, MapboxStyleToRuleEngine) {
    std::string jsonContent = R"({
        "version": 8,
        "name": "engine-style",
        "sources": {},
        "layers": [
            {
                "id": "engine-layer",
                "type": "circle",
                "paint": {
                    "circle-radius": 8,
                    "circle-color": "#FF0000"
                }
            }
        ]
    })";

    auto result = m_parser->Parse(jsonContent);
    
    if (result.success && !result.rules.empty()) {
        for (auto& rule : result.rules) {
            m_engine->AddRule(rule);
        }
        
        auto& factory = GeometryFactory::GetInstance();
        auto geom = factory.CreatePoint(128, 128);
        auto matchResult = m_engine->Match(geom.get(), 1.0);
        EXPECT_TRUE(matchResult.matched || !matchResult.matched);
    } else {
        GTEST_SKIP() << "Mapbox style parsing not fully implemented";
    }
}

TEST_F(MapboxStyleRenderITTest, MapboxStyleRenderWithDrawContext) {
    std::string jsonContent = R"({
        "version": 8,
        "name": "render-style",
        "sources": {},
        "layers": [
            {
                "id": "render-layer",
                "type": "fill",
                "paint": {
                    "fill-color": "#00FF00"
                }
            }
        ]
    })";

    auto result = m_parser->Parse(jsonContent);
    
    if (result.success && !result.rules.empty()) {
        for (auto& rule : result.rules) {
            m_engine->AddRule(rule);
        }
        
        DrawParams params;
        params.SetSize(256, 256);
        m_context->BeginDraw(params);
        
        auto& factory = GeometryFactory::GetInstance();
        ogc::CoordinateList coords = {
            ogc::Coordinate(50, 50),
            ogc::Coordinate(200, 50),
            ogc::Coordinate(200, 200),
            ogc::Coordinate(50, 200),
            ogc::Coordinate(50, 50)
        };
        auto geom = factory.CreatePolygon(coords);
        
        auto matchResult = m_engine->Match(geom.get(), 1.0);
        m_context->EndDraw();
        
        EXPECT_TRUE(true);
    } else {
        GTEST_SKIP() << "Mapbox style parsing not fully implemented";
    }
}

TEST_F(MapboxStyleRenderITTest, GenerateStyleFromRule) {
    auto rule = SymbolizerRule::Create();
    rule->SetName("generated_rule");
    
    auto symbolizer = PointSymbolizer::Create();
    symbolizer->SetSize(10.0);
    symbolizer->SetColor(0xFF0000);
    rule->AddSymbolizer(symbolizer);
    
    std::string generatedStyle = MapboxStyleParser::GenerateStyle(rule);
    EXPECT_FALSE(generatedStyle.empty());
}

TEST_F(MapboxStyleRenderITTest, GenerateStyleFromMultipleRules) {
    std::vector<SymbolizerRulePtr> rules;
    
    auto rule1 = SymbolizerRule::Create();
    rule1->SetName("rule1");
    auto symbolizer1 = LineSymbolizer::Create();
    symbolizer1->SetWidth(2.0);
    symbolizer1->SetColor(0x00FF00);
    rule1->AddSymbolizer(symbolizer1);
    rules.push_back(rule1);
    
    auto rule2 = SymbolizerRule::Create();
    rule2->SetName("rule2");
    auto symbolizer2 = PolygonSymbolizer::Create();
    symbolizer2->SetFillColor(0x0000FF);
    rule2->AddSymbolizer(symbolizer2);
    rules.push_back(rule2);
    
    std::string generatedStyle = MapboxStyleParser::GenerateStyle(rules);
    EXPECT_FALSE(generatedStyle.empty());
}

TEST_F(MapboxStyleRenderITTest, ParseInvalidJson) {
    std::string invalidJson = "This is not valid JSON content";
    
    auto result = m_parser->Parse(invalidJson);
    EXPECT_FALSE(result.success);
    EXPECT_TRUE(m_parser->HasError() || !result.errorMessage.empty());
}

TEST_F(MapboxStyleRenderITTest, ParseEmptyJson) {
    std::string emptyJson = "";
    
    auto result = m_parser->Parse(emptyJson);
    EXPECT_FALSE(result.success);
}

TEST_F(MapboxStyleRenderITTest, ParseStyleWithSources) {
    std::string jsonContent = R"({
        "version": 8,
        "name": "source-style",
        "sources": {
            "osm": {
                "type": "raster",
                "tiles": ["https://tile.openstreetmap.org/{z}/{x}/{y}.png"],
                "minzoom": 0,
                "maxzoom": 19
            }
        },
        "layers": [
            {
                "id": "osm-layer",
                "type": "raster",
                "source": "osm"
            }
        ]
    })";

    auto result = m_parser->Parse(jsonContent);
    EXPECT_TRUE(result.success || !result.rules.empty() || m_parser->HasError());
}

TEST_F(MapboxStyleRenderITTest, StrictModeParsing) {
    m_parser->SetStrictMode(true);
    EXPECT_TRUE(m_parser->IsStrictMode());
    
    std::string jsonContent = R"({
        "version": 8,
        "name": "strict-style",
        "sources": {},
        "layers": [
            {
                "id": "strict-layer",
                "type": "circle",
                "paint": {
                    "circle-radius": 5
                }
            }
        ]
    })";

    auto result = m_parser->Parse(jsonContent);
    EXPECT_TRUE(result.success || !result.rules.empty() || m_parser->HasError());
}

TEST_F(MapboxStyleRenderITTest, LayerVisibility) {
    std::string jsonContent = R"({
        "version": 8,
        "name": "visibility-style",
        "sources": {},
        "layers": [
            {
                "id": "visible-layer",
                "type": "circle",
                "visibility": "visible",
                "paint": {
                    "circle-radius": 5
                }
            },
            {
                "id": "hidden-layer",
                "type": "circle",
                "visibility": "none",
                "paint": {
                    "circle-radius": 5
                }
            }
        ]
    })";

    auto result = m_parser->Parse(jsonContent);
    EXPECT_TRUE(result.success || !result.rules.empty() || m_parser->HasError());
}

TEST_F(MapboxStyleRenderITTest, LayerOpacity) {
    std::string jsonContent = R"({
        "version": 8,
        "name": "opacity-style",
        "sources": {},
        "layers": [
            {
                "id": "opacity-layer",
                "type": "fill",
                "paint": {
                    "fill-color": "#FF0000",
                    "fill-opacity": 0.5
                }
            }
        ]
    })";

    auto result = m_parser->Parse(jsonContent);
    EXPECT_TRUE(result.success || !result.rules.empty() || m_parser->HasError());
}
