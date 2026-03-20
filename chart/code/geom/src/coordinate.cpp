#include "ogc/coordinate.h"
#include <sstream>
#include <iomanip>

namespace ogc {

std::string Coordinate::ToWKT(int precision) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision);
    
    if (IsEmpty()) {
        return "EMPTY";
    }
    
    oss << x << " " << y;
    
    if (Is3D()) {
        oss << " " << z;
    }
    
    if (IsMeasured()) {
        oss << " " << m;
    }
    
    return oss.str();
}

}
