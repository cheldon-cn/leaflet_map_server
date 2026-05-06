#ifndef OGC_PORTRAYAL_MODEL_ALERT_CATALOG_H
#define OGC_PORTRAYAL_MODEL_ALERT_CATALOG_H

#include "ogc/portrayal/model/file_reference.h"

namespace ogc {
namespace portrayal {
namespace model {

class AlertCatalog : public FileReference {
public:
    AlertCatalog() = default;
    using FileReference::FileReference;
};

} // namespace model
} // namespace portrayal
} // namespace ogc

#endif
