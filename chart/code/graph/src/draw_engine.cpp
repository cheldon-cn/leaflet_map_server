#include "ogc/draw/draw_engine.h"

namespace ogc {
namespace draw {

DrawEnginePtr DrawEngine::Create(EngineType type) {
    (void)type;
    return nullptr;
}

}  
}  
