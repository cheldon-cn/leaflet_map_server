#include "ogc/navi/track/track_player.h"
#include <cmath>
#include <algorithm>

namespace ogc {
namespace navi {

std::unique_ptr<TrackPlayer> TrackPlayer::Create() {
    return std::unique_ptr<TrackPlayer>(new TrackPlayer());
}

TrackPlayer::TrackPlayer() {
}

TrackPlayer::~TrackPlayer() {
}

void TrackPlayer::SetTrack(const TrackPlaybackData& track) {
    m_track = track;
    m_progress = 0.0;
    m_currentPointIndex = 0;
    m_elapsedTime = 0.0;
    m_state = PlaybackState::kStopped;
}

void TrackPlayer::SetTrack(TrackPlaybackData&& track) {
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
        PlaybackTrackPoint point = GetCurrentPoint();
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

PlaybackTrackPoint TrackPlayer::GetCurrentPoint() const {
    if (m_track.IsEmpty() || m_currentPointIndex >= m_track.GetPointCount()) {
        return PlaybackTrackPoint();
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
    
    return PlaybackTrackPoint();
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
    
    PlaybackTrackPoint currentPoint = GetCurrentPoint();
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

void TrackPlayer::NotifyProgress(const PlaybackTrackPoint& point) {
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
