#include <gtest/gtest.h>
#include "ogc/mokrender/database_manager.h"
#include "ogc/mokrender/common.h"
#include "ogc/point.h"

using namespace ogc::mokrender;

class DatabaseManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        dbManager = new DatabaseManager();
        testDbName = "test_db_manager_" + std::to_string(rand()) + ".db";
    }
    
    void TearDown() override {
        delete dbManager;
    }
    
    DatabaseManager* dbManager;
    std::string testDbName;
};

TEST_F(DatabaseManagerTest, InitializeWithValidPath) {
    MokRenderResult result = dbManager->Initialize(testDbName);
    EXPECT_TRUE(result.IsSuccess());
    dbManager->Close();
}

TEST_F(DatabaseManagerTest, InitializeWithEmptyPath) {
    MokRenderResult result = dbManager->Initialize("");
    EXPECT_TRUE(result.IsSuccess());
    dbManager->Close();
}

TEST_F(DatabaseManagerTest, InitializeTwice) {
    dbManager->Initialize(testDbName);
    MokRenderResult result = dbManager->Initialize(testDbName);
    EXPECT_TRUE(result.IsSuccess());
    dbManager->Close();
}

TEST_F(DatabaseManagerTest, CreateTableWithoutInitialize) {
    MokRenderResult result = dbManager->CreateTable("test_table", "Point");
    EXPECT_TRUE(result.IsError());
    EXPECT_EQ(result.code, MokRenderErrorCode::DATABASE_ERROR);
}

TEST_F(DatabaseManagerTest, InsertFeatureWithoutInitialize) {
    MokRenderResult result = dbManager->InsertFeature("test_table", nullptr);
    EXPECT_TRUE(result.IsError());
    EXPECT_EQ(result.code, MokRenderErrorCode::DATABASE_ERROR);
}

TEST_F(DatabaseManagerTest, CreateTableAfterInitialize) {
    dbManager->Initialize(testDbName);
    MokRenderResult result = dbManager->CreateTable("point_layer", "Point");
    EXPECT_TRUE(result.IsSuccess());
    dbManager->Close();
}

TEST_F(DatabaseManagerTest, CreateTableWithInvalidGeometryType) {
    dbManager->Initialize(testDbName);
    MokRenderResult result = dbManager->CreateTable("test_table", "InvalidType");
    EXPECT_TRUE(result.IsSuccess());
    dbManager->Close();
}

TEST_F(DatabaseManagerTest, CreateTableWithEmptyName) {
    dbManager->Initialize(testDbName);
    MokRenderResult result = dbManager->CreateTable("", "Point");
    EXPECT_TRUE(result.IsError());
    dbManager->Close();
}

TEST_F(DatabaseManagerTest, CreateTableAllGeometryTypes) {
    dbManager->Initialize(testDbName);
    
    EXPECT_TRUE(dbManager->CreateTable("point_layer", "Point").IsSuccess());
    EXPECT_TRUE(dbManager->CreateTable("line_layer", "LineString").IsSuccess());
    EXPECT_TRUE(dbManager->CreateTable("polygon_layer", "Polygon").IsSuccess());
    EXPECT_TRUE(dbManager->CreateTable("multipoint_layer", "MultiPoint").IsSuccess());
    EXPECT_TRUE(dbManager->CreateTable("multiline_layer", "MultiLineString").IsSuccess());
    EXPECT_TRUE(dbManager->CreateTable("multipolygon_layer", "MultiPolygon").IsSuccess());
    
    dbManager->Close();
}

TEST_F(DatabaseManagerTest, CreateTableDuplicate) {
    dbManager->Initialize(testDbName);
    
    MokRenderResult result1 = dbManager->CreateTable("dup_table", "Point");
    EXPECT_TRUE(result1.IsSuccess());
    
    MokRenderResult result2 = dbManager->CreateTable("dup_table", "Point");
    EXPECT_TRUE(result2.IsSuccess());
    
    dbManager->Close();
}

TEST_F(DatabaseManagerTest, InsertFeatureWithNullFeature) {
    dbManager->Initialize(testDbName);
    dbManager->CreateTable("point_layer", "Point");
    
    MokRenderResult result = dbManager->InsertFeature("point_layer", nullptr);
    EXPECT_TRUE(result.IsError());
    
    dbManager->Close();
}

TEST_F(DatabaseManagerTest, InsertFeatureToNonExistentTable) {
    dbManager->Initialize(testDbName);
    
    auto point = ogc::Point::Create(1.0, 2.0);
    MokRenderResult result = dbManager->InsertFeature("nonexistent", point.get());
    EXPECT_TRUE(result.IsError());
    
    dbManager->Close();
}

TEST_F(DatabaseManagerTest, CloseWithoutInitialize) {
    dbManager->Close();
}

TEST_F(DatabaseManagerTest, CloseTwice) {
    dbManager->Initialize(testDbName);
    dbManager->Close();
    dbManager->Close();
}
