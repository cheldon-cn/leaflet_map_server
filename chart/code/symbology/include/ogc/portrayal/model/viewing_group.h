#ifndef OGC_PORTRAYAL_MODEL_VIEWING_GROUP_H
#define OGC_PORTRAYAL_MODEL_VIEWING_GROUP_H

#include "ogc/portrayal/model/description.h"
#include "../portrayal_export.h"
#include <string>

namespace ogc {
namespace portrayal {
namespace model {

class OGC_PORTRAYAL_API ViewingGroup {
public:
    ViewingGroup() = default;

    const std::string& GetId() const;
    const Description& GetDescription() const;

    void SetId(const std::string& id);
    void SetDescription(const Description& desc);

private:
    std::string id_;
    Description description_;
};

} // namespace model
} // namespace portrayal
} // namespace ogc

#endif
