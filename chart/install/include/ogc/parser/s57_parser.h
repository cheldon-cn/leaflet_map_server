#ifndef S57_PARSER_H
#define S57_PARSER_H

#include "iparser.h"
#include "gdal_initializer.h"
#include <memory>

namespace chart {
namespace parser {

class S57GeometryConverter;
class S57AttributeParser;
class S57FeatureTypeMapper;

class S57Parser : public IParser {
public:
    S57Parser();
    virtual ~S57Parser();
    
    ParseResult ParseChart(const std::string& filePath, const ParseConfig& config = ParseConfig()) override;
    
    bool ParseFeature(const std::string& data, Feature& feature) override;
    
    std::vector<ChartFormat> GetSupportedFormats() const override;
    
    std::string GetName() const override { return "S57Parser"; }
    std::string GetVersion() const override { return "1.0.0"; }

private:
    bool OpenDataset(const std::string& filePath, void** dataset);
    void CloseDataset(void* dataset);
    
    bool ParseLayer(void* dataset, const std::string& layerName, std::vector<Feature>& features, const ParseConfig& config);
    bool ParseOGRFeature(void* feature, Feature& outFeature, const ParseConfig& config);
    
    std::unique_ptr<S57GeometryConverter> m_geometryConverter;
    std::unique_ptr<S57AttributeParser> m_attributeParser;
    std::unique_ptr<S57FeatureTypeMapper> m_featureTypeMapper;
};

} // namespace parser
} // namespace chart

#endif // S57_PARSER_H
