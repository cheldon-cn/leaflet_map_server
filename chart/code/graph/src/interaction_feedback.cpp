#include "ogc/draw/interaction_feedback.h"
#include "ogc/draw/draw_context.h"
#include "ogc/draw/point_symbolizer.h"
#include "ogc/draw/line_symbolizer.h"
#include "ogc/draw/polygon_symbolizer.h"
#include "ogc/draw/text_symbolizer.h"
#include "ogc/point.h"
#include "ogc/linestring.h"
#include "ogc/polygon.h"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cmath>

namespace ogc {
namespace draw {

FeedbackItemPtr FeedbackItem::Create(FeedbackType type)
{
    return FeedbackItemPtr(new FeedbackItem(type));
}

FeedbackItemPtr FeedbackItem::Create(FeedbackType type, const Geometry* geometry)
{
    auto item = FeedbackItemPtr(new FeedbackItem(type));
    if (geometry) {
        item->SetGeometry(geometry);
    }
    return item;
}

FeedbackItemPtr FeedbackItem::Create(FeedbackType type, const Envelope& envelope)
{
    auto item = FeedbackItemPtr(new FeedbackItem(type));
    item->SetEnvelope(envelope);
    return item;
}

FeedbackItemPtr FeedbackItem::Create(FeedbackType type, const Coordinate& point)
{
    auto item = FeedbackItemPtr(new FeedbackItem(type));
    item->SetPoint(point);
    return item;
}

FeedbackItem::FeedbackItem(FeedbackType type)
    : m_id(0)
    , m_type(type)
    , m_userData(nullptr)
{
    m_config.type = type;
}

void FeedbackItem::SetGeometry(const Geometry* geometry)
{
    if (geometry) {
        m_geometry = geometry->Clone();
    } else {
        m_geometry.reset();
    }
}

void FeedbackItem::SetEnvelope(const Envelope& envelope)
{
    m_envelope = envelope;
}

void FeedbackItem::SetPoint(const Coordinate& point)
{
    m_point = point;
}

void FeedbackItem::SetPoints(const std::vector<Coordinate>& points)
{
    m_points = points;
}

DrawResult FeedbackItem::Render(DrawContextPtr context)
{
    if (!context || !m_config.visible) {
        return DrawResult::kInvalidParameter;
    }
    
    DrawStyle style;
    style.pen.color = Color(m_config.strokeColor);
    style.pen.width = m_config.strokeWidth;
    style.brush.color = Color(m_config.fillColor);
    style.opacity = m_config.opacity;
    
    if (m_geometry) {
        auto geomType = m_geometry->GetGeometryType();
        
        switch (geomType) {
            case GeomType::kPoint:
            case GeomType::kMultiPoint: {
                auto symbolizer = PointSymbolizer::Create();
                symbolizer->SetColor(m_config.fillColor);
                symbolizer->SetSize(m_config.strokeWidth * 2);
                return DrawResult::kSuccess;
            }
            case GeomType::kLineString:
            case GeomType::kMultiLineString: {
                auto symbolizer = LineSymbolizer::Create();
                symbolizer->SetColor(m_config.strokeColor);
                symbolizer->SetWidth(m_config.strokeWidth);
                return DrawResult::kSuccess;
            }
            case GeomType::kPolygon:
            case GeomType::kMultiPolygon: {
                auto symbolizer = PolygonSymbolizer::Create();
                symbolizer->SetFillColor(m_config.fillColor);
                symbolizer->SetStrokeColor(m_config.strokeColor);
                symbolizer->SetStrokeWidth(m_config.strokeWidth);
                return DrawResult::kSuccess;
            }
            default:
                break;
        }
    }
    
    if (!m_points.empty()) {
        if (m_points.size() == 1) {
            auto symbolizer = PointSymbolizer::Create();
            symbolizer->SetColor(m_config.fillColor);
            symbolizer->SetSize(m_config.strokeWidth * 2);
            return DrawResult::kSuccess;
        } else {
            auto symbolizer = LineSymbolizer::Create();
            symbolizer->SetColor(m_config.strokeColor);
            symbolizer->SetWidth(m_config.strokeWidth);
            return DrawResult::kSuccess;
        }
    }
    
    if (!m_envelope.IsNull()) {
        auto symbolizer = PolygonSymbolizer::Create();
        symbolizer->SetFillColor(m_config.fillColor);
        symbolizer->SetStrokeColor(m_config.strokeColor);
        symbolizer->SetStrokeWidth(m_config.strokeWidth);
        return DrawResult::kSuccess;
    }
    
    return DrawResult::kSuccess;
}

FeedbackManager& FeedbackManager::Instance()
{
    static FeedbackManager instance;
    return instance;
}

FeedbackManager::FeedbackManager()
    : m_initialized(false)
    , m_animationEnabled(true)
    , m_nextId(1)
{
    FeedbackConfig selectionConfig;
    selectionConfig.type = FeedbackType::kSelection;
    selectionConfig.fillColor = 0x3000FF00;
    selectionConfig.strokeColor = 0xFF00FF00;
    selectionConfig.strokeWidth = 2.0;
    selectionConfig.opacity = 0.3;
    m_defaultConfigs[FeedbackType::kSelection] = selectionConfig;
    
    FeedbackConfig highlightConfig;
    highlightConfig.type = FeedbackType::kHighlight;
    highlightConfig.fillColor = 0x30FFFF00;
    highlightConfig.strokeColor = 0xFFFFFF00;
    highlightConfig.strokeWidth = 2.0;
    highlightConfig.opacity = 0.5;
    m_defaultConfigs[FeedbackType::kHighlight] = highlightConfig;
    
    FeedbackConfig hoverConfig;
    hoverConfig.type = FeedbackType::kHover;
    hoverConfig.fillColor = 0x200000FF;
    hoverConfig.strokeColor = 0xFF0000FF;
    hoverConfig.strokeWidth = 1.0;
    hoverConfig.opacity = 0.2;
    m_defaultConfigs[FeedbackType::kHover] = hoverConfig;
    
    FeedbackConfig measureConfig;
    measureConfig.type = FeedbackType::kMeasure;
    measureConfig.fillColor = 0x00FFFFFF;
    measureConfig.strokeColor = 0xFFFF0000;
    measureConfig.strokeWidth = 2.0;
    measureConfig.opacity = 1.0;
    m_defaultConfigs[FeedbackType::kMeasure] = measureConfig;
    
    FeedbackConfig rubberBandConfig;
    rubberBandConfig.type = FeedbackType::kRubberBand;
    rubberBandConfig.fillColor = 0x100000FF;
    rubberBandConfig.strokeColor = 0xFF0000FF;
    rubberBandConfig.strokeWidth = 1.0;
    rubberBandConfig.opacity = 0.1;
    m_defaultConfigs[FeedbackType::kRubberBand] = rubberBandConfig;
    
    FeedbackConfig crosshairConfig;
    crosshairConfig.type = FeedbackType::kCrosshair;
    crosshairConfig.strokeColor = 0xFF000000;
    crosshairConfig.strokeWidth = 1.0;
    crosshairConfig.opacity = 1.0;
    m_defaultConfigs[FeedbackType::kCrosshair] = crosshairConfig;
    
    FeedbackConfig tooltipConfig;
    tooltipConfig.type = FeedbackType::kTooltip;
    tooltipConfig.fillColor = 0xFFFFFFFF;
    tooltipConfig.strokeColor = 0xFF000000;
    tooltipConfig.strokeWidth = 1.0;
    tooltipConfig.opacity = 0.9;
    tooltipConfig.fontSize = 12.0;
    m_defaultConfigs[FeedbackType::kTooltip] = tooltipConfig;
}

FeedbackManager::~FeedbackManager()
{
    Finalize();
}

bool FeedbackManager::Initialize()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_initialized) {
        return true;
    }
    
    m_feedbackItems.clear();
    m_nextId = 1;
    m_initialized = true;
    return true;
}

void FeedbackManager::Finalize()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_initialized) {
        return;
    }
    
    m_feedbackItems.clear();
    m_screenToWorld = nullptr;
    m_worldToScreen = nullptr;
    m_initialized = false;
}

int FeedbackManager::AddFeedback(FeedbackItemPtr item)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!item) {
        return 0;
    }
    
    int id = m_nextId++;
    item->SetId(id);
    m_feedbackItems.push_back(item);
    
    if (m_feedbackChangedCallback) {
        m_feedbackChangedCallback();
    }
    
    return id;
}

void FeedbackManager::RemoveFeedback(int id)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = std::find_if(m_feedbackItems.begin(), m_feedbackItems.end(),
                           [id](const FeedbackItemPtr& item) {
                               return item && item->GetId() == id;
                           });
    
    if (it != m_feedbackItems.end()) {
        m_feedbackItems.erase(it);
        
        if (m_feedbackChangedCallback) {
            m_feedbackChangedCallback();
        }
    }
}

void FeedbackManager::RemoveFeedbackByTag(const std::string& tag)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = std::remove_if(m_feedbackItems.begin(), m_feedbackItems.end(),
                             [&tag](const FeedbackItemPtr& item) {
                                 return item && item->GetTag() == tag;
                             });
    
    m_feedbackItems.erase(it, m_feedbackItems.end());
    
    if (m_feedbackChangedCallback) {
        m_feedbackChangedCallback();
    }
}

void FeedbackManager::RemoveFeedbackByType(FeedbackType type)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = std::remove_if(m_feedbackItems.begin(), m_feedbackItems.end(),
                             [type](const FeedbackItemPtr& item) {
                                 return item && item->GetConfig().type == type;
                             });
    
    m_feedbackItems.erase(it, m_feedbackItems.end());
    
    if (m_feedbackChangedCallback) {
        m_feedbackChangedCallback();
    }
}

void FeedbackManager::RemoveAllFeedback()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_feedbackItems.clear();
    
    if (m_feedbackChangedCallback) {
        m_feedbackChangedCallback();
    }
}

FeedbackItemPtr FeedbackManager::GetFeedback(int id) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = std::find_if(m_feedbackItems.begin(), m_feedbackItems.end(),
                           [id](const FeedbackItemPtr& item) {
                               return item && item->GetId() == id;
                           });
    
    return it != m_feedbackItems.end() ? *it : nullptr;
}

std::vector<FeedbackItemPtr> FeedbackManager::GetFeedbackByType(FeedbackType type) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<FeedbackItemPtr> result;
    for (const auto& item : m_feedbackItems) {
        if (item && item->GetConfig().type == type) {
            result.push_back(item);
        }
    }
    return result;
}

std::vector<FeedbackItemPtr> FeedbackManager::GetFeedbackByTag(const std::string& tag) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<FeedbackItemPtr> result;
    for (const auto& item : m_feedbackItems) {
        if (item && item->GetTag() == tag) {
            result.push_back(item);
        }
    }
    return result;
}

std::vector<FeedbackItemPtr> FeedbackManager::GetAllFeedback() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_feedbackItems;
}

void FeedbackManager::SetFeedbackVisible(int id, bool visible)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = std::find_if(m_feedbackItems.begin(), m_feedbackItems.end(),
                           [id](const FeedbackItemPtr& item) {
                               return item && item->GetId() == id;
                           });
    
    if (it != m_feedbackItems.end()) {
        (*it)->SetVisible(visible);
        
        if (m_feedbackChangedCallback) {
            m_feedbackChangedCallback();
        }
    }
}

void FeedbackManager::SetFeedbackVisibleByType(FeedbackType type, bool visible)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    bool changed = false;
    for (auto& item : m_feedbackItems) {
        if (item && item->GetConfig().type == type) {
            item->SetVisible(visible);
            changed = true;
        }
    }
    
    if (changed && m_feedbackChangedCallback) {
        m_feedbackChangedCallback();
    }
}

size_t FeedbackManager::GetFeedbackCount() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_feedbackItems.size();
}

DrawResult FeedbackManager::Render(DrawContextPtr context)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!context) {
        return DrawResult::kInvalidParameter;
    }
    
    for (const auto& item : m_feedbackItems) {
        if (item && item->IsVisible()) {
            item->Render(context);
        }
    }
    
    return DrawResult::kSuccess;
}

DrawResult FeedbackManager::RenderByType(DrawContextPtr context, FeedbackType type)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!context) {
        return DrawResult::kInvalidParameter;
    }
    
    for (const auto& item : m_feedbackItems) {
        if (item && item->IsVisible() && item->GetConfig().type == type) {
            item->Render(context);
        }
    }
    
    return DrawResult::kSuccess;
}

int FeedbackManager::ShowSelectionFeedback(const std::vector<int64_t>& featureIds, 
                                            const std::string& layerName)
{
    RemoveFeedbackByType(FeedbackType::kSelection);
    
    auto config = m_defaultConfigs[FeedbackType::kSelection];
    auto item = FeedbackItem::Create(FeedbackType::kSelection);
    item->SetConfig(config);
    item->SetTag("selection_" + layerName);
    
    return AddFeedback(item);
}

int FeedbackManager::ShowHighlightFeedback(int64_t featureId, const std::string& layerName)
{
    RemoveFeedbackByType(FeedbackType::kHighlight);
    
    auto config = m_defaultConfigs[FeedbackType::kHighlight];
    auto item = FeedbackItem::Create(FeedbackType::kHighlight);
    item->SetConfig(config);
    item->SetTag("highlight_" + layerName + "_" + std::to_string(featureId));
    
    return AddFeedback(item);
}

int FeedbackManager::ShowHoverFeedback(double screenX, double screenY, const std::string& text)
{
    RemoveFeedbackByType(FeedbackType::kHover);
    
    auto config = m_defaultConfigs[FeedbackType::kHover];
    auto item = FeedbackItem::Create(FeedbackType::kHover);
    item->SetConfig(config);
    item->SetTag("hover");
    
    Coordinate worldPoint(screenX, screenY);
    if (m_screenToWorld) {
        worldPoint = m_screenToWorld(screenX, screenY);
    }
    item->SetPoint(worldPoint);
    
    return AddFeedback(item);
}

int FeedbackManager::ShowMeasureFeedback(const std::vector<Coordinate>& points, 
                                          double distance, double area)
{
    RemoveFeedbackByType(FeedbackType::kMeasure);
    
    auto config = m_defaultConfigs[FeedbackType::kMeasure];
    auto item = FeedbackItem::Create(FeedbackType::kMeasure);
    item->SetConfig(config);
    item->SetTag("measure");
    item->SetPoints(points);
    
    return AddFeedback(item);
}

int FeedbackManager::ShowRubberBandFeedback(const Envelope& rect)
{
    RemoveFeedbackByType(FeedbackType::kRubberBand);
    
    auto config = m_defaultConfigs[FeedbackType::kRubberBand];
    auto item = FeedbackItem::Create(FeedbackType::kRubberBand);
    item->SetConfig(config);
    item->SetTag("rubberband");
    item->SetEnvelope(rect);
    
    return AddFeedback(item);
}

int FeedbackManager::ShowCrosshairFeedback(double screenX, double screenY)
{
    RemoveFeedbackByType(FeedbackType::kCrosshair);
    
    auto config = m_defaultConfigs[FeedbackType::kCrosshair];
    auto item = FeedbackItem::Create(FeedbackType::kCrosshair);
    item->SetConfig(config);
    item->SetTag("crosshair");
    
    Coordinate worldPoint(screenX, screenY);
    if (m_screenToWorld) {
        worldPoint = m_screenToWorld(screenX, screenY);
    }
    item->SetPoint(worldPoint);
    
    return AddFeedback(item);
}

int FeedbackManager::ShowTooltipFeedback(double screenX, double screenY, const std::string& text)
{
    RemoveFeedbackByType(FeedbackType::kTooltip);
    
    auto config = m_defaultConfigs[FeedbackType::kTooltip];
    config.text = text;
    auto item = FeedbackItem::Create(FeedbackType::kTooltip);
    item->SetConfig(config);
    item->SetTag("tooltip");
    item->SetPoint(Coordinate(screenX, screenY));
    
    return AddFeedback(item);
}

void FeedbackManager::HideSelectionFeedback()
{
    RemoveFeedbackByType(FeedbackType::kSelection);
}

void FeedbackManager::HideHighlightFeedback()
{
    RemoveFeedbackByType(FeedbackType::kHighlight);
}

void FeedbackManager::HideHoverFeedback()
{
    RemoveFeedbackByType(FeedbackType::kHover);
}

void FeedbackManager::HideMeasureFeedback()
{
    RemoveFeedbackByType(FeedbackType::kMeasure);
}

void FeedbackManager::HideRubberBandFeedback()
{
    RemoveFeedbackByType(FeedbackType::kRubberBand);
}

void FeedbackManager::HideCrosshairFeedback()
{
    RemoveFeedbackByType(FeedbackType::kCrosshair);
}

void FeedbackManager::HideTooltipFeedback()
{
    RemoveFeedbackByType(FeedbackType::kTooltip);
}

void FeedbackManager::SetDefaultConfig(FeedbackType type, const FeedbackConfig& config)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_defaultConfigs[type] = config;
}

FeedbackConfig FeedbackManager::GetDefaultConfig(FeedbackType type) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_defaultConfigs.find(type);
    if (it != m_defaultConfigs.end()) {
        return it->second;
    }
    return FeedbackConfig();
}

void FeedbackManager::SetFeedbackChangedCallback(FeedbackChangedCallback callback)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_feedbackChangedCallback = callback;
}

void FeedbackManager::SetScreenToWorldTransform(std::function<Coordinate(double, double)> transform)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_screenToWorld = transform;
}

void FeedbackManager::SetWorldToScreenTransform(std::function<Coordinate(double, double)> transform)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_worldToScreen = transform;
}

void FeedbackManager::UpdateAnimation(double deltaTime)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_animationEnabled) {
        return;
    }
    
    for (auto& item : m_feedbackItems) {
        if (item && item->GetConfig().animated) {
            auto config = item->GetConfig();
            config.opacity = 0.5 + 0.5 * std::sin(deltaTime * config.animationSpeed);
            item->SetConfig(config);
        }
    }
}

DrawStyle FeedbackManager::CreateStyleFromConfig(const FeedbackConfig& config) const
{
    DrawStyle style;
    style.pen.color = Color(config.strokeColor);
    style.pen.width = config.strokeWidth;
    style.brush.color = Color(config.fillColor);
    style.opacity = config.opacity;
    return style;
}

MeasureTool& MeasureTool::Instance()
{
    static MeasureTool instance;
    return instance;
}

MeasureTool::MeasureTool()
    : m_mode(MeasureMode::kNone)
    , m_unit("m")
    , m_active(false)
    , m_feedbackId(0)
    , m_feedbackManager(nullptr)
{
}

void MeasureTool::AddPoint(const Coordinate& point)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_points.push_back(point);
    UpdateFeedback();
}

void MeasureTool::RemoveLastPoint()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_points.empty()) {
        m_points.pop_back();
        UpdateFeedback();
    }
}

void MeasureTool::ClearPoints()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_points.clear();
    UpdateFeedback();
}

double MeasureTool::GetTotalDistance() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_points.size() < 2) {
        return 0.0;
    }
    
    double total = 0.0;
    for (size_t i = 0; i < m_points.size() - 1; ++i) {
        double dx = m_points[i + 1].x - m_points[i].x;
        double dy = m_points[i + 1].y - m_points[i].y;
        total += std::sqrt(dx * dx + dy * dy);
    }
    return total;
}

double MeasureTool::GetArea() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_points.size() < 3) {
        return 0.0;
    }
    
    double area = 0.0;
    size_t n = m_points.size();
    for (size_t i = 0; i < n; ++i) {
        size_t j = (i + 1) % n;
        area += m_points[i].x * m_points[j].y;
        area -= m_points[j].x * m_points[i].y;
    }
    return std::abs(area) / 2.0;
}

double MeasureTool::GetAngle() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_points.size() < 3) {
        return 0.0;
    }
    
    size_t n = m_points.size();
    const Coordinate& p1 = m_points[n - 3];
    const Coordinate& p2 = m_points[n - 2];
    const Coordinate& p3 = m_points[n - 1];
    
    double v1x = p1.x - p2.x;
    double v1y = p1.y - p2.y;
    double v2x = p3.x - p2.x;
    double v2y = p3.y - p2.y;
    
    double dot = v1x * v2x + v1y * v2y;
    double cross = v1x * v2y - v1y * v2x;
    
    return std::atan2(cross, dot) * 180.0 / 3.14159265358979323846;
}

std::string MeasureTool::GetDistanceString() const
{
    double distance = GetTotalDistance();
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    
    if (m_unit == "km") {
        oss << distance / 1000.0 << " km";
    } else if (m_unit == "nm") {
        oss << distance / 1852.0 << " nm";
    } else {
        if (distance >= 1000) {
            oss << distance / 1000.0 << " km";
        } else {
            oss << distance << " m";
        }
    }
    
    return oss.str();
}

std::string MeasureTool::GetAreaString() const
{
    double area = GetArea();
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    
    if (area >= 1000000) {
        oss << area / 1000000.0 << " km²";
    } else {
        oss << area << " m²";
    }
    
    return oss.str();
}

std::string MeasureTool::GetAngleString() const
{
    double angle = GetAngle();
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << angle << "°";
    return oss.str();
}

void MeasureTool::Start()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_active = true;
    m_points.clear();
}

void MeasureTool::Stop()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_active = false;
    if (m_feedbackManager && m_feedbackId > 0) {
        m_feedbackManager->RemoveFeedback(m_feedbackId);
        m_feedbackId = 0;
    }
}

void MeasureTool::UpdateFeedback()
{
    if (m_feedbackManager && !m_points.empty()) {
        m_feedbackId = m_feedbackManager->ShowMeasureFeedback(
            m_points, GetTotalDistance(), GetArea());
    }
}

}
}
