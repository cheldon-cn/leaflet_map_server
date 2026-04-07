#include "ogc/graph/interaction/location_display_handler.h"
#include "ogc/graph/interaction/interaction_feedback.h"
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace ogc {
namespace graph {

std::unique_ptr<LocationDisplayHandler> LocationDisplayHandler::Create(const std::string& name) {
    return std::unique_ptr<LocationDisplayHandler>(new LocationDisplayHandler(name));
}

LocationDisplayHandler::LocationDisplayHandler(const std::string& name)
    : m_name(name)
{
    m_position.timestamp = std::chrono::system_clock::now();
    m_smoothedPosition.timestamp = std::chrono::system_clock::now();
}

LocationDisplayHandler::~LocationDisplayHandler() {
    ClearLocationFeedback();
}

void LocationDisplayHandler::SetEnabled(bool enabled) {
    m_enabled = enabled;
    if (!enabled) {
        ClearLocationFeedback();
    } else if (m_hasValidPosition) {
        UpdateLocationFeedback();
    }
}

bool LocationDisplayHandler::HandleEvent(const InteractionEvent& event) {
    if (!m_enabled) {
        return false;
    }
    
    switch (event.type) {
        case InteractionEventType::kKeyDown:
            if (event.key == 'C' && event.isCtrl) {
                CenterOnPosition();
                return true;
            }
            break;
        default:
            break;
    }
    
    return false;
}

void LocationDisplayHandler::UpdatePosition(const Position& position) {
    m_position = position;
    m_hasValidPosition = true;
    
    if (m_config.smoothPosition) {
        ApplySmoothing();
    } else {
        m_smoothedPosition = position;
    }
    
    UpdateTrack();
    UpdateLocationFeedback();
    NotifyPositionChanged();
    NotifyLocationUpdated();
    
    if (m_config.autoCenter && m_hasValidPosition) {
        CenterOnPosition(m_position.latitude, m_position.longitude);
    }
}

void LocationDisplayHandler::UpdateHeading(const Heading& heading) {
    m_heading = heading;
    m_hasValidHeading = true;
    
    if (m_config.smoothPosition) {
        m_smoothedHeading.degrees = heading.degrees;
        m_smoothedHeading.magneticVariation = heading.magneticVariation;
        m_smoothedHeading.isTrue = heading.isTrue;
        m_smoothedHeading.rateOfTurn = heading.rateOfTurn;
    }
    
    UpdateLocationFeedback();
    NotifyHeadingChanged();
    NotifyLocationUpdated();
    
    if (m_config.autoRotate && m_hasValidHeading) {
        if (m_rotateCallback) {
            m_rotateCallback(m_heading.degrees);
        }
    }
}

void LocationDisplayHandler::UpdateSpeed(const Speed& speed) {
    m_speed = speed;
    m_hasValidSpeed = true;
    
    UpdateLocationFeedback();
    NotifySpeedChanged();
    NotifyLocationUpdated();
}

void LocationDisplayHandler::UpdateLocation(const Position& position, const Heading& heading, const Speed& speed) {
    m_position = position;
    m_heading = heading;
    m_speed = speed;
    
    m_hasValidPosition = true;
    m_hasValidHeading = true;
    m_hasValidSpeed = true;
    
    if (m_config.smoothPosition) {
        ApplySmoothing();
        m_smoothedHeading = heading;
    } else {
        m_smoothedPosition = position;
        m_smoothedHeading = heading;
    }
    
    UpdateTrack();
    UpdateLocationFeedback();
    NotifyLocationUpdated();
    
    if (m_config.autoCenter && m_hasValidPosition) {
        CenterOnPosition(m_position.latitude, m_position.longitude);
    }
    
    if (m_config.autoRotate && m_hasValidHeading) {
        if (m_rotateCallback) {
            m_rotateCallback(m_heading.degrees);
        }
    }
}

void LocationDisplayHandler::SetPosition(double latitude, double longitude) {
    Position pos;
    pos.latitude = latitude;
    pos.longitude = longitude;
    pos.source = PositionSource::kManual;
    pos.status = PositionStatus::kManual;
    pos.timestamp = std::chrono::system_clock::now();
    UpdatePosition(pos);
}

void LocationDisplayHandler::SetPositionAndHeading(double latitude, double longitude, double headingDegrees) {
    Position pos;
    pos.latitude = latitude;
    pos.longitude = longitude;
    pos.source = PositionSource::kManual;
    pos.status = PositionStatus::kManual;
    pos.timestamp = std::chrono::system_clock::now();
    
    Heading hdg;
    hdg.degrees = headingDegrees;
    hdg.isTrue = true;
    
    UpdateLocation(pos, hdg, m_speed);
}

LocationInfo LocationDisplayHandler::GetLocationInfo() const {
    LocationInfo info;
    info.position = m_position;
    info.heading = m_heading;
    info.speed = m_speed;
    info.mapCoordinate = m_mapCoordinate;
    info.isValid = m_hasValidPosition;
    return info;
}

void LocationDisplayHandler::SetMapCoordinate(const Coordinate& coord) {
    m_mapCoordinate = coord;
}

void LocationDisplayHandler::SetLocationStyle(const LocationStyle& style) {
    m_style = style;
    if (m_hasValidPosition && m_enabled) {
        UpdateLocationFeedback();
    }
}

void LocationDisplayHandler::SetLocationConfig(const LocationConfig& config) {
    m_config = config;
}

void LocationDisplayHandler::SetAutoCenter(bool enabled) {
    m_config.autoCenter = enabled;
}

void LocationDisplayHandler::SetAutoRotate(bool enabled) {
    m_config.autoRotate = enabled;
}

void LocationDisplayHandler::CenterOnPosition() {
    if (m_hasValidPosition) {
        CenterOnPosition(m_position.latitude, m_position.longitude);
    }
}

void LocationDisplayHandler::CenterOnPosition(double latitude, double longitude) {
    if (m_centerCallback) {
        m_centerCallback(longitude, latitude);
    }
}

void LocationDisplayHandler::SetViewportSize(int width, int height) {
    m_viewportWidth = width;
    m_viewportHeight = height;
}

void LocationDisplayHandler::SetFeedbackManager(FeedbackManager* manager) {
    m_feedbackManager = manager;
}

void LocationDisplayHandler::SetScreenToWorldTransform(std::function<Coordinate(double, double)> transform) {
    m_screenToWorld = transform;
}

void LocationDisplayHandler::SetWorldToScreenTransform(std::function<Coordinate(double, double)> transform) {
    m_worldToScreen = transform;
}

void LocationDisplayHandler::SetCenterCallback(std::function<void(double, double)> callback) {
    m_centerCallback = callback;
}

void LocationDisplayHandler::SetRotateCallback(std::function<void(double)> callback) {
    m_rotateCallback = callback;
}

void LocationDisplayHandler::SetExtentCallback(std::function<void(const Envelope&)> callback) {
    m_extentCallback = callback;
}

Coordinate LocationDisplayHandler::ScreenToWorld(double screenX, double screenY) const {
    if (m_screenToWorld) {
        return m_screenToWorld(screenX, screenY);
    }
    return Coordinate(screenX, screenY);
}

Coordinate LocationDisplayHandler::WorldToScreen(double worldX, double worldY) const {
    if (m_worldToScreen) {
        return m_worldToScreen(worldX, worldY);
    }
    return Coordinate(worldX, worldY);
}

void LocationDisplayHandler::SetPositionChangedCallback(PositionChangedCallback callback) {
    m_positionChangedCallback = callback;
}

void LocationDisplayHandler::SetHeadingChangedCallback(HeadingChangedCallback callback) {
    m_headingChangedCallback = callback;
}

void LocationDisplayHandler::SetSpeedChangedCallback(SpeedChangedCallback callback) {
    m_speedChangedCallback = callback;
}

void LocationDisplayHandler::SetLocationUpdatedCallback(LocationUpdatedCallback callback) {
    m_locationUpdatedCallback = callback;
}

void LocationDisplayHandler::StartTrackRecording() {
    m_isRecordingTrack = true;
    m_lastTrackTime = std::chrono::system_clock::now();
}

void LocationDisplayHandler::StopTrackRecording() {
    m_isRecordingTrack = false;
}

void LocationDisplayHandler::ClearTrack() {
    m_trackPoints.clear();
}

void LocationDisplayHandler::ShowLocation(bool show) {
    m_showLocation = show;
    if (show && m_hasValidPosition) {
        UpdateLocationFeedback();
    } else {
        ClearLocationFeedback();
    }
}

double LocationDisplayHandler::DistanceToPosition(double screenX, double screenY) const {
    if (!m_hasValidPosition) {
        return -1.0;
    }
    
    Coordinate worldPos = ScreenToWorld(screenX, screenY);
    double dx = worldPos.x - m_mapCoordinate.x;
    double dy = worldPos.y - m_mapCoordinate.y;
    return std::sqrt(dx * dx + dy * dy);
}

bool LocationDisplayHandler::IsNearPosition(double screenX, double screenY, double threshold) const {
    double dist = DistanceToPosition(screenX, screenY);
    return dist >= 0 && dist <= threshold;
}

void LocationDisplayHandler::UpdateLocationFeedback() {
    if (!m_showLocation || !m_feedbackManager || !m_hasValidPosition) {
        return;
    }
    
    ClearLocationFeedback();
    
    if (m_style.showVesselSymbol) {
        auto item = FeedbackItem::Create(FeedbackType::kHighlight);
        if (item) {
            item->SetPoint(m_mapCoordinate);
            
            FeedbackConfig config;
            config.strokeColor = m_style.vesselColor;
            config.strokeWidth = m_style.vesselSize / 5.0;
            config.fillColor = m_style.vesselColor;
            config.opacity = 0.8;
            config.visible = true;
            item->SetConfig(config);
            
            m_feedbackId = m_feedbackManager->AddFeedback(item);
        }
    }
    
    if (m_style.showHeadingLine && m_hasValidHeading) {
        double rad = m_heading.degrees * M_PI / 180.0;
        double dx = std::sin(rad) * m_style.headingLength;
        double dy = -std::cos(rad) * m_style.headingLength;
        
        std::vector<Coordinate> headingLine;
        headingLine.push_back(m_mapCoordinate);
        headingLine.push_back(Coordinate(m_mapCoordinate.x + dx, m_mapCoordinate.y + dy));
        
        auto headingItem = FeedbackItem::Create(FeedbackType::kHighlight);
        if (headingItem) {
            headingItem->SetPoints(headingLine);
            
            FeedbackConfig config;
            config.strokeColor = m_style.headingColor;
            config.strokeWidth = 2.0;
            config.opacity = 1.0;
            config.visible = true;
            headingItem->SetConfig(config);
            
            m_feedbackManager->AddFeedback(headingItem);
        }
    }
    
    if (m_style.showAccuracyCircle && m_position.accuracy > 0) {
        double radius = m_position.accuracy;
        Envelope accuracyEnv(
            m_mapCoordinate.x - radius,
            m_mapCoordinate.y - radius,
            m_mapCoordinate.x + radius,
            m_mapCoordinate.y + radius
        );
        
        auto accuracyItem = FeedbackItem::Create(FeedbackType::kHighlight);
        if (accuracyItem) {
            accuracyItem->SetEnvelope(accuracyEnv);
            
            FeedbackConfig config;
            config.strokeColor = m_style.accuracyColor;
            config.strokeWidth = 1.0;
            config.fillColor = m_style.accuracyColor;
            config.opacity = 0.3;
            config.visible = true;
            accuracyItem->SetConfig(config);
            
            m_feedbackManager->AddFeedback(accuracyItem);
        }
    }
}

void LocationDisplayHandler::ClearLocationFeedback() {
    if (m_feedbackManager && m_feedbackId > 0) {
        m_feedbackManager->RemoveFeedback(m_feedbackId);
        m_feedbackId = 0;
    }
}

void LocationDisplayHandler::UpdateTrack() {
    if (!m_isRecordingTrack || !m_hasValidPosition) {
        return;
    }
    
    auto now = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration<double>(now - m_lastTrackTime).count();
    
    if (elapsed >= m_config.trackInterval) {
        m_trackPoints.push_back(m_mapCoordinate);
        
        if (m_trackPoints.size() > static_cast<size_t>(m_config.maxTrackPoints)) {
            m_trackPoints.erase(m_trackPoints.begin());
        }
        
        m_lastTrackTime = now;
    }
}

void LocationDisplayHandler::ApplySmoothing() {
    double factor = m_config.smoothFactor;
    
    m_smoothedPosition.latitude = m_smoothedPosition.latitude * (1.0 - factor) + 
                                   m_position.latitude * factor;
    m_smoothedPosition.longitude = m_smoothedPosition.longitude * (1.0 - factor) + 
                                    m_position.longitude * factor;
    m_smoothedPosition.altitude = m_smoothedPosition.altitude * (1.0 - factor) + 
                                   m_position.altitude * factor;
    m_smoothedPosition.accuracy = m_position.accuracy;
    m_smoothedPosition.speed = m_smoothedPosition.speed * (1.0 - factor) + 
                                m_position.speed * factor;
    m_smoothedPosition.bearing = m_position.bearing;
    m_smoothedPosition.source = m_position.source;
    m_smoothedPosition.status = m_position.status;
    m_smoothedPosition.timestamp = m_position.timestamp;
    m_smoothedPosition.satelliteCount = m_position.satelliteCount;
    m_smoothedPosition.hdop = m_position.hdop;
    m_smoothedPosition.vdop = m_position.vdop;
}

void LocationDisplayHandler::NotifyPositionChanged() {
    if (m_positionChangedCallback) {
        m_positionChangedCallback(m_position);
    }
}

void LocationDisplayHandler::NotifyHeadingChanged() {
    if (m_headingChangedCallback) {
        m_headingChangedCallback(m_heading);
    }
}

void LocationDisplayHandler::NotifySpeedChanged() {
    if (m_speedChangedCallback) {
        m_speedChangedCallback(m_speed);
    }
}

void LocationDisplayHandler::NotifyLocationUpdated() {
    if (m_locationUpdatedCallback) {
        m_locationUpdatedCallback(GetLocationInfo());
    }
}

}  
}  
