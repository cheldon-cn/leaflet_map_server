#ifndef OGC_DRAW_ENGINE_TYPE_H
#define OGC_DRAW_ENGINE_TYPE_H

#include <string>
#include <vector>

namespace ogc {
namespace draw {

enum class EngineType {
    kSimple2D = 0,
    kGPU = 1,
    kVector = 2,
    kTile = 3,
    kWebGL = 4
};

inline const char* EngineTypeToString(EngineType type) {
    switch (type) {
        case EngineType::kSimple2D: return "Simple2D";
        case EngineType::kGPU:      return "GPU";
        case EngineType::kVector:   return "Vector";
        case EngineType::kTile:     return "Tile";
        case EngineType::kWebGL:    return "WebGL";
        default:                    return "Unknown";
    }
}

inline std::vector<EngineType> GetAllEngineTypes() {
    return std::vector<EngineType>{
        EngineType::kSimple2D,
        EngineType::kGPU,
        EngineType::kVector,
        EngineType::kTile,
        EngineType::kWebGL
    };
}

}  
}  

#endif  
