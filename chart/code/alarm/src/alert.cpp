#include "alert/alert.h"
#include "alert/alert_types.h"
#include <sstream>
#include <iomanip>
#include <ctime>

namespace alert {

Alert::Alert()
    : m_alertId("")
    , m_type(AlertType::kDepthAlert)
    , m_level(AlertLevel::kNone)
    , m_status(AlertStatus::kPending)
    , m_acknowledged(false) {}

Alert::Alert(const std::string& alertId, AlertType type, AlertLevel level)
    : m_alertId(alertId)
    , m_type(type)
    , m_level(level)
    , m_status(AlertStatus::kPending)
    , m_acknowledged(false) {}

void Alert::SetPosition(double lon, double lat) {
    m_position.longitude = lon;
    m_position.latitude = lat;
}

void Alert::SetDetail(const std::string& key, double value) {
    m_details[key] = value;
}

double Alert::GetDetail(const std::string& key, double defaultValue) const {
    auto it = m_details.find(key);
    if (it != m_details.end()) {
        return it->second;
    }
    return defaultValue;
}

void Alert::AddPushRecord(const PushRecord& record) {
    m_pushRecords.push_back(record);
}

bool Alert::IsActive() const {
    return m_status == AlertStatus::kActive || m_status == AlertStatus::kPushed;
}

bool Alert::IsExpired() const {
    return m_status == AlertStatus::kExpired || m_status == AlertStatus::kCleared;
}

std::string Alert::ToString() const {
    std::ostringstream oss;
    oss << "Alert[" << m_alertId << ", type=" << AlertTypeToString(m_type)
        << ", level=" << AlertLevelToString(m_level)
        << ", status=" << AlertStatusToString(m_status) << "]";
    return oss.str();
}

std::string Alert::GenerateAlertId(AlertType type) {
    std::ostringstream oss;
    oss << "ALERT_";
    
    std::time_t now = std::time(nullptr);
    std::tm* tm = std::localtime(&now);
    oss << std::setfill('0')
        << std::setw(4) << (1900 + tm->tm_year)
        << std::setw(2) << (tm->tm_mon + 1)
        << std::setw(2) << tm->tm_mday << "_"
        << std::setw(2) << tm->tm_hour
        << std::setw(2) << tm->tm_min
        << std::setw(2) << tm->tm_sec;
    
    static int counter = 0;
    oss << "_" << std::setw(3) << (++counter);
    
    return oss.str();
}

}
