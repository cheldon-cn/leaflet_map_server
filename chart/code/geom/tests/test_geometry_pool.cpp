#include <gtest/gtest.h>
#include "ogc/geometry_pool.h"
#include "ogc/point.h"
#include "ogc/linestring.h"
#include "ogc/polygon.h"
#include "ogc/common.h"

namespace ogc {
namespace test {

class GeometryPoolTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_pool = &GeometryPool::Instance();
        m_pool->Clear();
    }
    
    void TearDown() override {}
    
    GeometryPool* m_pool;
};

TEST_F(GeometryPoolTest, Instance_ReturnsSingleton) {
    auto& pool1 = GeometryPool::Instance();
    auto& pool2 = GeometryPool::Instance();
    
    EXPECT_EQ(&pool1, &pool2);
}

TEST_F(GeometryPoolTest, AcquirePoint_ReturnsValidPoint) {
    auto point = m_pool->AcquirePoint();
    
    ASSERT_NE(point, nullptr);
}

TEST_F(GeometryPoolTest, ReleasePoint_ReturnsToPool) {
    auto point = m_pool->AcquirePoint();
    point->SetCoordinate(Coordinate(1, 2));
    
    m_pool->ReleasePoint(std::move(point));
    
    auto stats = m_pool->GetStatistics();
    EXPECT_GT(stats.pointPoolSize, 0);
}

TEST_F(GeometryPoolTest, AcquireLineString_ReturnsValidLineString) {
    auto line = m_pool->AcquireLineString();
    
    ASSERT_NE(line, nullptr);
}

TEST_F(GeometryPoolTest, ReleaseLineString_ReturnsToPool) {
    auto line = m_pool->AcquireLineString();
    m_pool->ReleaseLineString(std::move(line));
    
    auto stats = m_pool->GetStatistics();
    EXPECT_GT(stats.lineStringPoolSize, 0);
}

TEST_F(GeometryPoolTest, AcquirePolygon_ReturnsValidPolygon) {
    auto polygon = m_pool->AcquirePolygon();
    
    ASSERT_NE(polygon, nullptr);
}

TEST_F(GeometryPoolTest, ReleasePolygon_ReturnsToPool) {
    auto polygon = m_pool->AcquirePolygon();
    m_pool->ReleasePolygon(std::move(polygon));
    
    auto stats = m_pool->GetStatistics();
    EXPECT_GT(stats.polygonPoolSize, 0);
}

TEST_F(GeometryPoolTest, GetStatistics_ReturnsCorrectValues) {
    auto point = m_pool->AcquirePoint();
    auto line = m_pool->AcquireLineString();
    auto polygon = m_pool->AcquirePolygon();
    
    auto stats = m_pool->GetStatistics();
    
    EXPECT_GE(stats.totalCreated, 3);
}

TEST_F(GeometryPoolTest, Clear_RemovesAllPooledObjects) {
    auto point = m_pool->AcquirePoint();
    m_pool->ReleasePoint(std::move(point));
    
    m_pool->Clear();
    
    auto stats = m_pool->GetStatistics();
    EXPECT_EQ(stats.pointPoolSize, 0);
    EXPECT_EQ(stats.lineStringPoolSize, 0);
    EXPECT_EQ(stats.polygonPoolSize, 0);
}

TEST_F(GeometryPoolTest, SetPoolSize_UpdatesSize) {
    m_pool->SetPoolSize(128);
    
    EXPECT_EQ(m_pool->GetPoolSize(), 128);
}

TEST_F(GeometryPoolTest, ReusePoint_UsesPooledObject) {
    auto point1 = m_pool->AcquirePoint();
    point1->SetCoordinate(Coordinate(1, 2));
    m_pool->ReleasePoint(std::move(point1));
    
    auto point2 = m_pool->AcquirePoint();
    
    auto stats = m_pool->GetStatistics();
    EXPECT_GT(stats.totalReused, 0);
}

TEST_F(GeometryPoolTest, ReuseLineString_UsesPooledObject) {
    auto line1 = m_pool->AcquireLineString();
    m_pool->ReleaseLineString(std::move(line1));
    
    auto line2 = m_pool->AcquireLineString();
    
    auto stats = m_pool->GetStatistics();
    EXPECT_GT(stats.totalReused, 0);
}

TEST_F(GeometryPoolTest, ReusePolygon_UsesPooledObject) {
    auto polygon1 = m_pool->AcquirePolygon();
    m_pool->ReleasePolygon(std::move(polygon1));
    
    auto polygon2 = m_pool->AcquirePolygon();
    
    auto stats = m_pool->GetStatistics();
    EXPECT_GT(stats.totalReused, 0);
}

TEST_F(GeometryPoolTest, MultipleAcquireRelease_CyclesCorrectly) {
    for (int i = 0; i < 10; ++i) {
        auto point = m_pool->AcquirePoint();
        m_pool->ReleasePoint(std::move(point));
    }
    
    auto stats = m_pool->GetStatistics();
    EXPECT_GE(stats.totalReused, 9);
}

TEST_F(GeometryPoolTest, AcquireTemplate_Point_ReturnsValidPoint) {
    auto point = m_pool->Acquire<Point>();
    
    ASSERT_NE(point, nullptr);
}

TEST_F(GeometryPoolTest, AcquireTemplate_LineString_ReturnsValidLineString) {
    auto line = m_pool->Acquire<LineString>();
    
    ASSERT_NE(line, nullptr);
}

TEST_F(GeometryPoolTest, AcquireTemplate_Polygon_ReturnsValidPolygon) {
    auto polygon = m_pool->Acquire<Polygon>();
    
    ASSERT_NE(polygon, nullptr);
}

TEST_F(GeometryPoolTest, ReleaseTemplate_Point_ReturnsToPool) {
    auto point = m_pool->Acquire<Point>();
    m_pool->Release(std::move(point));
    
    auto stats = m_pool->GetStatistics();
    EXPECT_GT(stats.pointPoolSize, 0);
}

TEST_F(GeometryPoolTest, NullRelease_HandlesGracefully) {
    PointPtr nullPoint;
    m_pool->ReleasePoint(std::move(nullPoint));
    
    SUCCEED();
}

class ObjectPoolTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(ObjectPoolTest, Create_WithFactory_ReturnsValidPool) {
    auto factory = []() { return Point::Create(0, 0); };
    ObjectPool<Point> pool(factory, 10, 100);
    
    EXPECT_EQ(pool.Size(), 10);
}

TEST_F(ObjectPoolTest, Acquire_ReturnsValidObject) {
    auto factory = []() { return Point::Create(0, 0); };
    ObjectPool<Point> pool(factory, 10, 100);
    
    auto obj = pool.Acquire();
    
    ASSERT_NE(obj, nullptr);
}

TEST_F(ObjectPoolTest, Release_ReturnsObjectToPool) {
    auto factory = []() { return Point::Create(0, 0); };
    ObjectPool<Point> pool(factory, 0, 100);
    
    auto obj = pool.Acquire();
    pool.Release(std::move(obj));
    
    EXPECT_EQ(pool.Size(), 1);
}

TEST_F(ObjectPoolTest, MaxSize_LimitsPoolSize) {
    auto factory = []() { return Point::Create(0, 0); };
    ObjectPool<Point> pool(factory, 0, 5);
    
    for (int i = 0; i < 10; ++i) {
        auto obj = pool.Acquire();
        pool.Release(std::move(obj));
    }
    
    EXPECT_LE(pool.Size(), 5);
}

TEST_F(ObjectPoolTest, Clear_RemovesAllObjects) {
    auto factory = []() { return Point::Create(0, 0); };
    ObjectPool<Point> pool(factory, 20, 100);
    
    pool.Clear();
    
    EXPECT_EQ(pool.Size(), 0);
}

TEST_F(ObjectPoolTest, GetStatistics_ReturnsCorrectValues) {
    auto factory = []() { return Point::Create(0, 0); };
    ObjectPool<Point> pool(factory, 10, 100);
    
    auto stats = pool.GetStatistics();
    
    EXPECT_EQ(stats.poolSize, 10);
    EXPECT_EQ(stats.maxSize, 100);
}

TEST_F(ObjectPoolTest, Reuse_IncrementsReusedCounter) {
    auto factory = []() { return Point::Create(0, 0); };
    ObjectPool<Point> pool(factory, 1, 100);
    
    auto obj1 = pool.Acquire();
    pool.Release(std::move(obj1));
    
    auto obj2 = pool.Acquire();
    
    auto stats = pool.GetStatistics();
    EXPECT_GE(stats.reused, 1);
}

TEST_F(ObjectPoolTest, AcquireFromEmptyPool_CreatesNewObject) {
    auto factory = []() { return Point::Create(0, 0); };
    ObjectPool<Point> pool(factory, 0, 100);
    
    auto obj = pool.Acquire();
    
    ASSERT_NE(obj, nullptr);
    auto stats = pool.GetStatistics();
    EXPECT_GE(stats.created, 1);
}

}
}
