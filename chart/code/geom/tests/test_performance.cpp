#include <gtest/gtest.h>
#include <chrono>
#include "ogc/point.h"
#include "ogc/linestring.h"
#include "ogc/polygon.h"
#include "ogc/multipoint.h"
#include "ogc/multilinestring.h"
#include "ogc/multipolygon.h"
#include "ogc/geometrycollection.h"
#include "ogc/spatial_index.h"
#include "ogc/geometry_pool.h"
#include "ogc/factory.h"
#include "ogc/common.h"

namespace ogc {
namespace test {

class PerformanceTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_startTime = std::chrono::high_resolution_clock::now();
    }
    
    void TearDown() override {}
    
    std::chrono::high_resolution_clock::time_point m_startTime;
    
    double GetElapsedMs() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - m_startTime);
        return duration.count() / 1000.0;
    }
};

TEST_F(PerformanceTest, PointCreation_10000Points_CompletesQuickly) {
    const int count = 10000;
    std::vector<PointPtr> points;
    points.reserve(count);
    
    m_startTime = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < count; ++i) {
        points.push_back(Point::Create(i * 0.001, i * 0.001));
    }
    double elapsed = GetElapsedMs();
    
    EXPECT_EQ(points.size(), count);
    EXPECT_LT(elapsed, 100.0);
}

TEST_F(PerformanceTest, PointCreation_UsingPool_FasterThanDirect) {
    const int count = 10000;
    auto& pool = GeometryPool::Instance();
    pool.Clear();
    
    std::vector<PointPtr> pooledPoints;
    pooledPoints.reserve(count);
    
    m_startTime = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < count; ++i) {
        auto point = pool.AcquirePoint();
        point->SetCoordinate(Coordinate(i * 0.001, i * 0.001));
        pooledPoints.push_back(std::move(point));
    }
    double pooledTime = GetElapsedMs();
    
    std::vector<PointPtr> directPoints;
    directPoints.reserve(count);
    
    m_startTime = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < count; ++i) {
        directPoints.push_back(Point::Create(i * 0.001, i * 0.001));
    }
    double directTime = GetElapsedMs();
    
    EXPECT_EQ(pooledPoints.size(), count);
    EXPECT_EQ(directPoints.size(), count);
}

TEST_F(PerformanceTest, LineStringCreation_1000Lines_CompletesQuickly) {
    const int count = 1000;
    std::vector<LineStringPtr> lines;
    lines.reserve(count);
    
    m_startTime = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < count; ++i) {
        CoordinateList coords;
        coords.emplace_back(0, 0);
        coords.emplace_back(i, i);
        coords.emplace_back(i * 2, 0);
        lines.push_back(LineString::Create(coords));
    }
    double elapsed = GetElapsedMs();
    
    EXPECT_EQ(lines.size(), count);
    EXPECT_LT(elapsed, 100.0);
}

TEST_F(PerformanceTest, PolygonArea_1000Polygons_CompletesQuickly) {
    const int count = 1000;
    std::vector<PolygonPtr> polygons;
    
    for (int i = 0; i < count; ++i) {
        polygons.push_back(Polygon::CreateRectangle(0, 0, 100 + i, 100 + i));
    }
    
    m_startTime = std::chrono::high_resolution_clock::now();
    double totalArea = 0;
    for (const auto& poly : polygons) {
        totalArea += poly->Area();
    }
    double elapsed = GetElapsedMs();
    
    EXPECT_GT(totalArea, 0);
    EXPECT_LT(elapsed, 50.0);
}

TEST_F(PerformanceTest, PolygonContains_1000Points_CompletesQuickly) {
    auto polygon = Polygon::CreateRectangle(0, 0, 100, 100);
    const int count = 1000;
    
    m_startTime = std::chrono::high_resolution_clock::now();
    int containsCount = 0;
    for (int i = 0; i < count; ++i) {
        auto point = Point::Create(50, 50);
        if (polygon->Contains(point.get())) {
            containsCount++;
        }
    }
    double elapsed = GetElapsedMs();
    
    EXPECT_EQ(containsCount, count);
    EXPECT_LT(elapsed, 100.0);
}

TEST_F(PerformanceTest, MultiPolygonCreation_100Polygons_CompletesQuickly) {
    const int count = 100;
    std::vector<PolygonPtr> polygons;
    
    for (int i = 0; i < count; ++i) {
        polygons.push_back(Polygon::CreateRectangle(i * 10, i * 10, i * 10 + 5, i * 10 + 5));
    }
    
    m_startTime = std::chrono::high_resolution_clock::now();
    auto multiPolygon = MultiPolygon::Create(std::move(polygons));
    double elapsed = GetElapsedMs();
    
    EXPECT_EQ(multiPolygon->GetNumGeometries(), count);
    EXPECT_LT(elapsed, 50.0);
}

TEST_F(PerformanceTest, GeometryCollectionArea_MixedTypes_CompletesQuickly) {
    std::vector<GeometryPtr> geometries;
    const int count = 100;
    
    for (int i = 0; i < count; ++i) {
        if (i % 3 == 0) {
            geometries.push_back(Polygon::CreateRectangle(0, 0, 10, 10));
        } else if (i % 3 == 1) {
            geometries.push_back(Point::Create(5, 5));
        } else {
            CoordinateList coords;
            coords.emplace_back(0, 0);
            coords.emplace_back(10, 10);
            geometries.push_back(LineString::Create(coords));
        }
    }
    
    auto collection = GeometryCollection::Create(std::move(geometries));
    
    m_startTime = std::chrono::high_resolution_clock::now();
    double area = collection->Area();
    double elapsed = GetElapsedMs();
    
    EXPECT_GT(area, 0);
    EXPECT_LT(elapsed, 50.0);
}

TEST_F(PerformanceTest, RTreeInsert_10000Items_CompletesQuickly) {
    RTree<int> tree;
    const int count = 10000;
    
    m_startTime = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < count; ++i) {
        Envelope env(i, i + 1, i, i + 1);
        tree.Insert(env, i);
    }
    double elapsed = GetElapsedMs();
    
    EXPECT_EQ(tree.Size(), count);
    EXPECT_LT(elapsed, 500.0);
}

TEST_F(PerformanceTest, RTreeQuery_10000Items_CompletesQuickly) {
    RTree<int> tree;
    const int count = 10000;
    
    for (int i = 0; i < count; ++i) {
        Envelope env(i, i, i + 1, i + 1);
        tree.Insert(env, i);
    }
    
    m_startTime = std::chrono::high_resolution_clock::now();
    auto results = tree.Query(Envelope(1000, 1000, 2000, 2000));
    double elapsed = GetElapsedMs();
    
    EXPECT_GT(results.size(), static_cast<size_t>(0));
    EXPECT_LT(elapsed, 10.0);
}

TEST_F(PerformanceTest, RTreeBulkLoad_10000Items_FasterThanIndividual) {
    const int count = 10000;
    std::vector<std::pair<Envelope, int>> items;
    items.reserve(count);
    
    for (int i = 0; i < count; ++i) {
        items.emplace_back(Envelope(i, i + 1, i, i + 1), i);
    }
    
    m_startTime = std::chrono::high_resolution_clock::now();
    RTree<int> bulkTree;
    bulkTree.BulkLoad(items);
    double bulkTime = GetElapsedMs();
    
    m_startTime = std::chrono::high_resolution_clock::now();
    RTree<int> individualTree;
    for (const auto& item : items) {
        individualTree.Insert(item.first, item.second);
    }
    double individualTime = GetElapsedMs();
    
    EXPECT_EQ(bulkTree.Size(), count);
    EXPECT_EQ(individualTree.Size(), count);
}

TEST_F(PerformanceTest, QuadtreeQuery_10000Items_CompletesQuickly) {
    Quadtree<int>::Config config;
    config.bounds = Envelope(0, 0, 10000, 10000);
    Quadtree<int> tree(config);
    const int count = 10000;
    
    for (int i = 0; i < count; ++i) {
        Envelope env(i, i, i + 1, i + 1);
        tree.Insert(env, i);
    }
    
    m_startTime = std::chrono::high_resolution_clock::now();
    auto results = tree.Query(Envelope(1000, 1000, 2000, 2000));
    double elapsed = GetElapsedMs();
    
    EXPECT_GT(results.size(), static_cast<size_t>(0));
    EXPECT_LT(elapsed, 10.0);
}

TEST_F(PerformanceTest, GeometryPoolReuse_10000Cycles_EfficientReuse) {
    auto& pool = GeometryPool::Instance();
    pool.Clear();
    
    const int cycles = 10000;
    
    m_startTime = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < cycles; ++i) {
        auto point = pool.AcquirePoint();
        point->SetCoordinate(Coordinate(i, i));
        pool.ReleasePoint(std::move(point));
    }
    double elapsed = GetElapsedMs();
    
    auto stats = pool.GetStatistics();
    EXPECT_GT(stats.totalReused, static_cast<size_t>(0));
    EXPECT_LT(elapsed, 200.0);
}

TEST_F(PerformanceTest, CoordinateDistance_10000Calculations_CompletesQuickly) {
    const int count = 10000;
    Coordinate c1(0, 0);
    
    m_startTime = std::chrono::high_resolution_clock::now();
    double totalDist = 0;
    for (int i = 0; i < count; ++i) {
        Coordinate c2(i * 0.1, i * 0.1);
        totalDist += c1.Distance(c2);
    }
    double elapsed = GetElapsedMs();
    
    EXPECT_GT(totalDist, 0);
    EXPECT_LT(elapsed, 10.0);
}

TEST_F(PerformanceTest, EnvelopeIntersects_10000Checks_CompletesQuickly) {
    const int count = 10000;
    Envelope env1(0, 0, 100, 100);
    
    m_startTime = std::chrono::high_resolution_clock::now();
    int intersectCount = 0;
    for (int i = 0; i < count; ++i) {
        Envelope env2(50, 50, 150 + i * 0.001, 150 + i * 0.001);
        if (env1.Intersects(env2)) {
            intersectCount++;
        }
    }
    double elapsed = GetElapsedMs();
    
    EXPECT_GT(intersectCount, 0);
    EXPECT_LT(elapsed, 10.0);
}

TEST_F(PerformanceTest, PolygonClone_1000Clones_CompletesQuickly) {
    auto original = Polygon::CreateRectangle(0, 0, 100, 100);
    const int count = 1000;
    std::vector<GeometryPtr> clones;
    clones.reserve(count);
    
    m_startTime = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < count; ++i) {
        clones.push_back(original->Clone());
    }
    double elapsed = GetElapsedMs();
    
    EXPECT_EQ(clones.size(), count);
    EXPECT_LT(elapsed, 100.0);
}

TEST_F(PerformanceTest, LineStringLength_1000Lines_CompletesQuickly) {
    std::vector<LineStringPtr> lines;
    const int count = 1000;
    
    for (int i = 0; i < count; ++i) {
        CoordinateList coords;
        for (int j = 0; j < 100; ++j) {
            coords.emplace_back(j, j + i * 0.01);
        }
        lines.push_back(LineString::Create(coords));
    }
    
    m_startTime = std::chrono::high_resolution_clock::now();
    double totalLength = 0;
    for (const auto& line : lines) {
        totalLength += line->Length();
    }
    double elapsed = GetElapsedMs();
    
    EXPECT_GT(totalLength, 0);
    EXPECT_LT(elapsed, 100.0);
}

TEST_F(PerformanceTest, MultiPointCreation_1000Points_CompletesQuickly) {
    const int count = 1000;
    std::vector<MultiPointPtr> multiPoints;
    multiPoints.reserve(count);
    
    m_startTime = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < count; ++i) {
        auto mp = MultiPoint::Create();
        for (int j = 0; j < 10; ++j) {
            mp->AddPoint(Point::Create(i + j, i + j));
        }
        multiPoints.push_back(std::move(mp));
    }
    double elapsed = GetElapsedMs();
    
    EXPECT_EQ(multiPoints.size(), count);
    EXPECT_LT(elapsed, 200.0);
}

TEST_F(PerformanceTest, MultiPolygonContains_100Checks_CompletesQuickly) {
    std::vector<MultiPolygonPtr> multiPolygons;
    for (int i = 0; i < 10; ++i) {
        auto mp = MultiPolygon::Create();
        for (int j = 0; j < 5; ++j) {
            mp->AddPolygon(Polygon::CreateRectangle(i * 10 + j, i * 10 + j, i * 10 + j + 5, i * 10 + j + 5));
        }
        multiPolygons.push_back(std::move(mp));
    }
    
    m_startTime = std::chrono::high_resolution_clock::now();
    int containsCount = 0;
    for (const auto& mp : multiPolygons) {
        for (int i = 0; i < 10; ++i) {
            auto point = Point::Create(i * 10 + 2.5, i * 10 + 2.5);
            if (mp->Contains(point.get())) {
                containsCount++;
            }
        }
    }
    double elapsed = GetElapsedMs();
    
    EXPECT_GT(containsCount, 0);
    EXPECT_LT(elapsed, 100.0);
}

TEST_F(PerformanceTest, GeometryCollectionIteration_100Collections_CompletesQuickly) {
    std::vector<GeometryCollectionPtr> collections;
    for (int i = 0; i < 100; ++i) {
        auto gc = GeometryCollection::Create();
        for (int j = 0; j < 50; ++j) {
            gc->AddGeometry(Point::Create(i + j, i + j));
        }
        collections.push_back(std::move(gc));
    }
    
    m_startTime = std::chrono::high_resolution_clock::now();
    size_t totalCount = 0;
    for (const auto& gc : collections) {
        totalCount += gc->GetNumGeometries();
    }
    double elapsed = GetElapsedMs();
    
    EXPECT_EQ(totalCount, static_cast<size_t>(5000));
    EXPECT_LT(elapsed, 50.0);
}

TEST_F(PerformanceTest, AsTextGeneration_1000Geometries_CompletesQuickly) {
    std::vector<PointPtr> points;
    for (int i = 0; i < 1000; ++i) {
        points.push_back(Point::Create(i * 0.001, i * 0.001));
    }
    
    m_startTime = std::chrono::high_resolution_clock::now();
    size_t totalLength = 0;
    for (const auto& point : points) {
        totalLength += point->AsText().length();
    }
    double elapsed = GetElapsedMs();
    
    EXPECT_GT(totalLength, static_cast<size_t>(0));
    EXPECT_LT(elapsed, 100.0);
}

TEST_F(PerformanceTest, EnvelopeExpand_10000Operations_CompletesQuickly) {
    const int count = 10000;
    Envelope env(0, 0, 0, 0);
    
    m_startTime = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < count; ++i) {
        env.ExpandToInclude(Coordinate(i * 0.01, i * 0.01));
    }
    double elapsed = GetElapsedMs();
    
    EXPECT_GT(env.GetWidth(), 0);
    EXPECT_LT(elapsed, 10.0);
}

TEST_F(PerformanceTest, Coordinate3DOperations_10000Ops_CompletesQuickly) {
    const int count = 10000;
    std::vector<Coordinate> coords;
    coords.reserve(count);
    
    m_startTime = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < count; ++i) {
        coords.emplace_back(i * 0.001, i * 0.001, i * 0.001);
    }
    double elapsed = GetElapsedMs();
    
    EXPECT_EQ(coords.size(), count);
    EXPECT_LT(elapsed, 10.0);
}

TEST_F(PerformanceTest, FactoryCreateCircle_1000Circles_CompletesQuickly) {
    auto& factory = GeometryFactory::GetInstance();
    const int count = 1000;
    std::vector<PolygonPtr> circles;
    circles.reserve(count);
    
    m_startTime = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < count; ++i) {
        circles.push_back(factory.CreateCircle(i, i, 10.0, 16));
    }
    double elapsed = GetElapsedMs();
    
    EXPECT_EQ(circles.size(), count);
    EXPECT_LT(elapsed, 500.0);
}

TEST_F(PerformanceTest, SpatialIndexNearestQuery_100Queries_CompletesQuickly) {
    RTree<int> tree;
    for (int i = 0; i < 1000; ++i) {
        Envelope env(i, i + 1, i, i + 1);
        tree.Insert(env, i);
    }
    
    m_startTime = std::chrono::high_resolution_clock::now();
    int totalFound = 0;
    for (int i = 0; i < 100; ++i) {
        auto nearest = tree.QueryNearest(Coordinate(i * 10, i * 10), 5);
        totalFound += static_cast<int>(nearest.size());
    }
    double elapsed = GetElapsedMs();
    
    EXPECT_GT(totalFound, 0);
    EXPECT_LT(elapsed, 50.0);
}

TEST_F(PerformanceTest, ObjectPoolStress_50000AcquireRelease_CompletesQuickly) {
    auto& pool = GeometryPool::Instance();
    pool.Clear();
    
    const int cycles = 50000;
    
    m_startTime = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < cycles; ++i) {
        auto point = pool.AcquirePoint();
        point->SetCoordinate(Coordinate(i, i));
        pool.ReleasePoint(std::move(point));
    }
    double elapsed = GetElapsedMs();
    
    auto stats = pool.GetStatistics();
    EXPECT_GT(stats.totalReused, static_cast<size_t>(0));
    EXPECT_LT(elapsed, 1000.0);
}

}
}
