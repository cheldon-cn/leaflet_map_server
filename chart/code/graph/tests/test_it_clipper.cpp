#include <gtest/gtest.h>
#include <ogc/draw/clipper.h>
#include <ogc/draw/draw_context.h>
#include <ogc/draw/raster_image_device.h>
#include "ogc/graph/render/draw_params.h"
#include <ogc/draw/draw_style.h>
#include <ogc/draw/color.h>
#include "ogc/envelope.h"
#include "ogc/geometry.h"
#include <memory>

using namespace ogc::draw;
using ogc::Envelope;
using ogc::Geometry;
using ogc::GeometryPtr;
using ogc::Coordinate;

class IntegrationClipperTest : public ::testing::Test {
protected:
    void SetUp() override {
        clipper = Clipper::Create();
        ASSERT_NE(clipper, nullptr);
        
        clipEnvelope = Envelope(0, 0, 100, 100);
        clipper->SetClipEnvelope(clipEnvelope);
    }
    
    void TearDown() override {
        clipper.reset();
    }
    
    ClipperPtr clipper;
    Envelope clipEnvelope;
};

TEST_F(IntegrationClipperTest, ClipperCreation) {
    EXPECT_NE(clipper, nullptr);
    EXPECT_TRUE(clipper->HasClipEnvelope());
}

TEST_F(IntegrationClipperTest, SetClipEnvelope) {
    Envelope env(10, 10, 90, 90);
    clipper->SetClipEnvelope(env);
    
    Envelope retrieved = clipper->GetClipEnvelope();
    EXPECT_DOUBLE_EQ(retrieved.GetMinX(), 10);
    EXPECT_DOUBLE_EQ(retrieved.GetMinY(), 10);
    EXPECT_DOUBLE_EQ(retrieved.GetMaxX(), 90);
    EXPECT_DOUBLE_EQ(retrieved.GetMaxY(), 90);
}

TEST_F(IntegrationClipperTest, ClearClipEnvelope) {
    EXPECT_TRUE(clipper->HasClipEnvelope());
    
    clipper->ClearClipEnvelope();
    EXPECT_FALSE(clipper->HasClipEnvelope());
}

TEST_F(IntegrationClipperTest, TestPointInside) {
    ClipResult result = clipper->TestPoint(50, 50);
    EXPECT_EQ(result, ClipResult::kInside);
}

TEST_F(IntegrationClipperTest, TestPointOutside) {
    ClipResult result = clipper->TestPoint(150, 150);
    EXPECT_EQ(result, ClipResult::kOutside);
}

TEST_F(IntegrationClipperTest, TestPointOnBoundary) {
    ClipResult result = clipper->TestPoint(0, 50);
    EXPECT_TRUE(result == ClipResult::kInside || result == ClipResult::kPartial);
}

TEST_F(IntegrationClipperTest, TestPointCoordinate) {
    Coordinate coord(50, 50);
    ClipResult result = clipper->TestPoint(coord);
    EXPECT_EQ(result, ClipResult::kInside);
}

TEST_F(IntegrationClipperTest, TestEnvelopeInside) {
    Envelope env(10, 10, 90, 90);
    ClipResult result = clipper->TestEnvelope(env);
    EXPECT_EQ(result, ClipResult::kInside);
}

TEST_F(IntegrationClipperTest, TestEnvelopeOutside) {
    Envelope env(110, 110, 150, 150);
    ClipResult result = clipper->TestEnvelope(env);
    EXPECT_EQ(result, ClipResult::kOutside);
}

TEST_F(IntegrationClipperTest, TestEnvelopePartial) {
    Envelope env(50, 50, 150, 150);
    ClipResult result = clipper->TestEnvelope(env);
    EXPECT_EQ(result, ClipResult::kPartial);
}

TEST_F(IntegrationClipperTest, SetTolerance) {
    clipper->SetTolerance(0.001);
    EXPECT_DOUBLE_EQ(clipper->GetTolerance(), 0.001);
}

TEST_F(IntegrationClipperTest, IsPointInside) {
    EXPECT_TRUE(clipper->IsPointInside(50, 50));
    EXPECT_FALSE(clipper->IsPointInside(150, 150));
}

TEST_F(IntegrationClipperTest, IsPointOnBoundary) {
    EXPECT_TRUE(clipper->IsPointOnBoundary(0, 50));
    EXPECT_TRUE(clipper->IsPointOnBoundary(50, 0));
    EXPECT_TRUE(clipper->IsPointOnBoundary(100, 50));
    EXPECT_TRUE(clipper->IsPointOnBoundary(50, 100));
    EXPECT_FALSE(clipper->IsPointOnBoundary(50, 50));
}

TEST_F(IntegrationClipperTest, ClipEnvelopeMethod) {
    Envelope env(50, 50, 150, 150);
    Envelope clipped = clipper->ClipEnvelope(env);
    
    EXPECT_DOUBLE_EQ(clipped.GetMinX(), 50);
    EXPECT_DOUBLE_EQ(clipped.GetMinY(), 50);
    EXPECT_DOUBLE_EQ(clipped.GetMaxX(), 100);
    EXPECT_DOUBLE_EQ(clipped.GetMaxY(), 100);
}

TEST_F(IntegrationClipperTest, ClipEnvelopeOutside) {
    Envelope env(110, 110, 150, 150);
    Envelope clipped = clipper->ClipEnvelope(env);
    EXPECT_TRUE(clipped.IsNull());
}

TEST_F(IntegrationClipperTest, ClipGeometriesEmpty) {
    std::vector<const Geometry*> geometries;
    std::vector<GeometryPtr> clipped = clipper->ClipGeometries(geometries);
    EXPECT_TRUE(clipped.empty());
}

TEST_F(IntegrationClipperTest, CreateWithoutEnvelope) {
    ClipperPtr defaultClipper = Clipper::Create();
    EXPECT_NE(defaultClipper, nullptr);
    EXPECT_FALSE(defaultClipper->HasClipEnvelope());
}

TEST_F(IntegrationClipperTest, CreateWithEnvelope) {
    Envelope env(0, 0, 200, 200);
    ClipperPtr envClipper = Clipper::Create(env);
    EXPECT_NE(envClipper, nullptr);
    EXPECT_TRUE(envClipper->HasClipEnvelope());
    
    Envelope retrieved = envClipper->GetClipEnvelope();
    EXPECT_DOUBLE_EQ(retrieved.GetMinX(), 0);
    EXPECT_DOUBLE_EQ(retrieved.GetMinY(), 0);
    EXPECT_DOUBLE_EQ(retrieved.GetMaxX(), 200);
    EXPECT_DOUBLE_EQ(retrieved.GetMaxY(), 200);
}

TEST_F(IntegrationClipperTest, MultiplePointTests) {
    std::vector<std::pair<double, double>> insidePoints = {
        {50, 50}, {10, 10}, {90, 90}, {1, 1}, {99, 99}
    };
    
    for (const auto& pt : insidePoints) {
        ClipResult result = clipper->TestPoint(pt.first, pt.second);
        EXPECT_EQ(result, ClipResult::kInside) 
            << "Point (" << pt.first << ", " << pt.second << ") should be inside";
    }
    
    std::vector<std::pair<double, double>> outsidePoints = {
        {-10, 50}, {150, 50}, {50, -10}, {50, 150}, {-10, -10}, {150, 150}
    };
    
    for (const auto& pt : outsidePoints) {
        ClipResult result = clipper->TestPoint(pt.first, pt.second);
        EXPECT_EQ(result, ClipResult::kOutside) 
            << "Point (" << pt.first << ", " << pt.second << ") should be outside";
    }
}

TEST_F(IntegrationClipperTest, MultipleEnvelopeTests) {
    std::vector<Envelope> insideEnvelopes = {
        Envelope(10, 10, 20, 20),
        Envelope(1, 1, 99, 99),
        Envelope(50, 50, 60, 60)
    };
    
    for (const auto& env : insideEnvelopes) {
        ClipResult result = clipper->TestEnvelope(env);
        EXPECT_EQ(result, ClipResult::kInside);
    }
    
    std::vector<Envelope> outsideEnvelopes = {
        Envelope(-50, -50, -10, -10),
        Envelope(110, 110, 150, 150),
        Envelope(200, 200, 300, 300)
    };
    
    for (const auto& env : outsideEnvelopes) {
        ClipResult result = clipper->TestEnvelope(env);
        EXPECT_EQ(result, ClipResult::kOutside);
    }
    
    std::vector<Envelope> partialEnvelopes = {
        Envelope(-10, -10, 10, 10),
        Envelope(90, 90, 110, 110),
        Envelope(50, 50, 150, 150)
    };
    
    for (const auto& env : partialEnvelopes) {
        ClipResult result = clipper->TestEnvelope(env);
        EXPECT_EQ(result, ClipResult::kPartial);
    }
}

TEST_F(IntegrationClipperTest, ClipEnvelopeBoundaryCases) {
    Envelope env1(0, 0, 100, 100);
    Envelope clipped1 = clipper->ClipEnvelope(env1);
    EXPECT_FALSE(clipped1.IsNull());
    EXPECT_DOUBLE_EQ(clipped1.GetMinX(), 0);
    EXPECT_DOUBLE_EQ(clipped1.GetMinY(), 0);
    EXPECT_DOUBLE_EQ(clipped1.GetMaxX(), 100);
    EXPECT_DOUBLE_EQ(clipped1.GetMaxY(), 100);
    
    Envelope env2(-10, -10, 110, 110);
    Envelope clipped2 = clipper->ClipEnvelope(env2);
    EXPECT_FALSE(clipped2.IsNull());
    EXPECT_DOUBLE_EQ(clipped2.GetMinX(), 0);
    EXPECT_DOUBLE_EQ(clipped2.GetMinY(), 0);
    EXPECT_DOUBLE_EQ(clipped2.GetMaxX(), 100);
    EXPECT_DOUBLE_EQ(clipped2.GetMaxY(), 100);
}

TEST_F(IntegrationClipperTest, ToleranceAffectsBoundaryTest) {
    clipper->SetTolerance(1.0);
    
    EXPECT_TRUE(clipper->IsPointOnBoundary(0, 50));
    EXPECT_TRUE(clipper->IsPointOnBoundary(1, 50));
    EXPECT_FALSE(clipper->IsPointOnBoundary(2, 50));
}

TEST_F(IntegrationClipperTest, DifferentClipRegions) {
    Envelope triangularRegion(0, 0, 50, 50);
    clipper->SetClipEnvelope(triangularRegion);
    
    EXPECT_TRUE(clipper->IsPointInside(25, 25));
    EXPECT_FALSE(clipper->IsPointInside(75, 75));
    
    Envelope largeRegion(0, 0, 1000, 1000);
    clipper->SetClipEnvelope(largeRegion);
    
    EXPECT_TRUE(clipper->IsPointInside(500, 500));
    EXPECT_FALSE(clipper->IsPointInside(1500, 1500));
}

TEST_F(IntegrationClipperTest, ClipperWithDrawContext) {
    auto device = std::make_shared<RasterImageDevice>(256, 256, PixelFormat::kRGBA8888);
    ASSERT_NE(device, nullptr);
    device->Initialize();
    
    auto context = DrawContext::Create(device.get());
    ASSERT_NE(context, nullptr);
    
    context->Begin();
    context->Clear(Color::White());
    
    Envelope clipRect(50, 50, 200, 200);
    clipper->SetClipEnvelope(clipRect);
    
    ClipResult result = clipper->TestPoint(100, 100);
    EXPECT_EQ(result, ClipResult::kInside);
    
    context->End();
    device->Finalize();
}

TEST_F(IntegrationClipperTest, ClipperStateManagement) {
    EXPECT_TRUE(clipper->HasClipEnvelope());
    
    clipper->ClearClipEnvelope();
    EXPECT_FALSE(clipper->HasClipEnvelope());
    
    clipper->SetClipEnvelope(Envelope(0, 0, 50, 50));
    EXPECT_TRUE(clipper->HasClipEnvelope());
    
    clipper->ClearClipEnvelope();
    EXPECT_FALSE(clipper->HasClipEnvelope());
}
