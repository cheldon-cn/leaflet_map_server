#include <gtest/gtest.h>
#include <ogc/proj/coord_system_preset.h>

using namespace ogc::proj;

class CoordSystemPresetTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(CoordSystemPresetTest, Instance) {
    auto& instance = CoordSystemPreset::Instance();
    EXPECT_TRUE(true);
}

TEST_F(CoordSystemPresetTest, GetCoordSystem) {
    auto& instance = CoordSystemPreset::Instance();
    auto wgs84 = instance.GetCoordSystem("WGS84");
    EXPECT_EQ(wgs84.name, "WGS84");
    EXPECT_EQ(wgs84.epsg, "EPSG:4326");
}

TEST_F(CoordSystemPresetTest, GetCoordSystemByEpsg) {
    auto& instance = CoordSystemPreset::Instance();
    auto crs = instance.GetCoordSystemByEpsg("EPSG:4326");
    EXPECT_EQ(crs.name, "WGS84");
}

TEST_F(CoordSystemPresetTest, GetCoordSystemNames) {
    auto& instance = CoordSystemPreset::Instance();
    auto names = instance.GetCoordSystemNames();
    EXPECT_FALSE(names.empty());
}

TEST_F(CoordSystemPresetTest, GetAllCoordSystems) {
    auto& instance = CoordSystemPreset::Instance();
    auto systems = instance.GetAllCoordSystems();
    EXPECT_FALSE(systems.empty());
}

TEST_F(CoordSystemPresetTest, RegisterCoordSystem) {
    auto& instance = CoordSystemPreset::Instance();
    
    CoordSystemInfo info;
    info.name = "TestCRS";
    info.epsg = "EPSG:9999";
    info.description = "Test coordinate system";
    
    instance.RegisterCoordSystem(info);
    EXPECT_TRUE(instance.HasCoordSystem("TestCRS"));
    
    auto retrieved = instance.GetCoordSystem("TestCRS");
    EXPECT_EQ(retrieved.name, "TestCRS");
    EXPECT_EQ(retrieved.epsg, "EPSG:9999");
    
    instance.RemoveCoordSystem("TestCRS");
}

TEST_F(CoordSystemPresetTest, RemoveCoordSystem) {
    auto& instance = CoordSystemPreset::Instance();
    
    CoordSystemInfo info;
    info.name = "TempCRS";
    info.epsg = "EPSG:9998";
    
    instance.RegisterCoordSystem(info);
    EXPECT_TRUE(instance.HasCoordSystem("TempCRS"));
    
    bool removed = instance.RemoveCoordSystem("TempCRS");
    EXPECT_TRUE(removed);
    EXPECT_FALSE(instance.HasCoordSystem("TempCRS"));
}

TEST_F(CoordSystemPresetTest, HasCoordSystem) {
    auto& instance = CoordSystemPreset::Instance();
    EXPECT_TRUE(instance.HasCoordSystem("WGS84"));
    EXPECT_FALSE(instance.HasCoordSystem("NonExistent"));
}

TEST_F(CoordSystemPresetTest, StaticWGS84) {
    auto wgs84 = CoordSystemPreset::WGS84();
    EXPECT_EQ(wgs84, "EPSG:4326");
}

TEST_F(CoordSystemPresetTest, StaticWebMercator) {
    auto mercator = CoordSystemPreset::WebMercator();
    EXPECT_EQ(mercator, "EPSG:3857");
}

TEST_F(CoordSystemPresetTest, StaticWGS84UTM) {
    auto utmN50 = CoordSystemPreset::WGS84_UTM(50, true);
    EXPECT_EQ(utmN50, "EPSG:32650");
    
    auto utmS50 = CoordSystemPreset::WGS84_UTM(50, false);
    EXPECT_EQ(utmS50, "EPSG:32750");
    
    auto invalid = CoordSystemPreset::WGS84_UTM(0, true);
    EXPECT_TRUE(invalid.empty());
    
    auto invalid2 = CoordSystemPreset::WGS84_UTM(61, true);
    EXPECT_TRUE(invalid2.empty());
}

TEST_F(CoordSystemPresetTest, StaticCGCS2000) {
    auto cgcs2000 = CoordSystemPreset::CGCS2000();
    EXPECT_EQ(cgcs2000, "EPSG:4490");
}

TEST_F(CoordSystemPresetTest, StaticCGCS2000UTM) {
    auto utmN50 = CoordSystemPreset::CGCS2000_UTM(50, true);
    EXPECT_EQ(utmN50, "EPSG:4562");
    
    auto utmS50 = CoordSystemPreset::CGCS2000_UTM(50, false);
    EXPECT_EQ(utmS50, "EPSG:4604");
}

TEST_F(CoordSystemPresetTest, StaticBeijing54) {
    auto beijing54 = CoordSystemPreset::Beijing54();
    EXPECT_EQ(beijing54, "EPSG:4214");
}

TEST_F(CoordSystemPresetTest, StaticXian80) {
    auto xian80 = CoordSystemPreset::Xian80();
    EXPECT_EQ(xian80, "EPSG:4610");
}

TEST_F(CoordSystemPresetTest, StaticGCJ02) {
    auto gcj02 = CoordSystemPreset::GCJ02();
    EXPECT_EQ(gcj02, "GCJ02");
}

TEST_F(CoordSystemPresetTest, StaticBD09) {
    auto bd09 = CoordSystemPreset::BD09();
    EXPECT_EQ(bd09, "BD09");
}

TEST_F(CoordSystemPresetTest, IsChineseCoordSystem) {
    EXPECT_TRUE(CoordSystemPreset::IsChineseCoordSystem("CGCS2000"));
    EXPECT_TRUE(CoordSystemPreset::IsChineseCoordSystem("EPSG:4490"));
    EXPECT_TRUE(CoordSystemPreset::IsChineseCoordSystem("Beijing54"));
    EXPECT_TRUE(CoordSystemPreset::IsChineseCoordSystem("EPSG:4214"));
    EXPECT_TRUE(CoordSystemPreset::IsChineseCoordSystem("Xian80"));
    EXPECT_TRUE(CoordSystemPreset::IsChineseCoordSystem("EPSG:4610"));
    EXPECT_TRUE(CoordSystemPreset::IsChineseCoordSystem("GCJ02"));
    EXPECT_TRUE(CoordSystemPreset::IsChineseCoordSystem("BD09"));
    
    EXPECT_FALSE(CoordSystemPreset::IsChineseCoordSystem("WGS84"));
    EXPECT_FALSE(CoordSystemPreset::IsChineseCoordSystem("EPSG:4326"));
}

TEST_F(CoordSystemPresetTest, IsGeographicCRS) {
    EXPECT_TRUE(CoordSystemPreset::IsGeographicCRS("WGS84"));
    EXPECT_TRUE(CoordSystemPreset::IsGeographicCRS("EPSG:4326"));
    EXPECT_TRUE(CoordSystemPreset::IsGeographicCRS("CGCS2000"));
    EXPECT_TRUE(CoordSystemPreset::IsGeographicCRS("EPSG:4490"));
    EXPECT_TRUE(CoordSystemPreset::IsGeographicCRS("Beijing54"));
    EXPECT_TRUE(CoordSystemPreset::IsGeographicCRS("Xian80"));
    
    EXPECT_FALSE(CoordSystemPreset::IsGeographicCRS("EPSG:3857"));
    EXPECT_FALSE(CoordSystemPreset::IsGeographicCRS("WebMercator"));
}

TEST_F(CoordSystemPresetTest, IsProjectedCRS) {
    EXPECT_TRUE(CoordSystemPreset::IsProjectedCRS("EPSG:3857"));
    EXPECT_TRUE(CoordSystemPreset::IsProjectedCRS("WebMercator"));
    EXPECT_TRUE(CoordSystemPreset::IsProjectedCRS("WGS84_UTM_N50"));
    EXPECT_TRUE(CoordSystemPreset::IsProjectedCRS("UTM zone 50N"));
    
    EXPECT_FALSE(CoordSystemPreset::IsProjectedCRS("WGS84"));
    EXPECT_FALSE(CoordSystemPreset::IsProjectedCRS("EPSG:4326"));
}

TEST_F(CoordSystemPresetTest, ChineseCoordSystemsExist) {
    auto& instance = CoordSystemPreset::Instance();
    
    EXPECT_TRUE(instance.HasCoordSystem("CGCS2000"));
    EXPECT_TRUE(instance.HasCoordSystem("Beijing54"));
    EXPECT_TRUE(instance.HasCoordSystem("Xian80"));
    EXPECT_TRUE(instance.HasCoordSystem("GCJ02"));
    EXPECT_TRUE(instance.HasCoordSystem("BD09"));
}

TEST_F(CoordSystemPresetTest, UTMZonesExist) {
    auto& instance = CoordSystemPreset::Instance();
    
    EXPECT_TRUE(instance.HasCoordSystem("WGS84_UTM_N50"));
    EXPECT_TRUE(instance.HasCoordSystem("WGS84_UTM_S50"));
    EXPECT_TRUE(instance.HasCoordSystem("CGCS2000_UTM_N50"));
    EXPECT_TRUE(instance.HasCoordSystem("CGCS2000_UTM_S50"));
}
