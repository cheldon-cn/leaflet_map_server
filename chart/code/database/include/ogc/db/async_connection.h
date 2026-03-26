#pragma once

#include "common.h"
#include "result.h"
#include "connection.h"
#include <memory>
#include <string>
#include <functional>
#include <future>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace ogc {
namespace db {

using AsyncCallback = std::function<void(Result)>;
using AsyncGeometryCallback = std::function<void(Result, GeometryPtr)>;
using AsyncResultSetCallback = std::function<void(Result, DbResultSetPtr)>;

class OGC_DB_API DbAsyncExecutor {
public:
    static std::unique_ptr<DbAsyncExecutor> Create(int threadPoolSize = 4);
    
    explicit DbAsyncExecutor(int threadPoolSize);
    ~DbAsyncExecutor();
    
    void Shutdown();
    
    template<typename Func>
    std::future<Result> ExecuteAsync(Func&& func);
    
    template<typename Func, typename Callback>
    void ExecuteAsync(Func&& func, Callback&& callback);
    
    void CancelAll();
    
    size_t GetPendingCount() const;
    
    size_t GetRunningCount() const;

private:
    struct TaskBase {
        virtual ~TaskBase() = default;
        virtual void Execute() = 0;
    };
    
    template<typename Func>
    struct Task : TaskBase {
        Func func;
        std::promise<Result> promise;
        
        explicit Task(Func&& f) : func(std::move(f)) {}
        
        void Execute() override {
            try {
                Result result = func();
                promise.set_value(result);
            } catch (...) {
                promise.set_value(Result::Error(DbResult::kUnknownError, "Unknown exception"));
            }
        }
    };
    
    template<typename Func, typename Callback>
    struct TaskWithCallback : TaskBase {
        Func func;
        Callback callback;
        
        explicit TaskWithCallback(Func&& f, Callback&& cb) 
            : func(std::move(f)), callback(std::move(cb)) {}
        
        void Execute() override {
            try {
                callback(func());
            } catch (const std::exception& e) {
                try {
                    callback(typename std::decay<decltype(func())>::type());
                } catch (...) {
                }
            } catch (...) {
                try {
                    callback(typename std::decay<decltype(func())>::type());
                } catch (...) {
                }
            }
        }
    };
    
    std::vector<std::thread> m_workers;
    std::queue<TaskBase*> m_tasks;
    std::mutex m_mutex;
    std::condition_variable m_condition;
    std::atomic<bool> m_running;
    std::atomic<size_t> m_pendingCount;
    std::atomic<size_t> m_runningCount;
};

template<typename Func>
std::future<Result> DbAsyncExecutor::ExecuteAsync(Func&& func) {
    auto task = new Task<Func>(std::forward<Func>(func));
    
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_tasks.push(task);
        m_pendingCount++;
    }
    
    m_condition.notify_one();
    return task->promise.get_future();
}

template<typename Func, typename Callback>
void DbAsyncExecutor::ExecuteAsync(Func&& func, Callback&& callback) {
    auto task = new TaskWithCallback<Func, Callback>(
        std::forward<Func>(func), 
        std::forward<Callback>(callback)
    );
    
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_tasks.push(task);
        m_pendingCount++;
    }
    
    m_condition.notify_one();
}

class OGC_DB_API DbAsyncConnection {
public:
    static std::unique_ptr<DbAsyncConnection> Create(DbConnection* syncConnection);
    
    explicit DbAsyncConnection(DbConnection* syncConnection);
    ~DbAsyncConnection();
    
    void Shutdown();
    
    std::future<Result> ConnectAsync(const std::string& connectionString);
    
    std::future<void> DisconnectAsync();
    
    std::future<bool> IsConnectedAsync();
    
    std::future<Result> ExecuteAsync(const std::string& sql);
    
    std::future<Result> ExecuteQueryAsync(const std::string& sql, DbResultSetPtr& result);
    
    std::future<Result> InsertGeometryAsync(const std::string& table,
                                          const std::string& geomColumn,
                                          const Geometry* geometry,
                                          const std::map<std::string, std::string>& attributes,
                                          int64_t& outId);
    
    void ConnectAsync(const std::string& connectionString, AsyncCallback callback);
    void DisconnectAsync(AsyncCallback callback);
    void ExecuteAsync(const std::string& sql, AsyncCallback callback);
    void ExecuteQueryAsync(const std::string& sql, AsyncResultSetCallback callback);

private:
    DbConnection* m_syncConnection;
    std::unique_ptr<DbAsyncExecutor> m_executor;
    
    void ExecuteTasks();
};

class OGC_DB_API AsyncQueryBuilder {
public:
    explicit AsyncQueryBuilder(DbAsyncConnection* asyncConn);
    ~AsyncQueryBuilder();
    
    AsyncQueryBuilder& Select(const std::vector<std::string>& columns);
    AsyncQueryBuilder& From(const std::string& table);
    AsyncQueryBuilder& Where(const std::string& condition);
    AsyncQueryBuilder& OrderBy(const std::string& order);
    AsyncQueryBuilder& Limit(int limit);
    AsyncQueryBuilder& Offset(int offset);
    
    std::future<Result> ExecuteAsync();
    std::future<Result> ExecuteQueryAsync(DbResultSetPtr& result);
    
    void ExecuteAsync(AsyncCallback callback);
    void ExecuteQueryAsync(AsyncResultSetCallback callback);

private:
    DbAsyncConnection* m_asyncConn;
    std::string m_sql;
    std::vector<std::string> m_columns;
    std::string m_table;
    std::string m_where;
    std::string m_orderBy;
    int m_limit;
    int m_offset;
    
    std::string BuildSQL() const;
};

class OGC_DB_API AsyncBatchInserter {
public:
    explicit AsyncBatchInserter(DbAsyncConnection* asyncConn);
    ~AsyncBatchInserter();
    
    Result Initialize(const std::string& table,
                    const std::string& geomColumn,
                    const std::vector<std::string>& attributeColumns);
    
    Result AddGeometry(const Geometry* geometry,
                      const std::vector<std::string>& attributeValues);
    
    std::future<Result> FlushAsync();
    std::future<Result> FinalizeAsync(int& totalInserted);
    
    void FlushAsync(AsyncCallback callback);
    void FinalizeAsync(AsyncCallback callback, int& totalInserted);

private:
    DbAsyncConnection* m_asyncConn;
    std::string m_table;
    std::string m_geomColumn;
    std::vector<std::string> m_attributeColumns;
    
    std::mutex m_mutex;
    std::vector<std::pair<const Geometry*, std::vector<std::string>>> m_batch;
};

}
}
