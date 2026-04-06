#ifndef OGC_GRAPH_LABEL_INFO_H
#define OGC_GRAPH_LABEL_INFO_H

#include "ogc/graph/export.h"
#include "ogc/envelope.h"
#include <string>
#include <cstdint>

namespace ogc {
namespace graph {

struct LabelInfo {
    std::string text;
    double x;
    double y;
    double width;
    double height;
    double rotation;
    int priority;
    bool visible;
    uint32_t featureId;
    
    LabelInfo()
        : x(0), y(0), width(0), height(0), rotation(0)
        , priority(0), visible(true), featureId(0) {}
    
    Envelope GetBounds() const {
        return Envelope(x, y, x + width, y + height);
    }
    
    bool Intersects(const LabelInfo& other) const {
        return GetBounds().Intersects(other.GetBounds());
    }
};

}
}

#endif
