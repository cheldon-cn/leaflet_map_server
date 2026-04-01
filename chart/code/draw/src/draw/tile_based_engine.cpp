#include "ogc/draw/tile_based_engine.h"
#include <algorithm>

namespace ogc {
namespace draw {

TileBasedEngine::TileBasedEngine(TileDevice* device)
    : m_device(device)
    , m_transform(TransformMatrix::Identity())
    , m_opacity(1.0)
    , m_tolerance(0.0)
    , m_antialiasing(true)
    , m_active(false)
    , m_progress(0.0f)
    , m_completedTiles(0)
    , m_totalTiles(0) {
}

TileBasedEngine::~TileBasedEngine() {
    if (m_active) {
        End();
    }
}

DrawResult TileBasedEngine::Begin() {
    if (!m_device || !m_device->IsReady()) {
        return DrawResult::kDeviceError;
    }
    m_active = true;
    m_progress = 0.0f;
    m_completedTiles = 0;
    m_totalTiles = m_device->GetTotalTileCount();
    return DrawResult::kSuccess;
}

void TileBasedEngine::End() {
    m_active = false;
}

void TileBasedEngine::BeginTile(const TileInfo& tile) {
    RasterImageDevice* rasterTile = m_device->GetTile(tile.indexX, tile.indexY);
    if (!rasterTile) {
        return;
    }

    m_tileEngine.reset(new Simple2DEngine(rasterTile));
    m_tileEngine->Begin();
    m_tileEngine->SetTransform(m_transform);
    m_tileEngine->SetOpacity(m_opacity);
    m_tileEngine->SetAntialiasingEnabled(m_antialiasing);
    
    if (!m_clipRegion.IsEmpty()) {
        m_tileEngine->SetClipRegion(m_clipRegion);
    }
}

void TileBasedEngine::EndTile(const TileInfo& tile) {
    if (m_tileEngine) {
        m_tileEngine->End();
        m_device->SetTileDirty(tile.indexX, tile.indexY, true);
    }
    m_tileEngine.reset();
    
    m_completedTiles++;
    UpdateProgress();
}

std::vector<TileInfo> TileBasedEngine::CalculateInvolvedTiles(const Rect& bounds) {
    return m_device->GetTilesInRect(
        static_cast<int>(bounds.x),
        static_cast<int>(bounds.y),
        static_cast<int>(bounds.w),
        static_cast<int>(bounds.h));
}

void TileBasedEngine::UpdateProgress() {
    if (m_totalTiles > 0) {
        m_progress = static_cast<float>(m_completedTiles) / static_cast<float>(m_totalTiles);
        if (m_progressCallback) {
            m_progressCallback(m_progress);
        }
    }
}

void TileBasedEngine::SetProgressCallback(ProgressCallback callback) {
    m_progressCallback = callback;
}

DrawResult TileBasedEngine::DrawPoints(const double* x, const double* y, int count,
                                        const DrawStyle& style) {
    if (!m_active) return DrawResult::kInvalidState;
    if (!x || !y || count <= 0) return DrawResult::kInvalidParameter;

    for (int i = 0; i < count; ++i) {
        Point pt = m_transform.TransformPoint(Point(x[i], y[i]));
        std::vector<TileInfo> tiles = m_device->GetTilesInRect(
            static_cast<int>(pt.x - style.pen.width),
            static_cast<int>(pt.y - style.pen.width),
            static_cast<int>(style.pen.width * 2 + 1),
            static_cast<int>(style.pen.width * 2 + 1));

        for (const auto& tile : tiles) {
            BeginTile(tile);
            if (m_tileEngine) {
                double localX = x[i] - tile.pixelX;
                double localY = y[i] - tile.pixelY;
                m_tileEngine->DrawPoints(&localX, &localY, 1, style);
            }
            EndTile(tile);
        }
    }

    return DrawResult::kSuccess;
}

DrawResult TileBasedEngine::DrawLines(const double* x1, const double* y1,
                                       const double* x2, const double* y2, int count,
                                       const DrawStyle& style) {
    if (!m_active) return DrawResult::kInvalidState;
    if (!x1 || !y1 || !x2 || !y2 || count <= 0) return DrawResult::kInvalidParameter;

    for (int i = 0; i < count; ++i) {
        Point p1 = m_transform.TransformPoint(Point(x1[i], y1[i]));
        Point p2 = m_transform.TransformPoint(Point(x2[i], y2[i]));
        
        double minX = std::min(p1.x, p2.x);
        double minY = std::min(p1.y, p2.y);
        double maxX = std::max(p1.x, p2.x);
        double maxY = std::max(p1.y, p2.y);
        
        std::vector<TileInfo> tiles = m_device->GetTilesInRect(
            static_cast<int>(minX), static_cast<int>(minY),
            static_cast<int>(maxX - minX + 1), static_cast<int>(maxY - minY + 1));

        for (const auto& tile : tiles) {
            BeginTile(tile);
            if (m_tileEngine) {
                double lx1 = x1[i] - tile.pixelX;
                double ly1 = y1[i] - tile.pixelY;
                double lx2 = x2[i] - tile.pixelX;
                double ly2 = y2[i] - tile.pixelY;
                m_tileEngine->DrawLines(&lx1, &ly1, &lx2, &ly2, 1, style);
            }
            EndTile(tile);
        }
    }

    return DrawResult::kSuccess;
}

DrawResult TileBasedEngine::DrawLineString(const double* x, const double* y, int count,
                                            const DrawStyle& style) {
    if (!m_active) return DrawResult::kInvalidState;
    if (!x || !y || count < 2) return DrawResult::kInvalidParameter;

    for (int i = 0; i < count - 1; ++i) {
        double x1[] = { x[i] };
        double y1[] = { y[i] };
        double x2[] = { x[i + 1] };
        double y2[] = { y[i + 1] };
        DrawLines(x1, y1, x2, y2, 1, style);
    }

    return DrawResult::kSuccess;
}

DrawResult TileBasedEngine::DrawPolygon(const double* x, const double* y, int count,
                                         const DrawStyle& style, bool fill) {
    if (!m_active) return DrawResult::kInvalidState;
    if (!x || !y || count < 3) return DrawResult::kInvalidParameter;

    double minX = x[0], maxX = x[0];
    double minY = y[0], maxY = y[0];
    for (int i = 1; i < count; ++i) {
        minX = std::min(minX, x[i]);
        maxX = std::max(maxX, x[i]);
        minY = std::min(minY, y[i]);
        maxY = std::max(maxY, y[i]);
    }

    Point topLeft = m_transform.TransformPoint(Point(minX, minY));
    Point bottomRight = m_transform.TransformPoint(Point(maxX, maxY));

    std::vector<TileInfo> tiles = m_device->GetTilesInRect(
        static_cast<int>(std::min(topLeft.x, bottomRight.x)),
        static_cast<int>(std::min(topLeft.y, bottomRight.y)),
        static_cast<int>(std::abs(bottomRight.x - topLeft.x) + 1),
        static_cast<int>(std::abs(bottomRight.y - topLeft.y) + 1));

    for (const auto& tile : tiles) {
        BeginTile(tile);
        if (m_tileEngine) {
            std::vector<double> localX(count), localY(count);
            for (int i = 0; i < count; ++i) {
                localX[i] = x[i] - tile.pixelX;
                localY[i] = y[i] - tile.pixelY;
            }
            m_tileEngine->DrawPolygon(localX.data(), localY.data(), count, style, fill);
        }
        EndTile(tile);
    }

    return DrawResult::kSuccess;
}

DrawResult TileBasedEngine::DrawRect(double x, double y, double w, double h,
                                      const DrawStyle& style, bool fill) {
    double coords[] = { x, x + w, x + w, x };
    double ys[] = { y, y, y + h, y + h };
    return DrawPolygon(coords, ys, 4, style, fill);
}

DrawResult TileBasedEngine::DrawCircle(double cx, double cy, double radius,
                                        const DrawStyle& style, bool fill) {
    if (!m_active) return DrawResult::kInvalidState;
    if (radius <= 0) return DrawResult::kInvalidParameter;

    Point center = m_transform.TransformPoint(Point(cx, cy));

    std::vector<TileInfo> tiles = m_device->GetTilesInRect(
        static_cast<int>(center.x - radius),
        static_cast<int>(center.y - radius),
        static_cast<int>(radius * 2 + 1),
        static_cast<int>(radius * 2 + 1));

    for (const auto& tile : tiles) {
        BeginTile(tile);
        if (m_tileEngine) {
            double localCx = cx - tile.pixelX;
            double localCy = cy - tile.pixelY;
            m_tileEngine->DrawCircle(localCx, localCy, radius, style, fill);
        }
        EndTile(tile);
    }

    return DrawResult::kSuccess;
}

DrawResult TileBasedEngine::DrawEllipse(double cx, double cy, double rx, double ry,
                                         const DrawStyle& style, bool fill) {
    if (!m_active) return DrawResult::kInvalidState;
    if (rx <= 0 || ry <= 0) return DrawResult::kInvalidParameter;

    Point center = m_transform.TransformPoint(Point(cx, cy));

    std::vector<TileInfo> tiles = m_device->GetTilesInRect(
        static_cast<int>(center.x - rx),
        static_cast<int>(center.y - ry),
        static_cast<int>(rx * 2 + 1),
        static_cast<int>(ry * 2 + 1));

    for (const auto& tile : tiles) {
        BeginTile(tile);
        if (m_tileEngine) {
            double localCx = cx - tile.pixelX;
            double localCy = cy - tile.pixelY;
            m_tileEngine->DrawEllipse(localCx, localCy, rx, ry, style, fill);
        }
        EndTile(tile);
    }

    return DrawResult::kSuccess;
}

DrawResult TileBasedEngine::DrawArc(double cx, double cy, double rx, double ry,
                                     double startAngle, double sweepAngle,
                                     const DrawStyle& style) {
    if (!m_active) return DrawResult::kInvalidState;
    if (rx <= 0 || ry <= 0) return DrawResult::kInvalidParameter;

    Point center = m_transform.TransformPoint(Point(cx, cy));

    std::vector<TileInfo> tiles = m_device->GetTilesInRect(
        static_cast<int>(center.x - rx),
        static_cast<int>(center.y - ry),
        static_cast<int>(rx * 2 + 1),
        static_cast<int>(ry * 2 + 1));

    for (const auto& tile : tiles) {
        BeginTile(tile);
        if (m_tileEngine) {
            double localCx = cx - tile.pixelX;
            double localCy = cy - tile.pixelY;
            m_tileEngine->DrawArc(localCx, localCy, rx, ry, startAngle, sweepAngle, style);
        }
        EndTile(tile);
    }

    return DrawResult::kSuccess;
}

DrawResult TileBasedEngine::DrawText(double x, double y, const std::string& text,
                                      const Font& font, const Color& color) {
    if (!m_active) return DrawResult::kInvalidState;
    (void)text;
    (void)font;
    (void)color;
    return DrawResult::kNotImplemented;
}

DrawResult TileBasedEngine::DrawImage(double x, double y, const Image& image,
                                       double scaleX, double scaleY) {
    if (!m_active) return DrawResult::kInvalidState;
    if (!image.IsValid()) return DrawResult::kInvalidParameter;

    double w = image.GetWidth() * scaleX;
    double h = image.GetHeight() * scaleY;

    Point topLeft = m_transform.TransformPoint(Point(x, y));
    Point bottomRight = m_transform.TransformPoint(Point(x + w, y + h));

    std::vector<TileInfo> tiles = m_device->GetTilesInRect(
        static_cast<int>(std::min(topLeft.x, bottomRight.x)),
        static_cast<int>(std::min(topLeft.y, bottomRight.y)),
        static_cast<int>(std::abs(bottomRight.x - topLeft.x) + 1),
        static_cast<int>(std::abs(bottomRight.y - topLeft.y) + 1));

    for (const auto& tile : tiles) {
        BeginTile(tile);
        if (m_tileEngine) {
            double localX = x - tile.pixelX;
            double localY = y - tile.pixelY;
            m_tileEngine->DrawImage(localX, localY, image, scaleX, scaleY);
        }
        EndTile(tile);
    }

    return DrawResult::kSuccess;
}

DrawResult TileBasedEngine::DrawImageRect(double x, double y, double w, double h,
                                           const Image& image) {
    if (!image.IsValid()) return DrawResult::kInvalidParameter;
    double scaleX = w / image.GetWidth();
    double scaleY = h / image.GetHeight();
    return DrawImage(x, y, image, scaleX, scaleY);
}

DrawResult TileBasedEngine::DrawGeometry(const Geometry* geometry, const DrawStyle& style) {
    if (!m_active) return DrawResult::kInvalidState;
    if (!geometry) return DrawResult::kInvalidParameter;

    Envelope env = geometry->GetEnvelope();
    Rect bounds(env.GetMinX(), env.GetMinY(), env.GetWidth(), env.GetHeight());
    std::vector<TileInfo> tiles = CalculateInvolvedTiles(bounds);

    for (const auto& tile : tiles) {
        BeginTile(tile);
        if (m_tileEngine) {
            m_tileEngine->DrawGeometry(geometry, style);
        }
        EndTile(tile);
    }

    return DrawResult::kSuccess;
}

void TileBasedEngine::SetTransform(const TransformMatrix& matrix) {
    m_transform = matrix;
}

void TileBasedEngine::ResetTransform() {
    m_transform = TransformMatrix::Identity();
}

void TileBasedEngine::SetClipRect(double x, double y, double w, double h) {
    m_clipRegion = Region(Rect(x, y, w, h));
}

void TileBasedEngine::SetClipRegion(const Region& region) {
    m_clipRegion = region;
}

void TileBasedEngine::ResetClip() {
    m_clipRegion.Clear();
}

void TileBasedEngine::Save(StateFlags flags) {
    (void)flags;
}

void TileBasedEngine::Restore() {
}

void TileBasedEngine::SetOpacity(double opacity) {
    m_opacity = std::max(0.0, std::min(1.0, opacity));
}

TextMetrics TileBasedEngine::MeasureText(const std::string& text, const Font& font) {
    (void)text;
    TextMetrics metrics;
    double fontSize = font.GetSize();
    metrics.width = text.length() * fontSize * 0.6;
    metrics.height = fontSize;
    metrics.ascent = fontSize * 0.8;
    metrics.descent = fontSize * 0.2;
    metrics.lineHeight = fontSize * 1.2;
    return metrics;
}

void TileBasedEngine::Clear(const Color& color) {
    if (m_device) {
        m_device->Clear(color);
    }
}

void TileBasedEngine::Flush() {
}

}  
}  
