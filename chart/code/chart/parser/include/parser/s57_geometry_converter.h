#ifndef S57_GEOMETRY_CONVERTER_H
#define S57_GEOMETRY_CONVERTER_H

#include "parse_result.h"

namespace chart {
namespace parser {

class S57GeometryConverter {
public:
    S57GeometryConverter();
    ~S57GeometryConverter();
    
    bool ConvertGeometry(void* ogrGeometry, Geometry& outGeometry);
    
private:
    bool ConvertPoint(void* geom, Geometry& outGeometry);
    bool ConvertLineString(void* geom, Geometry& outGeometry);
    bool ConvertPolygon(void* geom, Geometry& outGeometry);
    bool ConvertMultiPoint(void* geom, Geometry& outGeometry);
    bool ConvertMultiLineString(void* geom, Geometry& outGeometry);
    bool ConvertMultiPolygon(void* geom, Geometry& outGeometry);
};

} // namespace parser
} // namespace chart

#endif // S57_GEOMETRY_CONVERTER_H
