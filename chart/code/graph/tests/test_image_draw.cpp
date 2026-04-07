#include <gtest/gtest.h>
#include <ogc/graph/util/image_draw.h>
#include <ogc/draw/color.h>
#include <vector>
#include <memory>

using namespace ogc::graph;
using ogc::draw::Color;

class ImageDrawTest : public ::testing::Test {
protected:
    void SetUp() override {
        imageData = ImageDataPtr(new ImageData(100, 100, 4));
        imageDraw = std::unique_ptr<ImageDraw>(new ImageDraw(imageData.get()));
    }
    
    void TearDown() override {
        imageDraw.reset();
        imageData.reset();
    }
    
    ImageDataPtr imageData;
    std::unique_ptr<ImageDraw> imageDraw;
};

TEST_F(ImageDrawTest, Constructor) {
    std::unique_ptr<ImageDraw> draw(new ImageDraw());
    EXPECT_EQ(draw->GetImageData(), nullptr);
}

TEST_F(ImageDrawTest, ConstructorWithImageData) {
    std::unique_ptr<ImageDraw> draw(new ImageDraw(imageData.get()));
    EXPECT_EQ(draw->GetImageData(), imageData.get());
}

TEST_F(ImageDrawTest, SetImageData) {
    ImageDataPtr newImage(new ImageData(50, 50, 4));
    imageDraw->SetImageData(newImage.get());
    EXPECT_EQ(imageDraw->GetImageData(), newImage.get());
}

TEST_F(ImageDrawTest, SetAndGetPixel) {
    Color color(255, 0, 0, 255);
    imageDraw->SetPixel(10, 10, color);
    
    Color retrieved = imageDraw->GetPixel(10, 10);
    EXPECT_EQ(retrieved.GetRed(), 255);
    EXPECT_EQ(retrieved.GetGreen(), 0);
    EXPECT_EQ(retrieved.GetBlue(), 0);
    EXPECT_EQ(retrieved.GetAlpha(), 255);
}

TEST_F(ImageDrawTest, DrawLine) {
    Color color(0, 255, 0, 255);
    EXPECT_NO_THROW(imageDraw->DrawLine(0, 0, 50, 50, color, 1));
}

TEST_F(ImageDrawTest, DrawLineAA) {
    Color color(0, 0, 255, 255);
    EXPECT_NO_THROW(imageDraw->DrawLineAA(0, 0, 50, 50, color, 1));
}

TEST_F(ImageDrawTest, DrawRect) {
    Color color(255, 255, 0, 255);
    EXPECT_NO_THROW(imageDraw->DrawRect(10, 10, 30, 30, color, 1));
}

TEST_F(ImageDrawTest, FillRect) {
    Color color(255, 0, 255, 255);
    EXPECT_NO_THROW(imageDraw->FillRect(10, 10, 30, 30, color));
}

TEST_F(ImageDrawTest, DrawCircle) {
    Color color(0, 255, 255, 255);
    EXPECT_NO_THROW(imageDraw->DrawCircle(50, 50, 20, color, 1));
}

TEST_F(ImageDrawTest, FillCircle) {
    Color color(128, 128, 128, 255);
    EXPECT_NO_THROW(imageDraw->FillCircle(50, 50, 20, color));
}

TEST_F(ImageDrawTest, DrawEllipse) {
    Color color(64, 64, 64, 255);
    EXPECT_NO_THROW(imageDraw->DrawEllipse(50, 50, 30, 20, color, 1));
}

TEST_F(ImageDrawTest, FillEllipse) {
    Color color(192, 192, 192, 255);
    EXPECT_NO_THROW(imageDraw->FillEllipse(50, 50, 30, 20, color));
}

TEST_F(ImageDrawTest, DrawPolygon) {
    Color color(100, 150, 200, 255);
    std::vector<std::pair<int, int>> points;
    points.push_back({10, 10});
    points.push_back({50, 10});
    points.push_back({30, 50});
    
    EXPECT_NO_THROW(imageDraw->DrawPolygon(points, color, 1));
}

TEST_F(ImageDrawTest, FillPolygon) {
    Color color(200, 150, 100, 255);
    std::vector<std::pair<int, int>> points;
    points.push_back({10, 10});
    points.push_back({50, 10});
    points.push_back({30, 50});
    
    EXPECT_NO_THROW(imageDraw->FillPolygon(points, color));
}

TEST_F(ImageDrawTest, DrawText) {
    Color color(0, 0, 0, 255);
    EXPECT_NO_THROW(imageDraw->DrawText(10, 10, "Test", color, "Arial", 12));
}

TEST_F(ImageDrawTest, DrawImage) {
    ImageDataPtr overlay(new ImageData(20, 20, 4));
    Color color(255, 255, 255, 255);
    for (int y = 0; y < 20; ++y) {
        for (int x = 0; x < 20; ++x) {
            overlay->SetPixel(x, y, color);
        }
    }
    
    EXPECT_NO_THROW(imageDraw->DrawImage(10, 10, overlay.get(), 1.0));
}

TEST_F(ImageDrawTest, SetClipRect) {
    EXPECT_NO_THROW(imageDraw->SetClipRect(10, 10, 50, 50));
    EXPECT_TRUE(imageDraw->HasClipRect());
}

TEST_F(ImageDrawTest, ClearClipRect) {
    imageDraw->SetClipRect(10, 10, 50, 50);
    EXPECT_TRUE(imageDraw->HasClipRect());
    
    EXPECT_NO_THROW(imageDraw->ClearClipRect());
    EXPECT_FALSE(imageDraw->HasClipRect());
}

TEST_F(ImageDrawTest, IsPointInClipRect) {
    imageDraw->SetClipRect(10, 10, 50, 50);
    
    EXPECT_TRUE(imageDraw->IsPointInClipRect(20, 20));
    EXPECT_FALSE(imageDraw->IsPointInClipRect(5, 5));
    EXPECT_FALSE(imageDraw->IsPointInClipRect(70, 70));
}

TEST_F(ImageDrawTest, ImageDataValid) {
    EXPECT_TRUE(imageData->IsValid());
    EXPECT_FALSE(imageData->IsEmpty());
}

TEST_F(ImageDrawTest, ImageDataSize) {
    EXPECT_EQ(imageData->GetWidth(), 100);
    EXPECT_EQ(imageData->GetHeight(), 100);
    EXPECT_EQ(imageData->GetChannels(), 4);
    EXPECT_EQ(imageData->GetSize(), 100 * 100 * 4);
}

TEST_F(ImageDrawTest, ImageDataFill) {
    Color fillColor(255, 128, 64, 255);
    EXPECT_NO_THROW(imageData->Fill(fillColor));
    
    Color pixel = imageData->GetPixel(50, 50);
    EXPECT_EQ(pixel.GetRed(), 255);
    EXPECT_EQ(pixel.GetGreen(), 128);
    EXPECT_EQ(pixel.GetBlue(), 64);
}

TEST_F(ImageDrawTest, ImageDataClear) {
    Color fillColor(255, 128, 64, 255);
    imageData->Fill(fillColor);
    
    EXPECT_NO_THROW(imageData->Clear());
}
