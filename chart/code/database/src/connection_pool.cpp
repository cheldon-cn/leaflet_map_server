#include "ogc/db/connection_pool.h"
#include <algorithm>
#include <cstring>

namespace ogc {
namespace db {

PooledConnection::PooledConnection()
    : state(PooledConnectionState::kAvailable), useCount(0) {
}

bool PooledConnection::IsExpired(int lifetimeMs) const {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - createdAt).count();
    return elapsed > lifetimeMs;
}

bool PooledConnection::IsIdleTooLong(int idleTimeMs) const {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUsedAt).count();
    return elapsed > idleTimeMs;
}

bool PooledConnection::NeedsValidation(int intervalMs) const {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastValidatedAt).count();
    return elapsed > intervalMs;
}

std::unique_ptr<DbConnectionPool> DbConnectionPool::Create() {
    return std::make_unique<DbConnectionPool>(DatabaseType::kPostGIS);
}

DbConnectionPool::DbConnectionPool(DatabaseType type)
    : m_dbType(type)
    , m_running(false)
    , m_totalConnections(0) {
}

DbConnectionPool::~DbConnectionPool() {
    Shutdown();
}

Result DbConnectionPool::Initialize(const PoolConfig& config, const std::string& connectionString) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_config = config;
    m_connectionString = connectionString;
    
    if (m_config.maxConnections <= 0) {
        return Result::Error(DbResult::kInvalidParameter, "maxConnections must be greater than 0");
    }
    
    if (m_config.minConnections > m_config.maxConnections) {
        return Result::Error(DbResult::kInvalidParameter, "minConnections cannot be greater than maxConnections");
    }
    
    if (m_config.maxIdleTimeMs <= 0) {
        return Result::Error(DbResult::kInvalidParameter, "maxIdleTimeMs must be greater than 0");
    }
    
    m_connections.resize(m_config.maxConnections);
    m_availableIndices = std::queue<size_t>();
    
    for (int i = 0; i < m_config.minConnections; ++i) {
        PooledConnection& pooled = m_connections[i];
        Result result = CreateConnection(pooled);
        if (!result.IsSuccess()) {
            return result;
        }
        m_availableIndices.push(i);
    }
    
    m_totalConnections = m_config.minConnections;
    
    return Result::Success();
}

Result DbConnectionPool::InitializePostGIS(const PoolConfig& config, const PostGISOptions& options) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_config = config;
    m_dbType = DatabaseType::kPostGIS;
    m_postgisOptions = options;
    
    m_connections.resize(m_config.maxConnections);
    m_availableIndices = std::queue<size_t>();
    
    for (int i = 0; i < m_config.minConnections; ++i) {
        PooledConnection& pooled = m_connections[i];
        Result result = CreateConnection(pooled);
        if (!result.IsSuccess()) {
            return result;
        }
        m_availableIndices.push(i);
    }
    
    m_totalConnections = m_config.minConnections;
    
    return Result::Success();
}

Result DbConnectionPool::InitializeSpatiaLite(const PoolConfig& config, const SpatiaLiteOptions& options) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_config = config;
    m_dbType = DatabaseType::kSpatiaLite;
    m_sqliteOptions = options;
    
    m_connections.resize(m_config.maxConnections);
    m_availableIndices = std::queue<size_t>();
    
    for (int i = 0; i < m_config.minConnections; ++i) {
        PooledConnection& pooled = m_connections[i];
        Result result = CreateConnection(pooled);
        if (!result.IsSuccess()) {
            return result;
        }
        m_availableIndices.push(i);
    }
    
    m_totalConnections = m_config.minConnections;
    
    return Result::Success();
}

Result DbConnectionPool::Acquire(DbConnectionPtr& connection) {
    std::unique_lock<std::mutex> lock(m_mutex);
    
    auto startTime = std::chrono::steady_clock::now();
    
    while (true) {
        if (!m_availableIndices.empty()) {
            size_t index = m_availableIndices.front();
            m_availableIndices.pop();
            
            PooledConnection& pooled = m_connections[index];
            
            if (pooled.IsExpired(m_config.maxLifetimeMs)) {
                CleanupConnection(pooled);
                Result result = CreateConnection(pooled);
                if (!result.IsSuccess()) {
                    continue;
                }
            }
            else if (pooled.NeedsValidation(m_config.validationIntervalMs)) {
                Result result = ValidateConnection(pooled);
                if (!result.IsSuccess()) {
                    CleanupConnection(pooled);
                    result = CreateConnection(pooled);
                    if (!result.IsSuccess()) {
                        continue;
                    }
                }
            }
            
            pooled.state = PooledConnectionState::kInUse;
            pooled.lastUsedAt = std::chrono::steady_clock::now();
            pooled.useCount++;
            
            connection = std::move(pooled.connection);
            return Result::Success();
        }
        
        if (m_totalConnections < m_config.maxConnections) {
            size_t index = FindAvailableSlot();
            if (index < m_connections.size()) {
                PooledConnection& pooled = m_connections[index];
                Result result = CreateConnection(pooled);
                if (!result.IsSuccess()) {
                    return result;
                }
                
                m_totalConnections++;
                pooled.state = PooledConnectionState::kInUse;
                pooled.lastUsedAt = std::chrono::steady_clock::now();
                pooled.useCount = 1;
                
                connection = std::move(pooled.connection);
                return Result::Success();
            }
        }
        
        auto elapsed = std::chrono::steady_clock::now() - startTime;
        auto waitTime = std::chrono::milliseconds(m_config.acquireTimeoutMs);
        
        if (elapsed >= waitTime) {
            return Result::Error(DbResult::kTimeout, "Failed to acquire connection: timeout");
        }
        
        m_condition.wait_for(lock, waitTime - elapsed);
    }
}

Result DbConnectionPool::Release(DbConnection* connection) {
    if (!connection) {
        return Result::Error(DbResult::kInvalidParameter, "Connection is null");
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    for (size_t i = 0; i < m_connections.size(); ++i) {
        if (m_connections[i].connection.get() == connection) {
            PooledConnection& pooled = m_connections[i];
            
            if (!connection->IsConnected()) {
                CleanupConnection(pooled);
                Result result = CreateConnection(pooled);
                if (!result.IsSuccess()) {
                    return result;
                }
            }
            
            pooled.state = PooledConnectionState::kAvailable;
            pooled.lastUsedAt = std::chrono::steady_clock::now();
            
            m_availableIndices.push(i);
            m_condition.notify_one();
            
            return Result::Success();
        }
    }
    
    return Result::Error(DbResult::kInvalidParameter, "Connection not found in pool");
}

void DbConnectionPool::Shutdown() {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_running = false;
        m_condition.notify_all();
    }
    
    if (m_healthCheckThread.joinable()) {
        m_healthCheckThread.join();
    }
    
    if (m_shrinkThread.joinable()) {
        m_shrinkThread.join();
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    for (auto& pooled : m_connections) {
        if (pooled.connection) {
            pooled.connection->Disconnect();
            pooled.connection.reset();
        }
    }
    
    m_connections.clear();
    m_totalConnections = 0;
}

void DbConnectionPool::SetConfig(const PoolConfig& config) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_config = config;
}

int DbConnectionPool::GetAvailableConnections() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return static_cast<int>(m_availableIndices.size());
}

int DbConnectionPool::GetInUseConnections() const {
    return m_totalConnections.load() - GetAvailableConnections();
}

void DbConnectionPool::StartHealthCheck() {
    if (m_running) return;
    
    m_running = true;
    m_healthCheckThread = std::thread(&DbConnectionPool::HealthCheckLoop, this);
    m_shrinkThread = std::thread(&DbConnectionPool::ShrinkLoop, this);
}

void DbConnectionPool::StopHealthCheck() {
    m_running = false;
    m_condition.notify_all();
    
    if (m_healthCheckThread.joinable()) {
        m_healthCheckThread.join();
    }
    
    if (m_shrinkThread.joinable()) {
        m_shrinkThread.join();
    }
}

Result DbConnectionPool::CreateConnection(PooledConnection& pooled) {
    std::unique_ptr<DbConnection> conn;
    
    if (m_factory) {
        conn = m_factory();
    } else {
        switch (m_dbType) {
            case DatabaseType::kPostGIS: {
                auto postgis = std::make_unique<PostGISConnection>();
                Result result = postgis->Connect(m_connectionString);
                if (!result.IsSuccess()) {
                    return result;
                }
                conn = std::move(postgis);
                break;
            }
            case DatabaseType::kSpatiaLite: {
                auto sqlite = std::make_unique<SpatiaLiteConnection>();
                Result result = sqlite->Connect(m_sqliteOptions.databasePath);
                if (!result.IsSuccess()) {
                    return result;
                }
                conn = std::move(sqlite);
                break;
            }
            default:
                return Result::Error(DbResult::kNotSupported, "Unsupported database type");
        }
    }
    
    pooled.connection = std::move(conn);
    pooled.state = PooledConnectionState::kAvailable;
    pooled.createdAt = std::chrono::steady_clock::now();
    pooled.lastUsedAt = std::chrono::steady_clock::now();
    pooled.lastValidatedAt = std::chrono::steady_clock::now();
    pooled.useCount = 0;
    
    return Result::Success();
}

Result DbConnectionPool::ValidateConnection(PooledConnection& pooled) {
    if (!pooled.connection) {
        return Result::Error(DbResult::kInvalidConnection, "Connection is null");
    }
    
    if (m_validator) {
        if (!m_validator(pooled.connection.get())) {
            return Result::Error(DbResult::kInvalidConnection, "Connection validation failed");
        }
    } else {
        if (!pooled.connection->Ping()) {
            return Result::Error(DbResult::kInvalidConnection, "Connection ping failed");
        }
    }
    
    pooled.lastValidatedAt = std::chrono::steady_clock::now();
    return Result::Success();
}

void DbConnectionPool::ShrinkPool() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    int targetCount = m_config.minConnections;
    int currentAvailable = static_cast<int>(m_availableIndices.size());
    
    if (currentAvailable <= targetCount) {
        return;
    }
    
    int toRemove = currentAvailable - targetCount;
    
    std::queue<size_t> newAvailable;
    std::vector<size_t> toRemoveIndices;
    
    while (!m_availableIndices.empty()) {
        size_t index = m_availableIndices.front();
        m_availableIndices.pop();
        
        if (toRemove > 0 && m_connections[index].useCount == 0) {
            CleanupConnection(m_connections[index]);
            toRemove--;
            m_totalConnections--;
        } else {
            newAvailable.push(index);
        }
    }
    
    m_availableIndices = std::move(newAvailable);
}

void DbConnectionPool::HealthCheckLoop() {
    while (m_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(m_config.validationIntervalMs));
        
        if (!m_running) break;
        
        std::lock_guard<std::mutex> lock(m_mutex);
        
        for (auto& pooled : m_connections) {
            if (pooled.state == PooledConnectionState::kAvailable && pooled.NeedsValidation(m_config.validationIntervalMs)) {
                ValidateConnection(pooled);
            }
        }
    }
}

void DbConnectionPool::ShrinkLoop() {
    while (m_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(m_config.shrinkIntervalMs));
        
        if (!m_running) break;
        
        if (m_config.autoShrink) {
            ShrinkPool();
        }
    }
}

size_t DbConnectionPool::FindAvailableSlot() {
    for (size_t i = 0; i < m_connections.size(); ++i) {
        if (m_connections[i].state == PooledConnectionState::kAvailable && 
            m_connections[i].connection == nullptr) {
            return i;
        }
    }
    return m_connections.size();
}

void DbConnectionPool::CleanupConnection(PooledConnection& pooled) {
    if (pooled.connection) {
        pooled.connection->Disconnect();
        pooled.connection.reset();
    }
    pooled.state = PooledConnectionState::kInvalid;
}

}
}
