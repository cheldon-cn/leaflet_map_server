#include "ogc/portrayal/model/viewing_group.h"

namespace ogc {
namespace portrayal {
namespace model {

const std::string& ViewingGroup::GetId() const {
    return id_;
}

const Description& ViewingGroup::GetDescription() const {
    return description_;
}

void ViewingGroup::SetId(const std::string& id) {
    id_ = id;
}

void ViewingGroup::SetDescription(const Description& desc) {
    description_ = desc;
}

} // namespace model
} // namespace portrayal
} // namespace ogc
