#ifndef OGC_DRAW_DEVICE_STATE_H
#define OGC_DRAW_DEVICE_STATE_H

#include "ogc/draw/export.h"
#include <string>

namespace ogc {
namespace draw {

enum class DeviceState {
    kUninitialized = 0,
    kReady = 1,
    kActive = 2,
    kError = 3,
    kLost = 4
};

OGC_DRAW_API const char* DeviceStateToString(DeviceState state);

OGC_DRAW_API std::string GetDeviceStateString(DeviceState state);

OGC_DRAW_API std::string GetDeviceStateDescription(DeviceState state);

}  
}  

#endif  
