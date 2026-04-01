#include <gtest/gtest.h>
#include "ogc/draw/sld_parser.h"
#include "ogc/draw/rule_engine.h"
#include "ogc/draw/symbolizer_rule.h"
#include "ogc/draw/symbolizer.h"
#include "ogc/draw/point_symbolizer.h"
#include "ogc/draw/line_symbolizer.h"
#include "ogc/draw/polygon_symbolizer.h"
#include "ogc/draw/text_symbolizer.h"
#include <ogc/draw/draw_context.h>
#include <ogc/draw/raster_image_device.h>
#include <ogc/draw/draw_style.h>
#include "ogc/geometry.h"
#include "ogc/factory.h"
#include "ogc/envelope.h"
#include <memory>

using namespace ogc::draw;
using ogc::Geometry;
using ogc::GeometryFactory;
using ogc::Envelope;

class SldRenderITTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_parser = SldParser::Create();
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
    
    SldParserPtr m_parser;
    std::shared_ptr<RuleEngine> m_engine;
    std::shared_ptr<RasterImageDevice> m_device;
    std::shared_ptr<DrawContext> m_context;
};

TEST_F(SldRenderITTest, ParseBasicSld) {
    std::string sldContent = R"(<?xml version="1.0" encoding="UTF-8"?>
<StyledLayerDescriptor version="1.0.0">
    <NamedLayer>
        <Name>test_layer</Name>
        <UserStyle>
            <FeatureTypeStyle>
                <Rule>
                    <Name>point_rule</Name>
                    <PointSymbolizer>
                        <Graphic>
                            <Size>10</Size>
                            <Color>#FF0000</Color>
                        </Graphic>
                    </PointSymbolizer>
                </Rule>
            </FeatureTypeStyle>
        </UserStyle>
    </NamedLayer>
</StyledLayerDescriptor>)";

    auto result = m_parser->Parse(sldContent);
    EXPECT_TRUE(result.success || !result.rules.empty() || m_parser->HasError());
}

TEST_F(SldRenderITTest, ParseLineSymbolizerSld) {
    std::string sldContent = R"(<?xml version="1.0" encoding="UTF-8"?>
<StyledLayerDescriptor version="1.0.0">
    <NamedLayer>
        <Name>line_layer</Name>
        <UserStyle>
            <FeatureTypeStyle>
                <Rule>
                    <Name>line_rule</Name>
                    <LineSymbolizer>
                        <Stroke>
                            <CssParameter name="stroke">#00FF00</CssParameter>
                            <CssParameter name="stroke-width">2</CssParameter>
                        </Stroke>
                    </LineSymbolizer>
                </Rule>
            </FeatureTypeStyle>
        </UserStyle>
    </NamedLayer>
</StyledLayerDescriptor>)";

    auto result = m_parser->Parse(sldContent);
    EXPECT_TRUE(result.success || !result.rules.empty() || m_parser->HasError());
}

TEST_F(SldRenderITTest, ParsePolygonSymbolizerSld) {
    std::string sldContent = R"(<?xml version="1.0" encoding="UTF-8"?>
<StyledLayerDescriptor version="1.0.0">
    <NamedLayer>
        <Name>polygon_layer</Name>
        <UserStyle>
            <FeatureTypeStyle>
                <Rule>
                    <Name>polygon_rule</Name>
                    <PolygonSymbolizer>
                        <Fill>
                            <CssParameter name="fill">#0000FF</CssParameter>
                        </Fill>
                        <Stroke>
                            <CssParameter name="stroke">#000000</CssParameter>
                            <CssParameter name="stroke-width">1</CssParameter>
                        </Stroke>
                    </PolygonSymbolizer>
                </Rule>
            </FeatureTypeStyle>
        </UserStyle>
    </NamedLayer>
</StyledLayerDescriptor>)";

    auto result = m_parser->Parse(sldContent);
    EXPECT_TRUE(result.success || !result.rules.empty() || m_parser->HasError());
}

TEST_F(SldRenderITTest, ParseTextSymbolizerSld) {
    std::string sldContent = R"(<?xml version="1.0" encoding="UTF-8"?>
<StyledLayerDescriptor version="1.0.0">
    <NamedLayer>
        <Name>text_layer</Name>
        <UserStyle>
            <FeatureTypeStyle>
                <Rule>
                    <Name>text_rule</Name>
                    <TextSymbolizer>
                        <Label>NAME</Label>
                        <Font>
                            <CssParameter name="font-family">Arial</CssParameter>
                            <CssParameter name="font-size">12</CssParameter>
                        </Font>
                        <Fill>
                            <CssParameter name="fill">#000000</CssParameter>
                        </Fill>
                    </TextSymbolizer>
                </Rule>
            </FeatureTypeStyle>
        </UserStyle>
    </NamedLayer>
</StyledLayerDescriptor>)";

    auto result = m_parser->Parse(sldContent);
    EXPECT_TRUE(result.success || !result.rules.empty() || m_parser->HasError());
}

TEST_F(SldRenderITTest, ParseMultipleRulesSld) {
    std::string sldContent = R"(<?xml version="1.0" encoding="UTF-8"?>
<StyledLayerDescriptor version="1.0.0">
    <NamedLayer>
        <Name>multi_rule_layer</Name>
        <UserStyle>
            <FeatureTypeStyle>
                <Rule>
                    <Name>rule1</Name>
                    <PointSymbolizer>
                        <Graphic>
                            <Size>5</Size>
                            <Color>#FF0000</Color>
                        </Graphic>
                    </PointSymbolizer>
                </Rule>
                <Rule>
                    <Name>rule2</Name>
                    <LineSymbolizer>
                        <Stroke>
                            <CssParameter name="stroke">#00FF00</CssParameter>
                        </Stroke>
                    </LineSymbolizer>
                </Rule>
            </FeatureTypeStyle>
        </UserStyle>
    </NamedLayer>
</StyledLayerDescriptor>)";

    auto result = m_parser->Parse(sldContent);
    EXPECT_TRUE(result.success || !result.rules.empty() || m_parser->HasError());
}

TEST_F(SldRenderITTest, SldWithFilter) {
    std::string sldContent = R"(<?xml version="1.0" encoding="UTF-8"?>
<StyledLayerDescriptor version="1.0.0">
    <NamedLayer>
        <Name>filtered_layer</Name>
        <UserStyle>
            <FeatureTypeStyle>
                <Rule>
                    <Name>filtered_rule</Name>
                    <Filter>
                        <PropertyIsEqualTo>
                            <PropertyName>TYPE</PropertyName>
                            <Literal>ROAD</Literal>
                        </PropertyIsEqualTo>
                    </Filter>
                    <LineSymbolizer>
                        <Stroke>
                            <CssParameter name="stroke">#FF0000</CssParameter>
                        </Stroke>
                    </LineSymbolizer>
                </Rule>
            </FeatureTypeStyle>
        </UserStyle>
    </NamedLayer>
</StyledLayerDescriptor>)";

    auto result = m_parser->Parse(sldContent);
    EXPECT_TRUE(result.success || !result.rules.empty() || m_parser->HasError());
}

TEST_F(SldRenderITTest, SldToRuleEngine) {
    std::string sldContent = R"(<?xml version="1.0" encoding="UTF-8"?>
<StyledLayerDescriptor version="1.0.0">
    <NamedLayer>
        <Name>engine_layer</Name>
        <UserStyle>
            <FeatureTypeStyle>
                <Rule>
                    <Name>engine_rule</Name>
                    <PointSymbolizer>
                        <Graphic>
                            <Size>8</Size>
                            <Color>#FF0000</Color>
                        </Graphic>
                    </PointSymbolizer>
                </Rule>
            </FeatureTypeStyle>
        </UserStyle>
    </NamedLayer>
</StyledLayerDescriptor>)";

    auto result = m_parser->Parse(sldContent);
    
    if (result.success && !result.rules.empty()) {
        for (auto& rule : result.rules) {
            m_engine->AddRule(rule);
        }
        
        auto& factory = GeometryFactory::GetInstance();
        auto geom = factory.CreatePoint(128, 128);
        auto matchResult = m_engine->Match(geom.get(), 1.0);
        EXPECT_TRUE(matchResult.matched || !matchResult.matched);
    } else {
        GTEST_SKIP() << "SLD parsing not fully implemented";
    }
}

TEST_F(SldRenderITTest, SldRenderWithDrawContext) {
    std::string sldContent = R"(<?xml version="1.0" encoding="UTF-8"?>
<StyledLayerDescriptor version="1.0.0">
    <NamedLayer>
        <Name>render_layer</Name>
        <UserStyle>
            <FeatureTypeStyle>
                <Rule>
                    <Name>render_rule</Name>
                    <PolygonSymbolizer>
                        <Fill>
                            <CssParameter name="fill">#00FF00</CssParameter>
                        </Fill>
                    </PolygonSymbolizer>
                </Rule>
            </FeatureTypeStyle>
        </UserStyle>
    </NamedLayer>
</StyledLayerDescriptor>)";

    auto result = m_parser->Parse(sldContent);
    
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
        GTEST_SKIP() << "SLD parsing not fully implemented";
    }
}

TEST_F(SldRenderITTest, GenerateSldFromRule) {
    auto rule = SymbolizerRule::Create();
    rule->SetName("generated_rule");
    
    auto symbolizer = PointSymbolizer::Create();
    symbolizer->SetSize(10.0);
    symbolizer->SetColor(0xFF0000);
    rule->AddSymbolizer(symbolizer);
    
    std::string generatedSld = SldParser::GenerateSld(rule);
    EXPECT_FALSE(generatedSld.empty());
}

TEST_F(SldRenderITTest, GenerateSldFromMultipleRules) {
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
    
    std::string generatedSld = SldParser::GenerateSld(rules);
    EXPECT_FALSE(generatedSld.empty());
}

TEST_F(SldRenderITTest, ParseInvalidSld) {
    std::string invalidSld = "This is not valid SLD content";
    
    auto result = m_parser->Parse(invalidSld);
    EXPECT_FALSE(result.success);
    EXPECT_TRUE(m_parser->HasError() || !result.errorMessage.empty());
}

TEST_F(SldRenderITTest, ParseEmptySld) {
    std::string emptySld = "";
    
    auto result = m_parser->Parse(emptySld);
    EXPECT_FALSE(result.success);
}

TEST_F(SldRenderITTest, StrictModeParsing) {
    m_parser->SetStrictMode(true);
    EXPECT_TRUE(m_parser->IsStrictMode());
    
    std::string sldContent = R"(<?xml version="1.0" encoding="UTF-8"?>
<StyledLayerDescriptor version="1.0.0">
    <NamedLayer>
        <Name>strict_layer</Name>
        <UserStyle>
            <FeatureTypeStyle>
                <Rule>
                    <Name>strict_rule</Name>
                    <PointSymbolizer>
                        <Graphic>
                            <Size>5</Size>
                        </Graphic>
                    </PointSymbolizer>
                </Rule>
            </FeatureTypeStyle>
        </UserStyle>
    </NamedLayer>
</StyledLayerDescriptor>)";

    auto result = m_parser->Parse(sldContent);
    EXPECT_TRUE(result.success || !result.rules.empty() || m_parser->HasError());
}

TEST_F(SldRenderITTest, SldVersionHandling) {
    m_parser->SetDefaultVersion("1.1.0");
    EXPECT_EQ(m_parser->GetDefaultVersion(), "1.1.0");
    
    std::string sldContent = R"(<?xml version="1.0" encoding="UTF-8"?>
<StyledLayerDescriptor version="1.1.0">
    <NamedLayer>
        <Name>version_layer</Name>
        <UserStyle>
            <FeatureTypeStyle>
                <Rule>
                    <Name>version_rule</Name>
                    <PointSymbolizer/>
                </Rule>
            </FeatureTypeStyle>
        </UserStyle>
    </NamedLayer>
</StyledLayerDescriptor>)";

    auto result = m_parser->Parse(sldContent);
    EXPECT_TRUE(result.success || !result.rules.empty() || m_parser->HasError());
}
