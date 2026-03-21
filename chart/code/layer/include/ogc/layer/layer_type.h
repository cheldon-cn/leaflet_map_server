#pragma once

#include "ogc/layer/export.h"

#include <string>

namespace ogc {

enum class CNLayerType {
    kUnknown = 0,
    kMemory,
    kVector,
    kRaster,
    kGroup,
    kWFS,
    kWMS,
    kWMTS
};

enum class CNLayerCapability {
    kSequentialRead,
    kRandomRead,
    kFastSpatialFilter,
    kFastFeatureCount,
    kFastGetExtent,
    kSequentialWrite,
    kRandomWrite,
    kCreateFeature,
    kDeleteFeature,
    kCreateField,
    kDeleteField,
    kReorderFields,
    kAlterFieldDefn,
    kTransactions,
    kStringsAsUTF8,
    kIgnoreFields,
    kCurveGeometries
};

enum class CNStatus {
    kSuccess = 0,
    kError = 1,
    kInvalidParameter = 2,
    kNullPointer = 3,
    kOutOfRange = 4,
    kNotSupported = 5,
    kNotImplemented = 6,
    kInvalidFeature = 100,
    kInvalidGeometry = 101,
    kInvalidFID = 102,
    kFeatureNotFound = 103,
    kDuplicateFID = 104,
    kFieldNotFound = 105,
    kTypeMismatch = 106,
    kIOError = 200,
    kFileNotFound = 201,
    kFileExists = 202,
    kPermissionDenied = 203,
    kDiskFull = 204,
    kCorruptData = 205,
    kNoTransaction = 300,
    kTransactionNotActive = 300,
    kTransactionActive = 301,
    kCommitFailed = 302,
    kRollbackFailed = 303,
    kLockFailed = 400,
    kTimeout = 401,
    kDeadlock = 402,
    kOutOfMemory = 500,
    kBufferOverflow = 501
};

enum class CNSpatialFilterType {
    kNone,
    kRectangular,
    kGeometric
};

enum class CNSpatialRelation {
    kEquals,
    kDisjoint,
    kIntersects,
    kTouches,
    kCrosses,
    kWithin,
    kContains,
    kOverlaps,
    kCovers,
    kCoveredBy
};

enum class SpatialIndexType {
    kNone,
    kRTree,
    kQuadTree,
    kGrid,
    kSTRtree
};

OGC_LAYER_API const char* GetStatusString(CNStatus status);

OGC_LAYER_API std::string GetStatusDescription(CNStatus status);

OGC_LAYER_API bool IsSuccess(CNStatus status);

}
