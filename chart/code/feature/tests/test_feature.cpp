#include "gtest/gtest.h"
#include "ogc/feature/feature.h"
#include "ogc/feature/feature_defn.h"
#include "ogc/feature/field_defn.h"
#include "ogc/geometry.h"
#include "ogc/point.h"

using namespace ogc;

TEST(CNFeature, DefaultConstructor) {
    CNFeature feature;
    EXPECT_EQ(feature.GetFID(), 0);
    EXPECT_EQ(feature.GetFieldCount(), 0);
    EXPECT_EQ(feature.GetGeomFieldCount(), 0);
}

TEST(CNFeature, FID) {
    CNFeature feature;
    EXPECT_EQ(feature.GetFID(), 0);
    
    feature.SetFID(12345);
    EXPECT_EQ(feature.GetFID(), 12345);
}

TEST(CNFeature, SetGeometry) {
    CNFeatureDefn* defn = CNFeatureDefn::Create("test");
    CNFeature feature(defn);
    
    PointPtr point = Point::Create(100.0, 200.0);
    feature.SetGeometry(std::move(point));
    
    GeometryPtr retrieved = feature.GetGeometry();
    EXPECT_NE(retrieved, nullptr);
    
    defn->ReleaseReference();
}

TEST(CNFeature, Clone) {
    CNFeatureDefn* defn = CNFeatureDefn::Create("test");
    CNFeature original(defn);
    original.SetFID(100);
    
    CNFeature* cloned = original.Clone();
    EXPECT_EQ(cloned->GetFID(), 100);
    
    delete cloned;
    defn->ReleaseReference();
}
