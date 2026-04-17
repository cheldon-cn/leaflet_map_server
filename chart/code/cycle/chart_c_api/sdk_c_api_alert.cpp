/**
 * @file sdk_c_api_alert.cpp
 * @brief OGC Chart SDK C API - Alert Module Implementation
 * @version v2.0
 * @date 2026-04-17
 */

#include "sdk_c_api.h"

#include <ogc/alert/types.h>
#include <ogc/alert/alert_engine.h>
#include <ogc/alert/acknowledge_service.h>
#include <ogc/alert/cpa_calculator.h>
#include <ogc/alert/ukc_calculator.h>

#include <cstring>
#include <cstdlib>
#include <memory>
#include <string>
#include <vector>

using namespace ogc;
using namespace ogc::alert;

#ifdef __cplusplus
extern "C" {
#endif

namespace { static std::string SafeString(const char* str) {
    return str ? std::string(str) : std::string();
}

AlertType FromCAlertType(ogc_alert_type_e type) {
    switch (type) {
        case OGC_ALERT_TYPE_DEPTH: return AlertType::kDepth;
        case OGC_ALERT_TYPE_WEATHER: return AlertType::kWeather;
        case OGC_ALERT_TYPE_COLLISION: return AlertType::kCollision;
        case OGC_ALERT_TYPE_CHANNEL: return AlertType::kChannel;
        case OGC_ALERT_TYPE_DEVIATION: return AlertType::kDeviation;
        case OGC_ALERT_TYPE_SPEED: return AlertType::kSpeed;
        case OGC_ALERT_TYPE_RESTRICTED_AREA: return AlertType::kRestrictedArea;
        default: return AlertType::kUnknown;
    }
}

ogc_alert_type_e ToCAlertType(AlertType type) {
    switch (type) {
        case AlertType::kDepth: return OGC_ALERT_TYPE_DEPTH;
        case AlertType::kWeather: return OGC_ALERT_TYPE_WEATHER;
        case AlertType::kCollision: return OGC_ALERT_TYPE_COLLISION;
        case AlertType::kChannel: return OGC_ALERT_TYPE_CHANNEL;
        case AlertType::kDeviation: return OGC_ALERT_TYPE_DEVIATION;
        case AlertType::kSpeed: return OGC_ALERT_TYPE_SPEED;
        case AlertType::kRestrictedArea: return OGC_ALERT_TYPE_RESTRICTED_AREA;
        default: return OGC_ALERT_TYPE_DEPTH;
    }
}

AlertLevel FromCAlertLevel(ogc_alert_level_e level) {
    switch (level) {
        case OGC_ALERT_LEVEL_INFO: return AlertLevel::kLevel1;
        case OGC_ALERT_LEVEL_WARNING: return AlertLevel::kLevel2;
        case OGC_ALERT_LEVEL_ALARM: return AlertLevel::kLevel3;
        case OGC_ALERT_LEVEL_CRITICAL: return AlertLevel::kLevel4;
        default: return AlertLevel::kNone;
    }
}

ogc_alert_level_e ToCAlertLevel(AlertLevel level) {
    switch (level) {
        case AlertLevel::kLevel1: return OGC_ALERT_LEVEL_INFO;
        case AlertLevel::kLevel2: return OGC_ALERT_LEVEL_WARNING;
        case AlertLevel::kLevel3: return OGC_ALERT_LEVEL_ALARM;
        case AlertLevel::kLevel4: return OGC_ALERT_LEVEL_CRITICAL;
        default: return OGC_ALERT_LEVEL_INFO;
    }
}

struct AlertWrapper {
    AlertEvent event;
    std::string message;
    int64_t id;
    ogc_alert_severity_e severity;
    bool acknowledged;
};

struct AlertEngineContext {
    IAlertEngine* engine;
    IAcknowledgeService* acknowledgeService;
    std::vector<AlertWrapper*> activeAlerts;
    
    AlertEngineContext() : engine(nullptr), acknowledgeService(nullptr) {}
    ~AlertEngineContext() {
        for (auto* alert : activeAlerts) {
            delete alert;
        }
        activeAlerts.clear();
        delete acknowledgeService;
        delete engine;
    }
};

static AlertEngineContext* g_alertEngineCtx = nullptr;

}  

ogc_alert_t* ogc_alert_create(ogc_alert_type_e type, ogc_alert_level_e level, const char* message) {
    AlertWrapper* wrapper = new AlertWrapper();
    wrapper->event.event_type = "alert";
    wrapper->event.priority = static_cast<int>(FromCAlertLevel(level));
    wrapper->message = SafeString(message);
    wrapper->id = 0;
    wrapper->severity = OGC_ALERT_SEVERITY_MEDIUM;
    wrapper->acknowledged = false;
    return reinterpret_cast<ogc_alert_t*>(wrapper);
}

void ogc_alert_destroy(ogc_alert_t* alert) {
    delete reinterpret_cast<AlertWrapper*>(alert);
}

ogc_alert_type_e ogc_alert_get_type(const ogc_alert_t* alert) {
    if (alert) {
        return ToCAlertType(AlertType::kUnknown);
    }
    return OGC_ALERT_TYPE_DEPTH;
}

ogc_alert_level_e ogc_alert_get_level(const ogc_alert_t* alert) {
    if (alert) {
        const AlertWrapper* wrapper = reinterpret_cast<const AlertWrapper*>(alert);
        return ToCAlertLevel(static_cast<AlertLevel>(wrapper->event.priority));
    }
    return OGC_ALERT_LEVEL_INFO;
}

const char* ogc_alert_get_message(const ogc_alert_t* alert) {
    if (alert) {
        const AlertWrapper* wrapper = reinterpret_cast<const AlertWrapper*>(alert);
        return wrapper->message.c_str();
    }
    return "";
}

double ogc_alert_get_timestamp(const ogc_alert_t* alert) {
    if (alert) {
        const AlertWrapper* wrapper = reinterpret_cast<const AlertWrapper*>(alert);
        return static_cast<double>(wrapper->event.timestamp.ToTimestamp());
    }
    return 0.0;
}

ogc_coordinate_t ogc_alert_get_position(const ogc_alert_t* alert) {
    ogc_coordinate_t result = {0, 0, 0, 0};
    if (alert) {
        const AlertWrapper* wrapper = reinterpret_cast<const AlertWrapper*>(alert);
        result.x = wrapper->event.position.longitude;
        result.y = wrapper->event.position.latitude;
        result.z = wrapper->event.position.altitude;
    }
    return result;
}

void ogc_alert_set_position(ogc_alert_t* alert, const ogc_coordinate_t* pos) {
    if (alert && pos) {
        AlertWrapper* wrapper = reinterpret_cast<AlertWrapper*>(alert);
        wrapper->event.position.longitude = pos->x;
        wrapper->event.position.latitude = pos->y;
        wrapper->event.position.altitude = pos->z;
    }
}

void ogc_alert_set_id(ogc_alert_t* alert, int64_t id) {
    if (alert) {
        reinterpret_cast<AlertWrapper*>(alert)->id = id;
    }
}

int64_t ogc_alert_get_id(const ogc_alert_t* alert) {
    if (alert) {
        return reinterpret_cast<const AlertWrapper*>(alert)->id;
    }
    return 0;
}

void ogc_alert_set_severity(ogc_alert_t* alert, ogc_alert_severity_e severity) {
    if (alert) {
        reinterpret_cast<AlertWrapper*>(alert)->severity = severity;
    }
}

ogc_alert_severity_e ogc_alert_get_severity(const ogc_alert_t* alert) {
    if (alert) {
        return reinterpret_cast<const AlertWrapper*>(alert)->severity;
    }
    return OGC_ALERT_SEVERITY_LOW;
}

void ogc_alert_set_message(ogc_alert_t* alert, const char* message) {
    if (alert) {
        reinterpret_cast<AlertWrapper*>(alert)->message = SafeString(message);
    }
}

void ogc_alert_set_timestamp(ogc_alert_t* alert, int64_t timestamp) {
    if (alert) {
        AlertWrapper* wrapper = reinterpret_cast<AlertWrapper*>(alert);
        wrapper->event.timestamp = DateTime::FromTimestamp(timestamp);
    }
}

void ogc_alert_set_acknowledged(ogc_alert_t* alert, int acknowledged) {
    if (alert) {
        reinterpret_cast<AlertWrapper*>(alert)->acknowledged = (acknowledged != 0);
    }
}

int ogc_alert_is_acknowledged(const ogc_alert_t* alert) {
    if (alert) {
        return reinterpret_cast<const AlertWrapper*>(alert)->acknowledged ? 1 : 0;
    }
    return 0;
}

ogc_alert_engine_t* ogc_alert_engine_create(void) {
    if (!g_alertEngineCtx) {
        g_alertEngineCtx = new AlertEngineContext();
        g_alertEngineCtx->engine = IAlertEngine::Create().release();
        g_alertEngineCtx->acknowledgeService = IAcknowledgeService::Create().release();
    }
    return reinterpret_cast<ogc_alert_engine_t*>(g_alertEngineCtx);
}

void ogc_alert_engine_destroy(ogc_alert_engine_t* engine) {
    if (engine && g_alertEngineCtx) {
        delete g_alertEngineCtx;
        g_alertEngineCtx = nullptr;
    }
}

int ogc_alert_engine_initialize(ogc_alert_engine_t* engine) {
    AlertEngineContext* ctx = reinterpret_cast<AlertEngineContext*>(engine);
    if (ctx && ctx->engine) {
        EngineConfig config;
        config.check_interval_ms = 1000;
        config.max_concurrent_checks = 10;
        config.enable_deduplication = true;
        config.enable_aggregation = false;
        config.dedup_window_seconds = 60;
        ctx->engine->Initialize(config);
        return 0;
    }
    return -1;
}

void ogc_alert_engine_shutdown(ogc_alert_engine_t* engine) {
    AlertEngineContext* ctx = reinterpret_cast<AlertEngineContext*>(engine);
    if (ctx && ctx->engine) {
        ctx->engine->Stop();
    }
}

void ogc_alert_engine_check_all(ogc_alert_engine_t* engine) {
    AlertEngineContext* ctx = reinterpret_cast<AlertEngineContext*>(engine);
    if (ctx && ctx->engine) {
        ctx->engine->Start();
        AlertStatistics stats = ctx->engine->GetStatistics();
        (void)stats;
    }
}

ogc_alert_t** ogc_alert_engine_get_active_alerts(ogc_alert_engine_t* engine, int* count) {
    AlertEngineContext* ctx = reinterpret_cast<AlertEngineContext*>(engine);
    if (count) *count = 0;
    if (ctx && count) {
        *count = static_cast<int>(ctx->activeAlerts.size());
        if (!ctx->activeAlerts.empty()) {
            ogc_alert_t** alerts = static_cast<ogc_alert_t**>(
                std::malloc(sizeof(ogc_alert_t*) * ctx->activeAlerts.size()));
            for (size_t i = 0; i < ctx->activeAlerts.size(); ++i) {
                alerts[i] = reinterpret_cast<ogc_alert_t*>(ctx->activeAlerts[i]);
            }
            return alerts;
        }
    }
    return nullptr;
}

void ogc_alert_engine_free_alerts(ogc_alert_t** alerts) {
    if (alerts) {
        std::free(alerts);
    }
}

void ogc_alert_engine_acknowledge_alert(ogc_alert_engine_t* engine, const ogc_alert_t* alert) {
    AlertEngineContext* ctx = reinterpret_cast<AlertEngineContext*>(engine);
    const AlertWrapper* wrapper = reinterpret_cast<const AlertWrapper*>(alert);
    if (ctx && ctx->acknowledgeService && wrapper) {
        std::string alertId = std::to_string(wrapper->id);
        ctx->acknowledgeService->AcknowledgeAlert(alertId, "system", "acknowledge");
        const_cast<AlertWrapper*>(wrapper)->acknowledged = true;
    }
}

int ogc_alert_engine_add_alert(ogc_alert_engine_t* engine, ogc_alert_t* alert) {
    AlertEngineContext* ctx = reinterpret_cast<AlertEngineContext*>(engine);
    AlertWrapper* wrapper = reinterpret_cast<AlertWrapper*>(alert);
    if (ctx && ctx->engine && wrapper) {
        ctx->engine->SubmitEvent(wrapper->event);
        ctx->activeAlerts.push_back(wrapper);
        return 0;
    }
    return -1;
}

int ogc_alert_engine_remove_alert(ogc_alert_engine_t* engine, int64_t id) {
    AlertEngineContext* ctx = reinterpret_cast<AlertEngineContext*>(engine);
    if (ctx) {
        for (auto it = ctx->activeAlerts.begin(); it != ctx->activeAlerts.end(); ++it) {
            if ((*it)->id == id) {
                ctx->activeAlerts.erase(it);
                return 0;
            }
        }
    }
    return -1;
}

ogc_alert_t* ogc_alert_engine_get_alert(ogc_alert_engine_t* engine, int64_t id) {
    (void)engine;
    (void)id;
    return nullptr;
}

size_t ogc_alert_engine_get_alert_count(const ogc_alert_engine_t* engine) {
    if (engine) {
        auto stats = reinterpret_cast<const AlertEngine*>(engine)->GetStatistics();
        return static_cast<size_t>(stats.total_alerts_generated);
    }
    return 0;
}

void ogc_alert_engine_clear_alerts(ogc_alert_engine_t* engine) {
    (void)engine;
}

void ogc_alert_engine_set_callback(ogc_alert_engine_t* engine,
    void (*callback)(int alert_type, int alert_level, const char* message, void* user_data),
    void* user_data) {
    if (engine && callback) {
        (void)callback;
        (void)user_data;
    }
}

size_t ogc_alert_engine_get_active_alert_count(const ogc_alert_engine_t* engine) {
    if (engine) {
        auto stats = reinterpret_cast<const AlertEngine*>(engine)->GetStatistics();
        return static_cast<size_t>(stats.active_alerts);
    }
    return 0;
}

int ogc_alert_engine_update(ogc_alert_engine_t* engine, double dt) {
    (void)engine;
    (void)dt;
    return 0;
}

void ogc_cpa_calculate(double own_lat, double own_lon, double own_speed, double own_course,
                       double target_lat, double target_lon, double target_speed, double target_course,
                       ogc_cpa_result_t* result) {
    if (result) {
        ShipMotion own;
        own.position.longitude = own_lon;
        own.position.latitude = own_lat;
        own.heading = own_course;
        own.speed = own_speed;
        own.UpdateVelocity();
        
        ShipMotion target;
        target.position.longitude = target_lon;
        target.position.latitude = target_lat;
        target.heading = target_course;
        target.speed = target_speed;
        target.UpdateVelocity();
        
        CPACalculator calculator;
        CPAResult cpa_result = calculator.Calculate(own, target);
        
        result->cpa = cpa_result.cpa;
        result->tcpa = cpa_result.tcpa;
        result->bearing = cpa_result.relative_bearing;
        result->danger_level = cpa_result.is_dangerous ? 2 : 0;
    }
}

void ogc_cpa_result_destroy(ogc_cpa_result_t* result) {
    std::free(result);
}

void ogc_ukc_calculate(double charted_depth, double tide_height, double draft, 
                       double speed, double squat_coeff, ogc_ukc_result_t* result) {
    if (result) {
        UKCInput input;
        input.chart_depth = charted_depth;
        input.tide_height = tide_height;
        input.ship_draft = draft;
        input.speed_knots = speed;
        input.squat = squat_coeff;
        input.heel_correction = 0.0;
        input.wave_allowance = 0.0;
        input.safety_margin = 0.0;
        input.water_density = 1.025;
        input.depth_at_position = charted_depth;
        
        UKCCalculator calculator;
        UKCResult ukc_result = calculator.Calculate(input);
        
        result->water_depth = ukc_result.effective_depth;
        result->charted_depth = charted_depth;
        result->tide_height = tide_height;
        result->squat = ukc_result.squat_estimate;
        result->ukc = ukc_result.ukc;
        result->safe = ukc_result.is_safe ? 1 : 0;
    }
}

void ogc_ukc_result_destroy(ogc_ukc_result_t* result) {
    std::free(result);
}
#ifdef __cplusplus
}
#endif
