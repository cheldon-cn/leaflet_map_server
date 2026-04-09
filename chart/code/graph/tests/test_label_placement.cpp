#include <gtest/gtest.h>
#include "ogc/graph/label/label_placement.h"
#include "ogc/geom/geometry.h"
#include "ogc/geom/point.h"
#include "ogc/geom/linestring.h"
#include "ogc/geom/polygon.h"
#include "ogc/geom/envelope.h"
#include <memory>

using namespace ogc::graph;
using namespace ogc;

class LabelPlacementTest : public ::testing::Test {
protected:
    void SetUp() override {
        placement = LabelPlacement::Create();
        ASSERT_NE(placement, nullptr);
    }
    
    void TearDown() override {
        placement.reset();
    }
    
    LabelPlacementPtr placement;
};

TEST_F(LabelPlacementTest, DefaultConstructor) {
    LabelPlacementPtr defaultPlacement = LabelPlacement::Create();
    EXPECT_NE(defaultPlacement, nullptr);
}

TEST_F(LabelPlacementTest, SetPlacementMode) {
    placement->SetPlacementMode(LabelPlacementMode::kPoint);
    EXPECT_EQ(placement->GetPlacementMode(), LabelPlacementMode::kPoint);
    
    placement->SetPlacementMode(LabelPlacementMode::kLine);
    EXPECT_EQ(placement->GetPlacementMode(), LabelPlacementMode::kLine);
    
    placement->SetPlacementMode(LabelPlacementMode::kInterior);
    EXPECT_EQ(placement->GetPlacementMode(), LabelPlacementMode::kInterior);
    
    placement->SetPlacementMode(LabelPlacementMode::kAuto);
    EXPECT_EQ(placement->GetPlacementMode(), LabelPlacementMode::kAuto);
}

TEST_F(LabelPlacementTest, SetOffset) {
    placement->SetOffset(10.0, 20.0);
    
    double dx, dy;
    placement->GetOffset(dx, dy);
    
    EXPECT_DOUBLE_EQ(dx, 10.0);
    EXPECT_DOUBLE_EQ(dy, 20.0);
}

TEST_F(LabelPlacementTest, SetAnchorPoint) {
    placement->SetAnchorPoint(0.5, 0.5);
    
    double x, y;
    placement->GetAnchorPoint(x, y);
    
    EXPECT_DOUBLE_EQ(x, 0.5);
    EXPECT_DOUBLE_EQ(y, 0.5);
}

TEST_F(LabelPlacementTest, SetDisplacement) {
    placement->SetDisplacement(5.0, 10.0);
    
    double dx, dy;
    placement->GetDisplacement(dx, dy);
    
    EXPECT_DOUBLE_EQ(dx, 5.0);
    EXPECT_DOUBLE_EQ(dy, 10.0);
}

TEST_F(LabelPlacementTest, SetRotation) {
    placement->SetRotation(45.0);
    EXPECT_DOUBLE_EQ(placement->GetRotation(), 45.0);
    
    placement->SetRotation(-30.0);
    EXPECT_DOUBLE_EQ(placement->GetRotation(), -30.0);
}

TEST_F(LabelPlacementTest, SetAutoRotation) {
    placement->SetAutoRotation(true);
    EXPECT_TRUE(placement->GetAutoRotation());
    
    placement->SetAutoRotation(false);
    EXPECT_FALSE(placement->GetAutoRotation());
}

TEST_F(LabelPlacementTest, SetMaxDisplacement) {
    placement->SetMaxDisplacement(100.0);
    EXPECT_DOUBLE_EQ(placement->GetMaxDisplacement(), 100.0);
}

TEST_F(LabelPlacementTest, SetRepeatDistance) {
    placement->SetRepeatDistance(200.0);
    EXPECT_DOUBLE_EQ(placement->GetRepeatDistance(), 200.0);
}

TEST_F(LabelPlacementTest, SetMinPadding) {
    placement->SetMinPadding(5.0);
    EXPECT_DOUBLE_EQ(placement->GetMinPadding(), 5.0);
}

TEST_F(LabelPlacementTest, GeneratePointCandidates) {
    ogc::PointPtr point = ogc::Point::Create(100, 100);
    
    std::vector<PlacementCandidate> candidates = 
        placement->GeneratePointCandidates(point.get(), 50.0, 20.0);
    
    EXPECT_FALSE(candidates.empty());
}

TEST_F(LabelPlacementTest, GetBestCandidate) {
    ogc::PointPtr point = ogc::Point::Create(100, 100);
    
    PlacementCandidate best = 
        placement->GetBestCandidate(point.get(), "Test", 50.0, 20.0);
    
    EXPECT_TRUE(best.isValid);
}

TEST_F(LabelPlacementTest, GenerateCandidatesForPoint) {
    ogc::PointPtr point = ogc::Point::Create(50, 50);
    
    placement->SetPlacementMode(LabelPlacementMode::kPoint);
    std::vector<PlacementCandidate> candidates = 
        placement->GenerateCandidates(point.get(), "Test", 50.0, 20.0);
    
    EXPECT_FALSE(candidates.empty());
}

TEST_F(LabelPlacementTest, GenerateLineCandidates) {
    ogc::CoordinateList coords;
    coords.push_back(ogc::Coordinate(0, 0));
    coords.push_back(ogc::Coordinate(100, 0));
    coords.push_back(ogc::Coordinate(100, 100));
    
    ogc::LineStringPtr line = ogc::LineString::Create(coords);
    
    std::vector<PlacementCandidate> candidates = 
        placement->GenerateLineCandidates(line.get(), "Test", 50.0, 20.0);
    
    EXPECT_FALSE(candidates.empty());
}

TEST_F(LabelPlacementTest, GeneratePolygonCandidates) {
    ogc::PolygonPtr polygon = ogc::Polygon::CreateRectangle(0, 0, 100, 100);
    
    placement->SetPlacementMode(LabelPlacementMode::kInterior);
    std::vector<PlacementCandidate> candidates = 
        placement->GeneratePolygonCandidates(polygon.get(), 50.0, 20.0);
    
    EXPECT_FALSE(candidates.empty());
}

TEST_F(LabelPlacementTest, PlacementCandidateDefaultValues) {
    PlacementCandidate candidate;
    
    EXPECT_DOUBLE_EQ(candidate.x, 0);
    EXPECT_DOUBLE_EQ(candidate.y, 0);
    EXPECT_DOUBLE_EQ(candidate.rotation, 0);
    EXPECT_DOUBLE_EQ(candidate.score, 0);
    EXPECT_EQ(candidate.position, LabelPosition::kCenter);
    EXPECT_FALSE(candidate.isValid);
}

TEST_F(LabelPlacementTest, OffsetAffectsPlacement) {
    ogc::PointPtr point = ogc::Point::Create(100, 100);
    
    placement->SetOffset(10, 20);
    PlacementCandidate candidate = 
        placement->GetBestCandidate(point.get(), "Test", 50.0, 20.0);
    
    EXPECT_TRUE(candidate.isValid);
}

TEST_F(LabelPlacementTest, RotationAffectsPlacement) {
    ogc::PointPtr point = ogc::Point::Create(100, 100);
    
    placement->SetRotation(45.0);
    PlacementCandidate candidate = 
        placement->GetBestCandidate(point.get(), "Test", 50.0, 20.0);
    
    EXPECT_TRUE(candidate.isValid);
}
