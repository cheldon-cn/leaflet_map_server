#include "ogc/draw/raster_image_device.h"
#include "ogc/draw/log.h"
#include <cmath>
#include <algorithm>
#include <cstring>

namespace ogc {
namespace draw {

RasterImageDevice::RasterImageDevice()
    : m_width(0)
    , m_height(0)
    , m_channels(4)
    , m_dpi(96.0)
    , m_state(DeviceState::kCreated)
    , m_drawing(false)
    , m_antialiasing(true)
    , m_opacity(1.0)
    , m_clipX(0)
    , m_clipY(0)
    , m_clipWidth(0)
    , m_clipHeight(0)
    , m_hasClipRect(false) {
}

RasterImageDevice::RasterImageDevice(int width, int height, int channels)
    : m_width(width)
    , m_height(height)
    , m_channels(channels)
    , m_dpi(96.0)
    , m_state(DeviceState::kCreated)
    , m_drawing(false)
    , m_antialiasing(true)
    , m_opacity(1.0)
    , m_clipX(0)
    , m_clipY(0)
    , m_clipWidth(width)
    , m_clipHeight(height)
    , m_hasClipRect(false) {
    if (width > 0 && height > 0 && channels >= 1 && channels <= 4) {
        m_data.resize(static_cast<size_t>(width) * height * channels, 255);
        m_state = DeviceState::kInitialized;
    }
}

RasterImageDevice::~RasterImageDevice() {
    Finalize();
}

DrawResult RasterImageDevice::Initialize() {
    if (m_width <= 0 || m_height <= 0) {
        return DrawResult::kInvalidParams;
    }
    
    m_data.resize(static_cast<size_t>(m_width) * m_height * m_channels, 255);
    m_state = DeviceState::kInitialized;
    return DrawResult::kSuccess;
}

DrawResult RasterImageDevice::Finalize() {
    m_data.clear();
    m_state = DeviceState::kClosed;
    return DrawResult::kSuccess;
}

DeviceState RasterImageDevice::GetState() const {
    return m_state;
}

bool RasterImageDevice::IsReady() const {
    return m_state == DeviceState::kInitialized || m_state == DeviceState::kActive;
}

DrawResult RasterImageDevice::BeginDraw(const DrawParams& params) {
    if (!IsReady()) {
        return DrawResult::kDeviceNotReady;
    }
    
    m_params = params;
    m_drawing = true;
    m_state = DeviceState::kActive;
    return DrawResult::kSuccess;
}

DrawResult RasterImageDevice::EndDraw() {
    m_drawing = false;
    m_state = DeviceState::kInitialized;
    return DrawResult::kSuccess;
}

DrawResult RasterImageDevice::AbortDraw() {
    m_drawing = false;
    m_state = DeviceState::kError;
    return DrawResult::kSuccess;
}

bool RasterImageDevice::IsDrawing() const {
    return m_drawing;
}

void RasterImageDevice::SetTransform(const TransformMatrix& transform) {
    m_transform = transform;
}

TransformMatrix RasterImageDevice::GetTransform() const {
    return m_transform;
}

void RasterImageDevice::ResetTransform() {
    m_transform = TransformMatrix::Identity();
}

void RasterImageDevice::SetClipRect(double x, double y, double width, double height) {
    m_clipX = x;
    m_clipY = y;
    m_clipWidth = width;
    m_clipHeight = height;
    m_hasClipRect = true;
}

void RasterImageDevice::ClearClipRect() {
    m_hasClipRect = false;
    m_clipX = 0;
    m_clipY = 0;
    m_clipWidth = m_width;
    m_clipHeight = m_height;
}

bool RasterImageDevice::HasClipRect() const {
    return m_hasClipRect;
}

DrawResult RasterImageDevice::DrawPoint(double x, double y, const DrawStyle& style) {
    if (!IsReady()) return DrawResult::kDeviceNotReady;
    
    double tx, ty;
    TransformPoint(x, y, tx, ty);
    
    int px = static_cast<int>(std::round(tx));
    int py = static_cast<int>(std::round(ty));
    
    if (IsPointVisible(tx, ty)) {
        Color color(style.fill.color);
        BlendPixel(px, py, color);
    }
    
    return DrawResult::kSuccess;
}

DrawResult RasterImageDevice::DrawLine(double x1, double y1, double x2, double y2, const DrawStyle& style) {
    if (!IsReady()) return DrawResult::kDeviceNotReady;
    
    double tx1, ty1, tx2, ty2;
    TransformPoint(x1, y1, tx1, ty1);
    TransformPoint(x2, y2, tx2, ty2);
    
    Color color(style.stroke.color);
    int px1 = static_cast<int>(std::round(tx1));
    int py1 = static_cast<int>(std::round(ty1));
    int px2 = static_cast<int>(std::round(tx2));
    int py2 = static_cast<int>(std::round(ty2));
    
    DrawLineBresenham(px1, py1, px2, py2, color);
    
    return DrawResult::kSuccess;
}

DrawResult RasterImageDevice::DrawRect(double x, double y, double width, double height, const DrawStyle& style) {
    if (!IsReady()) return DrawResult::kDeviceNotReady;
    
    if (style.HasFill()) {
        Color fillColor(style.fill.color);
        int startX = static_cast<int>(std::floor(x));
        int startY = static_cast<int>(std::floor(y));
        int endX = static_cast<int>(std::ceil(x + width));
        int endY = static_cast<int>(std::ceil(y + height));
        
        for (int py = startY; py < endY; ++py) {
            for (int px = startX; px < endX; ++px) {
                if (IsPointVisible(px, py)) {
                    BlendPixel(px, py, fillColor);
                }
            }
        }
    }
    
    if (style.HasStroke()) {
        DrawLine(x, y, x + width, y, style);
        DrawLine(x + width, y, x + width, y + height, style);
        DrawLine(x + width, y + height, x, y + height, style);
        DrawLine(x, y + height, x, y, style);
    }
    
    return DrawResult::kSuccess;
}

DrawResult RasterImageDevice::DrawCircle(double cx, double cy, double radius, const DrawStyle& style) {
    return DrawEllipse(cx, cy, radius, radius, style);
}

DrawResult RasterImageDevice::DrawEllipse(double cx, double cy, double rx, double ry, const DrawStyle& style) {
    if (!IsReady()) return DrawResult::kDeviceNotReady;
    
    if (style.HasFill()) {
        Color fillColor(style.fill.color);
        for (int py = 0; py < m_height; ++py) {
            for (int px = 0; px < m_width; ++px) {
                double dx = (px - cx) / rx;
                double dy = (py - cy) / ry;
                if (dx * dx + dy * dy <= 1.0) {
                    BlendPixel(px, py, fillColor);
                }
            }
        }
    }
    
    if (style.HasStroke()) {
        Color strokeColor(style.stroke.color);
        int steps = static_cast<int>(std::max(rx, ry) * 8);
        for (int i = 0; i < steps; ++i) {
            double angle = 2.0 * 3.14159265358979323846 * i / steps;
            int px = static_cast<int>(std::round(cx + rx * std::cos(angle)));
            int py = static_cast<int>(std::round(cy + ry * std::sin(angle)));
            BlendPixel(px, py, strokeColor);
        }
    }
    
    return DrawResult::kSuccess;
}

DrawResult RasterImageDevice::DrawArc(double cx, double cy, double rx, double ry, double startAngle, double sweepAngle, const DrawStyle& style) {
    if (!IsReady()) return DrawResult::kDeviceNotReady;
    
    Color strokeColor(style.stroke.color);
    int steps = static_cast<int>(std::max(rx, ry) * std::abs(sweepAngle) / 0.1);
    steps = std::max(steps, 10);
    
    for (int i = 0; i < steps; ++i) {
        double angle = startAngle + sweepAngle * i / steps;
        int px = static_cast<int>(std::round(cx + rx * std::cos(angle)));
        int py = static_cast<int>(std::round(cy + ry * std::sin(angle)));
        BlendPixel(px, py, strokeColor);
    }
    
    return DrawResult::kSuccess;
}

DrawResult RasterImageDevice::DrawPolygon(const double* x, const double* y, int count, const DrawStyle& style) {
    if (!IsReady() || count < 3) return DrawResult::kInvalidParams;
    
    if (style.HasFill()) {
        Color fillColor(style.fill.color);
        
        int minY = m_height, maxY = 0;
        for (int i = 0; i < count; ++i) {
            int py = static_cast<int>(std::round(y[i]));
            minY = std::min(minY, py);
            maxY = std::max(maxY, py);
        }
        
        for (int scanY = minY; scanY <= maxY; ++scanY) {
            std::vector<int> intersections;
            for (int i = 0; i < count; ++i) {
                int j = (i + 1) % count;
                double y1 = y[i], y2 = y[j];
                double x1 = x[i], x2 = x[j];
                
                if ((y1 <= scanY && y2 > scanY) || (y2 <= scanY && y1 > scanY)) {
                    double t = (scanY - y1) / (y2 - y1);
                    int ix = static_cast<int>(std::round(x1 + t * (x2 - x1)));
                    intersections.push_back(ix);
                }
            }
            
            std::sort(intersections.begin(), intersections.end());
            
            for (size_t k = 0; k + 1 < intersections.size(); k += 2) {
                for (int px = intersections[k]; px <= intersections[k + 1]; ++px) {
                    BlendPixel(px, scanY, fillColor);
                }
            }
        }
    }
    
    if (style.HasStroke()) {
        DrawPolyline(x, y, count, style);
        DrawLine(x[count - 1], y[count - 1], x[0], y[0], style);
    }
    
    return DrawResult::kSuccess;
}

DrawResult RasterImageDevice::DrawPolyline(const double* x, const double* y, int count, const DrawStyle& style) {
    if (!IsReady() || count < 2) return DrawResult::kInvalidParams;
    
    for (int i = 0; i < count - 1; ++i) {
        DrawLine(x[i], y[i], x[i + 1], y[i + 1], style);
    }
    
    return DrawResult::kSuccess;
}

DrawResult RasterImageDevice::DrawGeometry(const Geometry* geometry, const DrawStyle& style) {
    if (!IsReady() || !geometry) return DrawResult::kInvalidParams;
    
    (void)geometry;
    (void)style;
    
    return DrawResult::kSuccess;
}

DrawResult RasterImageDevice::DrawText(double x, double y, const std::string& text, const Font& font, const Color& color) {
    if (!IsReady()) return DrawResult::kDeviceNotReady;
    
    (void)x;
    (void)y;
    (void)text;
    (void)font;
    (void)color;
    
    return DrawResult::kSuccess;
}

DrawResult RasterImageDevice::DrawTextWithBackground(double x, double y, const std::string& text, const Font& font, const Color& textColor, const Color& bgColor) {
    if (!IsReady()) return DrawResult::kDeviceNotReady;
    
    (void)x;
    (void)y;
    (void)text;
    (void)font;
    (void)textColor;
    (void)bgColor;
    
    return DrawResult::kSuccess;
}

DrawResult RasterImageDevice::DrawImage(double x, double y, double width, double height, const uint8_t* data, int dataWidth, int dataHeight, int channels) {
    if (!IsReady() || !data) return DrawResult::kInvalidParams;
    
    int startX = static_cast<int>(std::floor(x));
    int startY = static_cast<int>(std::floor(y));
    int endX = static_cast<int>(std::ceil(x + width));
    int endY = static_cast<int>(std::ceil(y + height));
    
    double scaleX = static_cast<double>(dataWidth) / width;
    double scaleY = static_cast<double>(dataHeight) / height;
    
    for (int py = startY; py < endY && py < m_height; ++py) {
        for (int px = startX; px < endX && px < m_width; ++px) {
            if (px < 0 || py < 0) continue;
            
            int srcX = static_cast<int>((px - x) * scaleX);
            int srcY = static_cast<int>((py - y) * scaleY);
            
            if (srcX >= 0 && srcX < dataWidth && srcY >= 0 && srcY < dataHeight) {
                size_t srcIdx = static_cast<size_t>(srcY) * dataWidth * channels + srcX * channels;
                Color pixelColor;
                if (channels >= 3) {
                    pixelColor = Color(data[srcIdx], data[srcIdx + 1], data[srcIdx + 2],
                                       channels >= 4 ? data[srcIdx + 3] : 255);
                } else {
                    pixelColor = Color(data[srcIdx], data[srcIdx], data[srcIdx]);
                }
                BlendPixel(px, py, pixelColor);
            }
        }
    }
    
    return DrawResult::kSuccess;
}

DrawResult RasterImageDevice::DrawImageRegion(double destX, double destY, double destWidth, double destHeight,
                                               const uint8_t* data, int dataWidth, int dataHeight, int channels,
                                               int srcX, int srcY, int srcWidth, int srcHeight) {
    if (!IsReady() || !data) return DrawResult::kInvalidParams;
    
    double scaleX = static_cast<double>(srcWidth) / destWidth;
    double scaleY = static_cast<double>(srcHeight) / destHeight;
    
    int startX = static_cast<int>(std::floor(destX));
    int startY = static_cast<int>(std::floor(destY));
    int endX = static_cast<int>(std::ceil(destX + destWidth));
    int endY = static_cast<int>(std::ceil(destY + destHeight));
    
    for (int py = startY; py < endY && py < m_height; ++py) {
        for (int px = startX; px < endX && px < m_width; ++px) {
            if (px < 0 || py < 0) continue;
            
            int dataSrcX = srcX + static_cast<int>((px - destX) * scaleX);
            int dataSrcY = srcY + static_cast<int>((py - destY) * scaleY);
            
            if (dataSrcX >= 0 && dataSrcX < dataWidth && dataSrcY >= 0 && dataSrcY < dataHeight) {
                size_t srcIdx = static_cast<size_t>(dataSrcY) * dataWidth * channels + dataSrcX * channels;
                Color pixelColor;
                if (channels >= 3) {
                    pixelColor = Color(data[srcIdx], data[srcIdx + 1], data[srcIdx + 2],
                                       channels >= 4 ? data[srcIdx + 3] : 255);
                } else {
                    pixelColor = Color(data[srcIdx], data[srcIdx], data[srcIdx]);
                }
                BlendPixel(px, py, pixelColor);
            }
        }
    }
    
    return DrawResult::kSuccess;
}

void RasterImageDevice::Clear(const Color& color) {
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            SetPixel(x, y, color);
        }
    }
}

void RasterImageDevice::Fill(const Color& color) {
    Clear(color);
}

int RasterImageDevice::GetWidth() const { return m_width; }
int RasterImageDevice::GetHeight() const { return m_height; }
double RasterImageDevice::GetDpi() const { return m_dpi; }

void RasterImageDevice::SetDpi(double dpi) { m_dpi = dpi; }

DrawParams RasterImageDevice::GetDrawParams() const { return m_params; }

void RasterImageDevice::SetAntialiasing(bool enable) { m_antialiasing = enable; }
bool RasterImageDevice::IsAntialiasingEnabled() const { return m_antialiasing; }

void RasterImageDevice::SetOpacity(double opacity) { m_opacity = opacity; }
double RasterImageDevice::GetOpacity() const { return m_opacity; }

void RasterImageDevice::Resize(int width, int height, int channels) {
    m_width = width;
    m_height = height;
    m_channels = channels;
    m_data.resize(static_cast<size_t>(width) * height * channels, 255);
}

int RasterImageDevice::GetChannels() const { return m_channels; }

uint8_t* RasterImageDevice::GetData() { return m_data.data(); }
const uint8_t* RasterImageDevice::GetData() const { return m_data.data(); }
size_t RasterImageDevice::GetDataSize() const { return m_data.size(); }

bool RasterImageDevice::SaveToFile(const std::string& filepath, ImageFormat format) const {
    (void)filepath;
    (void)format;
    return false;
}

bool RasterImageDevice::LoadFromFile(const std::string& filepath) {
    (void)filepath;
    return false;
}

Color RasterImageDevice::GetPixel(int x, int y) const {
    if (x < 0 || x >= m_width || y < 0 || y >= m_height || m_data.empty()) {
        return Color::Transparent();
    }
    
    size_t idx = static_cast<size_t>(y) * m_width * m_channels + x * m_channels;
    
    if (m_channels >= 4) {
        return Color(m_data[idx], m_data[idx + 1], m_data[idx + 2], m_data[idx + 3]);
    } else if (m_channels == 3) {
        return Color(m_data[idx], m_data[idx + 1], m_data[idx + 2]);
    } else {
        return Color(m_data[idx], m_data[idx], m_data[idx]);
    }
}

void RasterImageDevice::SetPixel(int x, int y, const Color& color) {
    if (x < 0 || x >= m_width || y < 0 || y >= m_height || m_data.empty()) {
        return;
    }
    
    size_t idx = static_cast<size_t>(y) * m_width * m_channels + x * m_channels;
    
    m_data[idx] = color.GetRed();
    if (m_channels >= 2) {
        m_data[idx + 1] = color.GetGreen();
    }
    if (m_channels >= 3) {
        m_data[idx + 2] = color.GetBlue();
    }
    if (m_channels >= 4) {
        m_data[idx + 3] = color.GetAlpha();
    }
}

std::shared_ptr<RasterImageDevice> RasterImageDevice::Create(int width, int height, int channels) {
    return std::make_shared<RasterImageDevice>(width, height, channels);
}

void RasterImageDevice::TransformPoint(double x, double y, double& outX, double& outY) const {
    m_transform.Transform(x, y, outX, outY);
}

bool RasterImageDevice::IsPointVisible(double x, double y) const {
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
        return false;
    }
    if (m_hasClipRect) {
        return x >= m_clipX && x < m_clipX + m_clipWidth &&
               y >= m_clipY && y < m_clipY + m_clipHeight;
    }
    return true;
}

bool RasterImageDevice::IsRectVisible(double x, double y, double width, double height) const {
    if (x + width < 0 || x >= m_width || y + height < 0 || y >= m_height) {
        return false;
    }
    return true;
}

void RasterImageDevice::DrawLineBresenham(int x0, int y0, int x1, int y1, const Color& color) {
    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx - dy;
    
    while (true) {
        if (IsPointVisible(x0, y0)) {
            BlendPixel(x0, y0, color);
        }
        
        if (x0 == x1 && y0 == y1) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void RasterImageDevice::FillScanline(int y, int x0, int x1, const Color& color) {
    if (y < 0 || y >= m_height) return;
    
    int startX = std::max(0, x0);
    int endX = std::min(m_width - 1, x1);
    
    for (int x = startX; x <= endX; ++x) {
        BlendPixel(x, y, color);
    }
}

void RasterImageDevice::BlendPixel(int x, int y, const Color& color) {
    if (x < 0 || x >= m_width || y < 0 || y >= m_height || m_data.empty()) {
        return;
    }
    
    if (m_channels < 3) {
        SetPixel(x, y, color);
        return;
    }
    
    size_t idx = static_cast<size_t>(y) * m_width * m_channels + x * m_channels;
    
    double srcAlpha = color.GetAlphaF() * m_opacity;
    double dstAlpha = m_channels >= 4 ? m_data[idx + 3] / 255.0 : 1.0;
    
    double outAlpha = srcAlpha + dstAlpha * (1.0 - srcAlpha);
    if (outAlpha < 1e-10) {
        return;
    }
    
    double srcFactor = srcAlpha / outAlpha;
    double dstFactor = dstAlpha * (1.0 - srcAlpha) / outAlpha;
    
    m_data[idx] = static_cast<uint8_t>(std::round(color.GetRed() * srcFactor + m_data[idx] * dstFactor));
    m_data[idx + 1] = static_cast<uint8_t>(std::round(color.GetGreen() * srcFactor + m_data[idx + 1] * dstFactor));
    m_data[idx + 2] = static_cast<uint8_t>(std::round(color.GetBlue() * srcFactor + m_data[idx + 2] * dstFactor));
    
    if (m_channels >= 4) {
        m_data[idx + 3] = static_cast<uint8_t>(std::round(outAlpha * 255.0));
    }
}

}  
}  
