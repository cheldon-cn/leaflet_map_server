#include "ogc/mokrender/multilinestring_generator.h"
#include "ogc/multilinestring.h"
#include "ogc/linestring.h"
#include "ogc/coordinate.h"
#include <random>
#include <vector>

namespace ogc {
namespace mokrender {

class MultiLineStringGenerator::Impl {
public:
    double minX, minY, maxX, maxY;
    int srid;
    std::mt19937 rng;
    std::uniform_real_distribution<double> xDist;
    std::uniform_real_distribution<double> yDist;
    std::uniform_int_distribution<int> lineCountDist;
    std::uniform_int_distribution<int> pointCountDist;
    bool initialized;
    
    Impl() : minX(0), minY(0), maxX(1000), maxY(1000), srid(4326),
             lineCountDist(2, 4), pointCountDist(2, 6), initialized(false) {
        rng.seed(47);
    }
};

MultiLineStringGenerator::MultiLineStringGenerator() : m_impl(new Impl()) {
}

MultiLineStringGenerator::~MultiLineStringGenerator() {
}

MokRenderResult MultiLineStringGenerator::Initialize(double minX, double minY,
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

void* MultiLineStringGenerator::GenerateFeature() {
    if (!m_impl->initialized) {
        return nullptr;
    }
    
    int numLines = m_impl->lineCountDist(m_impl->rng);
    MultiLineStringPtr multiLineStringPtr = MultiLineString::Create();
    
    for (int j = 0; j < numLines; ++j) {
        int numPoints = m_impl->pointCountDist(m_impl->rng);
        std::vector<Coordinate> coords;
        
        for (int i = 0; i < numPoints; ++i) {
            double x = m_impl->xDist(m_impl->rng);
            double y = m_impl->yDist(m_impl->rng);
            coords.push_back(Coordinate(x, y));
        }
        
        LineStringPtr lineStringPtr = LineString::Create(coords);
        if (lineStringPtr) {
            lineStringPtr->SetSRID(m_impl->srid);
            multiLineStringPtr->AddLineString(std::move(lineStringPtr));
        }
    }
    
    if (multiLineStringPtr) {
        multiLineStringPtr->SetSRID(m_impl->srid);
    }
    
    return multiLineStringPtr.release();
}

std::string MultiLineStringGenerator::GetLayerName() const {
    return "multilinestring_layer";
}

std::string MultiLineStringGenerator::GetGeometryType() const {
    return "MultiLineString";
}

}
}
