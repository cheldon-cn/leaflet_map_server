#ifndef OGC_GRAPH_LABEL_CONFLICT_H
#define OGC_GRAPH_LABEL_CONFLICT_H

#include "ogc/graph/export.h"
#include "ogc/graph/label/label_info.h"
#include "ogc/geom/envelope.h"
#include <memory>
#include <vector>
#include <functional>

namespace ogc {
namespace graph {

struct ConflictInfo {
    int labelIndex1;
    int labelIndex2;
    double overlapArea;
    bool resolved;
    
    ConflictInfo()
        : labelIndex1(-1), labelIndex2(-1)
        , overlapArea(0), resolved(false) {}
};

class LabelConflictResolver;
typedef std::shared_ptr<LabelConflictResolver> LabelConflictResolverPtr;

class OGC_GRAPH_API LabelConflictResolver {
public:
    LabelConflictResolver();
    virtual ~LabelConflictResolver() = default;
    
    void SetMinDistance(double distance);
    double GetMinDistance() const;
    
    void SetAllowOverlap(bool allow);
    bool GetAllowOverlap() const;
    
    void SetPriorityMode(const std::string& mode);
    std::string GetPriorityMode() const;
    
    std::vector<ConflictInfo> DetectConflicts(const std::vector<LabelInfo>& labels);
    
    std::vector<LabelInfo> ResolveConflicts(const std::vector<LabelInfo>& labels);
    
    bool HasConflict(const LabelInfo& label1, const LabelInfo& label2) const;
    
    double CalculateOverlapArea(const LabelInfo& label1, const LabelInfo& label2) const;
    
    void Clear();
    
    static LabelConflictResolverPtr Create();

protected:
    virtual bool OnConflictDetected(const LabelInfo& label1, const LabelInfo& label2);
    virtual int SelectLabelToKeep(const LabelInfo& label1, const LabelInfo& label2);
    
private:
    Envelope GetLabelBounds(const LabelInfo& label) const;
    bool BoundsOverlap(const Envelope& env1, const Envelope& env2) const;
    double CalculateOverlapRatio(const LabelInfo& label1, const LabelInfo& label2) const;
    
    double m_minDistance;
    bool m_allowOverlap;
    std::string m_priorityMode;
};

class OGC_GRAPH_API SpatialIndexConflictResolver : public LabelConflictResolver {
public:
    SpatialIndexConflictResolver();
    ~SpatialIndexConflictResolver() override = default;
    
    std::vector<ConflictInfo> DetectConflictsOptimized(const std::vector<LabelInfo>& labels);
    
    static std::shared_ptr<SpatialIndexConflictResolver> Create();

private:
    void BuildSpatialIndex(const std::vector<LabelInfo>& labels);
    std::vector<int> QueryNearbyLabels(const LabelInfo& label);
};

}
}

#endif
