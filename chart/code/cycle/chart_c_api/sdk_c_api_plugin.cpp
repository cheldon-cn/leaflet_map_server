/**
 * @file sdk_c_api_plugin.cpp
 * @brief OGC Chart SDK C API - Plugin, Recovery, Health, Loader, Exception Modules Implementation
 * @version v1.0
 * @date 2026-04-09
 */

#include "sdk_c_api.h"

#include <ogc/plugin/plugin_manager.h>
#include <ogc/plugin/plugin_interface.h>
#include <ogc/base/error_recovery.h>
#include <ogc/base/health_check.h>
#include <ogc/base/library_loader.h>
#include <ogc/base/exception.h>

#include <cstring>
#include <cstdlib>
#include <memory>
#include <string>

using namespace ogc::plugin;
using namespace ogc::base;

#ifdef __cplusplus
extern "C" {
#endif

namespace {

std::string SafeString(const char* str) {
    return str ? std::string(str) : std::string();
}

char* AllocString(const std::string& str) {
    char* result = static_cast<char*>(std::malloc(str.size() + 1));
    if (result) {
        std::memcpy(result, str.c_str(), str.size() + 1);
    }
    return result;
}

}  

ogc_plugin_manager_t* ogc_plugin_manager_get_instance(void) {
    return reinterpret_cast<ogc_plugin_manager_t*>(&PluginManager::Instance());
}

int ogc_plugin_manager_load_plugin(ogc_plugin_manager_t* manager, const char* path) {
    if (manager && path) {
        return reinterpret_cast<PluginManager*>(manager)->LoadPlugin(SafeString(path)) ? 1 : 0;
    }
    return 0;
}

int ogc_plugin_manager_unload_plugin(ogc_plugin_manager_t* manager, const char* name) {
    if (manager && name) {
        return reinterpret_cast<PluginManager*>(manager)->UnloadPlugin(SafeString(name)) ? 1 : 0;
    }
    return 0;
}

size_t ogc_plugin_manager_get_plugin_count(const ogc_plugin_manager_t* manager) {
    if (manager) {
        return reinterpret_cast<const PluginManager*>(manager)->GetPluginCount();
    }
    return 0;
}

const char* ogc_plugin_manager_get_plugin_name(const ogc_plugin_manager_t* manager, size_t index) {
    if (manager) {
        return reinterpret_cast<const PluginManager*>(manager)->GetPluginName(index).c_str();
    }
    return "";
}

const char* ogc_plugin_manager_get_plugin_version(const ogc_plugin_manager_t* manager, size_t index) {
    if (manager) {
        return reinterpret_cast<const PluginManager*>(manager)->GetPluginVersion(index).c_str();
    }
    return "";
}

int ogc_plugin_manager_is_plugin_loaded(const ogc_plugin_manager_t* manager, const char* name) {
    if (manager && name) {
        return reinterpret_cast<const PluginManager*>(manager)->IsPluginLoaded(SafeString(name)) ? 1 : 0;
    }
    return 0;
}

int ogc_plugin_manager_enable_plugin(ogc_plugin_manager_t* manager, const char* name) {
    if (manager && name) {
        return reinterpret_cast<PluginManager*>(manager)->EnablePlugin(SafeString(name)) ? 1 : 0;
    }
    return 0;
}

int ogc_plugin_manager_disable_plugin(ogc_plugin_manager_t* manager, const char* name) {
    if (manager && name) {
        return reinterpret_cast<PluginManager*>(manager)->DisablePlugin(SafeString(name)) ? 1 : 0;
    }
    return 0;
}

ogc_plugin_t* ogc_plugin_manager_get_plugin(ogc_plugin_manager_t* manager, const char* name) {
    if (manager && name) {
        return reinterpret_cast<ogc_plugin_t*>(
            reinterpret_cast<PluginManager*>(manager)->GetPlugin(SafeString(name)));
    }
    return nullptr;
}

const char* ogc_plugin_get_name(const ogc_plugin_t* plugin) {
    if (plugin) {
        return reinterpret_cast<const Plugin*>(plugin)->GetName().c_str();
    }
    return "";
}

const char* ogc_plugin_get_version(const ogc_plugin_t* plugin) {
    if (plugin) {
        return reinterpret_cast<const Plugin*>(plugin)->GetVersion().c_str();
    }
    return "";
}

const char* ogc_plugin_get_description(const ogc_plugin_t* plugin) {
    if (plugin) {
        return reinterpret_cast<const Plugin*>(plugin)->GetDescription().c_str();
    }
    return "";
}

int ogc_plugin_is_enabled(const ogc_plugin_t* plugin) {
    if (plugin) {
        return reinterpret_cast<const Plugin*>(plugin)->IsEnabled() ? 1 : 0;
    }
    return 0;
}

int ogc_plugin_initialize(ogc_plugin_t* plugin) {
    if (plugin) {
        return reinterpret_cast<Plugin*>(plugin)->Initialize() ? 1 : 0;
    }
    return 0;
}

void ogc_plugin_shutdown(ogc_plugin_t* plugin) {
    if (plugin) {
        reinterpret_cast<Plugin*>(plugin)->Shutdown();
    }
}

ogc_error_recovery_t* ogc_error_recovery_create(void) {
    return reinterpret_cast<ogc_error_recovery_t*>(ErrorRecovery::Create().release());
}

void ogc_error_recovery_destroy(ogc_error_recovery_t* recovery) {
    delete reinterpret_cast<ErrorRecovery*>(recovery);
}

int ogc_error_recovery_initialize(ogc_error_recovery_t* recovery) {
    if (recovery) {
        return reinterpret_cast<ErrorRecovery*>(recovery)->Initialize() ? 1 : 0;
    }
    return 0;
}

void ogc_error_recovery_shutdown(ogc_error_recovery_t* recovery) {
    if (recovery) {
        reinterpret_cast<ErrorRecovery*>(recovery)->Shutdown();
    }
}

int ogc_error_recovery_save_state(ogc_error_recovery_t* recovery, const char* name) {
    if (recovery && name) {
        return reinterpret_cast<ErrorRecovery*>(recovery)->SaveState(SafeString(name)) ? 1 : 0;
    }
    return 0;
}

int ogc_error_recovery_restore_state(ogc_error_recovery_t* recovery, const char* name) {
    if (recovery && name) {
        return reinterpret_cast<ErrorRecovery*>(recovery)->RestoreState(SafeString(name)) ? 1 : 0;
    }
    return 0;
}

int ogc_error_recovery_delete_state(ogc_error_recovery_t* recovery, const char* name) {
    if (recovery && name) {
        return reinterpret_cast<ErrorRecovery*>(recovery)->DeleteState(SafeString(name)) ? 1 : 0;
    }
    return 0;
}

size_t ogc_error_recovery_get_state_count(const ogc_error_recovery_t* recovery) {
    if (recovery) {
        return reinterpret_cast<const ErrorRecovery*>(recovery)->GetStateCount();
    }
    return 0;
}

const char* ogc_error_recovery_get_state_name(const ogc_error_recovery_t* recovery, size_t index) {
    if (recovery) {
        return reinterpret_cast<const ErrorRecovery*>(recovery)->GetStateName(index).c_str();
    }
    return "";
}

ogc_health_check_t* ogc_health_check_create(void) {
    return reinterpret_cast<ogc_health_check_t*>(HealthCheck::Create().release());
}

void ogc_health_check_destroy(ogc_health_check_t* health) {
    delete reinterpret_cast<HealthCheck*>(health);
}

int ogc_health_check_run(ogc_health_check_t* health) {
    if (health) {
        return reinterpret_cast<HealthCheck*>(health)->Run() ? 1 : 0;
    }
    return 0;
}

int ogc_health_check_is_healthy(const ogc_health_check_t* health) {
    if (health) {
        return reinterpret_cast<const HealthCheck*>(health)->IsHealthy() ? 1 : 0;
    }
    return 0;
}

size_t ogc_health_check_get_issue_count(const ogc_health_check_t* health) {
    if (health) {
        return reinterpret_cast<const HealthCheck*>(health)->GetIssueCount();
    }
    return 0;
}

const char* ogc_health_check_get_issue(const ogc_health_check_t* health, size_t index) {
    if (health) {
        return reinterpret_cast<const HealthCheck*>(health)->GetIssue(index).c_str();
    }
    return "";
}

void ogc_health_check_clear_issues(ogc_health_check_t* health) {
    if (health) {
        reinterpret_cast<HealthCheck*>(health)->ClearIssues();
    }
}

ogc_library_loader_t* ogc_library_loader_create(void) {
    return reinterpret_cast<ogc_library_loader_t*>(LibraryLoader::Create().release());
}

void ogc_library_loader_destroy(ogc_library_loader_t* loader) {
    delete reinterpret_cast<LibraryLoader*>(loader);
}

ogc_library_handle_t ogc_library_loader_load(ogc_library_loader_t* loader, const char* path) {
    if (loader && path) {
        return reinterpret_cast<LibraryLoader*>(loader)->Load(SafeString(path));
    }
    return nullptr;
}

int ogc_library_loader_unload(ogc_library_loader_t* loader, ogc_library_handle_t handle) {
    if (loader && handle) {
        return reinterpret_cast<LibraryLoader*>(loader)->Unload(handle) ? 1 : 0;
    }
    return 0;
}

void* ogc_library_loader_get_symbol(ogc_library_loader_t* loader, ogc_library_handle_t handle, const char* name) {
    if (loader && handle && name) {
        return reinterpret_cast<LibraryLoader*>(loader)->GetSymbol(handle, SafeString(name));
    }
    return nullptr;
}

int ogc_library_loader_is_loaded(ogc_library_loader_t* loader, ogc_library_handle_t handle) {
    if (loader && handle) {
        return reinterpret_cast<LibraryLoader*>(loader)->IsLoaded(handle) ? 1 : 0;
    }
    return 0;
}

const char* ogc_library_loader_get_error(ogc_library_loader_t* loader) {
    if (loader) {
        return reinterpret_cast<LibraryLoader*>(loader)->GetLastError().c_str();
    }
    return "";
}

int ogc_exception_get_last_code(void) {
    return static_cast<int>(Exception::GetLastCode());
}

const char* ogc_exception_get_last_message(void) {
    return Exception::GetLastMessage().c_str();
}

void ogc_exception_clear(void) {
    Exception::Clear();
}

int ogc_exception_has_error(void) {
    return Exception::HasError() ? 1 : 0;
}

#ifdef __cplusplus
}
#endif
