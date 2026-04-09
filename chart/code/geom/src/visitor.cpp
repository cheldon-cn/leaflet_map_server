#include "ogc/geom/visitor.h"
#include "ogc/geom/geometry.h"
#include "ogc/geom/polygon.h"
#include "ogc/geom/multipolygon.h"

namespace ogc {

void AreaCalculator::VisitPolygon(const Polygon* polygon) {
    m_area = polygon->Area();
}

void AreaCalculator::VisitMultiPolygon(const MultiPolygon* multiPolygon) {
    m_area = 0.0;
    for (size_t i = 0; i < multiPolygon->GetNumGeometries(); ++i) {
        const Geometry* geom = multiPolygon->GetGeometryN(i);
        if (geom->GetGeometryType() == GeomType::kPolygon) {
            m_area += static_cast<const Polygon*>(geom)->Area();
        }
    }
}

}
