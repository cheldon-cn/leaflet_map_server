#ifndef OGC_DRAW_LABEL_ENGINE_H
#define OGC_DRAW_LABEL_ENGINE_H

#include "ogc/draw/symbolizer_rule.h"
#include "ogc/draw/draw_context.h"
#include "ogc/feature/feature.h"
#include "ogc/envelope.h"
#include <memory>
#include <vector>
#include <string>
#include <functional>

namespace ogc {
namespace draw {

struct LabelInfo {
    std::string text;
    double x;
    double y;
    double width;
    double height;
    double rotation;
    int priority;
    bool visible;
    uint32_t featureId;
    
    LabelInfo()
        : x(0), y(0), width(0), height(0), rotation(0)
        , priority(0), visible(true), featureId(0) {}
    
    Envelope GetBounds() const {
        return Envelope(x, y, x + width, y + height);
    }
    
    bool Intersects(const LabelInfo& other) const {
        return GetBounds().Intersects(other.GetBounds());
    }
};

struct LabelPlacementResult {
    bool success;
    double x;
    double y;
    double rotation;
    std::string message;
    
    LabelPlacementResult()
        : success(false), x(0), y(0), rotation(0) {}
};

class LabelConflictResolver;
typedef std::shared_ptr<LabelConflictResolver> LabelConflictResolverPtr;

class LabelEngine;
typedef std::shared_ptr<LabelEngine> LabelEnginePtr;

class LabelEngine {
public:
    LabelEngine();
    ~LabelEngine() = default;
    
    void SetLabelProperty(const std::string& propertyName);
    std::string GetLabelProperty() const;
    
    void SetFont(const Font& font);
    Font GetFont() const;
    
    void SetColor(uint32_t color);
    uint32_t GetColor() const;
    
    void SetHaloColor(uint32_t color);
    uint32_t GetHaloColor() const;
    
    void SetHaloRadius(double radius);
    double GetHaloRadius() const;
    
    void SetMinDistance(double distance);
    double GetMinDistance() const;
    
    void SetMaxDistance(double distance);
    double GetMaxDistance() const;
    
    void SetPriority(int priority);
    int GetPriority() const;
    
    void SetAllowOverlap(bool allow);
    bool GetAllowOverlap() const;
    
    void SetFollowLine(bool follow);
    bool GetFollowLine() const;
    
    void SetMaxAngleDelta(double delta);
    double GetMaxAngleDelta() const;
    
    void SetConflictResolver(LabelConflictResolverPtr resolver);
    LabelConflictResolverPtr GetConflictResolver() const;
    
    std::vector<LabelInfo> GenerateLabels(const std::vector<const CNFeature*>& features, DrawContext& context);
    std::vector<LabelInfo> GenerateLabels(const CNFeature* feature, DrawContext& context);
    
    LabelPlacementResult PlaceLabel(const CNFeature* feature, DrawContext& context);
    LabelPlacementResult PlacePointLabel(const Geometry* geometry, const std::string& text, DrawContext& context);
    LabelPlacementResult PlaceLineLabel(const Geometry* geometry, const std::string& text, DrawContext& context);
    LabelPlacementResult PlacePolygonLabel(const Geometry* geometry, const std::string& text, DrawContext& context);
    
    DrawResult RenderLabels(const std::vector<LabelInfo>& labels, DrawContext& context);
    
    void ClearCache();
    
    static LabelEnginePtr Create();

private:
    std::string GetLabelText(const CNFeature* feature) const;
    LabelPlacementResult PlaceLabelInternal(const Geometry* geometry, const std::string& text, DrawContext& context);
    
    std::string m_labelProperty;
    Font m_font;
    uint32_t m_color;
    uint32_t m_haloColor;
    double m_haloRadius;
    double m_minDistance;
    double m_maxDistance;
    int m_priority;
    bool m_allowOverlap;
    bool m_followLine;
    double m_maxAngleDelta;
    LabelConflictResolverPtr m_conflictResolver;
};

}
}

#endif
