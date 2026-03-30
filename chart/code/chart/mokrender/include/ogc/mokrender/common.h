#pragma once

#include <string>
#include <cstdint>

namespace ogc {
namespace mokrender {

enum class MokRenderErrorCode {
    SUCCESS = 0,
    INVALID_PARAMETER = 1,
    DATABASE_ERROR = 2,
    GEOMETRY_ERROR = 3,
    RENDER_ERROR = 4,
    FILE_NOT_FOUND = 5,
    UNSUPPORTED_TYPE = 6,
    OUT_OF_MEMORY = 7,
    INTERNAL_ERROR = 99
};

struct MokRenderResult {
    MokRenderErrorCode code;
    std::string message;
    
    MokRenderResult() : code(MokRenderErrorCode::SUCCESS), message("") {}
    MokRenderResult(MokRenderErrorCode c, const std::string& msg) 
        : code(c), message(msg) {}
    
    bool IsSuccess() const { return code == MokRenderErrorCode::SUCCESS; }
    bool IsError() const { return code != MokRenderErrorCode::SUCCESS; }
};

struct DataGeneratorConfig {
    int pointCount;
    int lineCount;
    int polygonCount;
    int annotationCount;
    int rasterCount;
    double minX;
    double minY;
    double maxX;
    double maxY;
    int srid;
    unsigned int randomSeed;
    
    DataGeneratorConfig()
        : pointCount(20)
        , lineCount(20)
        , polygonCount(20)
        , annotationCount(20)
        , rasterCount(20)
        , minX(0.0)
        , minY(0.0)
        , maxX(1000.0)
        , maxY(1000.0)
        , srid(4326)
        , randomSeed(42) {}
};

struct RenderConfig {
    int outputWidth;
    int outputHeight;
    double dpi;
    std::string outputFormat;
    bool enableAntialiasing;
    
    RenderConfig()
        : outputWidth(800)
        , outputHeight(600)
        , dpi(96.0)
        , outputFormat("png")
        , enableAntialiasing(true) {}
};

}
}
