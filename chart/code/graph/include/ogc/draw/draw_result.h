#ifndef OGC_DRAW_DRAW_RESULT_H
#define OGC_DRAW_DRAW_RESULT_H

#include "ogc/draw/export.h"
#include <string>

namespace ogc {
namespace draw {

enum class DrawResult {
    kSuccess = 0,
    kFailed = 1,
    kInvalidParams = 2,
    kDeviceNotReady = 3,
    kEngineNotReady = 4,
    kOutOfMemory = 5,
    kFileNotFound = 6,
    kUnsupportedFormat = 7,
    kOperationCancelled = 8,
    kTimeout = 9,
    kBufferOverflow = 10,
    kAccessDenied = 11
};

OGC_GRAPH_API std::string GetResultString(DrawResult result);

OGC_GRAPH_API std::string GetResultDescription(DrawResult result);

}  
}  

#endif  
