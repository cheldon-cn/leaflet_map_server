#include "ogc/draw/device_state.h"

namespace ogc {
namespace draw {

std::string GetDeviceStateString(DeviceState state) {
    switch (state) {
        case DeviceState::kCreated: return "Created";
        case DeviceState::kInitialized: return "Initialized";
        case DeviceState::kActive: return "Active";
        case DeviceState::kPaused: return "Paused";
        case DeviceState::kError: return "Error";
        case DeviceState::kClosed: return "Closed";
        default: return "Unknown";
    }
}

std::string GetDeviceStateDescription(DeviceState state) {
    switch (state) {
        case DeviceState::kCreated: return "Device has been created";
        case DeviceState::kInitialized: return "Device has been initialized";
        case DeviceState::kActive: return "Device is active and ready for operations";
        case DeviceState::kPaused: return "Device is paused";
        case DeviceState::kError: return "Device is in error state";
        case DeviceState::kClosed: return "Device has been closed";
        default: return "Unknown device state";
    }
}

}  
}  
