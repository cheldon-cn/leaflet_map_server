#ifndef OGC_DRAW_RESULT_H
#define OGC_DRAW_RESULT_H

#include <string>
#include <cstdint>

namespace ogc {
namespace draw {

enum class DrawResult {
    kSuccess = 0,
    kInvalidParameter = 1,
    kOutOfMemory = 2,
    kDeviceError = 3,
    kEngineError = 4,
    kNotImplemented = 5,
    kCancelled = 6,
    kTimeout = 7,
    kDeviceNotReady = 8,
    kDeviceLost = 9,
    kUnsupportedOperation = 10,
    kInvalidState = 11,
    kFileError = 12,
    kFontError = 13,
    kImageError = 14
};

inline const char* DrawResultToString(DrawResult result) {
    switch (result) {
        case DrawResult::kSuccess:           return "Success";
        case DrawResult::kInvalidParameter:  return "Invalid Parameter";
        case DrawResult::kOutOfMemory:       return "Out Of Memory";
        case DrawResult::kDeviceError:       return "Device Error";
        case DrawResult::kEngineError:       return "Engine Error";
        case DrawResult::kNotImplemented:    return "Not Implemented";
        case DrawResult::kCancelled:         return "Cancelled";
        case DrawResult::kTimeout:           return "Timeout";
        case DrawResult::kDeviceNotReady:    return "Device Not Ready";
        case DrawResult::kDeviceLost:        return "Device Lost";
        case DrawResult::kUnsupportedOperation: return "Unsupported Operation";
        case DrawResult::kInvalidState:      return "Invalid State";
        case DrawResult::kFileError:         return "File Error";
        case DrawResult::kFontError:         return "Font Error";
        case DrawResult::kImageError:        return "Image Error";
        default:                             return "Unknown Error";
    }
}

inline bool IsSuccess(DrawResult result) {
    return result == DrawResult::kSuccess;
}

inline bool IsError(DrawResult result) {
    return result != DrawResult::kSuccess;
}

class DrawError {
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

    static DrawError FromResult(DrawResult result, const std::string& context = "") {
        return DrawError(result, DrawResultToString(result), context);
    }

    static DrawError FromResultWithLocation(DrawResult result, 
                                            const std::string& context,
                                            const std::string& file,
                                            int line) {
        return DrawError(result, DrawResultToString(result), context, file, line);
    }

    std::string ToString() const {
        std::string result = DrawResultToString(code);
        if (!message.empty()) {
            result += ": " + message;
        }
        if (!context.empty()) {
            result += " [Context: " + context + "]";
        }
        if (!file.empty() && line > 0) {
            result += " [" + file + ":" + std::to_string(line) + "]";
        }
        return result;
    }

    bool IsSuccess() const {
        return code == DrawResult::kSuccess;
    }

    bool IsError() const {
        return code != DrawResult::kSuccess;
    }
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
