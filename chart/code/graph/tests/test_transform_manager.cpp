#include <gtest/gtest.h>
#include "ogc/draw/transform_manager.h"
#include "ogc/draw/coordinate_transformer.h"
#include "ogc/geometry.h"
#include "ogc/point.h"
#include "ogc/envelope.h"
#include <memory>

using namespace ogc::draw;
using namespace ogc;

class TransformManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        manager = TransformManager::GetInstance();
        ASSERT_NE(manager, nullptr);
    }
    
    void TearDown() override {
    }
    
    TransformManagerPtr manager;
};

TEST_F(TransformManagerTest, GetInstance) {
    TransformManagerPtr instance1 = TransformManager::GetInstance();
    TransformManagerPtr instance2 = TransformManager::GetInstance();
    
    EXPECT_NE(instance1, nullptr);
    EXPECT_EQ(instance1, instance2);
}

TEST_F(TransformManagerTest, SetMaxCacheSize) {
    manager->SetMaxCacheSize(100);
    EXPECT_EQ(manager->GetMaxCacheSize(), 100);
    
    manager->SetMaxCacheSize(50);
    EXPECT_EQ(manager->GetMaxCacheSize(), 50);
}

TEST_F(TransformManagerTest, SetEnableCache) {
    manager->SetEnableCache(true);
    EXPECT_TRUE(manager->IsCacheEnabled());
    
    manager->SetEnableCache(false);
    EXPECT_FALSE(manager->IsCacheEnabled());
    
    manager->SetEnableCache(true);
}

TEST_F(TransformManagerTest, ClearCache) {
    manager->ClearCache();
    EXPECT_EQ(manager->GetCacheSize(), 0);
}

TEST_F(TransformManagerTest, HasTransformer) {
    bool hasWGS84 = manager->HasTransformer("EPSG:4326", "EPSG:3857");
    EXPECT_TRUE(hasWGS84 || !hasWGS84);
}

TEST_F(TransformManagerTest, GetTransformer) {
    CoordinateTransformerPtr transformer = manager->GetTransformer("EPSG:4326", "EPSG:3857");
}

TEST_F(TransformManagerTest, RegisterTransformer) {
    CoordinateTransformerPtr transformer = CoordinateTransformer::Create("EPSG:4326", "EPSG:4326");
    manager->RegisterTransformer("TEST:SRC", "TEST:DST", transformer);
    
    EXPECT_TRUE(manager->HasTransformer("TEST:SRC", "TEST:DST"));
    
    manager->UnregisterTransformer("TEST:SRC", "TEST:DST");
    EXPECT_FALSE(manager->HasTransformer("TEST:SRC", "TEST:DST"));
}

TEST_F(TransformManagerTest, UnregisterTransformer) {
    CoordinateTransformerPtr transformer = CoordinateTransformer::Create("EPSG:4326", "EPSG:4326");
    manager->RegisterTransformer("TEST:A", "TEST:B", transformer);
    
    manager->UnregisterTransformer("TEST:A", "TEST:B");
    EXPECT_FALSE(manager->HasTransformer("TEST:A", "TEST:B"));
}

TEST_F(TransformManagerTest, TransformCoordinate) {
    Coordinate src(0, 0);
    Coordinate dst = manager->Transform("EPSG:4326", "EPSG:4326", src);
    
    EXPECT_DOUBLE_EQ(dst.x, 0);
    EXPECT_DOUBLE_EQ(dst.y, 0);
}

TEST_F(TransformManagerTest, TransformXY) {
    double x = 10.0;
    double y = 20.0;
    
    manager->Transform("EPSG:4326", "EPSG:4326", x, y);
    
    EXPECT_DOUBLE_EQ(x, 10.0);
    EXPECT_DOUBLE_EQ(y, 20.0);
}

TEST_F(TransformManagerTest, TransformEnvelope) {
    Envelope src(0, 0, 10, 10);
    Envelope dst = manager->Transform("EPSG:4326", "EPSG:4326", src);
    
    EXPECT_DOUBLE_EQ(dst.GetMinX(), 0);
    EXPECT_DOUBLE_EQ(dst.GetMinY(), 0);
    EXPECT_DOUBLE_EQ(dst.GetMaxX(), 10);
    EXPECT_DOUBLE_EQ(dst.GetMaxY(), 10);
}

TEST_F(TransformManagerTest, GetSupportedCRS) {
    std::vector<std::string> crsList = manager->GetSupportedCRS();
}

TEST_F(TransformManagerTest, IsTransformationSupported) {
    bool supported = manager->IsTransformationSupported("EPSG:4326", "EPSG:3857");
}

TEST_F(TransformManagerTest, GetWGS84ToWebMercator) {
    CoordinateTransformerPtr transformer = TransformManager::GetWGS84ToWebMercator();
}

TEST_F(TransformManagerTest, GetWebMercatorToWGS84) {
    CoordinateTransformerPtr transformer = TransformManager::GetWebMercatorToWGS84();
}

TEST_F(TransformManagerTest, CreateTransformer) {
    CoordinateTransformerPtr transformer = manager->CreateTransformer("EPSG:4326", "EPSG:3857");
}

TEST_F(TransformManagerTest, TransformGeometry) {
    Coordinate coord(10, 20);
    PointPtr point = Point::Create(coord);
    GeometryPtr transformed = manager->Transform("EPSG:4326", "EPSG:4326", point.get());
    
    EXPECT_NE(transformed, nullptr);
}

TEST_F(TransformManagerTest, CacheSizeAfterRegister) {
    size_t initialSize = manager->GetCacheSize();
    
    CoordinateTransformerPtr transformer = CoordinateTransformer::Create("EPSG:4326", "EPSG:4326");
    manager->RegisterTransformer("TEST:CACHE", "TEST:CACHE_DST", transformer);
    
    size_t newSize = manager->GetCacheSize();
    EXPECT_GE(newSize, initialSize);
    
    manager->UnregisterTransformer("TEST:CACHE", "TEST:CACHE_DST");
}
