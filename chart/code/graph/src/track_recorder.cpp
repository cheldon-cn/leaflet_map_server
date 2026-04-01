#include "ogc/draw/track_recorder.h"
#include <cmath>
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace ogc {
namespace draw {

std::unique_ptr<TrackRecorder> TrackRecorder::Create() {
    return std::unique_ptr<TrackRecorder>(new TrackRecorder());
}

TrackRecorder::TrackRecorder() {
}

TrackRecorder::~TrackRecorder() {
}

void TrackRecorder::StartRecording(const std::string& trackName) {
    m_isRecording = true;
    m_isPaused = false;
    m_hasLastPoint = false;
    
    m_currentTrack = TrackData();
    m_currentTrack.id = std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    m_currentTrack.name = trackName.empty() ? "Track_" + m_currentTrack.id : trackName;
    m_currentTrack.startTime = std::chrono::system_clock::now();
    
    TrackSegment segment;
    segment.startTime = m_currentTrack.startTime;
    m_currentTrack.segments.push_back(segment);
}

void TrackRecorder::StopRecording() {
    if (!m_isRecording) {
        return;
    }
    
    m_isRecording = false;
    m_isPaused = false;
    
    if (!m_currentTrack.segments.empty()) {
        m_currentTrack.segments.back().endTime = std::chrono::system_clock::now();
        m_currentTrack.endTime = m_currentTrack.segments.back().endTime;
    }
    
    UpdateStatistics();
    NotifyTrackUpdated();
}

void TrackRecorder::PauseRecording() {
    if (m_isRecording && !m_isPaused) {
        m_isPaused = true;
        
        if (!m_currentTrack.segments.empty()) {
            m_currentTrack.segments.back().endTime = std::chrono::system_clock::now();
        }
    }
}

void TrackRecorder::ResumeRecording() {
    if (m_isRecording && m_isPaused) {
        m_isPaused = false;
        
        TrackSegment newSegment;
        newSegment.startTime = std::chrono::system_clock::now();
        m_currentTrack.segments.push_back(newSegment);
    }
}

void TrackRecorder::AddPoint(const Coordinate& position, double heading, double speed) {
    TrackPoint point;
    point.position = position;
    point.heading = heading;
    point.speed = speed;
    point.timestamp = std::chrono::system_clock::now();
    AddPoint(point);
}

void TrackRecorder::AddPoint(const TrackPoint& point) {
    if (!m_isRecording || m_isPaused) {
        return;
    }
    
    if (!ShouldRecordPoint(point)) {
        return;
    }
    
    if (m_currentTrack.segments.empty()) {
        TrackSegment segment;
        segment.startTime = point.timestamp;
        m_currentTrack.segments.push_back(segment);
    }
    
    TrackPoint recordedPoint = point;
    
    if (m_hasLastPoint) {
        double dx = point.position.x - m_lastRecordedPoint.position.x;
        double dy = point.position.y - m_lastRecordedPoint.position.y;
        recordedPoint.distance = std::sqrt(dx * dx + dy * dy);
        
        auto duration = std::chrono::duration<double>(point.timestamp - m_lastRecordedPoint.timestamp);
        recordedPoint.duration = duration.count();
    }
    
    m_currentTrack.segments.back().points.push_back(recordedPoint);
    m_lastRecordedPoint = recordedPoint;
    m_hasLastPoint = true;
    
    UpdateStatistics();
    NotifyPointRecorded(recordedPoint);
    NotifyTrackUpdated();
}

TrackData TrackRecorder::GetCurrentTrack() const {
    return m_currentTrack;
}

void TrackRecorder::ClearCurrentTrack() {
    m_currentTrack = TrackData();
    m_hasLastPoint = false;
}

void TrackRecorder::SetTrackConfig(const TrackConfig& config) {
    m_config = config;
}

void TrackRecorder::SetTrackUpdatedCallback(TrackUpdatedCallback callback) {
    m_trackUpdatedCallback = callback;
}

void TrackRecorder::SetPointRecordedCallback(PointRecordedCallback callback) {
    m_pointRecordedCallback = callback;
}

size_t TrackRecorder::GetPointCount() const {
    return m_currentTrack.GetPointCount();
}

double TrackRecorder::GetTotalDistance() const {
    return m_currentTrack.totalDistance;
}

double TrackRecorder::GetTotalDuration() const {
    return m_currentTrack.totalDuration;
}

bool TrackRecorder::ShouldRecordPoint(const TrackPoint& point) const {
    if (!m_hasLastPoint) {
        return true;
    }
    
    double dx = point.position.x - m_lastRecordedPoint.position.x;
    double dy = point.position.y - m_lastRecordedPoint.position.y;
    double distance = std::sqrt(dx * dx + dy * dy);
    
    if (distance < m_config.minDistanceInterval) {
        return false;
    }
    
    if (distance > m_config.maxDistanceInterval) {
        return true;
    }
    
    auto elapsed = std::chrono::duration<double>(point.timestamp - m_lastRecordedPoint.timestamp);
    if (elapsed.count() < m_config.minTimeInterval) {
        return false;
    }
    
    return true;
}

void TrackRecorder::UpdateStatistics() {
    double totalDist = 0.0;
    double totalDur = 0.0;
    
    for (auto& segment : m_currentTrack.segments) {
        double segDist = 0.0;
        double segDur = 0.0;
        
        for (const auto& point : segment.points) {
            segDist += point.distance;
            segDur += point.duration;
        }
        
        segment.totalDistance = segDist;
        segment.totalDuration = segDur;
        totalDist += segDist;
        totalDur += segDur;
    }
    
    m_currentTrack.totalDistance = totalDist;
    m_currentTrack.totalDuration = totalDur;
}

void TrackRecorder::NotifyTrackUpdated() {
    if (m_trackUpdatedCallback) {
        m_trackUpdatedCallback(m_currentTrack);
    }
}

void TrackRecorder::NotifyPointRecorded(const TrackPoint& point) {
    if (m_pointRecordedCallback) {
        m_pointRecordedCallback(point);
    }
}

TrackData TrackRecorder::LoadTrack(const std::string& filePath) {
    TrackData track;
    return track;
}

bool TrackRecorder::SaveTrack(const TrackData& track, const std::string& filePath) {
    return false;
}

TrackData TrackRecorder::CompressTrack(const TrackData& track, double tolerance) {
    TrackData compressed = track;
    return compressed;
}

std::unique_ptr<TrackPlayer> TrackPlayer::Create() {
    return std::unique_ptr<TrackPlayer>(new TrackPlayer());
}

TrackPlayer::TrackPlayer() {
}

TrackPlayer::~TrackPlayer() {
}

void TrackPlayer::SetTrack(const TrackData& track) {
    m_track = track;
    m_progress = 0.0;
    m_currentPointIndex = 0;
    m_elapsedTime = 0.0;
    m_state = PlaybackState::kStopped;
}

void TrackPlayer::SetTrack(TrackData&& track) {
    m_track = std::move(track);
    m_progress = 0.0;
    m_currentPointIndex = 0;
    m_elapsedTime = 0.0;
    m_state = PlaybackState::kStopped;
}

void TrackPlayer::Play() {
    if (m_track.IsEmpty()) {
        return;
    }
    
    if (m_state == PlaybackState::kStopped) {
        m_progress = 0.0;
        m_currentPointIndex = 0;
        m_elapsedTime = 0.0;
    }
    
    m_state = PlaybackState::kPlaying;
    NotifyStateChanged(m_state);
}

void TrackPlayer::Pause() {
    if (m_state == PlaybackState::kPlaying) {
        m_state = PlaybackState::kPaused;
        NotifyStateChanged(m_state);
    }
}

void TrackPlayer::Stop() {
    m_state = PlaybackState::kStopped;
    m_progress = 0.0;
    m_currentPointIndex = 0;
    m_elapsedTime = 0.0;
    NotifyStateChanged(m_state);
}

void TrackPlayer::Seek(double progress) {
    progress = std::max(0.0, std::min(1.0, progress));
    
    size_t totalPoints = m_track.GetPointCount();
    if (totalPoints == 0) {
        return;
    }
    
    m_currentPointIndex = static_cast<size_t>(progress * (totalPoints - 1));
    m_progress = progress;
    UpdateProgress();
}

void TrackPlayer::SeekToTime(const std::chrono::system_clock::time_point& time) {
    size_t totalPoints = m_track.GetPointCount();
    if (totalPoints == 0) {
        return;
    }
    
    for (size_t i = 0; i < totalPoints; ++i) {
        TrackPoint point = GetCurrentPoint();
        if (point.timestamp >= time) {
            m_currentPointIndex = i;
            UpdateProgress();
            return;
        }
    }
}

void TrackPlayer::SeekToPoint(size_t pointIndex) {
    size_t totalPoints = m_track.GetPointCount();
    if (totalPoints == 0 || pointIndex >= totalPoints) {
        return;
    }
    
    m_currentPointIndex = pointIndex;
    UpdateProgress();
}

void TrackPlayer::SetPlaybackSpeed(double speed) {
    m_playbackSpeed = std::max(0.1, std::min(10.0, speed));
}

void TrackPlayer::SetPlaybackMode(PlaybackMode mode) {
    m_playbackMode = mode;
}

TrackPoint TrackPlayer::GetCurrentPoint() const {
    if (m_track.IsEmpty() || m_currentPointIndex >= m_track.GetPointCount()) {
        return TrackPoint();
    }
    
    size_t currentIdx = 0;
    for (const auto& segment : m_track.segments) {
        for (const auto& point : segment.points) {
            if (currentIdx == m_currentPointIndex) {
                return point;
            }
            ++currentIdx;
        }
    }
    
    return TrackPoint();
}

double TrackPlayer::GetElapsedTime() const {
    return m_elapsedTime;
}

double TrackPlayer::GetRemainingTime() const {
    return m_track.totalDuration - m_elapsedTime;
}

void TrackPlayer::SetLoopEnabled(bool enabled) {
    m_loopEnabled = enabled;
}

void TrackPlayer::SetProgressCallback(PlaybackProgressCallback callback) {
    m_progressCallback = callback;
}

void TrackPlayer::SetStateChangedCallback(PlaybackStateChangedCallback callback) {
    m_stateChangedCallback = callback;
}

void TrackPlayer::SetFinishedCallback(PlaybackFinishedCallback callback) {
    m_finishedCallback = callback;
}

void TrackPlayer::SetPositionCallback(std::function<void(const Coordinate&, double, double)> callback) {
    m_positionCallback = callback;
}

void TrackPlayer::Update(double deltaTime) {
    if (m_state != PlaybackState::kPlaying) {
        return;
    }
    
    AdvancePlayback(deltaTime);
}

void TrackPlayer::AdvancePlayback(double deltaTime) {
    double adjustedDelta = deltaTime * m_playbackSpeed;
    
    switch (m_playbackMode) {
        case PlaybackMode::kRealtime:
            m_elapsedTime += adjustedDelta;
            break;
        case PlaybackMode::kFastForward:
            m_elapsedTime += adjustedDelta * 2.0;
            break;
        case PlaybackMode::kSlowMotion:
            m_elapsedTime += adjustedDelta * 0.5;
            break;
        case PlaybackMode::kStepByStep:
            m_elapsedTime += adjustedDelta;
            break;
    }
    
    size_t totalPoints = m_track.GetPointCount();
    if (totalPoints == 0) {
        return;
    }
    
    if (m_elapsedTime >= m_track.totalDuration) {
        if (m_loopEnabled) {
            m_elapsedTime = 0.0;
            m_currentPointIndex = 0;
        } else {
            m_elapsedTime = m_track.totalDuration;
            m_state = PlaybackState::kStopped;
            NotifyFinished();
            NotifyStateChanged(m_state);
            return;
        }
    }
    
    m_progress = m_track.totalDuration > 0 ? m_elapsedTime / m_track.totalDuration : 0.0;
    
    size_t newIndex = static_cast<size_t>(m_progress * (totalPoints - 1));
    newIndex = std::min(newIndex, totalPoints - 1);
    
    if (newIndex != m_currentPointIndex) {
        m_currentPointIndex = newIndex;
        UpdateProgress();
    }
    
    TrackPoint currentPoint = GetCurrentPoint();
    NotifyProgress(currentPoint);
    
    if (m_positionCallback) {
        m_positionCallback(currentPoint.position, currentPoint.heading, currentPoint.speed);
    }
}

void TrackPlayer::UpdateProgress() {
    size_t totalPoints = m_track.GetPointCount();
    if (totalPoints > 0) {
        m_progress = static_cast<double>(m_currentPointIndex) / (totalPoints - 1);
    }
}

void TrackPlayer::NotifyProgress(const TrackPoint& point) {
    if (m_progressCallback) {
        m_progressCallback(point, m_progress);
    }
}

void TrackPlayer::NotifyStateChanged(PlaybackState state) {
    if (m_stateChangedCallback) {
        m_stateChangedCallback(state);
    }
}

void TrackPlayer::NotifyFinished() {
    if (m_finishedCallback) {
        m_finishedCallback();
    }
}

}  
}  
