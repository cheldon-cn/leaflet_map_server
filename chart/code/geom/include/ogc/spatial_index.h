#pragma once

/**
 * @file spatial_index.h
 * @brief 空间索引定义，包含R-Tree、Quadtree和GridIndex
 */

#include "geometry.h"
#include "envelope.h"
#include "export.h"
#include <vector>
#include <functional>
#include <memory>
#include <fstream>

namespace ogc {

/**
 * @brief 空间索引接口类
 * @tparam T 索引项类
 */
template<typename T>
class ISpatialIndex {
public:
    virtual ~ISpatialIndex() = default;
    
    virtual GeomResult Insert(const Envelope& envelope, const T& item) = 0;
    virtual GeomResult BulkLoad(const std::vector<std::pair<Envelope, T>>& items) = 0;
    virtual bool Remove(const Envelope& envelope, const T& item) = 0;
    virtual void Clear() noexcept = 0;
    
    virtual std::vector<T> Query(const Envelope& envelope) const = 0;
    virtual std::vector<T> Query(const Coordinate& point) const = 0;
    virtual std::vector<T> QueryIntersects(const Geometry* geom) const = 0;
    virtual std::vector<T> QueryNearest(const Coordinate& point, size_t k) const = 0;
    
    virtual size_t Size() const noexcept = 0;
    virtual bool IsEmpty() const noexcept = 0;
    virtual Envelope GetBounds() const = 0;
    virtual size_t GetHeight() const = 0;
    virtual size_t GetNodeCount() const = 0;
    
    virtual GeomResult Serialize(std::ostream& output) const = 0;
    virtual GeomResult Deserialize(std::istream& input) = 0;
    virtual size_t GetSerializedSize() const noexcept = 0;
    
    GeomResult SaveToFile(const std::string& filepath) const {
        std::ofstream file(filepath, std::ios::binary);
        if (!file) {
            return GeomResult::kIOError;
        }
        return Serialize(file);
    }
    
    GeomResult LoadFromFile(const std::string& filepath) {
        std::ifstream file(filepath, std::ios::binary);
        if (!file) {
            return GeomResult::kIOError;
        }
        return Deserialize(file);
    }
};

/**
 * @brief R-Tree空间索引类
 * @tparam T 索引项类
 */
template<typename T>
class OGC_GEOM_API RTree : public ISpatialIndex<T> {
public:
    enum class SplitStrategy {
        Linear,
        Quadratic,
        RStar
    };
    
    struct Config {
        size_t maxEntries = 16;
        size_t minEntries = 4;
        SplitStrategy splitStrategy = SplitStrategy::RStar;
        bool enableReinsert = true;
        double reinsertRatio = 0.3;
    };
    
    explicit RTree(const Config& config = Config());
    ~RTree() override;
    
    GeomResult Insert(const Envelope& envelope, const T& item) override;
    GeomResult BulkLoad(const std::vector<std::pair<Envelope, T>>& items) override;
    bool Remove(const Envelope& envelope, const T& item) override;
    void Clear() noexcept override;
    
    std::vector<T> Query(const Envelope& envelope) const override;
    std::vector<T> Query(const Coordinate& point) const override;
    std::vector<T> QueryIntersects(const Geometry* geom) const override;
    std::vector<T> QueryNearest(const Coordinate& point, size_t k) const override;
    
    size_t Size() const noexcept override { return m_size; }
    bool IsEmpty() const noexcept override { return m_size == 0; }
    Envelope GetBounds() const override;
    size_t GetHeight() const override;
    size_t GetNodeCount() const override;
    
    GeomResult Serialize(std::ostream& output) const override;
    GeomResult Deserialize(std::istream& input) override;
    size_t GetSerializedSize() const noexcept override;
    
    void SetConfig(const Config& config);
    const Config& GetConfig() const noexcept { return m_config; }
    
    struct Statistics {
        size_t totalNodes;
        size_t leafNodes;
        size_t internalNodes;
        size_t height;
        double avgNodeUtilization;
        size_t totalReinserts;
    };
    Statistics GetStatistics() const;
    
private:
    struct Node {
        Envelope bounds;
        bool isLeaf;
        std::vector<std::pair<Envelope, T>> entries;
        std::vector<std::unique_ptr<Node>> children;
    };
    
    std::unique_ptr<Node> m_root;
    Config m_config;
    size_t m_size = 0;
    
    Node* ChooseLeaf(const Envelope& envelope);
    void SplitNode(Node* node);
    void AdjustTree(Node* node);
    void Reinsert(Node* node, size_t level);
    Node* FindParent(Node* current, Node* target);
};

/**
 * @brief 四叉树空间索引类
 * @tparam T 索引项类
 */
template<typename T>
class OGC_GEOM_API Quadtree : public ISpatialIndex<T> {
public:
    struct Config {
        size_t maxItemsPerNode = 16;
        size_t maxDepth = 16;
        Envelope bounds;
    };
    
    explicit Quadtree(const Config& config = Config());
    
    GeomResult Insert(const Envelope& envelope, const T& item) override;
    GeomResult BulkLoad(const std::vector<std::pair<Envelope, T>>& items) override;
    bool Remove(const Envelope& envelope, const T& item) override;
    void Clear() noexcept override;
    
    std::vector<T> Query(const Envelope& envelope) const override;
    std::vector<T> Query(const Coordinate& point) const override;
    std::vector<T> QueryIntersects(const Geometry* geom) const override;
    std::vector<T> QueryNearest(const Coordinate& point, size_t k) const override;
    
    size_t Size() const noexcept override;
    bool IsEmpty() const noexcept override;
    Envelope GetBounds() const override;
    size_t GetHeight() const override;
    size_t GetNodeCount() const override;
    
    GeomResult Serialize(std::ostream& output) const override;
    GeomResult Deserialize(std::istream& input) override;
    size_t GetSerializedSize() const noexcept override;
    
private:
    struct QuadNode {
        Envelope bounds;
        std::vector<std::pair<Envelope, T>> items;
        std::unique_ptr<QuadNode> children[4];
    };
    
    std::unique_ptr<QuadNode> m_root;
    Config m_config;
    size_t m_size = 0;
    
    GeomResult InsertRecursive(QuadNode* node, const Envelope& envelope, const T& item, size_t depth);
    void SplitNode(QuadNode* node);
    void QueryRecursive(const QuadNode* node, const Envelope& envelope, std::vector<T>& results) const;
};

/**
 * @brief 网格空间索引类
 * @tparam T 索引项类
 */
template<typename T>
class OGC_GEOM_API GridIndex : public ISpatialIndex<T> {
public:
    struct Config {
        size_t gridX = 100;
        size_t gridY = 100;
        Envelope bounds;
    };
    
    explicit GridIndex(const Config& config = Config());
    
    GeomResult Insert(const Envelope& envelope, const T& item) override;
    GeomResult BulkLoad(const std::vector<std::pair<Envelope, T>>& items) override;
    bool Remove(const Envelope& envelope, const T& item) override;
    void Clear() noexcept override;
    
    std::vector<T> Query(const Envelope& envelope) const override;
    std::vector<T> Query(const Coordinate& point) const override;
    std::vector<T> QueryIntersects(const Geometry* geom) const override;
    std::vector<T> QueryNearest(const Coordinate& point, size_t k) const override;
    
    size_t Size() const noexcept override;
    bool IsEmpty() const noexcept override;
    Envelope GetBounds() const override;
    size_t GetHeight() const override { return 1; }
    size_t GetNodeCount() const override;
    
    GeomResult Serialize(std::ostream& output) const override;
    GeomResult Deserialize(std::istream& input) override;
    size_t GetSerializedSize() const noexcept override;
    
private:
    std::vector<std::vector<std::pair<Envelope, T>>> m_grid;
    Config m_config;
    size_t m_size = 0;
    
    size_t GetCellIndex(const Coordinate& coord) const;
    std::pair<size_t, size_t> GetCellIndices(const Coordinate& coord) const;
};

}
