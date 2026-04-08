#pragma once

#include "ogc/layer/export.h"
#include "ogc/layer/layer_type.h"
#include "ogc/layer/datasource.h"

#include <memory>
#include <string>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>

namespace ogc {

class OGC_LAYER_API CNDbConnection {
public:
    virtual ~CNDbConnection() = default;

    virtual bool IsConnected() const = 0;

    virtual bool Execute(const std::string& sql) = 0;

    virtual std::unique_ptr<CNDataSource> GetDataSource() = 0;

    virtual void* GetHandle() = 0;
};

struct CNConnectionPoolConfig {
    size_t min_size = 2;
    size_t max_size = 10;
    int idle_timeout_ms = 300000;
    int connection_timeout_ms = 5000;
    int health_check_interval_ms = 60000;
    bool auto_reconnect = true;
};

class OGC_LAYER_API CNConnectionPool {
public:
    virtual ~CNConnectionPool() = default;

    virtual std::shared_ptr<CNDbConnection> Acquire(int timeout_ms = 5000) = 0;

    virtual void Release(std::shared_ptr<CNDbConnection> conn) = 0;

    virtual size_t GetSize() const = 0;

    virtual size_t GetIdleCount() const = 0;

    virtual size_t GetActiveCount() const = 0;

    virtual size_t HealthCheck() = 0;

    virtual void Clear() = 0;

    virtual void Close() = 0;
};

class OGC_LAYER_API CNConnectionGuard {
public:
    explicit CNConnectionGuard(CNConnectionPool& pool, int timeout_ms = 5000);

    ~CNConnectionGuard();

    CNDbConnection* Get() {
        return conn_.get();
    }

    CNDbConnection* operator->() {
        return conn_.get();
    }

    explicit operator bool() const {
        return conn_ != nullptr;
    }

    CNConnectionGuard(const CNConnectionGuard&) = delete;
    CNConnectionGuard& operator=(const CNConnectionGuard&) = delete;

    CNConnectionGuard(CNConnectionGuard&& other) noexcept
        : pool_(other.pool_), conn_(std::move(other.conn_)) {
    }

    CNConnectionGuard& operator=(CNConnectionGuard&&) = delete;

private:
    CNConnectionPool& pool_;
    std::shared_ptr<CNDbConnection> conn_;
};

using CNConnectionPoolPtr = std::unique_ptr<CNConnectionPool>;

} // namespace ogc
