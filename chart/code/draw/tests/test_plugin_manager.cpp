#include <gtest/gtest.h>
#include "ogc/draw/plugin_manager.h"
#include "ogc/draw/draw_engine_plugin.h"
#include "ogc/draw/draw_device_plugin.h"
#include "ogc/draw/raster_image_device.h"

class TestEnginePlugin : public ogc::draw::DrawEnginePlugin {
public:
    std::string GetName() const override { return "TestEngine"; }
    std::string GetVersion() const override { return "1.0.0"; }
    std::string GetDescription() const override { return "Test engine plugin"; }
    ogc::draw::EngineType GetEngineType() const override { return ogc::draw::EngineType::kSimple2D; }
    
    std::unique_ptr<ogc::draw::DrawEngine> CreateEngine(ogc::draw::DrawDevice* device) override {
        (void)device;
        return nullptr;
    }
    
    bool IsAvailable() const override { return true; }
    std::vector<std::string> GetSupportedDeviceTypes() const override {
        return {"RasterImage"};
    }
};

class TestDevicePlugin : public ogc::draw::DrawDevicePlugin {
public:
    std::string GetName() const override { return "TestDevice"; }
    std::string GetVersion() const override { return "1.0.0"; }
    std::string GetDescription() const override { return "Test device plugin"; }
    ogc::draw::DeviceType GetSupportedDeviceType() const override { return ogc::draw::DeviceType::kRasterImage; }
    
    std::unique_ptr<ogc::draw::DrawDevice> CreateDevice(int width, int height) override {
        return std::unique_ptr<ogc::draw::DrawDevice>(new ogc::draw::RasterImageDevice(width, height));
    }
    
    std::unique_ptr<ogc::draw::DrawDevice> CreateDevice(void* nativeHandle, int width, int height) override {
        (void)nativeHandle;
        return std::unique_ptr<ogc::draw::DrawDevice>(new ogc::draw::RasterImageDevice(width, height));
    }
    
    bool IsAvailable() const override { return true; }
};

class PluginManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        auto& pm = ogc::draw::PluginManager::Instance();
        pm.UnloadAll();
    }
    
    void TearDown() override {
        auto& pm = ogc::draw::PluginManager::Instance();
        pm.UnloadAll();
    }
};

TEST_F(PluginManagerTest, Singleton) {
    auto& pm1 = ogc::draw::PluginManager::Instance();
    auto& pm2 = ogc::draw::PluginManager::Instance();
    EXPECT_EQ(&pm1, &pm2);
}

TEST_F(PluginManagerTest, RegisterEngineFactory) {
    auto& pm = ogc::draw::PluginManager::Instance();
    
    pm.RegisterEngineFactory("TestEngine", []() {
        return nullptr;
    });
    
    EXPECT_TRUE(pm.HasPlugin("TestEngine"));
    pm.UnregisterEngineFactory("TestEngine");
    EXPECT_FALSE(pm.HasPlugin("TestEngine"));
}

TEST_F(PluginManagerTest, RegisterDeviceFactory) {
    auto& pm = ogc::draw::PluginManager::Instance();
    
    pm.RegisterDeviceFactory("TestDevice", []() {
        return std::unique_ptr<ogc::draw::DrawDevice>(new ogc::draw::RasterImageDevice(100, 100));
    });
    
    EXPECT_TRUE(pm.HasPlugin("TestDevice"));
    
    auto device = pm.CreateDevice("TestDevice", 100, 100);
    EXPECT_NE(device, nullptr);
    
    pm.UnregisterDeviceFactory("TestDevice");
    EXPECT_FALSE(pm.HasPlugin("TestDevice"));
}

TEST_F(PluginManagerTest, GetLoadedPlugins) {
    auto& pm = ogc::draw::PluginManager::Instance();
    
    pm.RegisterEngineFactory("Engine1", []() { return nullptr; });
    pm.RegisterEngineFactory("Engine2", []() { return nullptr; });
    
    auto plugins = pm.GetLoadedPlugins();
    EXPECT_EQ(plugins.size(), 2);
    
    pm.UnloadAll();
    plugins = pm.GetLoadedPlugins();
    EXPECT_EQ(plugins.size(), 0);
}

TEST_F(PluginManagerTest, CreateDeviceFromFactory) {
    auto& pm = ogc::draw::PluginManager::Instance();
    
    pm.RegisterDeviceFactory("RasterDevice", []() {
        return std::unique_ptr<ogc::draw::DrawDevice>(new ogc::draw::RasterImageDevice(200, 200));
    });
    
    auto device = pm.CreateDevice("RasterDevice", 200, 200);
    ASSERT_NE(device, nullptr);
    EXPECT_EQ(device->GetWidth(), 200);
    EXPECT_EQ(device->GetHeight(), 200);
}

TEST_F(PluginManagerTest, PluginCount) {
    auto& pm = ogc::draw::PluginManager::Instance();
    
    EXPECT_EQ(pm.GetPluginCount(), 0);
    
    pm.RegisterEngineFactory("Engine1", []() { return nullptr; });
    EXPECT_EQ(pm.GetPluginCount(), 1);
    
    pm.RegisterDeviceFactory("Device1", []() { return nullptr; });
    EXPECT_EQ(pm.GetPluginCount(), 2);
    
    pm.UnloadAll();
    EXPECT_EQ(pm.GetPluginCount(), 0);
}

TEST_F(PluginManagerTest, EnginePluginInterface) {
    TestEnginePlugin plugin;
    
    EXPECT_EQ(plugin.GetName(), "TestEngine");
    EXPECT_EQ(plugin.GetVersion(), "1.0.0");
    EXPECT_EQ(plugin.GetDescription(), "Test engine plugin");
    EXPECT_EQ(plugin.GetEngineType(), ogc::draw::EngineType::kSimple2D);
    EXPECT_TRUE(plugin.IsAvailable());
    EXPECT_TRUE(plugin.SupportsDevice(ogc::draw::DeviceType::kRasterImage));
    EXPECT_FALSE(plugin.SupportsDevice(ogc::draw::DeviceType::kPdf));
}

TEST_F(PluginManagerTest, DevicePluginInterface) {
    TestDevicePlugin plugin;
    
    EXPECT_EQ(plugin.GetName(), "TestDevice");
    EXPECT_EQ(plugin.GetVersion(), "1.0.0");
    EXPECT_EQ(plugin.GetDescription(), "Test device plugin");
    EXPECT_EQ(plugin.GetSupportedDeviceType(), ogc::draw::DeviceType::kRasterImage);
    EXPECT_TRUE(plugin.IsAvailable());
    
    auto device = plugin.CreateDevice(100, 100);
    ASSERT_NE(device, nullptr);
    EXPECT_EQ(device->GetWidth(), 100);
    EXPECT_EQ(device->GetHeight(), 100);
}

TEST_F(PluginManagerTest, MultipleFactories) {
    auto& pm = ogc::draw::PluginManager::Instance();
    
    pm.RegisterEngineFactory("Engine1", []() { return nullptr; });
    pm.RegisterEngineFactory("Engine2", []() { return nullptr; });
    pm.RegisterDeviceFactory("Device1", []() { return nullptr; });
    pm.RegisterDeviceFactory("Device2", []() { return nullptr; });
    
    EXPECT_EQ(pm.GetPluginCount(), 4);
    
    EXPECT_TRUE(pm.HasPlugin("Engine1"));
    EXPECT_TRUE(pm.HasPlugin("Engine2"));
    EXPECT_TRUE(pm.HasPlugin("Device1"));
    EXPECT_TRUE(pm.HasPlugin("Device2"));
}

TEST_F(PluginManagerTest, UnregisterNonExistent) {
    auto& pm = ogc::draw::PluginManager::Instance();
    
    EXPECT_NO_THROW(pm.UnregisterEngineFactory("NonExistent"));
    EXPECT_NO_THROW(pm.UnregisterDeviceFactory("NonExistent"));
}

TEST_F(PluginManagerTest, CreateNonExistentDevice) {
    auto& pm = ogc::draw::PluginManager::Instance();
    
    auto device = pm.CreateDevice("NonExistent", 100, 100);
    EXPECT_EQ(device, nullptr);
}

TEST_F(PluginManagerTest, CreateNonExistentEngine) {
    auto& pm = ogc::draw::PluginManager::Instance();
    
    ogc::draw::RasterImageDevice device(100, 100);
    auto engine = pm.CreateEngine("NonExistent", &device);
    EXPECT_EQ(engine, nullptr);
}
