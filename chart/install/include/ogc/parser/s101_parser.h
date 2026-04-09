#ifndef S101_PARSER_H
#define S101_PARSER_H

#include "iparser.h"
#include "gdal_initializer.h"
#include <memory>

namespace chart {
namespace parser {

class S101GMLParser;

class S101Parser : public IParser {
public:
    S101Parser();
    virtual ~S101Parser();
    
    ParseResult ParseChart(const std::string& filePath, const ParseConfig& config = ParseConfig()) override;
    
    bool ParseFeature(const std::string& data, Feature& feature) override;
    
    std::vector<ChartFormat> GetSupportedFormats() const override;
    
    std::string GetName() const override { return "S101Parser"; }
    std::string GetVersion() const override { return "1.0.0"; }

private:
    bool OpenDataset(const std::string& filePath, void** dataset);
    void CloseDataset(void* dataset);
    
    bool ParseLayer(void* dataset, const std::string& layerName, std::vector<Feature>& features, const ParseConfig& config);
    bool ParseGMLFeature(void* feature, Feature& outFeature, const ParseConfig& config);
    
    std::unique_ptr<S101GMLParser> m_gmlParser;
};

} // namespace parser
} // namespace chart

#endif // S101_PARSER_H
