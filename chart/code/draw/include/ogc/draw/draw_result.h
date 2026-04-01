#ifndef OGC_DRAW_RESULT_H
#define OGC_DRAW_RESULT_H

#include "ogc/draw/export.h"
#include <string>
#include <cstdint>

namespace ogc {
namespace draw {

enum class DrawResult {
    kSuccess = 0,
    kFailed = 1,
    kInvalidParameter = 2,
    kOutOfMemory = 3,
    kDeviceError = 4,
    kEngineError = 5,
    kNotImplemented = 6,
    kCancelled = 7,
    kTimeout = 8,
    kDeviceNotReady = 9,
    kDeviceLost = 10,
    kEngineNotReady = 11,
    kUnsupportedOperation = 12,
    kInvalidState = 13,
    kFileError = 14,
    kFileNotFound = 15,
    kFontError = 16,
    kImageError = 17,
    kUnsupportedFormat = 18,
    kBufferOverflow = 19,
    kAccessDenied = 20
};

OGC_DRAW_API const char* DrawResultToString(DrawResult result);

OGC_DRAW_API std::string GetResultString(DrawResult result);

OGC_DRAW_API std::string GetResultDescription(DrawResult result);

OGC_DRAW_API bool IsSuccess(DrawResult result);

OGC_DRAW_API bool IsError(DrawResult result);

class OGC_DRAW_API DrawError {
public:
    DrawResult code;
    std::string message;
    std::string context;
    std::string file;
    int line;

    DrawError() 
        : code(DrawResult::kSuccess)
        , line(0) {}

    DrawError(DrawResult code_, const std::string& message_ = "",
              const std::string& context_ = "",
              const std::string& file_ = "", int line_ = 0)
        : code(code_)
        , message(message_)
        , context(context_)
        , file(file_)
        , line(line_) {}

    static DrawError FromResult(DrawResult result, const std::string& context = "");
    static DrawError FromResultWithLocation(DrawResult result, 
                                            const std::string& context,
                                            const std::string& file,
                                            int line);

    std::string ToString() const;

    bool IsSuccess() const;
    bool IsError() const;
};

#define DRAW_RETURN_IF_ERROR(expr) \
    do { \
        ::ogc::draw::DrawResult _r = (expr); \
        if (_r != ::ogc::draw::DrawResult::kSuccess) { \
            return _r; \
        } \
    } while(0)

#define DRAW_RETURN_ERROR_IF(cond, error_code) \
    do { \
        if (cond) { \
            return error_code; \
        } \
    } while(0)

#define DRAW_MAKE_ERROR(result, context) \
    ::ogc::draw::DrawError::FromResultWithLocation(result, context, __FILE__, __LINE__)

}  
}  

#endif  
