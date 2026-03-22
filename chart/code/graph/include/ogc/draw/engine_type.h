#ifndef OGC_DRAW_ENGINE_TYPE_H
#define OGC_DRAW_ENGINE_TYPE_H

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

std::string GetEngineTypeString(EngineType type);

std::string GetEngineTypeDescription(EngineType type);

}  
}  

#endif  
