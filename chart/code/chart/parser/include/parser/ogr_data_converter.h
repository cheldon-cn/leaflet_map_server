#ifndef OGR_DATA_CONVERTER_H
#define OGR_DATA_CONVERTER_H

#include "data_converter.h"

namespace chart {
namespace parser {

class OGRDataConverter : public IDataConverter {
public:
    OGRDataConverter();
    ~OGRDataConverter() override;
    
    bool ConvertGeometry(void* sourceGeometry, Geometry& outGeometry) override;
    bool ConvertAttributes(void* sourceFeature, AttributeMap& outAttributes) override;
    
    std::string GetName() const override { return "OGRDataConverter"; }
    std::string GetSourceType() const override { return "OGR"; }
    
private:
    bool ConvertPoint(void* geometry, Geometry& outGeometry);
    bool ConvertLineString(void* geometry, Geometry& outGeometry);
    bool ConvertPolygon(void* geometry, Geometry& outGeometry);
    bool ConvertMultiPoint(void* geometry, Geometry& outGeometry);
    bool ConvertMultiLineString(void* geometry, Geometry& outGeometry);
    bool ConvertMultiPolygon(void* geometry, Geometry& outGeometry);
};

} // namespace parser
} // namespace chart

#endif // OGR_DATA_CONVERTER_H
