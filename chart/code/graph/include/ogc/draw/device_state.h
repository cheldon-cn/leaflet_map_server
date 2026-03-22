#ifndef OGC_DRAW_DEVICE_STATE_H
#define OGC_DRAW_DEVICE_STATE_H

#include <string>

namespace ogc {
namespace draw {

enum class DeviceState {
    kCreated = 0,
    kInitialized = 1,
    kActive = 2,
    kPaused = 3,
    kError = 4,
    kClosed = 5
};

std::string GetDeviceStateString(DeviceState state);

std::string GetDeviceStateDescription(DeviceState state);

}  
}  

#endif  
