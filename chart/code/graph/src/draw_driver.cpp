#include "ogc/draw/draw_driver.h"

namespace ogc {
namespace draw {

DrawDriverPtr DrawDriver::Create(const std::string& name) {
    (void)name;
    return nullptr;
}

}  
}  
