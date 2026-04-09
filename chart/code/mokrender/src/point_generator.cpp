#include "ogc/mokrender/point_generator.h"
#include "ogc/geom/point.h"
#include "ogc/geom/coordinate.h"
#include <random>

namespace ogc {
namespace mokrender {

class PointGenerator::Impl {
public:
    double minX, minY, maxX, maxY;
    int srid;
    std::mt19937 rng;
    std::uniform_real_distribution<double> xDist;
    std::uniform_real_distribution<double> yDist;
    bool initialized;
    
    Impl() : minX(0), minY(0), maxX(1000), maxY(1000), srid(4326), 
             initialized(false) {
        rng.seed(42);
    }
};

PointGenerator::PointGenerator() : m_impl(new Impl()) {
}

PointGenerator::~PointGenerator() {
}

MokRenderResult PointGenerator::Initialize(double minX, double minY,
                                           double maxX, double maxY, int srid) {
    m_impl->minX = minX;
    m_impl->minY = minY;
    m_impl->maxX = maxX;
    m_impl->maxY = maxY;
    m_impl->srid = srid;
    
    m_impl->xDist = std::uniform_real_distribution<double>(minX, maxX);
    m_impl->yDist = std::uniform_real_distribution<double>(minY, maxY);
    m_impl->initialized = true;
    
    return MokRenderResult();
}

void* PointGenerator::GenerateFeature() {
    if (!m_impl->initialized) {
        return nullptr;
    }
    
    double x = m_impl->xDist(m_impl->rng);
    double y = m_impl->yDist(m_impl->rng);
    
    Coordinate coord(x, y);
    PointPtr pointPtr = Point::Create(coord);
    
    if (pointPtr) {
        pointPtr->SetSRID(m_impl->srid);
    }
    
    return pointPtr.release();
}

std::string PointGenerator::GetLayerName() const {
    return "point_layer";
}

std::string PointGenerator::GetGeometryType() const {
    return "Point";
}

}
}
