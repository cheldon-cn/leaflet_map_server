#ifndef PARSE_RESULT_H
#define PARSE_RESULT_H

#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include "error_codes.h"

namespace chart {
namespace parser {

enum class GeometryType {
    Unknown = 0,
    Point,
    Line,
    Area,
    MultiPoint,
    MultiLine,
    MultiArea
};

inline std::string GeometryTypeToString(GeometryType type) {
    switch (type) {
        case GeometryType::Point: return "Point";
        case GeometryType::Line: return "Line";
        case GeometryType::Area: return "Area";
        case GeometryType::MultiPoint: return "MultiPoint";
        case GeometryType::MultiLine: return "MultiLine";
        case GeometryType::MultiArea: return "MultiArea";
        default: return "Unknown";
    }
}

struct Point {
    double x;
    double y;
    double z;
    
    Point() : x(0), y(0), z(0) {}
    Point(double _x, double _y, double _z = 0) : x(_x), y(_y), z(_z) {}
};

struct Geometry {
    GeometryType type;
    std::vector<Point> points;
    std::vector<std::vector<Point>> rings;
    
    Geometry() : type(GeometryType::Unknown) {}
};

enum class FeatureType {
    Unknown = 0,
    SOUNDG,
    LIGHTS,
    BOYLAT,
    BOYSAW,
    BCNLAT,
    BCNSAW,
    DEPARE,
    DEPCNT,
    LNDARE,
    LNDELV,
    OBSTRN,
    WRECKS,
    UWTROC,
    COALNE,
    SLCONS,
    BUAARE,
    RESARE,
    NAVLNE,
    RADSTA,
    RTPBCN,
    SBDARE,
    SEAARE,
    TSSLPT,
    TSEZNE,
    DRGARE,
    FAIRWY,
    HULKES,
    PONTON,
    PILPNT,
    MORFAC,
    GATCON,
    LOCK,
    CBLARE,
    CBLOHD,
    BERTHS,
    BRIDGE,
    CAUSWY,
    DISMAR,
    FLODOC,
    HRBARE,
    HRBFAC,
    LOKBSN,
    PRDARE,
    PYLONS,
    TUNNEL,
    WTWARE,
    WTWAXS,
    MIPARE,
    OSPARE,
    OSTRBN,
    RDOSTA,
    RDOCAL,
    RETRFL,
    SILTNK,
    SISTAT,
    TOPMAR,
    VEGATN,
    WATFAL,
    WEDKLP,
    WEDRTW
};

inline std::string FeatureTypeToString(FeatureType type) {
    switch (type) {
        case FeatureType::SOUNDG: return "SOUNDG";
        case FeatureType::LIGHTS: return "LIGHTS";
        case FeatureType::BOYLAT: return "BOYLAT";
        case FeatureType::BOYSAW: return "BOYSAW";
        case FeatureType::BCNLAT: return "BCNLAT";
        case FeatureType::BCNSAW: return "BCNSAW";
        case FeatureType::DEPARE: return "DEPARE";
        case FeatureType::DEPCNT: return "DEPCNT";
        case FeatureType::LNDARE: return "LNDARE";
        case FeatureType::LNDELV: return "LNDELV";
        case FeatureType::OBSTRN: return "OBSTRN";
        case FeatureType::WRECKS: return "WRECKS";
        case FeatureType::UWTROC: return "UWTROC";
        case FeatureType::COALNE: return "COALNE";
        case FeatureType::SLCONS: return "SLCONS";
        case FeatureType::BUAARE: return "BUAARE";
        case FeatureType::RESARE: return "RESARE";
        case FeatureType::NAVLNE: return "NAVLNE";
        case FeatureType::RADSTA: return "RADSTA";
        case FeatureType::RTPBCN: return "RTPBCN";
        case FeatureType::SBDARE: return "SBDARE";
        case FeatureType::SEAARE: return "SEAARE";
        case FeatureType::TSSLPT: return "TSSLPT";
        case FeatureType::TSEZNE: return "TSEZNE";
        case FeatureType::DRGARE: return "DRGARE";
        case FeatureType::FAIRWY: return "FAIRWY";
        case FeatureType::HULKES: return "HULKES";
        case FeatureType::PONTON: return "PONTON";
        case FeatureType::PILPNT: return "PILPNT";
        case FeatureType::MORFAC: return "MORFAC";
        case FeatureType::GATCON: return "GATCON";
        case FeatureType::LOCK: return "LOCK";
        case FeatureType::CBLARE: return "CBLARE";
        case FeatureType::CBLOHD: return "CBLOHD";
        case FeatureType::BERTHS: return "BERTHS";
        case FeatureType::BRIDGE: return "BRIDGE";
        case FeatureType::CAUSWY: return "CAUSWY";
        case FeatureType::DISMAR: return "DISMAR";
        case FeatureType::FLODOC: return "FLODOC";
        case FeatureType::HRBARE: return "HRBARE";
        case FeatureType::HRBFAC: return "HRBFAC";
        case FeatureType::LOKBSN: return "LOKBSN";
        case FeatureType::PRDARE: return "PRDARE";
        case FeatureType::PYLONS: return "PYLONS";
        case FeatureType::TUNNEL: return "TUNNEL";
        case FeatureType::WTWARE: return "WTWARE";
        case FeatureType::WTWAXS: return "WTWAXS";
        case FeatureType::MIPARE: return "MIPARE";
        case FeatureType::OSPARE: return "OSPARE";
        case FeatureType::OSTRBN: return "OSTRBN";
        case FeatureType::RDOSTA: return "RDOSTA";
        case FeatureType::RDOCAL: return "RDOCAL";
        case FeatureType::RETRFL: return "RETRFL";
        case FeatureType::SILTNK: return "SILTNK";
        case FeatureType::SISTAT: return "SISTAT";
        case FeatureType::TOPMAR: return "TOPMAR";
        case FeatureType::VEGATN: return "VEGATN";
        case FeatureType::WATFAL: return "WATFAL";
        case FeatureType::WEDKLP: return "WEDKLP";
        case FeatureType::WEDRTW: return "WEDRTW";
        default: return "Unknown";
    }
}

struct AttributeValue {
    enum class Type {
        Integer,
        Double,
        String,
        List,
        Unknown
    };
    
    Type type;
    int intValue;
    double doubleValue;
    std::string stringValue;
    std::vector<std::string> listValue;
    
    AttributeValue() : type(Type::Unknown), intValue(0), doubleValue(0.0) {}
};

using AttributeMap = std::map<std::string, AttributeValue>;

struct Feature {
    std::string id;
    FeatureType type;
    Geometry geometry;
    AttributeMap attributes;
    std::string className;
    int32_t rcid;
    
    Feature() : type(FeatureType::Unknown), rcid(0) {}
};

struct ParseStatistics {
    int32_t totalFeatureCount;
    int32_t successCount;
    int32_t failedCount;
    int32_t skippedCount;
    double parseTimeMs;
    double memoryUsedMB;
    
    ParseStatistics()
        : totalFeatureCount(0)
        , successCount(0)
        , failedCount(0)
        , skippedCount(0)
        , parseTimeMs(0.0)
        , memoryUsedMB(0.0)
    {
    }
};

struct ParseResult {
    bool success;
    ErrorCode errorCode;
    std::string errorMessage;
    std::string filePath;
    
    std::vector<Feature> features;
    ParseStatistics statistics;
    
    std::map<std::string, std::string> metadata;
    
    ParseResult()
        : success(false)
        , errorCode(ErrorCode::Success)
    {
    }
    
    bool HasError() const { return errorCode != ErrorCode::Success; }
    
    void SetError(ErrorCode code, const std::string& message) {
        success = false;
        errorCode = code;
        errorMessage = message;
    }
    
    void ClearError() {
        success = true;
        errorCode = ErrorCode::Success;
        errorMessage.clear();
    }
};

} // namespace parser
} // namespace chart

#endif // PARSE_RESULT_H
