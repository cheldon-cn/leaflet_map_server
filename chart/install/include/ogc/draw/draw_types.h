#ifndef OGC_DRAW_TYPES_H
#define OGC_DRAW_TYPES_H

#include "ogc/draw/export.h"
#include "ogc/draw/color.h"
#include "ogc/draw/geometry_types.h"
#include "ogc/draw/device_type.h"
#include "ogc/draw/device_state.h"
#include "ogc/draw/engine_type.h"
#include "ogc/draw/draw_result.h"
#include "ogc/draw/font.h"
#include "ogc/draw/draw_style.h"
#include "ogc/draw/transform_matrix.h"
#include "ogc/draw/image.h"
#include "ogc/draw/text_metrics.h"
#include "ogc/draw/region.h"
#include <memory>

namespace ogc {
namespace draw {

class DrawDevice;
class DrawEngine;
class DrawContext;

using DrawDevicePtr = std::shared_ptr<DrawDevice>;
using DrawEnginePtr = std::shared_ptr<DrawEngine>;
using DrawContextPtr = std::shared_ptr<DrawContext>;

}  
}  

#endif  
