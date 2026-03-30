#include "ogc/mokrender/annotation_generator.h"
#include "ogc/point.h"
#include "ogc/coordinate.h"
#include <random>
#include <sstream>

namespace ogc {
namespace mokrender {

class AnnotationGenerator::Impl {
public:
    double minX, minY, maxX, maxY;
    int srid;
    std::mt19937 rng;
    std::uniform_real_distribution<double> xDist;
    std::uniform_real_distribution<double> yDist;
    int counter;
    bool initialized;
    
    Impl() : minX(0), minY(0), maxX(1000), maxY(1000), srid(4326),
             counter(0), initialized(false) {
        rng.seed(45);
    }
};

AnnotationGenerator::AnnotationGenerator() : m_impl(new Impl()) {
}

AnnotationGenerator::~AnnotationGenerator() {
}

MokRenderResult AnnotationGenerator::Initialize(double minX, double minY,
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

void* AnnotationGenerator::GenerateFeature() {
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
    
    ++m_impl->counter;
    
    return pointPtr.release();
}

std::string AnnotationGenerator::GetLayerName() const {
    return "annotation_layer";
}

std::string AnnotationGenerator::GetGeometryType() const {
    return "Point";
}

}
}
