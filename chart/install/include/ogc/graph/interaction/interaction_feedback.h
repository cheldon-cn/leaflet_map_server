#ifndef OGC_GRAPH_INTERACTION_FEEDBACK_H
#define OGC_GRAPH_INTERACTION_FEEDBACK_H

#include "ogc/graph/export.h"
#include <ogc/draw/draw_result.h>
#include <ogc/draw/draw_style.h>
#include <ogc/draw/draw_types.h>
#include "ogc/geom/coordinate.h"
#include "ogc/geom/envelope.h"
#include "ogc/geom/geometry.h"
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <mutex>
#include <map>

namespace ogc {
namespace graph {

using ogc::draw::DrawResult;

class DrawContext;
using DrawContextPtr = std::shared_ptr<DrawContext>;
class DrawDevice;

enum class FeedbackType {
    kNone = 0,
    kSelection,
    kHighlight,
    kHover,
    kMeasure,
    kEdit,
    kRubberBand,
    kCrosshair,
    kTooltip
};

enum class FeedbackStyle {
    kDefault = 0,
    kSolid,
    kDashed,
    kDotted,
    kAnimated
};

struct FeedbackConfig {
    FeedbackType type = FeedbackType::kNone;
    FeedbackStyle style = FeedbackStyle::kDefault;
    uint32_t fillColor = 0x00000000;
    uint32_t strokeColor = 0xFF000000;
    double strokeWidth = 1.0;
    double opacity = 1.0;
    bool visible = true;
    bool animated = false;
    double animationSpeed = 1.0;
    std::string text;
    std::string fontName;
    double fontSize = 12.0;
};

class FeedbackItem;
using FeedbackItemPtr = std::shared_ptr<FeedbackItem>;

class OGC_GRAPH_API FeedbackItem {
public:
    static FeedbackItemPtr Create(FeedbackType type);
    static FeedbackItemPtr Create(FeedbackType type, const Geometry* geometry);
    static FeedbackItemPtr Create(FeedbackType type, const Envelope& envelope);
    static FeedbackItemPtr Create(FeedbackType type, const Coordinate& point);
    
    void SetGeometry(const Geometry* geometry);
    void SetEnvelope(const Envelope& envelope);
    void SetPoint(const Coordinate& point);
    void SetPoints(const std::vector<Coordinate>& points);
    
    const Geometry* GetGeometry() const { return m_geometry.get(); }
    Envelope GetEnvelope() const { return m_envelope; }
    Coordinate GetPoint() const { return m_point; }
    
    void SetConfig(const FeedbackConfig& config) { m_config = config; }
    FeedbackConfig& GetConfig() { return m_config; }
    const FeedbackConfig& GetConfig() const { return m_config; }
    
    void SetVisible(bool visible) { m_config.visible = visible; }
    bool IsVisible() const { return m_config.visible; }
    
    void SetId(int id) { m_id = id; }
    int GetId() const { return m_id; }
    
    void SetTag(const std::string& tag) { m_tag = tag; }
    std::string GetTag() const { return m_tag; }
    
    void SetUserData(void* data) { m_userData = data; }
    void* GetUserData() const { return m_userData; }
    
    DrawResult Render(DrawContextPtr context);
    
private:
    FeedbackItem(FeedbackType type);
    
    int m_id;
    std::string m_tag;
    FeedbackType m_type;
    FeedbackConfig m_config;
    GeometryPtr m_geometry;
    Envelope m_envelope;
    Coordinate m_point;
    std::vector<Coordinate> m_points;
    void* m_userData;
};

class FeedbackManager;
using FeedbackManagerPtr = std::shared_ptr<FeedbackManager>;

class OGC_GRAPH_API FeedbackManager {
public:
    using FeedbackChangedCallback = std::function<void()>;
    
    static FeedbackManager& Instance();
    
    bool Initialize();
    void Finalize();
    bool IsInitialized() const { return m_initialized; }
    
    int AddFeedback(FeedbackItemPtr item);
    void RemoveFeedback(int id);
    void RemoveFeedbackByTag(const std::string& tag);
    void RemoveFeedbackByType(FeedbackType type);
    void RemoveAllFeedback();
    
    FeedbackItemPtr GetFeedback(int id) const;
    std::vector<FeedbackItemPtr> GetFeedbackByType(FeedbackType type) const;
    std::vector<FeedbackItemPtr> GetFeedbackByTag(const std::string& tag) const;
    std::vector<FeedbackItemPtr> GetAllFeedback() const;
    
    void SetFeedbackVisible(int id, bool visible);
    void SetFeedbackVisibleByType(FeedbackType type, bool visible);
    
    size_t GetFeedbackCount() const;
    
    DrawResult Render(DrawContextPtr context);
    DrawResult RenderByType(DrawContextPtr context, FeedbackType type);
    
    int ShowSelectionFeedback(const std::vector<int64_t>& featureIds, const std::string& layerName);
    int ShowHighlightFeedback(int64_t featureId, const std::string& layerName);
    int ShowHoverFeedback(double screenX, double screenY, const std::string& text = "");
    int ShowMeasureFeedback(const std::vector<Coordinate>& points, double distance, double area = 0.0);
    int ShowRubberBandFeedback(const Envelope& rect);
    int ShowCrosshairFeedback(double screenX, double screenY);
    int ShowTooltipFeedback(double screenX, double screenY, const std::string& text);
    
    void HideSelectionFeedback();
    void HideHighlightFeedback();
    void HideHoverFeedback();
    void HideMeasureFeedback();
    void HideRubberBandFeedback();
    void HideCrosshairFeedback();
    void HideTooltipFeedback();
    
    void SetDefaultConfig(FeedbackType type, const FeedbackConfig& config);
    FeedbackConfig GetDefaultConfig(FeedbackType type) const;
    
    void SetFeedbackChangedCallback(FeedbackChangedCallback callback);
    
    void SetScreenToWorldTransform(std::function<Coordinate(double, double)> transform);
    void SetWorldToScreenTransform(std::function<Coordinate(double, double)> transform);
    
    void SetAnimationEnabled(bool enabled) { m_animationEnabled = enabled; }
    bool IsAnimationEnabled() const { return m_animationEnabled; }
    
    void UpdateAnimation(double deltaTime);
    
private:
    FeedbackManager();
    ~FeedbackManager();
    
    FeedbackManager(const FeedbackManager&) = delete;
    FeedbackManager& operator=(const FeedbackManager&) = delete;
    
    ogc::draw::DrawStyle CreateStyleFromConfig(const FeedbackConfig& config) const;
    
    bool m_initialized;
    bool m_animationEnabled;
    std::vector<FeedbackItemPtr> m_feedbackItems;
    std::map<FeedbackType, FeedbackConfig> m_defaultConfigs;
    FeedbackChangedCallback m_feedbackChangedCallback;
    std::function<Coordinate(double, double)> m_screenToWorld;
    std::function<Coordinate(double, double)> m_worldToScreen;
    int m_nextId;
    mutable std::mutex m_mutex;
};

class OGC_GRAPH_API MeasureTool {
public:
    enum class MeasureMode {
        kNone = 0,
        kDistance,
        kArea,
        kAngle
    };
    
    static MeasureTool& Instance();
    
    void SetMode(MeasureMode mode) { m_mode = mode; }
    MeasureMode GetMode() const { return m_mode; }
    
    void AddPoint(const Coordinate& point);
    void RemoveLastPoint();
    void ClearPoints();
    
    const std::vector<Coordinate>& GetPoints() const { return m_points; }
    size_t GetPointCount() const { return m_points.size(); }
    
    double GetTotalDistance() const;
    double GetArea() const;
    double GetAngle() const;
    
    std::string GetDistanceString() const;
    std::string GetAreaString() const;
    std::string GetAngleString() const;
    
    void SetUnit(const std::string& unit) { m_unit = unit; }
    std::string GetUnit() const { return m_unit; }
    
    void Start();
    void Stop();
    bool IsActive() const { return m_active; }
    
    void SetFeedbackManager(FeedbackManager* manager) { m_feedbackManager = manager; }
    
private:
    MeasureTool();
    ~MeasureTool() = default;
    
    MeasureTool(const MeasureTool&) = delete;
    MeasureTool& operator=(const MeasureTool&) = delete;
    
    void UpdateFeedback();
    
    MeasureMode m_mode;
    std::vector<Coordinate> m_points;
    std::string m_unit;
    bool m_active;
    int m_feedbackId;
    FeedbackManager* m_feedbackManager;
    mutable std::mutex m_mutex;
};

}
}

#endif
