#include "ogc/graph/label/label_engine.h"
#include <ogc/draw/color.h>
#include "ogc/geom/point.h"
#include "ogc/geom/linestring.h"
#include "ogc/geom/polygon.h"
#include <cmath>

using ogc::Point;
using ogc::LineString;
using ogc::Polygon;
using ogc::Coordinate;
using ogc::Geometry;

namespace ogc {
namespace graph {

struct LabelEngine::Impl {
    std::string labelProperty;
    ogc::draw::Font font;
    uint32_t color;
    uint32_t haloColor;
    double haloRadius;
    double minDistance;
    double maxDistance;
    int priority;
    bool allowOverlap;
    bool followLine;
    double maxAngleDelta;
    LabelConflictResolverPtr conflictResolver;
    
    Impl()
        : color(0xFF000000)
        , haloColor(0xFFFFFFFF)
        , haloRadius(0.0)
        , minDistance(0.0)
        , maxDistance(256.0)
        , priority(0)
        , allowOverlap(false)
        , followLine(false)
        , maxAngleDelta(22.5) {
    }
};

LabelEngine::LabelEngine()
    : impl_(std::make_unique<Impl>()) {
}

LabelEngine::~LabelEngine() = default;

void LabelEngine::SetLabelProperty(const std::string& propertyName) {
    impl_->labelProperty = propertyName;
}

std::string LabelEngine::GetLabelProperty() const {
    return impl_->labelProperty;
}

void LabelEngine::SetFont(const ogc::draw::Font& font) {
    impl_->font = font;
}

ogc::draw::Font LabelEngine::GetFont() const {
    return impl_->font;
}

void LabelEngine::SetColor(uint32_t color) {
    impl_->color = color;
}

uint32_t LabelEngine::GetColor() const {
    return impl_->color;
}

void LabelEngine::SetHaloColor(uint32_t color) {
    impl_->haloColor = color;
}

uint32_t LabelEngine::GetHaloColor() const {
    return impl_->haloColor;
}

void LabelEngine::SetHaloRadius(double radius) {
    impl_->haloRadius = radius;
}

double LabelEngine::GetHaloRadius() const {
    return impl_->haloRadius;
}

void LabelEngine::SetMinDistance(double distance) {
    impl_->minDistance = distance;
}

double LabelEngine::GetMinDistance() const {
    return impl_->minDistance;
}

void LabelEngine::SetMaxDistance(double distance) {
    impl_->maxDistance = distance;
}

double LabelEngine::GetMaxDistance() const {
    return impl_->maxDistance;
}

void LabelEngine::SetPriority(int priority) {
    impl_->priority = priority;
}

int LabelEngine::GetPriority() const {
    return impl_->priority;
}

void LabelEngine::SetAllowOverlap(bool allow) {
    impl_->allowOverlap = allow;
}

bool LabelEngine::GetAllowOverlap() const {
    return impl_->allowOverlap;
}

void LabelEngine::SetFollowLine(bool follow) {
    impl_->followLine = follow;
}

bool LabelEngine::GetFollowLine() const {
    return impl_->followLine;
}

void LabelEngine::SetMaxAngleDelta(double delta) {
    impl_->maxAngleDelta = delta;
}

double LabelEngine::GetMaxAngleDelta() const {
    return impl_->maxAngleDelta;
}

void LabelEngine::SetConflictResolver(LabelConflictResolverPtr resolver) {
    impl_->conflictResolver = resolver;
}

LabelConflictResolverPtr LabelEngine::GetConflictResolver() const {
    return impl_->conflictResolver;
}

std::string LabelEngine::GetLabelText(const CNFeature* feature) const {
    if (!feature || impl_->labelProperty.empty()) {
        return "";
    }
    
    return feature->GetFieldAsString(impl_->labelProperty.c_str());
}

std::vector<LabelInfo> LabelEngine::GenerateLabels(const std::vector<const CNFeature*>& features, ogc::draw::DrawContext& context) {
    std::vector<LabelInfo> labels;
    
    for (const auto& feature : features) {
        if (!feature) continue;
        
        auto featureLabels = GenerateLabels(feature, context);
        labels.insert(labels.end(), featureLabels.begin(), featureLabels.end());
    }
    
    return labels;
}

std::vector<LabelInfo> LabelEngine::GenerateLabels(const CNFeature* feature, ogc::draw::DrawContext& context) {
    std::vector<LabelInfo> labels;
    
    if (!feature) {
        return labels;
    }
    
    std::string text = GetLabelText(feature);
    if (text.empty()) {
        return labels;
    }
    
    GeometryPtr geom = feature->GetGeometry();
    if (!geom) {
        return labels;
    }
    
    LabelPlacementResult placement = PlaceLabelInternal(geom.get(), text, context);
    
    if (placement.success) {
        LabelInfo info;
        info.text = text;
        info.x = placement.x;
        info.y = placement.y;
        info.rotation = placement.rotation;
        info.priority = impl_->priority;
        info.featureId = feature->GetFID();
        
        double textWidth = text.length() * impl_->font.GetSize() * 0.6;
        double textHeight = impl_->font.GetSize();
        info.width = textWidth;
        info.height = textHeight;
        
        labels.push_back(info);
    }
    
    return labels;
}

LabelPlacementResult LabelEngine::PlaceLabel(const CNFeature* feature, ogc::draw::DrawContext& context) {
    LabelPlacementResult result;
    
    if (!feature) {
        result.message = "Feature is null";
        return result;
    }
    
    std::string text = GetLabelText(feature);
    if (text.empty()) {
        result.message = "Label text is empty";
        return result;
    }
    
    GeometryPtr geom = feature->GetGeometry();
    if (!geom) {
        result.message = "Geometry is null";
        return result;
    }
    
    return PlaceLabelInternal(geom.get(), text, context);
}

LabelPlacementResult LabelEngine::PlacePointLabel(const Geometry* geometry, const std::string& text, ogc::draw::DrawContext& context) {
    LabelPlacementResult result;
    
    if (!geometry || text.empty()) {
        result.message = "Invalid parameters";
        return result;
    }
    
    const ogc::Point* point = dynamic_cast<const ogc::Point*>(geometry);
    if (!point) {
        result.message = "Geometry is not a point";
        return result;
    }
    
    ogc::Coordinate coord = point->GetCoordinate();
    result.success = true;
    result.x = coord.x;
    result.y = coord.y;
    result.rotation = 0.0;
    
    (void)context;
    return result;
}

LabelPlacementResult LabelEngine::PlaceLineLabel(const Geometry* geometry, const std::string& text, ogc::draw::DrawContext& context) {
    LabelPlacementResult result;
    
    if (!geometry || text.empty()) {
        result.message = "Invalid parameters";
        return result;
    }
    
    const ogc::LineString* line = dynamic_cast<const ogc::LineString*>(geometry);
    if (!line) {
        result.message = "Geometry is not a linestring";
        return result;
    }
    
    size_t numPoints = line->GetNumPoints();
    if (numPoints < 2) {
        result.message = "Line has too few points";
        return result;
    }
    
    size_t midIndex = numPoints / 2;
    ogc::Coordinate startCoord = line->GetPointN(midIndex - 1);
    ogc::Coordinate endCoord = line->GetPointN(midIndex);
    
    double dx = endCoord.x - startCoord.x;
    double dy = endCoord.y - startCoord.y;
    double rotation = std::atan2(dy, dx) * 180.0 / 3.14159265358979323846;
    
    double totalLength = 0.0;
    for (size_t i = 1; i < numPoints; ++i) {
        ogc::Coordinate c1 = line->GetPointN(i - 1);
        ogc::Coordinate c2 = line->GetPointN(i);
        double dx = c2.x - c1.x;
        double dy = c2.y - c1.y;
        totalLength += std::sqrt(dx * dx + dy * dy);
    }
    
    double halfLength = totalLength / 2.0;
    double currentLength = 0.0;
    double targetLength = halfLength;
    
    ogc::Coordinate labelCoord = line->GetPointN(0);
    for (size_t i = 1; i < numPoints; ++i) {
        ogc::Coordinate c1 = line->GetPointN(i - 1);
        ogc::Coordinate c2 = line->GetPointN(i);
        double dx = c2.x - c1.x;
        double dy = c2.y - c1.y;
        double segmentLength = std::sqrt(dx * dx + dy * dy);
        
        if (currentLength + segmentLength >= targetLength) {
            double ratio = (targetLength - currentLength) / segmentLength;
            labelCoord.x = c1.x + dx * ratio;
            labelCoord.y = c1.y + dy * ratio;
            break;
        }
        
        currentLength += segmentLength;
    }
    
    result.success = true;
    result.x = labelCoord.x;
    result.y = labelCoord.y;
    result.rotation = rotation;
    
    (void)context;
    return result;
}

LabelPlacementResult LabelEngine::PlacePolygonLabel(const Geometry* geometry, const std::string& text, ogc::draw::DrawContext& context) {
    LabelPlacementResult result;
    
    if (!geometry || text.empty()) {
        result.message = "Invalid parameters";
        return result;
    }
    
    const ogc::Polygon* polygon = dynamic_cast<const ogc::Polygon*>(geometry);
    if (!polygon) {
        result.message = "Geometry is not a polygon";
        return result;
    }
    
    ogc::Envelope env = polygon->GetEnvelope();
    ogc::Coordinate centre = env.GetCentre();
    
    result.success = true;
    result.x = centre.x;
    result.y = centre.y;
    result.rotation = 0.0;
    
    (void)context;
    return result;
}

LabelPlacementResult LabelEngine::PlaceLabelInternal(const Geometry* geometry, const std::string& text, ogc::draw::DrawContext& context) {
    if (!geometry) {
        LabelPlacementResult result;
        result.message = "Geometry is null";
        return result;
    }
    
    GeomType geomType = geometry->GetGeometryType();
    
    switch (geomType) {
        case GeomType::kPoint:
            return PlacePointLabel(geometry, text, context);
        case GeomType::kLineString:
            return PlaceLineLabel(geometry, text, context);
        case GeomType::kPolygon:
            return PlacePolygonLabel(geometry, text, context);
        default:
            break;
    }
    
    LabelPlacementResult result;
    result.message = "Unsupported geometry type for labeling";
    return result;
}

ogc::draw::DrawResult LabelEngine::RenderLabels(const std::vector<LabelInfo>& labels, ogc::draw::DrawContext& context) {
    for (const auto& label : labels) {
        if (!label.visible) {
            continue;
        }
        
        if (label.rotation != 0.0) {
            context.Save();
            context.Translate(label.x, label.y);
            context.Rotate(label.rotation);
            context.Translate(-label.x, -label.y);
        }
        
        if (impl_->haloRadius > 0.0) {
            ogc::draw::Color haloColor(impl_->haloColor);
            context.DrawText(label.x - 1, label.y, label.text, impl_->font, haloColor);
            context.DrawText(label.x + 1, label.y, label.text, impl_->font, haloColor);
            context.DrawText(label.x, label.y - 1, label.text, impl_->font, haloColor);
            context.DrawText(label.x, label.y + 1, label.text, impl_->font, haloColor);
        }
        
        ogc::draw::Color textColor(impl_->color);
        context.DrawText(label.x, label.y, label.text, impl_->font, textColor);
        
        if (label.rotation != 0.0) {
            context.Restore();
        }
    }
    
    return ogc::draw::DrawResult::kSuccess;
}

void LabelEngine::ClearCache() {
}

LabelEnginePtr LabelEngine::Create() {
    return std::make_shared<LabelEngine>();
}

}
}
