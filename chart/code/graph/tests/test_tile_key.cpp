#include <gtest/gtest.h>
#include "ogc/draw/tile_key.h"

TEST(TileKeyTest, DefaultConstructor) {
    ogc::draw::TileKey key;
    EXPECT_EQ(key.x, 0);
    EXPECT_EQ(key.y, 0);
    EXPECT_EQ(key.z, 0);
}

TEST(TileKeyTest, ParameterizedConstructor) {
    ogc::draw::TileKey key(10, 20, 5);
    EXPECT_EQ(key.x, 10);
    EXPECT_EQ(key.y, 20);
    EXPECT_EQ(key.z, 5);
}

TEST(TileKeyTest, Equality) {
    ogc::draw::TileKey key1(10, 20, 5);
    ogc::draw::TileKey key2(10, 20, 5);
    ogc::draw::TileKey key3(11, 20, 5);
    ogc::draw::TileKey key4(10, 21, 5);
    ogc::draw::TileKey key5(10, 20, 6);
    
    EXPECT_TRUE(key1 == key2);
    EXPECT_FALSE(key1 == key3);
    EXPECT_FALSE(key1 == key4);
    EXPECT_FALSE(key1 == key5);
    
    EXPECT_FALSE(key1 != key2);
    EXPECT_TRUE(key1 != key3);
}

TEST(TileKeyTest, LessThan) {
    ogc::draw::TileKey key1(10, 20, 5);
    ogc::draw::TileKey key2(10, 20, 6);
    ogc::draw::TileKey key3(9, 20, 5);
    ogc::draw::TileKey key4(10, 19, 5);
    
    EXPECT_TRUE(key1 < key2);
    EXPECT_FALSE(key2 < key1);
    EXPECT_TRUE(key3 < key1);
    EXPECT_TRUE(key4 < key1);
}

TEST(TileKeyTest, ToString) {
    ogc::draw::TileKey key(10, 20, 5);
    EXPECT_EQ(key.ToString(), "5/10/20");
}

TEST(TileKeyTest, FromString) {
    ogc::draw::TileKey key = ogc::draw::TileKey::FromString("5/10/20");
    EXPECT_EQ(key.z, 5);
    EXPECT_EQ(key.x, 10);
    EXPECT_EQ(key.y, 20);
}

TEST(TileKeyTest, ToIndex) {
    ogc::draw::TileKey key(10, 20, 5);
    uint64_t index = key.ToIndex();
    ogc::draw::TileKey key2 = ogc::draw::TileKey::FromIndex(index);
    EXPECT_EQ(key.x, key2.x);
    EXPECT_EQ(key.y, key2.y);
    EXPECT_EQ(key.z, key2.z);
}

TEST(TileKeyTest, GetParent) {
    ogc::draw::TileKey key(10, 20, 5);
    ogc::draw::TileKey parent = key.GetParent();
    EXPECT_EQ(parent.z, 4);
    EXPECT_EQ(parent.x, 5);
    EXPECT_EQ(parent.y, 10);
}

TEST(TileKeyTest, GetParentAtLevel0) {
    ogc::draw::TileKey key(0, 0, 0);
    ogc::draw::TileKey parent = key.GetParent();
    EXPECT_EQ(parent.z, 0);
    EXPECT_EQ(parent.x, 0);
    EXPECT_EQ(parent.y, 0);
}

TEST(TileKeyTest, GetChild) {
    ogc::draw::TileKey key(5, 10, 4);
    
    ogc::draw::TileKey child0 = key.GetChild(0);
    EXPECT_EQ(child0.z, 5);
    EXPECT_EQ(child0.x, 10);
    EXPECT_EQ(child0.y, 20);
    
    ogc::draw::TileKey child1 = key.GetChild(1);
    EXPECT_EQ(child1.z, 5);
    EXPECT_EQ(child1.x, 11);
    EXPECT_EQ(child1.y, 20);
    
    ogc::draw::TileKey child2 = key.GetChild(2);
    EXPECT_EQ(child2.z, 5);
    EXPECT_EQ(child2.x, 10);
    EXPECT_EQ(child2.y, 21);
    
    ogc::draw::TileKey child3 = key.GetChild(3);
    EXPECT_EQ(child3.z, 5);
    EXPECT_EQ(child3.x, 11);
    EXPECT_EQ(child3.y, 21);
}

TEST(TileKeyTest, GetChildren) {
    ogc::draw::TileKey key(5, 10, 4);
    ogc::draw::TileKey children[4];
    key.GetChildren(children);
    
    EXPECT_EQ(children[0].z, 5);
    EXPECT_EQ(children[1].z, 5);
    EXPECT_EQ(children[2].z, 5);
    EXPECT_EQ(children[3].z, 5);
    
    EXPECT_EQ(children[0].x, 10);
    EXPECT_EQ(children[1].x, 11);
    EXPECT_EQ(children[2].x, 10);
    EXPECT_EQ(children[3].x, 11);
}

TEST(TileKeyTest, IsValid) {
    ogc::draw::TileKey valid(5, 5, 3);
    EXPECT_TRUE(valid.IsValid());
    
    ogc::draw::TileKey invalidZ(5, 10, -1);
    EXPECT_FALSE(invalidZ.IsValid());
    
    ogc::draw::TileKey invalidX(10, 5, 3);
    EXPECT_FALSE(invalidX.IsValid());
    
    ogc::draw::TileKey invalidY(5, 10, 3);
    EXPECT_FALSE(invalidY.IsValid());
    
    ogc::draw::TileKey validAtEdge(7, 7, 3);
    EXPECT_TRUE(validAtEdge.IsValid());
    
    ogc::draw::TileKey outOfRange(8, 0, 3);
    EXPECT_FALSE(outOfRange.IsValid());
}

TEST(TileKeyTest, GetMaxTiles) {
    ogc::draw::TileKey key0(0, 0, 0);
    EXPECT_EQ(key0.GetMaxTiles(), 1);
    
    ogc::draw::TileKey key1(0, 0, 1);
    EXPECT_EQ(key1.GetMaxTiles(), 2);
    
    ogc::draw::TileKey key5(0, 0, 5);
    EXPECT_EQ(key5.GetMaxTiles(), 32);
}

TEST(TileKeyTest, InvalidChildIndex) {
    ogc::draw::TileKey key(5, 10, 4);
    ogc::draw::TileKey invalid = key.GetChild(-1);
    EXPECT_EQ(invalid.z, 0);
    EXPECT_EQ(invalid.x, 0);
    EXPECT_EQ(invalid.y, 0);
    
    ogc::draw::TileKey invalid2 = key.GetChild(4);
    EXPECT_EQ(invalid2.z, 0);
    EXPECT_EQ(invalid2.x, 0);
    EXPECT_EQ(invalid2.y, 0);
}
