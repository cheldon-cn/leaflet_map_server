#ifndef OGC_DRAW_GPU_DEVICE_SELECTOR_H
#define OGC_DRAW_GPU_DEVICE_SELECTOR_H

#include <ogc/draw/export.h>
#include <string>
#include <vector>
#include <functional>

namespace ogc {
namespace draw {

struct GPUDeviceInfo {
    int index = -1;
    std::string name;
    std::string vendor;
    size_t dedicatedMemory = 0;
    size_t sharedMemory = 0;
    size_t totalMemory = 0;
    int computeUnits = 0;
    int clockFrequency = 0;
    bool isIntegrated = false;
    bool isDiscrete = false;
    bool supportsCompute = false;
    bool supportsRayTracing = false;
    bool supportsRender = true;
    int maxTextureSize = 16384;
    int maxRenderTargets = 8;
    
    GPUDeviceInfo() = default;
    
    bool IsValid() const { return index >= 0 && !name.empty(); }
    size_t GetTotalMemory() const { return dedicatedMemory + sharedMemory; }
};

enum class GPUPreference {
    kAuto = 0,
    kDiscrete = 1,
    kIntegrated = 2,
    kHighMemory = 3,
    kLowPower = 4
};

class OGC_DRAW_API GPUDeviceSelector {
public:
    using DeviceFilter = std::function<bool(const GPUDeviceInfo&)>;
    
    static std::vector<GPUDeviceInfo> EnumerateGPUs();
    static GPUDeviceInfo SelectBestGPU(GPUPreference preference = GPUPreference::kAuto);
    static GPUDeviceInfo SelectBestGPU(DeviceFilter filter);
    
    static int CalculateGPUScore(const GPUDeviceInfo& gpu);
    static int CalculateGPUScore(const GPUDeviceInfo& gpu, GPUPreference preference);
    
    static GPUDeviceInfo GetGPUByIndex(int index);
    static GPUDeviceInfo GetGPUByName(const std::string& name);
    
    static int GetGPUCount();
    static bool HasGPU();
    static bool HasDiscreteGPU();
    static bool HasIntegratedGPU();
    
    static void SetPreferredGPU(int index);
    static int GetPreferredGPUIndex();
    static GPUDeviceInfo GetPreferredGPU();
    
    static void SetPreference(GPUPreference preference);
    static GPUPreference GetPreference();
    
    static std::string GPUInfoToString(const GPUDeviceInfo& gpu);
    static void PrintGPUInfo(const GPUDeviceInfo& gpu);
    
    static std::string PreferenceToString(GPUPreference preference);
    static GPUPreference StringToPreference(const std::string& str);
    
    static void SetEnableLogging(bool enable);
    static bool IsLoggingEnabled();

private:
    static GPUDeviceInfo SelectByScore(const std::vector<GPUDeviceInfo>& gpus);
    static GPUDeviceInfo SelectDiscrete(const std::vector<GPUDeviceInfo>& gpus);
    static GPUDeviceInfo SelectIntegrated(const std::vector<GPUDeviceInfo>& gpus);
    static GPUDeviceInfo SelectHighMemory(const std::vector<GPUDeviceInfo>& gpus);
    static GPUDeviceInfo SelectLowPower(const std::vector<GPUDeviceInfo>& gpus);
    
    static void LogDeviceList(const std::vector<GPUDeviceInfo>& gpus);
    static void LogSelection(const GPUDeviceInfo& gpu, GPUPreference preference);
    
    static int s_preferredGPUIndex;
    static GPUPreference s_preference;
    static bool s_enableLogging;
};

class OGC_DRAW_API GPUDeviceManager {
public:
    static GPUDeviceManager& Instance();
    
    void Initialize();
    void Shutdown();
    bool IsInitialized() const;
    
    void RefreshDevices();
    std::vector<GPUDeviceInfo> GetDevices() const;
    GPUDeviceInfo GetCurrentDevice() const;
    GPUDeviceInfo SelectDevice(GPUPreference preference);
    
    void SetDeviceChangeCallback(std::function<void(int, int)> callback);
    
private:
    GPUDeviceManager();
    ~GPUDeviceManager();
    
    GPUDeviceManager(const GPUDeviceManager&) = delete;
    GPUDeviceManager& operator=(const GPUDeviceManager&) = delete;
    
    std::vector<GPUDeviceInfo> m_devices;
    int m_currentDeviceIndex;
    bool m_initialized;
    std::function<void(int, int)> m_deviceChangeCallback;
};

}
}

#endif
