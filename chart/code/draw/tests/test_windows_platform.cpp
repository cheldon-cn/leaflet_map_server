#include <gtest/gtest.h>
#include "ogc/draw/windows_platform.h"
#include "ogc/draw/raster_image_device.h"

#ifdef _WIN32

using namespace ogc::draw;
using namespace ogc::draw::windows;

class WindowsPlatformTest : public ::testing::Test {
protected:
    void SetUp() override {
        WindowsPlatform::Initialize();
    }
    
    void TearDown() override {
    }
};

TEST_F(WindowsPlatformTest, Initialize) {
    EXPECT_TRUE(WindowsPlatform::Initialize());
}

TEST_F(WindowsPlatformTest, IsGDIPlusAvailable) {
    EXPECT_TRUE(WindowsPlatform::IsGDIPlusAvailable());
}

TEST_F(WindowsPlatformTest, IsDirect2DAvailable) {
    EXPECT_TRUE(WindowsPlatform::IsDirect2DAvailable());
}

TEST_F(WindowsPlatformTest, GetCapabilities) {
    auto caps = WindowsPlatform::GetCapabilities();
    
    EXPECT_TRUE(caps.hasGDIPlus);
    EXPECT_TRUE(caps.hasDirect2D);
    EXPECT_TRUE(caps.hasDirectWrite);
    EXPECT_TRUE(caps.hasHardwareAcceleration);
    EXPECT_GT(caps.maxTextureSize, 0);
    EXPECT_FALSE(caps.preferredEngine.empty());
}

TEST_F(WindowsPlatformTest, GetAvailableEngines) {
    auto engines = WindowsPlatform::GetAvailableEngines();
    
    EXPECT_GE(engines.size(), 1);
    
    bool hasGDIPlus = false;
    bool hasDirect2D = false;
    
    for (const auto& engine : engines) {
        if (engine == "GDIPlusEngine") hasGDIPlus = true;
        if (engine == "Direct2DEngine") hasDirect2D = true;
    }
    
    EXPECT_TRUE(hasGDIPlus);
    EXPECT_TRUE(hasDirect2D);
}

TEST_F(WindowsPlatformTest, GetRecommendedEngine) {
    RasterImageDevice device(100, 100);
    
    std::string recommended = WindowsPlatform::GetRecommendedEngine(&device);
    EXPECT_FALSE(recommended.empty());
    
    EXPECT_TRUE(recommended == "Direct2DEngine" || recommended == "GDIPlusEngine");
}

TEST_F(WindowsPlatformTest, GetRecommendedEngineNullDevice) {
    std::string recommended = WindowsPlatform::GetRecommendedEngine(nullptr);
    EXPECT_FALSE(recommended.empty());
}

TEST_F(WindowsPlatformTest, CreateOptimalEngine) {
    RasterImageDevice device(100, 100);
    
    auto engine = WindowsPlatform::CreateOptimalEngine(&device);
    ASSERT_NE(engine, nullptr);
    
    EXPECT_TRUE(engine->GetName() == "Direct2DEngine" || 
                engine->GetName() == "GDIPlusEngine");
}

TEST_F(WindowsPlatformTest, CreateEngineDirect2D) {
    RasterImageDevice device(100, 100);
    
    auto engine = WindowsPlatform::CreateEngine(&device, "Direct2DEngine");
    ASSERT_NE(engine, nullptr);
    EXPECT_EQ(engine->GetName(), "Direct2DEngine");
}

TEST_F(WindowsPlatformTest, CreateEngineGDIPlus) {
    RasterImageDevice device(100, 100);
    
    auto engine = WindowsPlatform::CreateEngine(&device, "GDIPlusEngine");
    ASSERT_NE(engine, nullptr);
    EXPECT_EQ(engine->GetName(), "GDIPlusEngine");
}

TEST_F(WindowsPlatformTest, CreateEngineInvalidName) {
    RasterImageDevice device(100, 100);
    
    auto engine = WindowsPlatform::CreateEngine(&device, "InvalidEngine");
    EXPECT_EQ(engine, nullptr);
}

TEST_F(WindowsPlatformTest, DrawWithOptimalEngine) {
    RasterImageDevice device(200, 200);
    
    auto engine = WindowsPlatform::CreateOptimalEngine(&device);
    ASSERT_NE(engine, nullptr);
    
    EXPECT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    double x[] = {50, 100, 150};
    double y[] = {50, 100, 150};
    
    DrawStyle style;
    style.pen = Pen(Color(255, 0, 0, 255), 1.0);
    style.brush = Brush(Color(255, 0, 0, 255));
    style.pointSize = 5.0;
    
    auto result = engine->DrawPoints(x, y, 3, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->End();
}

TEST_F(WindowsPlatformTest, DrawWithDirect2DEngine) {
    RasterImageDevice device(200, 200);
    
    auto engine = WindowsPlatform::CreateEngine(&device, "Direct2DEngine");
    ASSERT_NE(engine, nullptr);
    
    EXPECT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    DrawStyle style;
    style.brush = Brush(Color(0, 0, 255, 255));
    style.pen = Pen(Color(0, 0, 0, 255), 1.0);
    
    auto result = engine->DrawRect(50, 50, 100, 100, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->End();
}

TEST_F(WindowsPlatformTest, DrawWithGDIPlusEngine) {
    RasterImageDevice device(200, 200);
    
    auto engine = WindowsPlatform::CreateEngine(&device, "GDIPlusEngine");
    ASSERT_NE(engine, nullptr);
    
    EXPECT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    DrawStyle style;
    style.brush = Brush(Color(0, 255, 0, 255));
    style.pen = Pen(Color(0, 0, 0, 255), 1.0);
    
    auto result = engine->DrawCircle(100, 100, 50, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->End();
}

TEST_F(WindowsPlatformTest, MultipleEnginesSequential) {
    RasterImageDevice device1(100, 100);
    RasterImageDevice device2(100, 100);
    
    auto engine1 = WindowsPlatform::CreateEngine(&device1, "Direct2DEngine");
    auto engine2 = WindowsPlatform::CreateEngine(&device2, "GDIPlusEngine");
    
    ASSERT_NE(engine1, nullptr);
    ASSERT_NE(engine2, nullptr);
    
    EXPECT_EQ(engine1->Begin(), DrawResult::kSuccess);
    EXPECT_EQ(engine2->Begin(), DrawResult::kSuccess);
    
    DrawStyle style;
    style.pen = Pen(Color(255, 0, 0, 255), 1.0);
    style.brush = Brush(Color(255, 0, 0, 255));
    
    double x[] = {50};
    double y[] = {50};
    
    EXPECT_EQ(engine1->DrawPoints(x, y, 1, style), DrawResult::kSuccess);
    EXPECT_EQ(engine2->DrawPoints(x, y, 1, style), DrawResult::kSuccess);
    
    engine1->End();
    engine2->End();
}

TEST_F(WindowsPlatformTest, EngineType) {
    RasterImageDevice device(100, 100);
    
    auto d2dEngine = WindowsPlatform::CreateEngine(&device, "Direct2DEngine");
    if (d2dEngine) {
        EXPECT_EQ(d2dEngine->GetType(), EngineType::kGPU);
    }
    
    auto gdiEngine = WindowsPlatform::CreateEngine(&device, "GDIPlusEngine");
    if (gdiEngine) {
        EXPECT_EQ(gdiEngine->GetType(), EngineType::kSimple2D);
    }
}

TEST_F(WindowsPlatformTest, Shutdown) {
    WindowsPlatform::Shutdown();
    
    EXPECT_TRUE(WindowsPlatform::Initialize());
}

#else

TEST(WindowsPlatformNotAvailable, Skip) {
    GTEST_SKIP() << "Windows platform not available (non-Windows), skipping WindowsPlatform tests";
}

#endif
