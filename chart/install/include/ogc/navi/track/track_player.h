#ifndef OGC_NAVI_TRACK_PLAYER_H
#define OGC_NAVI_TRACK_PLAYER_H

#include "ogc/navi/export.h"
#include "ogc/navi/track/track.h"
#include "ogc/coordinate.h"
#include "ogc/envelope.h"
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <limits>
#include <map>

namespace ogc {
namespace navi {

struct PlaybackTrackPoint {
    Coordinate position;
    double heading = 0.0;
    double speed = 0.0;
    std::chrono::system_clock::time_point timestamp;
    double distance = 0.0;
    double duration = 0.0;
};

struct PlaybackTrackSegment {
    std::vector<PlaybackTrackPoint> points;
    double totalDistance = 0.0;
    double totalDuration = 0.0;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
};

struct TrackPlaybackData {
    std::string id;
    std::string name;
    std::vector<PlaybackTrackSegment> segments;
    double totalDistance = 0.0;
    double totalDuration = 0.0;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    std::string description;
    std::map<std::string, std::string> metadata;
    
    bool IsEmpty() const { return segments.empty(); }
    
    size_t GetPointCount() const {
        size_t count = 0;
        for (const auto& segment : segments) {
            count += segment.points.size();
        }
        return count;
    }
    
    Envelope GetBoundingBox() const {
        if (IsEmpty()) {
            return Envelope();
        }
        
        double minX = std::numeric_limits<double>::max();
        double minY = std::numeric_limits<double>::max();
        double maxX = std::numeric_limits<double>::lowest();
        double maxY = std::numeric_limits<double>::lowest();
        
        for (const auto& segment : segments) {
            for (const auto& point : segment.points) {
                minX = std::min(minX, point.position.x);
                minY = std::min(minY, point.position.y);
                maxX = std::max(maxX, point.position.x);
                maxY = std::max(maxY, point.position.y);
            }
        }
        
        return Envelope(minX, minY, maxX, maxY);
    }
};

class OGC_NAVI_API TrackPlayer {
public:
    enum class PlaybackState {
        kStopped,
        kPlaying,
        kPaused
    };
    
    enum class PlaybackMode {
        kRealtime,
        kFastForward,
        kSlowMotion,
        kStepByStep
    };
    
    using PlaybackProgressCallback = std::function<void(const PlaybackTrackPoint&, double progress)>;
    using PlaybackStateChangedCallback = std::function<void(PlaybackState)>;
    using PlaybackFinishedCallback = std::function<void()>;
    
    static std::unique_ptr<TrackPlayer> Create();
    
    ~TrackPlayer();
    
    void SetTrack(const TrackPlaybackData& track);
    void SetTrack(TrackPlaybackData&& track);
    const TrackPlaybackData& GetTrack() const { return m_track; }
    
    void Play();
    void Pause();
    void Stop();
    void Seek(double progress);
    void SeekToTime(const std::chrono::system_clock::time_point& time);
    void SeekToPoint(size_t pointIndex);
    
    void SetPlaybackSpeed(double speed);
    double GetPlaybackSpeed() const { return m_playbackSpeed; }
    
    void SetPlaybackMode(PlaybackMode mode);
    PlaybackMode GetPlaybackMode() const { return m_playbackMode; }
    
    PlaybackState GetState() const { return m_state; }
    bool IsPlaying() const { return m_state == PlaybackState::kPlaying; }
    bool IsPaused() const { return m_state == PlaybackState::kPaused; }
    bool IsStopped() const { return m_state == PlaybackState::kStopped; }
    
    double GetProgress() const { return m_progress; }
    size_t GetCurrentPointIndex() const { return m_currentPointIndex; }
    PlaybackTrackPoint GetCurrentPoint() const;
    
    double GetElapsedTime() const;
    double GetRemainingTime() const;
    
    void SetLoopEnabled(bool enabled);
    bool IsLoopEnabled() const { return m_loopEnabled; }
    
    void SetProgressCallback(PlaybackProgressCallback callback);
    void SetStateChangedCallback(PlaybackStateChangedCallback callback);
    void SetFinishedCallback(PlaybackFinishedCallback callback);
    
    void Update(double deltaTime);
    
    void SetPositionCallback(std::function<void(const Coordinate&, double, double)> callback);
    
private:
    TrackPlayer();
    
    void AdvancePlayback(double deltaTime);
    void UpdateProgress();
    void NotifyProgress(const PlaybackTrackPoint& point);
    void NotifyStateChanged(PlaybackState state);
    void NotifyFinished();
    
    TrackPlaybackData m_track;
    PlaybackState m_state = PlaybackState::kStopped;
    PlaybackMode m_playbackMode = PlaybackMode::kRealtime;
    
    double m_playbackSpeed = 1.0;
    double m_progress = 0.0;
    size_t m_currentPointIndex = 0;
    double m_elapsedTime = 0.0;
    
    bool m_loopEnabled = false;
    
    PlaybackProgressCallback m_progressCallback;
    PlaybackStateChangedCallback m_stateChangedCallback;
    PlaybackFinishedCallback m_finishedCallback;
    std::function<void(const Coordinate&, double, double)> m_positionCallback;
};

}
}

#endif
