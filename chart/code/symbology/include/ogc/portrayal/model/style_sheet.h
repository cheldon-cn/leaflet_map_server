#ifndef OGC_PORTRAYAL_MODEL_STYLE_SHEET_H
#define OGC_PORTRAYAL_MODEL_STYLE_SHEET_H

#include "ogc/portrayal/model/file_reference.h"

namespace ogc {
namespace portrayal {
namespace model {

class StyleSheet : public FileReference {
public:
    StyleSheet() = default;
    using FileReference::FileReference;
};

} // namespace model
} // namespace portrayal
} // namespace ogc

#endif
