#include "ogc/draw/device_state.h"

namespace ogc {
namespace draw {

const char* DeviceStateToString(DeviceState state) {
    switch (state) {
        case DeviceState::kUninitialized: return "Uninitialized";
        case DeviceState::kReady:         return "Ready";
        case DeviceState::kActive:        return "Active";
        case DeviceState::kError:         return "Error";
        case DeviceState::kLost:          return "Lost";
        default:                          return "Unknown";
    }
}

std::string GetDeviceStateString(DeviceState state) {
    return std::string(DeviceStateToString(state));
}

std::string GetDeviceStateDescription(DeviceState state) {
    switch (state) {
        case DeviceState::kUninitialized: return "Device is not initialized";
        case DeviceState::kReady:         return "Device is ready for use";
        case DeviceState::kActive:        return "Device is currently active";
        case DeviceState::kError:         return "Device is in error state";
        case DeviceState::kLost:          return "Device has been lost";
        default:                          return "Unknown device state";
    }
}

}  
}  
