#include "ogc/draw/draw_device.h"
#include "ogc/draw/raster_image_device.h"
#include "ogc/draw/pdf_device.h"
#include "ogc/draw/tile_device.h"
#include "ogc/draw/display_device.h"

namespace ogc {
namespace draw {

DrawDevicePtr DrawDevice::Create(DeviceType type) {
    switch (type) {
        case DeviceType::kRasterImage:
            return RasterImageDevice::Create(800, 600);
        case DeviceType::kPdf:
            return PdfDevice::Create(612, 792);
        case DeviceType::kTile:
            return TileDevice::Create(256);
        case DeviceType::kDisplay:
            return DisplayDevice::Create(800, 600);
        default:
            return nullptr;
    }
}

}  
}  
