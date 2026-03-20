#include "ogc/db/sqlite_transaction.h"
#include "ogc/db/sqlite_connection.h"
#include <sstream>
#include <algorithm>

namespace ogc {
namespace db {

static const char* kDefaultSavePointPrefix = "sp_";
static int kSavePointCounter = 0;

SqliteTransactionManager::SqliteTransactionManager(SpatiaLiteConnection* connection)
    : m_connection(connection)
    , m_inTransaction(false)
    , m_isolationLevel(TransactionIsolation::kDeferred) {
}

SqliteTransactionManager::~SqliteTransactionManager() {
    if (m_inTransaction && m_connection && m_connection->IsConnected()) {
        m_connection->Execute("ROLLBACK");
    }
}

Result SqliteTransactionManager::BeginTransaction(TransactionIsolation isolation) {
    if (m_inTransaction) {
        return Result::Error(DbResult::kTransactionError, "Transaction already in progress");
    }
    
    std::ostringstream sql;
    sql << "BEGIN " << GetIsolationString(isolation);
    
    Result result = m_connection->Execute(sql.str());
    
    if (result.IsSuccess()) {
        m_inTransaction = true;
        m_isolationLevel = isolation;
    }
    
    return result;
}

Result SqliteTransactionManager::Commit() {
    if (!m_inTransaction) {
        return Result::Error(DbResult::kTransactionError, "No transaction in progress");
    }
    
    if (!m_savePoints.empty()) {
        return Result::Error(DbResult::kTransactionError, "Cannot commit with active savepoints");
    }
    
    Result result = m_connection->Execute("COMMIT");
    
    if (result.IsSuccess()) {
        m_inTransaction = false;
    }
    
    return result;
}

Result SqliteTransactionManager::Rollback() {
    if (!m_inTransaction) {
        return Result::Error(DbResult::kTransactionError, "No transaction in progress");
    }
    
    Result result = m_connection->Execute("ROLLBACK");
    
    if (result.IsSuccess()) {
        m_inTransaction = false;
        while (!m_savePoints.empty()) {
            m_savePoints.pop();
        }
    }
    
    return result;
}

Result SqliteTransactionManager::SavePoint(const std::string& name) {
    if (!m_inTransaction) {
        return Result::Error(DbResult::kTransactionError, "No transaction in progress");
    }
    
    if (HasSavePoint(name)) {
        return Result::Error(DbResult::kTransactionError, "Savepoint already exists");
    }
    
    std::ostringstream sql;
    sql << "SAVEPOINT " << name;
    
    Result result = m_connection->Execute(sql.str());
    
    if (result.IsSuccess()) {
        m_savePoints.push(name);
    }
    
    return result;
}

Result SqliteTransactionManager::ReleaseSavePoint(const std::string& name) {
    if (!m_inTransaction) {
        return Result::Error(DbResult::kTransactionError, "No transaction in progress");
    }
    
    if (!HasSavePoint(name)) {
        return Result::Error(DbResult::kTransactionError, "Savepoint does not exist");
    }
    
    std::ostringstream sql;
    sql << "RELEASE SAVEPOINT " << name;
    
    Result result = m_connection->Execute(sql.str());
    
    if (result.IsSuccess()) {
        std::stack<std::string> temp;
        while (!m_savePoints.empty()) {
            std::string top = m_savePoints.top();
            m_savePoints.pop();
            if (top == name) break;
            temp.push(top);
        }
        while (!temp.empty()) {
            m_savePoints.push(temp.top());
            temp.pop();
        }
    }
    
    return result;
}

Result SqliteTransactionManager::RollbackToSavePoint(const std::string& name) {
    if (!m_inTransaction) {
        return Result::Error(DbResult::kTransactionError, "No transaction in progress");
    }
    
    if (!HasSavePoint(name)) {
        return Result::Error(DbResult::kTransactionError, "Savepoint does not exist");
    }
    
    std::ostringstream sql;
    sql << "ROLLBACK TO SAVEPOINT " << name;
    
    return m_connection->Execute(sql.str());
}

Result SqliteTransactionManager::SetIsolationLevel(TransactionIsolation level) {
    if (m_inTransaction) {
        return Result::Error(DbResult::kTransactionError, "Cannot change isolation level in transaction");
    }
    
    m_isolationLevel = level;
    return Result::Success();
}

std::string SqliteTransactionManager::GetIsolationString(TransactionIsolation isolation) const {
    switch (isolation) {
        case TransactionIsolation::kDeferred:
            return "DEFERRED";
        case TransactionIsolation::kImmediate:
            return "IMMEDIATE";
        case TransactionIsolation::kExclusive:
            return "EXCLUSIVE";
        default:
            return "DEFERRED";
    }
}

bool SqliteTransactionManager::HasSavePoint(const std::string& name) const {
    std::stack<std::string> temp = m_savePoints;
    while (!temp.empty()) {
        if (temp.top() == name) return true;
        temp.pop();
    }
    return false;
}

}
}
