#include <gtest/gtest.h>
#include "ogc/db/async_connection.h"
#include <thread>
#include <chrono>

using namespace ogc;
using namespace ogc::db;

class AsyncConnectionTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(AsyncConnectionTest, ExecutorCreation) {
    auto executor = DbAsyncExecutor::Create(4);
    EXPECT_NE(executor, nullptr);
    EXPECT_EQ(executor->GetPendingCount(), 0);
    EXPECT_EQ(executor->GetRunningCount(), 0);
}

TEST_F(AsyncConnectionTest, ExecutorShutdown) {
    auto executor = DbAsyncExecutor::Create(4);
    executor->Shutdown();
    EXPECT_EQ(executor->GetRunningCount(), 0);
}

TEST_F(AsyncConnectionTest, AsyncExecute) {
    auto executor = DbAsyncExecutor::Create(2);
    
    auto future = executor->ExecuteAsync([]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        return Result::Success();
    });
    
    EXPECT_EQ(future.get().GetCode(), DbResult::kSuccess);
    
    executor->Shutdown();
}

TEST_F(AsyncConnectionTest, AsyncExecuteWithCallback) {
    auto executor = DbAsyncExecutor::Create(2);
    
    bool callbackCalled = false;
    executor->ExecuteAsync(
        []() {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            return Result::Success();
        },
        [&callbackCalled](Result result) {
            callbackCalled = true;
            EXPECT_TRUE(result.IsSuccess());
        }
    );
    
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_TRUE(callbackCalled);
    
    executor->Shutdown();
}

TEST_F(AsyncConnectionTest, CancelAll) {
    auto executor = DbAsyncExecutor::Create(2);
    
    for (int i = 0; i < 10; ++i) {
        executor->ExecuteAsync([]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            return Result::Success();
        });
    }
    
    EXPECT_GT(executor->GetPendingCount(), 0);
    
    executor->CancelAll();
    
    EXPECT_EQ(executor->GetPendingCount(), 0);
    
    executor->Shutdown();
}

TEST_F(AsyncConnectionTest, MultipleConcurrentTasks) {
    auto executor = DbAsyncExecutor::Create(4);
    
    std::vector<std::future<Result>> futures;
    for (int i = 0; i < 8; ++i) {
        futures.push_back(executor->ExecuteAsync([i]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(10 * i));
            return Result::Success();
        }));
    }
    
    for (auto& future : futures) {
        EXPECT_EQ(future.get().GetCode(), DbResult::kSuccess);
    }
    
    executor->Shutdown();
}

TEST_F(AsyncConnectionTest, QueryBuilder) {
    auto executor = DbAsyncExecutor::Create(2);
    
    std::string sql;
    auto future = executor->ExecuteAsync([&sql]() {
        sql = "SELECT * FROM test";
        return Result::Success();
    });
    
    EXPECT_EQ(future.get().GetCode(), DbResult::kSuccess);
    EXPECT_EQ(sql, "SELECT * FROM test");
    
    executor->Shutdown();
}

TEST_F(AsyncConnectionTest, CallbackExceptionHandling) {
    auto executor = DbAsyncExecutor::Create(2);
    
    bool callbackCalled = false;
    executor->ExecuteAsync(
        []() -> Result {
            throw std::runtime_error("Test exception");
        },
        [&callbackCalled](Result result) {
            callbackCalled = true;
            EXPECT_FALSE(result.IsSuccess());
        }
    );
    
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_TRUE(callbackCalled);
    
    executor->Shutdown();
}

TEST_F(AsyncConnectionTest, PendingAndRunningCounts) {
    auto executor = DbAsyncExecutor::Create(1);
    
    EXPECT_EQ(executor->GetPendingCount(), 0);
    EXPECT_EQ(executor->GetRunningCount(), 0);
    
    auto future = executor->ExecuteAsync([]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        return Result::Success();
    });
    
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    EXPECT_GE(executor->GetRunningCount(), 1);
    
    future.get();
    
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_EQ(executor->GetRunningCount(), 0);
    
    executor->Shutdown();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
