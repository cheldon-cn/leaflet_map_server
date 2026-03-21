#include "gtest/gtest.h"
#include "ogc/layer/layer_type.h"

using namespace ogc;

TEST(CNLayerType, Values) {
    EXPECT_EQ(static_cast<int>(CNLayerType::kUnknown), 0);
    EXPECT_EQ(static_cast<int>(CNLayerType::kMemory), 1);
    EXPECT_EQ(static_cast<int>(CNLayerType::kVector), 2);
    EXPECT_EQ(static_cast<int>(CNLayerType::kRaster), 3);
    EXPECT_EQ(static_cast<int>(CNLayerType::kGroup), 4);
    EXPECT_EQ(static_cast<int>(CNLayerType::kWFS), 5);
    EXPECT_EQ(static_cast<int>(CNLayerType::kWMS), 6);
    EXPECT_EQ(static_cast<int>(CNLayerType::kWMTS), 7);
}

TEST(CNLayerCapability, Values) {
    EXPECT_EQ(static_cast<int>(CNLayerCapability::kSequentialRead), 0);
    EXPECT_EQ(static_cast<int>(CNLayerCapability::kRandomRead), 1);
    EXPECT_EQ(static_cast<int>(CNLayerCapability::kFastSpatialFilter), 2);
    EXPECT_EQ(static_cast<int>(CNLayerCapability::kFastFeatureCount), 3);
    EXPECT_EQ(static_cast<int>(CNLayerCapability::kFastGetExtent), 4);
    EXPECT_EQ(static_cast<int>(CNLayerCapability::kSequentialWrite), 5);
    EXPECT_EQ(static_cast<int>(CNLayerCapability::kRandomWrite), 6);
    EXPECT_EQ(static_cast<int>(CNLayerCapability::kCreateFeature), 7);
    EXPECT_EQ(static_cast<int>(CNLayerCapability::kDeleteFeature), 8);
    EXPECT_EQ(static_cast<int>(CNLayerCapability::kCreateField), 9);
    EXPECT_EQ(static_cast<int>(CNLayerCapability::kDeleteField), 10);
    EXPECT_EQ(static_cast<int>(CNLayerCapability::kReorderFields), 11);
    EXPECT_EQ(static_cast<int>(CNLayerCapability::kAlterFieldDefn), 12);
    EXPECT_EQ(static_cast<int>(CNLayerCapability::kTransactions), 13);
    EXPECT_EQ(static_cast<int>(CNLayerCapability::kStringsAsUTF8), 14);
    EXPECT_EQ(static_cast<int>(CNLayerCapability::kIgnoreFields), 15);
    EXPECT_EQ(static_cast<int>(CNLayerCapability::kCurveGeometries), 16);
}

TEST(CNStatus, Values) {
    EXPECT_EQ(static_cast<int>(CNStatus::kSuccess), 0);
    EXPECT_EQ(static_cast<int>(CNStatus::kError), 1);
    EXPECT_EQ(static_cast<int>(CNStatus::kInvalidParameter), 2);
    EXPECT_EQ(static_cast<int>(CNStatus::kNullPointer), 3);
    EXPECT_EQ(static_cast<int>(CNStatus::kOutOfRange), 4);
    EXPECT_EQ(static_cast<int>(CNStatus::kNotSupported), 5);
    EXPECT_EQ(static_cast<int>(CNStatus::kNotImplemented), 6);
    EXPECT_EQ(static_cast<int>(CNStatus::kInvalidFeature), 100);
    EXPECT_EQ(static_cast<int>(CNStatus::kInvalidGeometry), 101);
    EXPECT_EQ(static_cast<int>(CNStatus::kInvalidFID), 102);
    EXPECT_EQ(static_cast<int>(CNStatus::kFeatureNotFound), 103);
    EXPECT_EQ(static_cast<int>(CNStatus::kDuplicateFID), 104);
    EXPECT_EQ(static_cast<int>(CNStatus::kFieldNotFound), 105);
    EXPECT_EQ(static_cast<int>(CNStatus::kTypeMismatch), 106);
    EXPECT_EQ(static_cast<int>(CNStatus::kIOError), 200);
    EXPECT_EQ(static_cast<int>(CNStatus::kFileNotFound), 201);
    EXPECT_EQ(static_cast<int>(CNStatus::kFileExists), 202);
    EXPECT_EQ(static_cast<int>(CNStatus::kPermissionDenied), 203);
    EXPECT_EQ(static_cast<int>(CNStatus::kDiskFull), 204);
    EXPECT_EQ(static_cast<int>(CNStatus::kCorruptData), 205);
    EXPECT_EQ(static_cast<int>(CNStatus::kNoTransaction), 300);
    EXPECT_EQ(static_cast<int>(CNStatus::kTransactionActive), 301);
    EXPECT_EQ(static_cast<int>(CNStatus::kCommitFailed), 302);
    EXPECT_EQ(static_cast<int>(CNStatus::kRollbackFailed), 303);
    EXPECT_EQ(static_cast<int>(CNStatus::kLockFailed), 400);
    EXPECT_EQ(static_cast<int>(CNStatus::kTimeout), 401);
    EXPECT_EQ(static_cast<int>(CNStatus::kDeadlock), 402);
    EXPECT_EQ(static_cast<int>(CNStatus::kOutOfMemory), 500);
    EXPECT_EQ(static_cast<int>(CNStatus::kBufferOverflow), 501);
}

TEST(CNSpatialFilterType, Values) {
    EXPECT_EQ(static_cast<int>(CNSpatialFilterType::kNone), 0);
    EXPECT_EQ(static_cast<int>(CNSpatialFilterType::kRectangular), 1);
    EXPECT_EQ(static_cast<int>(CNSpatialFilterType::kGeometric), 2);
}

TEST(CNSpatialRelation, Values) {
    EXPECT_EQ(static_cast<int>(CNSpatialRelation::kEquals), 0);
    EXPECT_EQ(static_cast<int>(CNSpatialRelation::kDisjoint), 1);
    EXPECT_EQ(static_cast<int>(CNSpatialRelation::kIntersects), 2);
    EXPECT_EQ(static_cast<int>(CNSpatialRelation::kTouches), 3);
    EXPECT_EQ(static_cast<int>(CNSpatialRelation::kCrosses), 4);
    EXPECT_EQ(static_cast<int>(CNSpatialRelation::kWithin), 5);
    EXPECT_EQ(static_cast<int>(CNSpatialRelation::kContains), 6);
    EXPECT_EQ(static_cast<int>(CNSpatialRelation::kOverlaps), 7);
    EXPECT_EQ(static_cast<int>(CNSpatialRelation::kCovers), 8);
    EXPECT_EQ(static_cast<int>(CNSpatialRelation::kCoveredBy), 9);
}

// TEST(CNStatus, GetStatusString) {
//     EXPECT_STREQ(GetStatusString(CNStatus::kSuccess), "Success");
//     EXPECT_STREQ(GetStatusString(CNStatus::kError), "Error");
//     EXPECT_STREQ(GetStatusString(CNStatus::kInvalidParameter), "InvalidParameter");
//     EXPECT_STREQ(GetStatusString(CNStatus::kNotSupported), "NotSupported");
//     EXPECT_STREQ(GetStatusString(CNStatus::kFeatureNotFound), "FeatureNotFound");
//     EXPECT_STREQ(GetStatusString(CNStatus::kIOError), "IOError");
//     EXPECT_STREQ(GetStatusString(CNStatus::kOutOfMemory), "OutOfMemory");
// }

// TEST(CNStatus, GetStatusDescription) {
//     EXPECT_EQ(GetStatusDescription(CNStatus::kSuccess), "Operation completed successfully");
//     EXPECT_EQ(GetStatusDescription(CNStatus::kError), "General error");
//     EXPECT_EQ(GetStatusDescription(CNStatus::kInvalidParameter), "Invalid parameter");
//     EXPECT_EQ(GetStatusDescription(CNStatus::kNotSupported), "Operation not supported");
// }

// TEST(CNStatus, IsSuccess) {
//     EXPECT_TRUE(IsSuccess(CNStatus::kSuccess));
//     EXPECT_FALSE(IsSuccess(CNStatus::kError));
//     EXPECT_FALSE(IsSuccess(CNStatus::kInvalidParameter));
//     EXPECT_FALSE(IsSuccess(CNStatus::kFeatureNotFound));
//     EXPECT_FALSE(IsSuccess(CNStatus::kIOError));
// }
