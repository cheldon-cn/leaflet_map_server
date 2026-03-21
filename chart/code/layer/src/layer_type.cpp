#include "ogc/layer/layer_type.h"

namespace ogc {

const char* GetStatusString(CNStatus status) {
    switch (status) {
        case CNStatus::kSuccess:          return "Success";
        case CNStatus::kError:            return "Error";
        case CNStatus::kInvalidParameter: return "InvalidParameter";
        case CNStatus::kNullPointer:      return "NullPointer";
        case CNStatus::kOutOfRange:       return "OutOfRange";
        case CNStatus::kNotSupported:     return "NotSupported";
        case CNStatus::kNotImplemented:   return "NotImplemented";
        case CNStatus::kInvalidFeature:   return "InvalidFeature";
        case CNStatus::kInvalidGeometry:  return "InvalidGeometry";
        case CNStatus::kInvalidFID:       return "InvalidFID";
        case CNStatus::kFeatureNotFound:  return "FeatureNotFound";
        case CNStatus::kDuplicateFID:     return "DuplicateFID";
        case CNStatus::kFieldNotFound:     return "FieldNotFound";
        case CNStatus::kTypeMismatch:     return "TypeMismatch";
        case CNStatus::kIOError:          return "IOError";
        case CNStatus::kFileNotFound:     return "FileNotFound";
        case CNStatus::kFileExists:        return "FileExists";
        case CNStatus::kPermissionDenied: return "PermissionDenied";
        case CNStatus::kDiskFull:         return "DiskFull";
        case CNStatus::kCorruptData:      return "CorruptData";
        case CNStatus::kNoTransaction:     return "NoTransaction";
        case CNStatus::kTransactionActive: return "TransactionActive";
        case CNStatus::kCommitFailed:      return "CommitFailed";
        case CNStatus::kRollbackFailed:   return "RollbackFailed";
        case CNStatus::kLockFailed:       return "LockFailed";
        case CNStatus::kTimeout:          return "Timeout";
        case CNStatus::kDeadlock:         return "Deadlock";
        case CNStatus::kOutOfMemory:      return "OutOfMemory";
        case CNStatus::kBufferOverflow:   return "BufferOverflow";
        default:                          return "Unknown";
    }
}

std::string GetStatusDescription(CNStatus status) {
    switch (status) {
        case CNStatus::kSuccess:
            return "Operation completed successfully";
        case CNStatus::kError:
            return "General error";
        case CNStatus::kInvalidParameter:
            return "Invalid parameter";
        case CNStatus::kNullPointer:
            return "Null pointer";
        case CNStatus::kOutOfRange:
            return "Value out of range";
        case CNStatus::kNotSupported:
            return "Operation not supported";
        case CNStatus::kNotImplemented:
            return "Operation not implemented";
        case CNStatus::kInvalidFeature:
            return "Invalid feature";
        case CNStatus::kInvalidGeometry:
            return "Invalid geometry";
        case CNStatus::kInvalidFID:
            return "Invalid feature ID";
        case CNStatus::kFeatureNotFound:
            return "Feature not found";
        case CNStatus::kDuplicateFID:
            return "Duplicate feature ID";
        case CNStatus::kFieldNotFound:
            return "Field not found";
        case CNStatus::kTypeMismatch:
            return "Type mismatch";
        case CNStatus::kIOError:
            return "I/O error";
        case CNStatus::kFileNotFound:
            return "File not found";
        case CNStatus::kFileExists:
            return "File already exists";
        case CNStatus::kPermissionDenied:
            return "Permission denied";
        case CNStatus::kDiskFull:
            return "Disk full";
        case CNStatus::kCorruptData:
            return "Corrupt data";
        case CNStatus::kNoTransaction:
            return "No active transaction";
        case CNStatus::kTransactionActive:
            return "Transaction already active";
        case CNStatus::kCommitFailed:
            return "Transaction commit failed";
        case CNStatus::kRollbackFailed:
            return "Transaction rollback failed";
        case CNStatus::kLockFailed:
            return "Lock failed";
        case CNStatus::kTimeout:
            return "Operation timeout";
        case CNStatus::kDeadlock:
            return "Deadlock detected";
        case CNStatus::kOutOfMemory:
            return "Out of memory";
        case CNStatus::kBufferOverflow:
            return "Buffer overflow";
        default:
            return "Unknown error";
    }
}

bool IsSuccess(CNStatus status) {
    return status == CNStatus::kSuccess;
}

} // namespace ogc
