#include <gtest/gtest.h>
#include <ogc/proj/proj_transformer.h>
#include <ogc/proj/coordinate_transformer.h>
#include <ogc/draw/draw_context.h>
#include <ogc/draw/raster_image_device.h>
#include <ogc/draw/draw_style.h>
#include "ogc/geom/coordinate.h"
#include "ogc/geom/envelope.h"
#include <memory>

using namespace ogc::proj;
using namespace ogc::draw;
using ogc::Coordinate;
using ogc::Envelope;

class ProjTransformRenderITTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_device = std::make_shared<RasterImageDevice>(512, 512, PixelFormat::kRGBA8888);
        ASSERT_NE(m_device, nullptr);
        m_device->Initialize();
        
        m_context = DrawContext::Create(m_device.get());
        ASSERT_NE(m_context, nullptr);
    }
    
    void TearDown() override {
        m_context.reset();
        m_device.reset();
    }
    
    std::shared_ptr<RasterImageDevice> m_device;
    std::unique_ptr<DrawContext> m_context;
};

TEST_F(ProjTransformRenderITTest, BasicProjectionTransform) {
    auto transformer = ProjTransformer::Create("EPSG:4326", "EPSG:3857");
    if (!transformer || !transformer->IsValid()) {
        GTEST_SKIP() << "PROJ library not available";
    }
    
    Coordinate wgs84Coord(116.4, 39.9);
    Coordinate webMercator = transformer->Transform(wgs84Coord);
    
    EXPECT_FALSE(webMercator.IsEmpty());
    
    Coordinate backToWgs84 = transformer->TransformInverse(webMercator);
    EXPECT_NEAR(backToWgs84.x, wgs84Coord.x, 0.001);
    EXPECT_NEAR(backToWgs84.y, wgs84Coord.y, 0.001);
}

TEST_F(ProjTransformRenderITTest, RenderWithProjection) {
    auto transformer = ProjTransformer::Create("EPSG:4326", "EPSG:3857");
    if (!transformer || !transformer->IsValid()) {
        GTEST_SKIP() << "PROJ library not available";
    }
    
    Envelope wgs84Extent(116.0, 39.5, 117.0, 40.5);
    Envelope webMercatorExtent = transformer->Transform(wgs84Extent);
    
    EXPECT_FALSE(webMercatorExtent.IsEmpty());
    
    m_context->Begin();
    m_context->Clear(Color::White());
    
    Coordinate centerWgs84(116.5, 40.0);
    Coordinate centerWebMercator = transformer->Transform(centerWgs84);
    
    m_context->DrawPoint(256, 256);
    
    m_context->End();
}

TEST_F(ProjTransformRenderITTest, CoordinateTransformerBasic) {
    auto coordTransformer = CoordinateTransformer::Create("EPSG:4326", "EPSG:3857");
    if (!coordTransformer || !coordTransformer->IsValid()) {
        GTEST_SKIP() << "PROJ library not available";
    }
    
    Coordinate wgs84(116.5, 40.0);
    Coordinate webMercator = coordTransformer->Transform(wgs84);
    
    EXPECT_FALSE(webMercator.IsEmpty());
    
    Coordinate backToWgs84 = coordTransformer->TransformInverse(webMercator);
    EXPECT_NEAR(backToWgs84.x, wgs84.x, 0.001);
    EXPECT_NEAR(backToWgs84.y, wgs84.y, 0.001);
}

TEST_F(ProjTransformRenderITTest, MultiplePointsProjection) {
    auto transformer = ProjTransformer::Create("EPSG:4326", "EPSG:3857");
    if (!transformer || !transformer->IsValid()) {
        GTEST_SKIP() << "PROJ library not available";
    }
    
    std::vector<Coordinate> wgs84Coords = {
        Coordinate(116.0, 39.0),
        Coordinate(117.0, 39.0),
        Coordinate(117.0, 40.0),
        Coordinate(116.0, 40.0)
    };
    
    std::vector<double> x(wgs84Coords.size());
    std::vector<double> y(wgs84Coords.size());
    
    for (size_t i = 0; i < wgs84Coords.size(); i++) {
        x[i] = wgs84Coords[i].x;
        y[i] = wgs84Coords[i].y;
    }
    
    transformer->TransformArray(x.data(), y.data(), wgs84Coords.size());
    
    transformer->TransformArrayInverse(x.data(), y.data(), wgs84Coords.size());
    
    for (size_t i = 0; i < wgs84Coords.size(); i++) {
        EXPECT_NEAR(x[i], wgs84Coords[i].x, 0.001);
        EXPECT_NEAR(y[i], wgs84Coords[i].y, 0.001);
    }
}

TEST_F(ProjTransformRenderITTest, EnvelopeProjection) {
    auto transformer = ProjTransformer::Create("EPSG:4326", "EPSG:3857");
    if (!transformer || !transformer->IsValid()) {
        GTEST_SKIP() << "PROJ library not available";
    }
    
    Envelope wgs84Env(116.0, 39.0, 117.0, 40.0);
    Envelope webMercatorEnv = transformer->Transform(wgs84Env);
    
    EXPECT_FALSE(webMercatorEnv.IsEmpty());
    EXPECT_GT(webMercatorEnv.GetWidth(), 0);
    EXPECT_GT(webMercatorEnv.GetHeight(), 0);
    
    Envelope backToWgs84 = transformer->TransformInverse(webMercatorEnv);
    
    EXPECT_NEAR(backToWgs84.GetMinX(), wgs84Env.GetMinX(), 0.01);
    EXPECT_NEAR(backToWgs84.GetMinY(), wgs84Env.GetMinY(), 0.01);
    EXPECT_NEAR(backToWgs84.GetMaxX(), wgs84Env.GetMaxX(), 0.01);
    EXPECT_NEAR(backToWgs84.GetMaxY(), wgs84Env.GetMaxY(), 0.01);
}

TEST_F(ProjTransformRenderITTest, DifferentProjections) {
    auto wgs84ToWebMercator = ProjTransformer::Create("EPSG:4326", "EPSG:3857");
    if (!wgs84ToWebMercator || !wgs84ToWebMercator->IsValid()) {
        GTEST_SKIP() << "PROJ library not available";
    }
    
    auto webMercatorToWgs84 = ProjTransformer::Create("EPSG:3857", "EPSG:4326");
    ASSERT_NE(webMercatorToWgs84, nullptr);
    
    Coordinate wgs84(116.4, 39.9);
    Coordinate webMercator = wgs84ToWebMercator->Transform(wgs84);
    
    Coordinate backToWgs84 = webMercatorToWgs84->Transform(webMercator);
    
    EXPECT_NEAR(backToWgs84.x, wgs84.x, 0.001);
    EXPECT_NEAR(backToWgs84.y, wgs84.y, 0.001);
}

TEST_F(ProjTransformRenderITTest, ProjectionWithDrawContext) {
    auto transformer = ProjTransformer::Create("EPSG:4326", "EPSG:3857");
    if (!transformer || !transformer->IsValid()) {
        GTEST_SKIP() << "PROJ library not available";
    }
    
    Envelope wgs84Extent(116.0, 39.0, 117.0, 40.0);
    Envelope webMercatorExtent = transformer->Transform(wgs84Extent);
    
    m_context->Begin();
    m_context->Clear(Color::White());
    
    std::vector<Coordinate> points = {
        Coordinate(116.25, 39.25),
        Coordinate(116.50, 39.50),
        Coordinate(116.75, 39.75)
    };
    
    for (const auto& pt : points) {
        Coordinate transformed = transformer->Transform(pt);
        double screenX = (transformed.x - webMercatorExtent.GetMinX()) / webMercatorExtent.GetWidth() * 512;
        double screenY = (webMercatorExtent.GetMaxY() - transformed.y) / webMercatorExtent.GetHeight() * 512;
        m_context->DrawPoint(screenX, screenY);
    }
    
    m_context->End();
}

TEST_F(ProjTransformRenderITTest, ProjectionClone) {
    auto transformer = ProjTransformer::Create("EPSG:4326", "EPSG:3857");
    if (!transformer || !transformer->IsValid()) {
        GTEST_SKIP() << "PROJ library not available";
    }
    
    auto clone = transformer->Clone();
    ASSERT_NE(clone, nullptr);
    
    Coordinate coord(116.4, 39.9);
    Coordinate result1 = transformer->Transform(coord);
    Coordinate result2 = clone->Transform(coord);
    
    EXPECT_DOUBLE_EQ(result1.x, result2.x);
    EXPECT_DOUBLE_EQ(result1.y, result2.y);
}

TEST_F(ProjTransformRenderITTest, ProjectionInfo) {
    auto transformer = ProjTransformer::Create("EPSG:4326", "EPSG:3857");
    if (!transformer || !transformer->IsValid()) {
        GTEST_SKIP() << "PROJ library not available";
    }
    
    auto sourceCRS = transformer->GetSourceCRS();
    EXPECT_FALSE(sourceCRS.empty());
    
    auto targetCRS = transformer->GetTargetCRS();
    EXPECT_FALSE(targetCRS.empty());
}

TEST_F(ProjTransformRenderITTest, WGS84ToWebMercatorFactory) {
    auto transformer = CoordinateTransformer::CreateWGS84ToWebMercator();
    if (!transformer || !transformer->IsValid()) {
        GTEST_SKIP() << "PROJ library not available";
    }
    
    Coordinate wgs84(116.4, 39.9);
    Coordinate webMercator = transformer->Transform(wgs84);
    
    EXPECT_FALSE(webMercator.IsEmpty());
    EXPECT_GT(webMercator.x, 0);
    EXPECT_GT(webMercator.y, 0);
}

TEST_F(ProjTransformRenderITTest, WebMercatorToWGS84Factory) {
    auto wgs84ToWebMercator = CoordinateTransformer::CreateWGS84ToWebMercator();
    if (!wgs84ToWebMercator || !wgs84ToWebMercator->IsValid()) {
        GTEST_SKIP() << "PROJ library not available";
    }
    
    auto webMercatorToWgs84 = CoordinateTransformer::CreateWebMercatorToWGS84();
    if (!webMercatorToWgs84 || !webMercatorToWgs84->IsValid()) {
        GTEST_SKIP() << "PROJ library not available";
    }
    
    Coordinate wgs84(116.4, 39.9);
    Coordinate webMercator = wgs84ToWebMercator->Transform(wgs84);
    Coordinate backToWgs84 = webMercatorToWgs84->Transform(webMercator);
    
    EXPECT_NEAR(backToWgs84.x, wgs84.x, 0.001);
    EXPECT_NEAR(backToWgs84.y, wgs84.y, 0.001);
}
