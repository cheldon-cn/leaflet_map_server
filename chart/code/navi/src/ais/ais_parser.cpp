#include "ogc/navi/ais/ais_parser.h"
#include "ogc/navi/ais/ais_message.h"
#include <sstream>
#include <cmath>
#include <algorithm>

namespace ogc {
namespace navi {

AisParser::AisParser()
{
}

AisParser& AisParser::Instance() {
    static AisParser instance;
    return instance;
}

bool AisParser::ParseNmeaMessage(const std::string& nmea, AisMessage& message) {
    if (nmea.empty()) {
        return false;
    }
    
    if (nmea.find("!AIVDM") == std::string::npos &&
        nmea.find("!AIVDO") == std::string::npos) {
        return false;
    }
    
    std::vector<std::string> fields;
    std::istringstream iss(nmea);
    std::string field;
    
    while (std::getline(iss, field, ',')) {
        fields.push_back(field);
    }
    
    if (fields.size() < 6) {
        return false;
    }
    
    std::string payload;
    if (fields.size() > 5) {
        payload = fields[5];
    }
    
    if (payload.empty()) {
        return false;
    }
    
    std::vector<uint8_t> binary = DecodePayload(payload);
    if (binary.empty()) {
        return false;
    }
    
    message.message_type = GetMessageType(binary);
    message.mmsi = GetMmsi(binary);
    message.timestamp = 0.0;
    
    return true;
}

bool AisParser::ParsePositionReport(const std::vector<uint8_t>& payload, AisPositionReport& report) {
    if (payload.size() < 28) {
        return false;
    }
    
    report.message_type = GetMessageType(payload);
    report.mmsi = GetMmsi(payload);
    
    int nav_status = GetBits(payload, 38, 4);
    report.nav_status = static_cast<AisNavigationStatus>(nav_status);
    
    int lon_raw = GetBitsSigned(payload, 61, 28);
    report.longitude = lon_raw / 600000.0;
    
    int lat_raw = GetBitsSigned(payload, 89, 27);
    report.latitude = lat_raw / 600000.0;
    
    int sog_raw = GetBits(payload, 50, 10);
    report.speed_over_ground = sog_raw / 10.0;
    
    int cog_raw = GetBits(payload, 116, 12);
    report.course_over_ground = cog_raw / 10.0;
    
    int hdg_raw = GetBits(payload, 128, 9);
    report.heading = hdg_raw;
    
    return true;
}

bool AisParser::ParseStaticData(const std::vector<uint8_t>& payload, AisStaticData& data) {
    if (payload.size() < 56) {
        return false;
    }
    
    data.mmsi = GetMmsi(payload);
    
    data.callsign = DecodeString(payload, 70, 42);
    data.ship_name = DecodeString(payload, 112, 120);
    
    int ship_type_raw = GetBits(payload, 232, 8);
    data.ship_type = static_cast<AisShipType>(ship_type_raw);
    
    int dim_to_bow = GetBits(payload, 240, 9);
    int dim_to_stern = GetBits(payload, 249, 9);
    int dim_to_port = GetBits(payload, 258, 6);
    int dim_to_starboard = GetBits(payload, 264, 6);
    
    data.dimension_to_bow = dim_to_bow;
    data.dimension_to_stern = dim_to_stern;
    data.dimension_to_port = dim_to_port;
    data.dimension_to_starboard = dim_to_starboard;
    
    data.eta_month = GetBits(payload, 274, 4);
    data.eta_day = GetBits(payload, 278, 5);
    data.eta_hour = GetBits(payload, 283, 5);
    data.eta_minute = GetBits(payload, 288, 6);
    
    int draught_raw = GetBits(payload, 294, 8);
    data.draught = draught_raw / 10.0;
    
    data.destination = DecodeString(payload, 302, 120);
    
    return true;
}

std::vector<uint8_t> AisParser::DecodePayload(const std::string& payload) {
    std::vector<uint8_t> result;
    
    int bit_count = 0;
    uint32_t bits = 0;
    
    for (char c : payload) {
        if (c < '0' || c > 'W') {
            continue;
        }
        
        int val = c - 48;
        if (val > 40) {
            val -= 8;
        }
        
        bits = (bits << 6) | val;
        bit_count += 6;
        
        while (bit_count >= 8) {
            bit_count -= 8;
            result.push_back(static_cast<uint8_t>((bits >> bit_count) & 0xFF));
        }
    }
    
    return result;
}

int AisParser::GetMessageType(const std::vector<uint8_t>& payload) {
    if (payload.empty()) {
        return 0;
    }
    return (payload[0] >> 2) & 0x3F;
}

uint32_t AisParser::GetMmsi(const std::vector<uint8_t>& payload) {
    if (payload.size() < 4) {
        return 0;
    }
    
    uint32_t mmsi = 0;
    mmsi = ((payload[0] & 0x03) << 28);
    mmsi |= (payload[1] << 20);
    mmsi |= (payload[2] << 12);
    mmsi |= (payload[3] << 4);
    mmsi |= ((payload[4] >> 4) & 0x0F);
    
    return mmsi;
}

uint32_t AisParser::GetBits(const std::vector<uint8_t>& payload, int start, int count) {
    if (payload.empty() || start < 0 || count <= 0 || count > 32) {
        return 0;
    }
    
    uint32_t result = 0;
    int bits_collected = 0;
    
    while (bits_collected < count) {
        int bit_index = start + bits_collected;
        int byte_index = bit_index / 8;
        int bit_offset = 7 - (bit_index % 8);
        
        if (byte_index >= static_cast<int>(payload.size())) {
            break;
        }
        
        int bit = (payload[byte_index] >> bit_offset) & 1;
        result = (result << 1) | bit;
        bits_collected++;
    }
    
    return result;
}

int32_t AisParser::GetBitsSigned(const std::vector<uint8_t>& payload, int start, int count) {
    uint32_t unsigned_val = GetBits(payload, start, count);
    
    if (count > 0 && (unsigned_val & (1U << (count - 1)))) {
        unsigned_val |= ~((1U << count) - 1);
    }
    
    return static_cast<int32_t>(unsigned_val);
}

std::string AisParser::DecodeString(const std::vector<uint8_t>& payload, int start, int count) {
    std::string result;
    int char_count = count / 6;
    
    for (int i = 0; i < char_count; ++i) {
        int val = GetBits(payload, start + i * 6, 6);
        
        if (val < 32) {
            val += 64;
        }
        
        if (val >= 32 && val < 128) {
            result += static_cast<char>(val);
        }
    }
    
    while (!result.empty() && result.back() == '@') {
        result.pop_back();
    }
    
    return result;
}

}
}
