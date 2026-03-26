#include <gtest/gtest.h>
#include "ogc/feature/feature.h"
#include "ogc/feature/feature_defn.h"
#include "ogc/feature/field_defn.h"
#include "ogc/feature/field_value.h"
#include "ogc/geometry.h"
#include "ogc/point.h"
#include "ogc/linestring.h"
#include "ogc/linearring.h"
#include "ogc/polygon.h"

using namespace ogc;

class FeatureTest : public ::testing::Test {
protected:
    void SetUp() override {
        defn_ = CNFeatureDefn::Create("test_feature");
        
        CNFieldDefn* idField = CreateCNFieldDefn("id");
        idField->SetType(CNFieldType::kInteger);
        defn_->AddFieldDefn(idField);
        
        CNFieldDefn* nameField = CreateCNFieldDefn("name");
        nameField->SetType(CNFieldType::kString);
        defn_->AddFieldDefn(nameField);
        
        CNFieldDefn* valueField = CreateCNFieldDefn("value");
        valueField->SetType(CNFieldType::kReal);
        defn_->AddFieldDefn(valueField);
    }
    
    void TearDown() override {
        if (defn_) {
            defn_->ReleaseReference();
        }
    }
    
    CNFeatureDefn* defn_ = nullptr;
};

TEST_F(FeatureTest, DefaultConstructor) {
    CNFeature feature;
    EXPECT_EQ(feature.GetFID(), 0);
    EXPECT_EQ(feature.GetFieldCount(), 0);
    EXPECT_EQ(feature.GetGeomFieldCount(), 0);
}

TEST_F(FeatureTest, ConstructorWithDefinition) {
    CNFeature feature(defn_);
    EXPECT_EQ(feature.GetFeatureDefn(), defn_);
    EXPECT_EQ(feature.GetFID(), 0);
}

TEST_F(FeatureTest, FID_SetAndGet) {
    CNFeature feature;
    EXPECT_EQ(feature.GetFID(), 0);
    
    feature.SetFID(12345);
    EXPECT_EQ(feature.GetFID(), 12345);
    
    feature.SetFID(-1);
    EXPECT_EQ(feature.GetFID(), -1);
    
    feature.SetFID(INT64_MAX);
    EXPECT_EQ(feature.GetFID(), INT64_MAX);
    
    feature.SetFID(INT64_MIN);
    EXPECT_EQ(feature.GetFID(), INT64_MIN);
}

TEST_F(FeatureTest, SetGeometry_Point) {
    CNFeature feature(defn_);
    
    PointPtr point = Point::Create(100.0, 200.0);
    point->SetSRID(4326);
    feature.SetGeometry(std::move(point));
    
    GeometryPtr retrieved = feature.GetGeometry();
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->GetGeometryType(), GeomType::kPoint);
}

TEST_F(FeatureTest, SetGeometry_LineString) {
    CNFeature feature(defn_);
    
    auto line = LineString::Create({{0, 0}, {1, 1}, {2, 2}});
    feature.SetGeometry(std::move(line));
    
    GeometryPtr retrieved = feature.GetGeometry();
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->GetGeometryType(), GeomType::kLineString);
}

TEST_F(FeatureTest, SetGeometry_Polygon) {
    CNFeature feature(defn_);
    
    auto ring = LinearRing::Create({{0, 0}, {0, 10}, {10, 10}, {10, 0}, {0, 0}}, true);
    auto polygon = Polygon::Create(std::move(ring));
    feature.SetGeometry(std::move(polygon));
    
    GeometryPtr retrieved = feature.GetGeometry();
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->GetGeometryType(), GeomType::kPolygon);
}

TEST_F(FeatureTest, SetGeometry_NullGeometry) {
    CNFeature feature(defn_);
    
    GeometryPtr nullGeom;
    feature.SetGeometry(std::move(nullGeom));
    
    EXPECT_EQ(feature.GetGeometry(), nullptr);
}

TEST_F(FeatureTest, StealGeometry) {
    CNFeature feature(defn_);
    auto point = Point::Create(100.0, 200.0);
    feature.SetGeometry(std::move(point));
    
    GeometryPtr stolen = feature.StealGeometry();
    ASSERT_NE(stolen, nullptr);
    EXPECT_EQ(stolen->GetGeometryType(), GeomType::kPoint);
    EXPECT_EQ(feature.GetGeometry(), nullptr);
}

TEST_F(FeatureTest, GetEnvelope) {
    CNFeature feature(defn_);
    auto point = Point::Create(100.0, 200.0);
    feature.SetGeometry(std::move(point));
    
    auto envelope = feature.GetEnvelope();
    ASSERT_NE(envelope, nullptr);
}

TEST_F(FeatureTest, GetEnvelope_NoGeometry) {
    CNFeature feature(defn_);
    auto envelope = feature.GetEnvelope();
    EXPECT_EQ(envelope, nullptr);
}

TEST_F(FeatureTest, Clone) {
    CNFeature original(defn_);
    original.SetFID(100);
    original.SetGeometry(Point::Create(10.0, 20.0));
    
    CNFeature* cloned = original.Clone();
    ASSERT_NE(cloned, nullptr);
    EXPECT_EQ(cloned->GetFID(), 100);
    
    delete cloned;
}

TEST_F(FeatureTest, Clone_EmptyFeature) {
    CNFeature original;
    CNFeature* cloned = original.Clone();
    ASSERT_NE(cloned, nullptr);
    EXPECT_EQ(cloned->GetFID(), 0);
    
    delete cloned;
}

TEST_F(FeatureTest, CopyConstructor) {
    CNFeature original(defn_);
    original.SetFID(500);
    
    CNFeature copy(original);
    EXPECT_EQ(copy.GetFID(), 500);
}

TEST_F(FeatureTest, MoveConstructor) {
    CNFeature original(defn_);
    original.SetFID(600);
    
    CNFeature moved(std::move(original));
    EXPECT_EQ(moved.GetFID(), 600);
}

TEST_F(FeatureTest, CopyAssignment) {
    CNFeature original(defn_);
    original.SetFID(700);
    
    CNFeature copy;
    copy = original;
    EXPECT_EQ(copy.GetFID(), 700);
}

TEST_F(FeatureTest, MoveAssignment) {
    CNFeature original(defn_);
    original.SetFID(800);
    
    CNFeature moved;
    moved = std::move(original);
    EXPECT_EQ(moved.GetFID(), 800);
}

TEST_F(FeatureTest, Equal) {
    CNFeature f1(defn_);
    f1.SetFID(100);
    
    CNFeature f2(defn_);
    f2.SetFID(100);
    
    CNFeature f3(defn_);
    f3.SetFID(200);
    
    EXPECT_TRUE(f1.Equal(f2));
    EXPECT_FALSE(f1.Equal(f3));
}

TEST_F(FeatureTest, OperatorEqual) {
    CNFeature f1(defn_);
    f1.SetFID(100);
    
    CNFeature f2(defn_);
    f2.SetFID(100);
    
    CNFeature f3(defn_);
    f3.SetFID(200);
    
    EXPECT_TRUE(f1 == f2);
    EXPECT_FALSE(f1 == f3);
}

TEST_F(FeatureTest, OperatorNotEqual) {
    CNFeature f1(defn_);
    f1.SetFID(100);
    
    CNFeature f2(defn_);
    f2.SetFID(200);
    
    EXPECT_TRUE(f1 != f2);
}

TEST_F(FeatureTest, Swap) {
    CNFeature f1(defn_);
    f1.SetFID(1);
    
    CNFeature f2(defn_);
    f2.SetFID(2);
    
    f1.Swap(f2);
    
    EXPECT_EQ(f1.GetFID(), 2);
    EXPECT_EQ(f2.GetFID(), 1);
}

TEST_F(FeatureTest, SetFeatureDefn) {
    CNFeature feature;
    feature.SetFeatureDefn(defn_);
    EXPECT_EQ(feature.GetFeatureDefn(), defn_);
}

TEST_F(FeatureTest, SetFieldInteger) {
    CNFeature feature(defn_);
    feature.SetFieldInteger(static_cast<size_t>(0), 42);
    EXPECT_EQ(feature.GetFieldAsInteger(static_cast<size_t>(0)), 42);
}

TEST_F(FeatureTest, SetFieldInteger64) {
    CNFeature feature(defn_);
    feature.SetFieldInteger64(static_cast<size_t>(0), 1234567890123LL);
    EXPECT_EQ(feature.GetFieldAsInteger64(static_cast<size_t>(0)), 1234567890123LL);
}

TEST_F(FeatureTest, SetFieldReal) {
    CNFeature feature(defn_);
    feature.SetFieldReal(static_cast<size_t>(2), 3.14159);
    EXPECT_DOUBLE_EQ(feature.GetFieldAsReal(static_cast<size_t>(2)), 3.14159);
}

TEST_F(FeatureTest, SetFieldString) {
    CNFeature feature(defn_);
    feature.SetFieldString(static_cast<size_t>(1), "test_value");
    EXPECT_EQ(feature.GetFieldAsString(static_cast<size_t>(1)), "test_value");
}

TEST_F(FeatureTest, SetFieldString_Empty) {
    CNFeature feature(defn_);
    feature.SetFieldString(static_cast<size_t>(1), "");
    EXPECT_EQ(feature.GetFieldAsString(static_cast<size_t>(1)), "");
}

TEST_F(FeatureTest, SetFieldNull) {
    CNFeature feature(defn_);
    feature.SetFieldInteger(static_cast<size_t>(0), 42);
    feature.SetFieldNull(static_cast<size_t>(0));
    EXPECT_TRUE(feature.IsFieldNull(static_cast<size_t>(0)));
}

TEST_F(FeatureTest, IsFieldSet) {
    CNFeature feature(defn_);
    EXPECT_FALSE(feature.IsFieldSet(static_cast<size_t>(0)));
    
    feature.SetFieldInteger(static_cast<size_t>(0), 42);
    EXPECT_TRUE(feature.IsFieldSet(static_cast<size_t>(0)));
}

TEST_F(FeatureTest, UnsetField) {
    CNFeature feature(defn_);
    feature.SetFieldInteger(static_cast<size_t>(0), 42);
    feature.UnsetField(static_cast<size_t>(0));
    EXPECT_FALSE(feature.IsFieldSet(static_cast<size_t>(0)));
}

TEST_F(FeatureTest, SetFieldByName) {
    CNFeature feature(defn_);
    feature.SetFieldInteger("id", 100);
    feature.SetFieldReal("value", 3.14);
    feature.SetFieldString("name", "hello");
}

TEST_F(FeatureTest, GetFieldByName) {
    CNFeature feature(defn_);
    feature.SetFieldInteger("id", 42);
    feature.SetFieldString("name", "test");
    
    EXPECT_EQ(feature.GetFieldAsInteger("id"), 42);
    EXPECT_EQ(feature.GetFieldAsString("name"), "test");
}

TEST_F(FeatureTest, MultipleGeometryFields) {
    CNFeature feature(defn_);
    
    auto point1 = Point::Create(0, 0);
    auto point2 = Point::Create(1, 1);
    
    feature.SetGeometry(std::move(point1), 0);
    feature.SetGeometry(std::move(point2), 1);
    
    EXPECT_EQ(feature.GetGeomFieldCount(), 2);
}

TEST_F(FeatureTest, LargeFID) {
    CNFeature feature;
    feature.SetFID(999999999999999999LL);
    EXPECT_EQ(feature.GetFID(), 999999999999999999LL);
}

TEST_F(FeatureTest, NegativeFID) {
    CNFeature feature;
    feature.SetFID(-999999);
    EXPECT_EQ(feature.GetFID(), -999999);
}

TEST_F(FeatureTest, SelfAssignment) {
    CNFeature feature(defn_);
    feature.SetFID(100);
    
    feature = feature;
    EXPECT_EQ(feature.GetFID(), 100);
}

TEST_F(FeatureTest, SelfMove) {
    CNFeature feature(defn_);
    feature.SetFID(200);
    
    feature = std::move(feature);
    EXPECT_EQ(feature.GetFID(), 200);
}
