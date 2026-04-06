#include <gtest/gtest.h>
#include "ogc/draw/simple2d_engine.h"
#include "ogc/draw/raster_image_device.h"
#include "ogc/draw/draw_style.h"
#include "ogc/draw/draw_result.h"
#include "ogc/draw/geometry.h"
#include "ogc/draw/image.h"
#include "ogc/draw/font.h"
#include <memory>
#include <vector>
#include <cstring>

using namespace ogc::draw;

class ExceptionPathTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_device = new RasterImageDevice(800, 600, PixelFormat::kRGBA8888);
        m_engine = new Simple2DEngine(m_device);
    }

    void TearDown() override {
        delete m_engine;
        delete m_device;
    }

    RasterImageDevice* m_device;
    Simple2DEngine* m_engine;
};

TEST_F(ExceptionPathTest, DrawPoints_NullPointer) {
    m_engine->Begin();
    
    DrawStyle style;
    style.pen.color = Color::Red();
    
    DrawResult result = m_engine->DrawPoints(nullptr, nullptr, 0, style);
    EXPECT_EQ(result, DrawResult::kInvalidParameter);
    
    m_engine->End();
}

TEST_F(ExceptionPathTest, DrawPoints_CountMismatch) {
    m_engine->Begin();
    
    double x[] = { 100.0, 200.0 };
    double y[] = { 100.0 };
    DrawStyle style;
    
    DrawResult result = m_engine->DrawPoints(x, y, 1, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_engine->End();
}

TEST_F(ExceptionPathTest, DrawLines_NullPointer) {
    m_engine->Begin();
    
    DrawStyle style;
    style.pen.color = Color::Blue();
    
    DrawResult result = m_engine->DrawLines(nullptr, nullptr, nullptr, nullptr, 0, style);
    EXPECT_EQ(result, DrawResult::kInvalidParameter);
    
    m_engine->End();
}

TEST_F(ExceptionPathTest, DrawLines_NegativeCount) {
    m_engine->Begin();
    
    double x1[] = { 0.0 };
    double y1[] = { 0.0 };
    double x2[] = { 100.0 };
    double y2[] = { 100.0 };
    DrawStyle style;
    
    DrawResult result = m_engine->DrawLines(x1, y1, x2, y2, -1, style);
    EXPECT_EQ(result, DrawResult::kInvalidParameter);
    
    m_engine->End();
}

TEST_F(ExceptionPathTest, DrawPolygon_NullPointer) {
    m_engine->Begin();
    
    DrawStyle style;
    style.pen.color = Color::Green();
    style.brush.color = Color::Green();
    
    DrawResult result = m_engine->DrawPolygon(nullptr, nullptr, 0, style, true);
    EXPECT_EQ(result, DrawResult::kInvalidParameter);
    
    m_engine->End();
}

TEST_F(ExceptionPathTest, DrawPolygon_InsufficientPoints) {
    m_engine->Begin();
    
    double x[] = { 0.0, 100.0 };
    double y[] = { 0.0, 100.0 };
    DrawStyle style;
    
    DrawResult result = m_engine->DrawPolygon(x, y, 2, style, true);
    EXPECT_EQ(result, DrawResult::kInvalidParameter);
    
    m_engine->End();
}

TEST_F(ExceptionPathTest, DrawCircle_NegativeRadius) {
    m_engine->Begin();
    
    DrawStyle style;
    style.pen.color = Color::Red();
    
    DrawResult result = m_engine->DrawCircle(400.0, 300.0, -10.0, style, true);
    EXPECT_EQ(result, DrawResult::kInvalidParameter);
    
    m_engine->End();
}

TEST_F(ExceptionPathTest, DrawCircle_ZeroRadius) {
    m_engine->Begin();
    
    DrawStyle style;
    style.pen.color = Color::Red();
    
    DrawResult result = m_engine->DrawCircle(400.0, 300.0, 0.0, style, true);
    EXPECT_EQ(result, DrawResult::kInvalidParameter);
    
    m_engine->End();
}

TEST_F(ExceptionPathTest, DrawEllipse_NegativeRadius) {
    m_engine->Begin();
    
    DrawStyle style;
    style.pen.color = Color::Blue();
    
    DrawResult result = m_engine->DrawEllipse(400.0, 300.0, -50.0, 50.0, style, true);
    EXPECT_EQ(result, DrawResult::kInvalidParameter);
    
    m_engine->End();
}

TEST_F(ExceptionPathTest, DrawRect_NegativeSize) {
    m_engine->Begin();
    
    DrawStyle style;
    style.pen.color = Color::Green();
    style.brush.color = Color::Green();
    
    DrawResult result = m_engine->DrawRect(100.0, 100.0, -50.0, -50.0, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_engine->End();
}

TEST_F(ExceptionPathTest, DrawText_EmptyString) {
    m_engine->Begin();
    
    Font font;
    DrawResult result = m_engine->DrawText(100.0, 100.0, "", font, Color::Black());
    EXPECT_EQ(result, DrawResult::kNotImplemented);
    
    m_engine->End();
}

TEST_F(ExceptionPathTest, DrawText_InvalidFont) {
    m_engine->Begin();
    
    Font font;
    font.SetFamily("");
    font.SetSize(0);
    
    DrawResult result = m_engine->DrawText(100.0, 100.0, "Test", font, Color::Black());
    EXPECT_EQ(result, DrawResult::kNotImplemented);
    
    m_engine->End();
}

TEST_F(ExceptionPathTest, DrawImage_NullImage) {
    m_engine->Begin();
    
    Image img;
    DrawResult result = m_engine->DrawImage(100.0, 100.0, img, 1.0, 1.0);
    EXPECT_EQ(result, DrawResult::kInvalidParameter);
    
    m_engine->End();
}

TEST_F(ExceptionPathTest, DrawImage_InvalidScale) {
    m_engine->Begin();
    
    Image img(100, 100, 4);
    
    DrawResult result = m_engine->DrawImage(100.0, 100.0, img, 0.0, 0.0);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_engine->End();
}

TEST_F(ExceptionPathTest, DrawGeometry_NullGeometry) {
    m_engine->Begin();
    
    DrawStyle style;
    DrawResult result = m_engine->DrawGeometry(nullptr, style);
    EXPECT_EQ(result, DrawResult::kInvalidParameter);
    
    m_engine->End();
}

TEST_F(ExceptionPathTest, DrawGeometry_EmptyGeometry) {
    m_engine->Begin();
    
    auto geom = RectGeometry::Create();
    DrawStyle style;
    
    DrawResult result = m_engine->DrawGeometry(geom.get(), style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_engine->End();
}

TEST_F(ExceptionPathTest, SetOpacity_OutOfRange) {
    m_engine->Begin();
    
    m_engine->SetOpacity(-0.5);
    double opacity = m_engine->GetOpacity();
    EXPECT_GE(opacity, 0.0);
    EXPECT_LE(opacity, 1.0);
    
    m_engine->SetOpacity(1.5);
    opacity = m_engine->GetOpacity();
    EXPECT_GE(opacity, 0.0);
    EXPECT_LE(opacity, 1.0);
    
    m_engine->End();
}

TEST_F(ExceptionPathTest, SetTolerance_Negative) {
    m_engine->Begin();
    
    m_engine->SetTolerance(-0.01);
    double tolerance = m_engine->GetTolerance();
    EXPECT_LT(tolerance, 0.0);
    
    m_engine->End();
}

TEST_F(ExceptionPathTest, DrawWithoutBegin) {
    DrawStyle style;
    style.pen.color = Color::Red();
    
    double x[] = { 100.0 };
    double y[] = { 100.0 };
    
    DrawResult result = m_engine->DrawPoints(x, y, 1, style);
    EXPECT_NE(result, DrawResult::kSuccess);
}

TEST_F(ExceptionPathTest, MultipleBeginCalls) {
    DrawResult result1 = m_engine->Begin();
    EXPECT_EQ(result1, DrawResult::kSuccess);
    
    DrawResult result2 = m_engine->Begin();
    EXPECT_EQ(result2, DrawResult::kSuccess);
    
    m_engine->End();
}

TEST_F(ExceptionPathTest, EndWithoutBegin) {
    m_engine->End();
    EXPECT_FALSE(m_engine->IsActive());
}

TEST_F(ExceptionPathTest, MultipleEndCalls) {
    m_engine->Begin();
    m_engine->End();
    m_engine->End();
    EXPECT_FALSE(m_engine->IsActive());
}

TEST_F(ExceptionPathTest, SaveRestoreMismatch) {
    m_engine->Begin();
    
    m_engine->Save();
    m_engine->Restore();
    
    m_engine->Restore();
    
    m_engine->End();
}

TEST_F(ExceptionPathTest, SetClipRect_ZeroSize) {
    m_engine->Begin();
    
    m_engine->SetClipRect(100.0, 100.0, 0.0, 0.0);
    
    DrawStyle style;
    style.pen.color = Color::Red();
    double x[] = { 50.0 };
    double y[] = { 50.0 };
    DrawResult result = m_engine->DrawPoints(x, y, 1, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_engine->End();
}

TEST_F(ExceptionPathTest, SetClipRect_NegativeSize) {
    m_engine->Begin();
    
    m_engine->SetClipRect(100.0, 100.0, -50.0, -50.0);
    
    DrawStyle style;
    style.pen.color = Color::Red();
    double x[] = { 50.0 };
    double y[] = { 50.0 };
    DrawResult result = m_engine->DrawPoints(x, y, 1, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_engine->End();
}

TEST_F(ExceptionPathTest, DrawLineString_NullPointer) {
    m_engine->Begin();
    
    DrawStyle style;
    DrawResult result = m_engine->DrawLineString(nullptr, nullptr, 0, style);
    EXPECT_EQ(result, DrawResult::kInvalidParameter);
    
    m_engine->End();
}

TEST_F(ExceptionPathTest, DrawLineString_InsufficientPoints) {
    m_engine->Begin();
    
    double x[] = { 0.0 };
    double y[] = { 0.0 };
    DrawStyle style;
    
    DrawResult result = m_engine->DrawLineString(x, y, 1, style);
    EXPECT_EQ(result, DrawResult::kInvalidParameter);
    
    m_engine->End();
}

TEST_F(ExceptionPathTest, DrawArc_NegativeRadius) {
    m_engine->Begin();
    
    DrawStyle style;
    style.pen.color = Color::Blue();
    
    DrawResult result = m_engine->DrawArc(400.0, 300.0, -50.0, 50.0, 0.0, 90.0, style);
    EXPECT_EQ(result, DrawResult::kInvalidParameter);
    
    m_engine->End();
}

TEST_F(ExceptionPathTest, DrawImageRect_NullImage) {
    m_engine->Begin();
    
    Image img;
    DrawResult result = m_engine->DrawImageRect(100.0, 100.0, 50.0, 50.0, img);
    EXPECT_EQ(result, DrawResult::kInvalidParameter);
    
    m_engine->End();
}

TEST_F(ExceptionPathTest, DrawImageRect_ZeroSize) {
    m_engine->Begin();
    
    Image img(100, 100, 4);
    
    DrawResult result = m_engine->DrawImageRect(100.0, 100.0, 0.0, 0.0, img);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_engine->End();
}

TEST_F(ExceptionPathTest, TransformMatrix_Singular) {
    m_engine->Begin();
    
    TransformMatrix matrix;
    matrix.m[0][0] = 0;
    matrix.m[0][1] = 0;
    matrix.m[1][0] = 0;
    matrix.m[1][1] = 0;
    matrix.m[0][2] = 0;
    matrix.m[1][2] = 0;
    
    m_engine->SetTransform(matrix);
    
    DrawStyle style;
    style.pen.color = Color::Red();
    double x[] = { 100.0 };
    double y[] = { 100.0 };
    DrawResult result = m_engine->DrawPoints(x, y, 1, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_engine->End();
}

TEST_F(ExceptionPathTest, DrawResultToString_AllValues) {
    EXPECT_STREQ(DrawResultToString(DrawResult::kSuccess), "Success");
    EXPECT_STREQ(DrawResultToString(DrawResult::kFailed), "Failed");
    EXPECT_STREQ(DrawResultToString(DrawResult::kInvalidParameter), "Invalid Parameter");
    EXPECT_STREQ(DrawResultToString(DrawResult::kOutOfMemory), "Out Of Memory");
    EXPECT_STREQ(DrawResultToString(DrawResult::kDeviceError), "Device Error");
    EXPECT_STREQ(DrawResultToString(DrawResult::kEngineError), "Engine Error");
    EXPECT_STREQ(DrawResultToString(DrawResult::kNotImplemented), "Not Implemented");
    EXPECT_STREQ(DrawResultToString(DrawResult::kCancelled), "Cancelled");
    EXPECT_STREQ(DrawResultToString(DrawResult::kTimeout), "Timeout");
}

TEST_F(ExceptionPathTest, IsSuccess_IsError) {
    EXPECT_TRUE(IsSuccess(DrawResult::kSuccess));
    EXPECT_FALSE(IsSuccess(DrawResult::kFailed));
    
    EXPECT_FALSE(IsError(DrawResult::kSuccess));
    EXPECT_TRUE(IsError(DrawResult::kFailed));
    EXPECT_TRUE(IsError(DrawResult::kInvalidParameter));
    EXPECT_TRUE(IsError(DrawResult::kOutOfMemory));
}

TEST_F(ExceptionPathTest, DrawError_FromResult) {
    DrawError error = DrawError::FromResult(DrawResult::kInvalidParameter, "Test context");
    
    EXPECT_EQ(error.code, DrawResult::kInvalidParameter);
    EXPECT_EQ(error.context, "Test context");
    EXPECT_TRUE(error.IsError());
    EXPECT_FALSE(error.IsSuccess());
}

TEST_F(ExceptionPathTest, DrawError_ToString) {
    DrawError error(DrawResult::kFailed, "Test message", "Test context", "test.cpp", 10);
    std::string str = error.ToString();
    
    EXPECT_FALSE(str.empty());
    EXPECT_TRUE(str.find("Failed") != std::string::npos);
}

TEST_F(ExceptionPathTest, DrawStyle_InvalidPenWidth) {
    DrawStyle style;
    style.pen.width = -5.0;
    style.pen.color = Color::Red();
    
    m_engine->Begin();
    
    double x[] = { 100.0 };
    double y[] = { 100.0 };
    DrawResult result = m_engine->DrawPoints(x, y, 1, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_engine->End();
}

TEST_F(ExceptionPathTest, DrawStyle_TransparentColors) {
    DrawStyle style;
    style.pen.color = Color(255, 0, 0, 0);
    style.brush.color = Color(0, 255, 0, 0);
    
    m_engine->Begin();
    
    double x[] = { 100.0, 200.0, 150.0 };
    double y[] = { 100.0, 100.0, 200.0 };
    DrawResult result = m_engine->DrawPolygon(x, y, 3, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_engine->End();
}

TEST_F(ExceptionPathTest, DrawPoints_ExtremeCoordinates) {
    m_engine->Begin();
    
    double x[] = { 
        std::numeric_limits<double>::max(),
        std::numeric_limits<double>::lowest(),
        std::numeric_limits<double>::infinity(),
        -std::numeric_limits<double>::infinity()
    };
    double y[] = { 300.0, 300.0, 300.0, 300.0 };
    DrawStyle style;
    style.pen.color = Color::Red();
    
    DrawResult result = m_engine->DrawPoints(x, y, 4, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_engine->End();
}

TEST_F(ExceptionPathTest, DrawPoints_NaNCoordinates) {
    m_engine->Begin();
    
    double x[] = { std::numeric_limits<double>::quiet_NaN() };
    double y[] = { std::numeric_limits<double>::quiet_NaN() };
    DrawStyle style;
    style.pen.color = Color::Red();
    
    DrawResult result = m_engine->DrawPoints(x, y, 1, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_engine->End();
}

TEST_F(ExceptionPathTest, DrawText_SpecialCharacters) {
    m_engine->Begin();
    
    Font font("Arial", 12);
    
    DrawResult result = m_engine->DrawText(100.0, 100.0, "Test\nNewline\tTab", font, Color::Black());
    EXPECT_EQ(result, DrawResult::kNotImplemented);
    
    result = m_engine->DrawText(100.0, 150.0, "Unicode: \u4e2d\u6587", font, Color::Black());
    EXPECT_EQ(result, DrawResult::kNotImplemented);
    
    m_engine->End();
}

TEST_F(ExceptionPathTest, DrawText_VeryLongString) {
    m_engine->Begin();
    
    Font font("Arial", 12);
    
    std::string longText(10000, 'A');
    DrawResult result = m_engine->DrawText(100.0, 100.0, longText, font, Color::Black());
    EXPECT_EQ(result, DrawResult::kNotImplemented);
    
    m_engine->End();
}

TEST_F(ExceptionPathTest, DrawPolygon_SelfIntersecting) {
    m_engine->Begin();
    
    double x[] = { 0.0, 200.0, 0.0, 200.0 };
    double y[] = { 0.0, 200.0, 200.0, 0.0 };
    DrawStyle style;
    style.pen.color = Color::Red();
    style.brush.color = Color::Green();
    
    DrawResult result = m_engine->DrawPolygon(x, y, 4, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_engine->End();
}

TEST_F(ExceptionPathTest, DrawPolygon_CollinearPoints) {
    m_engine->Begin();
    
    double x[] = { 0.0, 100.0, 200.0, 300.0 };
    double y[] = { 100.0, 100.0, 100.0, 100.0 };
    DrawStyle style;
    style.pen.color = Color::Red();
    
    DrawResult result = m_engine->DrawPolygon(x, y, 4, style, false);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_engine->End();
}

TEST_F(ExceptionPathTest, Device_ZeroSize) {
    delete m_engine;
    delete m_device;
    
    m_device = new RasterImageDevice(0, 0, PixelFormat::kRGBA8888);
    m_engine = new Simple2DEngine(m_device);
    
    DrawResult result = m_engine->Begin();
    EXPECT_EQ(result, DrawResult::kDeviceError);
    
    m_engine->End();
}

TEST_F(ExceptionPathTest, Device_LargeSize) {
    delete m_engine;
    delete m_device;
    
    m_device = new RasterImageDevice(10000, 10000, PixelFormat::kRGBA8888);
    m_engine = new Simple2DEngine(m_device);
    
    DrawResult result = m_engine->Begin();
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_engine->End();
}

TEST_F(ExceptionPathTest, Image_InvalidOperations) {
    Image emptyImg;
    EXPECT_FALSE(emptyImg.IsValid());
    EXPECT_TRUE(emptyImg.IsEmpty());
    
    Image validImg(100, 100, 4);
    EXPECT_TRUE(validImg.IsValid());
    EXPECT_FALSE(validImg.IsEmpty());
    EXPECT_EQ(validImg.GetWidth(), 100);
    EXPECT_EQ(validImg.GetHeight(), 100);
    EXPECT_EQ(validImg.GetChannels(), 4);
}

TEST_F(ExceptionPathTest, Font_InvalidOperations) {
    Font emptyFont;
    EXPECT_TRUE(emptyFont.IsValid());
    EXPECT_FALSE(emptyFont.IsEmpty());
    
    Font validFont("Arial", 12);
    EXPECT_TRUE(validFont.IsValid());
    EXPECT_FALSE(validFont.IsEmpty());
    EXPECT_EQ(validFont.GetFamily(), "Arial");
    EXPECT_DOUBLE_EQ(validFont.GetSize(), 12.0);
}

TEST_F(ExceptionPathTest, Geometry_InvalidOperations) {
    auto emptyRect = RectGeometry::Create();
    EXPECT_NE(emptyRect.get(), nullptr);
    
    auto emptyCircle = CircleGeometry::Create();
    EXPECT_NE(emptyCircle.get(), nullptr);
    
    auto emptyEllipse = EllipseGeometry::Create();
    EXPECT_NE(emptyEllipse.get(), nullptr);
}

TEST_F(ExceptionPathTest, DrawGeometry_CircleGeometry) {
    m_engine->Begin();
    
    auto circle = CircleGeometry::Create(400.0, 300.0, 50.0);
    DrawStyle style;
    style.pen.color = Color::Red();
    style.brush.color = Color::Green();
    
    DrawResult result = m_engine->DrawGeometry(circle.get(), style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_engine->End();
}

TEST_F(ExceptionPathTest, DrawGeometry_EllipseGeometry) {
    m_engine->Begin();
    
    auto ellipse = EllipseGeometry::Create(400.0, 300.0, 100.0, 50.0);
    DrawStyle style;
    style.pen.color = Color::Blue();
    style.brush.color = Color::Yellow();
    
    DrawResult result = m_engine->DrawGeometry(ellipse.get(), style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_engine->End();
}

TEST_F(ExceptionPathTest, DrawGeometry_RectGeometry) {
    m_engine->Begin();
    
    auto rect = RectGeometry::Create(100.0, 100.0, 200.0, 150.0);
    DrawStyle style;
    style.pen.color = Color::Green();
    style.brush.color = Color::Cyan();
    
    DrawResult result = m_engine->DrawGeometry(rect.get(), style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_engine->End();
}

TEST_F(ExceptionPathTest, TransformMatrix_InvalidOperations) {
    TransformMatrix identity = TransformMatrix::Identity();
    EXPECT_TRUE(identity.IsIdentity());
    
    TransformMatrix translate = TransformMatrix::Translate(100, 200);
    EXPECT_FALSE(translate.IsIdentity());
    
    TransformMatrix scale = TransformMatrix::Scale(2.0, 3.0);
    EXPECT_FALSE(scale.IsIdentity());
    
    TransformMatrix rotate = TransformMatrix::Rotate(45.0);
    EXPECT_FALSE(rotate.IsIdentity());
}

TEST_F(ExceptionPathTest, Color_InvalidValues) {
    Color invalid(0, 0, 0, 0);
    EXPECT_EQ(invalid.GetRed(), 0);
    EXPECT_EQ(invalid.GetGreen(), 0);
    EXPECT_EQ(invalid.GetBlue(), 0);
    EXPECT_EQ(invalid.GetAlpha(), 0);
    
    Color overflow(255, 255, 255, 255);
    EXPECT_EQ(overflow.GetRed(), 255);
    EXPECT_EQ(overflow.GetGreen(), 255);
    EXPECT_EQ(overflow.GetBlue(), 255);
    EXPECT_EQ(overflow.GetAlpha(), 255);
}

TEST_F(ExceptionPathTest, DrawStyle_InvalidValues) {
    DrawStyle style;
    
    style.pen.width = -100.0;
    style.pen.color = Color(0, 0, 0, 0);
    
    style.brush.color = Color(255, 255, 255, 255);
    
    m_engine->Begin();
    
    double x[] = { 100.0, 200.0, 150.0 };
    double y[] = { 100.0, 100.0, 200.0 };
    DrawResult result = m_engine->DrawPolygon(x, y, 3, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_engine->End();
}
