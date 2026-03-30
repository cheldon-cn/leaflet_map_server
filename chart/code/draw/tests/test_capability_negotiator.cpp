#include <gtest/gtest.h>
#include "ogc/draw/capability_negotiator.h"
#include "ogc/draw/raster_image_device.h"

using namespace ogc::draw;

class CapabilityNegotiatorTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_caps = DeviceCapabilities();
        m_caps.supportsGPU = true;
        m_caps.supportsMultithreading = true;
        m_caps.supportsAlpha = true;
        m_caps.supportsAntialiasing = true;
        m_caps.maxTextureSize = 4096;
        m_caps.maxRenderTargets = 8;
        m_negotiator.SetCapabilities(m_caps);
    }
    
    DeviceCapabilities m_caps;
    CapabilityNegotiator m_negotiator;
};

TEST_F(CapabilityNegotiatorTest, CreateDefault) {
    CapabilityNegotiator negotiator;
    auto caps = negotiator.GetCapabilities();
    EXPECT_FALSE(caps.supportsGPU);
}

TEST_F(CapabilityNegotiatorTest, CreateWithCapabilities) {
    DeviceCapabilities caps;
    caps.supportsGPU = true;
    caps.maxTextureSize = 8192;
    
    CapabilityNegotiator negotiator(caps);
    EXPECT_TRUE(negotiator.SupportsGPU());
    EXPECT_EQ(negotiator.GetMaxTextureSize(), 8192);
}

TEST_F(CapabilityNegotiatorTest, SetCapabilities) {
    DeviceCapabilities caps;
    caps.supportsGPU = false;
    caps.maxTextureSize = 2048;
    
    m_negotiator.SetCapabilities(caps);
    EXPECT_FALSE(m_negotiator.SupportsGPU());
    EXPECT_EQ(m_negotiator.GetMaxTextureSize(), 2048);
}

TEST_F(CapabilityNegotiatorTest, NegotiateStyleNoDowngrade) {
    DrawStyle requested;
    requested.opacity = 0.5;
    requested.antialias = true;
    
    DrawStyle negotiated = m_negotiator.NegotiateStyle(requested);
    EXPECT_DOUBLE_EQ(negotiated.opacity, 0.5);
    EXPECT_TRUE(negotiated.antialias);
}

TEST_F(CapabilityNegotiatorTest, NegotiateStyleDowngradeTransparency) {
    m_caps.supportsAlpha = false;
    m_negotiator.SetCapabilities(m_caps);
    
    DrawStyle requested;
    requested.opacity = 0.5;
    requested.pen.color.a = 128;
    requested.brush.color.a = 200;
    
    DrawStyle negotiated = m_negotiator.NegotiateStyle(requested);
    EXPECT_DOUBLE_EQ(negotiated.opacity, 1.0);
    EXPECT_EQ(negotiated.pen.color.a, 255);
    EXPECT_EQ(negotiated.brush.color.a, 255);
}

TEST_F(CapabilityNegotiatorTest, NegotiateStyleDowngradeAntialiasing) {
    m_caps.supportsAntialiasing = false;
    m_negotiator.SetCapabilities(m_caps);
    
    DrawStyle requested;
    requested.antialias = true;
    
    DrawStyle negotiated = m_negotiator.NegotiateStyle(requested);
    EXPECT_FALSE(negotiated.antialias);
}

TEST_F(CapabilityNegotiatorTest, NegotiateFont) {
    Font requested;
    requested.family = "Arial";
    requested.size = 14.0;
    
    Font negotiated = m_negotiator.NegotiateFont(requested);
    EXPECT_EQ(negotiated.family, "Arial");
    EXPECT_DOUBLE_EQ(negotiated.size, 14.0);
}

TEST_F(CapabilityNegotiatorTest, IsFeatureSupported) {
    EXPECT_TRUE(m_negotiator.IsFeatureSupported("gpu"));
    EXPECT_TRUE(m_negotiator.IsFeatureSupported("multithreading"));
    EXPECT_TRUE(m_negotiator.IsFeatureSupported("alpha"));
    EXPECT_TRUE(m_negotiator.IsFeatureSupported("antialiasing"));
    EXPECT_FALSE(m_negotiator.IsFeatureSupported("unknown_feature"));
}

TEST_F(CapabilityNegotiatorTest, SupportsTransparency) {
    EXPECT_TRUE(m_negotiator.SupportsTransparency());
    
    m_caps.supportsAlpha = false;
    m_negotiator.SetCapabilities(m_caps);
    EXPECT_FALSE(m_negotiator.SupportsTransparency());
}

TEST_F(CapabilityNegotiatorTest, SupportsAntialiasing) {
    EXPECT_TRUE(m_negotiator.SupportsAntialiasing());
    
    m_caps.supportsAntialiasing = false;
    m_negotiator.SetCapabilities(m_caps);
    EXPECT_FALSE(m_negotiator.SupportsAntialiasing());
}

TEST_F(CapabilityNegotiatorTest, SupportsGPU) {
    EXPECT_TRUE(m_negotiator.SupportsGPU());
    
    m_caps.supportsGPU = false;
    m_negotiator.SetCapabilities(m_caps);
    EXPECT_FALSE(m_negotiator.SupportsGPU());
}

TEST_F(CapabilityNegotiatorTest, SupportsMultithreading) {
    EXPECT_TRUE(m_negotiator.SupportsMultithreading());
    
    m_caps.supportsMultithreading = false;
    m_negotiator.SetCapabilities(m_caps);
    EXPECT_FALSE(m_negotiator.SupportsMultithreading());
}

TEST_F(CapabilityNegotiatorTest, SupportsPartialUpdate) {
    m_caps.supportsPartialUpdate = true;
    m_negotiator.SetCapabilities(m_caps);
    EXPECT_TRUE(m_negotiator.SupportsPartialUpdate());
}

TEST_F(CapabilityNegotiatorTest, GetMaxTextureSize) {
    EXPECT_EQ(m_negotiator.GetMaxTextureSize(), 4096);
    
    m_caps.maxTextureSize = 8192;
    m_negotiator.SetCapabilities(m_caps);
    EXPECT_EQ(m_negotiator.GetMaxTextureSize(), 8192);
}

TEST_F(CapabilityNegotiatorTest, GetMaxRenderTargets) {
    EXPECT_EQ(m_negotiator.GetMaxRenderTargets(), 8);
    
    m_caps.maxRenderTargets = 4;
    m_negotiator.SetCapabilities(m_caps);
    EXPECT_EQ(m_negotiator.GetMaxRenderTargets(), 4);
}

TEST_F(CapabilityNegotiatorTest, GetUnsupportedFeatures) {
    DrawStyle style;
    style.opacity = 0.5;
    style.antialias = true;
    
    auto unsupported = m_negotiator.GetUnsupportedFeatures(style);
    EXPECT_TRUE(unsupported.empty());
}

TEST_F(CapabilityNegotiatorTest, GetUnsupportedFeaturesWithLimitations) {
    m_caps.supportsAlpha = false;
    m_caps.supportsAntialiasing = false;
    m_negotiator.SetCapabilities(m_caps);
    
    DrawStyle style;
    style.opacity = 0.5;
    style.antialias = true;
    
    auto unsupported = m_negotiator.GetUnsupportedFeatures(style);
    EXPECT_EQ(unsupported.size(), 2);
}

TEST_F(CapabilityNegotiatorTest, GetNegotiationReport) {
    DrawStyle style;
    style.opacity = 0.8;
    style.antialias = true;
    
    std::string report = m_negotiator.GetNegotiationReport(style);
    EXPECT_FALSE(report.empty());
    EXPECT_TRUE(report.find("Capability Negotiation Report") != std::string::npos);
}

TEST_F(CapabilityNegotiatorTest, MoveConstructor) {
    CapabilityNegotiator original(m_caps);
    CapabilityNegotiator moved(std::move(original));
    
    EXPECT_TRUE(moved.SupportsGPU());
    EXPECT_EQ(moved.GetMaxTextureSize(), 4096);
}

TEST_F(CapabilityNegotiatorTest, MoveAssignment) {
    CapabilityNegotiator original(m_caps);
    CapabilityNegotiator assigned;
    
    assigned = std::move(original);
    EXPECT_TRUE(assigned.SupportsGPU());
}

class DeviceRecoveryTest : public ::testing::Test {
protected:
    void SetUp() override {
        DeviceRecovery::SetMaxRecoveryAttempts(3);
        DeviceRecovery::SetRecoveryDelayMs(100);
        DeviceRecovery::ClearRecoveryCallbacks();
    }
    
    void TearDown() override {
        DeviceRecovery::ClearRecoveryCallbacks();
    }
};

TEST_F(DeviceRecoveryTest, SetMaxRecoveryAttempts) {
    DeviceRecovery::SetMaxRecoveryAttempts(5);
    EXPECT_EQ(DeviceRecovery::GetMaxRecoveryAttempts(), 5);
    
    DeviceRecovery::SetMaxRecoveryAttempts(0);
    EXPECT_EQ(DeviceRecovery::GetMaxRecoveryAttempts(), 3);
}

TEST_F(DeviceRecoveryTest, SetRecoveryDelayMs) {
    DeviceRecovery::SetRecoveryDelayMs(200);
    EXPECT_EQ(DeviceRecovery::GetRecoveryDelayMs(), 200);
    
    DeviceRecovery::SetRecoveryDelayMs(0);
    EXPECT_EQ(DeviceRecovery::GetRecoveryDelayMs(), 100);
}

TEST_F(DeviceRecoveryTest, RegisterRecoveryCallback) {
    int callbackCount = 0;
    DeviceRecovery::RegisterRecoveryCallback([&callbackCount]() {
        callbackCount++;
    });
    
    DeviceRecovery::RegisterRecoveryCallback([&callbackCount]() {
        callbackCount += 10;
    });
    
    DeviceRecovery::ClearRecoveryCallbacks();
}

TEST_F(DeviceRecoveryTest, HandleDeviceLostNullDevice) {
    auto result = DeviceRecovery::HandleDeviceLost(nullptr);
    EXPECT_EQ(result, DeviceRecovery::RecoveryResult::kUnsupportedDevice);
}

TEST_F(DeviceRecoveryTest, IsDeviceLostNullDevice) {
    EXPECT_FALSE(DeviceRecovery::IsDeviceLost(nullptr));
}

TEST_F(DeviceRecoveryTest, CanRecoverNullDevice) {
    EXPECT_FALSE(DeviceRecovery::CanRecover(nullptr));
}

class DeviceLostHandlerTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_handler = std::unique_ptr<DeviceLostHandler>(new DeviceLostHandler());
    }
    
    std::unique_ptr<DeviceLostHandler> m_handler;
};

TEST_F(DeviceLostHandlerTest, Create) {
    EXPECT_NE(m_handler.get(), nullptr);
    EXPECT_EQ(m_handler->GetDevice(), nullptr);
    EXPECT_EQ(m_handler->GetEngine(), nullptr);
    EXPECT_TRUE(m_handler->IsAutoRecoverEnabled());
}

TEST_F(DeviceLostHandlerTest, SetDevice) {
    RasterImageDevice device(800, 600, PixelFormat::kRGBA8888);
    m_handler->SetDevice(&device);
    EXPECT_EQ(m_handler->GetDevice(), &device);
}

TEST_F(DeviceLostHandlerTest, SetEngine) {
    m_handler->SetEngine(nullptr);
    EXPECT_EQ(m_handler->GetEngine(), nullptr);
}

TEST_F(DeviceLostHandlerTest, SetAutoRecover) {
    m_handler->SetAutoRecover(false);
    EXPECT_FALSE(m_handler->IsAutoRecoverEnabled());
    
    m_handler->SetAutoRecover(true);
    EXPECT_TRUE(m_handler->IsAutoRecoverEnabled());
}

TEST_F(DeviceLostHandlerTest, CheckAndRecoverNoDevice) {
    EXPECT_FALSE(m_handler->CheckAndRecover());
}

TEST_F(DeviceLostHandlerTest, RecoverNoDevice) {
    auto result = m_handler->Recover();
    EXPECT_EQ(result, DeviceRecovery::RecoveryResult::kUnsupportedDevice);
}

TEST_F(DeviceLostHandlerTest, OnDeviceLostCallback) {
    bool callbackCalled = false;
    m_handler->OnDeviceLost([&callbackCalled]() {
        callbackCalled = true;
    });
    
    m_handler->SetAutoRecover(false);
    RasterImageDevice device(800, 600, PixelFormat::kRGBA8888);
    m_handler->SetDevice(&device);
    
    EXPECT_FALSE(callbackCalled);
}

TEST_F(DeviceLostHandlerTest, OnDeviceRecoveredCallback) {
    bool callbackCalled = false;
    m_handler->OnDeviceRecovered([&callbackCalled]() {
        callbackCalled = true;
    });
    
    EXPECT_FALSE(callbackCalled);
}

TEST_F(DeviceLostHandlerTest, MoveConstructor) {
    RasterImageDevice device(800, 600, PixelFormat::kRGBA8888);
    m_handler->SetDevice(&device);
    m_handler->SetAutoRecover(false);
    
    DeviceLostHandler moved(std::move(*m_handler));
    EXPECT_EQ(moved.GetDevice(), &device);
    EXPECT_FALSE(moved.IsAutoRecoverEnabled());
}

TEST_F(DeviceLostHandlerTest, MoveAssignment) {
    RasterImageDevice device(800, 600, PixelFormat::kRGBA8888);
    m_handler->SetDevice(&device);
    m_handler->SetAutoRecover(false);
    
    DeviceLostHandler assigned;
    assigned = std::move(*m_handler);
    EXPECT_EQ(assigned.GetDevice(), &device);
    EXPECT_FALSE(assigned.IsAutoRecoverEnabled());
}
