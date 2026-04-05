#include "ogc/navi/track/track_recorder.h"
#include "ogc/navi/track/track_point.h"
#include "ogc/navi/navigation/navigation_calculator.h"
#include <cmath>
#include <map>

namespace ogc {
namespace navi {

struct TrackRecorder::Impl {
    Track* current_track = nullptr;
    RecordingConfig config;
    bool is_recording = false;
    bool is_paused = false;
    bool initialized = false;
    double last_record_time = 0.0;
    GeoPoint last_record_position;
    TrackPointRecordedCallback callback;
};

TrackRecorder::TrackRecorder()
    : impl_(new Impl())
{
}

TrackRecorder::~TrackRecorder() {
    Shutdown();
}

TrackRecorder& TrackRecorder::Instance() {
    static TrackRecorder instance;
    return instance;
}

bool TrackRecorder::Initialize() {
    if (impl_->initialized) {
        return true;
    }
    
    impl_->initialized = true;
    return true;
}

void TrackRecorder::Shutdown() {
    StopRecording();
    impl_->initialized = false;
}

Track* TrackRecorder::StartRecording(const std::string& name) {
    if (impl_->is_recording) {
        return nullptr;
    }
    
    impl_->current_track = Track::Create();
    impl_->current_track->SetName(name);
    impl_->current_track->SetType(TrackType::RealTime);
    
    impl_->is_recording = true;
    impl_->is_paused = false;
    impl_->last_record_time = 0.0;
    
    return impl_->current_track;
}

void TrackRecorder::StopRecording() {
    if (!impl_->is_recording) {
        return;
    }
    
    impl_->is_recording = false;
    impl_->is_paused = false;
}

void TrackRecorder::PauseRecording() {
    if (impl_->is_recording && !impl_->is_paused) {
        impl_->is_paused = true;
    }
}

void TrackRecorder::ResumeRecording() {
    if (impl_->is_recording && impl_->is_paused) {
        impl_->is_paused = false;
    }
}

bool TrackRecorder::IsRecording() const {
    return impl_->is_recording && !impl_->is_paused;
}

Track* TrackRecorder::GetCurrentTrack() const {
    return impl_->current_track;
}

void TrackRecorder::SetConfig(const RecordingConfig& config) {
    impl_->config = config;
}

RecordingConfig TrackRecorder::GetConfig() const {
    return impl_->config;
}

void TrackRecorder::SetTrackPointRecordedCallback(TrackPointRecordedCallback callback) {
    impl_->callback = callback;
}

void TrackRecorder::OnPositionUpdate(const PositionData& position) {
    if (!impl_->is_recording || impl_->is_paused) {
        return;
    }
    
    if (!ShouldRecordPoint(position)) {
        return;
    }
    
    if (!impl_->current_track) {
        return;
    }
    
    TrackPoint* point = TrackPoint::Create();
    point->SetPosition(position.longitude, position.latitude);
    point->SetTimestamp(position.timestamp);
    point->SetSpeed(position.speed);
    point->SetCourse(position.course);
    point->SetHeading(position.heading);
    point->SetAltitude(position.altitude);
    
    if (impl_->config.record_hdop) {
        point->SetHdop(position.hdop);
    }
    
    if (impl_->config.record_satellites) {
        point->SetSatelliteCount(position.satellite_count);
    }
    
    impl_->current_track->AddPoint(point);
    
    impl_->last_record_time = position.timestamp;
    impl_->last_record_position = GeoPoint(position.longitude, position.latitude);
    
    if (impl_->callback) {
        TrackPointData point_data = point->ToData();
        impl_->callback(point_data);
    }
}

bool TrackRecorder::ShouldRecordPoint(const PositionData& position) {
    if (impl_->config.auto_pause && 
        position.speed < impl_->config.auto_pause_speed) {
        return false;
    }
    
    if (impl_->last_record_time > 0.0) {
        double time_diff = position.timestamp - impl_->last_record_time;
        if (time_diff < impl_->config.min_interval_seconds) {
            return false;
        }
    }
    
    if (impl_->last_record_time > 0.0) {
        GeoPoint current_pos(position.longitude, position.latitude);
        double distance = NavigationCalculator::Instance().CalculateDistanceToWaypoint(
            impl_->last_record_position, current_pos);
        
        if (distance < impl_->config.min_distance_meters) {
            return false;
        }
    }
    
    return true;
}

}
}
