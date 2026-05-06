#include "ogc/portrayal/model/description.h"

namespace ogc {
namespace portrayal {
namespace model {

Description::Description(const std::string& name,
                         const std::string& desc,
                         const std::string& lang)
    : name_(name)
    , description_(desc)
    , language_(lang)
{
}

const std::string& Description::GetName() const {
    return name_;
}

const std::string& Description::GetDescription() const {
    return description_;
}

const std::string& Description::GetLanguage() const {
    return language_;
}

void Description::SetName(const std::string& name) {
    name_ = name;
}

void Description::SetDescription(const std::string& desc) {
    description_ = desc;
}

void Description::SetLanguage(const std::string& lang) {
    language_ = lang;
}

} // namespace model
} // namespace portrayal
} // namespace ogc
