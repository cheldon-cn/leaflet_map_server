#include <gtest/gtest.h>
#include "ogc/draw/pdf_device.h"
#include "ogc/draw/draw_engine.h"
#include <memory>

using namespace ogc::draw;

class PdfDeviceTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_device = PdfDevice::Create(800, 600);
        ASSERT_NE(m_device, nullptr);
    }
    
    void TearDown() override {
        m_device.reset();
    }
    
    PdfDevicePtr m_device;
};

TEST_F(PdfDeviceTest, Create) {
    EXPECT_NE(m_device, nullptr);
}

TEST_F(PdfDeviceTest, GetType) {
    EXPECT_EQ(m_device->GetType(), DeviceType::kPdf);
}

TEST_F(PdfDeviceTest, GetName) {
    EXPECT_EQ(m_device->GetName(), "PdfDevice");
}

TEST_F(PdfDeviceTest, Initialize) {
    auto result = m_device->Initialize();
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(PdfDeviceTest, Finalize) {
    m_device->Initialize();
    auto result = m_device->Finalize();
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(PdfDeviceTest, IsReady) {
    m_device->Initialize();
    EXPECT_TRUE(m_device->IsReady());
}

TEST_F(PdfDeviceTest, GetWidthHeight) {
    EXPECT_EQ(m_device->GetWidth(), 800);
    EXPECT_EQ(m_device->GetHeight(), 600);
}

TEST_F(PdfDeviceTest, GetDpi) {
    EXPECT_DOUBLE_EQ(m_device->GetDpi(), 72.0);
}

TEST_F(PdfDeviceTest, SetDpi) {
    m_device->SetDpi(150.0);
    EXPECT_DOUBLE_EQ(m_device->GetDpi(), 150.0);
}

TEST_F(PdfDeviceTest, GetColorDepth) {
    EXPECT_EQ(m_device->GetColorDepth(), 24);
}

TEST_F(PdfDeviceTest, SetTitle) {
    m_device->SetTitle("Test PDF");
}

TEST_F(PdfDeviceTest, SetAuthor) {
    m_device->SetAuthor("Test Author");
}

TEST_F(PdfDeviceTest, SetSubject) {
    m_device->SetSubject("Test Subject");
}

TEST_F(PdfDeviceTest, SetCreator) {
    m_device->SetCreator("Test Creator");
}

TEST_F(PdfDeviceTest, GetState) {
    auto state = m_device->GetState();
    EXPECT_NE(state, DeviceState::kError);
}

TEST_F(PdfDeviceTest, IsDeviceLost) {
    EXPECT_FALSE(m_device->IsDeviceLost());
}

TEST_F(PdfDeviceTest, RecoverDevice) {
    auto result = m_device->RecoverDevice();
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(PdfDeviceTest, GetSupportedEngineTypes) {
    auto types = m_device->GetSupportedEngineTypes();
    EXPECT_FALSE(types.empty());
}

TEST_F(PdfDeviceTest, GetPreferredEngineType) {
    auto type = m_device->GetPreferredEngineType();
    EXPECT_EQ(type, EngineType::kVector);
}

TEST_F(PdfDeviceTest, SetPreferredEngineType) {
    m_device->SetPreferredEngineType(EngineType::kSimple2D);
    EXPECT_EQ(m_device->GetPreferredEngineType(), EngineType::kSimple2D);
}

TEST_F(PdfDeviceTest, QueryCapabilities) {
    auto caps = m_device->QueryCapabilities();
    EXPECT_FALSE(caps.supportsGPU);
    EXPECT_TRUE(caps.supportsAlpha);
}

TEST_F(PdfDeviceTest, IsFeatureAvailable) {
    EXPECT_TRUE(m_device->IsFeatureAvailable("multipage"));
    EXPECT_TRUE(m_device->IsFeatureAvailable("metadata"));
    EXPECT_FALSE(m_device->IsFeatureAvailable("nonexistent"));
}

TEST_F(PdfDeviceTest, CreateEngine) {
    m_device->Initialize();
    auto engine = m_device->CreateEngine();
    EXPECT_NE(engine, nullptr);
}

TEST_F(PdfDeviceTest, NewPage) {
    m_device->Initialize();
    m_device->NewPage(800, 600);
    EXPECT_EQ(m_device->GetPageCount(), 2);
}

TEST_F(PdfDeviceTest, GetCurrentPage) {
    m_device->Initialize();
    EXPECT_EQ(m_device->GetCurrentPage(), 0);
    
    m_device->NewPage(800, 600);
    EXPECT_EQ(m_device->GetCurrentPage(), 1);
}

TEST_F(PdfDeviceTest, GetPageCount) {
    m_device->Initialize();
    EXPECT_EQ(m_device->GetPageCount(), 1);
    
    m_device->NewPage(800, 600);
    EXPECT_EQ(m_device->GetPageCount(), 2);
}

TEST_F(PdfDeviceTest, SaveToFile) {
    m_device->Initialize();
    bool result = m_device->SaveToFile("test_pdf_device_output.pdf");
    EXPECT_TRUE(result);
}

TEST_F(PdfDeviceTest, DefaultConstructor) {
    PdfDevice device;
    device.Initialize();
    EXPECT_TRUE(device.IsReady());
}

TEST_F(PdfDeviceTest, CustomSizeConstructor) {
    PdfDevice device(1024, 768, 96.0);
    device.Initialize();
    EXPECT_EQ(device.GetWidth(), 1024);
    EXPECT_EQ(device.GetHeight(), 768);
    EXPECT_DOUBLE_EQ(device.GetDpi(), 96.0);
}

TEST_F(PdfDeviceTest, MultiplePages) {
    m_device->Initialize();
    
    for (int i = 0; i < 5; ++i) {
        m_device->NewPage(800, 600);
    }
    
    EXPECT_EQ(m_device->GetPageCount(), 6);
}

TEST_F(PdfDeviceTest, Metadata) {
    m_device->Initialize();
    m_device->SetTitle("Test Title");
    m_device->SetAuthor("Test Author");
    m_device->SetSubject("Test Subject");
    m_device->SetCreator("Test Creator");
    
    bool result = m_device->SaveToFile("test_pdf_metadata.pdf");
    EXPECT_TRUE(result);
}

TEST_F(PdfDeviceTest, GetSize) {
    auto size = m_device->GetSize();
    EXPECT_DOUBLE_EQ(size.w, 800.0);
    EXPECT_DOUBLE_EQ(size.h, 600.0);
}

TEST_F(PdfDeviceTest, GetBounds) {
    auto bounds = m_device->GetBounds();
    EXPECT_DOUBLE_EQ(bounds.x, 0.0);
    EXPECT_DOUBLE_EQ(bounds.y, 0.0);
    EXPECT_DOUBLE_EQ(bounds.w, 800.0);
    EXPECT_DOUBLE_EQ(bounds.h, 600.0);
}

TEST_F(PdfDeviceTest, IsValid) {
    m_device->Initialize();
    EXPECT_TRUE(m_device->IsValid());
}
