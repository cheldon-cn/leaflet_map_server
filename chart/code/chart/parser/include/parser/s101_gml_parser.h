#ifndef S101_GML_PARSER_H
#define S101_GML_PARSER_H

#include "parse_result.h"

namespace chart {
namespace parser {

class S101GMLParser {
public:
    S101GMLParser();
    ~S101GMLParser();
    
    bool ConvertGeometry(void* ogrGeometry, Geometry& outGeometry);
    void ParseAttributes(void* feature, AttributeMap& attributes);
};

} // namespace parser
} // namespace chart

#endif // S101_GML_PARSER_H
