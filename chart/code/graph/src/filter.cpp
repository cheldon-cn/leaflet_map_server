#include "ogc/draw/filter.h"

namespace ogc {
namespace draw {

FilterPtr Filter::CreateAll() {
    return std::make_shared<NullFilter>();
}

FilterPtr Filter::CreateNone() {
    return std::make_shared<NoneFilter>();
}

bool Filter::IsAll() const {
    return dynamic_cast<const NullFilter*>(this) != nullptr;
}

bool Filter::IsNone() const {
    return dynamic_cast<const NoneFilter*>(this) != nullptr;
}

}
}
