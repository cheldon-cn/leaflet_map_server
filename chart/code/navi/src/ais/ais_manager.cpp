#include "ogc/navi/ais/ais_manager.h"
#include "ogc/navi/ais/ais_parser.h"
#include "ogc/navi/ais/collision_assessor.h"
#include "ogc/navi/navigation/navigation_calculator.h"
#include <map>
#include <cmath>
#include <algorithm>

namespace ogc {
namespace navi {

struct AisManager::Impl {
    std::map<uint32_t, AisTarget*> targets;
    bool initialized = false;
};

AisManager::AisManager()
    : impl_(new Impl())
    , cpa_threshold_(2.0)
    , tcpa_threshold_(30.0)
{
}

AisManager::~AisManager() {
    Shutdown();
}

AisManager& AisManager::Instance() {
    static AisManager instance;
    return instance;
}

bool AisManager::Initialize() {
    if (impl_->initialized) {
        return true;
    }
    
    impl_->initialized = true;
    return true;
}

void AisManager::Shutdown() {
    for (auto& pair : impl_->targets) {
        if (pair.second) {
            pair.second->ReleaseReference();
        }
    }
    impl_->targets.clear();
    impl_->initialized = false;
}

void AisManager::ProcessAisMessage(const std::string& nmea_message) {
    AisMessage msg;
    if (!AisParser::Instance().ParseNmeaMessage(nmea_message, msg)) {
        return;
    }
    
    std::vector<uint8_t> payload = DecodePayload(nmea_message);
    if (payload.empty()) {
        return;
    }
    
    int msg_type = msg.message_type;
    
    if (msg_type >= 1 && msg_type <= 3) {
        ParsePositionReport(payload, msg_type);
    } else if (msg_type == 5 || msg_type == 19 || msg_type == 24) {
        ParseStaticData(payload, msg_type);
    }
}

void AisManager::ProcessAisMessage(const std::vector<uint8_t>& payload) {
    if (payload.empty()) {
        return;
    }
    
    int msg_type = AisParser::Instance().GetMessageType(payload);
    
    if (msg_type >= 1 && msg_type <= 3) {
        ParsePositionReport(payload, msg_type);
    } else if (msg_type == 5 || msg_type == 19 || msg_type == 24) {
        ParseStaticData(payload, msg_type);
    }
}

int AisManager::GetTargetCount() const {
    return static_cast<int>(impl_->targets.size());
}

std::vector<AisTarget*> AisManager::GetAllTargets() {
    std::vector<AisTarget*> result;
    for (auto& pair : impl_->targets) {
        result.push_back(pair.second);
    }
    return result;
}

AisTarget* AisManager::GetTarget(uint32_t mmsi) const {
    auto it = impl_->targets.find(mmsi);
    if (it != impl_->targets.end()) {
        return it->second;
    }
    return nullptr;
}

void AisManager::RemoveStaleTargets(double max_age_seconds) {
    double current_time = 0.0;
    
    std::vector<uint32_t> to_remove;
    for (auto& pair : impl_->targets) {
        if (pair.second) {
            AisTargetData data = pair.second->ToData();
            if (current_time - data.last_update > max_age_seconds) {
                to_remove.push_back(pair.first);
            }
        }
    }
    
    for (uint32_t mmsi : to_remove) {
        auto it = impl_->targets.find(mmsi);
        if (it != impl_->targets.end()) {
            if (it->second) {
                it->second->ReleaseReference();
            }
            impl_->targets.erase(it);
        }
    }
}

void AisManager::CalculateCpaTcpa(
    const PositionData& own_position,
    const GeoPoint& target_position,
    double target_course,
    double target_speed,
    double& cpa,
    double& tcpa) {
    
    CollisionAssessor::Instance().CalculateCpaTcpa(
        own_position.latitude, own_position.longitude,
        own_position.course, own_position.speed,
        target_position.latitude, target_position.longitude,
        target_course, target_speed,
        cpa, tcpa);
}

std::vector<CollisionRisk> AisManager::AssessCollisionRisks(
    const PositionData& own_position,
    double cpa_threshold_nm,
    double tcpa_threshold_minutes) {
    
    std::vector<CollisionRisk> risks;
    
    for (auto& pair : impl_->targets) {
        if (!pair.second) {
            continue;
        }
        
        AisTargetData data = pair.second->ToData();
        
        double cpa = 0.0;
        double tcpa = 0.0;
        
        CalculateCpaTcpa(
            own_position,
            GeoPoint(data.longitude, data.latitude),
            data.course_over_ground,
            data.speed_over_ground,
            cpa, tcpa);
        
        CollisionRisk risk;
        risk.target_mmsi = pair.first;
        risk.cpa = cpa;
        risk.tcpa = tcpa;
        
        GeoPoint target_pos(data.longitude, data.latitude);
        GeoPoint own_pos(own_position.longitude, own_position.latitude);
        
        risk.distance = NavigationCalculator::Instance().CalculateDistanceToWaypoint(
            own_pos, target_pos) / 1852.0;
        risk.bearing = NavigationCalculator::Instance().CalculateBearingToWaypoint(
            own_pos, target_pos);
        
        risk.is_risk = (cpa < cpa_threshold_nm && tcpa > 0 && tcpa < tcpa_threshold_minutes);
        
        if (cpa < cpa_threshold_nm * 0.5 && tcpa < tcpa_threshold_minutes * 0.5) {
            risk.risk_level = AlertLevel::Critical;
        } else if (cpa < cpa_threshold_nm && tcpa < tcpa_threshold_minutes) {
            risk.risk_level = AlertLevel::Warning;
        } else {
            risk.risk_level = AlertLevel::Info;
        }
        
        risks.push_back(risk);
        
        if (risk.is_risk && collision_callback_) {
            collision_callback_(risk);
        }
    }
    
    return risks;
}

void AisManager::SetCpaThreshold(double cpa_nm) {
    cpa_threshold_ = cpa_nm;
}

void AisManager::SetTcpaThreshold(double tcpa_minutes) {
    tcpa_threshold_ = tcpa_minutes;
}

void AisManager::SetTargetUpdateCallback(AisTargetCallback callback) {
    target_callback_ = callback;
}

void AisManager::SetCollisionRiskCallback(CollisionRiskCallback callback) {
    collision_callback_ = callback;
}

void AisManager::ParsePositionReport(const std::vector<uint8_t>& payload, int message_type) {
    AisPositionReport report;
    if (!AisParser::Instance().ParsePositionReport(payload, report)) {
        return;
    }
    
    AisTarget* target = GetTarget(report.mmsi);
    if (!target) {
        target = AisTarget::Create(report.mmsi);
        impl_->targets[report.mmsi] = target;
    }
    
    target->SetPosition(report.longitude, report.latitude);
    target->SetCourseOverGround(report.course_over_ground);
    target->SetSpeedOverGround(report.speed_over_ground);
    target->SetHeading(report.heading);
    target->SetNavigationStatus(report.nav_status);
    target->SetLastUpdate(report.timestamp);
    
    if (target_callback_) {
        AisTargetData data = target->ToData();
        target_callback_(data);
    }
}

void AisManager::ParseStaticData(const std::vector<uint8_t>& payload, int message_type) {
    AisStaticData data;
    if (!AisParser::Instance().ParseStaticData(payload, data)) {
        return;
    }
    
    AisTarget* target = GetTarget(data.mmsi);
    if (!target) {
        target = AisTarget::Create(data.mmsi);
        impl_->targets[data.mmsi] = target;
    }
    
    target->SetShipName(data.ship_name);
    target->SetCallsign(data.callsign);
    target->SetShipType(data.ship_type);
    target->SetShipLength(data.dimension_to_bow + data.dimension_to_stern);
    target->SetShipWidth(data.dimension_to_port + data.dimension_to_starboard);
    target->SetDraught(data.draught);
    target->SetDestination(data.destination);
    
    if (target_callback_) {
        AisTargetData target_data = target->ToData();
        target_callback_(target_data);
    }
}

std::vector<uint8_t> AisManager::DecodePayload(const std::string& nmea) {
    size_t start = nmea.find(',');
    if (start == std::string::npos) {
        return std::vector<uint8_t>();
    }
    
    start++;
    size_t end = nmea.find(',', start);
    if (end == std::string::npos) {
        return std::vector<uint8_t>();
    }
    
    size_t payload_start = end + 1;
    end = nmea.find(',', payload_start);
    if (end == std::string::npos) {
        end = nmea.find('*', payload_start);
    }
    
    if (end == std::string::npos) {
        return std::vector<uint8_t>();
    }
    
    std::string payload = nmea.substr(payload_start, end - payload_start);
    return AisParser::Instance().DecodePayload(payload);
}

}
}
