#ifndef OGC_PORTRAYAL_MODEL_SYMBOL_H
#define OGC_PORTRAYAL_MODEL_SYMBOL_H

#include "ogc/portrayal/model/file_reference.h"

namespace ogc {
namespace portrayal {
namespace model {

class Symbol : public FileReference {
public:
    Symbol() = default;
    using FileReference::FileReference;
};

} // namespace model
} // namespace portrayal
} // namespace ogc

#endif
