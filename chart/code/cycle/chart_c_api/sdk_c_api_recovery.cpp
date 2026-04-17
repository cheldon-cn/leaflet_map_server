/**
 * @file sdk_c_api_recovery.cpp
 * @brief OGC Chart SDK C API - Recovery Module Implementation
 * @version v1.0
 * @date 2026-04-17
 */

#include "sdk_c_api.h"

#include <cstring>
#include <cstdlib>
#include <memory>
#include <string>
#include <map>
#include <set>
#include <chrono>
#include <mutex>

#ifdef __cplusplus
extern "C" {
#endif

namespace {

struct CircuitBreakerData {
    int failureThreshold;
    int recoveryTimeoutMs;
    ogc_circuit_state_e state;
    int failureCount;
    int successCount;
    std::chrono::steady_clock::time_point lastFailureTime;
    std::mutex mutex;
    
    CircuitBreakerData(int threshold, int timeout) 
        : failureThreshold(threshold)
        , recoveryTimeoutMs(timeout)
        , state(OGC_CIRCUIT_CLOSED)
        , failureCount(0)
        , successCount(0) {}
};

struct GracefulDegradationData {
    ogc_degradation_level_e level;
    std::set<std::string> disabledFeatures;
    std::mutex mutex;
    
    GracefulDegradationData() : level(OGC_DEGRADATION_NONE) {}
};

}

ogc_circuit_breaker_t* ogc_circuit_breaker_create(int failure_threshold, int recovery_timeout_ms) {
    if (failure_threshold <= 0 || recovery_timeout_ms <= 0) {
        return nullptr;
    }
    CircuitBreakerData* data = new CircuitBreakerData(failure_threshold, recovery_timeout_ms);
    return reinterpret_cast<ogc_circuit_breaker_t*>(data);
}

void ogc_circuit_breaker_destroy(ogc_circuit_breaker_t* breaker) {
    if (breaker) {
        delete reinterpret_cast<CircuitBreakerData*>(breaker);
    }
}

ogc_circuit_state_e ogc_circuit_breaker_get_state(const ogc_circuit_breaker_t* breaker) {
    if (breaker) {
        CircuitBreakerData* data = reinterpret_cast<CircuitBreakerData*>(const_cast<ogc_circuit_breaker_t*>(breaker));
        std::lock_guard<std::mutex> lock(data->mutex);
        
        if (data->state == OGC_CIRCUIT_OPEN) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - data->lastFailureTime);
            if (elapsed.count() >= data->recoveryTimeoutMs) {
                data->state = OGC_CIRCUIT_HALF_OPEN;
            }
        }
        return data->state;
    }
    return OGC_CIRCUIT_CLOSED;
}

int ogc_circuit_breaker_allow_request(ogc_circuit_breaker_t* breaker) {
    if (breaker) {
        CircuitBreakerData* data = reinterpret_cast<CircuitBreakerData*>(breaker);
        std::lock_guard<std::mutex> lock(data->mutex);
        
        if (data->state == OGC_CIRCUIT_CLOSED) {
            return 1;
        }
        
        if (data->state == OGC_CIRCUIT_OPEN) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - data->lastFailureTime);
            if (elapsed.count() >= data->recoveryTimeoutMs) {
                data->state = OGC_CIRCUIT_HALF_OPEN;
                return 1;
            }
            return 0;
        }
        
        if (data->state == OGC_CIRCUIT_HALF_OPEN) {
            return 1;
        }
    }
    return 0;
}

void ogc_circuit_breaker_record_success(ogc_circuit_breaker_t* breaker) {
    if (breaker) {
        CircuitBreakerData* data = reinterpret_cast<CircuitBreakerData*>(breaker);
        std::lock_guard<std::mutex> lock(data->mutex);
        
        data->successCount++;
        data->failureCount = 0;
        
        if (data->state == OGC_CIRCUIT_HALF_OPEN) {
            data->state = OGC_CIRCUIT_CLOSED;
        }
    }
}

void ogc_circuit_breaker_record_failure(ogc_circuit_breaker_t* breaker) {
    if (breaker) {
        CircuitBreakerData* data = reinterpret_cast<CircuitBreakerData*>(breaker);
        std::lock_guard<std::mutex> lock(data->mutex);
        
        data->failureCount++;
        data->lastFailureTime = std::chrono::steady_clock::now();
        
        if (data->state == OGC_CIRCUIT_HALF_OPEN) {
            data->state = OGC_CIRCUIT_OPEN;
        } else if (data->failureCount >= data->failureThreshold) {
            data->state = OGC_CIRCUIT_OPEN;
        }
    }
}

void ogc_circuit_breaker_reset(ogc_circuit_breaker_t* breaker) {
    if (breaker) {
        CircuitBreakerData* data = reinterpret_cast<CircuitBreakerData*>(breaker);
        std::lock_guard<std::mutex> lock(data->mutex);
        
        data->state = OGC_CIRCUIT_CLOSED;
        data->failureCount = 0;
        data->successCount = 0;
    }
}

ogc_graceful_degradation_t* ogc_graceful_degradation_create(void) {
    GracefulDegradationData* data = new GracefulDegradationData();
    return reinterpret_cast<ogc_graceful_degradation_t*>(data);
}

void ogc_graceful_degradation_destroy(ogc_graceful_degradation_t* degradation) {
    if (degradation) {
        delete reinterpret_cast<GracefulDegradationData*>(degradation);
    }
}

ogc_degradation_level_e ogc_graceful_degradation_get_level(const ogc_graceful_degradation_t* degradation) {
    if (degradation) {
        GracefulDegradationData* data = reinterpret_cast<GracefulDegradationData*>(const_cast<ogc_graceful_degradation_t*>(degradation));
        std::lock_guard<std::mutex> lock(data->mutex);
        return data->level;
    }
    return OGC_DEGRADATION_NONE;
}

void ogc_graceful_degradation_set_level(ogc_graceful_degradation_t* degradation, ogc_degradation_level_e level) {
    if (degradation) {
        GracefulDegradationData* data = reinterpret_cast<GracefulDegradationData*>(degradation);
        std::lock_guard<std::mutex> lock(data->mutex);
        data->level = level;
    }
}

int ogc_graceful_degradation_is_feature_enabled(const ogc_graceful_degradation_t* degradation, const char* feature) {
    if (degradation && feature) {
        GracefulDegradationData* data = reinterpret_cast<GracefulDegradationData*>(const_cast<ogc_graceful_degradation_t*>(degradation));
        std::lock_guard<std::mutex> lock(data->mutex);
        return data->disabledFeatures.find(std::string(feature)) == data->disabledFeatures.end() ? 1 : 0;
    }
    return 0;
}

void ogc_graceful_degradation_disable_feature(ogc_graceful_degradation_t* degradation, const char* feature) {
    if (degradation && feature) {
        GracefulDegradationData* data = reinterpret_cast<GracefulDegradationData*>(degradation);
        std::lock_guard<std::mutex> lock(data->mutex);
        data->disabledFeatures.insert(std::string(feature));
    }
}

void ogc_graceful_degradation_enable_feature(ogc_graceful_degradation_t* degradation, const char* feature) {
    if (degradation && feature) {
        GracefulDegradationData* data = reinterpret_cast<GracefulDegradationData*>(degradation);
        std::lock_guard<std::mutex> lock(data->mutex);
        data->disabledFeatures.erase(std::string(feature));
    }
}

#ifdef __cplusplus
}
#endif
