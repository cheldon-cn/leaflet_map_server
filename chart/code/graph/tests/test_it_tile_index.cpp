#include <gtest/gtest.h>
#include "ogc/draw/tile_key.h"
#include <ogc/draw/tile_device.h>
#include <ogc/draw/raster_image_device.h>
#include <ogc/draw/draw_context.h>
#include "ogc/draw/draw_params.h"
#include <ogc/draw/draw_style.h>
#include <ogc/draw/color.h>
#include "ogc/envelope.h"
#include <memory>

using namespace ogc::draw;

class IntegrationTileIndexTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_device = std::make_shared<RasterImageDevice>(256, 256, PixelFormat::kRGBA8888);
        ASSERT_NE(m_device, nullptr);
        m_device->Initialize();
        
        m_context = DrawContext::Create(m_device.get());
        ASSERT_NE(m_context, nullptr);
    }
    
    void TearDown() override {
        m_context.reset();
        m_device.reset();
    }
    
    std::shared_ptr<RasterImageDevice> m_device;
    std::unique_ptr<DrawContext> m_context;
};

TEST_F(IntegrationTileIndexTest, TileKeyBasicOperations) {
    TileKey key(3, 5, 10);
    
    EXPECT_EQ(key.x, 3);
    EXPECT_EQ(key.y, 5);
    EXPECT_EQ(key.z, 10);
}

TEST_F(IntegrationTileIndexTest, TileKeyIsValid) {
    TileKey validKey(0, 0, 0);
    EXPECT_TRUE(validKey.IsValid());
    
    TileKey invalidKey(-1, 0, 0);
    EXPECT_FALSE(invalidKey.IsValid());
    
    TileKey outOfRangeKey(10, 0, 3);
    EXPECT_FALSE(outOfRangeKey.IsValid());
}

TEST_F(IntegrationTileIndexTest, TileKeyComparison) {
    TileKey key1(1, 2, 3);
    TileKey key2(1, 2, 3);
    TileKey key3(2, 2, 3);
    
    EXPECT_TRUE(key1 == key2);
    EXPECT_FALSE(key1 == key3);
    EXPECT_TRUE(key1 != key3);
    EXPECT_FALSE(key1 != key2);
}

TEST_F(IntegrationTileIndexTest, TileKeyHash) {
    TileKey key1(1, 2, 3);
    TileKey key2(1, 2, 3);
    TileKey key3(2, 2, 3);
    
    std::hash<TileKey> hasher;
    EXPECT_EQ(hasher(key1), hasher(key2));
    EXPECT_NE(hasher(key1), hasher(key3));
}

TEST_F(IntegrationTileIndexTest, TileKeyParent) {
    TileKey key(2, 3, 10);
    TileKey parent = key.GetParent();
    
    EXPECT_EQ(parent.x, 1);
    EXPECT_EQ(parent.y, 1);
    EXPECT_EQ(parent.z, 9);
}

TEST_F(IntegrationTileIndexTest, TileKeyChildren) {
    TileKey key(1, 1, 1);
    TileKey children[4];
    key.GetChildren(children);
    
    EXPECT_EQ(children[0].x, 2);
    EXPECT_EQ(children[0].y, 2);
    EXPECT_EQ(children[0].z, 2);
    
    EXPECT_EQ(children[1].x, 3);
    EXPECT_EQ(children[1].y, 2);
    EXPECT_EQ(children[1].z, 2);
    
    EXPECT_EQ(children[2].x, 2);
    EXPECT_EQ(children[2].y, 3);
    EXPECT_EQ(children[2].z, 2);
    
    EXPECT_EQ(children[3].x, 3);
    EXPECT_EQ(children[3].y, 3);
    EXPECT_EQ(children[3].z, 2);
}

TEST_F(IntegrationTileIndexTest, TileKeyToString) {
    TileKey key(1, 2, 3);
    std::string str = key.ToString();
    EXPECT_EQ(str, "3/1/2");
    
    TileKey parsed = TileKey::FromString(str);
    EXPECT_EQ(parsed.x, 1);
    EXPECT_EQ(parsed.y, 2);
    EXPECT_EQ(parsed.z, 3);
}

TEST_F(IntegrationTileIndexTest, TileKeyToIndex) {
    TileKey key(1, 2, 3);
    uint64_t index = key.ToIndex();
    
    TileKey parsed = TileKey::FromIndex(index);
    EXPECT_EQ(parsed.x, 1);
    EXPECT_EQ(parsed.y, 2);
    EXPECT_EQ(parsed.z, 3);
}

TEST_F(IntegrationTileIndexTest, TileKeyZoomLevelRange) {
    for (int z = 0; z <= 18; ++z) {
        int maxTiles = 1 << z;
        
        TileKey validKey(0, 0, z);
        EXPECT_TRUE(validKey.IsValid());
        
        TileKey edgeKey(maxTiles - 1, maxTiles - 1, z);
        EXPECT_TRUE(edgeKey.IsValid());
        
        TileKey outOfRangeKey(maxTiles, 0, z);
        EXPECT_FALSE(outOfRangeKey.IsValid());
    }
}

TEST_F(IntegrationTileIndexTest, TileDeviceBasicOperations) {
    auto tileDevice = std::make_unique<TileDevice>(512, 512, 256);
    ASSERT_NE(tileDevice, nullptr);
    
    EXPECT_EQ(tileDevice->GetTileSize(), 256);
}

TEST_F(IntegrationTileIndexTest, TileDeviceInitialize) {
    auto tileDevice = std::make_unique<TileDevice>(512, 512, 256);
    
    DrawResult result = tileDevice->Initialize();
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    result = tileDevice->Finalize();
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(IntegrationTileIndexTest, TileDeviceState) {
    auto tileDevice = std::make_unique<TileDevice>(512, 512, 256);
    tileDevice->Initialize();
    
    EXPECT_EQ(tileDevice->GetState(), DeviceState::kReady);
    
    tileDevice->Finalize();
}

TEST_F(IntegrationTileIndexTest, TileDeviceGetSize) {
    auto tileDevice = std::make_unique<TileDevice>(1024, 1024, 512);
    
    EXPECT_EQ(tileDevice->GetTileSize(), 512);
    EXPECT_EQ(tileDevice->GetWidth(), 1024);
    EXPECT_EQ(tileDevice->GetHeight(), 1024);
}

TEST_F(IntegrationTileIndexTest, TileDeviceTileInfo) {
    auto tileDevice = std::make_unique<TileDevice>(512, 512, 256);
    tileDevice->Initialize();
    
    TileInfo info = tileDevice->GetTileInfo(0, 0);
    EXPECT_EQ(info.indexX, 0);
    EXPECT_EQ(info.indexY, 0);
    EXPECT_EQ(info.width, 256);
    EXPECT_EQ(info.height, 256);
    
    tileDevice->Finalize();
}

TEST_F(IntegrationTileIndexTest, RasterDeviceTileRender) {
    m_context->Begin();
    m_context->Clear(Color::White());
    
    DrawStyle style;
    style.pen = Pen(Color::Blue(), 2.0);
    m_context->SetStyle(style);
    
    DrawResult result = m_context->DrawRect(10, 10, 236, 236);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_context->End();
    
    Color pixel = m_device->GetPixel(5, 5);
    EXPECT_EQ(pixel.GetRed(), 255);
    EXPECT_EQ(pixel.GetGreen(), 255);
    EXPECT_EQ(pixel.GetBlue(), 255);
}
