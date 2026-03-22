#include "ogc/draw/draw_result.h"

namespace ogc {
namespace draw {

std::string GetResultString(DrawResult result) {
    switch (result) {
        case DrawResult::kSuccess: return "Success";
        case DrawResult::kFailed: return "Failed";
        case DrawResult::kInvalidParams: return "InvalidParams";
        case DrawResult::kDeviceNotReady: return "DeviceNotReady";
        case DrawResult::kEngineNotReady: return "EngineNotReady";
        case DrawResult::kOutOfMemory: return "OutOfMemory";
        case DrawResult::kFileNotFound: return "FileNotFound";
        case DrawResult::kUnsupportedFormat: return "UnsupportedFormat";
        case DrawResult::kOperationCancelled: return "OperationCancelled";
        case DrawResult::kTimeout: return "Timeout";
        case DrawResult::kBufferOverflow: return "BufferOverflow";
        case DrawResult::kAccessDenied: return "AccessDenied";
        default: return "Unknown";
    }
}

std::string GetResultDescription(DrawResult result) {
    switch (result) {
        case DrawResult::kSuccess: return "Operation completed successfully";
        case DrawResult::kFailed: return "Operation failed";
        case DrawResult::kInvalidParams: return "Invalid parameters provided";
        case DrawResult::kDeviceNotReady: return "Device is not ready for operation";
        case DrawResult::kEngineNotReady: return "Engine is not ready for operation";
        case DrawResult::kOutOfMemory: return "Out of memory";
        case DrawResult::kFileNotFound: return "File not found";
        case DrawResult::kUnsupportedFormat: return "Unsupported format";
        case DrawResult::kOperationCancelled: return "Operation was cancelled";
        case DrawResult::kTimeout: return "Operation timed out";
        case DrawResult::kBufferOverflow: return "Buffer overflow";
        case DrawResult::kAccessDenied: return "Access denied";
        default: return "Unknown result";
    }
}

}  
}  
