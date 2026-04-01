#include "ogc/draw/label_engine.h"
#include <ogc/draw/color.h>
#include "ogc/point.h"
#include "ogc/linestring.h"
#include "ogc/polygon.h"
#include <cmath>

using ogc::Point;
using ogc::LineString;
using ogc::Polygon;
using ogc::Coordinate;
using ogc::Geometry;

namespace ogc {
namespace draw {

LabelEngine::LabelEngine()
    : m_color(0xFF000000)
    , m_haloColor(0xFFFFFFFF)
    , m_haloRadius(0.0)
    , m_minDistance(0.0)
    , m_maxDistance(256.0)
    , m_priority(0)
    , m_allowOverlap(false)
    , m_followLine(false)
    , m_maxAngleDelta(22.5) {
}

void LabelEngine::SetLabelProperty(const std::string& propertyName) {
    m_labelProperty = propertyName;
}

std::string LabelEngine::GetLabelProperty() const {
    return m_labelProperty;
}

void LabelEngine::SetFont(const Font& font) {
    m_font = font;
}

Font LabelEngine::GetFont() const {
    return m_font;
}

void LabelEngine::SetColor(uint32_t color) {
    m_color = color;
}

uint32_t LabelEngine::GetColor() const {
    return m_color;
}

void LabelEngine::SetHaloColor(uint32_t color) {
    m_haloColor = color;
}

uint32_t LabelEngine::GetHaloColor() const {
    return m_haloColor;
}

void LabelEngine::SetHaloRadius(double radius) {
    m_haloRadius = radius;
}

double LabelEngine::GetHaloRadius() const {
    return m_haloRadius;
}

void LabelEngine::SetMinDistance(double distance) {
    m_minDistance = distance;
}

double LabelEngine::GetMinDistance() const {
    return m_minDistance;
}

void LabelEngine::SetMaxDistance(double distance) {
    m_maxDistance = distance;
}

double LabelEngine::GetMaxDistance() const {
    return m_maxDistance;
}

void LabelEngine::SetPriority(int priority) {
    m_priority = priority;
}

int LabelEngine::GetPriority() const {
    return m_priority;
}

void LabelEngine::SetAllowOverlap(bool allow) {
    m_allowOverlap = allow;
}

bool LabelEngine::GetAllowOverlap() const {
    return m_allowOverlap;
}

void LabelEngine::SetFollowLine(bool follow) {
    m_followLine = follow;
}

bool LabelEngine::GetFollowLine() const {
    return m_followLine;
}

void LabelEngine::SetMaxAngleDelta(double delta) {
    m_maxAngleDelta = delta;
}

double LabelEngine::GetMaxAngleDelta() const {
    return m_maxAngleDelta;
}

void LabelEngine::SetConflictResolver(LabelConflictResolverPtr resolver) {
    m_conflictResolver = resolver;
}

LabelConflictResolverPtr LabelEngine::GetConflictResolver() const {
    return m_conflictResolver;
}

std::string LabelEngine::GetLabelText(const CNFeature* feature) const {
    if (!feature || m_labelProperty.empty()) {
        return "";
    }
    
    return feature->GetFieldAsString(m_labelProperty.c_str());
}

std::vector<LabelInfo> LabelEngine::GenerateLabels(const std::vector<const CNFeature*>& features, DrawContext& context) {
    std::vector<LabelInfo> labels;
    
    for (const auto& feature : features) {
        if (!feature) continue;
        
        auto featureLabels = GenerateLabels(feature, context);
        labels.insert(labels.end(), featureLabels.begin(), featureLabels.end());
    }
    
    return labels;
}

std::vector<LabelInfo> LabelEngine::GenerateLabels(const CNFeature* feature, DrawContext& context) {
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
        info.priority = m_priority;
        info.featureId = feature->GetFID();
        
        double textWidth = text.length() * m_font.GetSize() * 0.6;
        double textHeight = m_font.GetSize();
        info.width = textWidth;
        info.height = textHeight;
        
        labels.push_back(info);
    }
    
    return labels;
}

LabelPlacementResult LabelEngine::PlaceLabel(const CNFeature* feature, DrawContext& context) {
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

LabelPlacementResult LabelEngine::PlacePointLabel(const Geometry* geometry, const std::string& text, DrawContext& context) {
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

LabelPlacementResult LabelEngine::PlaceLineLabel(const Geometry* geometry, const std::string& text, DrawContext& context) {
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

LabelPlacementResult LabelEngine::PlacePolygonLabel(const Geometry* geometry, const std::string& text, DrawContext& context) {
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

LabelPlacementResult LabelEngine::PlaceLabelInternal(const Geometry* geometry, const std::string& text, DrawContext& context) {
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

DrawResult LabelEngine::RenderLabels(const std::vector<LabelInfo>& labels, DrawContext& context) {
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
        
        if (m_haloRadius > 0.0) {
            Color haloColor(m_haloColor);
            context.DrawText(label.x - 1, label.y, label.text, m_font, haloColor);
            context.DrawText(label.x + 1, label.y, label.text, m_font, haloColor);
            context.DrawText(label.x, label.y - 1, label.text, m_font, haloColor);
            context.DrawText(label.x, label.y + 1, label.text, m_font, haloColor);
        }
        
        Color textColor(m_color);
        context.DrawText(label.x, label.y, label.text, m_font, textColor);
        
        if (label.rotation != 0.0) {
            context.Restore();
        }
    }
    
    return DrawResult::kSuccess;
}

void LabelEngine::ClearCache() {
}

LabelEnginePtr LabelEngine::Create() {
    return std::make_shared<LabelEngine>();
}

}
}
