#include "alert/ship_info.h"

namespace alert {

ShipInfo::ShipInfo()
    : m_shipId("")
    , m_shipName("")
    , m_shipType(ShipType::kUnknown)
    , m_draft(0)
    , m_length(0)
    , m_beam(0)
    , m_blockCoefficient(0.7)
    , m_height(0)
    , m_grossTonnage(0)
    , m_netTonnage(0)
    , m_maxSpeed(0) {}

ShipInfo::ShipInfo(const std::string& shipId, const std::string& shipName)
    : m_shipId(shipId)
    , m_shipName(shipName)
    , m_shipType(ShipType::kUnknown)
    , m_draft(0)
    , m_length(0)
    , m_beam(0)
    , m_blockCoefficient(0.7)
    , m_height(0)
    , m_grossTonnage(0)
    , m_netTonnage(0)
    , m_maxSpeed(0) {}

bool ShipInfo::IsValid() const {
    return !m_shipId.empty() && m_draft > 0;
}

}
