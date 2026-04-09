#ifndef IPARSER_H
#define IPARSER_H

#include <string>
#include <vector>
#include "chart_format.h"
#include "parse_config.h"
#include "parse_result.h"

namespace chart {
namespace parser {

class IParser {
public:
    virtual ~IParser() = default;
    
    virtual ParseResult ParseChart(const std::string& filePath, const ParseConfig& config = ParseConfig()) = 0;
    
    virtual bool ParseFeature(const std::string& data, Feature& feature) = 0;
    
    virtual std::vector<ChartFormat> GetSupportedFormats() const = 0;
    
    virtual bool SupportsFormat(ChartFormat format) const {
        auto formats = GetSupportedFormats();
        for (const auto& f : formats) {
            if (f == format) return true;
        }
        return false;
    }
    
    virtual std::string GetName() const = 0;
    virtual std::string GetVersion() const = 0;
};

} // namespace parser
} // namespace chart

#endif // IPARSER_H
