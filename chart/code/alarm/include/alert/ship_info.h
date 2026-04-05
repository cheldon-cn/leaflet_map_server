#ifndef ALERT_SYSTEM_SHIP_INFO_H
#define ALERT_SYSTEM_SHIP_INFO_H

#include "alert_types.h"
#include "coordinate.h"
#include <string>

namespace alert {

class ShipInfo {
public:
    ShipInfo();
    ShipInfo(const std::string& shipId, const std::string& shipName);
    
    const std::string& GetShipId() const { return m_shipId; }
    void SetShipId(const std::string& id) { m_shipId = id; }
    
    const std::string& GetShipName() const { return m_shipName; }
    void SetShipName(const std::string& name) { m_shipName = name; }
    
    ShipType GetShipType() const { return m_shipType; }
    void SetShipType(ShipType type) { m_shipType = type; }
    
    double GetDraft() const { return m_draft; }
    void SetDraft(double draft) { m_draft = draft; }
    
    double GetLength() const { return m_length; }
    void SetLength(double length) { m_length = length; }
    
    double GetBeam() const { return m_beam; }
    void SetBeam(double beam) { m_beam = beam; }
    
    double GetBlockCoefficient() const { return m_blockCoefficient; }
    void SetBlockCoefficient(double cb) { m_blockCoefficient = cb; }
    
    double GetHeight() const { return m_height; }
    void SetHeight(double height) { m_height = height; }
    
    double GetGrossTonnage() const { return m_grossTonnage; }
    void SetGrossTonnage(double gt) { m_grossTonnage = gt; }
    
    double GetNetTonnage() const { return m_netTonnage; }
    void SetNetTonnage(double nt) { m_netTonnage = nt; }
    
    double GetMaxSpeed() const { return m_maxSpeed; }
    void SetMaxSpeed(double speed) { m_maxSpeed = speed; }
    
    bool IsValid() const;
    
private:
    std::string m_shipId;
    std::string m_shipName;
    ShipType m_shipType;
    double m_draft;
    double m_length;
    double m_beam;
    double m_blockCoefficient;
    double m_height;
    double m_grossTonnage;
    double m_netTonnage;
    double m_maxSpeed;
};

}
#endif
