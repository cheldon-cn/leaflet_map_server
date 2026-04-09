#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <thread>
#include <chrono>
#include "ogc/geom/geometry.h"
#include "ogc/geom/point.h"
#include "ogc/geom/linestring.h"
#include "ogc/geom/polygon.h"
#include "ogc/geom/coordinate.h"
#include "ogc/geom/envelope.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace ogc;

class DatabaseIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(DatabaseIntegrationTest, GeometryStorageSimulation) {
    std::vector<PointPtr> points;
    for (int i = 0; i < 100; ++i) {
        points.push_back(Point::Create(Coordinate(i * 0.1, i * 0.1)));
    }
    
    EXPECT_EQ(points.size(), 100u);
    
    for (const auto& point : points) {
        EXPECT_TRUE(point->IsValid());
    }
    
    std::vector<LineStringPtr> lines;
    for (int i = 0; i < 50; ++i) {
        std::vector<Coordinate> coords = {
            Coordinate(i * 0.1, 0),
            Coordinate(i * 0.1 + 1, 1)
        };
        lines.push_back(LineString::Create(coords));
    }
    
    EXPECT_EQ(lines.size(), 50u);
    
    for (const auto& line : lines) {
        EXPECT_TRUE(line->IsValid());
    }
}

TEST_F(DatabaseIntegrationTest, EnvelopeIndexing) {
    std::vector<Envelope> envelopes;
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            envelopes.push_back(Envelope(i * 10, j * 10, (i + 1) * 10, (j + 1) * 10));
        }
    }
    
    EXPECT_EQ(envelopes.size(), 100u);
    
    Envelope queryEnv(45, 45, 55, 55);
    int intersectCount = 0;
    for (const auto& env : envelopes) {
        if (env.Intersects(queryEnv)) {
            intersectCount++;
        }
    }
    
    EXPECT_EQ(intersectCount, 4);
}

TEST_F(DatabaseIntegrationTest, GeometrySerialization) {
    auto point = Point::Create(Coordinate(116.397428, 39.90923));
    std::string wkt = point->AsText();
    EXPECT_FALSE(wkt.empty());
    EXPECT_TRUE(wkt.find("POINT") != std::string::npos);
    
    std::vector<Coordinate> coords = {
        Coordinate(0, 0),
        Coordinate(10, 10),
        Coordinate(20, 5)
    };
    auto lineString = LineString::Create(coords);
    std::string lineWkt = lineString->AsText();
    EXPECT_FALSE(lineWkt.empty());
    EXPECT_TRUE(lineWkt.find("LINESTRING") != std::string::npos);
}

TEST_F(DatabaseIntegrationTest, BatchGeometryOperations) {
    std::vector<PointPtr> points;
    points.reserve(1000);
    
    for (int i = 0; i < 1000; ++i) {
        points.push_back(Point::Create(Coordinate(i * 0.01, i * 0.01)));
    }
    
    EXPECT_EQ(points.size(), 1000u);
    
    Envelope totalEnv;
    for (const auto& point : points) {
        totalEnv.ExpandToInclude(point->GetCoordinate());
    }
    
    EXPECT_DOUBLE_EQ(totalEnv.GetMinX(), 0.0);
    EXPECT_DOUBLE_EQ(totalEnv.GetMinY(), 0.0);
    EXPECT_NEAR(totalEnv.GetMaxX(), 9.99, 0.01);
    EXPECT_NEAR(totalEnv.GetMaxY(), 9.99, 0.01);
}

TEST_F(DatabaseIntegrationTest, GeometryValidation) {
    auto validPoint = Point::Create(Coordinate(116.397428, 39.90923));
    EXPECT_TRUE(validPoint->IsValid());
    
    auto emptyPoint = Point::Create(Coordinate::Empty());
    EXPECT_TRUE(emptyPoint->IsEmpty());
    
    std::vector<Coordinate> validCoords = {
        Coordinate(0, 0),
        Coordinate(10, 10)
    };
    auto validLine = LineString::Create(validCoords);
    EXPECT_TRUE(validLine->IsValid());
    
    std::vector<Coordinate> emptyCoords;
    auto emptyLine = LineString::Create(emptyCoords);
    EXPECT_TRUE(emptyLine->IsEmpty());
}
