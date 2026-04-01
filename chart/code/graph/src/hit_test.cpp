#include "ogc/draw/hit_test.h"
#include "ogc/draw/layer_manager.h"
#include "ogc/point.h"
#include "ogc/linestring.h"
#include "ogc/polygon.h"
#include "ogc/envelope.h"
#include <algorithm>
#include <chrono>
#include <cmath>

namespace ogc {
namespace draw {

HitTestResultPtr HitTestResult::Create()
{
    return HitTestResultPtr(new HitTestResult());
}

void HitTestResult::AddItem(const HitTestResultItem& item)
{
    m_items.push_back(item);
}

void HitTestResult::AddItems(const std::vector<HitTestResultItem>& items)
{
    m_items.insert(m_items.end(), items.begin(), items.end());
}

void HitTestResult::Clear()
{
    m_items.clear();
}

const HitTestResultItem& HitTestResult::GetItem(size_t index) const
{
    return m_items.at(index);
}

HitTestResultItem& HitTestResult::GetItem(size_t index)
{
    return m_items.at(index);
}

void HitTestResult::SortByDistance()
{
    std::sort(m_items.begin(), m_items.end(), 
              [](const HitTestResultItem& a, const HitTestResultItem& b) {
                  return a.distance < b.distance;
              });
}

void HitTestResult::SortByLayerIndex()
{
    std::sort(m_items.begin(), m_items.end(), 
              [](const HitTestResultItem& a, const HitTestResultItem& b) {
                  return a.layerIndex < b.layerIndex;
              });
}

HitTestResultPtr HitTestResult::GetTopN(size_t n) const
{
    auto result = HitTestResult::Create();
    result->SetTestPoint(m_testPoint);
    result->SetTestEnvelope(m_testEnvelope);
    result->SetMode(m_mode);
    
    size_t count = std::min(n, m_items.size());
    for (size_t i = 0; i < count; ++i) {
        result->AddItem(m_items[i]);
    }
    return result;
}

HitTestResultPtr HitTestResult::GetByLayer(const std::string& layerName) const
{
    auto result = HitTestResult::Create();
    result->SetTestPoint(m_testPoint);
    result->SetTestEnvelope(m_testEnvelope);
    result->SetMode(m_mode);
    
    for (const auto& item : m_items) {
        if (item.layerName == layerName) {
            result->AddItem(item);
        }
    }
    return result;
}

HitTestResultPtr HitTestResult::GetByLayerIndex(int layerIndex) const
{
    auto result = HitTestResult::Create();
    result->SetTestPoint(m_testPoint);
    result->SetTestEnvelope(m_testEnvelope);
    result->SetMode(m_mode);
    
    for (const auto& item : m_items) {
        if (item.layerIndex == layerIndex) {
            result->AddItem(item);
        }
    }
    return result;
}

HitTestResultPtr HitTestResult::GetWithinDistance(double maxDistance) const
{
    auto result = HitTestResult::Create();
    result->SetTestPoint(m_testPoint);
    result->SetTestEnvelope(m_testEnvelope);
    result->SetMode(m_mode);
    
    for (const auto& item : m_items) {
        if (item.distance <= maxDistance) {
            result->AddItem(item);
        }
    }
    return result;
}

HitTesterPtr HitTester::Create()
{
    return HitTesterPtr(new HitTester());
}

HitTester::HitTester()
    : m_tolerance(5.0)
    , m_filter(HitTestFilter::kSelectable)
    , m_maxResults(100)
    , m_screenToWorld(nullptr)
    , m_worldToScreen(nullptr)
{
}

HitTestResultPtr HitTester::HitTest(LayerManager* layerManager,
                                     double screenX, double screenY,
                                     double tolerance)
{
    auto result = HitTestResult::Create();
    result->SetMode(HitTestMode::kPoint);
    
    if (!layerManager) {
        return result;
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    Coordinate worldPoint = ScreenToWorld(screenX, screenY);
    result->SetTestPoint(worldPoint);
    
    double worldTolerance = tolerance;
    if (m_screenToWorld) {
        Coordinate p1 = m_screenToWorld(screenX, screenY);
        Coordinate p2 = m_screenToWorld(screenX + tolerance, screenY);
        worldTolerance = std::abs(p2.x - p1.x);
    }
    
    int layerCount = layerManager->GetLayerCount();
    for (int i = layerCount - 1; i >= 0; --i) {
        auto layerItem = layerManager->GetLayer(i);
        if (!layerItem || !ShouldTestLayer(layerItem->GetLayer(), layerManager)) {
            continue;
        }
        
        auto layerResult = HitTestLayer(layerItem->GetLayer(), screenX, screenY, tolerance);
        if (layerResult && !layerResult->IsEmpty()) {
            for (size_t j = 0; j < layerResult->GetCount(); ++j) {
                auto item = layerResult->GetItem(j);
                item.layerIndex = i;
                item.layerName = layerItem->GetConfig().GetName();
                result->AddItem(item);
            }
        }
        
        if (result->GetCount() >= m_maxResults) {
            break;
        }
    }
    
    result->SortByDistance();
    
    auto end = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double, std::milli>(end - start).count();
    result->SetElapsedTime(elapsed);
    
    return result;
}

HitTestResultPtr HitTester::HitTest(LayerManager* layerManager,
                                     const Envelope& screenRect,
                                     HitTestMode mode)
{
    auto result = HitTestResult::Create();
    result->SetMode(mode);
    result->SetTestEnvelope(screenRect);
    
    if (!layerManager) {
        return result;
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    int layerCount = layerManager->GetLayerCount();
    for (int i = layerCount - 1; i >= 0; --i) {
        auto layerItem = layerManager->GetLayer(i);
        if (!layerItem || !ShouldTestLayer(layerItem->GetLayer(), layerManager)) {
            continue;
        }
        
        auto layerResult = HitTestLayer(layerItem->GetLayer(), screenRect, mode);
        if (layerResult && !layerResult->IsEmpty()) {
            for (size_t j = 0; j < layerResult->GetCount(); ++j) {
                auto item = layerResult->GetItem(j);
                item.layerIndex = i;
                item.layerName = layerItem->GetConfig().GetName();
                result->AddItem(item);
            }
        }
        
        if (result->GetCount() >= m_maxResults) {
            break;
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double, std::milli>(end - start).count();
    result->SetElapsedTime(elapsed);
    
    return result;
}

HitTestResultPtr HitTester::HitTest(LayerManager* layerManager,
                                     const Geometry* geometry,
                                     HitTestMode mode)
{
    auto result = HitTestResult::Create();
    result->SetMode(mode);
    
    if (!layerManager || !geometry) {
        return result;
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    Envelope env = geometry->GetEnvelope();
    result->SetTestEnvelope(env);
    
    int layerCount = layerManager->GetLayerCount();
    for (int i = layerCount - 1; i >= 0; --i) {
        auto layerItem = layerManager->GetLayer(i);
        if (!layerItem || !ShouldTestLayer(layerItem->GetLayer(), layerManager)) {
            continue;
        }
        
        if (result->GetCount() >= m_maxResults) {
            break;
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double, std::milli>(end - start).count();
    result->SetElapsedTime(elapsed);
    
    return result;
}

HitTestResultPtr HitTester::HitTestLayer(CNLayer* layer,
                                          double screenX, double screenY,
                                          double tolerance)
{
    auto result = HitTestResult::Create();
    result->SetMode(HitTestMode::kPoint);
    
    if (!layer) {
        return result;
    }
    
    Coordinate worldPoint = ScreenToWorld(screenX, screenY);
    result->SetTestPoint(worldPoint);
    
    double worldTolerance = tolerance;
    if (m_screenToWorld) {
        Coordinate p1 = m_screenToWorld(screenX, screenY);
        Coordinate p2 = m_screenToWorld(screenX + tolerance, screenY);
        worldTolerance = std::abs(p2.x - p1.x);
    }
    
    return result;
}

HitTestResultPtr HitTester::HitTestLayer(CNLayer* layer,
                                          const Envelope& screenRect,
                                          HitTestMode mode)
{
    auto result = HitTestResult::Create();
    result->SetMode(mode);
    result->SetTestEnvelope(screenRect);
    
    if (!layer) {
        return result;
    }
    
    return result;
}

void HitTester::SetScreenToWorldTransform(std::function<Coordinate(double, double)> transform)
{
    m_screenToWorld = transform;
}

void HitTester::SetWorldToScreenTransform(std::function<Coordinate(double, double)> transform)
{
    m_worldToScreen = transform;
}

Coordinate HitTester::ScreenToWorld(double screenX, double screenY) const
{
    if (m_screenToWorld) {
        return m_screenToWorld(screenX, screenY);
    }
    return Coordinate(screenX, screenY);
}

Coordinate HitTester::WorldToScreen(double worldX, double worldY) const
{
    if (m_worldToScreen) {
        return m_worldToScreen(worldX, worldY);
    }
    return Coordinate(worldX, worldY);
}

double HitTester::CalculateDistance(const Coordinate& p1, const Coordinate& p2)
{
    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;
    return std::sqrt(dx * dx + dy * dy);
}

double HitTester::CalculateDistanceToGeometry(const Coordinate& point, const Geometry* geometry)
{
    if (!geometry) {
        return std::numeric_limits<double>::max();
    }
    
    auto geomType = geometry->GetGeometryType();
    
    switch (geomType) {
        case GeomType::kPoint: {
            auto pt = dynamic_cast<const ogc::Point*>(geometry);
            if (pt) {
                return CalculateDistance(point, Coordinate(pt->GetX(), pt->GetY()));
            }
            break;
        }
        case GeomType::kLineString: {
            auto line = dynamic_cast<const LineString*>(geometry);
            if (line) {
                double minDist = std::numeric_limits<double>::max();
                for (size_t i = 0; i < line->GetNumPoints(); ++i) {
                    Coordinate pt = line->GetPointN(i);
                    double dist = CalculateDistance(point, pt);
                    minDist = std::min(minDist, dist);
                }
                return minDist;
            }
            break;
        }
        case GeomType::kPolygon: {
            auto poly = dynamic_cast<const Polygon*>(geometry);
            if (poly) {
                if (PointInGeometry(point, geometry, 0)) {
                    return 0.0;
                }
                auto exterior = poly->GetExteriorRing();
                if (exterior) {
                    double minDist = std::numeric_limits<double>::max();
                    for (size_t i = 0; i < exterior->GetNumPoints(); ++i) {
                        Coordinate pt = exterior->GetPointN(i);
                        double dist = CalculateDistance(point, pt);
                        minDist = std::min(minDist, dist);
                    }
                    return minDist;
                }
            }
            break;
        }
        default:
            break;
    }
    
    return std::numeric_limits<double>::max();
}

bool HitTester::PointInGeometry(const Coordinate& point, const Geometry* geometry, double tolerance)
{
    if (!geometry) {
        return false;
    }
    
    auto geomType = geometry->GetGeometryType();
    
    switch (geomType) {
        case GeomType::kPoint: {
            auto pt = dynamic_cast<const ogc::Point*>(geometry);
            if (pt) {
                double dist = CalculateDistance(point, Coordinate(pt->GetX(), pt->GetY()));
                return dist <= tolerance;
            }
            break;
        }
        case GeomType::kLineString: {
            auto line = dynamic_cast<const LineString*>(geometry);
            if (line) {
                for (size_t i = 0; i < line->GetNumPoints() - 1; ++i) {
                    Coordinate p1 = line->GetPointN(i);
                    Coordinate p2 = line->GetPointN(i + 1);
                    
                    double x1 = p1.x, y1 = p1.y;
                    double x2 = p2.x, y2 = p2.y;
                    double px = point.x, py = point.y;
                    
                    double dx = x2 - x1;
                    double dy = y2 - y1;
                    double len2 = dx * dx + dy * dy;
                    
                    if (len2 > 0) {
                        double t = std::max(0.0, std::min(1.0, ((px - x1) * dx + (py - y1) * dy) / len2));
                        double projX = x1 + t * dx;
                        double projY = y1 + t * dy;
                        double dist = CalculateDistance(point, Coordinate(projX, projY));
                        if (dist <= tolerance) {
                            return true;
                        }
                    }
                }
            }
            break;
        }
        case GeomType::kPolygon: {
            auto poly = dynamic_cast<const Polygon*>(geometry);
            if (poly) {
                auto exterior = poly->GetExteriorRing();
                if (exterior) {
                    bool inside = false;
                    size_t n = exterior->GetNumPoints();
                    for (size_t i = 0, j = n - 1; i < n; j = i++) {
                        Coordinate pi = exterior->GetPointN(i);
                        Coordinate pj = exterior->GetPointN(j);
                        if (((pi.y > point.y) != (pj.y > point.y)) &&
                            (point.x < (pj.x - pi.x) * (point.y - pi.y) / (pj.y - pi.y) + pi.x)) {
                            inside = !inside;
                        }
                    }
                    return inside;
                }
            }
            break;
        }
        default:
            break;
    }
    
    return false;
}

bool HitTester::GeometryIntersects(const Geometry* geom1, const Geometry* geom2)
{
    if (!geom1 || !geom2) {
        return false;
    }
    
    Envelope env1 = geom1->GetEnvelope();
    Envelope env2 = geom2->GetEnvelope();
    
    return env1.Intersects(env2);
}

bool HitTester::GeometryIntersectsEnvelope(const Geometry* geom, const Envelope& env)
{
    if (!geom) {
        return false;
    }
    
    Envelope geomEnv = geom->GetEnvelope();
    return geomEnv.Intersects(env);
}

bool HitTester::ShouldTestLayer(CNLayer* layer, const LayerManager* manager) const
{
    if (!layer) {
        return false;
    }
    
    switch (m_filter) {
        case HitTestFilter::kAll:
            return true;
        case HitTestFilter::kSelectable:
            return true;
        case HitTestFilter::kVisible:
            return true;
        case HitTestFilter::kEditable:
            return true;
        default:
            return true;
    }
}

SelectionManager& SelectionManager::Instance()
{
    static SelectionManager instance;
    return instance;
}

SelectionManager::SelectionManager()
    : m_initialized(false)
    , m_highlightColor(0xFFFFFF00)
    , m_selectionColor(0xFF00FF00)
{
}

SelectionManager::~SelectionManager()
{
    Finalize();
}

bool SelectionManager::Initialize()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_initialized) {
        return true;
    }
    
    m_selectedIds.clear();
    m_layerSelections.clear();
    m_initialized = true;
    return true;
}

void SelectionManager::Finalize()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_initialized) {
        return;
    }
    
    m_selectedIds.clear();
    m_layerSelections.clear();
    m_initialized = false;
}

void SelectionManager::Select(int64_t featureId, const std::string& layerName)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = std::find(m_selectedIds.begin(), m_selectedIds.end(), featureId);
    if (it == m_selectedIds.end()) {
        m_selectedIds.push_back(featureId);
    }
    
    if (!layerName.empty()) {
        auto& layerIds = m_layerSelections[layerName];
        auto layerIt = std::find(layerIds.begin(), layerIds.end(), featureId);
        if (layerIt == layerIds.end()) {
            layerIds.push_back(featureId);
        }
    }
    
    if (m_selectionChangedCallback) {
        m_selectionChangedCallback(m_selectedIds);
    }
}

void SelectionManager::Select(const std::vector<int64_t>& featureIds, const std::string& layerName)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    for (int64_t id : featureIds) {
        auto it = std::find(m_selectedIds.begin(), m_selectedIds.end(), id);
        if (it == m_selectedIds.end()) {
            m_selectedIds.push_back(id);
        }
    }
    
    if (!layerName.empty()) {
        auto& layerIds = m_layerSelections[layerName];
        for (int64_t id : featureIds) {
            auto layerIt = std::find(layerIds.begin(), layerIds.end(), id);
            if (layerIt == layerIds.end()) {
                layerIds.push_back(id);
            }
        }
    }
    
    if (m_selectionChangedCallback) {
        m_selectionChangedCallback(m_selectedIds);
    }
}

void SelectionManager::SelectByResult(HitTestResultPtr result)
{
    if (!result) {
        return;
    }
    
    std::vector<int64_t> ids;
    for (size_t i = 0; i < result->GetCount(); ++i) {
        ids.push_back(result->GetItem(i).featureId);
    }
    
    if (!ids.empty()) {
        Select(ids, result->GetItem(0).layerName);
    }
}

void SelectionManager::Deselect(int64_t featureId)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = std::find(m_selectedIds.begin(), m_selectedIds.end(), featureId);
    if (it != m_selectedIds.end()) {
        m_selectedIds.erase(it);
    }
    
    for (auto& pair : m_layerSelections) {
        auto& layerIds = pair.second;
        auto layerIt = std::find(layerIds.begin(), layerIds.end(), featureId);
        if (layerIt != layerIds.end()) {
            layerIds.erase(layerIt);
        }
    }
    
    if (m_selectionChangedCallback) {
        m_selectionChangedCallback(m_selectedIds);
    }
}

void SelectionManager::Deselect(const std::vector<int64_t>& featureIds)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    for (int64_t id : featureIds) {
        auto it = std::find(m_selectedIds.begin(), m_selectedIds.end(), id);
        if (it != m_selectedIds.end()) {
            m_selectedIds.erase(it);
        }
    }
    
    for (auto& pair : m_layerSelections) {
        auto& layerIds = pair.second;
        for (int64_t id : featureIds) {
            auto layerIt = std::find(layerIds.begin(), layerIds.end(), id);
            if (layerIt != layerIds.end()) {
                layerIds.erase(layerIt);
            }
        }
    }
    
    if (m_selectionChangedCallback) {
        m_selectionChangedCallback(m_selectedIds);
    }
}

void SelectionManager::DeselectAll()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_selectedIds.clear();
    m_layerSelections.clear();
    
    if (m_selectionChangedCallback) {
        m_selectionChangedCallback(m_selectedIds);
    }
}

bool SelectionManager::IsSelected(int64_t featureId) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return std::find(m_selectedIds.begin(), m_selectedIds.end(), featureId) != m_selectedIds.end();
}

std::vector<int64_t> SelectionManager::GetSelectedIds() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_selectedIds;
}

std::vector<int64_t> SelectionManager::GetSelectedIds(const std::string& layerName) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_layerSelections.find(layerName);
    if (it != m_layerSelections.end()) {
        return it->second;
    }
    return {};
}

void SelectionManager::SetCurrentLayer(const std::string& layerName)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_currentLayer = layerName;
}

void SelectionManager::SetSelectionChangedCallback(SelectionChangedCallback callback)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_selectionChangedCallback = callback;
}

void SelectionManager::SetHighlightColor(uint32_t color)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_highlightColor = color;
}

void SelectionManager::SetSelectionColor(uint32_t color)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_selectionColor = color;
}

}
}
