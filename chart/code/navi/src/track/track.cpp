#include "ogc/navi/track/track.h"
#include "ogc/navi/navigation/navigation_calculator.h"
#include "ogc/navi/positioning/coordinate_converter.h"
#include <algorithm>
#include <atomic>
#include <cmath>

namespace ogc {
namespace navi {

class TrackImpl : public Track {
public:
    TrackImpl()
        : type_(TrackType::RealTime)
        , start_time_(0.0)
        , end_time_(0.0)
        , total_distance_(0.0)
        , total_duration_(0.0)
        , ref_count_(1)
    {}
    
    explicit TrackImpl(const TrackData& data)
        : id_(data.id)
        , name_(data.name)
        , description_(data.description)
        , type_(data.type)
        , start_time_(data.start_time)
        , end_time_(data.end_time)
        , total_distance_(data.total_distance)
        , total_duration_(data.total_duration)
        , ref_count_(1)
    {
        for (const auto& pt_data : data.points) {
            TrackPoint* pt = TrackPoint::Create(pt_data);
            points_.push_back(pt);
        }
    }
    
    ~TrackImpl() override {
        for (auto* pt : points_) {
            pt->ReleaseReference();
        }
    }
    
    const std::string& GetId() const override { return id_; }
    void SetId(const std::string& id) override { id_ = id; }
    
    const std::string& GetName() const override { return name_; }
    void SetName(const std::string& name) override { name_ = name; }
    
    const std::string& GetDescription() const override { return description_; }
    void SetDescription(const std::string& desc) override { description_ = desc; }
    
    TrackType GetType() const override { return type_; }
    void SetType(TrackType type) override { type_ = type; }
    
    double GetStartTime() const override { return start_time_; }
    double GetEndTime() const override { return end_time_; }
    double GetTotalDistance() const override { return total_distance_; }
    double GetTotalDuration() const override { return total_duration_; }
    
    int GetPointCount() const override { return static_cast<int>(points_.size()); }
    
    TrackPoint* GetPoint(int index) override {
        if (index >= 0 && index < static_cast<int>(points_.size())) {
            return points_[index];
        }
        return nullptr;
    }
    
    const TrackPoint* GetPoint(int index) const override {
        if (index >= 0 && index < static_cast<int>(points_.size())) {
            return points_[index];
        }
        return nullptr;
    }
    
    void AddPoint(TrackPoint* point) override {
        if (!point) return;
        
        if (points_.empty()) {
            start_time_ = point->GetTimestamp();
        } else {
            const TrackPoint* last = points_.back();
            double distance = CoordinateConverter::Instance().CalculateGreatCircleDistance(
                last->GetLatitude(), last->GetLongitude(),
                point->GetLatitude(), point->GetLongitude());
            total_distance_ += distance;
        }
        
        points_.push_back(point);
        end_time_ = point->GetTimestamp();
        total_duration_ = end_time_ - start_time_;
    }
    
    void InsertPoint(int index, TrackPoint* point) override {
        if (!point || index < 0 || index > static_cast<int>(points_.size())) return;
        
        points_.insert(points_.begin() + index, point);
        UpdateDistances();
        UpdateTimeRange();
    }
    
    void RemovePoint(int index) override {
        if (index < 0 || index >= static_cast<int>(points_.size())) return;
        
        points_[index]->ReleaseReference();
        points_.erase(points_.begin() + index);
        UpdateDistances();
        UpdateTimeRange();
    }
    
    void ClearPoints() override {
        for (auto* pt : points_) {
            pt->ReleaseReference();
        }
        points_.clear();
        total_distance_ = 0.0;
        total_duration_ = 0.0;
        start_time_ = 0.0;
        end_time_ = 0.0;
    }
    
    void Simplify(double tolerance) override {
        if (points_.size() < 3) return;
        
        std::vector<TrackPoint*> simplified;
        simplified.push_back(points_[0]);
        
        for (size_t i = 1; i + 1 < points_.size(); ++i) {
            double distance = CalculatePerpendicularDistance(
                points_[i], points_[i - 1], points_[i + 1]);
            
            if (distance > tolerance) {
                simplified.push_back(points_[i]);
            } else {
                points_[i]->ReleaseReference();
            }
        }
        
        simplified.push_back(points_.back());
        points_ = simplified;
        UpdateDistances();
    }
    
    std::vector<TrackPoint*> GetPointsInTimeRange(double start, double end) override {
        std::vector<TrackPoint*> result;
        for (auto* pt : points_) {
            double ts = pt->GetTimestamp();
            if (ts >= start && ts <= end) {
                result.push_back(pt);
            }
        }
        return result;
    }
    
    std::vector<TrackPoint*> GetPointsInBoundingBox(const BoundingBox& bbox) override {
        std::vector<TrackPoint*> result;
        for (auto* pt : points_) {
            if (bbox.Contains(pt->GetLongitude(), pt->GetLatitude())) {
                result.push_back(pt);
            }
        }
        return result;
    }
    
    TrackData ToData() const override {
        TrackData data;
        data.id = id_;
        data.name = name_;
        data.description = description_;
        data.type = type_;
        data.start_time = start_time_;
        data.end_time = end_time_;
        data.total_distance = total_distance_;
        data.total_duration = total_duration_;
        data.point_count = static_cast<int>(points_.size());
        
        for (const auto* pt : points_) {
            data.points.push_back(pt->ToData());
        }
        
        return data;
    }
    
    void FromData(const TrackData& data) override {
        ClearPoints();
        
        id_ = data.id;
        name_ = data.name;
        description_ = data.description;
        type_ = data.type;
        start_time_ = data.start_time;
        end_time_ = data.end_time;
        total_distance_ = data.total_distance;
        total_duration_ = data.total_duration;
        
        for (const auto& pt_data : data.points) {
            TrackPoint* pt = TrackPoint::Create(pt_data);
            points_.push_back(pt);
        }
    }
    
    void ReleaseReference() override {
        if (--ref_count_ == 0) {
            delete this;
        }
    }
    
private:
    void UpdateDistances() {
        total_distance_ = 0.0;
        for (size_t i = 1; i < points_.size(); ++i) {
            total_distance_ += CoordinateConverter::Instance().CalculateGreatCircleDistance(
                points_[i - 1]->GetLatitude(), points_[i - 1]->GetLongitude(),
                points_[i]->GetLatitude(), points_[i]->GetLongitude());
        }
    }
    
    void UpdateTimeRange() {
        if (points_.empty()) {
            start_time_ = 0.0;
            end_time_ = 0.0;
            total_duration_ = 0.0;
            return;
        }
        
        start_time_ = points_.front()->GetTimestamp();
        end_time_ = points_.back()->GetTimestamp();
        total_duration_ = end_time_ - start_time_;
    }
    
    double CalculatePerpendicularDistance(TrackPoint* point,
                                          TrackPoint* line_start,
                                          TrackPoint* line_end) {
        double xtd = NavigationCalculator::Instance().CalculateCrossTrackError(
            GeoPoint(point->GetLongitude(), point->GetLatitude()),
            GeoPoint(line_start->GetLongitude(), line_start->GetLatitude()),
            GeoPoint(line_end->GetLongitude(), line_end->GetLatitude()));
        
        return std::abs(xtd);
    }
    
    std::string id_;
    std::string name_;
    std::string description_;
    TrackType type_;
    double start_time_;
    double end_time_;
    double total_distance_;
    double total_duration_;
    std::vector<TrackPoint*> points_;
    std::atomic<int> ref_count_;
};

Track* Track::Create() {
    return new TrackImpl();
}

Track* Track::Create(const TrackData& data) {
    return new TrackImpl(data);
}

}
}
