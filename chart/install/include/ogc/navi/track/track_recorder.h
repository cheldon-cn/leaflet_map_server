#pragma once

#include "ogc/navi/track/track.h"
#include "ogc/navi/positioning/position_manager.h"
#include "ogc/navi/export.h"
#include <memory>
#include <functional>

namespace ogc {
namespace navi {

struct RecordingConfig {
    double min_interval_seconds;
    double min_distance_meters;
    bool auto_pause;
    double auto_pause_speed;
    bool record_hdop;
    bool record_satellites;
    
    RecordingConfig()
        : min_interval_seconds(10.0)
        , min_distance_meters(50.0)
        , auto_pause(true)
        , auto_pause_speed(0.5)
        , record_hdop(true)
        , record_satellites(true)
    {}
};

using TrackPointRecordedCallback = std::function<void(const TrackPointData&)>;

class OGC_NAVI_API TrackRecorder {
public:
    static TrackRecorder& Instance();
    
    bool Initialize();
    void Shutdown();
    
    Track* StartRecording(const std::string& name);
    void StopRecording();
    void PauseRecording();
    void ResumeRecording();
    
    bool IsRecording() const;
    Track* GetCurrentTrack() const;
    
    void SetConfig(const RecordingConfig& config);
    RecordingConfig GetConfig() const;
    
    void SetTrackPointRecordedCallback(TrackPointRecordedCallback callback);
    
private:
    TrackRecorder();
    ~TrackRecorder();
    TrackRecorder(const TrackRecorder&) = delete;
    TrackRecorder& operator=(const TrackRecorder&) = delete;
    
    void OnPositionUpdate(const PositionData& position);
    bool ShouldRecordPoint(const PositionData& position);
    
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}
}
