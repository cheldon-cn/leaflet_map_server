#ifndef OGC_DRAW_TEXT_METRICS_H
#define OGC_DRAW_TEXT_METRICS_H

#include "ogc/draw/export.h"

namespace ogc {
namespace draw {

struct TextMetrics {
    double width;
    double height;
    double ascent;
    double descent;
    double lineHeight;
    double advanceX;
    double advanceY;

    TextMetrics() 
        : width(0), height(0), ascent(0), descent(0), 
          lineHeight(0), advanceX(0), advanceY(0) {}

    TextMetrics(double w, double h)
        : width(w), height(h), ascent(h * 0.8), descent(h * 0.2),
          lineHeight(h * 1.2), advanceX(w), advanceY(0) {}

    TextMetrics(double w, double h, double a, double d, double lh)
        : width(w), height(h), ascent(a), descent(d),
          lineHeight(lh), advanceX(w), advanceY(0) {}

    bool IsValid() const {
        return width > 0 || height > 0;
    }

    bool IsEmpty() const {
        return width == 0 && height == 0;
    }

    double GetBaselineOffset() const {
        return ascent;
    }

    double GetCenterOffset() const {
        return (ascent - descent) / 2.0;
    }
};

}  
}  

#endif  
