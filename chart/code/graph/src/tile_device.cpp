#include "ogc/draw/tile_device.h"
#include "ogc/point.h"
#include "ogc/linestring.h"
#include "ogc/polygon.h"
#include "ogc/common.h"
#include <fstream>
#include <cmath>
#include <algorithm>

namespace ogc {
namespace draw {

TileDevice::TileDevice()
    : m_tileSize(256)
    , m_dpi(96.0)
    , m_state(DeviceState::kCreated)
    , m_drawing(false)
    , m_antialiasing(true)
    , m_opacity(1.0)
    , m_currentX(0)
    , m_currentY(0)
    , m_currentZ(0)
    , m_channels(4)
    , m_hasClipRect(false)
    , m_tileFormat("png")
{
    m_transform = TransformMatrix::Identity();
}

TileDevice::TileDevice(int tileSize)
    : m_tileSize(tileSize)
    , m_dpi(96.0)
    , m_state(DeviceState::kCreated)
    , m_drawing(false)
    , m_antialiasing(true)
    , m_opacity(1.0)
    , m_currentX(0)
    , m_currentY(0)
    , m_currentZ(0)
    , m_channels(4)
    , m_hasClipRect(false)
    , m_tileFormat("png")
{
    m_transform = TransformMatrix::Identity();
}

TileDevice::~TileDevice() {
    if (m_drawing) {
        EndDraw();
    }
}

DrawResult TileDevice::Initialize() {
    if (m_state == DeviceState::kInitialized) {
        return DrawResult::kSuccess;
    }
    
    m_tileData.resize(m_tileSize * m_tileSize * m_channels, 0);
    m_tiles.clear();
    
    m_state = DeviceState::kInitialized;
    return DrawResult::kSuccess;
}

DrawResult TileDevice::Finalize() {
    if (m_drawing) {
        EndDraw();
    }
    
    m_tileData.clear();
    m_tiles.clear();
    m_state = DeviceState::kClosed;
    return DrawResult::kSuccess;
}

DeviceState TileDevice::GetState() const {
    return m_state;
}

bool TileDevice::IsReady() const {
    return m_state == DeviceState::kInitialized;
}

DrawResult TileDevice::BeginDraw(const DrawParams& params) {
    if (m_state != DeviceState::kInitialized) {
        return DrawResult::kDeviceNotReady;
    }
    
    if (m_drawing) {
        return DrawResult::kFailed;
    }
    
    m_params = params;
    m_drawing = true;
    
    return DrawResult::kSuccess;
}

DrawResult TileDevice::EndDraw() {
    if (!m_drawing) {
        return DrawResult::kFailed;
    }
    
    m_drawing = false;
    return DrawResult::kSuccess;
}

DrawResult TileDevice::AbortDraw() {
    m_drawing = false;
    return DrawResult::kSuccess;
}

bool TileDevice::IsDrawing() const {
    return m_drawing;
}

void TileDevice::SetTransform(const TransformMatrix& transform) {
    m_transform = transform;
}

TransformMatrix TileDevice::GetTransform() const {
    return m_transform;
}

void TileDevice::ResetTransform() {
    m_transform = TransformMatrix::Identity();
}

void TileDevice::SetClipRect(double x, double y, double width, double height) {
    m_clipX = x;
    m_clipY = y;
    m_clipWidth = width;
    m_clipHeight = height;
    m_hasClipRect = true;
}

void TileDevice::ClearClipRect() {
    m_hasClipRect = false;
}

bool TileDevice::HasClipRect() const {
    return m_hasClipRect;
}

DrawResult TileDevice::DrawPoint(double x, double y, const DrawStyle& style) {
    if (!m_drawing) {
        return DrawResult::kFailed;
    }
    
    double tx, ty;
    TransformPoint(x, y, tx, ty);
    
    if (!IsPointInTile(tx, ty)) {
        return DrawResult::kSuccess;
    }
    
    double radius = style.stroke.width > 0 ? style.stroke.width : 3.0;
    
    for (int dy = -radius; dy <= radius; ++dy) {
        for (int dx = -radius; dx <= radius; ++dx) {
            if (dx * dx + dy * dy <= radius * radius) {
                int px = static_cast<int>(tx + dx);
                int py = static_cast<int>(ty + dy);
                if (px >= 0 && px < m_tileSize && py >= 0 && py < m_tileSize) {
                    BlendPixel(px, py, Color(style.fill.color));
                }
            }
        }
    }
    
    return DrawResult::kSuccess;
}

DrawResult TileDevice::DrawLine(double x1, double y1, double x2, double y2, const DrawStyle& style) {
    if (!m_drawing) {
        return DrawResult::kFailed;
    }
    
    double tx1, ty1, tx2, ty2;
    TransformPoint(x1, y1, tx1, ty1);
    TransformPoint(x2, y2, tx2, ty2);
    
    DrawLineBresenham(static_cast<int>(tx1), static_cast<int>(ty1),
                      static_cast<int>(tx2), static_cast<int>(ty2),
                      Color(style.stroke.color));
    
    return DrawResult::kSuccess;
}

DrawResult TileDevice::DrawRect(double x, double y, double width, double height, const DrawStyle& style) {
    if (!m_drawing) {
        return DrawResult::kFailed;
    }
    
    double tx, ty;
    TransformPoint(x, y, tx, ty);
    
    if (style.fill.visible) {
        for (int py = static_cast<int>(ty); py < static_cast<int>(ty + height); ++py) {
            for (int px = static_cast<int>(tx); px < static_cast<int>(tx + width); ++px) {
                if (px >= 0 && px < m_tileSize && py >= 0 && py < m_tileSize) {
                    BlendPixel(px, py, Color(style.fill.color));
                }
            }
        }
    }
    
    if (style.stroke.visible) {
        DrawLine(x, y, x + width, y, style);
        DrawLine(x + width, y, x + width, y + height, style);
        DrawLine(x + width, y + height, x, y + height, style);
        DrawLine(x, y + height, x, y, style);
    }
    
    return DrawResult::kSuccess;
}

DrawResult TileDevice::DrawCircle(double cx, double cy, double radius, const DrawStyle& style) {
    return DrawEllipse(cx, cy, radius, radius, style);
}

DrawResult TileDevice::DrawEllipse(double cx, double cy, double rx, double ry, const DrawStyle& style) {
    if (!m_drawing) {
        return DrawResult::kFailed;
    }
    
    double tx, ty;
    TransformPoint(cx, cy, tx, ty);
    
    if (style.fill.visible) {
        for (int py = static_cast<int>(ty - ry); py <= static_cast<int>(ty + ry); ++py) {
            for (int px = static_cast<int>(tx - rx); px <= static_cast<int>(tx + rx); ++px) {
                double dx = px - tx;
                double dy = py - ty;
                if ((dx * dx) / (rx * rx) + (dy * dy) / (ry * ry) <= 1.0) {
                    if (px >= 0 && px < m_tileSize && py >= 0 && py < m_tileSize) {
                        BlendPixel(px, py, Color(style.fill.color));
                    }
                }
            }
        }
    }
    
    if (style.stroke.visible) {
        const int segments = 72;
        double angleStep = 2.0 * 3.14159265358979323846 / segments;
        
        for (int i = 0; i < segments; ++i) {
            double angle1 = i * angleStep;
            double angle2 = (i + 1) * angleStep;
            
            double x1 = tx + rx * std::cos(angle1);
            double y1 = ty + ry * std::sin(angle1);
            double x2 = tx + rx * std::cos(angle2);
            double y2 = ty + ry * std::sin(angle2);
            
            DrawLineBresenham(static_cast<int>(x1), static_cast<int>(y1),
                              static_cast<int>(x2), static_cast<int>(y2),
                              Color(style.stroke.color));
        }
    }
    
    return DrawResult::kSuccess;
}

DrawResult TileDevice::DrawArc(double cx, double cy, double rx, double ry,
                                double startAngle, double sweepAngle, const DrawStyle& style) {
    if (!m_drawing) {
        return DrawResult::kFailed;
    }
    
    double tx, ty;
    TransformPoint(cx, cy, tx, ty);
    
    const int segments = 36;
    double angleStep = sweepAngle / segments;
    
    for (int i = 0; i < segments; ++i) {
        double angle1 = startAngle + i * angleStep;
        double angle2 = startAngle + (i + 1) * angleStep;
        
        double x1 = tx + rx * std::cos(angle1);
        double y1 = ty + ry * std::sin(angle1);
        double x2 = tx + rx * std::cos(angle2);
        double y2 = ty + ry * std::sin(angle2);
        
        DrawLineBresenham(static_cast<int>(x1), static_cast<int>(y1),
                          static_cast<int>(x2), static_cast<int>(y2),
                          Color(style.stroke.color));
    }
    
    return DrawResult::kSuccess;
}

DrawResult TileDevice::DrawPolygon(const double* x, const double* y, int count, const DrawStyle& style) {
    if (!m_drawing || count < 3) {
        return DrawResult::kFailed;
    }
    
    std::vector<double> tx(count), ty(count);
    for (int i = 0; i < count; ++i) {
        TransformPoint(x[i], y[i], tx[i], ty[i]);
    }
    
    if (style.fill.visible) {
        double minY = ty[0], maxY = ty[0];
        for (int i = 1; i < count; ++i) {
            minY = std::min(minY, ty[i]);
            maxY = std::max(maxY, ty[i]);
        }
        
        for (int scanY = static_cast<int>(minY); scanY <= static_cast<int>(maxY); ++scanY) {
            std::vector<int> intersections;
            
            for (int i = 0; i < count; ++i) {
                int j = (i + 1) % count;
                
                if ((ty[i] <= scanY && ty[j] > scanY) || (ty[j] <= scanY && ty[i] > scanY)) {
                    double t = (scanY - ty[i]) / (ty[j] - ty[i]);
                    int x = static_cast<int>(tx[i] + t * (tx[j] - tx[i]));
                    intersections.push_back(x);
                }
            }
            
            std::sort(intersections.begin(), intersections.end());
            
            for (size_t k = 0; k + 1 < intersections.size(); k += 2) {
                for (int px = intersections[k]; px <= intersections[k + 1]; ++px) {
                    if (px >= 0 && px < m_tileSize && scanY >= 0 && scanY < m_tileSize) {
                        BlendPixel(px, scanY, Color(style.fill.color));
                    }
                }
            }
        }
    }
    
    if (style.stroke.visible) {
        for (int i = 0; i < count; ++i) {
            int j = (i + 1) % count;
            DrawLineBresenham(static_cast<int>(tx[i]), static_cast<int>(ty[i]),
                              static_cast<int>(tx[j]), static_cast<int>(ty[j]),
                              Color(style.stroke.color));
        }
    }
    
    return DrawResult::kSuccess;
}

DrawResult TileDevice::DrawPolyline(const double* x, const double* y, int count, const DrawStyle& style) {
    if (!m_drawing || count < 2) {
        return DrawResult::kFailed;
    }
    
    for (int i = 0; i < count - 1; ++i) {
        DrawLine(x[i], y[i], x[i + 1], y[i + 1], style);
    }
    
    return DrawResult::kSuccess;
}

DrawResult TileDevice::DrawGeometry(const Geometry* geometry, const DrawStyle& style) {
    if (!geometry || !m_drawing) {
        return DrawResult::kInvalidParams;
    }
    
    auto geomType = geometry->GetGeometryType();
    
    switch (geomType) {
        case GeomType::kPoint: {
            auto point = dynamic_cast<const Point*>(geometry);
            if (point) {
                return DrawPoint(point->GetX(), point->GetY(), style);
            }
            break;
        }
        case GeomType::kLineString: {
            auto lineString = dynamic_cast<const LineString*>(geometry);
            if (lineString) {
                int numPoints = lineString->GetNumPoints();
                std::vector<double> x(numPoints), y(numPoints);
                for (int i = 0; i < numPoints; ++i) {
                    auto coord = lineString->GetCoordinateN(i);
                    x[i] = coord.x;
                    y[i] = coord.y;
                }
                return DrawPolyline(x.data(), y.data(), numPoints, style);
            }
            break;
        }
        case GeomType::kPolygon: {
            auto polygon = dynamic_cast<const Polygon*>(geometry);
            if (polygon) {
                auto exteriorRing = polygon->GetExteriorRing();
                if (exteriorRing) {
                    int numPoints = exteriorRing->GetNumPoints();
                    std::vector<double> x(numPoints), y(numPoints);
                    for (int i = 0; i < numPoints; ++i) {
                        auto coord = exteriorRing->GetCoordinateN(i);
                        x[i] = coord.x;
                        y[i] = coord.y;
                    }
                    return DrawPolygon(x.data(), y.data(), numPoints, style);
                }
            }
            break;
        }
        default:
            break;
    }
    
    return DrawResult::kSuccess;
}

DrawResult TileDevice::DrawText(double x, double y, const std::string& text,
                                 const Font& font, const Color& color) {
    if (!m_drawing) {
        return DrawResult::kFailed;
    }
    
    double tx, ty;
    TransformPoint(x, y, tx, ty);
    
    double charWidth = font.GetSize() * 0.6;
    double charHeight = font.GetSize();
    
    for (size_t i = 0; i < text.length(); ++i) {
        int px = static_cast<int>(tx + i * charWidth);
        int py = static_cast<int>(ty);
        
        for (int dy = 0; dy < static_cast<int>(charHeight); ++dy) {
            for (int dx = 0; dx < static_cast<int>(charWidth); ++dx) {
                int pixelX = px + dx;
                int pixelY = py + dy;
                if (pixelX >= 0 && pixelX < m_tileSize && pixelY >= 0 && pixelY < m_tileSize) {
                    BlendPixel(pixelX, pixelY, color);
                }
            }
        }
    }
    
    return DrawResult::kSuccess;
}

DrawResult TileDevice::DrawTextWithBackground(double x, double y, const std::string& text,
                                               const Font& font, const Color& textColor,
                                               const Color& bgColor) {
    double textWidth = text.length() * font.GetSize() * 0.6;
    double textHeight = font.GetSize() * 1.2;
    
    DrawStyle bgStyle;
    bgStyle.fill.visible = true;
    bgStyle.fill.color = bgColor.GetRGBA();
    bgStyle.stroke.visible = false;
    
    DrawResult result = DrawRect(x, y - font.GetSize() * 0.2, textWidth, textHeight, bgStyle);
    if (result != DrawResult::kSuccess) {
        return result;
    }
    
    return DrawText(x, y, text, font, textColor);
}

DrawResult TileDevice::DrawImage(double x, double y, double width, double height,
                                  const uint8_t* data, int dataWidth, int dataHeight, int channels) {
    if (!m_drawing || !data) {
        return DrawResult::kInvalidParams;
    }
    
    double tx, ty;
    TransformPoint(x, y, tx, ty);
    
    double scaleX = width / dataWidth;
    double scaleY = height / dataHeight;
    
    for (int py = 0; py < static_cast<int>(height); ++py) {
        for (int px = 0; px < static_cast<int>(width); ++px) {
            int srcX = static_cast<int>(px / scaleX);
            int srcY = static_cast<int>(py / scaleY);
            
            int dstX = static_cast<int>(tx + px);
            int dstY = static_cast<int>(ty + py);
            
            if (dstX >= 0 && dstX < m_tileSize && dstY >= 0 && dstY < m_tileSize) {
                int srcIdx = (srcY * dataWidth + srcX) * channels;
                
                Color color;
                if (channels >= 3) {
                    color.SetRedF(data[srcIdx] / 255.0);
                    color.SetGreenF(data[srcIdx + 1] / 255.0);
                    color.SetBlueF(data[srcIdx + 2] / 255.0);
                    color.SetAlphaF((channels >= 4) ? data[srcIdx + 3] / 255.0 : 1.0);
                } else if (channels == 1) {
                    color.SetRedF(data[srcIdx] / 255.0);
                    color.SetGreenF(data[srcIdx] / 255.0);
                    color.SetBlueF(data[srcIdx] / 255.0);
                    color.SetAlphaF(1.0);
                }
                
                BlendPixel(dstX, dstY, color);
            }
        }
    }
    
    return DrawResult::kSuccess;
}

DrawResult TileDevice::DrawImageRegion(double destX, double destY, double destWidth, double destHeight,
                                        const uint8_t* data, int dataWidth, int dataHeight, int channels,
                                        int srcX, int srcY, int srcWidth, int srcHeight) {
    if (!data) {
        return DrawResult::kInvalidParams;
    }
    
    std::vector<uint8_t> regionData(srcWidth * srcHeight * channels);
    
    for (int y = 0; y < srcHeight; ++y) {
        for (int x = 0; x < srcWidth; ++x) {
            int srcIdx = ((srcY + y) * dataWidth + (srcX + x)) * channels;
            int dstIdx = (y * srcWidth + x) * channels;
            for (int c = 0; c < channels; ++c) {
                regionData[dstIdx + c] = data[srcIdx + c];
            }
        }
    }
    
    return DrawImage(destX, destY, destWidth, destHeight,
                     regionData.data(), srcWidth, srcHeight, channels);
}

void TileDevice::Clear(const Color& color) {
    uint8_t r = color.GetRed();
    uint8_t g = color.GetGreen();
    uint8_t b = color.GetBlue();
    uint8_t a = color.GetAlpha();
    
    for (size_t i = 0; i < m_tileData.size(); i += m_channels) {
        m_tileData[i] = r;
        if (m_channels > 1) m_tileData[i + 1] = g;
        if (m_channels > 2) m_tileData[i + 2] = b;
        if (m_channels > 3) m_tileData[i + 3] = a;
    }
}

void TileDevice::Fill(const Color& color) {
    Clear(color);
}

int TileDevice::GetWidth() const {
    return m_tileSize;
}

int TileDevice::GetHeight() const {
    return m_tileSize;
}

double TileDevice::GetDpi() const {
    return m_dpi;
}

void TileDevice::SetDpi(double dpi) {
    m_dpi = dpi;
}

DrawParams TileDevice::GetDrawParams() const {
    return m_params;
}

void TileDevice::SetAntialiasing(bool enable) {
    m_antialiasing = enable;
}

bool TileDevice::IsAntialiasingEnabled() const {
    return m_antialiasing;
}

void TileDevice::SetOpacity(double opacity) {
    m_opacity = opacity;
}

double TileDevice::GetOpacity() const {
    return m_opacity;
}

void TileDevice::SetTileSize(int size) {
    m_tileSize = size;
    m_tileData.resize(m_tileSize * m_tileSize * m_channels, 0);
}

int TileDevice::GetTileSize() const {
    return m_tileSize;
}

void TileDevice::SetCurrentTile(int x, int y, int z) {
    m_currentX = x;
    m_currentY = y;
    m_currentZ = z;
}

TileKey TileDevice::GetCurrentTileKey() const {
    return TileKey(m_currentX, m_currentY, m_currentZ);
}

DrawResult TileDevice::BeginTile(int x, int y, int z) {
    m_currentX = x;
    m_currentY = y;
    m_currentZ = z;
    
    m_tileData.assign(m_tileSize * m_tileSize * m_channels, 0);
    
    return DrawResult::kSuccess;
}

DrawResult TileDevice::EndTile() {
    TileData tile;
    tile.x = m_currentX;
    tile.y = m_currentY;
    tile.z = m_currentZ;
    tile.width = m_tileSize;
    tile.height = m_tileSize;
    tile.data = m_tileData;
    tile.valid = true;
    
    m_tiles[std::make_tuple(m_currentX, m_currentY, m_currentZ)] = tile;
    
    return DrawResult::kSuccess;
}

TileData TileDevice::GetCurrentTileData() const {
    auto it = m_tiles.find(std::make_tuple(m_currentX, m_currentY, m_currentZ));
    if (it != m_tiles.end()) {
        return it->second;
    }
    return TileData();
}

std::vector<TileData> TileDevice::GetAllTileData() const {
    std::vector<TileData> result;
    for (const auto& pair : m_tiles) {
        result.push_back(pair.second);
    }
    return result;
}

bool TileDevice::SaveTile(const std::string& directory, const std::string& format) {
    return false;
}

bool TileDevice::LoadTile(const std::string& filepath, int x, int y, int z) {
    return false;
}

void TileDevice::SetTileFormat(const std::string& format) {
    m_tileFormat = format;
}

std::string TileDevice::GetTileFormat() const {
    return m_tileFormat;
}

std::shared_ptr<TileDevice> TileDevice::Create(int tileSize) {
    auto device = std::make_shared<TileDevice>(tileSize);
    device->Initialize();
    return device;
}

void TileDevice::TransformPoint(double x, double y, double& outX, double& outY) const {
    outX = m_transform.GetM00() * x + m_transform.GetM01() * y + m_transform.GetM02();
    outY = m_transform.GetM10() * x + m_transform.GetM11() * y + m_transform.GetM12();
}

bool TileDevice::IsPointInTile(double x, double y) const {
    return x >= 0 && x < m_tileSize && y >= 0 && y < m_tileSize;
}

bool TileDevice::IsRectInTile(double x, double y, double width, double height) const {
    return x + width >= 0 && x < m_tileSize && y + height >= 0 && y < m_tileSize;
}

void TileDevice::DrawLineBresenham(int x0, int y0, int x1, int y1, const Color& color) {
    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx - dy;
    
    while (true) {
        if (x0 >= 0 && x0 < m_tileSize && y0 >= 0 && y0 < m_tileSize) {
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

void TileDevice::FillScanline(int y, int x0, int x1, const Color& color) {
    if (y < 0 || y >= m_tileSize) return;
    
    int startX = std::max(0, std::min(x0, x1));
    int endX = std::min(m_tileSize - 1, std::max(x0, x1));
    
    for (int x = startX; x <= endX; ++x) {
        BlendPixel(x, y, color);
    }
}

void TileDevice::BlendPixel(int x, int y, const Color& color) {
    if (x < 0 || x >= m_tileSize || y < 0 || y >= m_tileSize) return;
    
    size_t idx = (y * m_tileSize + x) * m_channels;
    
    if (m_channels >= 4) {
        double srcA = color.GetAlphaF() * m_opacity;
        double dstA = m_tileData[idx + 3] / 255.0;
        double outA = srcA + dstA * (1.0 - srcA);
        
        if (outA > 0) {
            m_tileData[idx] = static_cast<uint8_t>((color.GetRedF() * srcA + m_tileData[idx] / 255.0 * dstA * (1.0 - srcA)) / outA * 255);
            m_tileData[idx + 1] = static_cast<uint8_t>((color.GetGreenF() * srcA + m_tileData[idx + 1] / 255.0 * dstA * (1.0 - srcA)) / outA * 255);
            m_tileData[idx + 2] = static_cast<uint8_t>((color.GetBlueF() * srcA + m_tileData[idx + 2] / 255.0 * dstA * (1.0 - srcA)) / outA * 255);
            m_tileData[idx + 3] = static_cast<uint8_t>(outA * 255);
        }
    } else if (m_channels >= 3) {
        m_tileData[idx] = color.GetRed();
        m_tileData[idx + 1] = color.GetGreen();
        m_tileData[idx + 2] = color.GetBlue();
    }
}

std::string TileDevice::GetTileFilePath(const std::string& directory, int x, int y, int z) const {
    return directory + "/" + std::to_string(z) + "/" + std::to_string(x) + "/" + std::to_string(y) + "." + m_tileFormat;
}

}
}
