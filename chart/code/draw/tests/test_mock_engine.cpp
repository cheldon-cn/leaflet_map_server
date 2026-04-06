#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "test_mocks.h"
#include "ogc/draw/draw_style.h"
#include "ogc/draw/color.h"
#include "ogc/draw/font.h"
#include "ogc/draw/image.h"
#include "ogc/draw/transform_matrix.h"
#include "ogc/draw/region.h"
#include <memory>
#include <string>

using namespace ogc::draw;

class MockEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_device = std::unique_ptr<ogc::draw::testing::MockDrawDevice>(new ogc::draw::testing::MockDrawDevice());
        m_engine = std::unique_ptr<ogc::draw::testing::MockDrawEngine>(new ogc::draw::testing::MockDrawEngine());
    }

    void TearDown() override {
        m_engine.reset();
        m_device.reset();
    }

    std::unique_ptr<ogc::draw::testing::MockDrawDevice> m_device;
    std::unique_ptr<ogc::draw::testing::MockDrawEngine> m_engine;
};

TEST_F(MockEngineTest, DeviceGetType) {
    EXPECT_CALL(*m_device, GetType())
        .WillOnce(::testing::Return(DeviceType::kRasterImage));
    
    EXPECT_EQ(m_device->GetType(), DeviceType::kRasterImage);
}

TEST_F(MockEngineTest, DeviceGetSize) {
    EXPECT_CALL(*m_device, GetWidth())
        .WillOnce(::testing::Return(800));
    EXPECT_CALL(*m_device, GetHeight())
        .WillOnce(::testing::Return(600));
    
    EXPECT_EQ(m_device->GetWidth(), 800);
    EXPECT_EQ(m_device->GetHeight(), 600);
}

TEST_F(MockEngineTest, DeviceGetDpi) {
    EXPECT_CALL(*m_device, GetDpi())
        .WillOnce(::testing::Return(96.0));
    
    EXPECT_DOUBLE_EQ(m_device->GetDpi(), 96.0);
}

TEST_F(MockEngineTest, DeviceGetSupportedEngineTypes) {
    std::vector<EngineType> types = { EngineType::kSimple2D, EngineType::kGPU };
    EXPECT_CALL(*m_device, GetSupportedEngineTypes())
        .WillOnce(::testing::Return(types));
    
    auto result = m_device->GetSupportedEngineTypes();
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result[0], EngineType::kSimple2D);
    EXPECT_EQ(result[1], EngineType::kGPU);
}

TEST_F(MockEngineTest, DeviceGetPreferredEngineType) {
    EXPECT_CALL(*m_device, GetPreferredEngineType())
        .WillOnce(::testing::Return(EngineType::kGPU));
    
    EXPECT_EQ(m_device->GetPreferredEngineType(), EngineType::kGPU);
}

TEST_F(MockEngineTest, EngineBeginEnd) {
    EXPECT_CALL(*m_engine, Begin())
        .WillOnce(::testing::Return(DrawResult::kSuccess));
    EXPECT_CALL(*m_engine, End())
        .Times(1);
    
    EXPECT_EQ(m_engine->Begin(), DrawResult::kSuccess);
    m_engine->End();
}

TEST_F(MockEngineTest, EngineGetName) {
    EXPECT_CALL(*m_engine, GetName())
        .WillOnce(::testing::Return("MockEngine"));
    
    EXPECT_EQ(m_engine->GetName(), "MockEngine");
}

TEST_F(MockEngineTest, EngineGetType) {
    EXPECT_CALL(*m_engine, GetType())
        .WillOnce(::testing::Return(EngineType::kSimple2D));
    
    EXPECT_EQ(m_engine->GetType(), EngineType::kSimple2D);
}

TEST_F(MockEngineTest, EngineIsActive) {
    EXPECT_CALL(*m_engine, IsActive())
        .WillOnce(::testing::Return(false))
        .WillOnce(::testing::Return(true));
    
    EXPECT_FALSE(m_engine->IsActive());
    EXPECT_TRUE(m_engine->IsActive());
}

TEST_F(MockEngineTest, EngineDrawPoints) {
    double x[] = { 100.0, 200.0 };
    double y[] = { 100.0, 200.0 };
    DrawStyle style;
    
    EXPECT_CALL(*m_engine, DrawPoints(::testing::_, ::testing::_, 2, ::testing::_))
        .WillOnce(::testing::Return(DrawResult::kSuccess));
    
    EXPECT_EQ(m_engine->DrawPoints(x, y, 2, style), DrawResult::kSuccess);
}

TEST_F(MockEngineTest, EngineDrawLines) {
    double x1[] = { 100.0 };
    double y1[] = { 100.0 };
    double x2[] = { 200.0 };
    double y2[] = { 200.0 };
    DrawStyle style;
    
    EXPECT_CALL(*m_engine, DrawLines(::testing::_, ::testing::_, ::testing::_, ::testing::_, 1, ::testing::_))
        .WillOnce(::testing::Return(DrawResult::kSuccess));
    
    EXPECT_EQ(m_engine->DrawLines(x1, y1, x2, y2, 1, style), DrawResult::kSuccess);
}

TEST_F(MockEngineTest, EngineDrawPolygon) {
    double x[] = { 100.0, 200.0, 150.0 };
    double y[] = { 100.0, 100.0, 200.0 };
    DrawStyle style;
    
    EXPECT_CALL(*m_engine, DrawPolygon(::testing::_, ::testing::_, 3, ::testing::_, true))
        .WillOnce(::testing::Return(DrawResult::kSuccess));
    
    EXPECT_EQ(m_engine->DrawPolygon(x, y, 3, style, true), DrawResult::kSuccess);
}

TEST_F(MockEngineTest, EngineDrawText) {
    Font font("Arial", 12);
    Color color = Color::Black();
    
    EXPECT_CALL(*m_engine, DrawText(100.0, 100.0, "Test", ::testing::_, ::testing::_))
        .WillOnce(::testing::Return(DrawResult::kSuccess));
    
    EXPECT_EQ(m_engine->DrawText(100.0, 100.0, "Test", font, color), DrawResult::kSuccess);
}

TEST_F(MockEngineTest, EngineDrawImage) {
    Image img(100, 100, 4);
    
    EXPECT_CALL(*m_engine, DrawImage(100.0, 100.0, ::testing::_, 1.0, 1.0))
        .WillOnce(::testing::Return(DrawResult::kSuccess));
    
    EXPECT_EQ(m_engine->DrawImage(100.0, 100.0, img, 1.0, 1.0), DrawResult::kSuccess);
}

TEST_F(MockEngineTest, EngineTransform) {
    TransformMatrix matrix;
    matrix.ApplyTranslate(100.0, 50.0);
    
    EXPECT_CALL(*m_engine, SetTransform(::testing::_))
        .Times(1);
    EXPECT_CALL(*m_engine, GetTransform())
        .WillOnce(::testing::Return(matrix));
    EXPECT_CALL(*m_engine, ResetTransform())
        .Times(1);
    
    m_engine->SetTransform(matrix);
    EXPECT_DOUBLE_EQ(m_engine->GetTransform().GetTranslationX(), 100.0);
    m_engine->ResetTransform();
}

TEST_F(MockEngineTest, EngineClipRegion) {
    Region region;
    
    EXPECT_CALL(*m_engine, SetClipRegion(::testing::_))
        .Times(1);
    EXPECT_CALL(*m_engine, GetClipRegion())
        .WillOnce(::testing::Return(region));
    EXPECT_CALL(*m_engine, ResetClip())
        .Times(1);
    
    m_engine->SetClipRegion(region);
    m_engine->GetClipRegion();
    m_engine->ResetClip();
}

TEST_F(MockEngineTest, EngineStateManagement) {
    EXPECT_CALL(*m_engine, Save(::testing::_))
        .Times(1);
    EXPECT_CALL(*m_engine, Restore())
        .Times(1);
    
    m_engine->Save(static_cast<StateFlags>(StateFlag::kAll));
    m_engine->Restore();
}

TEST_F(MockEngineTest, EngineOpacity) {
    EXPECT_CALL(*m_engine, SetOpacity(0.5))
        .Times(1);
    EXPECT_CALL(*m_engine, GetOpacity())
        .WillOnce(::testing::Return(0.5));
    
    m_engine->SetOpacity(0.5);
    EXPECT_DOUBLE_EQ(m_engine->GetOpacity(), 0.5);
}

TEST_F(MockEngineTest, EngineTolerance) {
    EXPECT_CALL(*m_engine, SetTolerance(0.01))
        .Times(1);
    EXPECT_CALL(*m_engine, GetTolerance())
        .WillOnce(::testing::Return(0.01));
    
    m_engine->SetTolerance(0.01);
    EXPECT_DOUBLE_EQ(m_engine->GetTolerance(), 0.01);
}

TEST_F(MockEngineTest, EngineAntialiasing) {
    EXPECT_CALL(*m_engine, SetAntialiasingEnabled(true))
        .Times(1);
    EXPECT_CALL(*m_engine, IsAntialiasingEnabled())
        .WillOnce(::testing::Return(true));
    
    m_engine->SetAntialiasingEnabled(true);
    EXPECT_TRUE(m_engine->IsAntialiasingEnabled());
}

TEST_F(MockEngineTest, EngineClear) {
    Color clearColor = Color::White();
    
    EXPECT_CALL(*m_engine, Clear(::testing::_))
        .Times(1);
    
    m_engine->Clear(clearColor);
}

TEST_F(MockEngineTest, EngineFlush) {
    EXPECT_CALL(*m_engine, Flush())
        .Times(1);
    
    m_engine->Flush();
}

TEST_F(MockEngineTest, EngineMeasureText) {
    Font font("Arial", 12);
    TextMetrics metrics(50.0, 12.0);
    
    EXPECT_CALL(*m_engine, MeasureText("Test", ::testing::_))
        .WillOnce(::testing::Return(metrics));
    
    auto result = m_engine->MeasureText("Test", font);
    EXPECT_DOUBLE_EQ(result.width, 50.0);
    EXPECT_DOUBLE_EQ(result.height, 12.0);
}

TEST_F(MockEngineTest, EngineSupportsFeature) {
    EXPECT_CALL(*m_engine, SupportsFeature("GPU"))
        .WillOnce(::testing::Return(true));
    EXPECT_CALL(*m_engine, SupportsFeature("Unknown"))
        .WillOnce(::testing::Return(false));
    
    EXPECT_TRUE(m_engine->SupportsFeature("GPU"));
    EXPECT_FALSE(m_engine->SupportsFeature("Unknown"));
}

TEST_F(MockEngineTest, EngineDrawPointsError) {
    double x[] = { 100.0 };
    double y[] = { 100.0 };
    DrawStyle style;
    
    EXPECT_CALL(*m_engine, DrawPoints(::testing::_, ::testing::_, 1, ::testing::_))
        .WillOnce(::testing::Return(DrawResult::kInvalidParameter));
    
    EXPECT_EQ(m_engine->DrawPoints(x, y, 1, style), DrawResult::kInvalidParameter);
}

TEST_F(MockEngineTest, EngineBeginError) {
    EXPECT_CALL(*m_engine, Begin())
        .WillOnce(::testing::Return(DrawResult::kDeviceError));
    
    EXPECT_EQ(m_engine->Begin(), DrawResult::kDeviceError);
}

class MockPluginTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_enginePlugin = std::unique_ptr<ogc::draw::testing::MockDrawEnginePlugin>(new ogc::draw::testing::MockDrawEnginePlugin());
        m_devicePlugin = std::unique_ptr<ogc::draw::testing::MockDrawDevicePlugin>(new ogc::draw::testing::MockDrawDevicePlugin());
    }

    void TearDown() override {
        m_enginePlugin.reset();
        m_devicePlugin.reset();
    }

    std::unique_ptr<ogc::draw::testing::MockDrawEnginePlugin> m_enginePlugin;
    std::unique_ptr<ogc::draw::testing::MockDrawDevicePlugin> m_devicePlugin;
};

TEST_F(MockPluginTest, EnginePluginGetName) {
    EXPECT_CALL(*m_enginePlugin, GetName())
        .WillOnce(::testing::Return("TestEnginePlugin"));
    
    EXPECT_EQ(m_enginePlugin->GetName(), "TestEnginePlugin");
}

TEST_F(MockPluginTest, EnginePluginGetVersion) {
    EXPECT_CALL(*m_enginePlugin, GetVersion())
        .WillOnce(::testing::Return("1.0.0"));
    
    EXPECT_EQ(m_enginePlugin->GetVersion(), "1.0.0");
}

TEST_F(MockPluginTest, EnginePluginGetDescription) {
    EXPECT_CALL(*m_enginePlugin, GetDescription())
        .WillOnce(::testing::Return("Test engine plugin for unit testing"));
    
    EXPECT_EQ(m_enginePlugin->GetDescription(), "Test engine plugin for unit testing");
}

TEST_F(MockPluginTest, EnginePluginGetEngineType) {
    EXPECT_CALL(*m_enginePlugin, GetEngineType())
        .WillOnce(::testing::Return(EngineType::kSimple2D));
    
    EXPECT_EQ(m_enginePlugin->GetEngineType(), EngineType::kSimple2D);
}

TEST_F(MockPluginTest, EnginePluginIsAvailable) {
    EXPECT_CALL(*m_enginePlugin, IsAvailable())
        .WillOnce(::testing::Return(true));
    
    EXPECT_TRUE(m_enginePlugin->IsAvailable());
}

TEST_F(MockPluginTest, EnginePluginGetSupportedDeviceTypes) {
    std::vector<std::string> types = { "RasterImage", "Display" };
    EXPECT_CALL(*m_enginePlugin, GetSupportedDeviceTypes())
        .WillOnce(::testing::Return(types));
    
    auto result = m_enginePlugin->GetSupportedDeviceTypes();
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result[0], "RasterImage");
    EXPECT_EQ(result[1], "Display");
}

TEST_F(MockPluginTest, DevicePluginGetName) {
    EXPECT_CALL(*m_devicePlugin, GetName())
        .WillOnce(::testing::Return("TestDevicePlugin"));
    
    EXPECT_EQ(m_devicePlugin->GetName(), "TestDevicePlugin");
}

TEST_F(MockPluginTest, DevicePluginGetVersion) {
    EXPECT_CALL(*m_devicePlugin, GetVersion())
        .WillOnce(::testing::Return("1.0.0"));
    
    EXPECT_EQ(m_devicePlugin->GetVersion(), "1.0.0");
}

TEST_F(MockPluginTest, DevicePluginGetDescription) {
    EXPECT_CALL(*m_devicePlugin, GetDescription())
        .WillOnce(::testing::Return("Test device plugin for unit testing"));
    
    EXPECT_EQ(m_devicePlugin->GetDescription(), "Test device plugin for unit testing");
}

TEST_F(MockPluginTest, DevicePluginGetSupportedDeviceType) {
    EXPECT_CALL(*m_devicePlugin, GetSupportedDeviceType())
        .WillOnce(::testing::Return(DeviceType::kRasterImage));
    
    EXPECT_EQ(m_devicePlugin->GetSupportedDeviceType(), DeviceType::kRasterImage);
}

TEST_F(MockPluginTest, DevicePluginIsAvailable) {
    EXPECT_CALL(*m_devicePlugin, IsAvailable())
        .WillOnce(::testing::Return(true));
    
    EXPECT_TRUE(m_devicePlugin->IsAvailable());
}

class MockInteractionTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_device = new ogc::draw::testing::MockDrawDevice();
        m_engine = new ogc::draw::testing::MockDrawEngine();
    }

    void TearDown() override {
        delete m_engine;
        delete m_device;
    }

    ogc::draw::testing::MockDrawDevice* m_device;
    ogc::draw::testing::MockDrawEngine* m_engine;
};

TEST_F(MockInteractionTest, DeviceEngineInteraction) {
    EXPECT_CALL(*m_device, GetWidth())
        .WillOnce(::testing::Return(800));
    EXPECT_CALL(*m_device, GetHeight())
        .WillOnce(::testing::Return(600));
    EXPECT_CALL(*m_engine, Begin())
        .WillOnce(::testing::Return(DrawResult::kSuccess));
    EXPECT_CALL(*m_engine, DrawPoints(::testing::_, ::testing::_, 1, ::testing::_))
        .WillOnce(::testing::Return(DrawResult::kSuccess));
    EXPECT_CALL(*m_engine, End())
        .Times(1);
    
    EXPECT_EQ(m_device->GetWidth(), 800);
    EXPECT_EQ(m_device->GetHeight(), 600);
    EXPECT_EQ(m_engine->Begin(), DrawResult::kSuccess);
    
    double x[] = { 100.0 };
    double y[] = { 100.0 };
    DrawStyle style;
    EXPECT_EQ(m_engine->DrawPoints(x, y, 1, style), DrawResult::kSuccess);
    
    m_engine->End();
}

TEST_F(MockInteractionTest, MultipleDrawCalls) {
    EXPECT_CALL(*m_engine, Begin())
        .WillOnce(::testing::Return(DrawResult::kSuccess));
    EXPECT_CALL(*m_engine, DrawPoints(::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .Times(3)
        .WillRepeatedly(::testing::Return(DrawResult::kSuccess));
    EXPECT_CALL(*m_engine, DrawLines(::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .Times(2)
        .WillRepeatedly(::testing::Return(DrawResult::kSuccess));
    EXPECT_CALL(*m_engine, End())
        .Times(1);
    
    EXPECT_EQ(m_engine->Begin(), DrawResult::kSuccess);
    
    double x[] = { 100.0 };
    double y[] = { 100.0 };
    DrawStyle style;
    
    m_engine->DrawPoints(x, y, 1, style);
    m_engine->DrawPoints(x, y, 1, style);
    m_engine->DrawPoints(x, y, 1, style);
    
    m_engine->DrawLines(x, y, x, y, 1, style);
    m_engine->DrawLines(x, y, x, y, 1, style);
    
    m_engine->End();
}

TEST_F(MockInteractionTest, StateStackOperations) {
    EXPECT_CALL(*m_engine, Begin())
        .WillOnce(::testing::Return(DrawResult::kSuccess));
    EXPECT_CALL(*m_engine, Save(::testing::_))
        .Times(3);
    EXPECT_CALL(*m_engine, Restore())
        .Times(3);
    EXPECT_CALL(*m_engine, End())
        .Times(1);
    
    EXPECT_EQ(m_engine->Begin(), DrawResult::kSuccess);
    
    m_engine->Save(static_cast<StateFlags>(StateFlag::kAll));
    m_engine->Save(static_cast<StateFlags>(StateFlag::kAll));
    m_engine->Save(static_cast<StateFlags>(StateFlag::kAll));
    
    m_engine->Restore();
    m_engine->Restore();
    m_engine->Restore();
    
    m_engine->End();
}

TEST_F(MockInteractionTest, TransformSequence) {
    EXPECT_CALL(*m_engine, Begin())
        .WillOnce(::testing::Return(DrawResult::kSuccess));
    EXPECT_CALL(*m_engine, SetTransform(::testing::_))
        .Times(2);
    EXPECT_CALL(*m_engine, ResetTransform())
        .Times(1);
    EXPECT_CALL(*m_engine, End())
        .Times(1);
    
    EXPECT_EQ(m_engine->Begin(), DrawResult::kSuccess);
    
    TransformMatrix m1, m2;
    m_engine->SetTransform(m1);
    m_engine->SetTransform(m2);
    m_engine->ResetTransform();
    
    m_engine->End();
}

TEST_F(MockInteractionTest, ErrorHandling) {
    EXPECT_CALL(*m_engine, Begin())
        .WillOnce(::testing::Return(DrawResult::kDeviceError));
    
    EXPECT_EQ(m_engine->Begin(), DrawResult::kDeviceError);
}

TEST_F(MockInteractionTest, DeviceCapabilities) {
    DeviceCapabilities caps;
    caps.supportsGPU = true;
    caps.supportsMultithreading = true;
    caps.maxTextureSize = 4096;
    
    EXPECT_CALL(*m_device, QueryCapabilities())
        .WillOnce(::testing::Return(caps));
    
    auto result = m_device->QueryCapabilities();
    EXPECT_TRUE(result.supportsGPU);
    EXPECT_TRUE(result.supportsMultithreading);
    EXPECT_EQ(result.maxTextureSize, 4096);
}
