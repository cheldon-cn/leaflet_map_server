#include "ogc/graph/interaction/location_display_handler.h"
#include "ogc/graph/interaction/interaction_feedback.h"
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace ogc {
namespace graph {

struct LocationDisplayHandler::Impl {
    std::string name;
    int priority = 100;
    bool enabled = true;
    bool isActive = false;
    bool showLocation = true;
    
    Position position;
    Heading heading;
    Speed speed;
    Coordinate mapCoordinate;
    
    bool hasValidPosition = false;
    bool hasValidHeading = false;
    bool hasValidSpeed = false;
    
    LocationStyle style;
    LocationConfig config;
    
    int viewportWidth = 800;
    int viewportHeight = 600;
    
    FeedbackManager* feedbackManager = nullptr;
    int64_t feedbackId = 0;
    int64_t trackFeedbackId = 0;
    
    std::function<Coordinate(double, double)> screenToWorld;
    std::function<Coordinate(double, double)> worldToScreen;
    std::function<void(double, double)> centerCallback;
    std::function<void(double)> rotateCallback;
    std::function<void(const Envelope&)> extentCallback;
    
    PositionChangedCallback positionChangedCallback;
    HeadingChangedCallback headingChangedCallback;
    SpeedChangedCallback speedChangedCallback;
    LocationUpdatedCallback locationUpdatedCallback;
    
    bool isRecordingTrack = false;
    std::vector<Coordinate> trackPoints;
    std::chrono::system_clock::time_point lastTrackTime;
    
    Position smoothedPosition;
    Heading smoothedHeading;
    
    Impl(const std::string& n) : name(n) {
        position.timestamp = std::chrono::system_clock::now();
        smoothedPosition.timestamp = std::chrono::system_clock::now();
    }
};

std::unique_ptr<LocationDisplayHandler> LocationDisplayHandler::Create(const std::string& name) {
    return std::unique_ptr<LocationDisplayHandler>(new LocationDisplayHandler(name));
}

LocationDisplayHandler::LocationDisplayHandler(const std::string& name)
    : impl_(std::make_unique<Impl>(name))
{
}

LocationDisplayHandler::~LocationDisplayHandler() {
    ClearLocationFeedback();
}

std::string LocationDisplayHandler::GetName() const {
    return impl_->name;
}

int LocationDisplayHandler::GetPriority() const {
    return impl_->priority;
}

void LocationDisplayHandler::SetPriority(int priority) {
    impl_->priority = priority;
}

bool LocationDisplayHandler::IsEnabled() const {
    return impl_->enabled;
}

InteractionState LocationDisplayHandler::GetState() const {
    return impl_->isActive ? InteractionState::kPan : InteractionState::kNone;
}

void LocationDisplayHandler::SetEnabled(bool enabled) {
    impl_->enabled = enabled;
    if (!enabled) {
        ClearLocationFeedback();
    } else if (impl_->hasValidPosition) {
        UpdateLocationFeedback();
    }
}

bool LocationDisplayHandler::HandleEvent(const InteractionEvent& event) {
    if (!impl_->enabled) {
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
    impl_->position = position;
    impl_->hasValidPosition = true;
    
    if (impl_->config.smoothPosition) {
        ApplySmoothing();
    } else {
        impl_->smoothedPosition = position;
    }
    
    UpdateTrack();
    UpdateLocationFeedback();
    NotifyPositionChanged();
    NotifyLocationUpdated();
    
    if (impl_->config.autoCenter && impl_->hasValidPosition) {
        CenterOnPosition(impl_->position.latitude, impl_->position.longitude);
    }
}

void LocationDisplayHandler::UpdateHeading(const Heading& heading) {
    impl_->heading = heading;
    impl_->hasValidHeading = true;
    
    if (impl_->config.smoothPosition) {
        impl_->smoothedHeading.degrees = heading.degrees;
        impl_->smoothedHeading.magneticVariation = heading.magneticVariation;
        impl_->smoothedHeading.isTrue = heading.isTrue;
        impl_->smoothedHeading.rateOfTurn = heading.rateOfTurn;
    }
    
    UpdateLocationFeedback();
    NotifyHeadingChanged();
    NotifyLocationUpdated();
    
    if (impl_->config.autoRotate && impl_->hasValidHeading) {
        if (impl_->rotateCallback) {
            impl_->rotateCallback(impl_->heading.degrees);
        }
    }
}

void LocationDisplayHandler::UpdateSpeed(const Speed& speed) {
    impl_->speed = speed;
    impl_->hasValidSpeed = true;
    
    UpdateLocationFeedback();
    NotifySpeedChanged();
    NotifyLocationUpdated();
}

void LocationDisplayHandler::UpdateLocation(const Position& position, const Heading& heading, const Speed& speed) {
    impl_->position = position;
    impl_->heading = heading;
    impl_->speed = speed;
    
    impl_->hasValidPosition = true;
    impl_->hasValidHeading = true;
    impl_->hasValidSpeed = true;
    
    if (impl_->config.smoothPosition) {
        ApplySmoothing();
        impl_->smoothedHeading = heading;
    } else {
        impl_->smoothedPosition = position;
        impl_->smoothedHeading = heading;
    }
    
    UpdateTrack();
    UpdateLocationFeedback();
    NotifyLocationUpdated();
    
    if (impl_->config.autoCenter && impl_->hasValidPosition) {
        CenterOnPosition(impl_->position.latitude, impl_->position.longitude);
    }
    
    if (impl_->config.autoRotate && impl_->hasValidHeading) {
        if (impl_->rotateCallback) {
            impl_->rotateCallback(impl_->heading.degrees);
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
    
    UpdateLocation(pos, hdg, impl_->speed);
}

const Position& LocationDisplayHandler::GetPosition() const {
    return impl_->position;
}

const Heading& LocationDisplayHandler::GetHeading() const {
    return impl_->heading;
}

const Speed& LocationDisplayHandler::GetSpeed() const {
    return impl_->speed;
}

LocationInfo LocationDisplayHandler::GetLocationInfo() const {
    LocationInfo info;
    info.position = impl_->position;
    info.heading = impl_->heading;
    info.speed = impl_->speed;
    info.mapCoordinate = impl_->mapCoordinate;
    info.isValid = impl_->hasValidPosition;
    return info;
}

void LocationDisplayHandler::SetMapCoordinate(const Coordinate& coord) {
    impl_->mapCoordinate = coord;
}

Coordinate LocationDisplayHandler::GetMapCoordinate() const {
    return impl_->mapCoordinate;
}

void LocationDisplayHandler::SetLocationStyle(const LocationStyle& style) {
    impl_->style = style;
    if (impl_->hasValidPosition && impl_->enabled) {
        UpdateLocationFeedback();
    }
}

LocationStyle LocationDisplayHandler::GetLocationStyle() const {
    return impl_->style;
}

void LocationDisplayHandler::SetLocationConfig(const LocationConfig& config) {
    impl_->config = config;
}

LocationConfig LocationDisplayHandler::GetLocationConfig() const {
    return impl_->config;
}

void LocationDisplayHandler::SetAutoCenter(bool enabled) {
    impl_->config.autoCenter = enabled;
}

bool LocationDisplayHandler::IsAutoCenter() const {
    return impl_->config.autoCenter;
}

void LocationDisplayHandler::SetAutoRotate(bool enabled) {
    impl_->config.autoRotate = enabled;
}

bool LocationDisplayHandler::IsAutoRotate() const {
    return impl_->config.autoRotate;
}

void LocationDisplayHandler::CenterOnPosition() {
    if (impl_->hasValidPosition) {
        CenterOnPosition(impl_->position.latitude, impl_->position.longitude);
    }
}

void LocationDisplayHandler::CenterOnPosition(double latitude, double longitude) {
    if (impl_->centerCallback) {
        impl_->centerCallback(longitude, latitude);
    }
}

void LocationDisplayHandler::SetViewportSize(int width, int height) {
    impl_->viewportWidth = width;
    impl_->viewportHeight = height;
}

void LocationDisplayHandler::SetFeedbackManager(FeedbackManager* manager) {
    impl_->feedbackManager = manager;
}

FeedbackManager* LocationDisplayHandler::GetFeedbackManager() const {
    return impl_->feedbackManager;
}

void LocationDisplayHandler::SetScreenToWorldTransform(std::function<Coordinate(double, double)> transform) {
    impl_->screenToWorld = transform;
}

void LocationDisplayHandler::SetWorldToScreenTransform(std::function<Coordinate(double, double)> transform) {
    impl_->worldToScreen = transform;
}

void LocationDisplayHandler::SetCenterCallback(std::function<void(double, double)> callback) {
    impl_->centerCallback = callback;
}

void LocationDisplayHandler::SetRotateCallback(std::function<void(double)> callback) {
    impl_->rotateCallback = callback;
}

void LocationDisplayHandler::SetExtentCallback(std::function<void(const Envelope&)> callback) {
    impl_->extentCallback = callback;
}

Coordinate LocationDisplayHandler::ScreenToWorld(double screenX, double screenY) const {
    if (impl_->screenToWorld) {
        return impl_->screenToWorld(screenX, screenY);
    }
    return Coordinate(screenX, screenY);
}

Coordinate LocationDisplayHandler::WorldToScreen(double worldX, double worldY) const {
    if (impl_->worldToScreen) {
        return impl_->worldToScreen(worldX, worldY);
    }
    return Coordinate(worldX, worldY);
}

void LocationDisplayHandler::SetPositionChangedCallback(PositionChangedCallback callback) {
    impl_->positionChangedCallback = callback;
}

void LocationDisplayHandler::SetHeadingChangedCallback(HeadingChangedCallback callback) {
    impl_->headingChangedCallback = callback;
}

void LocationDisplayHandler::SetSpeedChangedCallback(SpeedChangedCallback callback) {
    impl_->speedChangedCallback = callback;
}

void LocationDisplayHandler::SetLocationUpdatedCallback(LocationUpdatedCallback callback) {
    impl_->locationUpdatedCallback = callback;
}

void LocationDisplayHandler::StartTrackRecording() {
    impl_->isRecordingTrack = true;
    impl_->lastTrackTime = std::chrono::system_clock::now();
}

void LocationDisplayHandler::StopTrackRecording() {
    impl_->isRecordingTrack = false;
}

bool LocationDisplayHandler::IsRecordingTrack() const {
    return impl_->isRecordingTrack;
}

void LocationDisplayHandler::ClearTrack() {
    impl_->trackPoints.clear();
}

const std::vector<Coordinate>& LocationDisplayHandler::GetTrackPoints() const {
    return impl_->trackPoints;
}

size_t LocationDisplayHandler::GetTrackPointCount() const {
    return impl_->trackPoints.size();
}

bool LocationDisplayHandler::HasValidPosition() const {
    return impl_->hasValidPosition;
}

bool LocationDisplayHandler::HasValidHeading() const {
    return impl_->hasValidHeading;
}

bool LocationDisplayHandler::HasValidSpeed() const {
    return impl_->hasValidSpeed;
}

void LocationDisplayHandler::ShowLocation(bool show) {
    impl_->showLocation = show;
    if (show && impl_->hasValidPosition) {
        UpdateLocationFeedback();
    } else {
        ClearLocationFeedback();
    }
}

bool LocationDisplayHandler::IsLocationVisible() const {
    return impl_->showLocation;
}

double LocationDisplayHandler::DistanceToPosition(double screenX, double screenY) const {
    if (!impl_->hasValidPosition) {
        return -1.0;
    }
    
    Coordinate worldPos = ScreenToWorld(screenX, screenY);
    double dx = worldPos.x - impl_->mapCoordinate.x;
    double dy = worldPos.y - impl_->mapCoordinate.y;
    return std::sqrt(dx * dx + dy * dy);
}

bool LocationDisplayHandler::IsNearPosition(double screenX, double screenY, double threshold) const {
    double dist = DistanceToPosition(screenX, screenY);
    return dist >= 0 && dist <= threshold;
}

void LocationDisplayHandler::UpdateLocationFeedback() {
    if (!impl_->showLocation || !impl_->feedbackManager || !impl_->hasValidPosition) {
        return;
    }
    
    ClearLocationFeedback();
    
    if (impl_->style.showVesselSymbol) {
        auto item = FeedbackItem::Create(FeedbackType::kHighlight);
        if (item) {
            item->SetPoint(impl_->mapCoordinate);
            
            FeedbackConfig config;
            config.strokeColor = impl_->style.vesselColor;
            config.strokeWidth = impl_->style.vesselSize / 5.0;
            config.fillColor = impl_->style.vesselColor;
            config.opacity = 0.8;
            config.visible = true;
            item->SetConfig(config);
            
            impl_->feedbackId = impl_->feedbackManager->AddFeedback(item);
        }
    }
    
    if (impl_->style.showHeadingLine && impl_->hasValidHeading) {
        double rad = impl_->heading.degrees * M_PI / 180.0;
        double dx = std::sin(rad) * impl_->style.headingLength;
        double dy = -std::cos(rad) * impl_->style.headingLength;
        
        std::vector<Coordinate> headingLine;
        headingLine.push_back(impl_->mapCoordinate);
        headingLine.push_back(Coordinate(impl_->mapCoordinate.x + dx, impl_->mapCoordinate.y + dy));
        
        auto headingItem = FeedbackItem::Create(FeedbackType::kHighlight);
        if (headingItem) {
            headingItem->SetPoints(headingLine);
            
            FeedbackConfig config;
            config.strokeColor = impl_->style.headingColor;
            config.strokeWidth = 2.0;
            config.opacity = 1.0;
            config.visible = true;
            headingItem->SetConfig(config);
            
            impl_->feedbackManager->AddFeedback(headingItem);
        }
    }
    
    if (impl_->style.showAccuracyCircle && impl_->position.accuracy > 0) {
        double radius = impl_->position.accuracy;
        Envelope accuracyEnv(
            impl_->mapCoordinate.x - radius,
            impl_->mapCoordinate.y - radius,
            impl_->mapCoordinate.x + radius,
            impl_->mapCoordinate.y + radius
        );
        
        auto accuracyItem = FeedbackItem::Create(FeedbackType::kHighlight);
        if (accuracyItem) {
            accuracyItem->SetEnvelope(accuracyEnv);
            
            FeedbackConfig config;
            config.strokeColor = impl_->style.accuracyColor;
            config.strokeWidth = 1.0;
            config.fillColor = impl_->style.accuracyColor;
            config.opacity = 0.3;
            config.visible = true;
            accuracyItem->SetConfig(config);
            
            impl_->feedbackManager->AddFeedback(accuracyItem);
        }
    }
}

void LocationDisplayHandler::ClearLocationFeedback() {
    if (impl_->feedbackManager && impl_->feedbackId > 0) {
        impl_->feedbackManager->RemoveFeedback(impl_->feedbackId);
        impl_->feedbackId = 0;
    }
}

void LocationDisplayHandler::UpdateTrack() {
    if (!impl_->isRecordingTrack || !impl_->hasValidPosition) {
        return;
    }
    
    auto now = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration<double>(now - impl_->lastTrackTime).count();
    
    if (elapsed >= impl_->config.trackInterval) {
        impl_->trackPoints.push_back(impl_->mapCoordinate);
        
        if (impl_->trackPoints.size() > static_cast<size_t>(impl_->config.maxTrackPoints)) {
            impl_->trackPoints.erase(impl_->trackPoints.begin());
        }
        
        impl_->lastTrackTime = now;
    }
}

void LocationDisplayHandler::ApplySmoothing() {
    double factor = impl_->config.smoothFactor;
    
    impl_->smoothedPosition.latitude = impl_->smoothedPosition.latitude * (1.0 - factor) + 
                                   impl_->position.latitude * factor;
    impl_->smoothedPosition.longitude = impl_->smoothedPosition.longitude * (1.0 - factor) + 
                                    impl_->position.longitude * factor;
    impl_->smoothedPosition.altitude = impl_->smoothedPosition.altitude * (1.0 - factor) + 
                                   impl_->position.altitude * factor;
    impl_->smoothedPosition.accuracy = impl_->position.accuracy;
    impl_->smoothedPosition.speed = impl_->smoothedPosition.speed * (1.0 - factor) + 
                                impl_->position.speed * factor;
    impl_->smoothedPosition.bearing = impl_->position.bearing;
    impl_->smoothedPosition.source = impl_->position.source;
    impl_->smoothedPosition.status = impl_->position.status;
    impl_->smoothedPosition.timestamp = impl_->position.timestamp;
    impl_->smoothedPosition.satelliteCount = impl_->position.satelliteCount;
    impl_->smoothedPosition.hdop = impl_->position.hdop;
    impl_->smoothedPosition.vdop = impl_->position.vdop;
}

void LocationDisplayHandler::NotifyPositionChanged() {
    if (impl_->positionChangedCallback) {
        impl_->positionChangedCallback(impl_->position);
    }
}

void LocationDisplayHandler::NotifyHeadingChanged() {
    if (impl_->headingChangedCallback) {
        impl_->headingChangedCallback(impl_->heading);
    }
}

void LocationDisplayHandler::NotifySpeedChanged() {
    if (impl_->speedChangedCallback) {
        impl_->speedChangedCallback(impl_->speed);
    }
}

void LocationDisplayHandler::NotifyLocationUpdated() {
    if (impl_->locationUpdatedCallback) {
        impl_->locationUpdatedCallback(GetLocationInfo());
    }
}

}  
}  
