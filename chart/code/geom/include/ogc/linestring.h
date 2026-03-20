#pragma once

/**
 * @file linestring.h
 * @brief 线串几何类定义
 */

#include "geometry.h"
#include <vector>

namespace ogc {

/**
 * @brief 线串几何类
 * 
 * 由一系列连续的坐标点组成的线
 */
class OGC_API LineString : public Geometry {
public:
    static LineStringPtr Create();
    static LineStringPtr Create(const CoordinateList& coords);
    static LineStringPtr Create(CoordinateList&& coords);
    
    GeomType GetGeometryType() const noexcept override { return GeomType::kLineString; }
    const char* GetGeometryTypeString() const noexcept override { return "LINESTRING"; }
    Dimension GetDimension() const noexcept override { return Dimension::Curve; }
    int GetCoordinateDimension() const noexcept override;
    
    bool IsEmpty() const noexcept override { return m_coords.empty(); }
    bool Is3D() const noexcept override;
    bool IsMeasured() const noexcept override;
    bool IsClosed() const noexcept;
    bool IsRing() const noexcept;
    
    size_t GetNumPoints() const noexcept { return m_coords.size(); }
    size_t GetNumCoordinates() const noexcept override { return m_coords.size(); }
    
    Coordinate GetCoordinateN(size_t index) const override;
    CoordinateList GetCoordinates() const override { return m_coords; }
    
    Coordinate GetPointN(size_t index) const;
    Coordinate GetStartPoint() const;
    Coordinate GetEndPoint() const;
    
    void SetCoordinates(const CoordinateList& coords);
    void SetCoordinates(CoordinateList&& coords);
    
    void AddPoint(const Coordinate& coord);
    void InsertPoint(size_t index, const Coordinate& coord);
    void RemovePoint(size_t index);
    void Clear();
    
    struct Segment {
        Coordinate start, end;
        double length;
    };
    Segment GetSegment(size_t index) const;
    size_t GetNumSegments() const noexcept;
    
    double Length() const override;
    double Length3D() const;
    std::vector<double> GetCumulativeLength() const;
    double GetSegmentLengthPercentage(size_t index) const;
    
    Coordinate Interpolate(double distance) const;
    double LocatePoint(const Coordinate& point) const;
    LineStringPtr GetSubLine(double startDistance, double endDistance) const;
    
    std::pair<LineStringPtr, LineStringPtr> Split(const Coordinate& point) const;
    std::vector<LineStringPtr> SplitAtDistances(const std::vector<double>& distances) const;
    
    LineStringPtr Offset(double distance) const;
    
    double GetCurvatureAt(size_t index) const;
    double GetTotalCurvature() const;
    double GetBearingAt(size_t index) const;
    
    void RemoveDuplicatePoints(double tolerance = DEFAULT_TOLERANCE);
    LineStringPtr Smooth(int iterations = 1) const;
    bool IsStraight(double tolerance = DEFAULT_TOLERANCE) const;
    
    bool GetCoordinateData(const double** data, size_t* count) const override {
        if (m_coords.empty()) {
            *data = nullptr;
            *count = 0;
            return false;
        }
        *data = reinterpret_cast<const double*>(m_coords.data());
        *count = m_coords.size() * 4;
        return true;
    }
    
    CoordinateList::const_iterator begin() const { return m_coords.begin(); }
    CoordinateList::const_iterator end() const { return m_coords.end(); }
    
    std::string AsText(int precision = DEFAULT_WKT_PRECISION) const override;
    std::vector<uint8_t> AsBinary() const override;
    GeometryPtr Clone() const override;
    GeometryPtr CloneEmpty() const override;
    
    void Apply(GeometryVisitor& visitor) override;
    void Apply(GeometryConstVisitor& visitor) const override;
    
    double PointToLineDistance(const Coordinate& point, const Coordinate& lineStart, const Coordinate& lineEnd) const;
    
protected:
    LineString() = default;
    explicit LineString(const CoordinateList& coords) : m_coords(coords) {}
    
    CoordinateList m_coords;
    
    Envelope ComputeEnvelope() const override;
    Coordinate ComputeCentroid() const override;
    
    double ProjectPointOnSegment(const Coordinate& point, const Coordinate& segStart, const Coordinate& segEnd) const;
    Coordinate InterpolateOnSegment(double t, const Coordinate& segStart, const Coordinate& segEnd) const;
};

}
