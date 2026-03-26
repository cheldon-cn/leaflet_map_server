#include "ogc/spatial_index.h"
#include "ogc/point.h"
#include <algorithm>
#include <queue>
#include <cmath>
#include <memory>
#include <string>
#include <set>

namespace ogc {

namespace {

double EnlargeArea(const Envelope& envelope, const Envelope& toAdd) {
    Envelope merged = envelope.Union(toAdd);
    return merged.GetArea() - envelope.GetArea();
}

}

template<typename T>
RTree<T>::RTree(const Config& config) : m_config(config) {
    m_root = std::make_unique<Node>();
    m_root->isLeaf = true;
}

template<typename T>
RTree<T>::~RTree() = default;

template<typename T>
GeomResult RTree<T>::Insert(const Envelope& envelope, const T& item) {
    if (envelope.IsNull()) {
        return GeomResult::kInvalidGeometry;
    }
    
    m_root->bounds.ExpandToInclude(envelope);
    
    Node* leaf = ChooseLeaf(envelope);
    
    leaf->entries.emplace_back(envelope, item);
    leaf->bounds.ExpandToInclude(envelope);
    m_size++;
    
    if (leaf->entries.size() > m_config.maxEntries) {
        SplitNode(leaf);
    } else {
        AdjustTree(leaf);
    }
    
    return GeomResult::kSuccess;
}

template<typename T>
GeomResult RTree<T>::BulkLoad(const std::vector<std::pair<Envelope, T>>& items) {
    if (items.empty()) {
        return GeomResult::kSuccess;
    }
    
    Clear();
    
    std::vector<std::pair<Envelope, T>> sortedItems = items;
    std::sort(sortedItems.begin(), sortedItems.end(),
              [](const auto& a, const auto& b) {
                  double centerA_x = (a.first.GetMinX() + a.first.GetMaxX()) / 2;
                  double centerA_y = (a.first.GetMinY() + a.first.GetMaxY()) / 2;
                  double centerB_x = (b.first.GetMinX() + b.first.GetMaxX()) / 2;
                  double centerB_y = (b.first.GetMinY() + b.first.GetMaxY()) / 2;
                  
                  if (std::abs(centerA_x - centerB_x) > DEFAULT_TOLERANCE) {
                      return centerA_x < centerB_x;
                  }
                  return centerA_y < centerB_y;
              });
    
    for (const auto& item : sortedItems) {
        Insert(item.first, item.second);
    }
    
    return GeomResult::kSuccess;
}

template<typename T>
bool RTree<T>::Remove(const Envelope& envelope, const T& item) {
    if (!m_root || envelope.IsNull()) {
        return false;
    }
    
    std::function<bool(Node*, const Envelope&, const T&)> removeRecursive;
    removeRecursive = [&](Node* node, const Envelope& env, const T& value) -> bool {
        if (node->isLeaf) {
            for (auto it = node->entries.begin(); it != node->entries.end(); ++it) {
                if (it->first == env && it->second == value) {
                    node->entries.erase(it);
                    m_size--;
                    return true;
                }
            }
            return false;
        }
        
        for (auto& child : node->children) {
            if (child->bounds.Intersects(env)) {
                if (removeRecursive(child.get(), env, value)) {
                    child->bounds = Envelope();
                    for (const auto& entry : child->entries) {
                        child->bounds.ExpandToInclude(entry.first);
                    }
                    for (const auto& grandchild : child->children) {
                        child->bounds.ExpandToInclude(grandchild->bounds);
                    }
                    return true;
                }
            }
        }
        return false;
    };
    
    bool result = removeRecursive(m_root.get(), envelope, item);
    
    if (result) {
        m_root->bounds = Envelope();
        for (const auto& entry : m_root->entries) {
            m_root->bounds.ExpandToInclude(entry.first);
        }
        for (const auto& child : m_root->children) {
            m_root->bounds.ExpandToInclude(child->bounds);
        }
    }
    
    return result;
}

template<typename T>
void RTree<T>::Clear() noexcept {
    m_root = std::make_unique<Node>();
    m_root->isLeaf = true;
    m_size = 0;
}

template<typename T>
std::vector<T> RTree<T>::Query(const Envelope& envelope) const {
    std::vector<T> results;
    
    if (!m_root || envelope.IsNull()) {
        return results;
    }
    
    std::queue<Node*> queue;
    queue.push(m_root.get());
    
    while (!queue.empty()) {
        Node* node = queue.front();
        queue.pop();
        
        if (!node->bounds.Intersects(envelope)) {
            continue;
        }
        
        if (node->isLeaf) {
            for (const auto& entry : node->entries) {
                if (entry.first.Intersects(envelope)) {
                    results.push_back(entry.second);
                }
            }
        } else {
            for (auto& child : node->children) {
                if (child->bounds.Intersects(envelope)) {
                    queue.push(child.get());
                }
            }
        }
    }
    
    return results;
}

template<typename T>
std::vector<T> RTree<T>::Query(const Coordinate& point) const {
    return Query(Envelope(point, point));
}

template<typename T>
std::vector<T> RTree<T>::QueryIntersects(const Geometry* geom) const {
    return Query(geom->GetEnvelope());
}

template<typename T>
std::vector<T> RTree<T>::QueryNearest(const Coordinate& point, size_t k) const {
    std::vector<std::pair<double, T>> distances;
    
    if (!m_root) {
        return std::vector<T>();
    }
    
    std::priority_queue<std::pair<double, Node*>> nodeQueue;
    nodeQueue.push(std::make_pair(-m_root->bounds.Distance(point), m_root.get()));
    
    while (!nodeQueue.empty() && distances.size() < k) {
        std::pair<double, Node*> top = nodeQueue.top();
        nodeQueue.pop();
        
        double negDist = top.first;
        Node* node = top.second;
        
        if (node->isLeaf) {
            for (const auto& entry : node->entries) {
                double dist = entry.first.Distance(point);
                distances.emplace_back(dist, entry.second);
            }
            
            if (distances.size() >= k) {
                std::partial_sort(distances.begin(), distances.begin() + k, distances.end(),
                                  [](const std::pair<double, T>& a, const std::pair<double, T>& b) { return a.first < b.first; });
                distances.resize(k);
            }
        } else {
            for (auto& child : node->children) {
                double dist = child->bounds.Distance(point);
                nodeQueue.push(std::make_pair(-dist, child.get()));
            }
        }
    }
    
    std::sort(distances.begin(), distances.end(),
              [](const std::pair<double, T>& a, const std::pair<double, T>& b) { return a.first < b.first; });
    
    std::vector<T> results;
    for (size_t i = 0; i < std::min(k, distances.size()); ++i) {
        results.push_back(distances[i].second);
    }
    
    return results;
}

template<typename T>
Envelope RTree<T>::GetBounds() const {
    if (!m_root || m_size == 0) return Envelope();
    return m_root->bounds;
}

template<typename T>
size_t RTree<T>::GetHeight() const {
    if (!m_root || m_root->entries.empty()) return 0;
    
    size_t height = 1;
    Node* node = m_root.get();
    while (!node->isLeaf) {
        height++;
        node = node->children[0].get();
    }
    return height;
}

template<typename T>
size_t RTree<T>::GetNodeCount() const {
    if (!m_root || m_size == 0) return 0;
    
    size_t count = 0;
    std::queue<Node*> queue;
    queue.push(m_root.get());
    
    while (!queue.empty()) {
        Node* node = queue.front();
        queue.pop();
        count++;
        
        for (auto& child : node->children) {
            queue.push(child.get());
        }
    }
    
    return count;
}

template<typename T>
GeomResult RTree<T>::Serialize(std::ostream& output) const {
    return GeomResult::kNotImplemented;
}

template<typename T>
GeomResult RTree<T>::Deserialize(std::istream& input) {
    return GeomResult::kNotImplemented;
}

template<typename T>
size_t RTree<T>::GetSerializedSize() const noexcept {
    return 0;
}

template<typename T>
void RTree<T>::SetConfig(const Config& config) {
    m_config = config;
}

template<typename T>
typename RTree<T>::Statistics RTree<T>::GetStatistics() const {
    Statistics stats;
    stats.totalNodes = GetNodeCount();
    stats.leafNodes = 0;
    stats.internalNodes = 0;
    stats.height = GetHeight();
    stats.avgNodeUtilization = 0.0;
    stats.totalReinserts = 0;
    
    if (!m_root) return stats;
    
    std::queue<Node*> queue;
    queue.push(m_root.get());
    
    double totalEntries = 0;
    double totalCapacity = 0;
    
    while (!queue.empty()) {
        Node* node = queue.front();
        queue.pop();
        
        if (node->isLeaf) {
            stats.leafNodes++;
            totalEntries += node->entries.size();
            totalCapacity += m_config.maxEntries;
        } else {
            stats.internalNodes++;
            totalEntries += node->children.size();
            totalCapacity += m_config.maxEntries;
        }
        
        for (auto& child : node->children) {
            queue.push(child.get());
        }
    }
    
    if (totalCapacity > 0) {
        stats.avgNodeUtilization = totalEntries / totalCapacity;
    }
    
    return stats;
}

template<typename T>
typename RTree<T>::Node* RTree<T>::ChooseLeaf(const Envelope& envelope) {
    Node* node = m_root.get();
    
    while (!node->isLeaf) {
        size_t best = 0;
        double minEnlarge = std::numeric_limits<double>::max();
        double minArea = std::numeric_limits<double>::max();
        
        for (size_t i = 0; i < node->children.size(); ++i) {
            double enlarge = EnlargeArea(node->children[i]->bounds, envelope);
            double area = node->children[i]->bounds.GetArea();
            
            if (enlarge < minEnlarge || (enlarge == minEnlarge && area < minArea)) {
                minEnlarge = enlarge;
                minArea = area;
                best = i;
            }
        }
        
        node = node->children[best].get();
        node->bounds.ExpandToInclude(envelope);
    }
    
    return node;
}

template<typename T>
void RTree<T>::SplitNode(Node* node) {
    std::vector<std::pair<Envelope, T>> entries = std::move(node->entries);
    std::vector<std::unique_ptr<Node>> children = std::move(node->children);
    
    if (children.empty()) {
        size_t seed1 = 0, seed2 = 1;
        double maxWaste = -1.0;
        for (size_t i = 0; i < entries.size(); ++i) {
            for (size_t j = i + 1; j < entries.size(); ++j) {
                Envelope combined = entries[i].first.Union(entries[j].first);
                double waste = combined.GetArea() - entries[i].first.GetArea() - entries[j].first.GetArea();
                
                if (waste > maxWaste) {
                    maxWaste = waste;
                    seed1 = i;
                    seed2 = j;
                }
            }
        }
        
        std::unique_ptr<Node> newNode(new Node());
        newNode->isLeaf = true;
        
        node->entries.clear();
        node->bounds = Envelope();
        newNode->bounds = Envelope();
        
        node->entries.push_back(entries[seed1]);
        node->bounds.ExpandToInclude(entries[seed1].first);
        
        newNode->entries.push_back(entries[seed2]);
        newNode->bounds.ExpandToInclude(entries[seed2].first);
        
        std::vector<bool> assigned(entries.size(), false);
        assigned[seed1] = true;
        assigned[seed2] = true;
        
        for (size_t i = 0; i < entries.size(); ++i) {
            if (assigned[i]) continue;
            
            double enlarge1 = EnlargeArea(node->bounds, entries[i].first);
            double enlarge2 = EnlargeArea(newNode->bounds, entries[i].first);
            
            if (enlarge1 < enlarge2 || 
                (enlarge1 == enlarge2 && node->entries.size() < newNode->entries.size())) {
                node->entries.push_back(entries[i]);
                node->bounds.ExpandToInclude(entries[i].first);
            } else {
                newNode->entries.push_back(entries[i]);
                newNode->bounds.ExpandToInclude(entries[i].first);
            }
            assigned[i] = true;
        }
        
        if (node == m_root.get()) {
            std::unique_ptr<Node> newRoot(new Node());
            newRoot->isLeaf = false;
            
            std::unique_ptr<Node> leftNode(new Node());
            leftNode->isLeaf = node->isLeaf;
            leftNode->entries = std::move(node->entries);
            leftNode->children = std::move(node->children);
            leftNode->bounds = node->bounds;
            
            newRoot->children.push_back(std::move(leftNode));
            newRoot->children.push_back(std::move(newNode));
            newRoot->bounds = newRoot->children[0]->bounds.Union(newRoot->children[1]->bounds);
            
            m_root = std::move(newRoot);
        } else {
            Node* parent = FindParent(m_root.get(), node);
            if (parent) {
                parent->children.push_back(std::move(newNode));
                parent->bounds = Envelope();
                for (auto& child : parent->children) {
                    parent->bounds.ExpandToInclude(child->bounds);
                }
                
                if (parent->children.size() > m_config.maxEntries) {
                    SplitNode(parent);
                }
            }
        }
    } else {
        size_t seed1 = 0, seed2 = 1;
        double maxWaste = -1.0;
        for (size_t i = 0; i < children.size(); ++i) {
            for (size_t j = i + 1; j < children.size(); ++j) {
                Envelope combined = children[i]->bounds.Union(children[j]->bounds);
                double waste = combined.GetArea() - children[i]->bounds.GetArea() - children[j]->bounds.GetArea();
                
                if (waste > maxWaste) {
                    maxWaste = waste;
                    seed1 = i;
                    seed2 = j;
                }
            }
        }
        
        std::unique_ptr<Node> newNode(new Node());
        newNode->isLeaf = false;
        
        node->children.clear();
        node->bounds = Envelope();
        newNode->bounds = Envelope();
        
        node->children.push_back(std::move(children[seed1]));
        node->bounds.ExpandToInclude(node->children.back()->bounds);
        
        newNode->children.push_back(std::move(children[seed2]));
        newNode->bounds.ExpandToInclude(newNode->children.back()->bounds);
        
        std::vector<bool> assigned(children.size(), false);
        assigned[seed1] = true;
        assigned[seed2] = true;
        
        for (size_t i = 0; i < children.size(); ++i) {
            if (assigned[i]) continue;
            
            double enlarge1 = EnlargeArea(node->bounds, children[i]->bounds);
            double enlarge2 = EnlargeArea(newNode->bounds, children[i]->bounds);
            
            if (enlarge1 < enlarge2 || 
                (enlarge1 == enlarge2 && node->children.size() < newNode->children.size())) {
                node->children.push_back(std::move(children[i]));
                node->bounds.ExpandToInclude(node->children.back()->bounds);
            } else {
                newNode->children.push_back(std::move(children[i]));
                newNode->bounds.ExpandToInclude(newNode->children.back()->bounds);
            }
            assigned[i] = true;
        }
        
        if (node == m_root.get()) {
            std::unique_ptr<Node> newRoot(new Node());
            newRoot->isLeaf = false;
            
            std::unique_ptr<Node> leftNode(new Node());
            leftNode->isLeaf = false;
            leftNode->children = std::move(node->children);
            leftNode->bounds = node->bounds;
            
            newRoot->children.push_back(std::move(leftNode));
            newRoot->children.push_back(std::move(newNode));
            newRoot->bounds = newRoot->children[0]->bounds.Union(newRoot->children[1]->bounds);
            
            m_root = std::move(newRoot);
        } else {
            Node* parent = FindParent(m_root.get(), node);
            if (parent) {
                parent->children.push_back(std::move(newNode));
                parent->bounds = Envelope();
                for (auto& child : parent->children) {
                    parent->bounds.ExpandToInclude(child->bounds);
                }
                
                if (parent->children.size() > m_config.maxEntries) {
                    SplitNode(parent);
                }
            }
        }
    }
}

template<typename T>
void RTree<T>::AdjustTree(Node* node) {
    if (!node || node == m_root.get()) return;
    
    while (node) {
        Node* parent = FindParent(m_root.get(), node);
        if (parent) {
            parent->bounds.ExpandToInclude(node->bounds);
            node = parent;
            if (node == m_root.get()) break;
        } else {
            break;
        }
    }
}

template<typename T>
typename RTree<T>::Node* RTree<T>::FindParent(Node* current, Node* target) {
    if (!current || current->isLeaf) return nullptr;
    
    for (auto& child : current->children) {
        if (child.get() == target) {
            return current;
        }
        Node* result = FindParent(child.get(), target);
        if (result) return result;
    }
    return nullptr;
}

template<typename T>
void RTree<T>::Reinsert(Node* node, size_t level) {
}

template class RTree<Geometry*>;
template class RTree<size_t>;
template class RTree<int>;
template class RTree<std::shared_ptr<Point>>;

template<typename T>
Quadtree<T>::Quadtree(const Config& config) : m_config(config) {
    if (!m_config.bounds.IsNull()) {
        m_root = std::make_unique<QuadNode>();
        m_root->bounds = m_config.bounds;
    }
}

template<typename T>
GeomResult Quadtree<T>::Insert(const Envelope& envelope, const T& item) {
    if (envelope.IsNull()) {
        return GeomResult::kInvalidGeometry;
    }
    
    if (!m_root) {
        m_root = std::make_unique<QuadNode>();
        m_root->bounds = envelope;
    }
    
    return InsertRecursive(m_root.get(), envelope, item, 0);
}

template<typename T>
GeomResult Quadtree<T>::InsertRecursive(QuadNode* node, const Envelope& envelope, const T& item, size_t depth) {
    if (depth >= m_config.maxDepth || node->items.size() < m_config.maxItemsPerNode) {
        node->items.emplace_back(envelope, item);
        m_size++;
        return GeomResult::kSuccess;
    }
    
    if (node->children[0] == nullptr) {
        SplitNode(node);
    }
    
    for (int i = 0; i < 4; ++i) {
        if (node->children[i]->bounds.Intersects(envelope)) {
            return InsertRecursive(node->children[i].get(), envelope, item, depth + 1);
        }
    }
    
    for (int i = 0; i < 4; ++i) {
        if (node->children[i]->bounds.Contains(envelope)) {
            return InsertRecursive(node->children[i].get(), envelope, item, depth + 1);
        }
    }
    
    double minDist = std::numeric_limits<double>::max();
    int bestChild = 0;
    for (int i = 0; i < 4; ++i) {
        double dist = std::abs((node->children[i]->bounds.GetMinX() + node->children[i]->bounds.GetMaxX()) / 2 - (envelope.GetMinX() + envelope.GetMaxX()) / 2) +
                      std::abs((node->children[i]->bounds.GetMinY() + node->children[i]->bounds.GetMaxY()) / 2 - (envelope.GetMinY() + envelope.GetMaxY()) / 2);
        if (dist < minDist) {
            minDist = dist;
            bestChild = i;
        }
    }
    
    return InsertRecursive(node->children[bestChild].get(), envelope, item, depth + 1);
}

template<typename T>
void Quadtree<T>::SplitNode(QuadNode* node) {
    double midX = (node->bounds.GetMinX() + node->bounds.GetMaxX()) / 2;
    double midY = (node->bounds.GetMinY() + node->bounds.GetMaxY()) / 2;
    
    for (int i = 0; i < 4; ++i) {
        node->children[i] = std::make_unique<QuadNode>();
    }
    
    node->children[0]->bounds = Envelope(node->bounds.GetMinX(), midY, midX, node->bounds.GetMaxY());
    node->children[1]->bounds = Envelope(midX, midY, node->bounds.GetMaxX(), node->bounds.GetMaxY());
    node->children[2]->bounds = Envelope(node->bounds.GetMinX(), node->bounds.GetMinY(), midX, midY);
    node->children[3]->bounds = Envelope(midX, node->bounds.GetMinY(), node->bounds.GetMaxX(), midY);
}

template<typename T>
GeomResult Quadtree<T>::BulkLoad(const std::vector<std::pair<Envelope, T>>& items) {
    for (const auto& item : items) {
        GeomResult result = Insert(item.first, item.second);
        if (result != GeomResult::kSuccess) {
            return result;
        }
    }
    return GeomResult::kSuccess;
}

template<typename T>
bool Quadtree<T>::Remove(const Envelope& envelope, const T& item) {
    return false;
}

template<typename T>
void Quadtree<T>::Clear() noexcept {
    m_root.reset();
    m_size = 0;
}

template<typename T>
std::vector<T> Quadtree<T>::Query(const Envelope& envelope) const {
    std::vector<T> results;
    if (!m_root) return results;
    
    QueryRecursive(m_root.get(), envelope, results);
    return results;
}

template<typename T>
void Quadtree<T>::QueryRecursive(const QuadNode* node, const Envelope& envelope, std::vector<T>& results) const {
    if (!node->bounds.Intersects(envelope)) return;
    
    for (const auto& item : node->items) {
        if (item.first.Intersects(envelope)) {
            results.push_back(item.second);
        }
    }
    
    for (int i = 0; i < 4; ++i) {
        if (node->children[i]) {
            QueryRecursive(node->children[i].get(), envelope, results);
        }
    }
}

template<typename T>
std::vector<T> Quadtree<T>::Query(const Coordinate& point) const {
    return Query(Envelope(point, point));
}

template<typename T>
std::vector<T> Quadtree<T>::QueryIntersects(const Geometry* geom) const {
    return Query(geom->GetEnvelope());
}

template<typename T>
std::vector<T> Quadtree<T>::QueryNearest(const Coordinate& point, size_t k) const {
    return std::vector<T>();
}

template<typename T>
size_t Quadtree<T>::Size() const noexcept {
    return m_size;
}

template<typename T>
bool Quadtree<T>::IsEmpty() const noexcept {
    return m_size == 0;
}

template<typename T>
Envelope Quadtree<T>::GetBounds() const {
    if (!m_root) return Envelope();
    return m_root->bounds;
}

template<typename T>
size_t Quadtree<T>::GetHeight() const {
    return 0;
}

template<typename T>
size_t Quadtree<T>::GetNodeCount() const {
    return 0;
}

template<typename T>
GeomResult Quadtree<T>::Serialize(std::ostream& output) const {
    return GeomResult::kNotImplemented;
}

template<typename T>
GeomResult Quadtree<T>::Deserialize(std::istream& input) {
    return GeomResult::kNotImplemented;
}

template<typename T>
size_t Quadtree<T>::GetSerializedSize() const noexcept {
    return 0;
}

template class Quadtree<Geometry*>;
template class Quadtree<size_t>;
template class Quadtree<int>;
template class Quadtree<std::string>;

template<typename T>
GridIndex<T>::GridIndex(const Config& config) : m_config(config) {
    m_grid.resize(m_config.gridX * m_config.gridY);
}

template<typename T>
GeomResult GridIndex<T>::Insert(const Envelope& envelope, const T& item) {
    if (envelope.IsNull() || m_config.bounds.IsNull()) {
        return GeomResult::kInvalidGeometry;
    }
    
    size_t minX = static_cast<size_t>(
        std::max(0.0, (envelope.GetMinX() - m_config.bounds.GetMinX()) / 
        (m_config.bounds.GetMaxX() - m_config.bounds.GetMinX()) * m_config.gridX));
    size_t maxX = static_cast<size_t>(
        std::min(static_cast<double>(m_config.gridX - 1),
        (envelope.GetMaxX() - m_config.bounds.GetMinX()) / 
        (m_config.bounds.GetMaxX() - m_config.bounds.GetMinX()) * m_config.gridX));
    size_t minY = static_cast<size_t>(
        std::max(0.0, (envelope.GetMinY() - m_config.bounds.GetMinY()) / 
        (m_config.bounds.GetMaxY() - m_config.bounds.GetMinY()) * m_config.gridY));
    size_t maxY = static_cast<size_t>(
        std::min(static_cast<double>(m_config.gridY - 1),
        (envelope.GetMaxY() - m_config.bounds.GetMinY()) / 
        (m_config.bounds.GetMaxY() - m_config.bounds.GetMinY()) * m_config.gridY));
    
    for (size_t y = minY; y <= maxY && y < m_config.gridY; ++y) {
        for (size_t x = minX; x <= maxX && x < m_config.gridX; ++x) {
            size_t index = y * m_config.gridX + x;
            m_grid[index].emplace_back(envelope, item);
        }
    }
    
    m_size++;
    return GeomResult::kSuccess;
}

template<typename T>
GeomResult GridIndex<T>::BulkLoad(const std::vector<std::pair<Envelope, T>>& items) {
    for (const auto& item : items) {
        GeomResult result = Insert(item.first, item.second);
        if (result != GeomResult::kSuccess) {
            return result;
        }
    }
    return GeomResult::kSuccess;
}

template<typename T>
bool GridIndex<T>::Remove(const Envelope& envelope, const T& item) {
    return false;
}

template<typename T>
void GridIndex<T>::Clear() noexcept {
    for (auto& cell : m_grid) {
        cell.clear();
    }
    m_size = 0;
}

template<typename T>
std::vector<T> GridIndex<T>::Query(const Envelope& envelope) const {
    std::vector<T> results;
    if (envelope.IsNull() || m_config.bounds.IsNull() || m_size == 0) {
        return results;
    }
    
    size_t minX = static_cast<size_t>(
        std::max(0.0, (envelope.GetMinX() - m_config.bounds.GetMinX()) / 
        (m_config.bounds.GetMaxX() - m_config.bounds.GetMinX()) * m_config.gridX));
    size_t maxX = static_cast<size_t>(
        std::min(static_cast<double>(m_config.gridX - 1),
        (envelope.GetMaxX() - m_config.bounds.GetMinX()) / 
        (m_config.bounds.GetMaxX() - m_config.bounds.GetMinX()) * m_config.gridX));
    size_t minY = static_cast<size_t>(
        std::max(0.0, (envelope.GetMinY() - m_config.bounds.GetMinY()) / 
        (m_config.bounds.GetMaxY() - m_config.bounds.GetMinY()) * m_config.gridY));
    size_t maxY = static_cast<size_t>(
        std::min(static_cast<double>(m_config.gridY - 1),
        (envelope.GetMaxY() - m_config.bounds.GetMinY()) / 
        (m_config.bounds.GetMaxY() - m_config.bounds.GetMinY()) * m_config.gridY));
    
    std::set<T> uniqueResults;
    for (size_t y = minY; y <= maxY && y < m_config.gridY; ++y) {
        for (size_t x = minX; x <= maxX && x < m_config.gridX; ++x) {
            size_t index = y * m_config.gridX + x;
            for (const auto& item : m_grid[index]) {
                if (item.first.Intersects(envelope)) {
                    uniqueResults.insert(item.second);
                }
            }
        }
    }
    
    results.assign(uniqueResults.begin(), uniqueResults.end());
    return results;
}

template<typename T>
std::vector<T> GridIndex<T>::Query(const Coordinate& point) const {
    return Query(Envelope(point, point));
}

template<typename T>
std::vector<T> GridIndex<T>::QueryIntersects(const Geometry* geom) const {
    return Query(geom->GetEnvelope());
}

template<typename T>
std::vector<T> GridIndex<T>::QueryNearest(const Coordinate& point, size_t k) const {
    return std::vector<T>();
}

template<typename T>
size_t GridIndex<T>::Size() const noexcept {
    return m_size;
}

template<typename T>
bool GridIndex<T>::IsEmpty() const noexcept {
    return m_size == 0;
}

template<typename T>
Envelope GridIndex<T>::GetBounds() const {
    return m_config.bounds;
}

template<typename T>
size_t GridIndex<T>::GetNodeCount() const {
    return m_grid.size();
}

template<typename T>
GeomResult GridIndex<T>::Serialize(std::ostream& output) const {
    return GeomResult::kNotImplemented;
}

template<typename T>
GeomResult GridIndex<T>::Deserialize(std::istream& input) {
    return GeomResult::kNotImplemented;
}

template<typename T>
size_t GridIndex<T>::GetSerializedSize() const noexcept {
    return 0;
}

template<typename T>
size_t GridIndex<T>::GetCellIndex(const Coordinate& coord) const {
    std::pair<size_t, size_t> indices = GetCellIndices(coord);
    return indices.second * m_config.gridX + indices.first;
}

template<typename T>
std::pair<size_t, size_t> GridIndex<T>::GetCellIndices(const Coordinate& coord) const {
    double cellWidth = m_config.bounds.GetWidth() / m_config.gridX;
    double cellHeight = m_config.bounds.GetHeight() / m_config.gridY;
    
    size_t ix = static_cast<size_t>((coord.x - m_config.bounds.GetMinX()) / cellWidth);
    size_t iy = static_cast<size_t>((coord.y - m_config.bounds.GetMinY()) / cellHeight);
    
    ix = std::min(ix, m_config.gridX - 1);
    iy = std::min(iy, m_config.gridY - 1);
    
    return std::make_pair(ix, iy);
}

template class GridIndex<Geometry*>;
template class GridIndex<size_t>;
template class GridIndex<int>;

}
