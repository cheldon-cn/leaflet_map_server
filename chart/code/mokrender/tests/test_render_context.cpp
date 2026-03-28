#include <gtest/gtest.h>
#include "ogc/mokrender/render_context.h"
#include "ogc/mokrender/common.h"

using namespace ogc::mokrender;

class RenderContextTest : public ::testing::Test {
protected:
    void SetUp() override {
        context = new RenderContext();
    }
    
    void TearDown() override {
        delete context;
    }
    
    RenderContext* context;
};

TEST_F(RenderContextTest, InitializeWithConfig) {
    RenderConfig config;
    config.outputWidth = 1024;
    config.outputHeight = 768;
    config.dpi = 150.0;
    
    MokRenderResult result = context->Initialize(config);
    EXPECT_TRUE(result.IsSuccess());
}

TEST_F(RenderContextTest, InitializeWithZeroSize) {
    RenderConfig config;
    config.outputWidth = 0;
    config.outputHeight = 0;
    config.dpi = 96.0;
    
    MokRenderResult result = context->Initialize(config);
    EXPECT_TRUE(result.IsSuccess());
}

TEST_F(RenderContextTest, InitializeWithNegativeSize) {
    RenderConfig config;
    config.outputWidth = -100;
    config.outputHeight = -100;
    config.dpi = 96.0;
    
    MokRenderResult result = context->Initialize(config);
    EXPECT_TRUE(result.IsSuccess());
}

TEST_F(RenderContextTest, InitializeWithZeroDPI) {
    RenderConfig config;
    config.outputWidth = 800;
    config.outputHeight = 600;
    config.dpi = 0.0;
    
    MokRenderResult result = context->Initialize(config);
    EXPECT_TRUE(result.IsSuccess());
}

TEST_F(RenderContextTest, SetExtent) {
    context->SetExtent(0.0, 0.0, 1000.0, 1000.0);
    
    double minX, minY, maxX, maxY;
    context->GetExtent(minX, minY, maxX, maxY);
    
    EXPECT_DOUBLE_EQ(minX, 0.0);
    EXPECT_DOUBLE_EQ(minY, 0.0);
    EXPECT_DOUBLE_EQ(maxX, 1000.0);
    EXPECT_DOUBLE_EQ(maxY, 1000.0);
}

TEST_F(RenderContextTest, SetExtent_NegativeCoords) {
    context->SetExtent(-180.0, -90.0, 180.0, 90.0);
    
    double minX, minY, maxX, maxY;
    context->GetExtent(minX, minY, maxX, maxY);
    
    EXPECT_DOUBLE_EQ(minX, -180.0);
    EXPECT_DOUBLE_EQ(minY, -90.0);
    EXPECT_DOUBLE_EQ(maxX, 180.0);
    EXPECT_DOUBLE_EQ(maxY, 90.0);
}

TEST_F(RenderContextTest, SetExtent_InvalidRange) {
    context->SetExtent(100.0, 100.0, 0.0, 0.0);
    
    double minX, minY, maxX, maxY;
    context->GetExtent(minX, minY, maxX, maxY);
    
    EXPECT_DOUBLE_EQ(minX, 100.0);
    EXPECT_DOUBLE_EQ(maxX, 0.0);
}

TEST_F(RenderContextTest, SetDPI) {
    context->SetDPI(300.0);
    EXPECT_DOUBLE_EQ(context->GetDPI(), 300.0);
}

TEST_F(RenderContextTest, SetDPI_Zero) {
    context->SetDPI(0.0);
    EXPECT_DOUBLE_EQ(context->GetDPI(), 0.0);
}

TEST_F(RenderContextTest, SetDPI_Negative) {
    context->SetDPI(-96.0);
    EXPECT_DOUBLE_EQ(context->GetDPI(), -96.0);
}

TEST_F(RenderContextTest, DefaultDPI) {
    EXPECT_DOUBLE_EQ(context->GetDPI(), 96.0);
}

TEST_F(RenderContextTest, MultipleExtentChanges) {
    context->SetExtent(0.0, 0.0, 100.0, 100.0);
    context->SetExtent(0.0, 0.0, 200.0, 200.0);
    context->SetExtent(0.0, 0.0, 300.0, 300.0);
    
    double minX, minY, maxX, maxY;
    context->GetExtent(minX, minY, maxX, maxY);
    
    EXPECT_DOUBLE_EQ(maxX, 300.0);
    EXPECT_DOUBLE_EQ(maxY, 300.0);
}

TEST_F(RenderContextTest, LargeOutputSize) {
    RenderConfig config;
    config.outputWidth = 10000;
    config.outputHeight = 10000;
    config.dpi = 300.0;
    
    MokRenderResult result = context->Initialize(config);
    EXPECT_TRUE(result.IsSuccess());
}

TEST_F(RenderContextTest, HighDPI) {
    RenderConfig config;
    config.outputWidth = 800;
    config.outputHeight = 600;
    config.dpi = 1200.0;
    
    MokRenderResult result = context->Initialize(config);
    EXPECT_TRUE(result.IsSuccess());
}
