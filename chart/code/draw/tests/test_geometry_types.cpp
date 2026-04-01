#include <gtest/gtest.h>
#include "ogc/draw/geometry_types.h"

using namespace ogc::draw;

class PointTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(PointTest, DefaultConstructor) {
    Point p;
    EXPECT_DOUBLE_EQ(p.x, 0.0);
    EXPECT_DOUBLE_EQ(p.y, 0.0);
}

TEST_F(PointTest, ParameterizedConstructor) {
    Point p(10.5, 20.3);
    EXPECT_DOUBLE_EQ(p.x, 10.5);
    EXPECT_DOUBLE_EQ(p.y, 20.3);
}

TEST_F(PointTest, Equality) {
    Point p1(10.0, 20.0);
    Point p2(10.0, 20.0);
    Point p3(10.0, 21.0);
    
    EXPECT_TRUE(p1 == p2);
    EXPECT_FALSE(p1 == p3);
    EXPECT_TRUE(p1 != p3);
}

TEST_F(PointTest, Addition) {
    Point p1(10.0, 20.0);
    Point p2(5.0, 3.0);
    Point result = p1 + p2;
    
    EXPECT_DOUBLE_EQ(result.x, 15.0);
    EXPECT_DOUBLE_EQ(result.y, 23.0);
}

TEST_F(PointTest, Subtraction) {
    Point p1(10.0, 20.0);
    Point p2(5.0, 3.0);
    Point result = p1 - p2;
    
    EXPECT_DOUBLE_EQ(result.x, 5.0);
    EXPECT_DOUBLE_EQ(result.y, 17.0);
}

TEST_F(PointTest, ScalarMultiplication) {
    Point p(10.0, 20.0);
    Point result = p * 2.0;
    
    EXPECT_DOUBLE_EQ(result.x, 20.0);
    EXPECT_DOUBLE_EQ(result.y, 40.0);
}

TEST_F(PointTest, DistanceTo) {
    Point p1(0.0, 0.0);
    Point p2(3.0, 4.0);
    
    EXPECT_DOUBLE_EQ(p1.DistanceTo(p2), 5.0);
}

TEST_F(PointTest, ToString) {
    Point p(10.5, 20.3);
    std::string str = p.ToString();
    
    EXPECT_TRUE(str.find("10.5") != std::string::npos);
    EXPECT_TRUE(str.find("20.3") != std::string::npos);
}

class SizeTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(SizeTest, DefaultConstructor) {
    Size s;
    EXPECT_DOUBLE_EQ(s.w, 0.0);
    EXPECT_DOUBLE_EQ(s.h, 0.0);
}

TEST_F(SizeTest, ParameterizedConstructor) {
    Size s(100.5, 200.3);
    EXPECT_DOUBLE_EQ(s.w, 100.5);
    EXPECT_DOUBLE_EQ(s.h, 200.3);
}

TEST_F(SizeTest, Equality) {
    Size s1(100.0, 200.0);
    Size s2(100.0, 200.0);
    Size s3(100.0, 201.0);
    
    EXPECT_TRUE(s1 == s2);
    EXPECT_FALSE(s1 == s3);
    EXPECT_TRUE(s1 != s3);
}

TEST_F(SizeTest, IsEmpty) {
    Size s1(0.0, 100.0);
    Size s2(100.0, 0.0);
    Size s3(-10.0, 100.0);
    Size s4(100.0, 200.0);
    
    EXPECT_TRUE(s1.IsEmpty());
    EXPECT_TRUE(s2.IsEmpty());
    EXPECT_TRUE(s3.IsEmpty());
    EXPECT_FALSE(s4.IsEmpty());
}

TEST_F(SizeTest, GetArea) {
    Size s(10.0, 20.0);
    EXPECT_DOUBLE_EQ(s.GetArea(), 200.0);
}

TEST_F(SizeTest, ToString) {
    Size s(100.5, 200.3);
    std::string str = s.ToString();
    
    EXPECT_TRUE(str.find("100.5") != std::string::npos);
    EXPECT_TRUE(str.find("200.3") != std::string::npos);
}

class RectTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(RectTest, DefaultConstructor) {
    Rect r;
    EXPECT_DOUBLE_EQ(r.x, 0.0);
    EXPECT_DOUBLE_EQ(r.y, 0.0);
    EXPECT_DOUBLE_EQ(r.w, 0.0);
    EXPECT_DOUBLE_EQ(r.h, 0.0);
}

TEST_F(RectTest, ParameterizedConstructor) {
    Rect r(10.0, 20.0, 100.0, 200.0);
    EXPECT_DOUBLE_EQ(r.x, 10.0);
    EXPECT_DOUBLE_EQ(r.y, 20.0);
    EXPECT_DOUBLE_EQ(r.w, 100.0);
    EXPECT_DOUBLE_EQ(r.h, 200.0);
}

TEST_F(RectTest, FromLTRB) {
    Rect r = Rect::FromLTRB(10.0, 20.0, 110.0, 220.0);
    
    EXPECT_DOUBLE_EQ(r.x, 10.0);
    EXPECT_DOUBLE_EQ(r.y, 20.0);
    EXPECT_DOUBLE_EQ(r.w, 100.0);
    EXPECT_DOUBLE_EQ(r.h, 200.0);
}

TEST_F(RectTest, FromPoints) {
    Point p1(10.0, 20.0);
    Point p2(110.0, 220.0);
    Rect r = Rect::FromPoints(p1, p2);
    
    EXPECT_DOUBLE_EQ(r.x, 10.0);
    EXPECT_DOUBLE_EQ(r.y, 20.0);
    EXPECT_DOUBLE_EQ(r.w, 100.0);
    EXPECT_DOUBLE_EQ(r.h, 200.0);
}

TEST_F(RectTest, Getters) {
    Rect r(10.0, 20.0, 100.0, 200.0);
    
    EXPECT_DOUBLE_EQ(r.GetLeft(), 10.0);
    EXPECT_DOUBLE_EQ(r.GetTop(), 20.0);
    EXPECT_DOUBLE_EQ(r.GetRight(), 110.0);
    EXPECT_DOUBLE_EQ(r.GetBottom(), 220.0);
}

TEST_F(RectTest, GetCorners) {
    Rect r(10.0, 20.0, 100.0, 200.0);
    
    Point tl = r.GetTopLeft();
    EXPECT_DOUBLE_EQ(tl.x, 10.0);
    EXPECT_DOUBLE_EQ(tl.y, 20.0);
    
    Point tr = r.GetTopRight();
    EXPECT_DOUBLE_EQ(tr.x, 110.0);
    EXPECT_DOUBLE_EQ(tr.y, 20.0);
    
    Point bl = r.GetBottomLeft();
    EXPECT_DOUBLE_EQ(bl.x, 10.0);
    EXPECT_DOUBLE_EQ(bl.y, 220.0);
    
    Point br = r.GetBottomRight();
    EXPECT_DOUBLE_EQ(br.x, 110.0);
    EXPECT_DOUBLE_EQ(br.y, 220.0);
    
    Point c = r.GetCenter();
    EXPECT_DOUBLE_EQ(c.x, 60.0);
    EXPECT_DOUBLE_EQ(c.y, 120.0);
}

TEST_F(RectTest, GetSize) {
    Rect r(10.0, 20.0, 100.0, 200.0);
    Size s = r.GetSize();
    
    EXPECT_DOUBLE_EQ(s.w, 100.0);
    EXPECT_DOUBLE_EQ(s.h, 200.0);
}

TEST_F(RectTest, IsEmpty) {
    Rect r1(0.0, 0.0, 0.0, 100.0);
    Rect r2(0.0, 0.0, 100.0, 0.0);
    Rect r3(0.0, 0.0, 100.0, 200.0);
    
    EXPECT_TRUE(r1.IsEmpty());
    EXPECT_TRUE(r2.IsEmpty());
    EXPECT_FALSE(r3.IsEmpty());
}

TEST_F(RectTest, ContainsPoint) {
    Rect r(10.0, 20.0, 100.0, 200.0);
    
    EXPECT_TRUE(r.Contains(Point(50.0, 100.0)));
    EXPECT_TRUE(r.Contains(Point(10.0, 20.0)));
    EXPECT_FALSE(r.Contains(Point(5.0, 100.0)));
    EXPECT_FALSE(r.Contains(Point(200.0, 100.0)));
}

TEST_F(RectTest, ContainsRect) {
    Rect r(10.0, 20.0, 100.0, 200.0);
    
    EXPECT_TRUE(r.Contains(Rect(20.0, 30.0, 50.0, 50.0)));
    EXPECT_FALSE(r.Contains(Rect(5.0, 30.0, 50.0, 50.0)));
}

TEST_F(RectTest, Intersects) {
    Rect r1(10.0, 20.0, 100.0, 200.0);
    Rect r2(50.0, 100.0, 100.0, 200.0);
    Rect r3(200.0, 300.0, 100.0, 200.0);
    
    EXPECT_TRUE(r1.Intersects(r2));
    EXPECT_FALSE(r1.Intersects(r3));
}

TEST_F(RectTest, Intersect) {
    Rect r1(10.0, 20.0, 100.0, 200.0);
    Rect r2(50.0, 100.0, 100.0, 200.0);
    Rect result = r1.Intersect(r2);
    
    EXPECT_DOUBLE_EQ(result.x, 50.0);
    EXPECT_DOUBLE_EQ(result.y, 100.0);
    EXPECT_DOUBLE_EQ(result.w, 60.0);
    EXPECT_DOUBLE_EQ(result.h, 120.0);
}

TEST_F(RectTest, Union) {
    Rect r1(10.0, 20.0, 100.0, 200.0);
    Rect r2(50.0, 100.0, 100.0, 200.0);
    Rect result = r1.Union(r2);
    
    EXPECT_DOUBLE_EQ(result.x, 10.0);
    EXPECT_DOUBLE_EQ(result.y, 20.0);
    EXPECT_DOUBLE_EQ(result.w, 140.0);
    EXPECT_DOUBLE_EQ(result.h, 280.0);
}

TEST_F(RectTest, Inflate) {
    Rect r(10.0, 20.0, 100.0, 200.0);
    Rect result = r.Inflate(5.0, 10.0);
    
    EXPECT_DOUBLE_EQ(result.x, 5.0);
    EXPECT_DOUBLE_EQ(result.y, 10.0);
    EXPECT_DOUBLE_EQ(result.w, 110.0);
    EXPECT_DOUBLE_EQ(result.h, 220.0);
}

TEST_F(RectTest, Offset) {
    Rect r(10.0, 20.0, 100.0, 200.0);
    Rect result = r.Offset(5.0, 10.0);
    
    EXPECT_DOUBLE_EQ(result.x, 15.0);
    EXPECT_DOUBLE_EQ(result.y, 30.0);
    EXPECT_DOUBLE_EQ(result.w, 100.0);
    EXPECT_DOUBLE_EQ(result.h, 200.0);
}

TEST_F(RectTest, Equality) {
    Rect r1(10.0, 20.0, 100.0, 200.0);
    Rect r2(10.0, 20.0, 100.0, 200.0);
    Rect r3(10.0, 20.0, 100.0, 201.0);
    
    EXPECT_TRUE(r1 == r2);
    EXPECT_FALSE(r1 == r3);
    EXPECT_TRUE(r1 != r3);
}

TEST_F(RectTest, ToString) {
    Rect r(10.5, 20.3, 100.2, 200.1);
    std::string str = r.ToString();
    
    EXPECT_TRUE(str.find("10.5") != std::string::npos);
    EXPECT_TRUE(str.find("20.3") != std::string::npos);
}
