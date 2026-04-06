#include "ogc/alert/data_source_manager.h"
#include "ogc/base/log.h"
#include <map>
#include <mutex>
#include <atomic>
#include <thread>
#include <chrono>

using ogc::base::LogLevel;
using ogc::base::LogHelper;

namespace ogc {
namespace alert {

class DataSourceManager::Impl {
public:
    Impl() : m_running(false), m_refreshIntervalMs(60000) {}
    
    ~Impl() {
        StopAutoRefresh();
    }
    
    void RegisterSource(IDataSourcePtr source) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (source) {
            m_sources[source->GetSourceId()] = source;
        }
    }
    
    void UnregisterSource(const std::string& source_id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_sources.erase(source_id);
    }
    
    IDataSourcePtr GetSource(const std::string& source_id) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_sources.find(source_id);
        if (it != m_sources.end()) {
            return it->second;
        }
        return nullptr;
    }
    
    IDataSourcePtr GetSource(DataSourceType type) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (const auto& pair : m_sources) {
            if (pair.second->GetType() == type) {
                return pair.second;
            }
        }
        return nullptr;
    }
    
    std::vector<IDataSourcePtr> GetAllSources() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::vector<IDataSourcePtr> result;
        for (const auto& pair : m_sources) {
            result.push_back(pair.second);
        }
        return result;
    }
    
    bool ConnectAll() {
        std::lock_guard<std::mutex> lock(m_mutex);
        bool allSuccess = true;
        for (auto& pair : m_sources) {
            if (!pair.second->Connect()) {
                allSuccess = false;
                LOG_ERROR() << "Failed to connect data source: " << pair.first;
            }
        }
        return allSuccess;
    }
    
    void DisconnectAll() {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (auto& pair : m_sources) {
            pair.second->Disconnect();
        }
    }
    
    bool RefreshAll() {
        std::lock_guard<std::mutex> lock(m_mutex);
        bool allSuccess = true;
        for (auto& pair : m_sources) {
            if (!pair.second->Refresh()) {
                allSuccess = false;
                LOG_ERROR() << "Failed to refresh data source: " << pair.first;
            }
        }
        return allSuccess;
    }
    
    void SetRefreshInterval(int interval_ms) {
        m_refreshIntervalMs = interval_ms;
    }
    
    void StartAutoRefresh() {
        if (m_running) return;
        m_running = true;
        m_thread = std::thread(&Impl::RefreshLoop, this);
    }
    
    void StopAutoRefresh() {
        if (!m_running) return;
        m_running = false;
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }
    
private:
    void RefreshLoop() {
        while (m_running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(m_refreshIntervalMs));
            if (!m_running) break;
            RefreshAll();
        }
    }
    
    std::map<std::string, IDataSourcePtr> m_sources;
    mutable std::mutex m_mutex;
    std::atomic<bool> m_running;
    int m_refreshIntervalMs;
    std::thread m_thread;
};

DataSourceManager::DataSourceManager() 
    : m_impl(std::make_unique<Impl>()) {
}

DataSourceManager::~DataSourceManager() {
}

void DataSourceManager::RegisterSource(IDataSourcePtr source) {
    m_impl->RegisterSource(source);
}

void DataSourceManager::UnregisterSource(const std::string& source_id) {
    m_impl->UnregisterSource(source_id);
}

IDataSourcePtr DataSourceManager::GetSource(const std::string& source_id) const {
    return m_impl->GetSource(source_id);
}

IDataSourcePtr DataSourceManager::GetSource(DataSourceType type) const {
    return m_impl->GetSource(type);
}

std::vector<IDataSourcePtr> DataSourceManager::GetAllSources() const {
    return m_impl->GetAllSources();
}

bool DataSourceManager::ConnectAll() {
    return m_impl->ConnectAll();
}

void DataSourceManager::DisconnectAll() {
    m_impl->DisconnectAll();
}

bool DataSourceManager::RefreshAll() {
    return m_impl->RefreshAll();
}

void DataSourceManager::SetRefreshInterval(int interval_ms) {
    m_impl->SetRefreshInterval(interval_ms);
}

void DataSourceManager::StartAutoRefresh() {
    m_impl->StartAutoRefresh();
}

void DataSourceManager::StopAutoRefresh() {
    m_impl->StopAutoRefresh();
}

std::unique_ptr<IDataSourceManager> IDataSourceManager::Create() {
    return std::unique_ptr<IDataSourceManager>(new DataSourceManager());
}

}
}
