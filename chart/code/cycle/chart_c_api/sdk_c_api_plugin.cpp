/**
 * @file sdk_c_api_plugin.cpp
 * @brief OGC Chart SDK C API - Plugin, Recovery, Health, Loader, Exception Modules Implementation
 * @version v1.0
 * @date 2026-04-09
 */

#include "sdk_c_api.h"

#include <ogc/draw/plugin_manager.h>
#include <ogc/draw/draw_engine_plugin.h>
#include <ogc/draw/draw_device_plugin.h>

#include <cstring>
#include <cstdlib>
#include <memory>
#include <string>
#include <vector>

using namespace ogc::draw;

#ifdef __cplusplus
extern "C" {
#endif

namespace { static std::string SafeString(const char* str) {
    return str ? std::string(str) : std::string();
}

class ExceptionData {
public:
    static int s_lastCode;
    static std::string s_lastMessage;
    
    static int GetLastCode() { return s_lastCode; }
    static std::string GetLastMessage() { return s_lastMessage; }
    static void Clear() { s_lastCode = 0; s_lastMessage.clear(); }
    static bool HasError() { return s_lastCode != 0; }
};

int ExceptionData::s_lastCode = 0;
std::string ExceptionData::s_lastMessage;

class HealthCheckData {
public:
    std::vector<std::string> m_issues;
    
    bool Run() { m_issues.clear(); return true; }
    bool IsHealthy() const { return m_issues.empty(); }
    size_t GetIssueCount() const { return m_issues.size(); }
    std::string GetIssue(size_t index) const { return index < m_issues.size() ? m_issues[index] : ""; }
    void ClearIssues() { m_issues.clear(); }
};

class LibraryLoaderData {
public:
    std::string m_lastError;
    
    void* Load(const std::string& path) { (void)path; m_lastError = "Not implemented"; return nullptr; }
    bool Unload(void* handle) { (void)handle; m_lastError = "Not implemented"; return false; }
    void* GetSymbol(void* handle, const std::string& name) { (void)handle; (void)name; m_lastError = "Not implemented"; return nullptr; }
    bool IsLoaded(void* handle) const { (void)handle; return false; }
    std::string GetLastError() const { return m_lastError; }
};

class ErrorRecoveryData {
public:
    std::vector<std::string> m_states;
    
    bool Initialize() { return true; }
    void Shutdown() {}
    bool SaveState(const std::string& name) { m_states.push_back(name); return true; }
    bool RestoreState(const std::string& name) { (void)name; return true; }
    bool DeleteState(const std::string& name) { 
        for (auto it = m_states.begin(); it != m_states.end(); ++it) {
            if (*it == name) { m_states.erase(it); return true; }
        }
        return false;
    }
    size_t GetStateCount() const { return m_states.size(); }
    std::string GetStateName(size_t index) const { return index < m_states.size() ? m_states[index] : ""; }
};

class ChartPluginData {
public:
    std::string name;
    std::string version;
    bool enabled;
    
    ChartPluginData() : enabled(false) {}
};

}

ogc_chart_plugin_t* ogc_chart_plugin_create(const char* name, const char* version) {
    ChartPluginData* plugin = new ChartPluginData();
    plugin->name = SafeString(name);
    plugin->version = SafeString(version);
    return reinterpret_cast<ogc_chart_plugin_t*>(plugin);
}

void ogc_chart_plugin_destroy(ogc_chart_plugin_t* plugin) {
    delete reinterpret_cast<ChartPluginData*>(plugin);
}

const char* ogc_chart_plugin_get_name(const ogc_chart_plugin_t* plugin) {
    if (plugin) {
        return reinterpret_cast<const ChartPluginData*>(plugin)->name.c_str();
    }
    return "";
}

const char* ogc_chart_plugin_get_version(const ogc_chart_plugin_t* plugin) {
    if (plugin) {
        return reinterpret_cast<const ChartPluginData*>(plugin)->version.c_str();
    }
    return "";
}

int ogc_chart_plugin_initialize(ogc_chart_plugin_t* plugin) {
    if (plugin) {
        reinterpret_cast<ChartPluginData*>(plugin)->enabled = true;
        return 1;
    }
    return 0;
}

void ogc_chart_plugin_shutdown(ogc_chart_plugin_t* plugin) {
    if (plugin) {
        reinterpret_cast<ChartPluginData*>(plugin)->enabled = false;
    }
}

ogc_plugin_manager_t* ogc_plugin_manager_get_instance(void) {
    return reinterpret_cast<ogc_plugin_manager_t*>(&PluginManager::Instance());
}

int ogc_plugin_manager_load_plugin(ogc_plugin_manager_t* mgr, const char* path) {
    if (mgr && path) {
        return reinterpret_cast<PluginManager*>(mgr)->LoadPlugin(SafeString(path)) ? 1 : 0;
    }
    return 0;
}

int ogc_plugin_manager_unload_plugin(ogc_plugin_manager_t* mgr, const char* name) {
    if (mgr && name) {
        reinterpret_cast<PluginManager*>(mgr)->UnloadPlugin(SafeString(name));
        return 1;
    }
    return 0;
}

ogc_chart_plugin_t* ogc_plugin_manager_get_plugin(const ogc_plugin_manager_t* mgr, const char* name) {
    if (mgr && name) {
        PluginManager* nonConstMgr = const_cast<PluginManager*>(reinterpret_cast<const PluginManager*>(mgr));
        DrawEnginePlugin* enginePlugin = nonConstMgr->GetEnginePlugin(SafeString(name));
        if (enginePlugin) {
            ChartPluginData* plugin = new ChartPluginData();
            plugin->name = enginePlugin->GetName();
            plugin->version = enginePlugin->GetVersion();
            plugin->enabled = enginePlugin->IsAvailable();
            return reinterpret_cast<ogc_chart_plugin_t*>(plugin);
        }
        DrawDevicePlugin* devicePlugin = nonConstMgr->GetDevicePlugin(SafeString(name));
        if (devicePlugin) {
            ChartPluginData* plugin = new ChartPluginData();
            plugin->name = devicePlugin->GetName();
            plugin->version = devicePlugin->GetVersion();
            plugin->enabled = devicePlugin->IsAvailable();
            return reinterpret_cast<ogc_chart_plugin_t*>(plugin);
        }
    }
    return nullptr;
}

int ogc_plugin_manager_get_plugin_count(const ogc_plugin_manager_t* mgr) {
    if (mgr) {
        return static_cast<int>(reinterpret_cast<const PluginManager*>(mgr)->GetPluginCount());
    }
    return 0;
}

const char* ogc_plugin_manager_get_plugin_name(const ogc_plugin_manager_t* mgr, int index) {
    if (mgr) {
        std::vector<std::string> plugins = reinterpret_cast<const PluginManager*>(mgr)->GetLoadedPlugins();
        if (index >= 0 && static_cast<size_t>(index) < plugins.size()) {
            return plugins[static_cast<size_t>(index)].c_str();
        }
    }
    return "";
}

ogc_error_recovery_manager_t* ogc_error_recovery_manager_create(void) {
    return reinterpret_cast<ogc_error_recovery_manager_t*>(new ErrorRecoveryData());
}

void ogc_error_recovery_manager_destroy(ogc_error_recovery_manager_t* recovery) {
    delete reinterpret_cast<ErrorRecoveryData*>(recovery);
}

int ogc_error_recovery_manager_initialize(ogc_error_recovery_manager_t* recovery) {
    if (recovery) {
        return reinterpret_cast<ErrorRecoveryData*>(recovery)->Initialize() ? 1 : 0;
    }
    return 0;
}

void ogc_error_recovery_manager_shutdown(ogc_error_recovery_manager_t* recovery) {
    if (recovery) {
        reinterpret_cast<ErrorRecoveryData*>(recovery)->Shutdown();
    }
}

int ogc_error_recovery_manager_save_state(ogc_error_recovery_manager_t* recovery, const char* name) {
    if (recovery && name) {
        return reinterpret_cast<ErrorRecoveryData*>(recovery)->SaveState(SafeString(name)) ? 1 : 0;
    }
    return 0;
}

int ogc_error_recovery_manager_restore_state(ogc_error_recovery_manager_t* recovery, const char* name) {
    if (recovery && name) {
        return reinterpret_cast<ErrorRecoveryData*>(recovery)->RestoreState(SafeString(name)) ? 1 : 0;
    }
    return 0;
}

int ogc_error_recovery_manager_delete_state(ogc_error_recovery_manager_t* recovery, const char* name) {
    if (recovery && name) {
        return reinterpret_cast<ErrorRecoveryData*>(recovery)->DeleteState(SafeString(name)) ? 1 : 0;
    }
    return 0;
}

int ogc_error_recovery_manager_get_state_count(const ogc_error_recovery_manager_t* recovery) {
    if (recovery) {
        return static_cast<int>(reinterpret_cast<const ErrorRecoveryData*>(recovery)->GetStateCount());
    }
    return 0;
}

const char* ogc_error_recovery_manager_get_state_name(const ogc_error_recovery_manager_t* recovery, int index) {
    if (recovery) {
        return reinterpret_cast<const ErrorRecoveryData*>(recovery)->GetStateName(static_cast<size_t>(index)).c_str();
    }
    return "";
}

ogc_health_check_t* ogc_health_check_create(const char* name) {
    (void)name;
    return reinterpret_cast<ogc_health_check_t*>(new HealthCheckData());
}

void ogc_health_check_destroy(ogc_health_check_t* check) {
    delete reinterpret_cast<HealthCheckData*>(check);
}

ogc_health_check_result_t* ogc_health_check_execute(ogc_health_check_t* check) {
    if (check) {
        reinterpret_cast<HealthCheckData*>(check)->Run();
    }
    return nullptr;
}

void ogc_health_check_result_destroy(ogc_health_check_result_t* result) {
    (void)result;
}

int ogc_health_check_run(ogc_health_check_t* health) {
    if (health) {
        return reinterpret_cast<HealthCheckData*>(health)->Run() ? 1 : 0;
    }
    return 0;
}

int ogc_health_check_is_healthy(const ogc_health_check_t* health) {
    if (health) {
        return reinterpret_cast<const HealthCheckData*>(health)->IsHealthy() ? 1 : 0;
    }
    return 0;
}

int ogc_health_check_get_issue_count(const ogc_health_check_t* health) {
    if (health) {
        return static_cast<int>(reinterpret_cast<const HealthCheckData*>(health)->GetIssueCount());
    }
    return 0;
}

const char* ogc_health_check_get_issue(const ogc_health_check_t* health, int index) {
    if (health) {
        return reinterpret_cast<const HealthCheckData*>(health)->GetIssue(static_cast<size_t>(index)).c_str();
    }
    return "";
}

void ogc_health_check_clear_issues(ogc_health_check_t* health) {
    if (health) {
        reinterpret_cast<HealthCheckData*>(health)->ClearIssues();
    }
}

ogc_library_loader_t* ogc_library_loader_create(void) {
    return reinterpret_cast<ogc_library_loader_t*>(new LibraryLoaderData());
}

void ogc_library_loader_destroy(ogc_library_loader_t* loader) {
    delete reinterpret_cast<LibraryLoaderData*>(loader);
}

ogc_library_handle_t ogc_library_loader_load(ogc_library_loader_t* loader, const char* library_path) {
    if (loader && library_path) {
        return reinterpret_cast<LibraryLoaderData*>(loader)->Load(SafeString(library_path));
    }
    return nullptr;
}

void ogc_library_loader_unload(ogc_library_loader_t* loader, ogc_library_handle_t handle) {
    if (loader && handle) {
        reinterpret_cast<LibraryLoaderData*>(loader)->Unload(handle);
    }
}

void* ogc_library_loader_get_symbol(ogc_library_loader_t* loader, ogc_library_handle_t handle, const char* symbol_name) {
    if (loader && handle && symbol_name) {
        return reinterpret_cast<LibraryLoaderData*>(loader)->GetSymbol(handle, SafeString(symbol_name));
    }
    return nullptr;
}

int ogc_library_loader_is_loaded(ogc_library_loader_t* loader, ogc_library_handle_t handle) {
    if (loader && handle) {
        return reinterpret_cast<LibraryLoaderData*>(loader)->IsLoaded(handle) ? 1 : 0;
    }
    return 0;
}

const char* ogc_library_loader_get_error(ogc_library_loader_t* loader) {
    if (loader) {
        return reinterpret_cast<LibraryLoaderData*>(loader)->GetLastError().c_str();
    }
    return "";
}

int ogc_exception_get_last_code(void) {
    return ExceptionData::GetLastCode();
}

const char* ogc_exception_get_last_message(void) {
    return ExceptionData::GetLastMessage().c_str();
}

void ogc_exception_clear(void) {
    ExceptionData::Clear();
}

int ogc_exception_has_error(void) {
    return ExceptionData::HasError() ? 1 : 0;
}

ogc_secure_library_loader_t* ogc_secure_library_loader_create(const char* allowed_paths) {
    (void)allowed_paths;
    return nullptr;
}

void ogc_secure_library_loader_destroy(ogc_secure_library_loader_t* loader) {
    (void)loader;
}

ogc_library_handle_t ogc_secure_library_loader_load(ogc_secure_library_loader_t* loader, const char* library_path) {
    (void)loader; (void)library_path;
    return nullptr;
}

int ogc_secure_library_loader_verify_signature(ogc_secure_library_loader_t* loader, ogc_library_handle_t handle, const char* public_key) {
    (void)loader; (void)handle; (void)public_key;
    return -1;
}

ogc_chart_exception_t* ogc_chart_exception_create(int error_code, const char* message) {
    ogc_chart_exception_t* ex = new ogc_chart_exception_t();
    ex->error_code = error_code;
    ex->message = message ? _strdup(message) : nullptr;
    ex->context = nullptr;
    return ex;
}

void ogc_chart_exception_destroy(ogc_chart_exception_t* ex) {
    if (ex) {
        if (ex->message) free(ex->message);
        if (ex->context) free(ex->context);
        delete ex;
    }
}

int ogc_chart_exception_get_error_code(const ogc_chart_exception_t* ex) {
    return ex ? ex->error_code : 0;
}

const char* ogc_chart_exception_get_message(const ogc_chart_exception_t* ex) {
    return ex ? (ex->message ? ex->message : "") : "";
}

const char* ogc_chart_exception_get_context(const ogc_chart_exception_t* ex) {
    return ex ? (ex->context ? ex->context : "") : "";
}

ogc_jni_exception_t* ogc_jni_exception_create(const char* message, const char* java_class) {
    ogc_jni_exception_t* ex = new ogc_jni_exception_t();
    ex->error_code = 0;
    ex->message = message ? _strdup(message) : nullptr;
    ex->java_exception_class = java_class ? _strdup(java_class) : nullptr;
    return ex;
}

void ogc_jni_exception_destroy(ogc_jni_exception_t* ex) {
    if (ex) {
        if (ex->message) free(ex->message);
        if (ex->java_exception_class) free(ex->java_exception_class);
        delete ex;
    }
}

ogc_render_exception_t* ogc_render_exception_create(const char* message, const char* layer_name) {
    ogc_render_exception_t* ex = new ogc_render_exception_t();
    ex->error_code = 0;
    ex->message = message ? _strdup(message) : nullptr;
    ex->layer_name = layer_name ? _strdup(layer_name) : nullptr;
    ex->operation = nullptr;
    return ex;
}

#ifdef __cplusplus
}
#endif
