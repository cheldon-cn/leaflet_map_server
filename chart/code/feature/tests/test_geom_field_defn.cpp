#include "gtest/gtest.h"
#include "ogc/feature/geom_field_defn.h"
#include "ogc/geometry.h"
#include "ogc/point.h"
#include "ogc/linestring.h"
#include "ogc/polygon.h"

using namespace ogc;

class GeomFieldDefnTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(GeomFieldDefnTest, DefaultConstructor) {
    CNGeomFieldDefn defn;
    EXPECT_EQ(defn.GetName(), "");
    EXPECT_EQ(defn.GetGeometryType(), GeomType::kUnknown);
    EXPECT_EQ(defn.GetSRID(), 0);
}

TEST_F(GeomFieldDefnTest, NamedConstructor) {
    CNGeomFieldDefn* defn = CNGeomFieldDefn::Create("geometry");
    EXPECT_NE(defn, nullptr);
    EXPECT_EQ(defn->GetName(), "geometry");
    defn->ReleaseReference();
}

TEST_F(GeomFieldDefnTest, NamedAndTypedConstructor) {
    CNGeomFieldDefn* defn = CNGeomFieldDefn::Create("geom", GeomType::kPoint);
    EXPECT_NE(defn, nullptr);
    EXPECT_EQ(defn->GetName(), "geom");
    EXPECT_EQ(defn.GetGeometryType(), GeomType::kPoint);
    defn->ReleaseReference();
}

TEST_F(GeomFieldDefnTest, SetName) {
    CNGeomFieldDefn defn;
    defn.SetName("shape");
    EXPECT_EQ(defn.GetName(), "shape");
}

TEST_F(GeomFieldDefnTest, SetGeometryType) {
    CNGeomFieldDefn defn;
    
    defn.SetGeometryType(GeomType::kPoint);
    EXPECT_EQ(defn.GetGeometryType(), GeomType::kPoint);
    
    defn.SetGeometryType(GeomType::kLineString);
    EXPECT_EQ(defn.GetGeometryType(), GeomType::kLineString);
    
    defn.SetGeometryType(GeomType::kPolygon);
    EXPECT_EQ(defn.GetGeometryType(), GeomType::kPolygon);
}

TEST_F(GeomFieldDefnTest, SetSRID) {
    CNGeomFieldDefn defn;
    
    defn.SetSRID(4326);
    EXPECT_EQ(defn.GetSRID(), 4326);
    
    defn.SetSRID(3857);
    EXPECT_EQ(defn.GetSRID(), 3857);
}

TEST_F(GeomFieldDefnTest, SetNullable) {
    CNGeomFieldDefn defn;
    
    defn.SetNullable(true);
    EXPECT_TRUE(defn.IsNullable());
    
    defn.SetNullable(false);
    EXPECT_FALSE(defn.IsNullable());
}

TEST_F(GeomFieldDefnTest, Clone) {
    CNGeomFieldDefn* original = CNGeomFieldDefn::Create("original");
    original->SetGeometryType(GeomType::kPolygon);
    original->SetSRID(4326);
    
    CNGeomFieldDefn* cloned = original->Clone();
    EXPECT_NE(cloned, nullptr);
    EXPECT_EQ(cloned->GetName(), "original");
    EXPECT_EQ(cloned->GetGeometryType(), GeomType::kPolygon);
    EXPECT_EQ(cloned->GetSRID(), 4326);
    
    delete cloned;
    original->ReleaseReference();
}

TEST_F(GeomFieldDefnTest, CopyConstructor) {
    CNGeomFieldDefn original;
    original.SetName("test_geom");
    original.SetGeometryType(GeomType::kLineString);
    original.SetSRID(3857);
    
    CNGeomFieldDefn copy(original);
    EXPECT_EQ(copy.GetName(), "test_geom");
    EXPECT_EQ(copy.GetGeometryType(), GeomType::kLineString);
    EXPECT_EQ(copy.GetSRID(), 3857);
}

TEST_F(GeomFieldDefnTest, AssignmentOperator) {
    CNGeomFieldDefn original;
    original.SetName("assigned_geom");
    original.SetGeometryType(GeomType::kMultiPolygon);
    
    CNGeomFieldDefn assigned;
    assigned = original;
    
    EXPECT_EQ(assigned.GetName(), "assigned_geom");
    EXPECT_EQ(assigned.GetGeometryType(), GeomType::kMultiPolygon);
}

TEST_F(GeomFieldDefnTest, AllGeometryTypes) {
    CNGeomFieldDefn defn;
    
    std::vector<GeomType> types = {
        GeomType::kUnknown,
        GeomType::kPoint,
        GeomType::kLineString,
        GeomType::kPolygon,
        GeomType::kMultiPoint,
        GeomType::kMultiLineString,
        GeomType::kMultiPolygon,
        GeomType::kGeometryCollection
    };
    
    for (auto type : types) {
        defn.SetGeometryType(type);
        EXPECT_EQ(defn.GetGeometryType(), type);
    }
}

TEST_F(GeomFieldDefnTest, ZeroSRID) {
    CNGeomFieldDefn defn;
    defn.SetSRID(0);
    EXPECT_EQ(defn.GetSRID(), 0);
}

TEST_F(GeomFieldDefnTest, NegativeSRID) {
    CNGeomFieldDefn defn;
    defn.SetSRID(-1);
    EXPECT_EQ(defn.GetSRID(), -1);
}

TEST_F(GeomFieldDefnTest, LargeSRID) {
    CNGeomFieldDefn defn;
    defn.SetSRID(999999);
    EXPECT_EQ(defn.GetSRID(), 999999);
}

TEST_F(GeomFieldDefnTest, EmptyName) {
    CNGeomFieldDefn defn;
    defn.SetName("");
    EXPECT_EQ(defn.GetName(), "");
}

TEST_F(GeomFieldDefnTest, LongName) {
    CNGeomFieldDefn defn;
    std::string longName(500, 'g');
    defn.SetName(longName);
    EXPECT_EQ(defn.GetName(), longName);
}

TEST_F(GeomFieldDefnTest, IsSame) {
    CNGeomFieldDefn* defn1 = CNGeomFieldDefn::Create("geom1");
    defn1->SetGeometryType(GeomType::kPoint);
    defn1->SetSRID(4326);
    
    CNGeomFieldDefn* defn2 = CNGeomFieldDefn::Create("geom1");
    defn2->SetGeometryType(GeomType::kPoint);
    defn2->SetSRID(4326);
    
    CNGeomFieldDefn* defn3 = CNGeomFieldDefn::Create("geom2");
    defn3->SetGeometryType(GeomType::kPolygon);
    
    EXPECT_TRUE(defn1->IsSame(defn2));
    EXPECT_FALSE(defn1->IsSame(defn3));
    
    defn1->ReleaseReference();
    defn2->ReleaseReference();
    defn3->ReleaseReference();
}

TEST_F(GeomFieldDefnTest, DefaultNullable) {
    CNGeomFieldDefn defn;
    EXPECT_TRUE(defn.IsNullable());
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
