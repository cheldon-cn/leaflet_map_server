#ifndef OGC_DRAW_DEVICE_TYPE_H
#define OGC_DRAW_DEVICE_TYPE_H

#include "ogc/draw/export.h"
#include <string>

namespace ogc {
namespace draw {

enum class DeviceType {
    kUnknown = 0,
    kDisplay = 1,
    kRasterImage = 2,
    kPdf = 3,
    kSvg = 4,
    kTile = 5,
    kWebGL = 6,
    kPrinter = 7
};

OGC_DRAW_API const char* DeviceTypeToString(DeviceType type);

OGC_DRAW_API std::string GetDeviceTypeString(DeviceType type);

OGC_DRAW_API std::string GetDeviceTypeDescription(DeviceType type);

}  
}  

#endif  
