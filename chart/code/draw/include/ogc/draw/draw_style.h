#ifndef OGC_DRAW_STYLE_H
#define OGC_DRAW_STYLE_H

#include "ogc/draw/font.h"
#include "ogc/draw/color.h"
#include <string>
#include <vector>

namespace ogc {
namespace draw {

enum class PenStyle {
    kSolid = 0,
    kDash = 1,
    kDot = 2,
    kDashDot = 3,
    kDashDotDot = 4,
    kNone = 5
};

enum class LineCap {
    kFlat = 0,
    kRound = 1,
    kSquare = 2
};

enum class LineJoin {
    kMiter = 0,
    kRound = 1,
    kBevel = 2
};

enum class BrushStyle {
    kSolid = 0,
    kNone = 1,
    kHorizontal = 2,
    kVertical = 3,
    kCross = 4,
    kBDiagonal = 5,
    kFDiagonal = 6,
    kDiagonalCross = 7,
    kTexture = 8
};

enum class FillRule {
    kEvenOdd = 0,
    kNonZero = 1
};

enum class PointMarkerType {
    kCircle = 0,
    kSquare = 1,
    kTriangle = 2,
    kDiamond = 3,
    kStar = 4,
    kCross = 5,
    kX = 6
};

struct Pen {
    Color color;
    double width;
    PenStyle style;
    LineCap cap;
    LineJoin join;
    std::vector<double> dashPattern;
    double dashOffset;
    double miterLimit;

    Pen()
        : color(0, 0, 0, 255)
        , width(1.0)
        , style(PenStyle::kSolid)
        , cap(LineCap::kFlat)
        , join(LineJoin::kMiter)
        , dashOffset(0.0)
        , miterLimit(10.0) {}

    Pen(const Color& color_, double width_ = 1.0)
        : color(color_)
        , width(width_)
        , style(PenStyle::kSolid)
        , cap(LineCap::kFlat)
        , join(LineJoin::kMiter)
        , dashOffset(0.0)
        , miterLimit(10.0) {}

    static Pen Solid(const Color& color, double width = 1.0) {
        Pen pen(color, width);
        pen.style = PenStyle::kSolid;
        return pen;
    }

    static Pen Dash(const Color& color, double width = 1.0) {
        Pen pen(color, width);
        pen.style = PenStyle::kDash;
        pen.dashPattern = {4.0, 2.0};
        return pen;
    }

    static Pen Dot(const Color& color, double width = 1.0) {
        Pen pen(color, width);
        pen.style = PenStyle::kDot;
        pen.dashPattern = {1.0, 2.0};
        return pen;
    }

    static Pen NoPen() {
        Pen pen;
        pen.style = PenStyle::kNone;
        return pen;
    }

    Pen WithWidth(double w) const {
        Pen p = *this;
        p.width = w;
        return p;
    }

    Pen WithColor(const Color& c) const {
        Pen p = *this;
        p.color = c;
        return p;
    }

    Pen WithStyle(PenStyle s) const {
        Pen p = *this;
        p.style = s;
        return p;
    }

    Pen WithCap(LineCap c) const {
        Pen p = *this;
        p.cap = c;
        return p;
    }

    Pen WithJoin(LineJoin j) const {
        Pen p = *this;
        p.join = j;
        return p;
    }

    bool IsVisible() const {
        return style != PenStyle::kNone && color.GetAlpha() > 0 && width > 0;
    }
};

struct Brush {
    Color color;
    BrushStyle style;
    std::string texturePath;
    double opacity;

    Brush()
        : color(255, 255, 255, 255)
        , style(BrushStyle::kSolid)
        , opacity(1.0) {}

    explicit Brush(const Color& color_)
        : color(color_)
        , style(BrushStyle::kSolid)
        , opacity(1.0) {}

    static Brush Solid(const Color& color) {
        Brush brush(color);
        brush.style = BrushStyle::kSolid;
        return brush;
    }

    static Brush NoBrush() {
        Brush brush;
        brush.style = BrushStyle::kNone;
        return brush;
    }

    static Brush Horizontal(const Color& color) {
        Brush brush(color);
        brush.style = BrushStyle::kHorizontal;
        return brush;
    }

    static Brush Vertical(const Color& color) {
        Brush brush(color);
        brush.style = BrushStyle::kVertical;
        return brush;
    }

    static Brush Cross(const Color& color) {
        Brush brush(color);
        brush.style = BrushStyle::kCross;
        return brush;
    }

    static Brush Texture(const std::string& path) {
        Brush brush;
        brush.style = BrushStyle::kTexture;
        brush.texturePath = path;
        return brush;
    }

    Brush WithColor(const Color& c) const {
        Brush b = *this;
        b.color = c;
        return b;
    }

    Brush WithOpacity(double o) const {
        Brush b = *this;
        b.opacity = o;
        return b;
    }

    bool IsVisible() const {
        return style != BrushStyle::kNone && 
               (style == BrushStyle::kTexture || color.GetAlpha() > 0);
    }
};

struct DrawStyle {
    Pen pen;
    Brush brush;
    Font font;
    double opacity;
    FillRule fillRule;
    bool antialias;
    PointMarkerType pointMarker;
    double pointSize;

    DrawStyle()
        : opacity(1.0)
        , fillRule(FillRule::kEvenOdd)
        , antialias(true)
        , pointMarker(PointMarkerType::kCircle)
        , pointSize(5.0) {}

    static DrawStyle Default() {
        return DrawStyle();
    }

    static DrawStyle Stroke(const Color& color, double width = 1.0) {
        DrawStyle style;
        style.pen = Pen::Solid(color, width);
        style.brush = Brush::NoBrush();
        return style;
    }

    static DrawStyle Fill(const Color& color) {
        DrawStyle style;
        style.pen = Pen::NoPen();
        style.brush = Brush::Solid(color);
        return style;
    }

    static DrawStyle StrokeAndFill(const Color& strokeColor, double strokeWidth,
                                   const Color& fillColor) {
        DrawStyle style;
        style.pen = Pen::Solid(strokeColor, strokeWidth);
        style.brush = Brush::Solid(fillColor);
        return style;
    }

    DrawStyle WithPen(const Pen& p) const {
        DrawStyle s = *this;
        s.pen = p;
        return s;
    }

    DrawStyle WithBrush(const Brush& b) const {
        DrawStyle s = *this;
        s.brush = b;
        return s;
    }

    DrawStyle WithFont(const Font& f) const {
        DrawStyle s = *this;
        s.font = f;
        return s;
    }

    DrawStyle WithOpacity(double o) const {
        DrawStyle s = *this;
        s.opacity = o;
        return s;
    }

    DrawStyle WithFillRule(FillRule r) const {
        DrawStyle s = *this;
        s.fillRule = r;
        return s;
    }

    DrawStyle WithAntialias(bool a) const {
        DrawStyle s = *this;
        s.antialias = a;
        return s;
    }

    DrawStyle WithPointMarker(PointMarkerType m, double size = 5.0) const {
        DrawStyle s = *this;
        s.pointMarker = m;
        s.pointSize = size;
        return s;
    }

    bool HasStroke() const {
        return pen.IsVisible();
    }

    bool HasFill() const {
        return brush.IsVisible();
    }
};

}  
}  

#endif  
