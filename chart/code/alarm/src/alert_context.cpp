#include "alert/alert_context.h"

namespace alert {

AlertContext::AlertContext()
    : m_speed(0), m_course(0), m_heading(0), m_rateOfTurn(0) {}

AlertContext::AlertContext(const ShipInfo& shipInfo, const Coordinate& position)
    : m_shipInfo(shipInfo)
    , m_position(position)
    , m_speed(0)
    , m_course(0)
    , m_heading(0)
    , m_rateOfTurn(0) {}

void AlertContext::SetParameter(const std::string& key, double value) {
    m_parameters[key] = value;
}

double AlertContext::GetParameter(const std::string& key, double defaultValue) const {
    auto it = m_parameters.find(key);
    if (it != m_parameters.end()) {
        return it->second;
    }
    return defaultValue;
}

bool AlertContext::IsValid() const {
    return m_position.IsValid() && m_shipInfo.IsValid();
}

}
