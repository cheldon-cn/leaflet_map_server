#include "ogc/draw/capability_negotiator.h"
#include "ogc/draw/draw_engine.h"
#include <algorithm>
#include <sstream>

namespace ogc {
namespace draw {

CapabilityNegotiator::CapabilityNegotiator()
    : m_capabilities() {
}

CapabilityNegotiator::CapabilityNegotiator(const DeviceCapabilities& caps)
    : m_capabilities(caps) {
}

CapabilityNegotiator::~CapabilityNegotiator() {
}

CapabilityNegotiator::CapabilityNegotiator(CapabilityNegotiator&& other) noexcept
    : m_capabilities(std::move(other.m_capabilities)) {
}

CapabilityNegotiator& CapabilityNegotiator::operator=(CapabilityNegotiator&& other) noexcept {
    if (this != &other) {
        m_capabilities = std::move(other.m_capabilities);
    }
    return *this;
}

void CapabilityNegotiator::SetCapabilities(const DeviceCapabilities& caps) {
    m_capabilities = caps;
}

const DeviceCapabilities& CapabilityNegotiator::GetCapabilities() const {
    return m_capabilities;
}

DrawStyle CapabilityNegotiator::NegotiateStyle(const DrawStyle& requested) const {
    DrawStyle negotiated = requested;
    
    DowngradeTransparency(negotiated);
    DowngradeAntialiasing(negotiated);
    DowngradeComplexStyles(negotiated);
    
    return negotiated;
}

Font CapabilityNegotiator::NegotiateFont(const Font& requested) const {
    Font negotiated = requested;
    DowngradeTextRotation(negotiated);
    return negotiated;
}

bool CapabilityNegotiator::IsFeatureSupported(const std::string& feature) const {
    if (feature == "gpu") return m_capabilities.supportsGPU;
    if (feature == "multithreading") return m_capabilities.supportsMultithreading;
    if (feature == "vsync") return m_capabilities.supportsVSync;
    if (feature == "double_buffer") return m_capabilities.supportsDoubleBuffer;
    if (feature == "partial_update") return m_capabilities.supportsPartialUpdate;
    if (feature == "alpha") return m_capabilities.supportsAlpha;
    if (feature == "antialiasing") return m_capabilities.supportsAntialiasing;
    
    for (const auto& fmt : m_capabilities.supportedFormats) {
        if (fmt == feature) return true;
    }
    
    return false;
}

bool CapabilityNegotiator::SupportsTransparency() const {
    return m_capabilities.supportsAlpha;
}

bool CapabilityNegotiator::SupportsAntialiasing() const {
    return m_capabilities.supportsAntialiasing;
}

bool CapabilityNegotiator::SupportsGPU() const {
    return m_capabilities.supportsGPU;
}

bool CapabilityNegotiator::SupportsMultithreading() const {
    return m_capabilities.supportsMultithreading;
}

bool CapabilityNegotiator::SupportsPartialUpdate() const {
    return m_capabilities.supportsPartialUpdate;
}

int CapabilityNegotiator::GetMaxTextureSize() const {
    return m_capabilities.maxTextureSize;
}

int CapabilityNegotiator::GetMaxRenderTargets() const {
    return m_capabilities.maxRenderTargets;
}

std::vector<std::string> CapabilityNegotiator::GetUnsupportedFeatures(const DrawStyle& style) const {
    std::vector<std::string> unsupported;
    
    if (style.opacity < 1.0 && !m_capabilities.supportsAlpha) {
        unsupported.push_back("transparency");
    }
    
    if (style.antialias && !m_capabilities.supportsAntialiasing) {
        unsupported.push_back("antialiasing");
    }
    
    if (!style.pen.dashPattern.empty() && !m_capabilities.supportsGPU) {
        unsupported.push_back("dash_pattern");
    }
    
    return unsupported;
}

std::string CapabilityNegotiator::GetNegotiationReport(const DrawStyle& requested) const {
    std::ostringstream oss;
    oss << "Capability Negotiation Report:\n";
    oss << "  Device Capabilities:\n";
    oss << "    GPU: " << (m_capabilities.supportsGPU ? "Yes" : "No") << "\n";
    oss << "    Multithreading: " << (m_capabilities.supportsMultithreading ? "Yes" : "No") << "\n";
    oss << "    Alpha: " << (m_capabilities.supportsAlpha ? "Yes" : "No") << "\n";
    oss << "    Antialiasing: " << (m_capabilities.supportsAntialiasing ? "Yes" : "No") << "\n";
    oss << "    Max Texture Size: " << m_capabilities.maxTextureSize << "\n";
    
    DrawStyle negotiated = NegotiateStyle(requested);
    auto unsupported = GetUnsupportedFeatures(requested);
    
    oss << "  Requested Style:\n";
    oss << "    Opacity: " << requested.opacity << "\n";
    oss << "    Antialias: " << (requested.antialias ? "Yes" : "No") << "\n";
    
    oss << "  Negotiated Style:\n";
    oss << "    Opacity: " << negotiated.opacity << "\n";
    oss << "    Antialias: " << (negotiated.antialias ? "Yes" : "No") << "\n";
    
    if (!unsupported.empty()) {
        oss << "  Unsupported Features:\n";
        for (const auto& f : unsupported) {
            oss << "    - " << f << "\n";
        }
    }
    
    return oss.str();
}

void CapabilityNegotiator::DowngradeTransparency(DrawStyle& style) const {
    if (!m_capabilities.supportsAlpha && style.opacity < 1.0) {
        style.opacity = 1.0;
    }
    
    if (!m_capabilities.supportsAlpha) {
        if (style.pen.color.a < 255) {
            style.pen.color.a = 255;
        }
        if (style.brush.color.a < 255) {
            style.brush.color.a = 255;
        }
    }
}

void CapabilityNegotiator::DowngradeAntialiasing(DrawStyle& style) const {
    if (!m_capabilities.supportsAntialiasing && style.antialias) {
        style.antialias = false;
    }
}

void CapabilityNegotiator::DowngradeTextRotation(Font& font) const {
    (void)font;
}

void CapabilityNegotiator::DowngradeComplexStyles(DrawStyle& style) const {
    if (!m_capabilities.supportsGPU && !style.pen.dashPattern.empty()) {
        style.pen.dashPattern.clear();
        style.pen.style = PenStyle::kSolid;
    }
}

int DeviceRecovery::s_maxRecoveryAttempts = 3;
int DeviceRecovery::s_recoveryDelayMs = 100;
std::vector<DeviceRecovery::RecoveryCallback> DeviceRecovery::s_recoveryCallbacks;

DeviceRecovery::RecoveryResult DeviceRecovery::HandleDeviceLost(DrawDevice* device) {
    if (!device) {
        return RecoveryResult::kUnsupportedDevice;
    }
    
    if (!device->IsDeviceLost()) {
        return RecoveryResult::kDeviceNotLost;
    }
    
    LogRecoveryEvent("Device lost detected, attempting recovery");
    
    if (TryRecoverGPU(device)) {
        LogRecoveryEvent("Device recovery successful");
        NotifyRecoveryCallbacks();
        return RecoveryResult::kSuccess;
    }
    
    LogRecoveryEvent("Device recovery failed");
    return RecoveryResult::kFailed;
}

DeviceRecovery::RecoveryResult DeviceRecovery::HandleDeviceLostWithEngine(DrawDevice* device, DrawEngine* engine) {
    if (!device) {
        return RecoveryResult::kUnsupportedDevice;
    }
    
    if (!device->IsDeviceLost()) {
        return RecoveryResult::kDeviceNotLost;
    }
    
    LogRecoveryEvent("Device lost detected with engine, attempting recovery");
    
    if (TryRecoverWithEngine(device, engine)) {
        LogRecoveryEvent("Device recovery with engine successful");
        NotifyRecoveryCallbacks();
        return RecoveryResult::kSuccess;
    }
    
    LogRecoveryEvent("Device recovery with engine failed");
    return RecoveryResult::kFailed;
}

void DeviceRecovery::RegisterRecoveryCallback(RecoveryCallback callback) {
    s_recoveryCallbacks.push_back(callback);
}

void DeviceRecovery::ClearRecoveryCallbacks() {
    s_recoveryCallbacks.clear();
}

bool DeviceRecovery::IsDeviceLost(DrawDevice* device) {
    return device && device->IsDeviceLost();
}

bool DeviceRecovery::CanRecover(DrawDevice* device) {
    return device && device->GetState() != DeviceState::kError;
}

void DeviceRecovery::SetMaxRecoveryAttempts(int attempts) {
    s_maxRecoveryAttempts = attempts > 0 ? attempts : 3;
}

int DeviceRecovery::GetMaxRecoveryAttempts() {
    return s_maxRecoveryAttempts;
}

void DeviceRecovery::SetRecoveryDelayMs(int delayMs) {
    s_recoveryDelayMs = delayMs > 0 ? delayMs : 100;
}

int DeviceRecovery::GetRecoveryDelayMs() {
    return s_recoveryDelayMs;
}

bool DeviceRecovery::TryRecoverGPU(DrawDevice* device) {
    for (int attempt = 0; attempt < s_maxRecoveryAttempts; ++attempt) {
        DrawResult result = device->RecoverDevice();
        if (result == DrawResult::kSuccess) {
            return true;
        }
    }
    return false;
}

bool DeviceRecovery::TryRecoverWithEngine(DrawDevice* device, DrawEngine* engine) {
    if (TryRecoverGPU(device)) {
        if (engine) {
            engine->End();
            DrawResult beginResult = engine->Begin();
            return beginResult == DrawResult::kSuccess;
        }
        return true;
    }
    return false;
}

void DeviceRecovery::NotifyRecoveryCallbacks() {
    for (auto& callback : s_recoveryCallbacks) {
        if (callback) {
            callback();
        }
    }
}

void DeviceRecovery::LogRecoveryEvent(const std::string& event) {
    (void)event;
}

DeviceLostHandler::DeviceLostHandler()
    : m_device(nullptr)
    , m_engine(nullptr)
    , m_autoRecover(true) {
}

DeviceLostHandler::~DeviceLostHandler() {
}

DeviceLostHandler::DeviceLostHandler(DeviceLostHandler&& other) noexcept
    : m_device(other.m_device)
    , m_engine(other.m_engine)
    , m_autoRecover(other.m_autoRecover)
    , m_onDeviceLost(std::move(other.m_onDeviceLost))
    , m_onDeviceRecovered(std::move(other.m_onDeviceRecovered)) {
    other.m_device = nullptr;
    other.m_engine = nullptr;
}

DeviceLostHandler& DeviceLostHandler::operator=(DeviceLostHandler&& other) noexcept {
    if (this != &other) {
        m_device = other.m_device;
        m_engine = other.m_engine;
        m_autoRecover = other.m_autoRecover;
        m_onDeviceLost = std::move(other.m_onDeviceLost);
        m_onDeviceRecovered = std::move(other.m_onDeviceRecovered);
        other.m_device = nullptr;
        other.m_engine = nullptr;
    }
    return *this;
}

void DeviceLostHandler::SetDevice(DrawDevice* device) {
    m_device = device;
}

DrawDevice* DeviceLostHandler::GetDevice() const {
    return m_device;
}

void DeviceLostHandler::SetEngine(DrawEngine* engine) {
    m_engine = engine;
}

DrawEngine* DeviceLostHandler::GetEngine() const {
    return m_engine;
}

void DeviceLostHandler::SetAutoRecover(bool autoRecover) {
    m_autoRecover = autoRecover;
}

bool DeviceLostHandler::IsAutoRecoverEnabled() const {
    return m_autoRecover;
}

bool DeviceLostHandler::CheckAndRecover() {
    if (!m_device) {
        return false;
    }
    
    if (!m_device->IsDeviceLost()) {
        return true;
    }
    
    if (m_onDeviceLost) {
        m_onDeviceLost();
    }
    
    if (m_autoRecover) {
        DeviceRecovery::RecoveryResult result = Recover();
        if (result == DeviceRecovery::RecoveryResult::kSuccess) {
            if (m_onDeviceRecovered) {
                m_onDeviceRecovered();
            }
            return true;
        }
    }
    
    return false;
}

DeviceRecovery::RecoveryResult DeviceLostHandler::Recover() {
    if (!m_device) {
        return DeviceRecovery::RecoveryResult::kUnsupportedDevice;
    }
    
    if (m_engine) {
        return DeviceRecovery::HandleDeviceLostWithEngine(m_device, m_engine);
    }
    
    return DeviceRecovery::HandleDeviceLost(m_device);
}

void DeviceLostHandler::OnDeviceLost(std::function<void()> callback) {
    m_onDeviceLost = callback;
}

void DeviceLostHandler::OnDeviceRecovered(std::function<void()> callback) {
    m_onDeviceRecovered = callback;
}

}
}
