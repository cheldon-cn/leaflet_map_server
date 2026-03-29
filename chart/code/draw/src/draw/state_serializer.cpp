#include "ogc/draw/state_serializer.h"
#include "ogc/draw/draw_engine.h"

namespace ogc {
namespace draw {

EngineState StateSerializer::Capture(const DrawEngine* engine) {
    EngineState state;
    
    if (!engine) {
        return state;
    }
    
    state.transform = engine->GetTransform();
    state.clipRegion = engine->GetClipRegion();
    state.opacity = engine->GetOpacity();
    state.antialiasing = engine->IsAntialiasingEnabled();
    state.tolerance = engine->GetTolerance();
    state.savedFlags = static_cast<StateFlags>(StateFlag::kAll);
    
    return state;
}

void StateSerializer::Restore(DrawEngine* engine, const EngineState& state) {
    if (!engine) {
        return;
    }
    
    if (state.savedFlags & static_cast<StateFlags>(StateFlag::kTransform)) {
        engine->SetTransform(state.transform);
    }
    
    if (state.savedFlags & static_cast<StateFlags>(StateFlag::kClip)) {
        engine->SetClipRegion(state.clipRegion);
    }
    
    if (state.savedFlags & static_cast<StateFlags>(StateFlag::kOpacity)) {
        engine->SetOpacity(state.opacity);
    }
    
    engine->SetAntialiasingEnabled(state.antialiasing);
    engine->SetTolerance(state.tolerance);
}

void StateSerializer::Merge(EngineState& target, const EngineState& source, StateFlags flags) {
    if (flags & static_cast<StateFlags>(StateFlag::kTransform)) {
        target.transform = source.transform;
    }
    if (flags & static_cast<StateFlags>(StateFlag::kClip)) {
        target.clipRegion = source.clipRegion;
    }
    if (flags & static_cast<StateFlags>(StateFlag::kOpacity)) {
        target.opacity = source.opacity;
    }
    if (flags & static_cast<StateFlags>(StateFlag::kStyle)) {
        target.background = source.background;
        target.antialiasing = source.antialiasing;
        target.tolerance = source.tolerance;
    }
    target.savedFlags = flags;
}

}  
}  
