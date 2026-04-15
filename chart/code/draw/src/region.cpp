#include "ogc/draw/region.h"
#include <algorithm>

namespace ogc {
namespace draw {

struct Region::Impl {
    std::vector<Rect> rects;
};

Region::Region() : impl_(new Impl()) {}

Region::Region(const Rect& rect) : impl_(new Impl()) {
    if (!rect.IsEmpty()) {
        impl_->rects.push_back(rect);
    }
}

Region::Region(const Region& other) : impl_(new Impl(*other.impl_)) {}

Region::Region(Region&& other) noexcept : impl_(std::move(other.impl_)) {
    other.impl_.reset(new Impl());
}

Region::~Region() = default;

Region& Region::operator=(const Region& other) {
    if (this != &other) {
        *impl_ = *other.impl_;
    }
    return *this;
}

Region& Region::operator=(Region&& other) noexcept {
    if (this != &other) {
        impl_ = std::move(other.impl_);
        other.impl_.reset(new Impl());
    }
    return *this;
}

bool Region::IsEmpty() const { return impl_->rects.empty(); }
bool Region::IsValid() const { return true; }
size_t Region::GetRectCount() const { return impl_->rects.size(); }
std::vector<Rect> Region::GetRects() const { return impl_->rects; }

Rect Region::GetBounds() const {
    if (impl_->rects.empty()) {
        return Rect();
    }
    
    double minX = impl_->rects[0].x;
    double minY = impl_->rects[0].y;
    double maxX = minX + impl_->rects[0].w;
    double maxY = minY + impl_->rects[0].h;
    
    for (const auto& rect : impl_->rects) {
        minX = std::min(minX, rect.x);
        minY = std::min(minY, rect.y);
        maxX = std::max(maxX, rect.x + rect.w);
        maxY = std::max(maxY, rect.y + rect.h);
    }
    
    return Rect(minX, minY, maxX - minX, maxY - minY);
}

void Region::AddRect(const Rect& rect) {
    if (!rect.IsEmpty()) {
        impl_->rects.push_back(rect);
    }
}

void Region::RemoveRect(const Rect& rect) {
    impl_->rects.erase(
        std::remove_if(impl_->rects.begin(), impl_->rects.end(),
            [&rect](const Rect& r) { return r == rect; }),
        impl_->rects.end());
}

void Region::Clear() {
    impl_->rects.clear();
}

bool Region::Contains(const Point& point) const {
    return Contains(point.x, point.y);
}

bool Region::Contains(double x, double y) const {
    for (const auto& rect : impl_->rects) {
        if (rect.Contains(Point(x, y))) {
            return true;
        }
    }
    return false;
}

bool Region::Intersects(const Rect& rect) const {
    for (const auto& r : impl_->rects) {
        if (r.Intersects(rect)) {
            return true;
        }
    }
    return false;
}

Region Region::United(const Region& other) const {
    Region result = *this;
    for (const auto& rect : other.impl_->rects) {
        result.AddRect(rect);
    }
    return result;
}

Region Region::Intersected(const Region& other) const {
    Region result;
    for (const auto& r1 : impl_->rects) {
        for (const auto& r2 : other.impl_->rects) {
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
    for (const auto& rect : impl_->rects) {
        result.AddRect(Rect(rect.x + dx, rect.y + dy, rect.w, rect.h));
    }
    return result;
}

void Region::Translate(double dx, double dy) {
    for (auto& rect : impl_->rects) {
        rect.x += dx;
        rect.y += dy;
    }
}

bool Region::operator==(const Region& other) const {
    return impl_->rects == other.impl_->rects;
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
    for (const auto& rect : other.impl_->rects) {
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
