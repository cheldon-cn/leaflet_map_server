#ifndef OGC_DRAW_STATE_SERIALIZER_H
#define OGC_DRAW_STATE_SERIALIZER_H

#include "ogc/draw/export.h"
#include "ogc/draw/transform_matrix.h"
#include "ogc/draw/draw_types.h"
#include "ogc/draw/draw_style.h"
#include "ogc/draw/draw_engine.h"
#include <cstdint>

namespace ogc {
namespace draw {

class DrawEngine;

struct EngineState {
    TransformMatrix transform;
    Region clipRegion;
    Color background;
    StateFlags savedFlags;
    double tolerance;
    double opacity;
    bool antialiasing;

    EngineState()
        : background()
        , savedFlags(static_cast<StateFlags>(StateFlag::kAll))
        , tolerance(0.0)
        , opacity(1.0)
        , antialiasing(true) {}
};

class OGC_DRAW_API StateSerializer {
public:
    static EngineState Capture(const DrawEngine* engine);
    static void Restore(DrawEngine* engine, const EngineState& state);
    static void Merge(EngineState& target, const EngineState& source, StateFlags flags);

private:
    StateSerializer() = delete;
    ~StateSerializer() = delete;
};

}  
}  

#endif  
