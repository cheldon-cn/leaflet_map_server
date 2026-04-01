#include <gtest/gtest.h>
#include <ogc/draw/raster_image_device.h>
#include <ogc/draw/draw_style.h>
#include <ogc/draw/color.h>
#include "ogc/envelope.h"

#include <chrono>
#include <memory>

using namespace ogc::draw;
using ogc::Envelope;

class GraphPerformanceTest : public ::testing::Test {
protected:
    void SetUp() override {
        device = std::make_unique<RasterImageDevice>(256, 256, PixelFormat::kRGBA8888);
        ASSERT_NE(device, nullptr);
        device->Initialize();
    }
    
    void TearDown() override {
        device.reset();
    }
    
    std::unique_ptr<RasterImageDevice> device;
};

TEST_F(GraphPerformanceTest, DeviceCreation_ResponseTime) {
    const int iterations = 100;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        auto testDevice = std::make_unique<RasterImageDevice>(256, 256, PixelFormat::kRGBA8888);
        testDevice->Initialize();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double avg_time_us = static_cast<double>(duration.count()) / iterations;
    EXPECT_LT(avg_time_us, 10000.0) << "Average device creation time should be < 10ms";
}

TEST_F(GraphPerformanceTest, ColorCreation_ResponseTime) {
    const int iterations = 10000;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        Color color(i % 256, (i / 256) % 256, (i / 65536) % 256);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double avg_time_us = static_cast<double>(duration.count()) / iterations;
    EXPECT_LT(avg_time_us, 1.0) << "Average color creation time should be < 1us";
}

TEST_F(GraphPerformanceTest, DrawStyleCreation_ResponseTime) {
    const int iterations = 10000;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        DrawStyle style;
        style.pen.color = Color::Red();
        style.pen.width = 1.0;
        style.brush.color = Color::Blue();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double avg_time_us = static_cast<double>(duration.count()) / iterations;
    EXPECT_LT(avg_time_us, 5.0) << "Average style creation time should be < 5us";
}

TEST_F(GraphPerformanceTest, GetPixel_ResponseTime) {
    const int iterations = 10000;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        Color pixel = device->GetPixel(i % 256, (i / 256) % 256);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double avg_time_us = static_cast<double>(duration.count()) / iterations;
    EXPECT_LT(avg_time_us, 10.0) << "Average GetPixel time should be < 10us";
}

TEST_F(GraphPerformanceTest, EnvelopeCreation_ResponseTime) {
    const int iterations = 10000;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        Envelope env(i, i, i + 100, i + 100);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double avg_time_us = static_cast<double>(duration.count()) / iterations;
    EXPECT_LT(avg_time_us, 1.0) << "Average envelope creation time should be < 1us";
}
