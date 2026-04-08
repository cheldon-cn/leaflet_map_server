#ifndef OGC_DRAW_ENGINE_TYPE_H
#define OGC_DRAW_ENGINE_TYPE_H

#include "ogc/draw/export.h"
#include <string>
#include <vector>

namespace ogc {
namespace draw {

enum class EngineType {
    kUnknown = 0,
    kSimple2D = 1,
    kGPU = 2,
    kVector = 3,
    kTile = 4,
    kWebGL = 5
};

OGC_DRAW_API const char* EngineTypeToString(EngineType type);

OGC_DRAW_API std::string GetEngineTypeString(EngineType type);

OGC_DRAW_API std::string GetEngineTypeDescription(EngineType type);

OGC_DRAW_API std::vector<EngineType> GetAllEngineTypes();

}  
}  

#endif  
