/**
 * @file sdk_c_api_draw.cpp
 * @brief OGC Chart SDK C API - Draw Module Implementation
 * @version v1.0
 * @date 2026-04-09
 */

#include "sdk_c_api.h"

#include <ogc/draw/color.h>
#include <ogc/draw/font.h>
#include <ogc/draw/draw_style.h>
#include <ogc/draw/image.h>
#include <ogc/draw/draw_device.h>
#include <ogc/draw/draw_engine.h>
#include <ogc/draw/draw_context.h>

#include <cstring>
#include <cstdlib>
#include <memory>
#include <string>

using namespace ogc::draw;

#ifdef __cplusplus
extern "C" {
#endif

namespace {

std::string SafeString(const char* str) {
    return str ? std::string(str) : std::string();
}

char* AllocString(const std::string& str) {
    char* result = static_cast<char*>(std::malloc(str.size() + 1));
    if (result) {
        std::memcpy(result, str.c_str(), str.size() + 1);
    }
    return result;
}

}  

ogc_color_t ogc_color_from_rgb(uint8_t r, uint8_t g, uint8_t b) {
    ogc_color_t color;
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = 255;
    return color;
}

ogc_color_t ogc_color_from_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    ogc_color_t color;
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = a;
    return color;
}

ogc_color_t ogc_color_from_hex(uint32_t hex) {
    ogc_color_t color;
    color.r = static_cast<uint8_t>((hex >> 16) & 0xFF);
    color.g = static_cast<uint8_t>((hex >> 8) & 0xFF);
    color.b = static_cast<uint8_t>(hex & 0xFF);
    color.a = 255;
    return color;
}

uint32_t ogc_color_to_hex(ogc_color_t color) {
    return (static_cast<uint32_t>(color.r) << 16) |
           (static_cast<uint32_t>(color.g) << 8) |
           static_cast<uint32_t>(color.b);
}

ogc_font_t* ogc_font_create(const char* family, int size) {
    return reinterpret_cast<ogc_font_t*>(
        Font::Create(SafeString(family), size).release());
}

void ogc_font_destroy(ogc_font_t* font) {
    delete reinterpret_cast<Font*>(font);
}

const char* ogc_font_get_family(const ogc_font_t* font) {
    if (font) {
        return reinterpret_cast<const Font*>(font)->GetFamily().c_str();
    }
    return "";
}

void ogc_font_set_family(ogc_font_t* font, const char* family) {
    if (font) {
        reinterpret_cast<Font*>(font)->SetFamily(SafeString(family));
    }
}

int ogc_font_get_size(const ogc_font_t* font) {
    if (font) {
        return reinterpret_cast<const Font*>(font)->GetSize();
    }
    return 0;
}

void ogc_font_set_size(ogc_font_t* font, int size) {
    if (font) {
        reinterpret_cast<Font*>(font)->SetSize(size);
    }
}

int ogc_font_is_bold(const ogc_font_t* font) {
    if (font) {
        return reinterpret_cast<const Font*>(font)->IsBold() ? 1 : 0;
    }
    return 0;
}

void ogc_font_set_bold(ogc_font_t* font, int bold) {
    if (font) {
        reinterpret_cast<Font*>(font)->SetBold(bold != 0);
    }
}

int ogc_font_is_italic(const ogc_font_t* font) {
    if (font) {
        return reinterpret_cast<const Font*>(font)->IsItalic() ? 1 : 0;
    }
    return 0;
}

void ogc_font_set_italic(ogc_font_t* font, int italic) {
    if (font) {
        reinterpret_cast<Font*>(font)->SetItalic(italic != 0);
    }
}

ogc_pen_t* ogc_pen_create(void) {
    return reinterpret_cast<ogc_pen_t*>(Pen::Create().release());
}

void ogc_pen_destroy(ogc_pen_t* pen) {
    delete reinterpret_cast<Pen*>(pen);
}

ogc_color_t ogc_pen_get_color(const ogc_pen_t* pen) {
    ogc_color_t result = {0, 0, 0, 255};
    if (pen) {
        Color c = reinterpret_cast<const Pen*>(pen)->GetColor();
        result.r = c.GetRed();
        result.g = c.GetGreen();
        result.b = c.GetBlue();
        result.a = c.GetAlpha();
    }
    return result;
}

void ogc_pen_set_color(ogc_pen_t* pen, ogc_color_t color) {
    if (pen) {
        reinterpret_cast<Pen*>(pen)->SetColor(Color(color.r, color.g, color.b, color.a));
    }
}

double ogc_pen_get_width(const ogc_pen_t* pen) {
    if (pen) {
        return reinterpret_cast<const Pen*>(pen)->GetWidth();
    }
    return 1.0;
}

void ogc_pen_set_width(ogc_pen_t* pen, double width) {
    if (pen) {
        reinterpret_cast<Pen*>(pen)->SetWidth(width);
    }
}

ogc_pen_style_e ogc_pen_get_style(const ogc_pen_t* pen) {
    if (pen) {
        auto style = reinterpret_cast<const Pen*>(pen)->GetStyle();
        switch (style) {
            case PenStyle::kSolid: return OGC_PEN_STYLE_SOLID;
            case PenStyle::kDash: return OGC_PEN_STYLE_DASH;
            case PenStyle::kDot: return OGC_PEN_STYLE_DOT;
            case PenStyle::kDashDot: return OGC_PEN_STYLE_DASHDOT;
            case PenStyle::kDashDotDot: return OGC_PEN_STYLE_DASHDOTDOT;
            case PenStyle::kNone: return OGC_PEN_STYLE_NONE;
            default: return OGC_PEN_STYLE_SOLID;
        }
    }
    return OGC_PEN_STYLE_SOLID;
}

void ogc_pen_set_style(ogc_pen_t* pen, ogc_pen_style_e style) {
    if (pen) {
        PenStyle ps = PenStyle::kSolid;
        switch (style) {
            case OGC_PEN_STYLE_SOLID: ps = PenStyle::kSolid; break;
            case OGC_PEN_STYLE_DASH: ps = PenStyle::kDash; break;
            case OGC_PEN_STYLE_DOT: ps = PenStyle::kDot; break;
            case OGC_PEN_STYLE_DASHDOT: ps = PenStyle::kDashDot; break;
            case OGC_PEN_STYLE_DASHDOTDOT: ps = PenStyle::kDashDotDot; break;
            case OGC_PEN_STYLE_NONE: ps = PenStyle::kNone; break;
        }
        reinterpret_cast<Pen*>(pen)->SetStyle(ps);
    }
}

ogc_brush_t* ogc_brush_create(void) {
    return reinterpret_cast<ogc_brush_t*>(Brush::Create().release());
}

void ogc_brush_destroy(ogc_brush_t* brush) {
    delete reinterpret_cast<Brush*>(brush);
}

ogc_color_t ogc_brush_get_color(const ogc_brush_t* brush) {
    ogc_color_t result = {0, 0, 0, 255};
    if (brush) {
        Color c = reinterpret_cast<const Brush*>(brush)->GetColor();
        result.r = c.GetRed();
        result.g = c.GetGreen();
        result.b = c.GetBlue();
        result.a = c.GetAlpha();
    }
    return result;
}

void ogc_brush_set_color(ogc_brush_t* brush, ogc_color_t color) {
    if (brush) {
        reinterpret_cast<Brush*>(brush)->SetColor(Color(color.r, color.g, color.b, color.a));
    }
}

ogc_brush_style_e ogc_brush_get_style(const ogc_brush_t* brush) {
    if (brush) {
        auto style = reinterpret_cast<const Brush*>(brush)->GetStyle();
        switch (style) {
            case BrushStyle::kSolid: return OGC_BRUSH_STYLE_SOLID;
            case BrushStyle::kNone: return OGC_BRUSH_STYLE_NONE;
            case BrushStyle::kHorizontal: return OGC_BRUSH_STYLE_HORIZONTAL;
            case BrushStyle::kVertical: return OGC_BRUSH_STYLE_VERTICAL;
            case BrushStyle::kCross: return OGC_BRUSH_STYLE_CROSS;
            case BrushStyle::kDiagonal: return OGC_BRUSH_STYLE_DIAGONAL;
            default: return OGC_BRUSH_STYLE_SOLID;
        }
    }
    return OGC_BRUSH_STYLE_SOLID;
}

void ogc_brush_set_style(ogc_brush_t* brush, ogc_brush_style_e style) {
    if (brush) {
        BrushStyle bs = BrushStyle::kSolid;
        switch (style) {
            case OGC_BRUSH_STYLE_SOLID: bs = BrushStyle::kSolid; break;
            case OGC_BRUSH_STYLE_NONE: bs = BrushStyle::kNone; break;
            case OGC_BRUSH_STYLE_HORIZONTAL: bs = BrushStyle::kHorizontal; break;
            case OGC_BRUSH_STYLE_VERTICAL: bs = BrushStyle::kVertical; break;
            case OGC_BRUSH_STYLE_CROSS: bs = BrushStyle::kCross; break;
            case OGC_BRUSH_STYLE_DIAGONAL: bs = BrushStyle::kDiagonal; break;
        }
        reinterpret_cast<Brush*>(brush)->SetStyle(bs);
    }
}

ogc_draw_style_t* ogc_draw_style_create(void) {
    return reinterpret_cast<ogc_draw_style_t*>(DrawStyle::Create().release());
}

void ogc_draw_style_destroy(ogc_draw_style_t* style) {
    delete reinterpret_cast<DrawStyle*>(style);
}

ogc_pen_t* ogc_draw_style_get_pen(ogc_draw_style_t* style) {
    if (style) {
        return reinterpret_cast<ogc_pen_t*>(
            reinterpret_cast<DrawStyle*>(style)->GetPen());
    }
    return nullptr;
}

void ogc_draw_style_set_pen(ogc_draw_style_t* style, ogc_pen_t* pen) {
    if (style && pen) {
        reinterpret_cast<DrawStyle*>(style)->SetPen(
            PenPtr(reinterpret_cast<Pen*>(pen)));
    }
}

ogc_brush_t* ogc_draw_style_get_brush(ogc_draw_style_t* style) {
    if (style) {
        return reinterpret_cast<ogc_brush_t*>(
            reinterpret_cast<DrawStyle*>(style)->GetBrush());
    }
    return nullptr;
}

void ogc_draw_style_set_brush(ogc_draw_style_t* style, ogc_brush_t* brush) {
    if (style && brush) {
        reinterpret_cast<DrawStyle*>(style)->SetBrush(
            BrushPtr(reinterpret_cast<Brush*>(brush)));
    }
}

ogc_font_t* ogc_draw_style_get_font(ogc_draw_style_t* style) {
    if (style) {
        return reinterpret_cast<ogc_font_t*>(
            reinterpret_cast<DrawStyle*>(style)->GetFont());
    }
    return nullptr;
}

void ogc_draw_style_set_font(ogc_draw_style_t* style, ogc_font_t* font) {
    if (style && font) {
        reinterpret_cast<DrawStyle*>(style)->SetFont(
            FontPtr(reinterpret_cast<Font*>(font)));
    }
}

ogc_image_t* ogc_image_create(int width, int height) {
    return reinterpret_cast<ogc_image_t*>(Image::Create(width, height).release());
}

void ogc_image_destroy(ogc_image_t* image) {
    delete reinterpret_cast<Image*>(image);
}

int ogc_image_get_width(const ogc_image_t* image) {
    if (image) {
        return reinterpret_cast<const Image*>(image)->GetWidth();
    }
    return 0;
}

int ogc_image_get_height(const ogc_image_t* image) {
    if (image) {
        return reinterpret_cast<const Image*>(image)->GetHeight();
    }
    return 0;
}

int ogc_image_save(const ogc_image_t* image, const char* filepath, const char* format) {
    if (image && filepath) {
        return reinterpret_cast<const Image*>(image)->Save(
            SafeString(filepath), SafeString(format)) ? 1 : 0;
    }
    return 0;
}

ogc_image_t* ogc_image_load(const char* filepath) {
    if (filepath) {
        return reinterpret_cast<ogc_image_t*>(Image::Load(SafeString(filepath)).release());
    }
    return nullptr;
}

ogc_draw_device_t* ogc_draw_device_create(int type) {
    return reinterpret_cast<ogc_draw_device_t*>(DrawDevice::Create(type).release());
}

void ogc_draw_device_destroy(ogc_draw_device_t* device) {
    delete reinterpret_cast<DrawDevice*>(device);
}

int ogc_draw_device_begin_paint(ogc_draw_device_t* device) {
    if (device) {
        return reinterpret_cast<DrawDevice*>(device)->BeginPaint() ? 1 : 0;
    }
    return 0;
}

void ogc_draw_device_end_paint(ogc_draw_device_t* device) {
    if (device) {
        reinterpret_cast<DrawDevice*>(device)->EndPaint();
    }
}

void ogc_draw_device_clear(ogc_draw_device_t* device, ogc_color_t color) {
    if (device) {
        reinterpret_cast<DrawDevice*>(device)->Clear(Color(color.r, color.g, color.b, color.a));
    }
}

void ogc_draw_device_draw_point(ogc_draw_device_t* device, double x, double y, ogc_color_t color) {
    if (device) {
        reinterpret_cast<DrawDevice*>(device)->DrawPoint(x, y, Color(color.r, color.g, color.b, color.a));
    }
}

void ogc_draw_device_draw_line(ogc_draw_device_t* device, double x1, double y1, double x2, double y2, ogc_pen_t* pen) {
    if (device && pen) {
        reinterpret_cast<DrawDevice*>(device)->DrawLine(x1, y1, x2, y2, *reinterpret_cast<Pen*>(pen));
    }
}

void ogc_draw_device_draw_rect(ogc_draw_device_t* device, double x, double y, double width, double height, ogc_pen_t* pen) {
    if (device && pen) {
        reinterpret_cast<DrawDevice*>(device)->DrawRect(x, y, width, height, *reinterpret_cast<Pen*>(pen));
    }
}

void ogc_draw_device_fill_rect(ogc_draw_device_t* device, double x, double y, double width, double height, ogc_brush_t* brush) {
    if (device && brush) {
        reinterpret_cast<DrawDevice*>(device)->FillRect(x, y, width, height, *reinterpret_cast<Brush*>(brush));
    }
}

void ogc_draw_device_draw_circle(ogc_draw_device_t* device, double cx, double cy, double radius, ogc_pen_t* pen) {
    if (device && pen) {
        reinterpret_cast<DrawDevice*>(device)->DrawCircle(cx, cy, radius, *reinterpret_cast<Pen*>(pen));
    }
}

void ogc_draw_device_fill_circle(ogc_draw_device_t* device, double cx, double cy, double radius, ogc_brush_t* brush) {
    if (device && brush) {
        reinterpret_cast<DrawDevice*>(device)->FillCircle(cx, cy, radius, *reinterpret_cast<Brush*>(brush));
    }
}

void ogc_draw_device_draw_text(ogc_draw_device_t* device, const char* text, double x, double y, ogc_font_t* font, ogc_color_t color) {
    if (device && text && font) {
        reinterpret_cast<DrawDevice*>(device)->DrawText(SafeString(text), x, y, 
            *reinterpret_cast<Font*>(font), Color(color.r, color.g, color.b, color.a));
    }
}

void ogc_draw_device_draw_geometry(ogc_draw_device_t* device, const ogc_geometry_t* geom, ogc_draw_style_t* style) {
    if (device && geom && style) {
        reinterpret_cast<DrawDevice*>(device)->DrawGeometry(
            *reinterpret_cast<const ogc::geom::Geometry*>(geom),
            *reinterpret_cast<DrawStyle*>(style));
    }
}

ogc_draw_engine_t* ogc_draw_engine_create(void) {
    return reinterpret_cast<ogc_draw_engine_t*>(DrawEngine::Create().release());
}

void ogc_draw_engine_destroy(ogc_draw_engine_t* engine) {
    delete reinterpret_cast<DrawEngine*>(engine);
}

int ogc_draw_engine_initialize(ogc_draw_engine_t* engine, ogc_draw_device_t* device) {
    if (engine && device) {
        return reinterpret_cast<DrawEngine*>(engine)->Initialize(
            reinterpret_cast<DrawDevice*>(device)) ? 1 : 0;
    }
    return 0;
}

void ogc_draw_engine_shutdown(ogc_draw_engine_t* engine) {
    if (engine) {
        reinterpret_cast<DrawEngine*>(engine)->Shutdown();
    }
}

ogc_draw_context_t* ogc_draw_context_create(ogc_draw_device_t* device) {
    if (device) {
        return reinterpret_cast<ogc_draw_context_t*>(
            DrawContext::Create(reinterpret_cast<DrawDevice*>(device)).release());
    }
    return nullptr;
}

void ogc_draw_context_destroy(ogc_draw_context_t* ctx) {
    delete reinterpret_cast<DrawContext*>(ctx);
}

void ogc_draw_context_push_state(ogc_draw_context_t* ctx) {
    if (ctx) {
        reinterpret_cast<DrawContext*>(ctx)->PushState();
    }
}

void ogc_draw_context_pop_state(ogc_draw_context_t* ctx) {
    if (ctx) {
        reinterpret_cast<DrawContext*>(ctx)->PopState();
    }
}

void ogc_draw_context_set_transform(ogc_draw_context_t* ctx, double m11, double m12, double m21, double m22, double dx, double dy) {
    if (ctx) {
        reinterpret_cast<DrawContext*>(ctx)->SetTransform(m11, m12, m21, m22, dx, dy);
    }
}

void ogc_draw_context_reset_transform(ogc_draw_context_t* ctx) {
    if (ctx) {
        reinterpret_cast<DrawContext*>(ctx)->ResetTransform();
    }
}

void ogc_draw_context_translate(ogc_draw_context_t* ctx, double dx, double dy) {
    if (ctx) {
        reinterpret_cast<DrawContext*>(ctx)->Translate(dx, dy);
    }
}

void ogc_draw_context_scale(ogc_draw_context_t* ctx, double sx, double sy) {
    if (ctx) {
        reinterpret_cast<DrawContext*>(ctx)->Scale(sx, sy);
    }
}

void ogc_draw_context_rotate(ogc_draw_context_t* ctx, double angle) {
    if (ctx) {
        reinterpret_cast<DrawContext*>(ctx)->Rotate(angle);
    }
}

void ogc_draw_context_set_clip_rect(ogc_draw_context_t* ctx, double x, double y, double width, double height) {
    if (ctx) {
        reinterpret_cast<DrawContext*>(ctx)->SetClipRect(x, y, width, height);
    }
}

void ogc_draw_context_reset_clip(ogc_draw_context_t* ctx) {
    if (ctx) {
        reinterpret_cast<DrawContext*>(ctx)->ResetClip();
    }
}

#ifdef __cplusplus
}
#endif
