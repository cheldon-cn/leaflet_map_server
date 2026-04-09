#include "ogc/mokrender/multipolygon_generator.h"
#include "ogc/geom/multipolygon.h"
#include "ogc/geom/polygon.h"
#include "ogc/geom/linearring.h"
#include "ogc/geom/coordinate.h"
#include <random>
#include <vector>
#include <cmath>

namespace ogc {
namespace mokrender {

class MultiPolygonGenerator::Impl {
public:
    double minX, minY, maxX, maxY;
    int srid;
    std::mt19937 rng;
    std::uniform_real_distribution<double> xDist;
    std::uniform_real_distribution<double> yDist;
    std::uniform_int_distribution<int> polyCountDist;
    std::uniform_int_distribution<int> pointCountDist;
    bool initialized;
    
    Impl() : minX(0), minY(0), maxX(1000), maxY(1000), srid(4326),
             polyCountDist(2, 3), pointCountDist(4, 8), initialized(false) {
        rng.seed(48);
    }
};

MultiPolygonGenerator::MultiPolygonGenerator() : m_impl(new Impl()) {
}

MultiPolygonGenerator::~MultiPolygonGenerator() {
}

MokRenderResult MultiPolygonGenerator::Initialize(double minX, double minY,
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

void* MultiPolygonGenerator::GenerateFeature() {
    if (!m_impl->initialized) {
        return nullptr;
    }
    
    int numPolygons = m_impl->polyCountDist(m_impl->rng);
    MultiPolygonPtr multiPolygonPtr = MultiPolygon::Create();
    
    for (int p = 0; p < numPolygons; ++p) {
        double centerX = m_impl->xDist(m_impl->rng);
        double centerY = m_impl->yDist(m_impl->rng);
        
        int numPoints = m_impl->pointCountDist(m_impl->rng);
        std::vector<Coordinate> coords;
        
        double radius = 10.0 + (m_impl->rng() % 30);
        
        for (int i = 0; i < numPoints; ++i) {
            double angle = 2.0 * 3.14159265358979323846 * i / numPoints;
            double x = centerX + radius * std::cos(angle);
            double y = centerY + radius * std::sin(angle);
            coords.push_back(Coordinate(x, y));
        }
        coords.push_back(coords[0]);
        
        LinearRingPtr exteriorRingPtr = LinearRing::Create(coords);
        PolygonPtr polygonPtr = Polygon::Create(std::move(exteriorRingPtr));
        
        if (polygonPtr) {
            polygonPtr->SetSRID(m_impl->srid);
            multiPolygonPtr->AddPolygon(std::move(polygonPtr));
        }
    }
    
    if (multiPolygonPtr) {
        multiPolygonPtr->SetSRID(m_impl->srid);
    }
    
    return multiPolygonPtr.release();
}

std::string MultiPolygonGenerator::GetLayerName() const {
    return "multipolygon_layer";
}

std::string MultiPolygonGenerator::GetGeometryType() const {
    return "MultiPolygon";
}

}
}
