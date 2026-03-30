#include "ogc/draw/gpu_device_selector.h"
#include <algorithm>
#include <sstream>
#include <iostream>

namespace ogc {
namespace draw {

int GPUDeviceSelector::s_preferredGPUIndex = -1;
GPUPreference GPUDeviceSelector::s_preference = GPUPreference::kAuto;
bool GPUDeviceSelector::s_enableLogging = false;

std::vector<GPUDeviceInfo> GPUDeviceSelector::EnumerateGPUs() {
    std::vector<GPUDeviceInfo> gpus;
    
    GPUDeviceInfo defaultGPU;
    defaultGPU.index = 0;
    defaultGPU.name = "Default GPU";
    defaultGPU.vendor = "Unknown";
    defaultGPU.dedicatedMemory = 1024 * 1024 * 1024;
    defaultGPU.sharedMemory = 512 * 1024 * 1024;
    defaultGPU.totalMemory = defaultGPU.dedicatedMemory + defaultGPU.sharedMemory;
    defaultGPU.computeUnits = 8;
    defaultGPU.clockFrequency = 1000;
    defaultGPU.isDiscrete = true;
    defaultGPU.supportsCompute = true;
    defaultGPU.supportsRender = true;
    defaultGPU.maxTextureSize = 16384;
    defaultGPU.maxRenderTargets = 8;
    gpus.push_back(defaultGPU);
    
    if (s_enableLogging) {
        LogDeviceList(gpus);
    }
    
    return gpus;
}

GPUDeviceInfo GPUDeviceSelector::SelectBestGPU(GPUPreference preference) {
    GPUPreference effectivePreference = (preference == GPUPreference::kAuto && s_preference != GPUPreference::kAuto) 
        ? s_preference : preference;
    
    if (s_preferredGPUIndex >= 0) {
        GPUDeviceInfo preferred = GetGPUByIndex(s_preferredGPUIndex);
        if (preferred.IsValid()) {
            if (s_enableLogging) {
                LogSelection(preferred, effectivePreference);
            }
            return preferred;
        }
    }
    
    std::vector<GPUDeviceInfo> gpus = EnumerateGPUs();
    if (gpus.empty()) {
        return GPUDeviceInfo();
    }
    
    GPUDeviceInfo selected;
    switch (effectivePreference) {
        case GPUPreference::kDiscrete:
            selected = SelectDiscrete(gpus);
            break;
        case GPUPreference::kIntegrated:
            selected = SelectIntegrated(gpus);
            break;
        case GPUPreference::kHighMemory:
            selected = SelectHighMemory(gpus);
            break;
        case GPUPreference::kLowPower:
            selected = SelectLowPower(gpus);
            break;
        case GPUPreference::kAuto:
        default:
            selected = SelectByScore(gpus);
            break;
    }
    
    if (!selected.IsValid() && !gpus.empty()) {
        selected = gpus[0];
    }
    
    if (s_enableLogging) {
        LogSelection(selected, effectivePreference);
    }
    
    return selected;
}

GPUDeviceInfo GPUDeviceSelector::SelectBestGPU(DeviceFilter filter) {
    std::vector<GPUDeviceInfo> gpus = EnumerateGPUs();
    if (gpus.empty()) {
        return GPUDeviceInfo();
    }
    
    std::vector<GPUDeviceInfo> filtered;
    for (const auto& gpu : gpus) {
        if (filter(gpu)) {
            filtered.push_back(gpu);
        }
    }
    
    if (filtered.empty()) {
        return GPUDeviceInfo();
    }
    
    return SelectByScore(filtered);
}

int GPUDeviceSelector::CalculateGPUScore(const GPUDeviceInfo& gpu) {
    return CalculateGPUScore(gpu, GPUPreference::kAuto);
}

int GPUDeviceSelector::CalculateGPUScore(const GPUDeviceInfo& gpu, GPUPreference preference) {
    if (!gpu.IsValid()) {
        return 0;
    }
    
    int score = 0;
    
    score += static_cast<int>(gpu.dedicatedMemory / (1024 * 1024));
    score += static_cast<int>(gpu.sharedMemory / (1024 * 1024 * 2));
    
    score += gpu.computeUnits * 10;
    score += gpu.clockFrequency / 10;
    
    if (gpu.isDiscrete) {
        score += 500;
    }
    if (gpu.supportsCompute) {
        score += 200;
    }
    if (gpu.supportsRayTracing) {
        score += 300;
    }
    if (gpu.supportsRender) {
        score += 100;
    }
    
    score += gpu.maxTextureSize / 100;
    score += gpu.maxRenderTargets * 10;
    
    switch (preference) {
        case GPUPreference::kDiscrete:
            if (gpu.isDiscrete) score += 1000;
            break;
        case GPUPreference::kIntegrated:
            if (gpu.isIntegrated) score += 1000;
            break;
        case GPUPreference::kHighMemory:
            score += static_cast<int>(gpu.GetTotalMemory() / (1024 * 1024));
            break;
        case GPUPreference::kLowPower:
            if (gpu.isIntegrated) score += 500;
            if (!gpu.supportsRayTracing) score += 200;
            break;
        default:
            break;
    }
    
    return score;
}

GPUDeviceInfo GPUDeviceSelector::GetGPUByIndex(int index) {
    std::vector<GPUDeviceInfo> gpus = EnumerateGPUs();
    for (const auto& gpu : gpus) {
        if (gpu.index == index) {
            return gpu;
        }
    }
    return GPUDeviceInfo();
}

GPUDeviceInfo GPUDeviceSelector::GetGPUByName(const std::string& name) {
    std::vector<GPUDeviceInfo> gpus = EnumerateGPUs();
    for (const auto& gpu : gpus) {
        if (gpu.name == name) {
            return gpu;
        }
    }
    return GPUDeviceInfo();
}

int GPUDeviceSelector::GetGPUCount() {
    return static_cast<int>(EnumerateGPUs().size());
}

bool GPUDeviceSelector::HasGPU() {
    return GetGPUCount() > 0;
}

bool GPUDeviceSelector::HasDiscreteGPU() {
    std::vector<GPUDeviceInfo> gpus = EnumerateGPUs();
    for (const auto& gpu : gpus) {
        if (gpu.isDiscrete) {
            return true;
        }
    }
    return false;
}

bool GPUDeviceSelector::HasIntegratedGPU() {
    std::vector<GPUDeviceInfo> gpus = EnumerateGPUs();
    for (const auto& gpu : gpus) {
        if (gpu.isIntegrated) {
            return true;
        }
    }
    return false;
}

void GPUDeviceSelector::SetPreferredGPU(int index) {
    s_preferredGPUIndex = index;
}

int GPUDeviceSelector::GetPreferredGPUIndex() {
    return s_preferredGPUIndex;
}

GPUDeviceInfo GPUDeviceSelector::GetPreferredGPU() {
    if (s_preferredGPUIndex >= 0) {
        return GetGPUByIndex(s_preferredGPUIndex);
    }
    return SelectBestGPU();
}

void GPUDeviceSelector::SetPreference(GPUPreference preference) {
    s_preference = preference;
}

GPUPreference GPUDeviceSelector::GetPreference() {
    return s_preference;
}

std::string GPUDeviceSelector::GPUInfoToString(const GPUDeviceInfo& gpu) {
    std::ostringstream oss;
    oss << "GPU Device:\n";
    oss << "  Index: " << gpu.index << "\n";
    oss << "  Name: " << gpu.name << "\n";
    oss << "  Vendor: " << gpu.vendor << "\n";
    oss << "  Dedicated Memory: " << (gpu.dedicatedMemory / (1024 * 1024)) << " MB\n";
    oss << "  Shared Memory: " << (gpu.sharedMemory / (1024 * 1024)) << " MB\n";
    oss << "  Compute Units: " << gpu.computeUnits << "\n";
    oss << "  Clock Frequency: " << gpu.clockFrequency << " MHz\n";
    oss << "  Type: " << (gpu.isDiscrete ? "Discrete" : (gpu.isIntegrated ? "Integrated" : "Unknown")) << "\n";
    oss << "  Supports Compute: " << (gpu.supportsCompute ? "Yes" : "No") << "\n";
    oss << "  Supports Ray Tracing: " << (gpu.supportsRayTracing ? "Yes" : "No") << "\n";
    oss << "  Max Texture Size: " << gpu.maxTextureSize << "\n";
    oss << "  Max Render Targets: " << gpu.maxRenderTargets << "\n";
    oss << "  Score: " << CalculateGPUScore(gpu) << "\n";
    return oss.str();
}

void GPUDeviceSelector::PrintGPUInfo(const GPUDeviceInfo& gpu) {
    std::cout << GPUInfoToString(gpu) << std::endl;
}

std::string GPUDeviceSelector::PreferenceToString(GPUPreference preference) {
    switch (preference) {
        case GPUPreference::kAuto: return "Auto";
        case GPUPreference::kDiscrete: return "Discrete";
        case GPUPreference::kIntegrated: return "Integrated";
        case GPUPreference::kHighMemory: return "HighMemory";
        case GPUPreference::kLowPower: return "LowPower";
        default: return "Unknown";
    }
}

GPUPreference GPUDeviceSelector::StringToPreference(const std::string& str) {
    if (str == "Discrete" || str == "discrete") return GPUPreference::kDiscrete;
    if (str == "Integrated" || str == "integrated") return GPUPreference::kIntegrated;
    if (str == "HighMemory" || str == "highmemory" || str == "high_memory") return GPUPreference::kHighMemory;
    if (str == "LowPower" || str == "lowpower" || str == "low_power") return GPUPreference::kLowPower;
    return GPUPreference::kAuto;
}

void GPUDeviceSelector::SetEnableLogging(bool enable) {
    s_enableLogging = enable;
}

bool GPUDeviceSelector::IsLoggingEnabled() {
    return s_enableLogging;
}

GPUDeviceInfo GPUDeviceSelector::SelectByScore(const std::vector<GPUDeviceInfo>& gpus) {
    if (gpus.empty()) {
        return GPUDeviceInfo();
    }
    
    GPUDeviceInfo best = gpus[0];
    int bestScore = CalculateGPUScore(best);
    
    for (size_t i = 1; i < gpus.size(); ++i) {
        int score = CalculateGPUScore(gpus[i]);
        if (score > bestScore) {
            bestScore = score;
            best = gpus[i];
        }
    }
    
    return best;
}

GPUDeviceInfo GPUDeviceSelector::SelectDiscrete(const std::vector<GPUDeviceInfo>& gpus) {
    std::vector<GPUDeviceInfo> discrete;
    for (const auto& gpu : gpus) {
        if (gpu.isDiscrete) {
            discrete.push_back(gpu);
        }
    }
    
    if (discrete.empty()) {
        return SelectByScore(gpus);
    }
    
    return SelectByScore(discrete);
}

GPUDeviceInfo GPUDeviceSelector::SelectIntegrated(const std::vector<GPUDeviceInfo>& gpus) {
    std::vector<GPUDeviceInfo> integrated;
    for (const auto& gpu : gpus) {
        if (gpu.isIntegrated) {
            integrated.push_back(gpu);
        }
    }
    
    if (integrated.empty()) {
        return SelectByScore(gpus);
    }
    
    return SelectByScore(integrated);
}

GPUDeviceInfo GPUDeviceSelector::SelectHighMemory(const std::vector<GPUDeviceInfo>& gpus) {
    if (gpus.empty()) {
        return GPUDeviceInfo();
    }
    
    GPUDeviceInfo best = gpus[0];
    size_t maxMemory = best.GetTotalMemory();
    
    for (size_t i = 1; i < gpus.size(); ++i) {
        size_t memory = gpus[i].GetTotalMemory();
        if (memory > maxMemory) {
            maxMemory = memory;
            best = gpus[i];
        }
    }
    
    return best;
}

GPUDeviceInfo GPUDeviceSelector::SelectLowPower(const std::vector<GPUDeviceInfo>& gpus) {
    std::vector<GPUDeviceInfo> lowPower;
    for (const auto& gpu : gpus) {
        if (gpu.isIntegrated || !gpu.supportsRayTracing) {
            lowPower.push_back(gpu);
        }
    }
    
    if (lowPower.empty()) {
        return SelectByScore(gpus);
    }
    
    return SelectByScore(lowPower);
}

void GPUDeviceSelector::LogDeviceList(const std::vector<GPUDeviceInfo>& gpus) {
    (void)gpus;
}

void GPUDeviceSelector::LogSelection(const GPUDeviceInfo& gpu, GPUPreference preference) {
    (void)gpu;
    (void)preference;
}

GPUDeviceManager& GPUDeviceManager::Instance() {
    static GPUDeviceManager instance;
    return instance;
}

GPUDeviceManager::GPUDeviceManager()
    : m_currentDeviceIndex(-1)
    , m_initialized(false) {
}

GPUDeviceManager::~GPUDeviceManager() {
    Shutdown();
}

void GPUDeviceManager::Initialize() {
    if (m_initialized) {
        return;
    }
    
    RefreshDevices();
    m_initialized = true;
}

void GPUDeviceManager::Shutdown() {
    m_devices.clear();
    m_currentDeviceIndex = -1;
    m_initialized = false;
}

bool GPUDeviceManager::IsInitialized() const {
    return m_initialized;
}

void GPUDeviceManager::RefreshDevices() {
    m_devices = GPUDeviceSelector::EnumerateGPUs();
    
    if (m_currentDeviceIndex < 0 && !m_devices.empty()) {
        GPUDeviceInfo best = GPUDeviceSelector::SelectBestGPU();
        m_currentDeviceIndex = best.index;
    }
}

std::vector<GPUDeviceInfo> GPUDeviceManager::GetDevices() const {
    return m_devices;
}

GPUDeviceInfo GPUDeviceManager::GetCurrentDevice() const {
    if (m_currentDeviceIndex < 0 || m_currentDeviceIndex >= static_cast<int>(m_devices.size())) {
        return GPUDeviceInfo();
    }
    return m_devices[m_currentDeviceIndex];
}

GPUDeviceInfo GPUDeviceManager::SelectDevice(GPUPreference preference) {
    GPUDeviceInfo selected = GPUDeviceSelector::SelectBestGPU(preference);
    if (selected.IsValid()) {
        int oldIndex = m_currentDeviceIndex;
        m_currentDeviceIndex = selected.index;
        
        if (m_deviceChangeCallback && oldIndex != m_currentDeviceIndex) {
            m_deviceChangeCallback(oldIndex, m_currentDeviceIndex);
        }
    }
    return selected;
}

void GPUDeviceManager::SetDeviceChangeCallback(std::function<void(int, int)> callback) {
    m_deviceChangeCallback = callback;
}

}
}
