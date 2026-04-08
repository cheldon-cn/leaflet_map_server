#ifndef OGC_DRAW_QT_ENGINE_H
#define OGC_DRAW_QT_ENGINE_H

#include "ogc/draw/export.h"
#include "ogc/draw/simple2d_engine.h"

#ifdef DRAW_HAS_QT

#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QColor>
#include <QPaintDevice>
#include <stack>

namespace ogc {
namespace draw {

class OGC_DRAW_API QtEngine : public Simple2DEngine {
public:
    explicit QtEngine(DrawDevice* device);
    ~QtEngine() override;
    
    std::string GetName() const override { return "QtEngine"; }
    EngineType GetType() const override { return EngineType::kSimple2D; }
    
    bool Begin() override;
    void End() override;
    
    DrawResult DrawPoints(const double* x, const double* y, int count, const DrawStyle& style) override;
    DrawResult DrawLines(const double* x1, const double* y1, const double* x2, const double* y2, int count, const DrawStyle& style) override;
    DrawResult DrawPolygon(const double* x, const double* y, int count, const DrawStyle& style, bool fill = true) override;
    DrawResult DrawText(double x, double y, const std::string& text, const Font& font, const Color& color) override;
    DrawResult DrawImage(double x, double y, const Image& image, double scaleX = 1.0, double scaleY = 1.0) override;
    
    void SetTransform(const TransformMatrix& matrix) override;
    void SetClipRegion(const Region& region) override;
    void Save(StateFlags flags = StateFlag::kAll) override;
    void Restore() override;
    
    QPainter* GetPainter() const { return m_painter; }
    QPaintDevice* GetQtDevice() const { return m_qtDevice; }
    
    static bool IsQtAvailable();

protected:
    void ApplyStyle(const DrawStyle& style);
    QPen CreatePen(const DrawStyle& style);
    QBrush CreateBrush(const DrawStyle& style);
    QFont CreateFont(const Font& font);
    QColor ToQColor(const Color& color);
    QPointF ToQPointF(double x, double y);
    QPolygonF ToQPolygonF(const double* x, const double* y, int count);
    
    bool InitializePainter();
    void CleanupPainter();

private:
    QPainter* m_painter = nullptr;
    QPaintDevice* m_qtDevice = nullptr;
    QPen m_pen;
    QBrush m_brush;
    QFont m_font;
    std::stack<int> m_stateStack;
    bool m_isActive = false;
};

}  
}  

#endif

#endif
