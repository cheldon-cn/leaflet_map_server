#ifndef OGC_DRAW_LABEL_PLACEMENT_H
#define OGC_DRAW_LABEL_PLACEMENT_H

#include "ogc/draw/export.h"
#include "ogc/draw/label_engine.h"
#include "ogc/geometry.h"
#include "ogc/envelope.h"
#include <memory>
#include <vector>

namespace ogc {
namespace draw {

enum class LabelPlacementMode {
    kPoint,
    kLine,
    kInterior,
    kAuto
};

enum class LabelPosition {
    kCenter,
    kTop,
    kBottom,
    kLeft,
    kRight,
    kTopLeft,
    kTopRight,
    kBottomLeft,
    kBottomRight
};

struct PlacementCandidate {
    double x;
    double y;
    double rotation;
    double score;
    LabelPosition position;
    bool isValid;
    
    PlacementCandidate()
        : x(0), y(0), rotation(0), score(0)
        , position(LabelPosition::kCenter), isValid(false) {}
};

class OGC_GRAPH_API LabelPlacement {
public:
    LabelPlacement();
    ~LabelPlacement() = default;
    
    void SetPlacementMode(LabelPlacementMode mode);
    LabelPlacementMode GetPlacementMode() const;
    
    void SetOffset(double dx, double dy);
    void GetOffset(double& dx, double& dy) const;
    
    void SetAnchorPoint(double x, double y);
    void GetAnchorPoint(double& x, double& y) const;
    
    void SetDisplacement(double dx, double dy);
    void GetDisplacement(double& dx, double& dy) const;
    
    void SetRotation(double angle);
    double GetRotation() const;
    
    void SetAutoRotation(bool autoRotate);
    bool GetAutoRotation() const;
    
    void SetMaxDisplacement(double displacement);
    double GetMaxDisplacement() const;
    
    void SetRepeatDistance(double distance);
    double GetRepeatDistance() const;
    
    void SetMinPadding(double padding);
    double GetMinPadding() const;
    
    std::vector<PlacementCandidate> GenerateCandidates(const Geometry* geometry, const std::string& text, double textWidth, double textHeight);
    
    PlacementCandidate GetBestCandidate(const Geometry* geometry, const std::string& text, double textWidth, double textHeight);
    
    std::vector<PlacementCandidate> GeneratePointCandidates(const Geometry* geometry, double textWidth, double textHeight);
    std::vector<PlacementCandidate> GenerateLineCandidates(const Geometry* geometry, const std::string& text, double textWidth, double textHeight);
    std::vector<PlacementCandidate> GeneratePolygonCandidates(const Geometry* geometry, double textWidth, double textHeight);
    
    static LabelPlacementPtr Create();

private:
    PlacementCandidate CreateCandidate(double x, double y, double rotation, LabelPosition pos, double score);
    bool IsCandidateValid(const PlacementCandidate& candidate, const Envelope& bounds) const;
    
    LabelPlacementMode m_mode;
    double m_offsetX;
    double m_offsetY;
    double m_anchorX;
    double m_anchorY;
    double m_displacementX;
    double m_displacementY;
    double m_rotation;
    bool m_autoRotation;
    double m_maxDisplacement;
    double m_repeatDistance;
    double m_minPadding;
};

}
}

#endif
