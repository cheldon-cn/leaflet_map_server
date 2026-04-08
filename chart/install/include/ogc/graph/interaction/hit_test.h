#ifndef OGC_GRAPH_HIT_TEST_H
#define OGC_GRAPH_HIT_TEST_H

#include "ogc/graph/export.h"
#include <ogc/draw/draw_result.h>
#include "ogc/coordinate.h"
#include "ogc/envelope.h"
#include "ogc/geometry.h"
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <map>
#include <mutex>

namespace ogc {

class CNLayer;
class CNFeature;

namespace graph {

class LayerManager;

namespace draw {
class DrawContext;
}

enum class HitTestMode {
    kPoint = 0,
    kBox,
    kPolygon,
    kRadius
};

enum class HitTestFilter {
    kAll = 0,
    kSelectable,
    kVisible,
    kEditable
};

struct HitTestResultItem {
    int64_t featureId = 0;
    std::string featureName;
    std::string layerName;
    int layerIndex = -1;
    double distance = 0.0;
    Coordinate hitPoint;
    bool isSelected = false;
    bool isHighlighted = false;
};

class HitTestResult;
using HitTestResultPtr = std::shared_ptr<HitTestResult>;

class OGC_GRAPH_API HitTestResult {
public:
    static HitTestResultPtr Create();
    
    void AddItem(const HitTestResultItem& item);
    void AddItems(const std::vector<HitTestResultItem>& items);
    void Clear();
    
    size_t GetCount() const { return m_items.size(); }
    bool IsEmpty() const { return m_items.empty(); }
    
    const HitTestResultItem& GetItem(size_t index) const;
    HitTestResultItem& GetItem(size_t index);
    
    const std::vector<HitTestResultItem>& GetItems() const { return m_items; }
    std::vector<HitTestResultItem>& GetItems() { return m_items; }
    
    void SortByDistance();
    void SortByLayerIndex();
    
    HitTestResultPtr GetTopN(size_t n) const;
    HitTestResultPtr GetByLayer(const std::string& layerName) const;
    HitTestResultPtr GetByLayerIndex(int layerIndex) const;
    HitTestResultPtr GetWithinDistance(double maxDistance) const;
    
    void SetTestPoint(const Coordinate& point) { m_testPoint = point; }
    Coordinate GetTestPoint() const { return m_testPoint; }
    
    void SetTestEnvelope(const Envelope& env) { m_testEnvelope = env; }
    Envelope GetTestEnvelope() const { return m_testEnvelope; }
    
    void SetMode(HitTestMode mode) { m_mode = mode; }
    HitTestMode GetMode() const { return m_mode; }
    
    void SetElapsedTime(double ms) { m_elapsedMs = ms; }
    double GetElapsedTime() const { return m_elapsedMs; }
    
private:
    HitTestResult() = default;
    
    std::vector<HitTestResultItem> m_items;
    Coordinate m_testPoint;
    Envelope m_testEnvelope;
    HitTestMode m_mode = HitTestMode::kPoint;
    double m_elapsedMs = 0.0;
};

class HitTester;
using HitTesterPtr = std::shared_ptr<HitTester>;

class OGC_GRAPH_API HitTester {
public:
    static HitTesterPtr Create();
    
    HitTestResultPtr HitTest(LayerManager* layerManager,
                              double screenX, double screenY,
                              double tolerance = 5.0);
    
    HitTestResultPtr HitTest(LayerManager* layerManager,
                              const Envelope& screenRect,
                              HitTestMode mode = HitTestMode::kBox);
    
    HitTestResultPtr HitTest(LayerManager* layerManager,
                              const Geometry* geometry,
                              HitTestMode mode = HitTestMode::kPolygon);
    
    HitTestResultPtr HitTestLayer(CNLayer* layer,
                                   double screenX, double screenY,
                                   double tolerance = 5.0);
    
    HitTestResultPtr HitTestLayer(CNLayer* layer,
                                   const Envelope& screenRect,
                                   HitTestMode mode = HitTestMode::kBox);
    
    void SetTolerance(double tolerance) { m_tolerance = tolerance; }
    double GetTolerance() const { return m_tolerance; }
    
    void SetFilter(HitTestFilter filter) { m_filter = filter; }
    HitTestFilter GetFilter() const { return m_filter; }
    
    void SetMaxResults(size_t maxResults) { m_maxResults = maxResults; }
    size_t GetMaxResults() const { return m_maxResults; }
    
    void SetScreenToWorldTransform(std::function<Coordinate(double, double)> transform);
    void SetWorldToScreenTransform(std::function<Coordinate(double, double)> transform);
    
    Coordinate ScreenToWorld(double screenX, double screenY) const;
    Coordinate WorldToScreen(double worldX, double worldY) const;
    
    static double CalculateDistance(const Coordinate& p1, const Coordinate& p2);
    static double CalculateDistanceToGeometry(const Coordinate& point, const Geometry* geometry);
    static bool PointInGeometry(const Coordinate& point, const Geometry* geometry, double tolerance);
    static bool GeometryIntersects(const Geometry* geom1, const Geometry* geom2);
    static bool GeometryIntersectsEnvelope(const Geometry* geom, const Envelope& env);
    
private:
    HitTester();
    
    bool ShouldTestLayer(CNLayer* layer, const LayerManager* manager) const;
    void TestFeature(CNFeature* feature, const std::string& layerName, int layerIndex,
                     const Coordinate& worldPoint, double tolerance,
                     HitTestResultPtr result);
    
    double m_tolerance;
    HitTestFilter m_filter;
    size_t m_maxResults;
    std::function<Coordinate(double, double)> m_screenToWorld;
    std::function<Coordinate(double, double)> m_worldToScreen;
};

class SelectionManager;
using SelectionManagerPtr = std::shared_ptr<SelectionManager>;

class OGC_GRAPH_API SelectionManager {
public:
    using SelectionChangedCallback = std::function<void(const std::vector<int64_t>&)>;
    
    static SelectionManager& Instance();
    
    bool Initialize();
    void Finalize();
    bool IsInitialized() const { return m_initialized; }
    
    void Select(int64_t featureId, const std::string& layerName);
    void Select(const std::vector<int64_t>& featureIds, const std::string& layerName);
    void SelectByResult(HitTestResultPtr result);
    
    void Deselect(int64_t featureId);
    void Deselect(const std::vector<int64_t>& featureIds);
    void DeselectAll();
    
    bool IsSelected(int64_t featureId) const;
    std::vector<int64_t> GetSelectedIds() const;
    std::vector<int64_t> GetSelectedIds(const std::string& layerName) const;
    
    void SetCurrentLayer(const std::string& layerName);
    std::string GetCurrentLayer() const { return m_currentLayer; }
    
    void SetSelectionChangedCallback(SelectionChangedCallback callback);
    
    void SetHighlightColor(uint32_t color);
    uint32_t GetHighlightColor() const { return m_highlightColor; }
    
    void SetSelectionColor(uint32_t color);
    uint32_t GetSelectionColor() const { return m_selectionColor; }
    
private:
    SelectionManager();
    ~SelectionManager();
    
    SelectionManager(const SelectionManager&) = delete;
    SelectionManager& operator=(const SelectionManager&) = delete;
    
    bool m_initialized;
    std::vector<int64_t> m_selectedIds;
    std::map<std::string, std::vector<int64_t>> m_layerSelections;
    std::string m_currentLayer;
    SelectionChangedCallback m_selectionChangedCallback;
    uint32_t m_highlightColor;
    uint32_t m_selectionColor;
    mutable std::mutex m_mutex;
};

}
}

#endif
