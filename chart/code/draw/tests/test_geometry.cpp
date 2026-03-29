#include <gtest/gtest.h>
#include "ogc/draw/geometry.h"
#include "ogc/point.h"
#include "ogc/linestring.h"
#include "ogc/polygon.h"
#include <cmath>

TEST(RectGeometryTest, DefaultConstructor) {
    auto geom = ogc::draw::RectGeometry::Create();
    EXPECT_TRUE(geom->GetRect().IsEmpty());
}

TEST(RectGeometryTest, ParameterizedConstructor) {
    auto geom = ogc::draw::RectGeometry::Create(10.0, 20.0, 100.0, 200.0);
    ogc::draw::Rect r = geom->GetRect();
    
    EXPECT_DOUBLE_EQ(r.x, 10.0);
    EXPECT_DOUBLE_EQ(r.y, 20.0);
    EXPECT_DOUBLE_EQ(r.w, 100.0);
    EXPECT_DOUBLE_EQ(r.h, 200.0);
}

TEST(RectGeometryTest, GetType) {
    auto geom = ogc::draw::RectGeometry::Create();
    EXPECT_EQ(geom->GetGeometryType(), ogc::GeomType::kPolygon);
    EXPECT_STREQ(geom->GetGeometryTypeString(), "RECT");
}

TEST(RectGeometryTest, IsEmpty) {
    auto geom = ogc::draw::RectGeometry::Create();
    EXPECT_TRUE(geom->IsEmpty());
    
    auto geom2 = ogc::draw::RectGeometry::Create(10.0, 20.0, 100.0, 200.0);
    EXPECT_FALSE(geom2->IsEmpty());
}

TEST(RectGeometryTest, Clone) {
    auto geom = ogc::draw::RectGeometry::Create(10.0, 20.0, 100.0, 200.0);
    ogc::GeometryPtr cloned = geom->Clone();
    
    ASSERT_NE(cloned, nullptr);
    EXPECT_EQ(cloned->GetGeometryType(), ogc::GeomType::kPolygon);
    
    ogc::draw::RectGeometry* clonedRect = dynamic_cast<ogc::draw::RectGeometry*>(cloned.get());
    ASSERT_NE(clonedRect, nullptr);
    ogc::draw::Rect r = clonedRect->GetRect();
    EXPECT_DOUBLE_EQ(r.x, 10.0);
    EXPECT_DOUBLE_EQ(r.y, 20.0);
}

TEST(RectGeometryTest, TransformByMatrix) {
    auto geom = ogc::draw::RectGeometry::Create(10.0, 20.0, 100.0, 200.0);
    ogc::draw::TransformMatrix t = ogc::draw::TransformMatrix::Translate(5.0, 10.0);
    geom->TransformByMatrix(t);
    
    ogc::draw::Rect r = geom->GetRect();
    EXPECT_DOUBLE_EQ(r.x, 15.0);
    EXPECT_DOUBLE_EQ(r.y, 30.0);
}

TEST(CircleGeometryTest, DefaultConstructor) {
    auto geom = ogc::draw::CircleGeometry::Create();
    EXPECT_DOUBLE_EQ(geom->GetRadius(), 0.0);
    EXPECT_TRUE(geom->IsEmpty());
}

TEST(CircleGeometryTest, ParameterizedConstructor) {
    auto geom = ogc::draw::CircleGeometry::Create(10.0, 20.0, 5.0);
    
    EXPECT_DOUBLE_EQ(geom->GetCenter().x, 10.0);
    EXPECT_DOUBLE_EQ(geom->GetCenter().y, 20.0);
    EXPECT_DOUBLE_EQ(geom->GetRadius(), 5.0);
}

TEST(CircleGeometryTest, GetType) {
    auto geom = ogc::draw::CircleGeometry::Create();
    EXPECT_EQ(geom->GetGeometryType(), ogc::GeomType::kPolygon);
    EXPECT_STREQ(geom->GetGeometryTypeString(), "CIRCLE");
}

TEST(CircleGeometryTest, IsEmpty) {
    auto geom1 = ogc::draw::CircleGeometry::Create(10.0, 10.0, 5.0);
    EXPECT_FALSE(geom1->IsEmpty());
    
    auto geom2 = ogc::draw::CircleGeometry::Create(10.0, 10.0, 0.0);
    EXPECT_TRUE(geom2->IsEmpty());
}

TEST(CircleGeometryTest, Clone) {
    auto geom = ogc::draw::CircleGeometry::Create(10.0, 20.0, 5.0);
    ogc::GeometryPtr cloned = geom->Clone();
    
    ASSERT_NE(cloned, nullptr);
    EXPECT_EQ(cloned->GetGeometryType(), ogc::GeomType::kPolygon);
    
    ogc::draw::CircleGeometry* clonedCircle = dynamic_cast<ogc::draw::CircleGeometry*>(cloned.get());
    ASSERT_NE(clonedCircle, nullptr);
    EXPECT_DOUBLE_EQ(clonedCircle->GetCenter().x, 10.0);
    EXPECT_DOUBLE_EQ(clonedCircle->GetCenter().y, 20.0);
    EXPECT_DOUBLE_EQ(clonedCircle->GetRadius(), 5.0);
}

TEST(EllipseGeometryTest, DefaultConstructor) {
    auto geom = ogc::draw::EllipseGeometry::Create();
    EXPECT_DOUBLE_EQ(geom->GetRadiusX(), 0.0);
    EXPECT_DOUBLE_EQ(geom->GetRadiusY(), 0.0);
    EXPECT_TRUE(geom->IsEmpty());
}

TEST(EllipseGeometryTest, ParameterizedConstructor) {
    auto geom = ogc::draw::EllipseGeometry::Create(10.0, 20.0, 5.0, 8.0);
    
    EXPECT_DOUBLE_EQ(geom->GetCenter().x, 10.0);
    EXPECT_DOUBLE_EQ(geom->GetCenter().y, 20.0);
    EXPECT_DOUBLE_EQ(geom->GetRadiusX(), 5.0);
    EXPECT_DOUBLE_EQ(geom->GetRadiusY(), 8.0);
}

TEST(EllipseGeometryTest, GetType) {
    auto geom = ogc::draw::EllipseGeometry::Create();
    EXPECT_EQ(geom->GetGeometryType(), ogc::GeomType::kPolygon);
    EXPECT_STREQ(geom->GetGeometryTypeString(), "ELLIPSE");
}

TEST(EllipseGeometryTest, IsEmpty) {
    auto geom1 = ogc::draw::EllipseGeometry::Create(10.0, 10.0, 5.0, 8.0);
    EXPECT_FALSE(geom1->IsEmpty());
    
    auto geom2 = ogc::draw::EllipseGeometry::Create(10.0, 10.0, 0.0, 8.0);
    EXPECT_TRUE(geom2->IsEmpty());
    
    auto geom3 = ogc::draw::EllipseGeometry::Create(10.0, 10.0, 5.0, 0.0);
    EXPECT_TRUE(geom3->IsEmpty());
}

TEST(EllipseGeometryTest, Clone) {
    auto geom = ogc::draw::EllipseGeometry::Create(10.0, 20.0, 5.0, 8.0);
    ogc::GeometryPtr cloned = geom->Clone();
    
    ASSERT_NE(cloned, nullptr);
    EXPECT_EQ(cloned->GetGeometryType(), ogc::GeomType::kPolygon);
    
    ogc::draw::EllipseGeometry* clonedEllipse = dynamic_cast<ogc::draw::EllipseGeometry*>(cloned.get());
    ASSERT_NE(clonedEllipse, nullptr);
    EXPECT_DOUBLE_EQ(clonedEllipse->GetCenter().x, 10.0);
    EXPECT_DOUBLE_EQ(clonedEllipse->GetCenter().y, 20.0);
    EXPECT_DOUBLE_EQ(clonedEllipse->GetRadiusX(), 5.0);
    EXPECT_DOUBLE_EQ(clonedEllipse->GetRadiusY(), 8.0);
}

TEST(OGCGeometryTest, PointCreate) {
    auto point = ogc::Point::Create(10.0, 20.0);
    ASSERT_NE(point, nullptr);
    EXPECT_DOUBLE_EQ(point->GetX(), 10.0);
    EXPECT_DOUBLE_EQ(point->GetY(), 20.0);
    EXPECT_EQ(point->GetGeometryType(), ogc::GeomType::kPoint);
}

TEST(OGCGeometryTest, LineStringCreate) {
    auto line = ogc::LineString::Create();
    ASSERT_NE(line, nullptr);
    EXPECT_EQ(line->GetGeometryType(), ogc::GeomType::kLineString);
    EXPECT_TRUE(line->IsEmpty());
    
    line->AddPoint(ogc::Coordinate(0.0, 0.0));
    line->AddPoint(ogc::Coordinate(10.0, 20.0));
    EXPECT_EQ(line->GetNumPoints(), 2);
    EXPECT_FALSE(line->IsEmpty());
}

TEST(OGCGeometryTest, PolygonCreate) {
    ogc::CoordinateList coords;
    coords.push_back(ogc::Coordinate(0.0, 0.0));
    coords.push_back(ogc::Coordinate(10.0, 0.0));
    coords.push_back(ogc::Coordinate(10.0, 10.0));
    coords.push_back(ogc::Coordinate(0.0, 10.0));
    coords.push_back(ogc::Coordinate(0.0, 0.0));
    
    auto ring = ogc::LinearRing::Create(coords);
    auto polygon = ogc::Polygon::Create(std::move(ring));
    
    ASSERT_NE(polygon, nullptr);
    EXPECT_EQ(polygon->GetGeometryType(), ogc::GeomType::kPolygon);
    EXPECT_FALSE(polygon->IsEmpty());
}
