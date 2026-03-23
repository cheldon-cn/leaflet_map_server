#include <gtest/gtest.h>
#include "ogc/draw/driver_manager.h"
#include "ogc/draw/raster_image_device.h"
#include <memory>

using namespace ogc::draw;

class DriverManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_manager = &DriverManager::Instance();
    }
    
    void TearDown() override {
        m_manager->Clear();
    }
    
    DriverManager* m_manager;
};

TEST_F(DriverManagerTest, Instance) {
    EXPECT_NE(m_manager, nullptr);
}

TEST_F(DriverManagerTest, RegisterDevice) {
    auto device = RasterImageDevice::Create(256, 256, 4);
    auto result = m_manager->RegisterDevice("test_device", device);
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(DriverManagerTest, UnregisterDevice) {
    auto device = RasterImageDevice::Create(256, 256, 4);
    m_manager->RegisterDevice("test_device", device);
    
    auto result = m_manager->UnregisterDevice("test_device");
    EXPECT_EQ(result, DrawResult::kSuccess);
    EXPECT_FALSE(m_manager->HasDevice("test_device"));
}

TEST_F(DriverManagerTest, GetDevice) {
    auto device = RasterImageDevice::Create(256, 256, 4);
    m_manager->RegisterDevice("test_device", device);
    
    auto retrieved = m_manager->GetDevice("test_device");
    EXPECT_NE(retrieved, nullptr);
}

TEST_F(DriverManagerTest, GetDeviceNames) {
    auto device1 = RasterImageDevice::Create(256, 256, 4);
    auto device2 = RasterImageDevice::Create(512, 512, 4);
    
    m_manager->RegisterDevice("device1", device1);
    m_manager->RegisterDevice("device2", device2);
    
    auto names = m_manager->GetDeviceNames();
    EXPECT_EQ(names.size(), 2);
}

TEST_F(DriverManagerTest, HasDevice) {
    auto device = RasterImageDevice::Create(256, 256, 4);
    m_manager->RegisterDevice("test_device", device);
    
    EXPECT_TRUE(m_manager->HasDevice("test_device"));
    EXPECT_FALSE(m_manager->HasDevice("nonexistent"));
}

TEST_F(DriverManagerTest, Clear) {
    auto device = RasterImageDevice::Create(256, 256, 4);
    
    m_manager->RegisterDevice("device", device);
    
    m_manager->Clear();
    
    EXPECT_EQ(m_manager->GetDeviceCount(), 0);
}

TEST_F(DriverManagerTest, GetCounts) {
    auto device = RasterImageDevice::Create(256, 256, 4);
    
    m_manager->RegisterDevice("device", device);
    
    EXPECT_EQ(m_manager->GetDeviceCount(), 1);
}

TEST_F(DriverManagerTest, GetDriverCount) {
    EXPECT_EQ(m_manager->GetDriverCount(), 0);
}
