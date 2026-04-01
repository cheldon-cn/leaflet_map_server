#include "ogc/draw/draw_result.h"

namespace ogc {
namespace draw {

const char* DrawResultToString(DrawResult result) {
    switch (result) {
        case DrawResult::kSuccess:           return "Success";
        case DrawResult::kFailed:            return "Failed";
        case DrawResult::kInvalidParameter:  return "Invalid Parameter";
        case DrawResult::kOutOfMemory:       return "Out Of Memory";
        case DrawResult::kDeviceError:       return "Device Error";
        case DrawResult::kEngineError:       return "Engine Error";
        case DrawResult::kNotImplemented:    return "Not Implemented";
        case DrawResult::kCancelled:         return "Cancelled";
        case DrawResult::kTimeout:           return "Timeout";
        case DrawResult::kDeviceNotReady:    return "Device Not Ready";
        case DrawResult::kDeviceLost:        return "Device Lost";
        case DrawResult::kEngineNotReady:    return "Engine Not Ready";
        case DrawResult::kUnsupportedOperation: return "Unsupported Operation";
        case DrawResult::kInvalidState:      return "Invalid State";
        case DrawResult::kFileError:         return "File Error";
        case DrawResult::kFileNotFound:      return "File Not Found";
        case DrawResult::kFontError:         return "Font Error";
        case DrawResult::kImageError:        return "Image Error";
        case DrawResult::kUnsupportedFormat: return "Unsupported Format";
        case DrawResult::kBufferOverflow:    return "Buffer Overflow";
        case DrawResult::kAccessDenied:      return "Access Denied";
        default:                             return "Unknown Error";
    }
}

std::string GetResultString(DrawResult result) {
    return std::string(DrawResultToString(result));
}

std::string GetResultDescription(DrawResult result) {
    switch (result) {
        case DrawResult::kSuccess:           return "Operation completed successfully";
        case DrawResult::kFailed:            return "Operation failed";
        case DrawResult::kInvalidParameter:  return "Invalid parameter provided";
        case DrawResult::kOutOfMemory:       return "Out of memory";
        case DrawResult::kDeviceError:       return "Device error occurred";
        case DrawResult::kEngineError:       return "Engine error occurred";
        case DrawResult::kNotImplemented:    return "Feature not implemented";
        case DrawResult::kCancelled:         return "Operation was cancelled";
        case DrawResult::kTimeout:           return "Operation timed out";
        case DrawResult::kDeviceNotReady:    return "Device is not ready";
        case DrawResult::kDeviceLost:        return "Device was lost";
        case DrawResult::kEngineNotReady:    return "Engine is not ready";
        case DrawResult::kUnsupportedOperation: return "Unsupported operation";
        case DrawResult::kInvalidState:      return "Invalid state for operation";
        case DrawResult::kFileError:         return "File error occurred";
        case DrawResult::kFileNotFound:      return "File not found";
        case DrawResult::kFontError:         return "Font error occurred";
        case DrawResult::kImageError:        return "Image error occurred";
        case DrawResult::kUnsupportedFormat: return "Unsupported format";
        case DrawResult::kBufferOverflow:    return "Buffer overflow";
        case DrawResult::kAccessDenied:      return "Access denied";
        default:                             return "Unknown error";
    }
}

bool IsSuccess(DrawResult result) {
    return result == DrawResult::kSuccess;
}

bool IsError(DrawResult result) {
    return result != DrawResult::kSuccess;
}

DrawError DrawError::FromResult(DrawResult result, const std::string& context) {
    return DrawError(result, DrawResultToString(result), context);
}

DrawError DrawError::FromResultWithLocation(DrawResult result, 
                                            const std::string& context,
                                            const std::string& file,
                                            int line) {
    return DrawError(result, DrawResultToString(result), context, file, line);
}

std::string DrawError::ToString() const {
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

bool DrawError::IsSuccess() const {
    return code == DrawResult::kSuccess;
}

bool DrawError::IsError() const {
    return code != DrawResult::kSuccess;
}

}  
}  
