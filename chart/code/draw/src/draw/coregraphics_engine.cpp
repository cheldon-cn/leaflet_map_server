#ifdef __APPLE__

#include "ogc/draw/coregraphics_engine.h"
#include "ogc/draw/raster_image_device.h"
#include <cmath>

namespace ogc {
namespace draw {

CoreGraphicsEngine::CoreGraphicsEngine(DrawDevice* device) 
    : Simple2DEngine(device) {
}

CoreGraphicsEngine::~CoreGraphicsEngine() {
    if (m_context && m_ownsContext) {
        CGContextRelease(m_context);
        m_context = nullptr;
    }
}

bool CoreGraphicsEngine::Begin() {
    if (!m_device) {
        return false;
    }
    
    int width = m_device->GetWidth();
    int height = m_device->GetHeight();
    
    if (width <= 0 || height <= 0) {
        return false;
    }
    
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    if (!colorSpace) {
        return false;
    }
    
    size_t bytesPerRow = width * 4;
    void* data = calloc(height, bytesPerRow);
    if (!data) {
        CGColorSpaceRelease(colorSpace);
        return false;
    }
    
    CGContextRef context = CGBitmapContextCreate(
        data,
        width,
        height,
        8,
        bytesPerRow,
        colorSpace,
        kCGImageAlphaPremultipliedLast
    );
    
    CGColorSpaceRelease(colorSpace);
    
    if (!context) {
        free(data);
        return false;
    }
    
    CGContextSetRGBFillColor(context, 1.0, 1.0, 1.0, 1.0);
    CGContextFillRect(context, CGRectMake(0, 0, width, height));
    
    CGContextTranslateCTM(context, 0, height);
    CGContextScaleCTM(context, 1.0, -1.0);
    
    if (m_context && m_ownsContext) {
        CGContextRelease(m_context);
    }
    
    m_context = context;
    m_ownsContext = true;
    
    return true;
}

void CoreGraphicsEngine::End() {
    if (m_context && m_ownsContext) {
        CGContextRelease(m_context);
        m_context = nullptr;
        m_ownsContext = false;
    }
}

void CoreGraphicsEngine::SetContext(CGContextRef context) {
    if (m_context && m_ownsContext) {
        CGContextRelease(m_context);
    }
    m_context = context;
    m_ownsContext = false;
}

void CoreGraphicsEngine::SetStrokeColor(const Color& color) {
    CGContextSetRGBStrokeColor(m_context, 
                               color.r / 255.0, 
                               color.g / 255.0, 
                               color.b / 255.0, 
                               color.a / 255.0);
}

void CoreGraphicsEngine::SetFillColor(const Color& color) {
    CGContextSetRGBFillColor(m_context, 
                            color.r / 255.0, 
                            color.g / 255.0, 
                            color.b / 255.0, 
                            color.a / 255.0);
}

void CoreGraphicsEngine::ApplyPen(const Pen& pen) {
    SetStrokeColor(pen.color);
    CGContextSetLineWidth(m_context, pen.width);
    
    switch (pen.style) {
        case PenStyle::Solid:
            CGContextSetLineDash(m_context, 0, nullptr, 0);
            break;
        case PenStyle::Dash:
            {
                CGFloat dashes[] = {4.0, 2.0};
                CGContextSetLineDash(m_context, 0, dashes, 2);
            }
            break;
        case PenStyle::Dot:
            {
                CGFloat dashes[] = {1.0, 1.0};
                CGContextSetLineDash(m_context, 0, dashes, 2);
            }
            break;
        case PenStyle::DashDot:
            {
                CGFloat dashes[] = {4.0, 2.0, 1.0, 2.0};
                CGContextSetLineDash(m_context, 0, dashes, 4);
            }
            break;
        case PenStyle::DashDotDot:
            {
                CGFloat dashes[] = {4.0, 2.0, 1.0, 2.0, 1.0, 2.0};
                CGContextSetLineDash(m_context, 0, dashes, 6);
            }
            break;
        case PenStyle::Null:
            CGContextSetLineWidth(m_context, 0);
            break;
    }
    
    switch (pen.cap) {
        case PenCap::Flat:
            CGContextSetLineCap(m_context, kCGLineCapButt);
            break;
        case PenCap::Round:
            CGContextSetLineCap(m_context, kCGLineCapRound);
            break;
        case PenCap::Square:
            CGContextSetLineCap(m_context, kCGLineCapSquare);
            break;
    }
    
    switch (pen.join) {
        case PenJoin::Miter:
            CGContextSetLineJoin(m_context, kCGLineJoinMiter);
            break;
        case PenJoin::Round:
            CGContextSetLineJoin(m_context, kCGLineJoinRound);
            break;
        case PenJoin::Bevel:
            CGContextSetLineJoin(m_context, kCGLineJoinBevel);
            break;
    }
}

void CoreGraphicsEngine::ApplyBrush(const Brush& brush) {
    SetFillColor(brush.color);
}

void CoreGraphicsEngine::ApplyStyle(const DrawStyle& style) {
    ApplyPen(style.pen);
}

CGAffineTransform CoreGraphicsEngine::CreateAffineTransform(const TransformMatrix& matrix) {
    CGAffineTransform transform;
    transform.a = matrix.m11;
    transform.b = matrix.m12;
    transform.c = matrix.m21;
    transform.d = matrix.m22;
    transform.tx = matrix.dx;
    transform.ty = matrix.dy;
    return transform;
}

DrawResult CoreGraphicsEngine::DrawPoints(const double* x, const double* y, int count, const DrawStyle& style) {
    if (!m_context || count <= 0) {
        return DrawResult::InvalidParameter;
    }
    
    ApplyPen(style.pen);
    SetFillColor(style.pen.color);
    
    double pointSize = style.pen.width > 0 ? style.pen.width : 1.0;
    
    for (int i = 0; i < count; ++i) {
        CGRect rect = CGRectMake(x[i] - pointSize / 2, y[i] - pointSize / 2, pointSize, pointSize);
        CGContextFillEllipseInRect(m_context, rect);
    }
    
    return DrawResult::Success;
}

DrawResult CoreGraphicsEngine::DrawLines(const double* x1, const double* y1, const double* x2, const double* y2, int count, const DrawStyle& style) {
    if (!m_context || count <= 0) {
        return DrawResult::InvalidParameter;
    }
    
    ApplyPen(style.pen);
    
    CGContextBeginPath(m_context);
    
    for (int i = 0; i < count; ++i) {
        CGContextMoveToPoint(m_context, x1[i], y1[i]);
        CGContextAddLineToPoint(m_context, x2[i], y2[i]);
    }
    
    CGContextStrokePath(m_context);
    
    return DrawResult::Success;
}

DrawResult CoreGraphicsEngine::DrawPolygon(const double* x, const double* y, int count, const DrawStyle& style, bool fill) {
    if (!m_context || count < 3) {
        return DrawResult::InvalidParameter;
    }
    
    CGContextBeginPath(m_context);
    CGContextMoveToPoint(m_context, x[0], y[0]);
    
    for (int i = 1; i < count; ++i) {
        CGContextAddLineToPoint(m_context, x[i], y[i]);
    }
    
    CGContextClosePath(m_context);
    
    if (fill && style.brush.style != BrushStyle::Null) {
        ApplyBrush(style.brush);
        
        if (style.pen.style != PenStyle::Null) {
            CGContextDrawPath(m_context, kCGPathFillStroke);
        } else {
            CGContextFillPath(m_context);
        }
    } else if (style.pen.style != PenStyle::Null) {
        ApplyPen(style.pen);
        CGContextStrokePath(m_context);
    }
    
    return DrawResult::Success;
}

DrawResult CoreGraphicsEngine::DrawText(double x, double y, const std::string& text, const Font& font, const Color& color) {
    if (!m_context || text.empty()) {
        return DrawResult::InvalidParameter;
    }
    
    SetFillColor(color);
    
    CFStringRef fontFamily = CFStringCreateWithCString(kCFAllocatorDefault, 
                                                        font.family.empty() ? "Helvetica" : font.family.c_str(),
                                                        kCFStringEncodingUTF8);
    
    CTFontSymbolicTraits traits = 0;
    if (font.bold) traits |= kCTFontBoldTrait;
    if (font.italic) traits |= kCTFontItalicTrait;
    
    CTFontDescriptorRef descriptor = CTFontDescriptorCreateWithNameAndSize(fontFamily, font.size);
    CTFontRef ctFont = CTFontCreateWithFontDescriptor(descriptor, font.size, nullptr);
    
    CFRelease(fontFamily);
    CFRelease(descriptor);
    
    CFStringRef cfText = CFStringCreateWithCString(kCFAllocatorDefault, text.c_str(), kCFStringEncodingUTF8);
    CFStringRef keys[] = { kCTFontAttributeName };
    CFTypeRef values[] = { ctFont };
    CFDictionaryRef attributes = CFDictionaryCreate(kCFAllocatorDefault, 
                                                     (const void**)keys, 
                                                     (const void**)values,
                                                     1,
                                                     &kCFTypeDictionaryKeyCallBacks,
                                                     &kCFTypeDictionaryValueCallBacks);
    
    CFAttributedStringRef attrString = CFAttributedStringCreate(kCFAllocatorDefault, cfText, attributes);
    CTLineRef line = CTLineCreateWithAttributedString(attrString);
    
    CGContextSetTextPosition(m_context, x, y);
    CTLineDraw(line, m_context);
    
    CFRelease(line);
    CFRelease(attrString);
    CFRelease(attributes);
    CFRelease(cfText);
    CFRelease(ctFont);
    
    return DrawResult::Success;
}

DrawResult CoreGraphicsEngine::DrawImage(double x, double y, const Image& image, double scaleX, double scaleY) {
    if (!m_context || image.data.empty()) {
        return DrawResult::InvalidParameter;
    }
    
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    
    CGDataProviderRef provider = CGDataProviderCreateWithData(
        nullptr,
        image.data.data(),
        image.data.size(),
        nullptr
    );
    
    CGImageRef cgImage = CGImageCreate(
        image.width,
        image.height,
        8,
        32,
        image.width * 4,
        colorSpace,
        kCGImageAlphaPremultipliedLast,
        provider,
        nullptr,
        false,
        kCGRenderingIntentDefault
    );
    
    CGColorSpaceRelease(colorSpace);
    CGDataProviderRelease(provider);
    
    if (!cgImage) {
        return DrawResult::InvalidParameter;
    }
    
    CGContextSaveGState(m_context);
    
    CGContextTranslateCTM(m_context, x, y);
    CGContextScaleCTM(m_context, scaleX, scaleY);
    
    CGRect rect = CGRectMake(0, 0, image.width, image.height);
    CGContextDrawImage(m_context, rect, cgImage);
    
    CGContextRestoreGState(m_context);
    CGImageRelease(cgImage);
    
    return DrawResult::Success;
}

void CoreGraphicsEngine::SetTransform(const TransformMatrix& matrix) {
    if (!m_context) {
        return;
    }
    
    CGAffineTransform transform = CreateAffineTransform(matrix);
    CGContextSetCTM(m_context, transform);
}

void CoreGraphicsEngine::SetClipRegion(const Region& region) {
    if (!m_context) {
        return;
    }
    
    CGContextResetClip(m_context);
    
    if (region.rects.empty()) {
        return;
    }
    
    CGMutablePathRef path = CGPathCreateMutable();
    
    for (const auto& rect : region.rects) {
        CGPathAddRect(path, nullptr, CGRectMake(rect.x, rect.y, rect.width, rect.height));
    }
    
    CGContextAddPath(m_context, path);
    CGContextClip(m_context);
    
    CGPathRelease(path);
}

}
}

#endif
