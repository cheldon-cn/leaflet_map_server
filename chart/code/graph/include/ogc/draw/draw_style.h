#ifndef OGC_DRAW_DRAW_STYLE_H
#define OGC_DRAW_DRAW_STYLE_H

#include <cstdint>
#include <string>

namespace ogc {
namespace draw {

enum class LineCapStyle {
    kButt = 0,
    kRound = 1,
    kSquare = 2
};

enum class LineJoinStyle {
    kMiter = 0,
    kRound = 1,
    kBevel = 2
};

struct StrokeStyle {
    uint32_t color;
    double width;
    LineCapStyle cap;
    LineJoinStyle join;
    double miter_limit;
    double* dash_array;
    int dash_count;
    double dash_offset;
    bool visible;

    StrokeStyle();
    StrokeStyle(uint32_t c, double w);
    ~StrokeStyle();
    
    void SetColor(uint32_t c);
    void SetWidth(double w);
    void SetDashArray(const double* dashes, int count);
    void ClearDashArray();
    void Reset();
};

struct FillStyle {
    uint32_t color;
    double opacity;
    bool visible;
    uint32_t* gradient_colors;
    double* gradient_stops;
    int gradient_count;
    bool has_gradient;

    FillStyle();
    FillStyle(uint32_t c);
    ~FillStyle();
    
    void SetColor(uint32_t c);
    void SetOpacity(double op);
    void SetGradient(const uint32_t* colors, const double* stops, int count);
    void ClearGradient();
    void Reset();
};

struct DrawStyle {
    StrokeStyle stroke;
    FillStyle fill;
    double opacity;
    std::string name;
    int z_order;
    bool enabled;

    DrawStyle();
    DrawStyle(const StrokeStyle& s, const FillStyle& f);
    
    void SetStroke(const StrokeStyle& s);
    void SetFill(const FillStyle& f);
    void SetOpacity(double op);
    void SetZOrder(int order);
    void SetName(const std::string& n);
    
    bool HasStroke() const;
    bool HasFill() const;
    bool IsVisible() const;
    
    void Reset();
};

}  
}  

#endif  
