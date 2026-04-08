#pragma once

#include "ogc/navi/ais/ais_message.h"
#include "ogc/navi/export.h"
#include <string>
#include <vector>
#include <cstdint>

namespace ogc {
namespace navi {

class OGC_NAVI_API AisParser {
public:
    static AisParser& Instance();
    
    bool ParseNmeaMessage(const std::string& nmea, AisMessage& message);
    bool ParsePositionReport(const std::vector<uint8_t>& payload, AisPositionReport& report);
    bool ParseStaticData(const std::vector<uint8_t>& payload, AisStaticData& data);
    
    std::vector<uint8_t> DecodePayload(const std::string& payload);
    int GetMessageType(const std::vector<uint8_t>& payload);
    uint32_t GetMmsi(const std::vector<uint8_t>& payload);
    uint32_t GetBits(const std::vector<uint8_t>& payload, int start, int count);
    int32_t GetBitsSigned(const std::vector<uint8_t>& payload, int start, int count);
    std::string DecodeString(const std::vector<uint8_t>& payload, int start, int count);
    
private:
    AisParser();
    AisParser(const AisParser&) = delete;
    AisParser& operator=(const AisParser&) = delete;
};

}
}
