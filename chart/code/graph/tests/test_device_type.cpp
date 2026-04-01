#include <gtest/gtest.h>
#include <ogc/draw/device_type.h>

TEST(DeviceTypeTest, AllValues) {
    EXPECT_EQ(static_cast<int>(ogc::draw::DeviceType::kUnknown), 0);
    EXPECT_EQ(static_cast<int>(ogc::draw::DeviceType::kRasterImage), 1);
    EXPECT_EQ(static_cast<int>(ogc::draw::DeviceType::kPdf), 2);
    EXPECT_EQ(static_cast<int>(ogc::draw::DeviceType::kTile), 3);
    EXPECT_EQ(static_cast<int>(ogc::draw::DeviceType::kDisplay), 4);
    EXPECT_EQ(static_cast<int>(ogc::draw::DeviceType::kSvg), 5);
    EXPECT_EQ(static_cast<int>(ogc::draw::DeviceType::kPrinter), 6);
}

TEST(DeviceTypeTest, GetDeviceTypeString) {
    EXPECT_EQ(ogc::draw::GetDeviceTypeString(ogc::draw::DeviceType::kUnknown), "Unknown");
    EXPECT_EQ(ogc::draw::GetDeviceTypeString(ogc::draw::DeviceType::kRasterImage), "RasterImage");
    EXPECT_EQ(ogc::draw::GetDeviceTypeString(ogc::draw::DeviceType::kPdf), "Pdf");
    EXPECT_EQ(ogc::draw::GetDeviceTypeString(ogc::draw::DeviceType::kTile), "Tile");
    EXPECT_EQ(ogc::draw::GetDeviceTypeString(ogc::draw::DeviceType::kDisplay), "Display");
    EXPECT_EQ(ogc::draw::GetDeviceTypeString(ogc::draw::DeviceType::kSvg), "Svg");
    EXPECT_EQ(ogc::draw::GetDeviceTypeString(ogc::draw::DeviceType::kPrinter), "Printer");
}

TEST(DeviceTypeTest, GetDeviceTypeDescription) {
    EXPECT_FALSE(ogc::draw::GetDeviceTypeDescription(ogc::draw::DeviceType::kRasterImage).empty());
    EXPECT_FALSE(ogc::draw::GetDeviceTypeDescription(ogc::draw::DeviceType::kPdf).empty());
}
