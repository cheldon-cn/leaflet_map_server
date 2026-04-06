#include "ogc/graph/util/image_draw.h"
#include <algorithm>
#include <cmath>
#include <fstream>

namespace ogc {
namespace graph {

ImageData::ImageData()
    : m_width(0)
    , m_height(0)
    , m_channels(4)
    , m_data(nullptr)
{
}

ImageData::ImageData(int width, int height, int channels)
    : m_width(width)
    , m_height(height)
    , m_channels(channels)
    , m_data(nullptr)
{
    if (width > 0 && height > 0 && channels > 0) {
        m_data.reset(new unsigned char[width * height * channels]());
    }
}

ImageData::ImageData(int width, int height, int channels, const unsigned char* data)
    : m_width(width)
    , m_height(height)
    , m_channels(channels)
    , m_data(nullptr)
{
    if (width > 0 && height > 0 && channels > 0 && data) {
        size_t size = width * height * channels;
        m_data.reset(new unsigned char[size]);
        memcpy(m_data.get(), data, size);
    }
}

ImageData::~ImageData()
{
}

ogc::draw::Color ImageData::GetPixel(int x, int y) const
{
    if (!IsValid() || x < 0 || x >= m_width || y < 0 || y >= m_height) {
        return ogc::draw::Color();
    }
    
    size_t index = (y * m_width + x) * m_channels;
    
    if (m_channels >= 4) {
        return ogc::draw::Color(m_data[index], m_data[index + 1], m_data[index + 2], m_data[index + 3]);
    } else if (m_channels == 3) {
        return ogc::draw::Color(m_data[index], m_data[index + 1], m_data[index + 2], 255);
    } else if (m_channels == 1) {
        return ogc::draw::Color(m_data[index], m_data[index], m_data[index], 255);
    }
    
    return ogc::draw::Color();
}

void ImageData::SetPixel(int x, int y, const ogc::draw::Color& color)
{
    if (!IsValid() || x < 0 || x >= m_width || y < 0 || y >= m_height) {
        return;
    }
    
    size_t index = (y * m_width + x) * m_channels;
    
    if (m_channels >= 4) {
        m_data[index] = color.GetRed();
        m_data[index + 1] = color.GetGreen();
        m_data[index + 2] = color.GetBlue();
        m_data[index + 3] = color.GetAlpha();
    } else if (m_channels == 3) {
        m_data[index] = color.GetRed();
        m_data[index + 1] = color.GetGreen();
        m_data[index + 2] = color.GetBlue();
    } else if (m_channels == 1) {
        m_data[index] = static_cast<unsigned char>((color.GetRed() + color.GetGreen() + color.GetBlue()) / 3);
    }
}

void ImageData::Fill(const ogc::draw::Color& color)
{
    if (!IsValid()) {
        return;
    }
    
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            SetPixel(x, y, color);
        }
    }
}

void ImageData::Clear()
{
    m_width = 0;
    m_height = 0;
    m_channels = 4;
    m_data.reset();
}

ImageDataPtr ImageData::Clone() const
{
    if (!IsValid()) {
        return ImageDataPtr(new ImageData());
    }
    
    return ImageDataPtr(new ImageData(m_width, m_height, m_channels, m_data.get()));
}

bool ImageData::Save(const std::string& path, ImageFormat format) const
{
    (void)path;
    (void)format;
    return false;
}

ImageDataPtr ImageData::Load(const std::string& path)
{
    (void)path;
    return ImageDataPtr(new ImageData());
}

ImageDataPtr ImageData::Resize(int newWidth, int newHeight) const
{
    if (!IsValid() || newWidth <= 0 || newHeight <= 0) {
        return ImageDataPtr(new ImageData());
    }
    
    ImageDataPtr result(new ImageData(newWidth, newHeight, m_channels));
    
    double scaleX = static_cast<double>(m_width) / newWidth;
    double scaleY = static_cast<double>(m_height) / newHeight;
    
    for (int y = 0; y < newHeight; ++y) {
        for (int x = 0; x < newWidth; ++x) {
            int srcX = static_cast<int>(x * scaleX);
            int srcY = static_cast<int>(y * scaleY);
            result->SetPixel(x, y, GetPixel(srcX, srcY));
        }
    }
    
    return result;
}

ImageDataPtr ImageData::Crop(int x, int y, int width, int height) const
{
    if (!IsValid() || width <= 0 || height <= 0) {
        return ImageDataPtr(new ImageData());
    }
    
    x = std::max(0, x);
    y = std::max(0, y);
    width = std::min(width, m_width - x);
    height = std::min(height, m_height - y);
    
    if (width <= 0 || height <= 0) {
        return ImageDataPtr(new ImageData());
    }
    
    ImageDataPtr result(new ImageData(width, height, m_channels));
    
    for (int dy = 0; dy < height; ++dy) {
        for (int dx = 0; dx < width; ++dx) {
            result->SetPixel(dx, dy, GetPixel(x + dx, y + dy));
        }
    }
    
    return result;
}

ImageDataPtr ImageData::FlipHorizontal() const
{
    if (!IsValid()) {
        return ImageDataPtr(new ImageData());
    }
    
    ImageDataPtr result(new ImageData(m_width, m_height, m_channels));
    
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            result->SetPixel(m_width - 1 - x, y, GetPixel(x, y));
        }
    }
    
    return result;
}

ImageDataPtr ImageData::FlipVertical() const
{
    if (!IsValid()) {
        return ImageDataPtr(new ImageData());
    }
    
    ImageDataPtr result(new ImageData(m_width, m_height, m_channels));
    
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            result->SetPixel(x, m_height - 1 - y, GetPixel(x, y));
        }
    }
    
    return result;
}

ImageDataPtr ImageData::Rotate90() const
{
    if (!IsValid()) {
        return ImageDataPtr(new ImageData());
    }
    
    ImageDataPtr result(new ImageData(m_height, m_width, m_channels));
    
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            result->SetPixel(m_height - 1 - y, x, GetPixel(x, y));
        }
    }
    
    return result;
}

ImageDataPtr ImageData::Rotate180() const
{
    if (!IsValid()) {
        return ImageDataPtr(new ImageData());
    }
    
    ImageDataPtr result(new ImageData(m_width, m_height, m_channels));
    
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            result->SetPixel(m_width - 1 - x, m_height - 1 - y, GetPixel(x, y));
        }
    }
    
    return result;
}

ImageDataPtr ImageData::Rotate270() const
{
    if (!IsValid()) {
        return ImageDataPtr(new ImageData());
    }
    
    ImageDataPtr result(new ImageData(m_height, m_width, m_channels));
    
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            result->SetPixel(y, m_width - 1 - x, GetPixel(x, y));
        }
    }
    
    return result;
}

ImageDraw::ImageDraw()
    : m_imageData(nullptr)
    , m_clipX(0)
    , m_clipY(0)
    , m_clipWidth(0)
    , m_clipHeight(0)
    , m_hasClipRect(false)
{
}

ImageDraw::ImageDraw(ImageData* imageData)
    : m_imageData(imageData)
    , m_clipX(0)
    , m_clipY(0)
    , m_clipWidth(0)
    , m_clipHeight(0)
    , m_hasClipRect(false)
{
}

ImageDraw::~ImageDraw()
{
}

void ImageDraw::SetImageData(ImageData* imageData)
{
    m_imageData = imageData;
}

void ImageDraw::SetPixel(int x, int y, const ogc::draw::Color& color)
{
    if (!IsValid() || !IsPointInClipRect(x, y)) {
        return;
    }
    
    m_imageData->SetPixel(x, y, color);
}

ogc::draw::Color ImageDraw::GetPixel(int x, int y) const
{
    if (!IsValid()) {
        return ogc::draw::Color();
    }
    
    return m_imageData->GetPixel(x, y);
}

void ImageDraw::DrawLine(int x1, int y1, int x2, int y2, const ogc::draw::Color& color, int thickness)
{
    DrawLineBresenham(x1, y1, x2, y2, color, thickness);
}

void ImageDraw::DrawLineAA(int x1, int y1, int x2, int y2, const ogc::draw::Color& color, int thickness)
{
    DrawLineXiaolinWu(x1, y1, x2, y2, color, thickness);
}

void ImageDraw::DrawRect(int x, int y, int width, int height, const ogc::draw::Color& color, int thickness)
{
    for (int i = 0; i < thickness; ++i) {
        DrawLine(x + i, y + i, x + width - 1 - i, y + i, color);
        DrawLine(x + width - 1 - i, y + i, x + width - 1 - i, y + height - 1 - i, color);
        DrawLine(x + width - 1 - i, y + height - 1 - i, x + i, y + height - 1 - i, color);
        DrawLine(x + i, y + height - 1 - i, x + i, y + i, color);
    }
}

void ImageDraw::FillRect(int x, int y, int width, int height, const ogc::draw::Color& color)
{
    if (!IsValid()) {
        return;
    }
    
    for (int dy = y; dy < y + height; ++dy) {
        for (int dx = x; dx < x + width; ++dx) {
            if (IsPointInClipRect(dx, dy)) {
                m_imageData->SetPixel(dx, dy, color);
            }
        }
    }
}

void ImageDraw::DrawCircle(int cx, int cy, int radius, const ogc::draw::Color& color, int thickness)
{
    DrawCircleMidpoint(cx, cy, radius, color, thickness);
}

void ImageDraw::FillCircle(int cx, int cy, int radius, const ogc::draw::Color& color)
{
    FillCircleScanline(cx, cy, radius, color);
}

void ImageDraw::DrawEllipse(int cx, int cy, int rx, int ry, const ogc::draw::Color& color, int thickness)
{
    DrawEllipseMidpoint(cx, cy, rx, ry, color, thickness);
}

void ImageDraw::FillEllipse(int cx, int cy, int rx, int ry, const ogc::draw::Color& color)
{
    FillEllipseScanline(cx, cy, rx, ry, color);
}

void ImageDraw::DrawPolygon(const std::vector<std::pair<int, int>>& points, const ogc::draw::Color& color, int thickness)
{
    if (points.size() < 2) {
        return;
    }
    
    for (size_t i = 0; i < points.size(); ++i) {
        size_t next = (i + 1) % points.size();
        DrawLine(points[i].first, points[i].second, points[next].first, points[next].second, color, thickness);
    }
}

void ImageDraw::FillPolygon(const std::vector<std::pair<int, int>>& points, const ogc::draw::Color& color)
{
    FillPolygonScanline(points, color);
}

void ImageDraw::DrawText(int x, int y, const std::string& text, const ogc::draw::Color& color, const std::string& fontFamily, int fontSize)
{
    (void)x;
    (void)y;
    (void)text;
    (void)color;
    (void)fontFamily;
    (void)fontSize;
}

void ImageDraw::DrawImage(int x, int y, const ImageData* image, double opacity)
{
    if (!IsValid() || !image || !image->IsValid()) {
        return;
    }
    
    DrawImage(x, y, image, 0, 0, image->GetWidth(), image->GetHeight(), opacity);
}

void ImageDraw::DrawImage(int x, int y, const ImageData* image, int srcX, int srcY, int srcWidth, int srcHeight, double opacity)
{
    if (!IsValid() || !image || !image->IsValid()) {
        return;
    }
    
    opacity = std::max(0.0, std::min(1.0, opacity));
    
    for (int dy = 0; dy < srcHeight; ++dy) {
        for (int dx = 0; dx < srcWidth; ++dx) {
            int dstX = x + dx;
            int dstY = y + dy;
            
            if (!IsPointInClipRect(dstX, dstY)) {
                continue;
            }
            
            ogc::draw::Color srcColor = image->GetPixel(srcX + dx, srcY + dy);
            ogc::draw::Color dstColor = m_imageData->GetPixel(dstX, dstY);
            
            double alpha = srcColor.GetAlpha() / 255.0 * opacity;
            
            unsigned char r = static_cast<unsigned char>(srcColor.GetRed() * alpha + dstColor.GetRed() * (1 - alpha));
            unsigned char g = static_cast<unsigned char>(srcColor.GetGreen() * alpha + dstColor.GetGreen() * (1 - alpha));
            unsigned char b = static_cast<unsigned char>(srcColor.GetBlue() * alpha + dstColor.GetBlue() * (1 - alpha));
            unsigned char a = static_cast<unsigned char>(255 * alpha + dstColor.GetAlpha() * (1 - alpha));
            
            m_imageData->SetPixel(dstX, dstY, ogc::draw::Color(r, g, b, a));
        }
    }
}

void ImageDraw::Blend(int x, int y, const ogc::draw::Color& color, double alpha)
{
    if (!IsValid() || !IsPointInClipRect(x, y)) {
        return;
    }
    
    ogc::draw::Color dstColor = m_imageData->GetPixel(x, y);
    
    alpha = std::max(0.0, std::min(1.0, alpha));
    
    unsigned char r = static_cast<unsigned char>(color.GetRed() * alpha + dstColor.GetRed() * (1 - alpha));
    unsigned char g = static_cast<unsigned char>(color.GetGreen() * alpha + dstColor.GetGreen() * (1 - alpha));
    unsigned char b = static_cast<unsigned char>(color.GetBlue() * alpha + dstColor.GetBlue() * (1 - alpha));
    unsigned char a = static_cast<unsigned char>(color.GetAlpha() * alpha + dstColor.GetAlpha() * (1 - alpha));
    
    m_imageData->SetPixel(x, y, ogc::draw::Color(r, g, b, a));
}

void ImageDraw::Clear(const ogc::draw::Color& color)
{
    if (!IsValid()) {
        return;
    }
    
    m_imageData->Fill(color);
}

int ImageDraw::GetWidth() const
{
    return IsValid() ? m_imageData->GetWidth() : 0;
}

int ImageDraw::GetHeight() const
{
    return IsValid() ? m_imageData->GetHeight() : 0;
}

void ImageDraw::SetClipRect(int x, int y, int width, int height)
{
    m_clipX = x;
    m_clipY = y;
    m_clipWidth = width;
    m_clipHeight = height;
    m_hasClipRect = true;
}

void ImageDraw::ClearClipRect()
{
    m_clipX = 0;
    m_clipY = 0;
    m_clipWidth = 0;
    m_clipHeight = 0;
    m_hasClipRect = false;
}

bool ImageDraw::IsPointInClipRect(int x, int y) const
{
    if (!m_hasClipRect) {
        return true;
    }
    
    return x >= m_clipX && x < m_clipX + m_clipWidth &&
           y >= m_clipY && y < m_clipY + m_clipHeight;
}

void ImageDraw::DrawLineBresenham(int x1, int y1, int x2, int y2, const ogc::draw::Color& color, int thickness)
{
    int dx = std::abs(x2 - x1);
    int dy = std::abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    
    while (true) {
        for (int tx = -thickness / 2; tx <= thickness / 2; ++tx) {
            for (int ty = -thickness / 2; ty <= thickness / 2; ++ty) {
                if (IsPointInClipRect(x1 + tx, y1 + ty)) {
                    m_imageData->SetPixel(x1 + tx, y1 + ty, color);
                }
            }
        }
        
        if (x1 == x2 && y1 == y2) {
            break;
        }
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void ImageDraw::DrawLineXiaolinWu(int x1, int y1, int x2, int y2, const ogc::draw::Color& color, int thickness)
{
    (void)thickness;
    
    bool steep = std::abs(y2 - y1) > std::abs(x2 - x1);
    
    if (steep) {
        std::swap(x1, y1);
        std::swap(x2, y2);
    }
    
    if (x1 > x2) {
        std::swap(x1, x2);
        std::swap(y1, y2);
    }
    
    int dx = x2 - x1;
    int dy = y2 - y1;
    double gradient = (dx == 0) ? 1.0 : static_cast<double>(dy) / dx;
    
    int xend = x1;
    double yend = y1 + gradient * (xend - x1);
    double xgap = 1.0 - ((x1 + 0.5) - static_cast<int>(x1 + 0.5));
    int xpxl1 = xend;
    int ypxl1 = static_cast<int>(yend);
    
    if (steep) {
        Blend(xpxl1, ypxl1, color, (1.0 - (yend - ypxl1)) * xgap);
        Blend(xpxl1, ypxl1 + 1, color, (yend - ypxl1) * xgap);
    } else {
        Blend(ypxl1, xpxl1, color, (1.0 - (yend - ypxl1)) * xgap);
        Blend(ypxl1 + 1, xpxl1, color, (yend - ypxl1) * xgap);
    }
    
    double intery = yend + gradient;
    
    xend = x2;
    yend = y2 + gradient * (xend - x2);
    xgap = (x2 + 0.5) - static_cast<int>(x2 + 0.5);
    int xpxl2 = xend;
    int ypxl2 = static_cast<int>(yend);
    
    if (steep) {
        Blend(xpxl2, ypxl2, color, (1.0 - (yend - ypxl2)) * xgap);
        Blend(xpxl2, ypxl2 + 1, color, (yend - ypxl2) * xgap);
    } else {
        Blend(ypxl2, xpxl2, color, (1.0 - (yend - ypxl2)) * xgap);
        Blend(ypxl2 + 1, xpxl2, color, (yend - ypxl2) * xgap);
    }
    
    if (steep) {
        for (int x = xpxl1 + 1; x < xpxl2; ++x) {
            Blend(x, static_cast<int>(intery), color, 1.0 - (intery - static_cast<int>(intery)));
            Blend(x, static_cast<int>(intery) + 1, color, intery - static_cast<int>(intery));
            intery += gradient;
        }
    } else {
        for (int x = xpxl1 + 1; x < xpxl2; ++x) {
            Blend(static_cast<int>(intery), x, color, 1.0 - (intery - static_cast<int>(intery)));
            Blend(static_cast<int>(intery) + 1, x, color, intery - static_cast<int>(intery));
            intery += gradient;
        }
    }
}

void ImageDraw::DrawCircleMidpoint(int cx, int cy, int radius, const ogc::draw::Color& color, int thickness)
{
    for (int t = 0; t < thickness; ++t) {
        int r = radius - t;
        if (r < 0) break;
        
        int x = r;
        int y = 0;
        int err = 0;
        
        while (x >= y) {
            if (IsPointInClipRect(cx + x, cy + y)) m_imageData->SetPixel(cx + x, cy + y, color);
            if (IsPointInClipRect(cx + y, cy + x)) m_imageData->SetPixel(cx + y, cy + x, color);
            if (IsPointInClipRect(cx - y, cy + x)) m_imageData->SetPixel(cx - y, cy + x, color);
            if (IsPointInClipRect(cx - x, cy + y)) m_imageData->SetPixel(cx - x, cy + y, color);
            if (IsPointInClipRect(cx - x, cy - y)) m_imageData->SetPixel(cx - x, cy - y, color);
            if (IsPointInClipRect(cx - y, cy - x)) m_imageData->SetPixel(cx - y, cy - x, color);
            if (IsPointInClipRect(cx + y, cy - x)) m_imageData->SetPixel(cx + y, cy - x, color);
            if (IsPointInClipRect(cx + x, cy - y)) m_imageData->SetPixel(cx + x, cy - y, color);
            
            y++;
            err += 1 + 2 * y;
            if (2 * (err - x) + 1 > 0) {
                x--;
                err += 1 - 2 * x;
            }
        }
    }
}

void ImageDraw::FillCircleScanline(int cx, int cy, int radius, const ogc::draw::Color& color)
{
    for (int y = -radius; y <= radius; ++y) {
        int width = static_cast<int>(std::sqrt(radius * radius - y * y));
        for (int x = -width; x <= width; ++x) {
            if (IsPointInClipRect(cx + x, cy + y)) {
                m_imageData->SetPixel(cx + x, cy + y, color);
            }
        }
    }
}

void ImageDraw::DrawEllipseMidpoint(int cx, int cy, int rx, int ry, const ogc::draw::Color& color, int thickness)
{
    for (int t = 0; t < thickness; ++t) {
        int currentRx = rx - t;
        int currentRy = ry - t;
        if (currentRx < 0 || currentRy < 0) break;
        
        int x = 0;
        int y = currentRy;
        double rx2 = currentRx * currentRx;
        double ry2 = currentRy * currentRy;
        double err = ry2 - rx2 * currentRy + 0.25 * rx2;
        
        while (ry2 * x <= rx2 * y) {
            if (IsPointInClipRect(cx + x, cy + y)) m_imageData->SetPixel(cx + x, cy + y, color);
            if (IsPointInClipRect(cx - x, cy + y)) m_imageData->SetPixel(cx - x, cy + y, color);
            if (IsPointInClipRect(cx + x, cy - y)) m_imageData->SetPixel(cx + x, cy - y, color);
            if (IsPointInClipRect(cx - x, cy - y)) m_imageData->SetPixel(cx - x, cy - y, color);
            
            x++;
            err += 2 * ry2 * x + ry2;
            if (err > 0) {
                y--;
                err -= 2 * rx2 * y;
            }
        }
        
        err = ry2 * (x + 0.5) * (x + 0.5) + rx2 * (y - 1) * (y - 1) - rx2 * ry2;
        
        while (y >= 0) {
            if (IsPointInClipRect(cx + x, cy + y)) m_imageData->SetPixel(cx + x, cy + y, color);
            if (IsPointInClipRect(cx - x, cy + y)) m_imageData->SetPixel(cx - x, cy + y, color);
            if (IsPointInClipRect(cx + x, cy - y)) m_imageData->SetPixel(cx + x, cy - y, color);
            if (IsPointInClipRect(cx - x, cy - y)) m_imageData->SetPixel(cx - x, cy - y, color);
            
            y--;
            err -= 2 * rx2 * y + rx2;
            if (err < 0) {
                x++;
                err += 2 * ry2 * x;
            }
        }
    }
}

void ImageDraw::FillEllipseScanline(int cx, int cy, int rx, int ry, const ogc::draw::Color& color)
{
    for (int y = -ry; y <= ry; ++y) {
        double width = rx * std::sqrt(1.0 - static_cast<double>(y * y) / (ry * ry));
        for (int x = -static_cast<int>(width); x <= static_cast<int>(width); ++x) {
            if (IsPointInClipRect(cx + x, cy + y)) {
                m_imageData->SetPixel(cx + x, cy + y, color);
            }
        }
    }
}

void ImageDraw::FillPolygonScanline(const std::vector<std::pair<int, int>>& points, const ogc::draw::Color& color)
{
    if (points.size() < 3) {
        return;
    }
    
    int minY = points[0].second;
    int maxY = points[0].second;
    for (const auto& p : points) {
        minY = std::min(minY, p.second);
        maxY = std::max(maxY, p.second);
    }
    
    for (int y = minY; y <= maxY; ++y) {
        std::vector<int> intersections;
        
        for (size_t i = 0; i < points.size(); ++i) {
            size_t next = (i + 1) % points.size();
            
            int y1 = points[i].second;
            int y2 = points[next].second;
            int x1 = points[i].first;
            int x2 = points[next].first;
            
            if ((y1 <= y && y2 > y) || (y2 <= y && y1 > y)) {
                double x = x1 + (double)(y - y1) / (y2 - y1) * (x2 - x1);
                intersections.push_back(static_cast<int>(x));
            }
        }
        
        std::sort(intersections.begin(), intersections.end());
        
        for (size_t i = 0; i + 1 < intersections.size(); i += 2) {
            for (int x = intersections[i]; x <= intersections[i + 1]; ++x) {
                if (IsPointInClipRect(x, y)) {
                    m_imageData->SetPixel(x, y, color);
                }
            }
        }
    }
}

unsigned char ImageDraw::BlendChannel(unsigned char dst, unsigned char src, double alpha)
{
    return static_cast<unsigned char>(src * alpha + dst * (1 - alpha));
}

ImageFormat GetImageFormatFromExtension(const std::string& path)
{
    size_t dotPos = path.rfind('.');
    if (dotPos == std::string::npos) {
        return ImageFormat::kUnknown;
    }
    
    std::string ext = path.substr(dotPos + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "bmp") return ImageFormat::kBMP;
    if (ext == "jpg" || ext == "jpeg") return ImageFormat::kJPEG;
    if (ext == "png") return ImageFormat::kPNG;
    if (ext == "tif" || ext == "tiff") return ImageFormat::kTIFF;
    if (ext == "gif") return ImageFormat::kGIF;
    if (ext == "webp") return ImageFormat::kWEBP;
    
    return ImageFormat::kUnknown;
}

std::string GetExtensionFromImageFormat(ImageFormat format)
{
    switch (format) {
        case ImageFormat::kBMP: return "bmp";
        case ImageFormat::kJPEG: return "jpg";
        case ImageFormat::kPNG: return "png";
        case ImageFormat::kTIFF: return "tif";
        case ImageFormat::kGIF: return "gif";
        case ImageFormat::kWEBP: return "webp";
        default: return "";
    }
}

}
}
