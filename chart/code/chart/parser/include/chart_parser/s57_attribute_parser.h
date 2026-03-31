#ifndef S57_ATTRIBUTE_PARSER_H
#define S57_ATTRIBUTE_PARSER_H

#include "parse_result.h"
#include <string>

namespace chart {
namespace parser {

class S57AttributeParser {
public:
    S57AttributeParser();
    ~S57AttributeParser();
    
    void ParseAttributes(void* feature, AttributeMap& attributes);
    
    std::string GetStringValue(const AttributeMap& attrs, const std::string& name, const std::string& defaultValue = "");
    int GetIntValue(const AttributeMap& attrs, const std::string& name, int defaultValue = 0);
    double GetDoubleValue(const AttributeMap& attrs, const std::string& name, double defaultValue = 0.0);
};

} // namespace parser
} // namespace chart

#endif // S57_ATTRIBUTE_PARSER_H
