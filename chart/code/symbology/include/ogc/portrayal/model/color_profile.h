#ifndef OGC_PORTRAYAL_MODEL_COLOR_PROFILE_H
#define OGC_PORTRAYAL_MODEL_COLOR_PROFILE_H

#include "ogc/portrayal/model/file_reference.h"

namespace ogc {
namespace portrayal {
namespace model {

class ColorProfile : public FileReference {
public:
    ColorProfile() = default;
    using FileReference::FileReference;
};

} // namespace model
} // namespace portrayal
} // namespace ogc

#endif
