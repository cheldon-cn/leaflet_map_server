#include "ogc/draw/region.h"
#include <algorithm>

namespace ogc {
namespace draw {

Region::Region() {}

Region::Region(const Rect& rect) {
    if (!rect.IsEmpty()) {
        m_rects.push_back(rect);
    }
}

Region::Region(const Region& other) 
    : m_rects(other.m_rects) {}

Region::Region(Region&& other) noexcept
    : m_rects(std::move(other.m_rects)) {}

Region::~Region() {}

Region& Region::operator=(const Region& other) {
    if (this != &other) {
        m_rects = other.m_rects;
    }
    return *this;
}

Region& Region::operator=(Region&& other) noexcept {
    if (this != &other) {
        m_rects = std::move(other.m_rects);
    }
    return *this;
}

Rect Region::GetBounds() const {
    if (m_rects.empty()) {
        return Rect();
    }
    
    double minX = m_rects[0].x;
    double minY = m_rects[0].y;
    double maxX = minX + m_rects[0].w;
    double maxY = minY + m_rects[0].h;
    
    for (const auto& rect : m_rects) {
        minX = std::min(minX, rect.x);
        minY = std::min(minY, rect.y);
        maxX = std::max(maxX, rect.x + rect.w);
        maxY = std::max(maxY, rect.y + rect.h);
    }
    
    return Rect(minX, minY, maxX - minX, maxY - minY);
}

void Region::AddRect(const Rect& rect) {
    if (!rect.IsEmpty()) {
        m_rects.push_back(rect);
    }
}

void Region::RemoveRect(const Rect& rect) {
    m_rects.erase(
        std::remove_if(m_rects.begin(), m_rects.end(),
            [&rect](const Rect& r) { return r == rect; }),
        m_rects.end());
}

void Region::Clear() {
    m_rects.clear();
}

bool Region::Contains(const Point& point) const {
    return Contains(point.x, point.y);
}

bool Region::Contains(double x, double y) const {
    for (const auto& rect : m_rects) {
        if (rect.Contains(Point(x, y))) {
            return true;
        }
    }
    return false;
}

bool Region::Intersects(const Rect& rect) const {
    for (const auto& r : m_rects) {
        if (r.Intersects(rect)) {
            return true;
        }
    }
    return false;
}

Region Region::United(const Region& other) const {
    Region result = *this;
    for (const auto& rect : other.m_rects) {
        result.AddRect(rect);
    }
    return result;
}

Region Region::Intersected(const Region& other) const {
    Region result;
    for (const auto& r1 : m_rects) {
        for (const auto& r2 : other.m_rects) {
            if (r1.Intersects(r2)) {
                Rect intersection = r1.Intersect(r2);
                if (!intersection.IsEmpty()) {
                    result.AddRect(intersection);
                }
            }
        }
    }
    return result;
}

Region Region::Subtracted(const Region& other) const {
    (void)other;
    return *this;
}

Region Region::Xored(const Region& other) const {
    (void)other;
    return *this;
}

Region Region::Translated(double dx, double dy) const {
    Region result;
    for (const auto& rect : m_rects) {
        result.AddRect(Rect(rect.x + dx, rect.y + dy, rect.w, rect.h));
    }
    return result;
}

void Region::Translate(double dx, double dy) {
    for (auto& rect : m_rects) {
        rect.x += dx;
        rect.y += dy;
    }
}

bool Region::operator==(const Region& other) const {
    return m_rects == other.m_rects;
}

bool Region::operator!=(const Region& other) const {
    return !(*this == other);
}

Region Region::operator|(const Region& other) const {
    return United(other);
}

Region Region::operator&(const Region& other) const {
    return Intersected(other);
}

Region Region::operator-(const Region& other) const {
    return Subtracted(other);
}

Region Region::operator^(const Region& other) const {
    return Xored(other);
}

Region& Region::operator|=(const Region& other) {
    for (const auto& rect : other.m_rects) {
        AddRect(rect);
    }
    return *this;
}

Region& Region::operator&=(const Region& other) {
    *this = Intersected(other);
    return *this;
}

Region& Region::operator-=(const Region& other) {
    *this = Subtracted(other);
    return *this;
}

Region& Region::operator^=(const Region& other) {
    *this = Xored(other);
    return *this;
}

Region Region::Empty() {
    return Region();
}

Region Region::FromRect(const Rect& rect) {
    return Region(rect);
}

Region Region::FromRects(const std::vector<Rect>& rects) {
    Region result;
    for (const auto& rect : rects) {
        result.AddRect(rect);
    }
    return result;
}

}  
}  
