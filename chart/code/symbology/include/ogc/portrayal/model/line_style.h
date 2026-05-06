#ifndef OGC_PORTRAYAL_MODEL_LINE_STYLE_H
#define OGC_PORTRAYAL_MODEL_LINE_STYLE_H

#include "ogc/portrayal/model/file_reference.h"

namespace ogc {
namespace portrayal {
namespace model {

class LineStyle : public FileReference {
public:
    LineStyle() = default;
    using FileReference::FileReference;
};

} // namespace model
} // namespace portrayal
} // namespace ogc

#endif
