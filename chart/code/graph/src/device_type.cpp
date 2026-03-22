#include "ogc/draw/device_type.h"

namespace ogc {
namespace draw {

std::string GetDeviceTypeString(DeviceType type) {
    switch (type) {
        case DeviceType::kUnknown: return "Unknown";
        case DeviceType::kRasterImage: return "RasterImage";
        case DeviceType::kPdf: return "Pdf";
        case DeviceType::kTile: return "Tile";
        case DeviceType::kDisplay: return "Display";
        case DeviceType::kSvg: return "Svg";
        case DeviceType::kPrinter: return "Printer";
        default: return "Unknown";
    }
}

std::string GetDeviceTypeDescription(DeviceType type) {
    switch (type) {
        case DeviceType::kUnknown: return "Unknown device type";
        case DeviceType::kRasterImage: return "Raster image device (PNG, JPEG, etc.)";
        case DeviceType::kPdf: return "PDF document device";
        case DeviceType::kTile: return "Map tile device";
        case DeviceType::kDisplay: return "Display device (screen)";
        case DeviceType::kSvg: return "SVG vector graphics device";
        case DeviceType::kPrinter: return "Printer device";
        default: return "Unknown device type";
    }
}

}  
}  
