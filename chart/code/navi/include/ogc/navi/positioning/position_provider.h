#pragma once

#include "ogc/navi/types.h"
#include "ogc/navi/export.h"
#include <string>
#include <functional>
#include <memory>

namespace ogc {
namespace navi {

struct ProviderConfig {
    std::string device_path;
    int baud_rate;
    int data_bits;
    int stop_bits;
    char parity;
    int timeout_ms;
    bool auto_reconnect;
    int retry_interval_ms;
    
    ProviderConfig()
        : baud_rate(4800)
        , data_bits(8)
        , stop_bits(1)
        , parity('N')
        , timeout_ms(1000)
        , auto_reconnect(true)
        , retry_interval_ms(5000)
    {}
};

using PositionCallback = std::function<void(const PositionData&)>;
using ErrorCallback = std::function<void(const std::string&)>;

class OGC_NAVI_API IPositionProvider {
public:
    virtual ~IPositionProvider() = default;
    
    virtual bool Initialize(const ProviderConfig& config) = 0;
    virtual void Shutdown() = 0;
    virtual bool IsConnected() const = 0;
    virtual PositionData GetLastPosition() const = 0;
    
    virtual void SetPositionCallback(PositionCallback callback) = 0;
    virtual void SetErrorCallback(ErrorCallback callback) = 0;
    
    static IPositionProvider* Create(PositionSource source);
};

}
}
