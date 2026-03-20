#include "ogc/db/async_connection.h"
#include <algorithm>

namespace ogc {
namespace db {

std::unique_ptr<DbAsyncExecutor> DbAsyncExecutor::Create(int threadPoolSize) {
    return std::make_unique<DbAsyncExecutor>(threadPoolSize);
}

DbAsyncExecutor::DbAsyncExecutor(int threadPoolSize)
    : m_running(true), m_pendingCount(0), m_runningCount(0) {
    
    for (int i = 0; i < threadPoolSize; ++i) {
        m_workers.emplace_back([this] {
            while (m_running) {
                TaskBase* task = nullptr;
                
                {
                    std::unique_lock<std::mutex> lock(m_mutex);
                    m_condition.wait(lock, [this] {
                        return !m_running || !m_tasks.empty();
                    });
                    
                    if (!m_running && m_tasks.empty()) {
                        break;
                    }
                    
                    if (!m_tasks.empty()) {
                        task = m_tasks.front();
                        m_tasks.pop();
                        m_pendingCount--;
                        m_runningCount++;
                    }
                }
                
                if (task) {
                    task->Execute();
                    delete task;
                    m_runningCount--;
                }
            }
        });
    }
}

DbAsyncExecutor::~DbAsyncExecutor() {
    Shutdown();
}

void DbAsyncExecutor::Shutdown() {
    m_running = false;
    m_condition.notify_all();
    
    for (auto& worker : m_workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    while (!m_tasks.empty()) {
        delete m_tasks.front();
        m_tasks.pop();
    }
}

void DbAsyncExecutor::CancelAll() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    while (!m_tasks.empty()) {
        delete m_tasks.front();
        m_tasks.pop();
    }
    
    m_pendingCount = 0;
}

size_t DbAsyncExecutor::GetPendingCount() const {
    return m_pendingCount.load();
}

size_t DbAsyncExecutor::GetRunningCount() const {
    return m_runningCount.load();
}

std::unique_ptr<DbAsyncConnection> DbAsyncConnection::Create(DbConnection* syncConnection) {
    return std::make_unique<DbAsyncConnection>(syncConnection);
}

DbAsyncConnection::DbAsyncConnection(DbConnection* syncConnection)
    : m_syncConnection(syncConnection), m_executor(DbAsyncExecutor::Create(4)) {
}

DbAsyncConnection::~DbAsyncConnection() {
    Shutdown();
}

void DbAsyncConnection::Shutdown() {
    if (m_executor) {
        m_executor->Shutdown();
        m_executor.reset();
    }
}

std::future<Result> DbAsyncConnection::ConnectAsync(const std::string& connectionString) {
    return m_executor->ExecuteAsync([this, connectionString]() -> Result {
        return m_syncConnection->Connect(connectionString);
    });
}

std::future<void> DbAsyncConnection::DisconnectAsync() {
    return std::async(std::launch::async, [this]() {
        m_syncConnection->Disconnect();
    });
}

std::future<bool> DbAsyncConnection::IsConnectedAsync() {
    return std::async(std::launch::async, [this]() -> bool {
        return m_syncConnection->IsConnected();
    });
}

std::future<Result> DbAsyncConnection::ExecuteAsync(const std::string& sql) {
    return m_executor->ExecuteAsync([this, sql]() -> Result {
        return m_syncConnection->Execute(sql);
    });
}

std::future<Result> DbAsyncConnection::ExecuteQueryAsync(const std::string& sql, DbResultSetPtr& result) {
    return m_executor->ExecuteAsync([this, sql, &result]() -> Result {
        return m_syncConnection->ExecuteQuery(sql, result);
    });
}

std::future<Result> DbAsyncConnection::InsertGeometryAsync(const std::string& table,
                                                           const std::string& geomColumn,
                                                           const Geometry* geometry,
                                                           const std::map<std::string, std::string>& attributes,
                                                           int64_t& outId) {
    return m_executor->ExecuteAsync([this, table, geomColumn, geometry, attributes, &outId]() -> Result {
        return m_syncConnection->InsertGeometry(table, geomColumn, geometry, attributes, outId);
    });
}

void DbAsyncConnection::ConnectAsync(const std::string& connectionString, AsyncCallback callback) {
    m_executor->ExecuteAsync([this, connectionString]() -> Result {
        return m_syncConnection->Connect(connectionString);
    }, callback);
}

void DbAsyncConnection::DisconnectAsync(AsyncCallback callback) {
    m_executor->ExecuteAsync([this]() -> Result {
        m_syncConnection->Disconnect();
        return Result::Success();
    }, callback);
}

void DbAsyncConnection::ExecuteAsync(const std::string& sql, AsyncCallback callback) {
    m_executor->ExecuteAsync([this, sql]() -> Result {
        return m_syncConnection->Execute(sql);
    }, callback);
}

void DbAsyncConnection::ExecuteQueryAsync(const std::string& sql, AsyncResultSetCallback callback) {
    m_executor->ExecuteAsync([this, sql]() -> std::pair<Result, DbResultSetPtr> {
        DbResultSetPtr result;
        Result dbResult = m_syncConnection->ExecuteQuery(sql, result);
        return {dbResult, std::move(result)};
    }, [callback](std::pair<Result, DbResultSetPtr> result) {
        callback(result.first, result.second);
    });
}

void DbAsyncConnection::ExecuteTasks() {
}

AsyncQueryBuilder::AsyncQueryBuilder(DbAsyncConnection* asyncConn)
    : m_asyncConn(asyncConn), m_limit(-1), m_offset(0) {
}

AsyncQueryBuilder::~AsyncQueryBuilder() {
}

AsyncQueryBuilder& AsyncQueryBuilder::Select(const std::vector<std::string>& columns) {
    m_columns = columns;
    return *this;
}

AsyncQueryBuilder& AsyncQueryBuilder::From(const std::string& table) {
    m_table = table;
    return *this;
}

AsyncQueryBuilder& AsyncQueryBuilder::Where(const std::string& condition) {
    m_where = condition;
    return *this;
}

AsyncQueryBuilder& AsyncQueryBuilder::OrderBy(const std::string& order) {
    m_orderBy = order;
    return *this;
}

AsyncQueryBuilder& AsyncQueryBuilder::Limit(int limit) {
    m_limit = limit;
    return *this;
}

AsyncQueryBuilder& AsyncQueryBuilder::Offset(int offset) {
    m_offset = offset;
    return *this;
}

std::string AsyncQueryBuilder::BuildSQL() const {
    std::string sql = "SELECT ";
    
    if (m_columns.empty()) {
        sql += "*";
    } else {
        for (size_t i = 0; i < m_columns.size(); ++i) {
            if (i > 0) sql += ", ";
            sql += m_columns[i];
        }
    }
    
    sql += " FROM " + m_table;
    
    if (!m_where.empty()) {
        sql += " WHERE " + m_where;
    }
    
    if (!m_orderBy.empty()) {
        sql += " ORDER BY " + m_orderBy;
    }
    
    if (m_limit > 0) {
        sql += " LIMIT " + std::to_string(m_limit);
    }
    
    if (m_offset > 0) {
        sql += " OFFSET " + std::to_string(m_offset);
    }
    
    return sql;
}

std::future<Result> AsyncQueryBuilder::ExecuteAsync() {
    return m_asyncConn->ExecuteAsync(BuildSQL());
}

std::future<Result> AsyncQueryBuilder::ExecuteQueryAsync(DbResultSetPtr& result) {
    return m_asyncConn->ExecuteQueryAsync(BuildSQL(), result);
}

void AsyncQueryBuilder::ExecuteAsync(AsyncCallback callback) {
    m_asyncConn->ExecuteAsync(BuildSQL(), callback);
}

void AsyncQueryBuilder::ExecuteQueryAsync(AsyncResultSetCallback callback) {
    m_asyncConn->ExecuteQueryAsync(BuildSQL(), callback);
}

AsyncBatchInserter::AsyncBatchInserter(DbAsyncConnection* asyncConn)
    : m_asyncConn(asyncConn) {
}

AsyncBatchInserter::~AsyncBatchInserter() {
}

Result AsyncBatchInserter::Initialize(const std::string& table,
                                    const std::string& geomColumn,
                                    const std::vector<std::string>& attributeColumns) {
    m_table = table;
    m_geomColumn = geomColumn;
    m_attributeColumns = attributeColumns;
    return Result::Success();
}

Result AsyncBatchInserter::AddGeometry(const Geometry* geometry,
                                     const std::vector<std::string>& attributeValues) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (attributeValues.size() != m_attributeColumns.size()) {
        return Result::Error(DbResult::kInvalidParameter, "Attribute count mismatch");
    }
    
    m_batch.emplace_back(geometry, attributeValues);
    return Result::Success();
}

std::future<Result> AsyncBatchInserter::FlushAsync() {
    return std::async(std::launch::async, [this]() -> Result {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (m_batch.empty()) {
            return Result::Success();
        }
        
        m_batch.clear();
        return Result::Success();
    });
}

std::future<Result> AsyncBatchInserter::FinalizeAsync(int& totalInserted) {
    return std::async(std::launch::async, [this, &totalInserted]() -> Result {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        totalInserted = static_cast<int>(m_batch.size());
        m_batch.clear();
        
        return Result::Success();
    });
}

void AsyncBatchInserter::FlushAsync(AsyncCallback callback) {
    std::async(std::launch::async, [this, callback]() {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (m_batch.empty()) {
            callback(Result::Success());
            return;
        }
        
        m_batch.clear();
        callback(Result::Success());
    });
}

void AsyncBatchInserter::FinalizeAsync(AsyncCallback callback, int& totalInserted) {
    std::async(std::launch::async, [this, callback, &totalInserted]() {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        totalInserted = static_cast<int>(m_batch.size());
        m_batch.clear();
        
        callback(Result::Success());
    });
}

}
}
