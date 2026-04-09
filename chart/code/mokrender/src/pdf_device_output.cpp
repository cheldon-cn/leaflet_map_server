#include "ogc/mokrender/pdf_device_output.h"
#include "ogc/draw/raster_image_device.h"
#include "ogc/draw/draw_style.h"
#include "ogc/draw/font.h"
#include "ogc/geom/envelope.h"
#include <fstream>
#include <vector>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef _WIN32
#include <direct.h>
#endif

namespace ogc {
namespace mokrender {

class PdfDeviceOutput::Impl {
public:
    int width, height;
    double dpi;
    bool initialized;
    
    Impl() : width(800), height(600), dpi(96.0), initialized(false) {}
};

PdfDeviceOutput::PdfDeviceOutput() : m_impl(new Impl()) {
}

PdfDeviceOutput::~PdfDeviceOutput() {
}

MokRenderResult PdfDeviceOutput::Initialize(const RenderConfig& config) {
    m_impl->width = config.outputWidth;
    m_impl->height = config.outputHeight;
    m_impl->dpi = config.dpi;
    m_impl->initialized = true;
    return MokRenderResult();
}

static bool SaveAsBMP(const std::string& filepath, int width, int height, const uint8_t* data, int channels) {
    int rowSize = width * 3;
    int padding = (4 - (rowSize % 4)) % 4;
    int paddedRowSize = rowSize + padding;
    int imageSize = paddedRowSize * height;
    
    int fileSize = 54 + imageSize;
    
    std::vector<uint8_t> header(54, 0);
    
    header[0] = 'B';
    header[1] = 'M';
    
    header[2] = fileSize & 0xFF;
    header[3] = (fileSize >> 8) & 0xFF;
    header[4] = (fileSize >> 16) & 0xFF;
    header[5] = (fileSize >> 24) & 0xFF;
    
    header[10] = 54;
    
    header[14] = 40;
    
    header[18] = width & 0xFF;
    header[19] = (width >> 8) & 0xFF;
    header[20] = (width >> 16) & 0xFF;
    header[21] = (width >> 24) & 0xFF;
    
    header[22] = height & 0xFF;
    header[23] = (height >> 8) & 0xFF;
    header[24] = (height >> 16) & 0xFF;
    header[25] = (height >> 24) & 0xFF;
    
    header[26] = 1;
    header[28] = 24;
    
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    file.write(reinterpret_cast<const char*>(header.data()), 54);
    
    std::vector<uint8_t> row(paddedRowSize, 0);
    
    for (int y = height - 1; y >= 0; --y) {
        for (int x = 0; x < width; ++x) {
            int srcIdx = (y * width + (width - 1 - x)) * channels;
            int dstIdx = x * 3;
            
            if (channels >= 3) {
                row[dstIdx + 0] = data[srcIdx + 2];
                row[dstIdx + 1] = data[srcIdx + 1];
                row[dstIdx + 2] = data[srcIdx + 0];
            } else {
                row[dstIdx + 0] = data[srcIdx];
                row[dstIdx + 1] = data[srcIdx];
                row[dstIdx + 2] = data[srcIdx];
            }
        }
        file.write(reinterpret_cast<const char*>(row.data()), paddedRowSize);
    }
    
    file.close();
    return true;
}

MokRenderResult PdfDeviceOutput::Render(const std::string& outputPath) {
    if (!m_impl->initialized) {
        return MokRenderResult(MokRenderErrorCode::RENDER_ERROR,
                              "Not initialized");
    }
    
    auto device = draw::RasterImageDevice::Create(m_impl->width, m_impl->height, 4);
    if (!device) {
        return MokRenderResult(MokRenderErrorCode::RENDER_ERROR,
                              "Failed to create raster device for PDF");
    }
    
    draw::DrawResult result = device->Initialize();
    if (result != draw::DrawResult::kSuccess) {
        return MokRenderResult(MokRenderErrorCode::RENDER_ERROR,
                              "Failed to initialize device");
    }
    
    draw::Color white = draw::Color::White();
    device->Clear(white);
    
    draw::DrawParams params;
    params.extent = Envelope(0, 0, 1000, 1000);
    params.pixel_width = m_impl->width;
    params.pixel_height = m_impl->height;
    params.dpi = m_impl->dpi;
    
    result = device->BeginDraw(params);
    if (result != draw::DrawResult::kSuccess) {
        return MokRenderResult(MokRenderErrorCode::RENDER_ERROR,
                              "Failed to begin drawing");
    }
    
    draw::DrawStyle redStyle;
    redStyle.stroke.color = draw::Color(255, 0, 0, 255).GetRGBA();
    redStyle.stroke.width = 2.0;
    redStyle.stroke.visible = true;
    
    draw::DrawStyle blueStyle;
    blueStyle.fill.color = draw::Color(0, 0, 255, 100).GetRGBA();
    blueStyle.fill.visible = true;
    blueStyle.stroke.color = draw::Color(0, 0, 255, 255).GetRGBA();
    blueStyle.stroke.width = 1.0;
    blueStyle.stroke.visible = true;
    
    draw::DrawStyle greenStyle;
    greenStyle.stroke.color = draw::Color(0, 255, 0, 255).GetRGBA();
    greenStyle.stroke.width = 3.0;
    greenStyle.stroke.visible = true;
    
    draw::DrawStyle yellowFillStyle;
    yellowFillStyle.fill.color = draw::Color(255, 255, 0, 180).GetRGBA();
    yellowFillStyle.fill.visible = true;
    yellowFillStyle.stroke.color = draw::Color(255, 165, 0, 255).GetRGBA();
    yellowFillStyle.stroke.width = 2.0;
    yellowFillStyle.stroke.visible = true;
    
    draw::DrawStyle purpleFillStyle;
    purpleFillStyle.fill.color = draw::Color(128, 0, 128, 150).GetRGBA();
    purpleFillStyle.fill.visible = true;
    purpleFillStyle.stroke.color = draw::Color(75, 0, 130, 255).GetRGBA();
    purpleFillStyle.stroke.width = 2.0;
    purpleFillStyle.stroke.visible = true;
    
    for (int i = 0; i < 10; ++i) {
        double x = 50 + i * 75;
        double y = 50 + (i % 3) * 30;
        device->DrawCircle(x, y, 15, redStyle);
    }
    
    for (int i = 0; i < 5; ++i) {
        double cx = 100 + i * 140;
        double cy = 150;
        double rx = 30 + i * 5;
        double ry = 20 + i * 3;
        device->DrawEllipse(cx, cy, rx, ry, yellowFillStyle);
    }
    
    for (int i = 0; i < 5; ++i) {
        double x = 80 + i * 140;
        device->DrawLine(x, 220, x + 80, 320, greenStyle);
    }
    
    for (int i = 0; i < 3; ++i) {
        double x = 120 + i * 220;
        double y = 380 + i * 40;
        device->DrawRect(x, y, 120, 70, blueStyle);
    }
    
    for (int i = 0; i < 4; ++i) {
        double cx = 150 + i * 180;
        double cy = 520;
        device->DrawEllipse(cx, cy, 50, 30, purpleFillStyle);
    }
    
    draw::Font font("Arial", 14);
    draw::Color textColor = draw::Color(0, 0, 0, 255);
    
    const char* annotations[] = {
        "Point 1", "Point 2", "Point 3", "Point 4", "Point 5",
        "Line A", "Line B", "Line C",
        "Polygon 1", "Polygon 2",
        "Ellipse Zone", "Circle Area",
        "Annotation Demo", "Mock Data Render"
    };
    
    double textPositions[][2] = {
        {30, 30}, {105, 30}, {180, 30}, {255, 30}, {330, 30},
        {100, 200}, {240, 200}, {380, 200},
        {150, 360}, {370, 400},
        {100, 500}, {280, 500},
        {350, 550}, {500, 570}
    };
    
    for (int i = 0; i < 14; ++i) {
        device->DrawText(textPositions[i][0], textPositions[i][1], 
                         annotations[i], font, textColor);
    }
    
    result = device->EndDraw();
    if (result != draw::DrawResult::kSuccess) {
        return MokRenderResult(MokRenderErrorCode::RENDER_ERROR,
                              "Failed to end drawing");
    }
    
    const uint8_t* data = device->GetData();
    int width = device->GetWidth();
    int height = device->GetHeight();
    int channels = device->GetChannels();
    
    if (!data || width <= 0 || height <= 0) {
        return MokRenderResult(MokRenderErrorCode::RENDER_ERROR,
                              "Invalid image data");
    }
    
    std::string bmpPath = outputPath;
    if (bmpPath.size() >= 4 && bmpPath.substr(bmpPath.size() - 4) == ".pdf") {
        bmpPath = bmpPath.substr(0, bmpPath.size() - 4) + ".bmp";
    }
    
    size_t lastSlash = bmpPath.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        std::string dir = bmpPath.substr(0, lastSlash);
        #ifdef _WIN32
        _mkdir(dir.c_str());
        #else
        mkdir(dir.c_str(), 0755);
        #endif
    }
    
    bool saved = SaveAsBMP(bmpPath, width, height, data, channels);
    if (!saved) {
        return MokRenderResult(MokRenderErrorCode::RENDER_ERROR,
                              "Failed to save output file: " + bmpPath);
    }
    
    return MokRenderResult();
}

}
}
