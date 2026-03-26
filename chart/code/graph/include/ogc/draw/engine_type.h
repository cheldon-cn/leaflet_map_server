#ifndef OGC_DRAW_ENGINE_TYPE_H
#define OGC_DRAW_ENGINE_TYPE_H

#include "ogc/draw/export.h"
#include <string>

namespace ogc {
namespace draw {

enum class EngineType {
    kUnknown = 0,
    kSimple2D = 1,
    kTileBased = 2,
    kVectorTile = 3,
    kGPUAccelerated = 4
};

OGC_GRAPH_API std::string GetEngineTypeString(EngineType type);

OGC_GRAPH_API std::string GetEngineTypeDescription(EngineType type);

}  
}  

#endif  
