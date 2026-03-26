#include <gtest/gtest.h>
#include "ogc/spatial_index.h"
#include "ogc/point.h"
#include "ogc/polygon.h"
#include "ogc/common.h"

namespace ogc {
namespace test {

class RTreeTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_config.maxEntries = 4;
        m_config.minEntries = 2;
        m_config.splitStrategy = RTree<int>::SplitStrategy::Linear;
    }
    
    void TearDown() override {}
    
    typename RTree<int>::Config m_config;
};

TEST_F(RTreeTest, Create_Default_ReturnsEmptyTree) {
    RTree<int> tree(m_config);
    EXPECT_TRUE(tree.IsEmpty());
    EXPECT_EQ(tree.Size(), 0);
}

TEST_F(RTreeTest, Insert_SingleItem_ReturnsSuccess) {
    RTree<int> tree(m_config);
    
    Envelope env(0, 10, 0, 10);
    GeomResult result = tree.Insert(env, 1);
    
    EXPECT_EQ(result, GeomResult::kSuccess);
    EXPECT_EQ(tree.Size(), 1);
    EXPECT_FALSE(tree.IsEmpty());
}

TEST_F(RTreeTest, Insert_MultipleItems_ReturnsSuccess) {
    RTree<int> tree(m_config);
    
    for (int i = 0; i < 10; ++i) {
        Envelope env(i * 10, i * 10 + 10, i * 10, i * 10 + 10);
        EXPECT_EQ(tree.Insert(env, i), GeomResult::kSuccess);
    }
    
    EXPECT_EQ(tree.Size(), 10);
}

TEST_F(RTreeTest, BulkLoad_MultipleItems_ReturnsSuccess) {
    RTree<int> tree(m_config);
    
    std::vector<std::pair<Envelope, int>> items;
    for (int i = 0; i < 100; ++i) {
        Envelope env(i, i + 1, i, i + 1);
        items.emplace_back(env, i);
    }
    
    GeomResult result = tree.BulkLoad(items);
    EXPECT_EQ(result, GeomResult::kSuccess);
    EXPECT_EQ(tree.Size(), 100);
}

TEST_F(RTreeTest, Query_Envelope_ReturnsCorrectItems) {
    RTree<int> tree(m_config);
    
    tree.Insert(Envelope(0, 0, 10, 10), 1);
    tree.Insert(Envelope(5, 5, 15, 15), 2);
    tree.Insert(Envelope(20, 20, 30, 30), 3);
    
    std::vector<int> results = tree.Query(Envelope(0, 0, 15, 15));
    EXPECT_EQ(results.size(), 2);
}

TEST_F(RTreeTest, Query_Point_ReturnsCorrectItems) {
    RTree<int> tree(m_config);
    
    tree.Insert(Envelope(0, 0, 10, 10), 1);
    tree.Insert(Envelope(20, 20, 30, 30), 2);
    
    std::vector<int> results = tree.Query(Coordinate(5, 5));
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(results[0], 1);
}

TEST_F(RTreeTest, Query_PointOutside_ReturnsEmpty) {
    RTree<int> tree(m_config);
    
    tree.Insert(Envelope(0, 0, 10, 10), 1);
    
    std::vector<int> results = tree.Query(Coordinate(50, 50));
    EXPECT_TRUE(results.empty());
}

TEST_F(RTreeTest, QueryNearest_ReturnsCorrectItems) {
    RTree<int> tree(m_config);
    
    tree.Insert(Envelope(0, 0, 1, 1), 1);
    tree.Insert(Envelope(10, 10, 11, 11), 2);
    tree.Insert(Envelope(20, 20, 21, 21), 3);
    
    std::vector<int> results = tree.QueryNearest(Coordinate(5, 5), 2);
    EXPECT_EQ(results.size(), 2);
}

TEST_F(RTreeTest, QueryIntersects_Geometry_ReturnsCorrectItems) {
    RTree<int> tree(m_config);
    
    tree.Insert(Envelope(0, 0, 10, 10), 1);
    tree.Insert(Envelope(20, 20, 30, 30), 2);
    
    auto polygon = Polygon::CreateRectangle(5, 5, 15, 15);
    std::vector<int> results = tree.QueryIntersects(polygon.get());
    EXPECT_EQ(results.size(), 1);
}

TEST_F(RTreeTest, Remove_ExistingItem_ReturnsTrue) {
    RTree<int> tree(m_config);
    
    Envelope env(0, 0, 10, 10);
    tree.Insert(env, 1);
    
    bool result = tree.Remove(env, 1);
    EXPECT_TRUE(result);
    EXPECT_EQ(tree.Size(), 0);
}

TEST_F(RTreeTest, Remove_NonExistingItem_ReturnsFalse) {
    RTree<int> tree(m_config);
    
    tree.Insert(Envelope(0, 0, 10, 10), 1);
    
    bool result = tree.Remove(Envelope(20, 20, 30, 30), 2);
    EXPECT_FALSE(result);
}

TEST_F(RTreeTest, Clear_RemovesAllItems) {
    RTree<int> tree(m_config);
    
    for (int i = 0; i < 10; ++i) {
        tree.Insert(Envelope(i, i + 1, i, i + 1), i);
    }
    
    tree.Clear();
    EXPECT_TRUE(tree.IsEmpty());
    EXPECT_EQ(tree.Size(), 0);
}

TEST_F(RTreeTest, GetBounds_ReturnsCorrectEnvelope) {
    RTree<int> tree(m_config);
    
    tree.Insert(Envelope(0, 0, 10, 10), 1);
    tree.Insert(Envelope(20, 20, 30, 30), 2);
    
    Envelope bounds = tree.GetBounds();
    EXPECT_DOUBLE_EQ(bounds.GetMinX(), 0);
    EXPECT_DOUBLE_EQ(bounds.GetMaxX(), 30);
    EXPECT_DOUBLE_EQ(bounds.GetMinY(), 0);
    EXPECT_DOUBLE_EQ(bounds.GetMaxY(), 30);
}

TEST_F(RTreeTest, GetHeight_ReturnsCorrectValue) {
    RTree<int> tree(m_config);
    
    EXPECT_EQ(tree.GetHeight(), 0);
    
    tree.Insert(Envelope(0, 0, 10, 10), 1);
    EXPECT_GE(tree.GetHeight(), 1);
}

TEST_F(RTreeTest, GetNodeCount_ReturnsCorrectValue) {
    RTree<int> tree(m_config);
    
    EXPECT_EQ(tree.GetNodeCount(), 0);
    
    for (int i = 0; i < 10; ++i) {
        tree.Insert(Envelope(i, i + 1, i, i + 1), i);
    }
    
    EXPECT_GT(tree.GetNodeCount(), 0);
}

TEST_F(RTreeTest, SetConfig_UpdatesConfiguration) {
    RTree<int> tree(m_config);
    
    typename RTree<int>::Config newConfig;
    newConfig.maxEntries = 8;
    newConfig.minEntries = 3;
    
    tree.SetConfig(newConfig);
}

class QuadtreeTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_config.maxItemsPerNode = 4;
        m_config.maxDepth = 8;
        m_config.bounds = Envelope(0, 0, 100, 100);
    }
    
    void TearDown() override {}
    
    typename Quadtree<int>::Config m_config;
};

TEST_F(QuadtreeTest, Create_Default_ReturnsEmptyTree) {
    Quadtree<int> tree(m_config);
    EXPECT_TRUE(tree.IsEmpty());
    EXPECT_EQ(tree.Size(), 0);
}

TEST_F(QuadtreeTest, Insert_SingleItem_ReturnsSuccess) {
    Quadtree<int> tree(m_config);
    
    Envelope env(10, 20, 10, 20);
    GeomResult result = tree.Insert(env, 1);
    
    EXPECT_EQ(result, GeomResult::kSuccess);
    EXPECT_EQ(tree.Size(), 1);
}

TEST_F(QuadtreeTest, Insert_MultipleItems_ReturnsSuccess) {
    Quadtree<int> tree(m_config);
    
    for (int i = 0; i < 20; ++i) {
        Envelope env(i * 5, i * 5 + 5, i * 5, i * 5 + 5);
        EXPECT_EQ(tree.Insert(env, i), GeomResult::kSuccess);
    }
    
    EXPECT_EQ(tree.Size(), 20);
}

TEST_F(QuadtreeTest, Query_Envelope_ReturnsCorrectItems) {
    Quadtree<int> tree(m_config);
    
    tree.Insert(Envelope(10, 10, 20, 20), 1);
    tree.Insert(Envelope(30, 30, 40, 40), 2);
    tree.Insert(Envelope(50, 50, 60, 60), 3);
    
    std::vector<int> results = tree.Query(Envelope(15, 15, 35, 35));
    EXPECT_EQ(results.size(), 2);
}

TEST_F(QuadtreeTest, Query_Point_ReturnsCorrectItems) {
    Quadtree<int> tree(m_config);
    
    tree.Insert(Envelope(10, 10, 20, 20), 1);
    tree.Insert(Envelope(30, 30, 40, 40), 2);
    
    std::vector<int> results = tree.Query(Coordinate(15, 15));
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(results[0], 1);
}

TEST_F(QuadtreeTest, QueryNearest_ReturnsCorrectItems) {
    Quadtree<int> tree(m_config);
    
    tree.Insert(Envelope(10, 10, 11, 11), 1);
    tree.Insert(Envelope(30, 30, 31, 31), 2);
    tree.Insert(Envelope(50, 50, 51, 51), 3);
    
    std::vector<int> results = tree.QueryNearest(Coordinate(20, 20), 2);
    EXPECT_EQ(results.size(), 2);
}

TEST_F(QuadtreeTest, Remove_ExistingItem_ReturnsTrue) {
    Quadtree<int> tree(m_config);
    
    Envelope env(10, 10, 20, 20);
    tree.Insert(env, 1);
    
    bool result = tree.Remove(env, 1);
    EXPECT_TRUE(result);
    EXPECT_EQ(tree.Size(), 0);
}

TEST_F(QuadtreeTest, Clear_RemovesAllItems) {
    Quadtree<int> tree(m_config);
    
    for (int i = 0; i < 10; ++i) {
        tree.Insert(Envelope(i * 5, i * 5, i * 5 + 5, i * 5 + 5), i);
    }
    
    tree.Clear();
    EXPECT_TRUE(tree.IsEmpty());
}

TEST_F(QuadtreeTest, GetBounds_ReturnsCorrectEnvelope) {
    Quadtree<int> tree(m_config);
    
    Envelope bounds = tree.GetBounds();
    EXPECT_DOUBLE_EQ(bounds.GetMinX(), 0);
    EXPECT_DOUBLE_EQ(bounds.GetMaxX(), 100);
}

TEST_F(QuadtreeTest, GetNodeCount_ReturnsCorrectValue) {
    Quadtree<int> tree(m_config);
    
    for (int i = 0; i < 20; ++i) {
        tree.Insert(Envelope(i * 5, i * 5, i * 5 + 5, i * 5 + 5), i);
    }
    
    EXPECT_GT(tree.GetNodeCount(), 0);
}

class GridIndexTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_config.gridX = 10;
        m_config.gridY = 10;
        m_config.bounds = Envelope(0, 0, 100, 100);
    }
    
    void TearDown() override {}
    
    typename GridIndex<int>::Config m_config;
};

TEST_F(GridIndexTest, Create_Default_ReturnsEmptyIndex) {
    GridIndex<int> index(m_config);
    EXPECT_TRUE(index.IsEmpty());
    EXPECT_EQ(index.Size(), 0);
}

TEST_F(GridIndexTest, Insert_SingleItem_ReturnsSuccess) {
    GridIndex<int> index(m_config);
    
    Envelope env(10, 10, 20, 20);
    GeomResult result = index.Insert(env, 1);
    
    EXPECT_EQ(result, GeomResult::kSuccess);
    EXPECT_EQ(index.Size(), 1);
}

TEST_F(GridIndexTest, Insert_MultipleItems_ReturnsSuccess) {
    GridIndex<int> index(m_config);
    
    for (int i = 0; i < 50; ++i) {
        Envelope env(i * 2, i * 2, i * 2 + 1, i * 2 + 1);
        EXPECT_EQ(index.Insert(env, i), GeomResult::kSuccess);
    }
    
    EXPECT_EQ(index.Size(), 50);
}

TEST_F(GridIndexTest, Query_Envelope_ReturnsCorrectItems) {
    GridIndex<int> index(m_config);
    
    index.Insert(Envelope(10, 10, 20, 20), 1);
    index.Insert(Envelope(30, 30, 40, 40), 2);
    index.Insert(Envelope(50, 50, 60, 60), 3);
    
    std::vector<int> results = index.Query(Envelope(15, 15, 55, 55));
    EXPECT_GE(results.size(), 2);
}

TEST_F(GridIndexTest, Query_Point_ReturnsCorrectItems) {
    GridIndex<int> index(m_config);
    
    index.Insert(Envelope(10, 10, 20, 20), 1);
    
    std::vector<int> results = index.Query(Coordinate(15, 15));
    EXPECT_EQ(results.size(), 1);
}

TEST_F(GridIndexTest, Clear_RemovesAllItems) {
    GridIndex<int> index(m_config);
    
    for (int i = 0; i < 20; ++i) {
        index.Insert(Envelope(i * 5, i * 5, i * 5 + 5, i * 5 + 5), i);
    }
    
    index.Clear();
    EXPECT_TRUE(index.IsEmpty());
}

TEST_F(GridIndexTest, GetBounds_ReturnsCorrectEnvelope) {
    GridIndex<int> index(m_config);
    
    Envelope bounds = index.GetBounds();
    EXPECT_DOUBLE_EQ(bounds.GetMinX(), 0);
    EXPECT_DOUBLE_EQ(bounds.GetMaxX(), 100);
}

}
}
