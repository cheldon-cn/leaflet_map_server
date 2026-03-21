#include "gtest/gtest.h"
#include "ogc/feature/feature_defn.h"

using namespace ogc;

TEST(CNFeatureDefn, DefaultConstructor) {
    CNFeatureDefn defn;
    EXPECT_EQ(defn.GetFieldCount(), 0);
    EXPECT_EQ(defn.GetGeomFieldCount(), 0);
}

TEST(CNFeatureDefn, NamedConstructor) {
    CNFeatureDefn* defn = CNFeatureDefn::Create("test_feature");
    EXPECT_NE(defn, nullptr);
    defn->ReleaseReference();
}

TEST(CNFeatureDefn, Clone) {
    CNFeatureDefn* original = CNFeatureDefn::Create("original");
    CNFeatureDefn* cloned = original->Clone();
    EXPECT_NE(cloned, nullptr);
    delete cloned;
    original->ReleaseReference();
}
