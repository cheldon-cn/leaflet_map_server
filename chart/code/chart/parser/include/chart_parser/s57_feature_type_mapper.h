#ifndef S57_FEATURE_TYPE_MAPPER_H
#define S57_FEATURE_TYPE_MAPPER_H

#include "parse_result.h"
#include <string>
#include <map>

namespace chart {
namespace parser {

class S57FeatureTypeMapper {
public:
    S57FeatureTypeMapper();
    ~S57FeatureTypeMapper();
    
    FeatureType MapFeatureType(const std::string& className);
    
private:
    void InitializeMapping();
    
    std::map<std::string, FeatureType> m_typeMapping;
};

} // namespace parser
} // namespace chart

#endif // S57_FEATURE_TYPE_MAPPER_H
