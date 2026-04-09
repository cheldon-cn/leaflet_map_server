#include "ogc/graph/label/label_placement.h"
#include "ogc/geom/point.h"
#include "ogc/geom/linestring.h"
#include "ogc/geom/polygon.h"
#include <cmath>
#include <algorithm>

using ogc::Point;
using ogc::LineString;
using ogc::Polygon;
using ogc::Coordinate;
using ogc::Geometry;

namespace ogc {
namespace graph {

LabelPlacement::LabelPlacement()
    : m_mode(LabelPlacementMode::kAuto)
    , m_offsetX(0)
    , m_offsetY(0)
    , m_anchorX(0.5)
    , m_anchorY(0.5)
    , m_displacementX(0)
    , m_displacementY(0)
    , m_rotation(0)
    , m_autoRotation(true)
    , m_maxDisplacement(256.0)
    , m_repeatDistance(0)
    , m_minPadding(0) {
}

void LabelPlacement::SetPlacementMode(LabelPlacementMode mode) {
    m_mode = mode;
}

LabelPlacementMode LabelPlacement::GetPlacementMode() const {
    return m_mode;
}

void LabelPlacement::SetOffset(double dx, double dy) {
    m_offsetX = dx;
    m_offsetY = dy;
}

void LabelPlacement::GetOffset(double& dx, double& dy) const {
    dx = m_offsetX;
    dy = m_offsetY;
}

void LabelPlacement::SetAnchorPoint(double x, double y) {
    m_anchorX = x;
    m_anchorY = y;
}

void LabelPlacement::GetAnchorPoint(double& x, double& y) const {
    x = m_anchorX;
    y = m_anchorY;
}

void LabelPlacement::SetDisplacement(double dx, double dy) {
    m_displacementX = dx;
    m_displacementY = dy;
}

void LabelPlacement::GetDisplacement(double& dx, double& dy) const {
    dx = m_displacementX;
    dy = m_displacementY;
}

void LabelPlacement::SetRotation(double angle) {
    m_rotation = angle;
}

double LabelPlacement::GetRotation() const {
    return m_rotation;
}

void LabelPlacement::SetAutoRotation(bool autoRotate) {
    m_autoRotation = autoRotate;
}

bool LabelPlacement::GetAutoRotation() const {
    return m_autoRotation;
}

void LabelPlacement::SetMaxDisplacement(double displacement) {
    m_maxDisplacement = displacement;
}

double LabelPlacement::GetMaxDisplacement() const {
    return m_maxDisplacement;
}

void LabelPlacement::SetRepeatDistance(double distance) {
    m_repeatDistance = distance;
}

double LabelPlacement::GetRepeatDistance() const {
    return m_repeatDistance;
}

void LabelPlacement::SetMinPadding(double padding) {
    m_minPadding = padding;
}

double LabelPlacement::GetMinPadding() const {
    return m_minPadding;
}

PlacementCandidate LabelPlacement::CreateCandidate(double x, double y, double rotation, LabelPosition pos, double score) {
    PlacementCandidate candidate;
    candidate.x = x + m_offsetX + m_displacementX;
    candidate.y = y + m_offsetY + m_displacementY;
    candidate.rotation = rotation;
    candidate.position = pos;
    candidate.score = score;
    candidate.isValid = true;
    return candidate;
}

bool LabelPlacement::IsCandidateValid(const PlacementCandidate& candidate, const Envelope& bounds) const {
    if (!candidate.isValid) {
        return false;
    }
    
    if (bounds.IsNull()) {
        return true;
    }
    
    return bounds.Contains(Coordinate(candidate.x, candidate.y));
}

std::vector<PlacementCandidate> LabelPlacement::GenerateCandidates(const Geometry* geometry, const std::string& text, double textWidth, double textHeight) {
    if (!geometry) {
        return std::vector<PlacementCandidate>();
    }
    
    LabelPlacementMode mode = m_mode;
    if (mode == LabelPlacementMode::kAuto) {
        GeomType geomType = geometry->GetGeometryType();
        switch (geomType) {
            case GeomType::kPoint:
                mode = LabelPlacementMode::kPoint;
                break;
            case GeomType::kLineString:
                mode = LabelPlacementMode::kLine;
                break;
            case GeomType::kPolygon:
                mode = LabelPlacementMode::kInterior;
                break;
            default:
                mode = LabelPlacementMode::kPoint;
                break;
        }
    }
    
    switch (mode) {
        case LabelPlacementMode::kPoint:
            return GeneratePointCandidates(geometry, textWidth, textHeight);
        case LabelPlacementMode::kLine:
            return GenerateLineCandidates(geometry, text, textWidth, textHeight);
        case LabelPlacementMode::kInterior:
            return GeneratePolygonCandidates(geometry, textWidth, textHeight);
        default:
            return GeneratePointCandidates(geometry, textWidth, textHeight);
    }
}

PlacementCandidate LabelPlacement::GetBestCandidate(const Geometry* geometry, const std::string& text, double textWidth, double textHeight) {
    auto candidates = GenerateCandidates(geometry, text, textWidth, textHeight);
    
    if (candidates.empty()) {
        return PlacementCandidate();
    }
    
    std::sort(candidates.begin(), candidates.end(),
        [](const PlacementCandidate& a, const PlacementCandidate& b) {
            return a.score > b.score;
        });
    
    return candidates[0];
}

std::vector<PlacementCandidate> LabelPlacement::GeneratePointCandidates(const Geometry* geometry, double textWidth, double textHeight) {
    std::vector<PlacementCandidate> candidates;
    
    const ogc::Point* point = dynamic_cast<const ogc::Point*>(geometry);
    if (!point) {
        return candidates;
    }
    
    ogc::Coordinate coord = point->GetCoordinate();
    double x = coord.x;
    double y = coord.y;
    
    candidates.push_back(CreateCandidate(x, y, m_rotation, LabelPosition::kCenter, 1.0));
    
    candidates.push_back(CreateCandidate(x, y + textHeight, m_rotation, LabelPosition::kTop, 0.8));
    candidates.push_back(CreateCandidate(x, y - textHeight, m_rotation, LabelPosition::kBottom, 0.8));
    candidates.push_back(CreateCandidate(x - textWidth, y, m_rotation, LabelPosition::kLeft, 0.7));
    candidates.push_back(CreateCandidate(x + textWidth, y, m_rotation, LabelPosition::kRight, 0.7));
    
    candidates.push_back(CreateCandidate(x - textWidth, y + textHeight, m_rotation, LabelPosition::kTopLeft, 0.6));
    candidates.push_back(CreateCandidate(x + textWidth, y + textHeight, m_rotation, LabelPosition::kTopRight, 0.6));
    candidates.push_back(CreateCandidate(x - textWidth, y - textHeight, m_rotation, LabelPosition::kBottomLeft, 0.6));
    candidates.push_back(CreateCandidate(x + textWidth, y - textHeight, m_rotation, LabelPosition::kBottomRight, 0.6));
    
    return candidates;
}

std::vector<PlacementCandidate> LabelPlacement::GenerateLineCandidates(const Geometry* geometry, const std::string& text, double textWidth, double textHeight) {
    std::vector<PlacementCandidate> candidates;
    
    const ogc::LineString* line = dynamic_cast<const ogc::LineString*>(geometry);
    if (!line) {
        return candidates;
    }
    
    size_t numPoints = line->GetNumPoints();
    if (numPoints < 2) {
        return candidates;
    }
    
    double totalLength = 0.0;
    for (size_t i = 1; i < numPoints; ++i) {
        ogc::Coordinate c1 = line->GetPointN(i - 1);
        ogc::Coordinate c2 = line->GetPointN(i);
        double dx = c2.x - c1.x;
        double dy = c2.y - c1.y;
        totalLength += std::sqrt(dx * dx + dy * dy);
    }
    
    if (totalLength < textWidth) {
        return candidates;
    }
    
    double halfLength = totalLength / 2.0;
    double currentLength = 0.0;
    ogc::Coordinate labelCoord = line->GetPointN(0);
    double labelRotation = 0.0;
    
    for (size_t i = 1; i < numPoints; ++i) {
        ogc::Coordinate c1 = line->GetPointN(i - 1);
        ogc::Coordinate c2 = line->GetPointN(i);
        double dx = c2.x - c1.x;
        double dy = c2.y - c1.y;
        double segmentLength = std::sqrt(dx * dx + dy * dy);
        
        if (currentLength + segmentLength >= halfLength) {
            double ratio = (halfLength - currentLength) / segmentLength;
            labelCoord.x = c1.x + dx * ratio;
            labelCoord.y = c1.y + dy * ratio;
            labelRotation = std::atan2(dy, dx) * 180.0 / 3.14159265358979323846;
            break;
        }
        
        currentLength += segmentLength;
    }
    
    double finalRotation = m_autoRotation ? labelRotation : m_rotation;
    candidates.push_back(CreateCandidate(labelCoord.x, labelCoord.y, finalRotation, LabelPosition::kCenter, 1.0));
    
    if (m_repeatDistance > 0 && totalLength > textWidth + m_repeatDistance * 2) {
        double positions[] = {totalLength * 0.25, totalLength * 0.75};
        for (double pos : positions) {
            currentLength = 0.0;
            for (size_t i = 1; i < numPoints; ++i) {
                ogc::Coordinate c1 = line->GetPointN(i - 1);
                ogc::Coordinate c2 = line->GetPointN(i);
                double dx = c2.x - c1.x;
                double dy = c2.y - c1.y;
                double segmentLength = std::sqrt(dx * dx + dy * dy);
                
                if (currentLength + segmentLength >= pos) {
                    double ratio = (pos - currentLength) / segmentLength;
                    double x = c1.x + dx * ratio;
                    double y = c1.y + dy * ratio;
                    double rot = std::atan2(dy, dx) * 180.0 / 3.14159265358979323846;
                    candidates.push_back(CreateCandidate(x, y, m_autoRotation ? rot : m_rotation, LabelPosition::kCenter, 0.5));
                    break;
                }
                
                currentLength += segmentLength;
            }
        }
    }
    
    (void)text;
    (void)textHeight;
    return candidates;
}

std::vector<PlacementCandidate> LabelPlacement::GeneratePolygonCandidates(const Geometry* geometry, double textWidth, double textHeight) {
    std::vector<PlacementCandidate> candidates;
    
    const ogc::Polygon* polygon = dynamic_cast<const ogc::Polygon*>(geometry);
    if (!polygon) {
        return candidates;
    }
    
    ogc::Envelope env = polygon->GetEnvelope();
    ogc::Coordinate centre = env.GetCentre();
    
    candidates.push_back(CreateCandidate(centre.x, centre.y, m_rotation, LabelPosition::kCenter, 1.0));
    
    double envWidth = env.GetWidth();
    double envHeight = env.GetHeight();
    
    if (envWidth > textWidth * 2 && envHeight > textHeight * 2) {
        candidates.push_back(CreateCandidate(centre.x, centre.y + envHeight * 0.25, m_rotation, LabelPosition::kTop, 0.7));
        candidates.push_back(CreateCandidate(centre.x, centre.y - envHeight * 0.25, m_rotation, LabelPosition::kBottom, 0.7));
    }
    
    return candidates;
}

LabelPlacementPtr LabelPlacement::Create() {
    return std::make_shared<LabelPlacement>();
}

}
}
