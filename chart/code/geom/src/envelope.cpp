#include "ogc/envelope.h"
#include <sstream>
#include <iomanip>

namespace ogc {

std::string Envelope::ToWKT() const {
    if (m_isNull) {
        return "EMPTY";
    }
    
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(DEFAULT_WKT_PRECISION);
    oss << "POLYGON(("
        << m_minX << " " << m_minY << ", "
        << m_maxX << " " << m_minY << ", "
        << m_maxX << " " << m_maxY << ", "
        << m_minX << " " << m_maxY << ", "
        << m_minX << " " << m_minY << "))";
    return oss.str();
}

std::string Envelope::ToString() const {
    if (m_isNull) {
        return "Envelope[NULL]";
    }
    
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6);
    oss << "Envelope[(" << m_minX << ", " << m_minY << ") - ("
        << m_maxX << ", " << m_maxY << ")]";
    return oss.str();
}

}
