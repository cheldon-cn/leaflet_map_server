#include "ogc/draw/qt_engine.h"

#ifdef DRAW_HAS_QT

#include <QImage>
#include <QPixmap>
#include <QPainterPath>
#include <QDebug>
#include <QTransform>

namespace ogc {
namespace draw {

QtEngine::QtEngine(DrawDevice* device)
    : Simple2DEngine(device)
    , m_painter(nullptr)
    , m_qtDevice(nullptr)
    , m_isActive(false)
{
}

QtEngine::~QtEngine() {
    if (m_isActive) {
        End();
    }
    CleanupPainter();
}

bool QtEngine::IsQtAvailable() {
    return true;
}

bool QtEngine::Begin() {
    if (m_isActive) {
        return true;
    }
    
    if (!InitializePainter()) {
        return false;
    }
    
    m_isActive = true;
    return true;
}

void QtEngine::End() {
    if (m_painter && m_painter->isActive()) {
        m_painter->end();
    }
    m_isActive = false;
}

DrawResult QtEngine::DrawPoints(const double* x, const double* y, int count, const DrawStyle& style) {
    if (!m_painter || !m_painter->isActive()) {
        return DrawResult::kInvalidState;
    }
    
    if (!x || !y || count <= 0) {
        return DrawResult::kInvalidParameter;
    }
    
    ApplyStyle(style);
    
    m_painter->setBrush(Qt::NoBrush);
    
    double pointSize = style.pointSize > 0 ? style.pointSize : 3.0;
    
    for (int i = 0; i < count; ++i) {
        QPointF point = ToQPointF(x[i], y[i]);
        m_painter->drawEllipse(point, pointSize, pointSize);
    }
    
    return DrawResult::kSuccess;
}

DrawResult QtEngine::DrawLines(const double* x1, const double* y1, const double* x2, const double* y2, int count, const DrawStyle& style) {
    if (!m_painter || !m_painter->isActive()) {
        return DrawResult::kInvalidState;
    }
    
    if (!x1 || !y1 || !x2 || !y2 || count <= 0) {
        return DrawResult::kInvalidParameter;
    }
    
    ApplyStyle(style);
    m_painter->setBrush(Qt::NoBrush);
    
    for (int i = 0; i < count; ++i) {
        QPointF p1 = ToQPointF(x1[i], y1[i]);
        QPointF p2 = ToQPointF(x2[i], y2[i]);
        m_painter->drawLine(p1, p2);
    }
    
    return DrawResult::kSuccess;
}

DrawResult QtEngine::DrawPolygon(const double* x, const double* y, int count, const DrawStyle& style, bool fill) {
    if (!m_painter || !m_painter->isActive()) {
        return DrawResult::kInvalidState;
    }
    
    if (!x || !y || count < 3) {
        return DrawResult::kInvalidParameter;
    }
    
    ApplyStyle(style);
    
    QPolygonF polygon = ToQPolygonF(x, y, count);
    
    if (fill) {
        m_painter->drawPolygon(polygon);
    } else {
        m_painter->setBrush(Qt::NoBrush);
        m_painter->drawPolyline(polygon);
    }
    
    return DrawResult::kSuccess;
}

DrawResult QtEngine::DrawText(double x, double y, const std::string& text, const Font& font, const Color& color) {
    if (!m_painter || !m_painter->isActive()) {
        return DrawResult::kInvalidState;
    }
    
    if (text.empty()) {
        return DrawResult::kInvalidParameter;
    }
    
    QFont qFont = CreateFont(font);
    QColor qColor = ToQColor(color);
    
    m_painter->setFont(qFont);
    m_painter->setPen(qColor);
    
    QPointF point = ToQPointF(x, y);
    QString qText = QString::fromUtf8(text.c_str());
    
    m_painter->drawText(point, qText);
    
    return DrawResult::kSuccess;
}

DrawResult QtEngine::DrawImage(double x, double y, const Image& image, double scaleX, double scaleY) {
    if (!m_painter || !m_painter->isActive()) {
        return DrawResult::kInvalidState;
    }
    
    return DrawResult::kNotImplemented;
}

void QtEngine::SetTransform(const TransformMatrix& matrix) {
    if (!m_painter || !m_painter->isActive()) {
        return;
    }
    
    QTransform qTransform;
    qTransform.setMatrix(
        matrix.m11, matrix.m12, matrix.m13,
        matrix.m21, matrix.m22, matrix.m23,
        matrix.m31, matrix.m32, matrix.m33
    );
    
    m_painter->setTransform(qTransform);
    Simple2DEngine::SetTransform(matrix);
}

void QtEngine::SetClipRegion(const Region& region) {
    if (!m_painter || !m_painter->isActive()) {
        return;
    }
    
    QPainterPath path;
    path.addRect(QRectF(region.x, region.y, region.width, region.height));
    m_painter->setClipPath(path);
    Simple2DEngine::SetClipRegion(region);
}

void QtEngine::Save(StateFlags flags) {
    if (!m_painter || !m_painter->isActive()) {
        return;
    }
    
    m_painter->save();
    m_stateStack.push(static_cast<int>(flags));
    Simple2DEngine::Save(flags);
}

void QtEngine::Restore() {
    if (!m_painter || !m_painter->isActive()) {
        return;
    }
    
    if (!m_stateStack.empty()) {
        m_stateStack.pop();
        m_painter->restore();
        Simple2DEngine::Restore();
    }
}

void QtEngine::ApplyStyle(const DrawStyle& style) {
    m_pen = CreatePen(style);
    m_brush = CreateBrush(style);
    
    m_painter->setPen(m_pen);
    
    if (style.fill) {
        m_painter->setBrush(m_brush);
    } else {
        m_painter->setBrush(Qt::NoBrush);
    }
}

QPen QtEngine::CreatePen(const DrawStyle& style) {
    QPen pen;
    
    QColor color = ToQColor(style.lineColor);
    pen.setColor(color);
    
    if (style.lineWidth > 0) {
        pen.setWidthF(style.lineWidth);
    } else {
        pen.setWidthF(1.0);
    }
    
    switch (style.lineStyle) {
        case LineStyle::kSolid:
            pen.setStyle(Qt::SolidLine);
            break;
        case LineStyle::kDash:
            pen.setStyle(Qt::DashLine);
            break;
        case LineStyle::kDot:
            pen.setStyle(Qt::DotLine);
            break;
        case LineStyle::kDashDot:
            pen.setStyle(Qt::DashDotLine);
            break;
        case LineStyle::kDashDotDot:
            pen.setStyle(Qt::DashDotDotLine);
            break;
        default:
            pen.setStyle(Qt::SolidLine);
            break;
    }
    
    return pen;
}

QBrush QtEngine::CreateBrush(const DrawStyle& style) {
    QBrush brush;
    
    QColor color = ToQColor(style.fillColor);
    brush.setColor(color);
    brush.setStyle(Qt::SolidPattern);
    
    return brush;
}

QFont QtEngine::CreateFont(const Font& font) {
    QFont qFont;
    
    if (!font.family.empty()) {
        qFont.setFamily(QString::fromUtf8(font.family.c_str()));
    }
    
    if (font.size > 0) {
        qFont.setPointSizeF(font.size);
    } else {
        qFont.setPointSizeF(12.0);
    }
    
    qFont.setBold(font.bold);
    qFont.setItalic(font.italic);
    qFont.setUnderline(font.underline);
    
    return qFont;
}

QColor QtEngine::ToQColor(const Color& color) {
    return QColor(
        static_cast<int>(color.r * 255),
        static_cast<int>(color.g * 255),
        static_cast<int>(color.b * 255),
        static_cast<int>(color.a * 255)
    );
}

QPointF QtEngine::ToQPointF(double x, double y) {
    return QPointF(x, y);
}

QPolygonF QtEngine::ToQPolygonF(const double* x, const double* y, int count) {
    QPolygonF polygon;
    for (int i = 0; i < count; ++i) {
        polygon.append(QPointF(x[i], y[i]));
    }
    return polygon;
}

bool QtEngine::InitializePainter() {
    return false;
}

void QtEngine::CleanupPainter() {
    if (m_painter) {
        if (m_painter->isActive()) {
            m_painter->end();
        }
        delete m_painter;
        m_painter = nullptr;
    }
    m_qtDevice = nullptr;
}

}  
}  

#endif
