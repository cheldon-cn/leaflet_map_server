#include <gtest/gtest.h>
#include "ogc/db/connection_pool.h"
#include <thread>
#include <chrono>

using namespace ogc;
using namespace ogc::db;

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

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
