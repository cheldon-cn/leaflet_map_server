#ifndef OGC_DRAW_DEVICE_TYPE_H
#define OGC_DRAW_DEVICE_TYPE_H

namespace ogc {
namespace draw {

enum class DeviceType {
    kDisplay = 0,
    kRasterImage = 1,
    kPdf = 2,
    kSvg = 3,
    kTile = 4,
    kWebGL = 5,
    kPrinter = 6
};

inline const char* DeviceTypeToString(DeviceType type) {
    switch (type) {
        case DeviceType::kDisplay:     return "Display";
        case DeviceType::kRasterImage: return "RasterImage";
        case DeviceType::kPdf:         return "Pdf";
        case DeviceType::kSvg:         return "Svg";
        case DeviceType::kTile:        return "Tile";
        case DeviceType::kWebGL:       return "WebGL";
        case DeviceType::kPrinter:     return "Printer";
        default:                       return "Unknown";
    }
}

}  
}  

#endif  
