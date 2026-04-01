#ifndef OGC_DRAW_REGION_H
#define OGC_DRAW_REGION_H

#include "ogc/draw/export.h"
#include "ogc/draw/geometry_types.h"
#include <vector>

namespace ogc {
namespace draw {

class OGC_DRAW_API Region {
public:
    Region();
    explicit Region(const Rect& rect);
    Region(const Region& other);
    Region(Region&& other) noexcept;
    ~Region();
    
    Region& operator=(const Region& other);
    Region& operator=(Region&& other) noexcept;
    
    bool IsEmpty() const;
    bool IsValid() const;
    
    size_t GetRectCount() const;
    const std::vector<Rect>& GetRects() const;
    
    Rect GetBounds() const;
    
    void AddRect(const Rect& rect);
    void RemoveRect(const Rect& rect);
    void Clear();
    
    bool Contains(const Point& point) const;
    bool Contains(double x, double y) const;
    bool Intersects(const Rect& rect) const;
    
    Region United(const Region& other) const;
    Region Intersected(const Region& other) const;
    Region Subtracted(const Region& other) const;
    Region Xored(const Region& other) const;
    
    Region Translated(double dx, double dy) const;
    
    void Translate(double dx, double dy);
    
    bool operator==(const Region& other) const;
    bool operator!=(const Region& other) const;
    
    Region operator|(const Region& other) const;
    Region operator&(const Region& other) const;
    Region operator-(const Region& other) const;
    Region operator^(const Region& other) const;
    
    Region& operator|=(const Region& other);
    Region& operator&=(const Region& other);
    Region& operator-=(const Region& other);
    Region& operator^=(const Region& other);
    
    static Region Empty();
    static Region FromRect(const Rect& rect);
    static Region FromRects(const std::vector<Rect>& rects);

private:
    std::vector<Rect> m_rects;
};

inline bool Region::IsEmpty() const { return m_rects.empty(); }
inline bool Region::IsValid() const { return true; }
inline size_t Region::GetRectCount() const { return m_rects.size(); }
inline const std::vector<Rect>& Region::GetRects() const { return m_rects; }

}  
}  

#endif  
