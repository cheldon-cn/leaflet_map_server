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

struct DbConnectionPool::Impl {
    DatabaseType dbType;
    PoolConfig config;
    std::string connectionString;
    PostGISOptions postgisOptions;
    SpatiaLiteOptions sqliteOptions;
    
    std::vector<PooledConnection> connections;
    std::queue<size_t> availableIndices;
    
    mutable std::mutex mutex;
    std::condition_variable condition;
    
    std::atomic<bool> running;
    std::atomic<int> totalConnections;
    std::thread healthCheckThread;
    std::thread shrinkThread;
    
    ConnectionValidator validator;
    ConnectionFactory factory;
    
    Impl() : dbType(DatabaseType::kPostGIS), running(false), totalConnections(0) {}
};

std::unique_ptr<DbConnectionPool> DbConnectionPool::Create() {
    return std::make_unique<DbConnectionPool>(DatabaseType::kPostGIS);
}

DbConnectionPool::DbConnectionPool(DatabaseType type)
    : impl_(new Impl()) {
    impl_->dbType = type;
}

DbConnectionPool::~DbConnectionPool() {
    Shutdown();
}

Result DbConnectionPool::Initialize(const PoolConfig& config, const std::string& connectionString) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    impl_->config = config;
    impl_->connectionString = connectionString;
    
    if (impl_->config.maxConnections <= 0) {
        return Result::Error(DbResult::kInvalidParameter, "maxConnections must be greater than 0");
    }
    
    if (impl_->config.minConnections > impl_->config.maxConnections) {
        return Result::Error(DbResult::kInvalidParameter, "minConnections cannot be greater than maxConnections");
    }
    
    if (impl_->config.maxIdleTimeMs <= 0) {
        return Result::Error(DbResult::kInvalidParameter, "maxIdleTimeMs must be greater than 0");
    }
    
    impl_->connections.resize(impl_->config.maxConnections);
    impl_->availableIndices = std::queue<size_t>();
    
    for (int i = 0; i < impl_->config.minConnections; ++i) {
        PooledConnection& pooled = impl_->connections[i];
        Result result = CreateConnection(pooled);
        if (!result.IsSuccess()) {
            return result;
        }
        impl_->availableIndices.push(i);
    }
    
    impl_->totalConnections = impl_->config.minConnections;
    
    return Result::Success();
}

Result DbConnectionPool::InitializePostGIS(const PoolConfig& config, const PostGISOptions& options) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    impl_->config = config;
    impl_->dbType = DatabaseType::kPostGIS;
    impl_->postgisOptions = options;
    
    impl_->connections.resize(impl_->config.maxConnections);
    impl_->availableIndices = std::queue<size_t>();
    
    for (int i = 0; i < impl_->config.minConnections; ++i) {
        PooledConnection& pooled = impl_->connections[i];
        Result result = CreateConnection(pooled);
        if (!result.IsSuccess()) {
            return result;
        }
        impl_->availableIndices.push(i);
    }
    
    impl_->totalConnections = impl_->config.minConnections;
    
    return Result::Success();
}

Result DbConnectionPool::InitializeSpatiaLite(const PoolConfig& config, const SpatiaLiteOptions& options) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    impl_->config = config;
    impl_->dbType = DatabaseType::kSpatiaLite;
    impl_->sqliteOptions = options;
    
    impl_->connections.resize(impl_->config.maxConnections);
    impl_->availableIndices = std::queue<size_t>();
    
    for (int i = 0; i < impl_->config.minConnections; ++i) {
        PooledConnection& pooled = impl_->connections[i];
        Result result = CreateConnection(pooled);
        if (!result.IsSuccess()) {
            return result;
        }
        impl_->availableIndices.push(i);
    }
    
    impl_->totalConnections = impl_->config.minConnections;
    
    return Result::Success();
}

Result DbConnectionPool::Acquire(DbConnectionPtr& connection) {
    std::unique_lock<std::mutex> lock(impl_->mutex);
    
    auto startTime = std::chrono::steady_clock::now();
    
    while (true) {
        if (!impl_->availableIndices.empty()) {
            size_t index = impl_->availableIndices.front();
            impl_->availableIndices.pop();
            
            PooledConnection& pooled = impl_->connections[index];
            
            if (pooled.IsExpired(impl_->config.maxLifetimeMs)) {
                CleanupConnection(pooled);
                Result result = CreateConnection(pooled);
                if (!result.IsSuccess()) {
                    continue;
                }
            }
            else if (pooled.NeedsValidation(impl_->config.validationIntervalMs)) {
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
        
        if (impl_->totalConnections < impl_->config.maxConnections) {
            size_t index = FindAvailableSlot();
            if (index < impl_->connections.size()) {
                PooledConnection& pooled = impl_->connections[index];
                Result result = CreateConnection(pooled);
                if (!result.IsSuccess()) {
                    return result;
                }
                
                impl_->totalConnections++;
                pooled.state = PooledConnectionState::kInUse;
                pooled.lastUsedAt = std::chrono::steady_clock::now();
                pooled.useCount = 1;
                
                connection = std::move(pooled.connection);
                return Result::Success();
            }
        }
        
        auto elapsed = std::chrono::steady_clock::now() - startTime;
        auto waitTime = std::chrono::milliseconds(impl_->config.acquireTimeoutMs);
        
        if (elapsed >= waitTime) {
            return Result::Error(DbResult::kTimeout, "Failed to acquire connection: timeout");
        }
        
        impl_->condition.wait_for(lock, waitTime - elapsed);
    }
}

Result DbConnectionPool::Release(DbConnection* connection) {
    if (!connection) {
        return Result::Error(DbResult::kInvalidParameter, "Connection is null");
    }
    
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    for (size_t i = 0; i < impl_->connections.size(); ++i) {
        if (impl_->connections[i].connection.get() == connection) {
            PooledConnection& pooled = impl_->connections[i];
            
            if (!connection->IsConnected()) {
                CleanupConnection(pooled);
                Result result = CreateConnection(pooled);
                if (!result.IsSuccess()) {
                    return result;
                }
            }
            
            pooled.state = PooledConnectionState::kAvailable;
            pooled.lastUsedAt = std::chrono::steady_clock::now();
            
            impl_->availableIndices.push(i);
            impl_->condition.notify_one();
            
            return Result::Success();
        }
    }
    
    return Result::Error(DbResult::kInvalidParameter, "Connection not found in pool");
}

void DbConnectionPool::Shutdown() {
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);
        impl_->running = false;
        impl_->condition.notify_all();
    }
    
    if (impl_->healthCheckThread.joinable()) {
        impl_->healthCheckThread.join();
    }
    
    if (impl_->shrinkThread.joinable()) {
        impl_->shrinkThread.join();
    }
    
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    for (auto& pooled : impl_->connections) {
        if (pooled.connection) {
            pooled.connection->Disconnect();
            pooled.connection.reset();
        }
    }
    
    impl_->connections.clear();
    impl_->totalConnections = 0;
}

PoolConfig DbConnectionPool::GetConfig() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->config;
}

void DbConnectionPool::SetConfig(const PoolConfig& config) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->config = config;
}

int DbConnectionPool::GetTotalConnections() const {
    return impl_->totalConnections.load();
}

int DbConnectionPool::GetAvailableConnections() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return static_cast<int>(impl_->availableIndices.size());
}

int DbConnectionPool::GetInUseConnections() const {
    return impl_->totalConnections.load() - GetAvailableConnections();
}

void DbConnectionPool::SetValidator(ConnectionValidator validator) {
    impl_->validator = validator;
}

void DbConnectionPool::SetFactory(ConnectionFactory factory) {
    impl_->factory = factory;
}

void DbConnectionPool::StartHealthCheck() {
    if (impl_->running) return;
    
    impl_->running = true;
    impl_->healthCheckThread = std::thread(&DbConnectionPool::HealthCheckLoop, this);
    impl_->shrinkThread = std::thread(&DbConnectionPool::ShrinkLoop, this);
}

void DbConnectionPool::StopHealthCheck() {
    impl_->running = false;
    impl_->condition.notify_all();
    
    if (impl_->healthCheckThread.joinable()) {
        impl_->healthCheckThread.join();
    }
    
    if (impl_->shrinkThread.joinable()) {
        impl_->shrinkThread.join();
    }
}

Result DbConnectionPool::CreateConnection(PooledConnection& pooled) {
    std::unique_ptr<DbConnection> conn;
    
    if (impl_->factory) {
        conn = impl_->factory();
    } else {
        switch (impl_->dbType) {
            case DatabaseType::kPostGIS: {
                auto postgis = std::make_unique<PostGISConnection>();
                Result result = postgis->Connect(impl_->connectionString);
                if (!result.IsSuccess()) {
                    return result;
                }
                conn = std::move(postgis);
                break;
            }
            case DatabaseType::kSpatiaLite: {
                auto sqlite = std::make_unique<SpatiaLiteConnection>();
                Result result = sqlite->Connect(impl_->sqliteOptions.databasePath);
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
    
    if (impl_->validator) {
        if (!impl_->validator(pooled.connection.get())) {
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
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    int targetCount = impl_->config.minConnections;
    int currentAvailable = static_cast<int>(impl_->availableIndices.size());
    
    if (currentAvailable <= targetCount) {
        return;
    }
    
    int toRemove = currentAvailable - targetCount;
    
    std::queue<size_t> newAvailable;
    std::vector<size_t> toRemoveIndices;
    
    while (!impl_->availableIndices.empty()) {
        size_t index = impl_->availableIndices.front();
        impl_->availableIndices.pop();
        
        if (toRemove > 0 && impl_->connections[index].useCount == 0) {
            CleanupConnection(impl_->connections[index]);
            toRemove--;
            impl_->totalConnections--;
        } else {
            newAvailable.push(index);
        }
    }
    
    impl_->availableIndices = std::move(newAvailable);
}

void DbConnectionPool::HealthCheckLoop() {
    while (impl_->running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(impl_->config.validationIntervalMs));
        
        if (!impl_->running) break;
        
        std::lock_guard<std::mutex> lock(impl_->mutex);
        
        for (auto& pooled : impl_->connections) {
            if (pooled.state == PooledConnectionState::kAvailable && pooled.NeedsValidation(impl_->config.validationIntervalMs)) {
                ValidateConnection(pooled);
            }
        }
    }
}

void DbConnectionPool::ShrinkLoop() {
    while (impl_->running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(impl_->config.shrinkIntervalMs));
        
        if (!impl_->running) break;
        
        if (impl_->config.autoShrink) {
            ShrinkPool();
        }
    }
}

size_t DbConnectionPool::FindAvailableSlot() {
    for (size_t i = 0; i < impl_->connections.size(); ++i) {
        if (!impl_->connections[i].connection) {
            return i;
        }
    }
    return impl_->connections.size();
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
