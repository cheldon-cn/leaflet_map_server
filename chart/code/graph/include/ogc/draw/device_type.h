#ifndef OGC_DRAW_DEVICE_TYPE_H
#define OGC_DRAW_DEVICE_TYPE_H

#include <string>

namespace ogc {
namespace draw {

enum class DeviceType {
    kUnknown = 0,
    kRasterImage = 1,
    kPdf = 2,
    kTile = 3,
    kDisplay = 4,
    kSvg = 5,
    kPrinter = 6
};

std::string GetDeviceTypeString(DeviceType type);

std::string GetDeviceTypeDescription(DeviceType type);

}  
}  

#endif  
