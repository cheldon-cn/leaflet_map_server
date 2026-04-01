#include "ogc/draw/engine_type.h"

namespace ogc {
namespace draw {

const char* EngineTypeToString(EngineType type) {
    switch (type) {
        case EngineType::kUnknown:  return "Unknown";
        case EngineType::kSimple2D: return "Simple2D";
        case EngineType::kGPU:      return "GPU";
        case EngineType::kVector:   return "Vector";
        case EngineType::kTile:     return "Tile";
        case EngineType::kWebGL:    return "WebGL";
        default:                    return "Unknown";
    }
}

std::string GetEngineTypeString(EngineType type) {
    return std::string(EngineTypeToString(type));
}

std::string GetEngineTypeDescription(EngineType type) {
    switch (type) {
        case EngineType::kUnknown:  return "Unknown engine type";
        case EngineType::kSimple2D: return "Simple 2D rendering engine";
        case EngineType::kGPU:      return "GPU-accelerated rendering engine";
        case EngineType::kVector:   return "Vector graphics rendering engine";
        case EngineType::kTile:     return "Tile-based rendering engine";
        case EngineType::kWebGL:    return "WebGL-based rendering engine";
        default:                    return "Unknown engine type";
    }
}

std::vector<EngineType> GetAllEngineTypes() {
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
