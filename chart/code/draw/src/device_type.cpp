#include "ogc/draw/device_type.h"

namespace ogc {
namespace draw {

const char* DeviceTypeToString(DeviceType type) {
    switch (type) {
        case DeviceType::kUnknown:    return "Unknown";
        case DeviceType::kDisplay:    return "Display";
        case DeviceType::kRasterImage: return "RasterImage";
        case DeviceType::kPdf:        return "Pdf";
        case DeviceType::kSvg:        return "Svg";
        case DeviceType::kTile:       return "Tile";
        case DeviceType::kWebGL:      return "WebGL";
        case DeviceType::kPrinter:    return "Printer";
        default:                      return "Unknown";
    }
}

std::string GetDeviceTypeString(DeviceType type) {
    return std::string(DeviceTypeToString(type));
}

std::string GetDeviceTypeDescription(DeviceType type) {
    switch (type) {
        case DeviceType::kUnknown:    return "Unknown device type";
        case DeviceType::kDisplay:    return "Display device for on-screen rendering";
        case DeviceType::kRasterImage: return "Raster image device for pixel-based output";
        case DeviceType::kPdf:        return "PDF device for document generation";
        case DeviceType::kSvg:        return "SVG device for vector graphics output";
        case DeviceType::kTile:       return "Tile device for map tile rendering";
        case DeviceType::kWebGL:      return "WebGL device for web-based 3D rendering";
        case DeviceType::kPrinter:    return "Printer device for hardcopy output";
        default:                      return "Unknown device type";
    }
}

}  
}  
