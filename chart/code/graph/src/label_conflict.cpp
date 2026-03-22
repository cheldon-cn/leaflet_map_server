#include "ogc/draw/label_conflict.h"
#include <algorithm>

namespace ogc {
namespace draw {

LabelConflictResolver::LabelConflictResolver()
    : m_minDistance(0)
    , m_allowOverlap(false)
    , m_priorityMode("priority") {
}

void LabelConflictResolver::SetMinDistance(double distance) {
    m_minDistance = distance;
}

double LabelConflictResolver::GetMinDistance() const {
    return m_minDistance;
}

void LabelConflictResolver::SetAllowOverlap(bool allow) {
    m_allowOverlap = allow;
}

bool LabelConflictResolver::GetAllowOverlap() const {
    return m_allowOverlap;
}

void LabelConflictResolver::SetPriorityMode(const std::string& mode) {
    m_priorityMode = mode;
}

std::string LabelConflictResolver::GetPriorityMode() const {
    return m_priorityMode;
}

Envelope LabelConflictResolver::GetLabelBounds(const LabelInfo& label) const {
    double halfWidth = label.width / 2.0 + m_minDistance;
    double halfHeight = label.height / 2.0 + m_minDistance;
    
    return Envelope(
        label.x - halfWidth,
        label.y - halfHeight,
        label.x + halfWidth,
        label.y + halfHeight
    );
}

bool LabelConflictResolver::BoundsOverlap(const Envelope& env1, const Envelope& env2) const {
    return env1.Intersects(env2);
}

double LabelConflictResolver::CalculateOverlapRatio(const LabelInfo& label1, const LabelInfo& label2) const {
    Envelope env1 = GetLabelBounds(label1);
    Envelope env2 = GetLabelBounds(label2);
    
    if (!BoundsOverlap(env1, env2)) {
        return 0.0;
    }
    
    double x1 = std::max(env1.GetMinX(), env2.GetMinX());
    double y1 = std::max(env1.GetMinY(), env2.GetMinY());
    double x2 = std::min(env1.GetMaxX(), env2.GetMaxX());
    double y2 = std::min(env1.GetMaxY(), env2.GetMaxY());
    
    double overlapWidth = std::max(0.0, x2 - x1);
    double overlapHeight = std::max(0.0, y2 - y1);
    double overlapArea = overlapWidth * overlapHeight;
    
    double area1 = env1.GetWidth() * env1.GetHeight();
    double area2 = env2.GetWidth() * env2.GetHeight();
    
    if (area1 <= 0 || area2 <= 0) {
        return 0.0;
    }
    
    return overlapArea / std::min(area1, area2);
}

bool LabelConflictResolver::HasConflict(const LabelInfo& label1, const LabelInfo& label2) const {
    if (m_allowOverlap) {
        return false;
    }
    
    if (!label1.visible || !label2.visible) {
        return false;
    }
    
    Envelope env1 = GetLabelBounds(label1);
    Envelope env2 = GetLabelBounds(label2);
    
    return BoundsOverlap(env1, env2);
}

double LabelConflictResolver::CalculateOverlapArea(const LabelInfo& label1, const LabelInfo& label2) const {
    Envelope env1 = GetLabelBounds(label1);
    Envelope env2 = GetLabelBounds(label2);
    
    if (!BoundsOverlap(env1, env2)) {
        return 0.0;
    }
    
    double x1 = std::max(env1.GetMinX(), env2.GetMinX());
    double y1 = std::max(env1.GetMinY(), env2.GetMinY());
    double x2 = std::min(env1.GetMaxX(), env2.GetMaxX());
    double y2 = std::min(env1.GetMaxY(), env2.GetMaxY());
    
    return (x2 - x1) * (y2 - y1);
}

std::vector<ConflictInfo> LabelConflictResolver::DetectConflicts(const std::vector<LabelInfo>& labels) {
    std::vector<ConflictInfo> conflicts;
    
    for (size_t i = 0; i < labels.size(); ++i) {
        for (size_t j = i + 1; j < labels.size(); ++j) {
            if (HasConflict(labels[i], labels[j])) {
                ConflictInfo info;
                info.labelIndex1 = static_cast<int>(i);
                info.labelIndex2 = static_cast<int>(j);
                info.overlapArea = CalculateOverlapArea(labels[i], labels[j]);
                info.resolved = false;
                conflicts.push_back(info);
            }
        }
    }
    
    return conflicts;
}

bool LabelConflictResolver::OnConflictDetected(const LabelInfo& label1, const LabelInfo& label2) {
    (void)label1;
    (void)label2;
    return true;
}

int LabelConflictResolver::SelectLabelToKeep(const LabelInfo& label1, const LabelInfo& label2) {
    if (m_priorityMode == "priority") {
        if (label1.priority != label2.priority) {
            return label1.priority > label2.priority ? 0 : 1;
        }
    }
    
    if (m_priorityMode == "size") {
        double area1 = label1.width * label1.height;
        double area2 = label2.width * label2.height;
        if (area1 != area2) {
            return area1 > area2 ? 0 : 1;
        }
    }
    
    return 0;
}

std::vector<LabelInfo> LabelConflictResolver::ResolveConflicts(const std::vector<LabelInfo>& labels) {
    if (m_allowOverlap) {
        return labels;
    }
    
    std::vector<LabelInfo> result = labels;
    auto conflicts = DetectConflicts(result);
    
    bool changed = true;
    int maxIterations = 100;
    int iteration = 0;
    
    while (changed && iteration < maxIterations) {
        changed = false;
        iteration++;
        
        conflicts = DetectConflicts(result);
        
        for (const auto& conflict : conflicts) {
            if (conflict.resolved) {
                continue;
            }
            
            int idx1 = conflict.labelIndex1;
            int idx2 = conflict.labelIndex2;
            
            if (!result[idx1].visible || !result[idx2].visible) {
                continue;
            }
            
            OnConflictDetected(result[idx1], result[idx2]);
            
            int keepIdx = SelectLabelToKeep(result[idx1], result[idx2]);
            int hideIdx = (keepIdx == 0) ? idx2 : idx1;
            
            result[hideIdx].visible = false;
            changed = true;
        }
    }
    
    return result;
}

void LabelConflictResolver::Clear() {
}

LabelConflictResolverPtr LabelConflictResolver::Create() {
    return std::make_shared<LabelConflictResolver>();
}

SpatialIndexConflictResolver::SpatialIndexConflictResolver()
    : LabelConflictResolver() {
}

void SpatialIndexConflictResolver::BuildSpatialIndex(const std::vector<LabelInfo>& labels) {
    (void)labels;
}

std::vector<int> SpatialIndexConflictResolver::QueryNearbyLabels(const LabelInfo& label) {
    (void)label;
    return std::vector<int>();
}

std::vector<ConflictInfo> SpatialIndexConflictResolver::DetectConflictsOptimized(const std::vector<LabelInfo>& labels) {
    return DetectConflicts(labels);
}

std::shared_ptr<SpatialIndexConflictResolver> SpatialIndexConflictResolver::Create() {
    return std::make_shared<SpatialIndexConflictResolver>();
}

}
}
