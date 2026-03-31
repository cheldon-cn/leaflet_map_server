#ifndef PARSE_CONFIG_H
#define PARSE_CONFIG_H

#include <string>
#include <cstdint>

namespace chart {
namespace parser {

struct ParseConfig {
    bool validateGeometry;
    bool validateAttributes;
    bool strictMode;
    bool includeMetadata;
    int32_t maxFeatureCount;
    std::string coordinateSystem;
    double tolerance;
    
    ParseConfig()
        : validateGeometry(true)
        , validateAttributes(true)
        , strictMode(false)
        , includeMetadata(true)
        , maxFeatureCount(0)
        , coordinateSystem("EPSG:4326")
        , tolerance(0.0001)
    {
    }
};

} // namespace parser
} // namespace chart

#endif // PARSE_CONFIG_H
