#pragma once

#include "common.h"
#include "connection.h"
#include "postgis_connection.h"
#include "sqlite_connection.h"
#include <string>
#include <memory>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <functional>

namespace ogc {
namespace db {

enum class PooledConnectionState {
    kAvailable,
    kInUse,
    kExpired,
    kInvalid
};

struct PoolConfig {
    int minConnections = 1;
    int maxConnections = 10;
    int maxIdleTimeMs = 30000;
    int maxLifetimeMs = 3600000;
    int acquireTimeoutMs = 5000;
    int validationIntervalMs = 60000;
    bool autoShrink = true;
    int shrinkIntervalMs = 300000;
    float shrinkThreshold = 0.3f;
};

struct PooledConnection {
    std::unique_ptr<DbConnection> connection;
    PooledConnectionState state;
    std::chrono::steady_clock::time_point createdAt;
    std::chrono::steady_clock::time_point lastUsedAt;
    std::chrono::steady_clock::time_point lastValidatedAt;
    int useCount;
    
    PooledConnection();
    bool IsExpired(int lifetimeMs) const;
    bool IsIdleTooLong(int idleTimeMs) const;
    bool NeedsValidation(int intervalMs) const;
};

using ConnectionValidator = std::function<bool(DbConnection*)>;
using ConnectionFactory = std::function<std::unique_ptr<DbConnection>()>;

class OGC_DB_API DbConnectionPool {
public:
    static std::unique_ptr<DbConnectionPool> Create();
    
    explicit DbConnectionPool(DatabaseType type);
    ~DbConnectionPool();
    
    Result Initialize(const PoolConfig& config, const std::string& connectionString);
    
    Result InitializePostGIS(const PoolConfig& config, const PostGISOptions& options);
    
    Result InitializeSpatiaLite(const PoolConfig& config, const SpatiaLiteOptions& options);
    
    Result Acquire(DbConnectionPtr& connection);
    
    Result Release(DbConnection* connection);
    
    void Shutdown();
    
    PoolConfig GetConfig() const { return m_config; }
    
    void SetConfig(const PoolConfig& config);
    
    int GetTotalConnections() const { return m_totalConnections.load(); }
    
    int GetAvailableConnections() const;
    
    int GetInUseConnections() const;
    
    void SetValidator(ConnectionValidator validator) { m_validator = validator; }
    
    void SetFactory(ConnectionFactory factory) { m_factory = factory; }
    
    void StartHealthCheck();
    
    void StopHealthCheck();

private:
    DatabaseType m_dbType;
    PoolConfig m_config;
    std::string m_connectionString;
    PostGISOptions m_postgisOptions;
    SpatiaLiteOptions m_sqliteOptions;
    
    std::vector<PooledConnection> m_connections;
    std::queue<size_t> m_availableIndices;
    
    std::mutex m_mutex;
    std::condition_variable m_condition;
    
    std::atomic<bool> m_running;
    std::atomic<int> m_totalConnections;
    std::thread m_healthCheckThread;
    std::thread m_shrinkThread;
    
    ConnectionValidator m_validator;
    ConnectionFactory m_factory;
    
    Result CreateConnection(PooledConnection& pooled);
    
    Result ValidateConnection(PooledConnection& pooled);
    
    void ShrinkPool();
    
    void HealthCheckLoop();
    
    void ShrinkLoop();
    
    size_t FindAvailableSlot();
    
    void CleanupConnection(PooledConnection& pooled);
};

class OGC_DB_API PooledConnectionGuard {
public:
    PooledConnectionGuard(DbConnectionPool* pool, DbConnectionPtr& conn);
    ~PooledConnectionGuard();
    
    PooledConnectionGuard(const PooledConnectionGuard&) = delete;
    PooledConnectionGuard& operator=(const PooledConnectionGuard&) = delete;
    
    PooledConnectionGuard(PooledConnectionGuard&& other) noexcept;
    PooledConnectionGuard& operator=(PooledConnectionGuard&& other) noexcept;
    
    DbConnection* Get() { return m_connection.get(); }
    DbConnection* operator->() { return m_connection.get(); }
    
    void Release();
    
    bool IsValid() const { return m_connection != nullptr; }

private:
    DbConnectionPool* m_pool;
    DbConnectionPtr m_connection;
    bool m_released;
};

inline PooledConnectionGuard::PooledConnectionGuard(DbConnectionPool* pool, DbConnectionPtr& conn)
    : m_pool(pool), m_connection(std::move(conn)), m_released(false) {
}

inline PooledConnectionGuard::~PooledConnectionGuard() {
    if (!m_released && m_pool && m_connection) {
        m_pool->Release(m_connection.get());
    }
}

inline PooledConnectionGuard::PooledConnectionGuard(PooledConnectionGuard&& other) noexcept
    : m_pool(other.m_pool), m_connection(std::move(other.m_connection)), m_released(other.m_released) {
    other.m_released = true;
}

inline PooledConnectionGuard& PooledConnectionGuard::operator=(PooledConnectionGuard&& other) noexcept {
    if (this != &other) {
        if (!m_released && m_pool && m_connection) {
            m_pool->Release(m_connection.get());
        }
        m_pool = other.m_pool;
        m_connection = std::move(other.m_connection);
        m_released = other.m_released;
        other.m_released = true;
    }
    return *this;
}

inline void PooledConnectionGuard::Release() {
    if (!m_released && m_pool && m_connection) {
        m_pool->Release(m_connection.get());
        m_released = true;
    }
}

}
}
