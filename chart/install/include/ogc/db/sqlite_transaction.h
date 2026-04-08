#pragma once

#include "common.h"
#include "connection.h"
#include "result.h"
#include "sqlite_connection.h"
#include <string>
#include <memory>
#include <vector>
#include <stack>

namespace ogc {
namespace db {

class OGC_DB_API SqliteTransactionManager {
public:
    explicit SqliteTransactionManager(SpatiaLiteConnection* connection);
    ~SqliteTransactionManager();
    
    Result BeginTransaction(TransactionIsolation isolation = TransactionIsolation::kDeferred);
    
    Result Commit();
    
    Result Rollback();
    
    Result SavePoint(const std::string& name);
    
    Result ReleaseSavePoint(const std::string& name);
    
    Result RollbackToSavePoint(const std::string& name);
    
    bool InTransaction() const { return m_inTransaction; }
    
    TransactionIsolation GetIsolationLevel() const { return m_isolationLevel; }
    
    Result SetIsolationLevel(TransactionIsolation level);
    
    int GetSavePointCount() const { return static_cast<int>(m_savePoints.size()); }

private:
    SpatiaLiteConnection* m_connection;
    bool m_inTransaction;
    TransactionIsolation m_isolationLevel;
    std::stack<std::string> m_savePoints;
    
    std::string GetIsolationString(TransactionIsolation isolation) const;
    bool HasSavePoint(const std::string& name) const;
};

class OGC_DB_API SqliteSavePoint {
public:
    SqliteSavePoint(SpatiaLiteConnection* connection, const std::string& name);
    ~SqliteSavePoint();
    
    Result Commit();
    
    void Rollback();
    
    const std::string& GetName() const { return m_name; }
    bool IsActive() const { return m_active; }

private:
    SpatiaLiteConnection* m_connection;
    std::string m_name;
    bool m_active;
};

inline SqliteSavePoint::SqliteSavePoint(SpatiaLiteConnection* connection, const std::string& name)
    : m_connection(connection), m_name(name), m_active(false) {
    if (m_connection && m_connection->IsConnected()) {
        Result result = m_connection->Execute("SAVEPOINT " + m_name);
        m_active = result.IsSuccess();
    }
}

inline SqliteSavePoint::~SqliteSavePoint() {
    if (m_active && m_connection && m_connection->IsConnected()) {
        m_connection->Execute("RELEASE SAVEPOINT " + m_name);
    }
}

inline Result SqliteSavePoint::Commit() {
    if (!m_active) {
        return Result::Error(DbResult::kTransactionError, "Savepoint not active");
    }
    
    Result result = m_connection->Execute("RELEASE SAVEPOINT " + m_name);
    if (result.IsSuccess()) {
        m_active = false;
    }
    return result;
}

inline void SqliteSavePoint::Rollback() {
    if (m_active && m_connection && m_connection->IsConnected()) {
        m_connection->Execute("ROLLBACK TO SAVEPOINT " + m_name);
        m_active = false;
    }
}

}
}
