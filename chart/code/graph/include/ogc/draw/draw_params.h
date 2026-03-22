#ifndef OGC_DRAW_DRAW_PARAMS_H
#define OGC_DRAW_DRAW_PARAMS_H

#include "ogc/envelope.h"
#include <cstdint>

namespace ogc {
namespace draw {

struct DrawParams {
    Envelope extent;
    double logical_width;
    double logical_height;
    double dpi;
    double scale;
    double rotation;
    double center_x;
    double center_y;
    int32_t pixel_width;
    int32_t pixel_height;
    double tolerance;
    bool has_background;
    uint32_t background_color;

    DrawParams();
    
    DrawParams(const Envelope& ext, int32_t width, int32_t height, double dpi_val = 96.0);
    
    void SetExtent(const Envelope& ext);
    void SetSize(int32_t width, int32_t height);
    void SetDpi(double dpi_val);
    void SetRotation(double angle);
    void SetCenter(double cx, double cy);
    void SetBackgroundColor(uint32_t color);
    
    void UpdateScale();
    void UpdateTransform();
    
    double GetPixelSizeX() const;
    double GetPixelSizeY() const;
    
    bool IsValid() const;
    bool IsEmpty() const;
    
    void Reset();
};

}  
}  

#endif  
