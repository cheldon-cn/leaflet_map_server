#include "parser/s57_attribute_parser.h"
#include "parser/error_handler.h"

#include <ogr_feature.h>

namespace chart {
namespace parser {

S57AttributeParser::S57AttributeParser() {
}

S57AttributeParser::~S57AttributeParser() {
}

void S57AttributeParser::ParseAttributes(void* feature, AttributeMap& attributes) {
    OGRFeature* ogrFeature = static_cast<OGRFeature*>(feature);
    
    if (!ogrFeature) {
        LOG_ERROR("Null feature");
        return;
    }
    
    OGRFeatureDefn* featureDefn = ogrFeature->GetDefnRef();
    if (!featureDefn) {
        return;
    }
    
    int fieldCount = featureDefn->GetFieldCount();
    
    for (int i = 0; i < fieldCount; ++i) {
        OGRFieldDefn* fieldDefn = featureDefn->GetFieldDefn(i);
        if (!fieldDefn) {
            continue;
        }
        
        std::string fieldName = fieldDefn->GetNameRef();
        
        if (!ogrFeature->IsFieldSet(i) || ogrFeature->IsFieldNull(i)) {
            continue;
        }
        
        AttributeValue value;
        OGRFieldType fieldType = fieldDefn->GetType();
        
        switch (fieldType) {
            case OFTInteger:
                value.type = AttributeValue::Type::Integer;
                value.intValue = ogrFeature->GetFieldAsInteger(i);
                break;
            
            case OFTInteger64:
                value.type = AttributeValue::Type::Integer;
                value.intValue = static_cast<int>(ogrFeature->GetFieldAsInteger64(i));
                break;
            
            case OFTReal:
                value.type = AttributeValue::Type::Double;
                value.doubleValue = ogrFeature->GetFieldAsDouble(i);
                break;
            
            case OFTString:
                value.type = AttributeValue::Type::String;
                value.stringValue = ogrFeature->GetFieldAsString(i);
                break;
            
            case OFTIntegerList: {
                value.type = AttributeValue::Type::List;
                int count = 0;
                const int* values = ogrFeature->GetFieldAsIntegerList(i, &count);
                for (int j = 0; j < count; ++j) {
                    value.listValue.push_back(std::to_string(values[j]));
                }
                break;
            }
            
            case OFTRealList: {
                value.type = AttributeValue::Type::List;
                int count = 0;
                const double* values = ogrFeature->GetFieldAsDoubleList(i, &count);
                for (int j = 0; j < count; ++j) {
                    value.listValue.push_back(std::to_string(values[j]));
                }
                break;
            }
            
            case OFTStringList: {
                value.type = AttributeValue::Type::List;
                char** values = ogrFeature->GetFieldAsStringList(i);
                if (values) {
                    for (int j = 0; values[j] != nullptr; ++j) {
                        value.listValue.push_back(values[j]);
                    }
                }
                break;
            }
            
            default:
                value.type = AttributeValue::Type::String;
                value.stringValue = ogrFeature->GetFieldAsString(i);
                break;
        }
        
        attributes[fieldName] = value;
    }
}

std::string S57AttributeParser::GetStringValue(const AttributeMap& attrs, const std::string& name, const std::string& defaultValue) {
    auto it = attrs.find(name);
    if (it == attrs.end()) {
        return defaultValue;
    }
    
    const AttributeValue& value = it->second;
    
    switch (value.type) {
        case AttributeValue::Type::String:
            return value.stringValue;
        
        case AttributeValue::Type::Integer:
            return std::to_string(value.intValue);
        
        case AttributeValue::Type::Double:
            return std::to_string(value.doubleValue);
        
        default:
            return defaultValue;
    }
}

int S57AttributeParser::GetIntValue(const AttributeMap& attrs, const std::string& name, int defaultValue) {
    auto it = attrs.find(name);
    if (it == attrs.end()) {
        return defaultValue;
    }
    
    const AttributeValue& value = it->second;
    
    switch (value.type) {
        case AttributeValue::Type::Integer:
            return value.intValue;
        
        case AttributeValue::Type::Double:
            return static_cast<int>(value.doubleValue);
        
        case AttributeValue::Type::String:
            try {
                return std::stoi(value.stringValue);
            } catch (...) {
                return defaultValue;
            }
        
        default:
            return defaultValue;
    }
}

double S57AttributeParser::GetDoubleValue(const AttributeMap& attrs, const std::string& name, double defaultValue) {
    auto it = attrs.find(name);
    if (it == attrs.end()) {
        return defaultValue;
    }
    
    const AttributeValue& value = it->second;
    
    switch (value.type) {
        case AttributeValue::Type::Double:
            return value.doubleValue;
        
        case AttributeValue::Type::Integer:
            return static_cast<double>(value.intValue);
        
        case AttributeValue::Type::String:
            try {
                return std::stod(value.stringValue);
            } catch (...) {
                return defaultValue;
            }
        
        default:
            return defaultValue;
    }
}

} // namespace parser
} // namespace chart
