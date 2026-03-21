#include "ogc/feature/field_type.h"

namespace ogc {

const char* ogc::GetFieldTypeName(CNFieldType type) {
    switch (type) {
        case CNFieldType::kInteger:     return "Integer";
        case CNFieldType::kInteger64:   return "Integer64";
        case CNFieldType::kReal:        return "Real";
        case CNFieldType::kString:      return "String";
        case CNFieldType::kDate:        return "Date";
        case CNFieldType::kTime:        return "Time";
        case CNFieldType::kDateTime:    return "DateTime";
        case CNFieldType::kBinary:      return "Binary";
        case CNFieldType::kIntegerList: return "IntegerList";
        case CNFieldType::kInteger64List: return "Integer64List";
        case CNFieldType::kRealList:    return "RealList";
        case CNFieldType::kStringList:  return "StringList";
        case CNFieldType::kBoolean:     return "Boolean";
        case CNFieldType::kWideString:  return "WideString";
        case CNFieldType::kNull:        return "Null";
        case CNFieldType::kUnset:       return "Unset";
        case CNFieldType::kUnknown:     return "Unknown";
        default:                        return "Unknown";
    }
}

std::string ogc::GetFieldTypeDescription(CNFieldType type) {
    switch (type) {
        case CNFieldType::kInteger:
            return "32-bit signed integer";
        case CNFieldType::kInteger64:
            return "64-bit signed integer";
        case CNFieldType::kReal:
            return "64-bit IEEE 754 floating point";
        case CNFieldType::kString:
            return "UTF-8 text string";
        case CNFieldType::kDate:
            return "Date (year, month, day)";
        case CNFieldType::kTime:
            return "Time (hour, minute, second)";
        case CNFieldType::kDateTime:
            return "Date and time";
        case CNFieldType::kBinary:
            return "Raw binary data";
        case CNFieldType::kIntegerList:
            return "List of 32-bit integers";
        case CNFieldType::kInteger64List:
            return "List of 64-bit integers";
        case CNFieldType::kRealList:
            return "List of floating point numbers";
        case CNFieldType::kStringList:
            return "List of strings";
        case CNFieldType::kBoolean:
            return "Boolean value";
        case CNFieldType::kWideString:
            return "Wide character string";
        case CNFieldType::kNull:
            return "Null value";
        case CNFieldType::kUnset:
            return "Unset value";
        case CNFieldType::kUnknown:
            return "Unknown type";
        default:
            return "Unknown type";
    }
}

bool ogc::IsListType(CNFieldType type) {
    return type == CNFieldType::kIntegerList ||
           type == CNFieldType::kInteger64List ||
           type == CNFieldType::kRealList ||
           type == CNFieldType::kStringList;
}

CNFieldType ogc::GetListElementType(CNFieldType listType) {
    switch (listType) {
        case CNFieldType::kIntegerList:    return CNFieldType::kInteger;
        case CNFieldType::kInteger64List:   return CNFieldType::kInteger64;
        case CNFieldType::kRealList:       return CNFieldType::kReal;
        case CNFieldType::kStringList:     return CNFieldType::kString;
        default:                            return CNFieldType::kUnknown;
    }
}

} // namespace ogc
