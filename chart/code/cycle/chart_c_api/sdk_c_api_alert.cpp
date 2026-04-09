/**
 * @file sdk_c_api_alert.cpp
 * @brief OGC Chart SDK C API - Alert Module Implementation
 * @version v1.0
 * @date 2026-04-09
 */

#include "sdk_c_api.h"

#include <ogc/alert/types.h>
#include <ogc/alert/alert_engine.h>
#include <ogc/alert/cpa_calculator.h>
#include <ogc/alert/ukc_calculator.h>

#include <cstring>
#include <cstdlib>
#include <memory>
#include <string>

using namespace ogc;

#ifdef __cplusplus
extern "C" {
#endif

namespace {

std::string SafeString(const char* str) {
    return str ? std::string(str) : std::string();
}

}  

ogc_alert_t* ogc_alert_create(int type, int severity) {
    return reinterpret_cast<ogc_alert_t*>(
        Alert::Create(static_cast<AlertType>(type), static_cast<AlertSeverity>(severity)).release());
}

void ogc_alert_destroy(ogc_alert_t* alert) {
    delete reinterpret_cast<Alert*>(alert);
}

int ogc_alert_get_type(const ogc_alert_t* alert) {
    if (alert) {
        return static_cast<int>(reinterpret_cast<const Alert*>(alert)->GetType());
    }
    return 0;
}

int ogc_alert_get_severity(const ogc_alert_t* alert) {
    if (alert) {
        return static_cast<int>(reinterpret_cast<const Alert*>(alert)->GetSeverity());
    }
    return 0;
}

const char* ogc_alert_get_message(const ogc_alert_t* alert) {
    if (alert) {
        return reinterpret_cast<const Alert*>(alert)->GetMessage().c_str();
    }
    return "";
}

void ogc_alert_set_message(ogc_alert_t* alert, const char* message) {
    if (alert) {
        reinterpret_cast<Alert*>(alert)->SetMessage(SafeString(message));
    }
}

const char* ogc_alert_get_source(const ogc_alert_t* alert) {
    if (alert) {
        return reinterpret_cast<const Alert*>(alert)->GetSource().c_str();
    }
    return "";
}

void ogc_alert_set_source(ogc_alert_t* alert, const char* source) {
    if (alert) {
        reinterpret_cast<Alert*>(alert)->SetSource(SafeString(source));
    }
}

double ogc_alert_get_timestamp(const ogc_alert_t* alert) {
    if (alert) {
        return reinterpret_cast<const Alert*>(alert)->GetTimestamp();
    }
    return 0.0;
}

void ogc_alert_set_timestamp(ogc_alert_t* alert, double timestamp) {
    if (alert) {
        reinterpret_cast<Alert*>(alert)->SetTimestamp(timestamp);
    }
}

int ogc_alert_is_active(const ogc_alert_t* alert) {
    if (alert) {
        return reinterpret_cast<const Alert*>(alert)->IsActive() ? 1 : 0;
    }
    return 0;
}

void ogc_alert_set_active(ogc_alert_t* alert, int active) {
    if (alert) {
        reinterpret_cast<Alert*>(alert)->SetActive(active != 0);
    }
}

int ogc_alert_is_acknowledged(const ogc_alert_t* alert) {
    if (alert) {
        return reinterpret_cast<const Alert*>(alert)->IsAcknowledged() ? 1 : 0;
    }
    return 0;
}

void ogc_alert_acknowledge(ogc_alert_t* alert) {
    if (alert) {
        reinterpret_cast<Alert*>(alert)->Acknowledge();
    }
}

ogc_alert_engine_t* ogc_alert_engine_create(void) {
    return reinterpret_cast<ogc_alert_engine_t*>(AlertEngine::Create().release());
}

void ogc_alert_engine_destroy(ogc_alert_engine_t* engine) {
    delete reinterpret_cast<AlertEngine*>(engine);
}

int ogc_alert_engine_initialize(ogc_alert_engine_t* engine) {
    if (engine) {
        return reinterpret_cast<AlertEngine*>(engine)->Initialize() ? 1 : 0;
    }
    return 0;
}

void ogc_alert_engine_shutdown(ogc_alert_engine_t* engine) {
    if (engine) {
        reinterpret_cast<AlertEngine*>(engine)->Shutdown();
    }
}

int ogc_alert_engine_add_alert(ogc_alert_engine_t* engine, ogc_alert_t* alert) {
    if (engine && alert) {
        return reinterpret_cast<AlertEngine*>(engine)->AddAlert(
            AlertPtr(reinterpret_cast<Alert*>(alert))) ? 1 : 0;
    }
    return 0;
}

int ogc_alert_engine_remove_alert(ogc_alert_engine_t* engine, const ogc_alert_t* alert) {
    if (engine && alert) {
        return reinterpret_cast<AlertEngine*>(engine)->RemoveAlert(
            *reinterpret_cast<const Alert*>(alert)) ? 1 : 0;
    }
    return 0;
}

size_t ogc_alert_engine_get_alert_count(const ogc_alert_engine_t* engine) {
    if (engine) {
        return reinterpret_cast<const AlertEngine*>(engine)->GetAlertCount();
    }
    return 0;
}

ogc_alert_t* ogc_alert_engine_get_alert(ogc_alert_engine_t* engine, size_t index) {
    if (engine) {
        return reinterpret_cast<ogc_alert_t*>(
            reinterpret_cast<AlertEngine*>(engine)->GetAlert(index));
    }
    return nullptr;
}

void ogc_alert_engine_clear_alerts(ogc_alert_engine_t* engine) {
    if (engine) {
        reinterpret_cast<AlertEngine*>(engine)->ClearAlerts();
    }
}

size_t ogc_alert_engine_get_active_alert_count(const ogc_alert_engine_t* engine) {
    if (engine) {
        return reinterpret_cast<const AlertEngine*>(engine)->GetActiveAlertCount();
    }
    return 0;
}

int ogc_alert_engine_update(ogc_alert_engine_t* engine, double dt) {
    if (engine) {
        return reinterpret_cast<AlertEngine*>(engine)->Update(dt) ? 1 : 0;
    }
    return 0;
}

ogc_cpa_result_t* ogc_cpa_calculate(const ogc_vessel_info_t* own_ship, const ogc_vessel_info_t* target_ship) {
    if (own_ship && target_ship) {
        VesselInfo own;
        own.position.x = own_ship->position.x;
        own.position.y = own_ship->position.y;
        own.course = own_ship->course;
        own.speed = own_ship->speed;
        
        VesselInfo target;
        target.position.x = target_ship->position.x;
        target.position.y = target_ship->position.y;
        target.course = target_ship->course;
        target.speed = target_ship->speed;
        
        CPAResult result = CPACalculator::Calculate(own, target);
        
        ogc_cpa_result_t* cpa_result = static_cast<ogc_cpa_result_t*>(std::malloc(sizeof(ogc_cpa_result_t)));
        if (cpa_result) {
            cpa_result->tcpa = result.tcpa;
            cpa_result->dcpa = result.dcpa;
            cpa_result->cpa_position.x = result.cpa_position.x;
            cpa_result->cpa_position.y = result.cpa_position.y;
            cpa_result->valid = result.valid ? 1 : 0;
        }
        return cpa_result;
    }
    return nullptr;
}

void ogc_cpa_result_destroy(ogc_cpa_result_t* result) {
    std::free(result);
}

ogc_ukc_result_t* ogc_ukc_calculate(double depth, double draft, double squat, double ukc_required) {
    UKCResult result = UKCCalculator::Calculate(depth, draft, squat, ukc_required);
    
    ogc_ukc_result_t* ukc_result = static_cast<ogc_ukc_result_t*>(std::malloc(sizeof(ogc_ukc_result_t)));
    if (ukc_result) {
        ukc_result->ukc = result.ukc;
        ukc_result->ukc_margin = result.ukc_margin;
        ukc_result->safe = result.safe ? 1 : 0;
        ukc_result->warning_level = result.warning_level;
    }
    return ukc_result;
}

void ogc_ukc_result_destroy(ogc_ukc_result_t* result) {
    std::free(result);
}

#ifdef __cplusplus
}
#endif
