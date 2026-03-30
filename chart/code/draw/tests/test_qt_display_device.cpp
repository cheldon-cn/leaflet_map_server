#include <gtest/gtest.h>
#include "ogc/draw/qt_display_device.h"
#include "ogc/draw/qt_engine.h"

#ifdef DRAW_HAS_QT

#include <QImage>
#include <QPixmap>
#include <QWidget>
#include <QTemporaryFile>

using namespace ogc::draw;

class QtDisplayDeviceTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(QtDisplayDeviceTest, CreateFromSize) {
    QtDisplayDevice device(200, 100);
    
    EXPECT_EQ(device.GetWidth(), 200);
    EXPECT_EQ(device.GetHeight(), 100);
    EXPECT_EQ(device.GetType(), DeviceType::kDisplay);
    EXPECT_TRUE(device.IsValid());
}

TEST_F(QtDisplayDeviceTest, CreateFromQSize) {
    QSize size(300, 200);
    QtDisplayDevice device(size);
    
    EXPECT_EQ(device.GetWidth(), 300);
    EXPECT_EQ(device.GetHeight(), 200);
    EXPECT_TRUE(device.IsValid());
}

TEST_F(QtDisplayDeviceTest, GetSize) {
    QtDisplayDevice device(150, 75);
    
    QSize size = device.GetSize();
    EXPECT_EQ(size.width(), 150);
    EXPECT_EQ(size.height(), 75);
}

TEST_F(QtDisplayDeviceTest, GetDpi) {
    QtDisplayDevice device(100, 100);
    
    double dpi = device.GetDpi();
    EXPECT_GT(dpi, 0);
}

TEST_F(QtDisplayDeviceTest, SetDpi) {
    QtDisplayDevice device(100, 100);
    
    device.SetDpi(150.0);
    EXPECT_DOUBLE_EQ(device.GetDpi(), 150.0);
}

TEST_F(QtDisplayDeviceTest, CreateEngine) {
    QtDisplayDevice device(100, 100);
    
    auto engine = device.CreateEngine();
    ASSERT_NE(engine, nullptr);
    EXPECT_EQ(engine->GetName(), "QtEngine");
}

TEST_F(QtDisplayDeviceTest, GetSupportedEngineTypes) {
    QtDisplayDevice device(100, 100);
    
    auto types = device.GetSupportedEngineTypes();
    ASSERT_EQ(types.size(), 1u);
    EXPECT_EQ(types[0], EngineType::kSimple2D);
}

TEST_F(QtDisplayDeviceTest, GetPreferredEngineType) {
    QtDisplayDevice device(100, 100);
    
    EXPECT_EQ(device.GetPreferredEngineType(), EngineType::kSimple2D);
}

TEST_F(QtDisplayDeviceTest, GetPaintDevice) {
    QtDisplayDevice device(100, 100);
    
    QPaintDevice* paintDevice = device.GetPaintDevice();
    ASSERT_NE(paintDevice, nullptr);
    EXPECT_EQ(paintDevice->width(), 100);
    EXPECT_EQ(paintDevice->height(), 100);
}

TEST_F(QtDisplayDeviceTest, GetImage) {
    QtDisplayDevice device(100, 100);
    
    QImage* image = device.GetImage();
    ASSERT_NE(image, nullptr);
    EXPECT_FALSE(image->isNull());
}

TEST_F(QtDisplayDeviceTest, GetPixmap) {
    QtDisplayDevice device(100, 100);
    
    QPixmap* pixmap = device.GetPixmap();
    EXPECT_EQ(pixmap, nullptr);
}

TEST_F(QtDisplayDeviceTest, GetWidget) {
    QtDisplayDevice device(100, 100);
    
    QWidget* widget = device.GetWidget();
    EXPECT_EQ(widget, nullptr);
}

TEST_F(QtDisplayDeviceTest, SaveToFilePng) {
    QtDisplayDevice device(100, 100);
    
    QString tempPath = QDir::tempPath() + "/test_qt_display_device_XXXXXX.png";
    QTemporaryFile tempFile(tempPath);
    tempFile.setAutoRemove(true);
    
    if (tempFile.open()) {
        std::string path = tempFile.fileName().toStdString();
        EXPECT_TRUE(device.SaveToFile(path, "PNG"));
    }
}

TEST_F(QtDisplayDeviceTest, SaveToFileJpeg) {
    QtDisplayDevice device(100, 100);
    
    QString tempPath = QDir::tempPath() + "/test_qt_display_device_XXXXXX.jpg";
    QTemporaryFile tempFile(tempPath);
    tempFile.setAutoRemove(true);
    
    if (tempFile.open()) {
        std::string path = tempFile.fileName().toStdString();
        EXPECT_TRUE(device.SaveToFile(path, "JPEG", 90));
    }
}

TEST_F(QtDisplayDeviceTest, FromImage) {
    QImage image(100, 100, QImage::Format_ARGB32);
    image.fill(Qt::white);
    
    QtDisplayDevice* device = QtDisplayDevice::FromImage(&image);
    ASSERT_NE(device, nullptr);
    EXPECT_EQ(device->GetWidth(), 100);
    EXPECT_EQ(device->GetHeight(), 100);
    
    delete device;
}

TEST_F(QtDisplayDeviceTest, FromImageNull) {
    QtDisplayDevice* device = QtDisplayDevice::FromImage(nullptr);
    EXPECT_EQ(device, nullptr);
}

TEST_F(QtDisplayDeviceTest, FromPixmap) {
    QPixmap pixmap(100, 100);
    pixmap.fill(Qt::white);
    
    QtDisplayDevice* device = QtDisplayDevice::FromPixmap(&pixmap);
    ASSERT_NE(device, nullptr);
    EXPECT_EQ(device->GetWidth(), 100);
    EXPECT_EQ(device->GetHeight(), 100);
    
    delete device;
}

TEST_F(QtDisplayDeviceTest, FromPixmapNull) {
    QtDisplayDevice* device = QtDisplayDevice::FromPixmap(nullptr);
    EXPECT_EQ(device, nullptr);
}

TEST_F(QtDisplayDeviceTest, FromWidgetNull) {
    QtDisplayDevice* device = QtDisplayDevice::FromWidget(nullptr);
    EXPECT_EQ(device, nullptr);
}

TEST_F(QtDisplayDeviceTest, DifferentFormats) {
    QtDisplayDevice deviceARGB32(100, 100, QImage::Format_ARGB32);
    EXPECT_TRUE(deviceARGB32.IsValid());
    
    QtDisplayDevice deviceRGB32(100, 100, QImage::Format_RGB32);
    EXPECT_TRUE(deviceRGB32.IsValid());
    
    QtDisplayDevice deviceRGB888(100, 100, QImage::Format_RGB888);
    EXPECT_TRUE(deviceRGB888.IsValid());
}

TEST_F(QtDisplayDeviceTest, ZeroSize) {
    QtDisplayDevice device(0, 0);
    EXPECT_FALSE(device.IsValid());
}

TEST_F(QtDisplayDeviceTest, NegativeSize) {
    QtDisplayDevice device(-100, -100);
    EXPECT_FALSE(device.IsValid());
}

TEST_F(QtDisplayDeviceTest, LargeSize) {
    QtDisplayDevice device(10000, 10000);
    EXPECT_TRUE(device.IsValid());
    EXPECT_EQ(device.GetWidth(), 10000);
    EXPECT_EQ(device.GetHeight(), 10000);
}

TEST_F(QtDisplayDeviceTest, RenderAndSave) {
    QtDisplayDevice device(200, 200);
    
    auto engine = device.CreateEngine();
    ASSERT_NE(engine, nullptr);
    
    ASSERT_TRUE(engine->Begin());
    
    double x[] = {50, 150, 100};
    double y[] = {50, 50, 150};
    
    DrawStyle style;
    style.fillColor = Color(1.0, 0.0, 0.0, 1.0);
    style.lineColor = Color(0.0, 0.0, 0.0, 1.0);
    style.lineWidth = 2.0;
    style.fill = true;
    
    auto result = engine->DrawPolygon(x, y, 3, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->End();
    
    QString tempPath = QDir::tempPath() + "/test_qt_display_device_render_XXXXXX.png";
    QTemporaryFile tempFile(tempPath);
    tempFile.setAutoRemove(true);
    
    if (tempFile.open()) {
        std::string path = tempFile.fileName().toStdString();
        EXPECT_TRUE(device.SaveToFile(path, "PNG"));
    }
}

TEST_F(QtDisplayDeviceTest, MultipleEngines) {
    QtDisplayDevice device(100, 100);
    
    auto engine1 = device.CreateEngine();
    auto engine2 = device.CreateEngine();
    
    ASSERT_NE(engine1, nullptr);
    ASSERT_NE(engine2, nullptr);
    EXPECT_NE(engine1.get(), engine2.get());
}

#else

TEST(QtDisplayDeviceNotAvailable, Skip) {
    GTEST_SKIP() << "Qt not available, skipping QtDisplayDevice tests";
}

#endif
