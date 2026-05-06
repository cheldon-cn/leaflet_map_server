#ifndef OGC_PORTRAYAL_MODEL_AREA_FILL_H
#define OGC_PORTRAYAL_MODEL_AREA_FILL_H

#include "ogc/portrayal/model/file_reference.h"

namespace ogc {
namespace portrayal {
namespace model {

class AreaFill : public FileReference {
public:
    AreaFill() = default;
    using FileReference::FileReference;
};

} // namespace model
} // namespace portrayal
} // namespace ogc

#endif
