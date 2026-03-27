#include <gtest/gtest.h>
#include "ogc/db/connection_pool.h"
#include <thread>
#include <chrono>

using namespace ogc;
using namespace ogc::db;

namespace {

class SimpleMockConnection : public DbConnection {
public:
    SimpleMockConnection() : m_connected(true) {}
    
    Result Connect(const std::string&) override { 
        m_connected = true; 
        return Result::Success(); 
    }
    
    void Disconnect() override { m_connected = false; }
    
    bool IsConnected() const override { return m_connected; }
    bool Ping() const override { return m_connected; }
    
    Result Execute(const std::string&) override { return Result::Success(); }
    
    Result ExecuteQuery(const std::string&, DbResultSetPtr&) override { 
        return Result::Success(); 
    }
    
    Result PrepareStatement(const std::string&, const std::string&, DbStatementPtr&) override {
        return Result::Success();
    }
    
    Result BeginTransaction() override { return Result::Success(); }
    Result Commit() override { return Result::Success(); }
    Result Rollback() override { return Result::Success(); }
    bool InTransaction() const override { return false; }
    
    Result InsertGeometry(const std::string&, const std::string&, 
                         const Geometry*, const std::map<std::string, std::string>&,
                         int64_t&) override { 
        return Result::Success(); 
    }
    
    Result InsertGeometries(const std::string&, const std::string&,
                           const std::vector<const Geometry*>&,
                           const std::vector<std::map<std::string, std::string>>&,
                           std::vector<int64_t>&) override {
        return Result::Success();
    }
    
    Result SelectGeometries(const std::string&, const std::string&, const std::string&,
                           std::vector<db::GeometryPtr>&) override {
        return Result::Success();
    }
    
    Result SpatialQuery(const std::string&, const std::string&, SpatialOperator,
                       const Geometry*, std::vector<db::GeometryPtr>&) override {
        return Result::Success();
    }
    
    Result SpatialQueryWithEnvelope(const std::string&, const std::string&, SpatialOperator,
                                   const Geometry*, const ::ogc::Envelope&,
                                   std::vector<db::GeometryPtr>&) override {
        return Result::Success();
    }
    
    Result UpdateGeometry(const std::string&, const std::string&, int64_t,
                         const Geometry*) override {
        return Result::Success();
    }
    
    Result DeleteGeometry(const std::string&, int64_t) override {
        return Result::Success();
    }
    
    Result CreateSpatialTable(const std::string&, const std::string&, int, int, int) override {
        return Result::Success();
    }
    
    Result CreateSpatialIndex(const std::string&, const std::string&) override {
        return Result::Success();
    }
    
    Result DropSpatialIndex(const std::string&, const std::string&) override {
        return Result::Success();
    }
    
    bool SpatialTableExists(const std::string&) const override { return false; }
    
    Result GetTableInfo(const std::string&, TableInfo&) const override {
        return Result::Success();
    }
    
    Result GetColumns(const std::string&, std::vector<ColumnInfo>&) const override {
        return Result::Success();
    }
    
    DatabaseType GetType() const override { return DatabaseType::kPostGIS; }
    std::string GetVersion() const override { return "mock"; }
    int64_t GetLastInsertId() const override { return 0; }
    int64_t GetRowsAffected() const override { return 0; }
    
    Result SetIsolationLevel(TransactionIsolation) override { return Result::Success(); }
    TransactionIsolation GetIsolationLevel() const override { 
        return TransactionIsolation::kReadCommitted; 
    }
    
    const ConnectionInfo& GetConnectionInfo() const override { return m_info; }
    std::string EscapeString(const std::string& value) const override { return value; }

private:
    bool m_connected;
    ConnectionInfo m_info;
};

}

class ConnectionPoolTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(ConnectionPoolTest, PoolCreation) {
    PoolConfig config;
    config.minConnections = 2;
    config.maxConnections = 5;
    config.maxIdleTimeMs = 60000;
    config.validationIntervalMs = 30000;
    config.shrinkThreshold = 2;
    
    auto pool = DbConnectionPool::Create();
    EXPECT_NE(pool, nullptr);
}

TEST_F(ConnectionPoolTest, PoolConfigValidation) {
    PoolConfig config;
    config.minConnections = 0;
    config.maxConnections = 0;
    config.maxIdleTimeMs = 0;
    
    auto pool = DbConnectionPool::Create();
    ASSERT_NE(pool, nullptr);
    
    Result result = pool->Initialize(config, "");
    EXPECT_FALSE(result.IsSuccess());
}

TEST_F(ConnectionPoolTest, PoolMinMaxValidation) {
    PoolConfig config;
    config.minConnections = 10;
    config.maxConnections = 5;
    
    auto pool = DbConnectionPool::Create();
    ASSERT_NE(pool, nullptr);
    
    Result result = pool->Initialize(config, "");
    EXPECT_FALSE(result.IsSuccess());
}

TEST_F(ConnectionPoolTest, AcquireRelease) {
    PoolConfig config;
    config.minConnections = 1;
    config.maxConnections = 2;
    config.maxIdleTimeMs = 60000;
    config.validationIntervalMs = 30000;
    
    auto pool = DbConnectionPool::Create();
    ASSERT_NE(pool, nullptr);
    
    DbConnectionPtr conn;
    Result result = pool->Acquire(conn);
    
    if (result.IsSuccess()) {
        EXPECT_NE(conn, nullptr);
        pool->Release(conn.get());
    }
}

TEST_F(ConnectionPoolTest, MultipleAcquire) {
    PoolConfig config;
    config.minConnections = 1;
    config.maxConnections = 3;
    config.maxIdleTimeMs = 60000;
    config.validationIntervalMs = 30000;
    
    auto pool = DbConnectionPool::Create();
    ASSERT_NE(pool, nullptr);
    
    DbConnectionPtr conn1, conn2, conn3;
    
    Result r1 = pool->Acquire(conn1);
    if (!r1.IsSuccess()) return;
    
    Result r2 = pool->Acquire(conn2);
    if (!r2.IsSuccess()) {
        pool->Release(conn1.get());
        return;
    }
    
    EXPECT_NE(conn1.get(), conn2.get());
    
    pool->Release(conn1.get());
    pool->Release(conn2.get());
}

TEST_F(ConnectionPoolTest, AcquireTimeout) {
    PoolConfig config;
    config.minConnections = 1;
    config.maxConnections = 1;
    config.maxIdleTimeMs = 60000;
    config.validationIntervalMs = 30000;
    config.acquireTimeoutMs = 1000;
    
    auto pool = DbConnectionPool::Create();
    ASSERT_NE(pool, nullptr);
    
    DbConnectionPtr conn1;
    Result r1 = pool->Acquire(conn1);
    if (!r1.IsSuccess()) return;
    
    DbConnectionPtr conn2;
    Result r2 = pool->Acquire(conn2);
    
    EXPECT_FALSE(r2.IsSuccess());
    
    pool->Release(conn1.get());
}

TEST_F(ConnectionPoolTest, Shutdown) {
    PoolConfig config;
    config.minConnections = 1;
    config.maxConnections = 2;
    config.maxIdleTimeMs = 60000;
    config.validationIntervalMs = 30000;
    
    auto pool = DbConnectionPool::Create();
    ASSERT_NE(pool, nullptr);
    
    pool->Shutdown();
    
    DbConnectionPtr conn;
    Result result = pool->Acquire(conn);
    EXPECT_FALSE(result.IsSuccess());
}

TEST_F(ConnectionPoolTest, ConnectionFailure_InvalidConnectionString) {
    PoolConfig config;
    config.minConnections = 1;
    config.maxConnections = 2;
    
    auto pool = DbConnectionPool::Create();
    ASSERT_NE(pool, nullptr);
    
    Result result = pool->Initialize(config, "invalid_connection_string");
    EXPECT_FALSE(result.IsSuccess()) << "Should fail with invalid connection string";
}

TEST_F(ConnectionPoolTest, ConnectionFailure_EmptyConnectionString) {
    PoolConfig config;
    config.minConnections = 1;
    config.maxConnections = 2;
    
    auto pool = DbConnectionPool::Create();
    ASSERT_NE(pool, nullptr);
    
    Result result = pool->Initialize(config, "");
    EXPECT_FALSE(result.IsSuccess()) << "Should fail with empty connection string";
}

TEST_F(ConnectionPoolTest, ReleaseInvalidConnection) {
    PoolConfig config;
    config.minConnections = 1;
    config.maxConnections = 2;
    
    auto pool = DbConnectionPool::Create();
    ASSERT_NE(pool, nullptr);
    
    Result result = pool->Release(nullptr);
    EXPECT_FALSE(result.IsSuccess()) << "Should fail when releasing null connection";
}

TEST_F(ConnectionPoolTest, DoubleShutdown) {
    PoolConfig config;
    config.minConnections = 1;
    config.maxConnections = 2;
    
    auto pool = DbConnectionPool::Create();
    ASSERT_NE(pool, nullptr);
    
    pool->Shutdown();
    pool->Shutdown();
    
    DbConnectionPtr conn;
    Result result = pool->Acquire(conn);
    EXPECT_FALSE(result.IsSuccess()) << "Pool should remain shutdown";
}

TEST_F(ConnectionPoolTest, ConcurrentAcquire_ThreadSafety) {
    PoolConfig config;
    config.minConnections = 1;
    config.maxConnections = 3;
    config.maxIdleTimeMs = 60000;
    config.acquireTimeoutMs = 5000;
    
    auto pool = DbConnectionPool::Create();
    ASSERT_NE(pool, nullptr);
    
    pool->SetFactory([]() -> std::unique_ptr<DbConnection> {
        return std::make_unique<SimpleMockConnection>();
    });
    
    Result initResult = pool->Initialize(config, "");
    ASSERT_TRUE(initResult.IsSuccess()) << "Pool should initialize with mock factory";
    
    std::vector<std::thread> threads;
    std::atomic<int> successCount{0};
    std::atomic<int> failCount{0};
    
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&]() {
            DbConnectionPtr conn;
            if (pool->Acquire(conn).IsSuccess()) {
                successCount++;
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                pool->Release(conn.get());
            } else {
                failCount++;
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    EXPECT_GT(successCount, 0) << "At least some threads should succeed";
}

TEST_F(ConnectionPoolTest, AcquireAfterRelease) {
    PoolConfig config;
    config.minConnections = 1;
    config.maxConnections = 1;
    config.acquireTimeoutMs = 1000;
    
    auto pool = DbConnectionPool::Create();
    ASSERT_NE(pool, nullptr);
    
    DbConnectionPtr conn1;
    Result r1 = pool->Acquire(conn1);
    if (!r1.IsSuccess()) return;
    
    pool->Release(conn1.get());
    
    DbConnectionPtr conn2;
    Result r2 = pool->Acquire(conn2);
    EXPECT_TRUE(r2.IsSuccess()) << "Should acquire after release";
    
    pool->Release(conn2.get());
}
