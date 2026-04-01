#ifndef OGC_DRAW_LOCATION_DISPLAY_HANDLER_H
#define OGC_DRAW_LOCATION_DISPLAY_HANDLER_H

#include "ogc/draw/export.h"
#include "ogc/draw/interaction_handler.h"
#include "ogc/draw/interaction_feedback.h"
#include "ogc/coordinate.h"
#include "ogc/envelope.h"
#include <memory>
#include <string>
#include <set>
#include <functional>
#include <chrono>

namespace ogc {
namespace draw {

class LayerManager;
class FeedbackManager;

enum class PositionSource {
    kNone = 0,
    kInternalGPS,
    kExternalGPS,
    kBeidou,
    kRTK,
    kManual
};

enum class PositionStatus {
    kNoFix = 0,
    k2DFix,
    k3DFix,
    kDifferential,
    kRTK,
    kFloatRTK,
    kEstimated,
    kManual
};

struct Position {
    double latitude = 0.0;
    double longitude = 0.0;
    double altitude = 0.0;
    double accuracy = 0.0;
    double speed = 0.0;
    double bearing = 0.0;
    PositionSource source = PositionSource::kNone;
    PositionStatus status = PositionStatus::kNoFix;
    std::chrono::system_clock::time_point timestamp;
    int satelliteCount = 0;
    double hdop = 0.0;
    double vdop = 0.0;
};

struct Heading {
    double degrees = 0.0;
    double magneticVariation = 0.0;
    bool isTrue = true;
    double rateOfTurn = 0.0;
};

struct Speed {
    double knots = 0.0;
    double kmh = 0.0;
    double mph = 0.0;
};

struct LocationStyle {
    uint32_t vesselColor = 0xFFFF0000;
    uint32_t headingColor = 0xFF0000FF;
    uint32_t trackColor = 0xFF00FF00;
    uint32_t accuracyColor = 0x800000FF;
    double vesselSize = 20.0;
    double headingLength = 50.0;
    double accuracyRadius = 0.0;
    bool showAccuracyCircle = true;
    bool showHeadingLine = true;
    bool showVesselSymbol = true;
    bool showTrackLine = true;
    double trackLineWidth = 2.0;
};

struct LocationConfig {
    bool autoCenter = false;
    bool autoRotate = false;
    double centerThreshold = 100.0;
    double rotateThreshold = 5.0;
    bool smoothPosition = true;
    double smoothFactor = 0.3;
    bool recordTrack = false;
    double trackInterval = 1.0;
    int maxTrackPoints = 10000;
};

struct LocationInfo {
    Position position;
    Heading heading;
    Speed speed;
    Coordinate mapCoordinate;
    bool isValid = false;
};

class OGC_GRAPH_API LocationDisplayHandler : public IInteractionHandler {
public:
    using PositionChangedCallback = std::function<void(const Position&)>;
    using HeadingChangedCallback = std::function<void(const Heading&)>;
    using SpeedChangedCallback = std::function<void(const Speed&)>;
    using LocationUpdatedCallback = std::function<void(const LocationInfo&)>;
    
    static std::unique_ptr<LocationDisplayHandler> Create(const std::string& name = "LocationDisplay");
    
    ~LocationDisplayHandler() override;
    
    std::string GetName() const override { return m_name; }
    int GetPriority() const override { return m_priority; }
    void SetPriority(int priority) override { m_priority = priority; }
    
    bool IsEnabled() const override { return m_enabled; }
    void SetEnabled(bool enabled) override;
    
    bool HandleEvent(const InteractionEvent& event) override;
    
    InteractionState GetState() const override { 
        return m_isActive ? InteractionState::kPan : InteractionState::kNone; 
    }
    
    void UpdatePosition(const Position& position);
    void UpdateHeading(const Heading& heading);
    void UpdateSpeed(const Speed& speed);
    void UpdateLocation(const Position& position, const Heading& heading, const Speed& speed);
    
    void SetPosition(double latitude, double longitude);
    void SetPositionAndHeading(double latitude, double longitude, double headingDegrees);
    
    const Position& GetPosition() const { return m_position; }
    const Heading& GetHeading() const { return m_heading; }
    const Speed& GetSpeed() const { return m_speed; }
    LocationInfo GetLocationInfo() const;
    
    void SetMapCoordinate(const Coordinate& coord);
    Coordinate GetMapCoordinate() const { return m_mapCoordinate; }
    
    void SetLocationStyle(const LocationStyle& style);
    LocationStyle GetLocationStyle() const { return m_style; }
    
    void SetLocationConfig(const LocationConfig& config);
    LocationConfig GetLocationConfig() const { return m_config; }
    
    void SetAutoCenter(bool enabled);
    bool IsAutoCenter() const { return m_config.autoCenter; }
    
    void SetAutoRotate(bool enabled);
    bool IsAutoRotate() const { return m_config.autoRotate; }
    
    void CenterOnPosition();
    void CenterOnPosition(double latitude, double longitude);
    
    void SetViewportSize(int width, int height);
    
    void SetFeedbackManager(FeedbackManager* manager);
    FeedbackManager* GetFeedbackManager() const { return m_feedbackManager; }
    
    void SetScreenToWorldTransform(std::function<Coordinate(double, double)> transform);
    void SetWorldToScreenTransform(std::function<Coordinate(double, double)> transform);
    void SetCenterCallback(std::function<void(double, double)> callback);
    void SetRotateCallback(std::function<void(double)> callback);
    void SetExtentCallback(std::function<void(const Envelope&)> callback);
    
    Coordinate ScreenToWorld(double screenX, double screenY) const;
    Coordinate WorldToScreen(double worldX, double worldY) const;
    
    void SetPositionChangedCallback(PositionChangedCallback callback);
    void SetHeadingChangedCallback(HeadingChangedCallback callback);
    void SetSpeedChangedCallback(SpeedChangedCallback callback);
    void SetLocationUpdatedCallback(LocationUpdatedCallback callback);
    
    void StartTrackRecording();
    void StopTrackRecording();
    bool IsRecordingTrack() const { return m_isRecordingTrack; }
    
    void ClearTrack();
    const std::vector<Coordinate>& GetTrackPoints() const { return m_trackPoints; }
    size_t GetTrackPointCount() const { return m_trackPoints.size(); }
    
    bool HasValidPosition() const { return m_hasValidPosition; }
    bool HasValidHeading() const { return m_hasValidHeading; }
    bool HasValidSpeed() const { return m_hasValidSpeed; }
    
    void ShowLocation(bool show);
    bool IsLocationVisible() const { return m_showLocation; }
    
    double DistanceToPosition(double screenX, double screenY) const;
    bool IsNearPosition(double screenX, double screenY, double threshold) const;
    
private:
    explicit LocationDisplayHandler(const std::string& name);
    
    void UpdateLocationFeedback();
    void ClearLocationFeedback();
    void UpdateTrack();
    void ApplySmoothing();
    void NotifyPositionChanged();
    void NotifyHeadingChanged();
    void NotifySpeedChanged();
    void NotifyLocationUpdated();
    
    std::string m_name;
    int m_priority = 100;
    bool m_enabled = true;
    bool m_isActive = false;
    bool m_showLocation = true;
    
    Position m_position;
    Heading m_heading;
    Speed m_speed;
    Coordinate m_mapCoordinate;
    
    bool m_hasValidPosition = false;
    bool m_hasValidHeading = false;
    bool m_hasValidSpeed = false;
    
    LocationStyle m_style;
    LocationConfig m_config;
    
    int m_viewportWidth = 800;
    int m_viewportHeight = 600;
    
    FeedbackManager* m_feedbackManager = nullptr;
    int64_t m_feedbackId = 0;
    int64_t m_trackFeedbackId = 0;
    
    std::function<Coordinate(double, double)> m_screenToWorld;
    std::function<Coordinate(double, double)> m_worldToScreen;
    std::function<void(double, double)> m_centerCallback;
    std::function<void(double)> m_rotateCallback;
    std::function<void(const Envelope&)> m_extentCallback;
    
    PositionChangedCallback m_positionChangedCallback;
    HeadingChangedCallback m_headingChangedCallback;
    SpeedChangedCallback m_speedChangedCallback;
    LocationUpdatedCallback m_locationUpdatedCallback;
    
    bool m_isRecordingTrack = false;
    std::vector<Coordinate> m_trackPoints;
    std::chrono::system_clock::time_point m_lastTrackTime;
    
    Position m_smoothedPosition;
    Heading m_smoothedHeading;
};

}  
}  

#endif
