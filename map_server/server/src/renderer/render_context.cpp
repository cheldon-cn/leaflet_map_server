#include "render_context.h"
#include "../utils/logger.h"
#include <cmath>
#include <algorithm>

namespace cycle {
namespace renderer {

RenderContext::RenderContext(int width, int height, const BoundingBox& bounds)
    : width_(width)
    , height_(height)
    , bounds_(bounds)
    , transform_(Transform2D::Identity())
    , currentStyle_() {
    
    if (width_ <= 0 || height_ <= 0) {
        LOG_ERROR("Invalid render context dimensions: " + 
                  std::to_string(width_) + "x" + std::to_string(height_));
        return;
    }
    
    if (!bounds_.IsValid()) {
        LOG_ERROR("Invalid render context bounds");
        return;
    }
    
    CalculateWorldToScreenTransform();
    
    LOG_DEBUG("RenderContext created: " + std::to_string(width_) + "x" + 
              std::to_string(height_) + " bounds: [" + 
              std::to_string(bounds_.minX) + "," + std::to_string(bounds_.minY) + 
              "]-[" + std::to_string(bounds_.maxX) + "," + 
              std::to_string(bounds_.maxY) + "]");
}

RenderContext::~RenderContext() {
    LOG_DEBUG("RenderContext destroyed");
}

void RenderContext::PushState() {
    transformStack_.push(transform_);
    styleStack_.push(currentStyle_);
    
    LOG_TRACE("RenderContext state pushed");
}

void RenderContext::PopState() {
    if (transformStack_.empty() || styleStack_.empty()) {
        LOG_WARN("Cannot pop state - stack is empty");
        return;
    }
    
    transform_ = transformStack_.top();
    transformStack_.pop();
    
    currentStyle_ = styleStack_.top();
    styleStack_.pop();
    
    LOG_TRACE("RenderContext state popped");
}

void RenderContext::SetTransform(const Transform2D& transform) {
    transform_ = transform;
    LOG_TRACE("RenderContext transform set");
}

Transform2D RenderContext::GetTransform() const {
    return transform_;
}

PointD RenderContext::ToScreen(double x, double y) const {
    if (!IsValid()) {
        LOG_ERROR("Cannot convert to screen - invalid context");
        return PointD(0, 0);
    }
    
    PointD worldPoint = transform_.Map(x, y);
    PointD screenPoint = worldToScreen_.Map(worldPoint.x, worldPoint.y);
    
    return screenPoint;
}

PointD RenderContext::ToWorld(int x, int y) const {
    if (!IsValid()) {
        LOG_ERROR("Cannot convert to world - invalid context");
        return PointD(0, 0);
    }
    
    PointD screenPoint(static_cast<double>(x), static_cast<double>(y));
    PointD worldPoint = screenToWorld_.Map(screenPoint.x, screenPoint.y);
    
    return worldPoint;
}

void RenderContext::SetStyle(const Style& style) {
    if (!style.IsValid()) {
        LOG_WARN("Invalid style provided");
        return;
    }
    
    currentStyle_ = style;
    LOG_TRACE("RenderContext style set");
}

const Style& RenderContext::GetStyle() const {
    return currentStyle_;
}

bool RenderContext::IsValid() const {
    return width_ > 0 && height_ > 0 && bounds_.IsValid();
}

bool RenderContext::ValidatePoint(double x, double y) const {
    if (!IsValid()) {
        return false;
    }
    
    return x >= bounds_.minX && x <= bounds_.maxX &&
           y >= bounds_.minY && y <= bounds_.maxY;
}

bool RenderContext::ValidateScreenPoint(int x, int y) const {
    if (!IsValid()) {
        return false;
    }
    
    return x >= 0 && x < width_ && y >= 0 && y < height_;
}

void RenderContext::CalculateWorldToScreenTransform() {
    if (!IsValid()) {
        return;
    }
    
    double worldWidth = bounds_.maxX - bounds_.minX;
    double worldHeight = bounds_.maxY - bounds_.minY;
    
    if (worldWidth <= 0 || worldHeight <= 0) {
        LOG_ERROR("Invalid world dimensions for transform calculation");
        return;
    }
    
    double scaleX = static_cast<double>(width_) / worldWidth;
    double scaleY = static_cast<double>(height_) / worldHeight;
    
    worldToScreen_ = Transform2D::Scale(scaleX, -scaleY) *
                     Transform2D::Translate(-bounds_.minX, -bounds_.maxY);
    
    double invScaleX = 1.0 / scaleX;
    double invScaleY = 1.0 / scaleY;
    
    screenToWorld_ = Transform2D::Translate(bounds_.minX, bounds_.maxY) *
                     Transform2D::Scale(invScaleX, -invScaleY);
    
    LOG_DEBUG("World to screen transform calculated: scale=" + 
              std::to_string(scaleX) + "," + std::to_string(scaleY));
}

} // namespace renderer
} // namespace cycle