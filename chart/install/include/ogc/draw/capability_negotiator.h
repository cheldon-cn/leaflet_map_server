#ifndef OGC_DRAW_CAPABILITY_NEGOTIATOR_H
#define OGC_DRAW_CAPABILITY_NEGOTIATOR_H

#include <ogc/draw/export.h>
#include <ogc/draw/draw_device.h>
#include <ogc/draw/draw_style.h>
#include <string>
#include <vector>
#include <functional>

namespace ogc {
namespace draw {

class OGC_DRAW_API CapabilityNegotiator {
public:
    CapabilityNegotiator();
    explicit CapabilityNegotiator(const DeviceCapabilities& caps);
    ~CapabilityNegotiator();

    CapabilityNegotiator(const CapabilityNegotiator&) = delete;
    CapabilityNegotiator& operator=(const CapabilityNegotiator&) = delete;
    CapabilityNegotiator(CapabilityNegotiator&&) noexcept;
    CapabilityNegotiator& operator=(CapabilityNegotiator&&) noexcept;

    void SetCapabilities(const DeviceCapabilities& caps);
    const DeviceCapabilities& GetCapabilities() const;

    DrawStyle NegotiateStyle(const DrawStyle& requested) const;
    Font NegotiateFont(const Font& requested) const;
    bool IsFeatureSupported(const std::string& feature) const;

    bool SupportsTransparency() const;
    bool SupportsAntialiasing() const;
    bool SupportsGPU() const;
    bool SupportsMultithreading() const;
    bool SupportsPartialUpdate() const;
    int GetMaxTextureSize() const;
    int GetMaxRenderTargets() const;

    std::vector<std::string> GetUnsupportedFeatures(const DrawStyle& style) const;
    std::string GetNegotiationReport(const DrawStyle& requested) const;

private:
    void DowngradeTransparency(DrawStyle& style) const;
    void DowngradeAntialiasing(DrawStyle& style) const;
    void DowngradeTextRotation(Font& font) const;
    void DowngradeComplexStyles(DrawStyle& style) const;

    DeviceCapabilities m_capabilities;
};

class OGC_DRAW_API DeviceRecovery {
public:
    using RecoveryCallback = std::function<void()>;
    using RecoveryResult = enum class RecoveryResult {
        kSuccess = 0,
        kFailed = 1,
        kFallbackToCPU = 2,
        kDeviceNotLost = 3,
        kUnsupportedDevice = 4
    };

    static RecoveryResult HandleDeviceLost(DrawDevice* device);
    static RecoveryResult HandleDeviceLostWithEngine(DrawDevice* device, DrawEngine* engine);

    static void RegisterRecoveryCallback(RecoveryCallback callback);
    static void ClearRecoveryCallbacks();

    static bool IsDeviceLost(DrawDevice* device);
    static bool CanRecover(DrawDevice* device);

    static void SetMaxRecoveryAttempts(int attempts);
    static int GetMaxRecoveryAttempts();

    static void SetRecoveryDelayMs(int delayMs);
    static int GetRecoveryDelayMs();

private:
    static bool TryRecoverGPU(DrawDevice* device);
    static bool TryRecoverWithEngine(DrawDevice* device, DrawEngine* engine);
    static void NotifyRecoveryCallbacks();
    static void LogRecoveryEvent(const std::string& event);

    static int s_maxRecoveryAttempts;
    static int s_recoveryDelayMs;
    static std::vector<RecoveryCallback> s_recoveryCallbacks;
};

class OGC_DRAW_API DeviceLostHandler {
public:
    DeviceLostHandler();
    ~DeviceLostHandler();

    DeviceLostHandler(const DeviceLostHandler&) = delete;
    DeviceLostHandler& operator=(const DeviceLostHandler&) = delete;
    DeviceLostHandler(DeviceLostHandler&&) noexcept;
    DeviceLostHandler& operator=(DeviceLostHandler&&) noexcept;

    void SetDevice(DrawDevice* device);
    DrawDevice* GetDevice() const;

    void SetEngine(DrawEngine* engine);
    DrawEngine* GetEngine() const;

    void SetAutoRecover(bool autoRecover);
    bool IsAutoRecoverEnabled() const;

    bool CheckAndRecover();
    DeviceRecovery::RecoveryResult Recover();

    void OnDeviceLost(std::function<void()> callback);
    void OnDeviceRecovered(std::function<void()> callback);

private:
    DrawDevice* m_device;
    DrawEngine* m_engine;
    bool m_autoRecover;
    std::function<void()> m_onDeviceLost;
    std::function<void()> m_onDeviceRecovered;
};

}
}

#endif
