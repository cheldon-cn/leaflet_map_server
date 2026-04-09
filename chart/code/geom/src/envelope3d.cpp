#include "ogc/geom/envelope3d.h"
#include "ogc/geom/common.h"
#include <sstream>
#include <iomanip>

namespace ogc {

std::string Envelope3D::ToWKT() const {
    if (IsNull()) return "ENVELOPE EMPTY";
    
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(DEFAULT_WKT_PRECISION);
    
    if (m_hasZ) {
        oss << "ENVELOPE3D(" 
            << GetMinX() << " " << GetMinY() << " " << m_minZ << ", "
            << GetMaxX() << " " << GetMaxY() << " " << m_maxZ << ")";
    } else {
        oss << "ENVELOPE(" 
            << GetMinX() << " " << GetMinY() << ", "
            << GetMaxX() << " " << GetMaxY() << ")";
    }
    
    return oss.str();
}

std::string Envelope3D::ToString() const {
    if (IsNull()) return "Envelope3D[NULL]";
    
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(DEFAULT_WKT_PRECISION);
    
    oss << "Envelope3D[";
    oss << GetMinX() << ", " << GetMinY();
    if (m_hasZ) oss << ", " << m_minZ;
    oss << " : ";
    oss << GetMaxX() << ", " << GetMaxY();
    if (m_hasZ) oss << ", " << m_maxZ;
    oss << "]";
    
    return oss.str();
}

}
