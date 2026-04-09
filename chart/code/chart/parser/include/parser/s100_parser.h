#ifndef S100_PARSER_H
#define S100_PARSER_H

#include "iparser.h"
#include "gdal_initializer.h"
#include <memory>

namespace chart {
namespace parser {

class S100DatasetInfo {
public:
    std::string productId;
    std::string editionNumber;
    std::string updateNumber;
    std::string issueDate;
    std::string boundingBox;
    std::string coordinateReferenceSystem;
    std::map<std::string, std::string> metadata;
};

class S100Parser : public IParser {
public:
    S100Parser();
    virtual ~S100Parser();
    
    ParseResult ParseChart(const std::string& filePath, const ParseConfig& config = ParseConfig()) override;
    
    bool ParseFeature(const std::string& data, Feature& feature) override;
    
    std::vector<ChartFormat> GetSupportedFormats() const override;
    
    std::string GetName() const override { return "S100Parser"; }
    std::string GetVersion() const override { return "1.0.0"; }
    
    virtual bool GetDatasetInfo(const std::string& filePath, S100DatasetInfo& info);
    
    static bool IsS100File(const std::string& filePath);

protected:
    bool OpenDataset(const std::string& filePath, void** dataset);
    void CloseDataset(void* dataset);
    
    bool ParseMetadata(void* dataset, S100DatasetInfo& info);
    bool ParseLayer(void* dataset, const std::string& layerName, std::vector<Feature>& features, const ParseConfig& config);
    bool ParseOGRFeature(void* feature, Feature& outFeature, const ParseConfig& config);
    
    virtual std::string DetectProductType(void* dataset);
};

} // namespace parser
} // namespace chart

#endif // S100_PARSER_H
