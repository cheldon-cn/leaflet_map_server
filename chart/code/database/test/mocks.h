#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <string>

namespace ogc {
namespace db {
namespace test {

class MockDbConnection : public DbConnection {
public:
    MOCK_METHOD(Result, Connect, (const std::string&), (override));
    MOCK_METHOD(void, Disconnect, (), (override));
    MOCK_METHOD(bool, IsConnected, (), (const, override));
    MOCK_METHOD(DatabaseType, GetType, (), (const, override));
    MOCK_METHOD(Result, Execute, (const std::string&), (override));
    MOCK_METHOD(Result, ExecuteQuery, (const std::string&, DbResultSetPtr&), (override));
    MOCK_METHOD(Result, BeginTransaction, (), (override));
    MOCK_METHOD(Result, Commit, (), (override));
    MOCK_METHOD(Result, Rollback, (), (override));
    MOCK_METHOD(Result, InsertGeometry, (const std::string&, const std::string&, const Geometry*, const std::map<std::string, std::string>&, int64_t&), (override));
};

class MockDbStatement : public DbStatement {
public:
    MOCK_METHOD(Result, Prepare, (const std::string&), (override));
    MOCK_METHOD(Result, Execute, (), (override));
    MOCK_METHOD(Result, ExecuteQuery, (DbResultSetPtr&), (override));
    MOCK_METHOD(Result, BindInt, (int, int32_t), (override));
    MOCK_METHOD(Result, BindInt64, (int, int64_t), (override));
    MOCK_METHOD(Result, BindDouble, (int, double), (override));
    MOCK_METHOD(Result, BindString, (int, const std::string&), (override));
    MOCK_METHOD(Result, BindGeometry, (int, const Geometry*), (override));
    MOCK_METHOD(Result, BindNull, (int), (override));
    MOCK_METHOD(void, Reset, (), (override));
    MOCK_METHOD(void, Close, (), (override));
};

class MockDbResultSet : public DbResultSet {
public:
    MOCK_METHOD(bool, Next, (), (override));
    MOCK_METHOD(bool, IsEOF, (), (const, override));
    MOCK_METHOD(int32_t, GetInt, (int), (const, override));
    MOCK_METHOD(int64_t, GetInt64, (int), (const, override));
    MOCK_METHOD(double, GetDouble, (int), (const, override));
    MOCK_METHOD(std::string, GetString, (int), (const, override));
    MOCK_METHOD(std::vector<uint8_t>, GetBlob, (int), (const, override));
    MOCK_METHOD(bool, IsNull, (int), (const, override));
    MOCK_METHOD(int, GetColumnCount, (), (const, override));
    MOCK_METHOD(std::string, GetColumnName, (int), (const, override));
    MOCK_METHOD(void, Close, (), (override));
};

class WkbConverterTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

class PostgisConnectionTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

class SpatiaLiteConnectionTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

class ConnectionPoolTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

class GeoJsonConverterTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

class SridManagerTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

class AsyncConnectionTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

}
}
}
