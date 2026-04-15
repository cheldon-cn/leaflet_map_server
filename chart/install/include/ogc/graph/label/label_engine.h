#ifndef OGC_GRAPH_LABEL_ENGINE_H
#define OGC_GRAPH_LABEL_ENGINE_H

#include "ogc/graph/export.h"
#include "ogc/graph/label/label_info.h"
#include <ogc/draw/draw_context.h>
#include "ogc/feature/feature.h"
#include "ogc/geom/envelope.h"
#include <memory>
#include <vector>
#include <string>
#include <functional>

namespace ogc {
namespace graph {

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

class OGC_GRAPH_API LabelEngine {
public:
    LabelEngine();
    ~LabelEngine();
    
    void SetLabelProperty(const std::string& propertyName);
    std::string GetLabelProperty() const;
    
    void SetFont(const ogc::draw::Font& font);
    ogc::draw::Font GetFont() const;
    
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
    
    std::vector<LabelInfo> GenerateLabels(const std::vector<const CNFeature*>& features, ogc::draw::DrawContext& context);
    std::vector<LabelInfo> GenerateLabels(const CNFeature* feature, ogc::draw::DrawContext& context);
    
    LabelPlacementResult PlaceLabel(const CNFeature* feature, ogc::draw::DrawContext& context);
    LabelPlacementResult PlacePointLabel(const Geometry* geometry, const std::string& text, ogc::draw::DrawContext& context);
    LabelPlacementResult PlaceLineLabel(const Geometry* geometry, const std::string& text, ogc::draw::DrawContext& context);
    LabelPlacementResult PlacePolygonLabel(const Geometry* geometry, const std::string& text, ogc::draw::DrawContext& context);
    
    ogc::draw::DrawResult RenderLabels(const std::vector<LabelInfo>& labels, ogc::draw::DrawContext& context);
    
    void ClearCache();
    
    static LabelEnginePtr Create();

private:
    std::string GetLabelText(const CNFeature* feature) const;
    LabelPlacementResult PlaceLabelInternal(const Geometry* geometry, const std::string& text, ogc::draw::DrawContext& context);
    
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}
}

#endif
