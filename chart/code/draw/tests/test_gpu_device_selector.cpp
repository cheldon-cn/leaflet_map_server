#include <gtest/gtest.h>
#include "ogc/draw/gpu_device_selector.h"

using namespace ogc::draw;

class GPUDeviceSelectorTest : public ::testing::Test {
protected:
    void SetUp() override {
        GPUDeviceSelector::SetPreferredGPU(-1);
        GPUDeviceSelector::SetPreference(GPUPreference::kAuto);
        GPUDeviceSelector::SetEnableLogging(false);
    }
    
    void TearDown() override {
        GPUDeviceSelector::SetPreferredGPU(-1);
        GPUDeviceSelector::SetPreference(GPUPreference::kAuto);
    }
};

TEST_F(GPUDeviceSelectorTest, EnumerateGPUs) {
    auto gpus = GPUDeviceSelector::EnumerateGPUs();
    EXPECT_FALSE(gpus.empty());
}

TEST_F(GPUDeviceSelectorTest, SelectBestGPUAuto) {
    auto gpu = GPUDeviceSelector::SelectBestGPU(GPUPreference::kAuto);
    EXPECT_TRUE(gpu.IsValid());
}

TEST_F(GPUDeviceSelectorTest, SelectBestGPUDiscrete) {
    auto gpu = GPUDeviceSelector::SelectBestGPU(GPUPreference::kDiscrete);
    EXPECT_TRUE(gpu.IsValid());
}

TEST_F(GPUDeviceSelectorTest, SelectBestGPUIntegrated) {
    auto gpu = GPUDeviceSelector::SelectBestGPU(GPUPreference::kIntegrated);
    EXPECT_TRUE(gpu.IsValid());
}

TEST_F(GPUDeviceSelectorTest, SelectBestGPUHighMemory) {
    auto gpu = GPUDeviceSelector::SelectBestGPU(GPUPreference::kHighMemory);
    EXPECT_TRUE(gpu.IsValid());
}

TEST_F(GPUDeviceSelectorTest, SelectBestGPULowPower) {
    auto gpu = GPUDeviceSelector::SelectBestGPU(GPUPreference::kLowPower);
    EXPECT_TRUE(gpu.IsValid());
}

TEST_F(GPUDeviceSelectorTest, SelectBestGPUWithFilter) {
    auto gpu = GPUDeviceSelector::SelectBestGPU([](const GPUDeviceInfo& info) {
        return info.supportsRender;
    });
    EXPECT_TRUE(gpu.IsValid());
}

TEST_F(GPUDeviceSelectorTest, CalculateGPUScore) {
    GPUDeviceInfo gpu;
    gpu.index = 0;
    gpu.name = "Test GPU";
    gpu.dedicatedMemory = 1024 * 1024 * 1024;
    gpu.sharedMemory = 512 * 1024 * 1024;
    gpu.computeUnits = 16;
    gpu.clockFrequency = 1500;
    gpu.isDiscrete = true;
    gpu.supportsCompute = true;
    gpu.supportsRayTracing = true;
    gpu.supportsRender = true;
    gpu.maxTextureSize = 16384;
    gpu.maxRenderTargets = 8;
    
    int score = GPUDeviceSelector::CalculateGPUScore(gpu);
    EXPECT_GT(score, 0);
}

TEST_F(GPUDeviceSelectorTest, CalculateGPUScoreWithPreference) {
    GPUDeviceInfo gpu;
    gpu.index = 0;
    gpu.name = "Test GPU";
    gpu.dedicatedMemory = 1024 * 1024 * 1024;
    gpu.isDiscrete = true;
    
    int autoScore = GPUDeviceSelector::CalculateGPUScore(gpu, GPUPreference::kAuto);
    int discreteScore = GPUDeviceSelector::CalculateGPUScore(gpu, GPUPreference::kDiscrete);
    int highMemoryScore = GPUDeviceSelector::CalculateGPUScore(gpu, GPUPreference::kHighMemory);
    
    EXPECT_GT(discreteScore, autoScore);
    EXPECT_GT(highMemoryScore, 0);
}

TEST_F(GPUDeviceSelectorTest, GetGPUByIndex) {
    auto gpus = GPUDeviceSelector::EnumerateGPUs();
    if (!gpus.empty()) {
        int index = gpus[0].index;
        auto gpu = GPUDeviceSelector::GetGPUByIndex(index);
        EXPECT_TRUE(gpu.IsValid());
        EXPECT_EQ(gpu.index, index);
    }
}

TEST_F(GPUDeviceSelectorTest, GetGPUByIndexInvalid) {
    auto gpu = GPUDeviceSelector::GetGPUByIndex(-999);
    EXPECT_FALSE(gpu.IsValid());
}

TEST_F(GPUDeviceSelectorTest, GetGPUByName) {
    auto gpus = GPUDeviceSelector::EnumerateGPUs();
    if (!gpus.empty()) {
        std::string name = gpus[0].name;
        auto gpu = GPUDeviceSelector::GetGPUByName(name);
        EXPECT_TRUE(gpu.IsValid());
        EXPECT_EQ(gpu.name, name);
    }
}

TEST_F(GPUDeviceSelectorTest, GetGPUByNameInvalid) {
    auto gpu = GPUDeviceSelector::GetGPUByName("NonExistentGPU");
    EXPECT_FALSE(gpu.IsValid());
}

TEST_F(GPUDeviceSelectorTest, GetGPUCount) {
    int count = GPUDeviceSelector::GetGPUCount();
    EXPECT_GT(count, 0);
}

TEST_F(GPUDeviceSelectorTest, HasGPU) {
    EXPECT_TRUE(GPUDeviceSelector::HasGPU());
}

TEST_F(GPUDeviceSelectorTest, HasDiscreteGPU) {
    bool hasDiscrete = GPUDeviceSelector::HasDiscreteGPU();
    (void)hasDiscrete;
}

TEST_F(GPUDeviceSelectorTest, HasIntegratedGPU) {
    bool hasIntegrated = GPUDeviceSelector::HasIntegratedGPU();
    (void)hasIntegrated;
}

TEST_F(GPUDeviceSelectorTest, SetPreferredGPU) {
    GPUDeviceSelector::SetPreferredGPU(0);
    EXPECT_EQ(GPUDeviceSelector::GetPreferredGPUIndex(), 0);
}

TEST_F(GPUDeviceSelectorTest, GetPreferredGPU) {
    GPUDeviceSelector::SetPreferredGPU(-1);
    auto gpu = GPUDeviceSelector::GetPreferredGPU();
    EXPECT_TRUE(gpu.IsValid());
}

TEST_F(GPUDeviceSelectorTest, SetPreference) {
    GPUDeviceSelector::SetPreference(GPUPreference::kDiscrete);
    EXPECT_EQ(GPUDeviceSelector::GetPreference(), GPUPreference::kDiscrete);
}

TEST_F(GPUDeviceSelectorTest, GPUInfoToString) {
    GPUDeviceInfo gpu;
    gpu.index = 0;
    gpu.name = "Test GPU";
    gpu.vendor = "Test Vendor";
    gpu.dedicatedMemory = 1024 * 1024 * 1024;
    
    std::string info = GPUDeviceSelector::GPUInfoToString(gpu);
    EXPECT_FALSE(info.empty());
    EXPECT_TRUE(info.find("Test GPU") != std::string::npos);
    EXPECT_TRUE(info.find("Test Vendor") != std::string::npos);
}

TEST_F(GPUDeviceSelectorTest, PreferenceToString) {
    EXPECT_EQ(GPUDeviceSelector::PreferenceToString(GPUPreference::kAuto), "Auto");
    EXPECT_EQ(GPUDeviceSelector::PreferenceToString(GPUPreference::kDiscrete), "Discrete");
    EXPECT_EQ(GPUDeviceSelector::PreferenceToString(GPUPreference::kIntegrated), "Integrated");
    EXPECT_EQ(GPUDeviceSelector::PreferenceToString(GPUPreference::kHighMemory), "HighMemory");
    EXPECT_EQ(GPUDeviceSelector::PreferenceToString(GPUPreference::kLowPower), "LowPower");
}

TEST_F(GPUDeviceSelectorTest, StringToPreference) {
    EXPECT_EQ(GPUDeviceSelector::StringToPreference("Auto"), GPUPreference::kAuto);
    EXPECT_EQ(GPUDeviceSelector::StringToPreference("Discrete"), GPUPreference::kDiscrete);
    EXPECT_EQ(GPUDeviceSelector::StringToPreference("Integrated"), GPUPreference::kIntegrated);
    EXPECT_EQ(GPUDeviceSelector::StringToPreference("HighMemory"), GPUPreference::kHighMemory);
    EXPECT_EQ(GPUDeviceSelector::StringToPreference("LowPower"), GPUPreference::kLowPower);
    EXPECT_EQ(GPUDeviceSelector::StringToPreference("Unknown"), GPUPreference::kAuto);
}

TEST_F(GPUDeviceSelectorTest, SetEnableLogging) {
    GPUDeviceSelector::SetEnableLogging(true);
    EXPECT_TRUE(GPUDeviceSelector::IsLoggingEnabled());
    
    GPUDeviceSelector::SetEnableLogging(false);
    EXPECT_FALSE(GPUDeviceSelector::IsLoggingEnabled());
}

TEST_F(GPUDeviceSelectorTest, GPUDeviceInfoIsValid) {
    GPUDeviceInfo valid;
    valid.index = 0;
    valid.name = "Test";
    EXPECT_TRUE(valid.IsValid());
    
    GPUDeviceInfo invalid;
    invalid.index = -1;
    EXPECT_FALSE(invalid.IsValid());
}

TEST_F(GPUDeviceSelectorTest, GPUDeviceInfoGetTotalMemory) {
    GPUDeviceInfo gpu;
    gpu.dedicatedMemory = 1024;
    gpu.sharedMemory = 512;
    EXPECT_EQ(gpu.GetTotalMemory(), 1536);
}

class GPUDeviceManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        GPUDeviceManager::Instance().Shutdown();
    }
    
    void TearDown() override {
        GPUDeviceManager::Instance().Shutdown();
    }
};

TEST_F(GPUDeviceManagerTest, Instance) {
    auto& instance = GPUDeviceManager::Instance();
    EXPECT_NE(&instance, nullptr);
}

TEST_F(GPUDeviceManagerTest, Initialize) {
    auto& manager = GPUDeviceManager::Instance();
    manager.Initialize();
    EXPECT_TRUE(manager.IsInitialized());
}

TEST_F(GPUDeviceManagerTest, Shutdown) {
    auto& manager = GPUDeviceManager::Instance();
    manager.Initialize();
    manager.Shutdown();
    EXPECT_FALSE(manager.IsInitialized());
}

TEST_F(GPUDeviceManagerTest, RefreshDevices) {
    auto& manager = GPUDeviceManager::Instance();
    manager.Initialize();
    manager.RefreshDevices();
    auto devices = manager.GetDevices();
    EXPECT_FALSE(devices.empty());
}

TEST_F(GPUDeviceManagerTest, GetDevices) {
    auto& manager = GPUDeviceManager::Instance();
    manager.Initialize();
    auto devices = manager.GetDevices();
    EXPECT_FALSE(devices.empty());
}

TEST_F(GPUDeviceManagerTest, GetCurrentDevice) {
    auto& manager = GPUDeviceManager::Instance();
    manager.Initialize();
    auto device = manager.GetCurrentDevice();
    EXPECT_TRUE(device.IsValid());
}

TEST_F(GPUDeviceManagerTest, SelectDevice) {
    auto& manager = GPUDeviceManager::Instance();
    manager.Initialize();
    auto device = manager.SelectDevice(GPUPreference::kAuto);
    EXPECT_TRUE(device.IsValid());
}

TEST_F(GPUDeviceManagerTest, SetDeviceChangeCallback) {
    auto& manager = GPUDeviceManager::Instance();
    manager.Initialize();
    
    bool callbackCalled = false;
    manager.SetDeviceChangeCallback([&callbackCalled](int, int) {
        callbackCalled = true;
    });
    
    EXPECT_FALSE(callbackCalled);
}
