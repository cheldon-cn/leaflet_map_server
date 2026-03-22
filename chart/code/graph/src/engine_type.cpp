#include "ogc/draw/engine_type.h"

namespace ogc {
namespace draw {

std::string GetEngineTypeString(EngineType type) {
    switch (type) {
        case EngineType::kUnknown: return "Unknown";
        case EngineType::kSimple2D: return "Simple2D";
        case EngineType::kTileBased: return "TileBased";
        case EngineType::kVectorTile: return "VectorTile";
        case EngineType::kGPUAccelerated: return "GPUAccelerated";
        default: return "Unknown";
    }
}

std::string GetEngineTypeDescription(EngineType type) {
    switch (type) {
        case EngineType::kUnknown: return "Unknown engine type";
        case EngineType::kSimple2D: return "Simple 2D rendering engine";
        case EngineType::kTileBased: return "Tile-based rendering engine";
        case EngineType::kVectorTile: return "Vector tile rendering engine";
        case EngineType::kGPUAccelerated: return "GPU-accelerated rendering engine";
        default: return "Unknown engine type";
    }
}

}  
}  
