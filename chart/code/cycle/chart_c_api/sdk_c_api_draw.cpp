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
#include <ogc/draw/raster_image_device.h>
#include <ogc/draw/svg_device.h>
#include <ogc/draw/tile_device.h>

#include <cstring>
#include <cstdlib>
#include <memory>
#include <string>

using namespace ogc::draw;

#ifdef __cplusplus
extern "C" {
#endif

namespace { static std::string SafeString(const char* str) {
    return str ? std::string(str) : std::string();
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

ogc_color_t ogc_color_from_hex(unsigned int hex) {
    ogc_color_t color;
    color.r = static_cast<uint8_t>((hex >> 16) & 0xFF);
    color.g = static_cast<uint8_t>((hex >> 8) & 0xFF);
    color.b = static_cast<uint8_t>(hex & 0xFF);
    color.a = static_cast<uint8_t>((hex >> 24) & 0xFF);
    if (color.a == 0) color.a = 255;
    return color;
}

unsigned int ogc_color_to_hex(const ogc_color_t* color) {
    if (color) {
        return (static_cast<unsigned int>(color->r) << 16) |
               (static_cast<unsigned int>(color->g) << 8) |
               static_cast<unsigned int>(color->b);
    }
    return 0;
}

void ogc_color_to_rgba(const ogc_color_t* color, unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a) {
    if (color) {
        if (r) *r = color->r;
        if (g) *g = color->g;
        if (b) *b = color->b;
        if (a) *a = color->a;
    }
}

ogc_font_t* ogc_font_create(const char* family, double size) {
    return reinterpret_cast<ogc_font_t*>(new Font(SafeString(family), size));
}

void ogc_font_destroy(ogc_font_t* font) {
    delete reinterpret_cast<Font*>(font);
}

const char* ogc_font_get_family(const ogc_font_t* font) {
    if (font) {
        static thread_local std::string result;
        result = reinterpret_cast<const Font*>(font)->GetFamily();
        return result.c_str();
    }
    return "";
}

void ogc_font_set_family(ogc_font_t* font, const char* family) {
    if (font) {
        reinterpret_cast<Font*>(font)->SetFamily(SafeString(family));
    }
}
double ogc_font_get_size(const ogc_font_t* font) {
    if (font) {
        return reinterpret_cast<const Font*>(font)->GetSize();
    }
    return 0.0;
}

int ogc_font_is_bold(const ogc_font_t* font) {
    if (font) {
        return reinterpret_cast<const Font*>(font)->IsBold() ? 1 : 0;
    }
    return 0;
}

int ogc_font_is_italic(const ogc_font_t* font) {
    if (font) {
        return reinterpret_cast<const Font*>(font)->IsItalic() ? 1 : 0;
    }
    return 0;
}

void ogc_font_set_bold(ogc_font_t* font, int bold) {
    if (font) {
        reinterpret_cast<Font*>(font)->SetBold(bold != 0);
    }
}

void ogc_font_set_italic(ogc_font_t* font, int italic) {
    if (font) {
        reinterpret_cast<Font*>(font)->SetItalic(italic != 0);
    }
}

ogc_pen_t* ogc_pen_create(void) {
    return reinterpret_cast<ogc_pen_t*>(new Pen());
}

void ogc_pen_destroy(ogc_pen_t* pen) {
    delete reinterpret_cast<Pen*>(pen);
}

ogc_color_t ogc_pen_get_color(const ogc_pen_t* pen) {
    ogc_color_t result = {0, 0, 0, 255};
    if (pen) {
        const Pen* p = reinterpret_cast<const Pen*>(pen);
        result.r = p->color.GetRed();
        result.g = p->color.GetGreen();
        result.b = p->color.GetBlue();
        result.a = p->color.GetAlpha();
    }
    return result;
}

double ogc_pen_get_width(const ogc_pen_t* pen) {
    if (pen) {
        return reinterpret_cast<const Pen*>(pen)->width;
    }
    return 1.0;
}

void ogc_pen_set_color(ogc_pen_t* pen, ogc_color_t color) {
    if (pen) {
        reinterpret_cast<Pen*>(pen)->color = Color(color.r, color.g, color.b, color.a);
    }
}

void ogc_pen_set_width(ogc_pen_t* pen, double width) {
    if (pen) {
        reinterpret_cast<Pen*>(pen)->width = width;
    }
}

ogc_pen_style_e ogc_pen_get_style(const ogc_pen_t* pen) {
    if (pen) {
        auto style = reinterpret_cast<const Pen*>(pen)->style;
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
        reinterpret_cast<Pen*>(pen)->style = ps;
    }
}
ogc_brush_t* ogc_brush_create(void) {
    return reinterpret_cast<ogc_brush_t*>(new Brush());
}

void ogc_brush_destroy(ogc_brush_t* brush) {
    delete reinterpret_cast<Brush*>(brush);
}

ogc_color_t ogc_brush_get_color(const ogc_brush_t* brush) {
    ogc_color_t result = {0, 0, 0, 255};
    if (brush) {
        const Brush* b = reinterpret_cast<const Brush*>(brush);
        result.r = b->color.GetRed();
        result.g = b->color.GetGreen();
        result.b = b->color.GetBlue();
        result.a = b->color.GetAlpha();
    }
    return result;
}

void ogc_brush_set_color(ogc_brush_t* brush, ogc_color_t color) {
    if (brush) {
        reinterpret_cast<Brush*>(brush)->color = Color(color.r, color.g, color.b, color.a);
    }
}

ogc_brush_style_e ogc_brush_get_style(const ogc_brush_t* brush) {
    if (brush) {
        auto style = reinterpret_cast<const Brush*>(brush)->style;
        switch (style) {
            case BrushStyle::kSolid: return OGC_BRUSH_STYLE_SOLID;
            case BrushStyle::kNone: return OGC_BRUSH_STYLE_NONE;
            case BrushStyle::kHorizontal: return OGC_BRUSH_STYLE_HORIZONTAL;
            case BrushStyle::kVertical: return OGC_BRUSH_STYLE_VERTICAL;
            case BrushStyle::kCross: return OGC_BRUSH_STYLE_CROSS;
            case BrushStyle::kBDiagonal: return OGC_BRUSH_STYLE_BDIAGONAL;
            case BrushStyle::kFDiagonal: return OGC_BRUSH_STYLE_FDIAGONAL;
            case BrushStyle::kDiagonalCross: return OGC_BRUSH_STYLE_DIAGONAL_CROSS;
            case BrushStyle::kTexture: return OGC_BRUSH_STYLE_TEXTURE;
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
            case OGC_BRUSH_STYLE_BDIAGONAL: bs = BrushStyle::kBDiagonal; break;
            case OGC_BRUSH_STYLE_FDIAGONAL: bs = BrushStyle::kFDiagonal; break;
            case OGC_BRUSH_STYLE_DIAGONAL_CROSS: bs = BrushStyle::kDiagonalCross; break;
            case OGC_BRUSH_STYLE_TEXTURE: bs = BrushStyle::kTexture; break;
        }
        reinterpret_cast<Brush*>(brush)->style = bs;
    }
}
ogc_draw_style_t* ogc_draw_style_create(void) {
    return reinterpret_cast<ogc_draw_style_t*>(new DrawStyle());
}

void ogc_draw_style_destroy(ogc_draw_style_t* style) {
    delete reinterpret_cast<DrawStyle*>(style);
}

ogc_pen_t* ogc_draw_style_get_pen(ogc_draw_style_t* style) {
    if (style) {
        return reinterpret_cast<ogc_pen_t*>(&reinterpret_cast<DrawStyle*>(style)->pen);
    }
    return nullptr;
}

void ogc_draw_style_set_pen(ogc_draw_style_t* style, ogc_pen_t* pen) {
    if (style && pen) {
        reinterpret_cast<DrawStyle*>(style)->pen = *reinterpret_cast<Pen*>(pen);
    }
}

ogc_brush_t* ogc_draw_style_get_brush(ogc_draw_style_t* style) {
    if (style) {
        return reinterpret_cast<ogc_brush_t*>(&reinterpret_cast<DrawStyle*>(style)->brush);
    }
    return nullptr;
}

void ogc_draw_style_set_brush(ogc_draw_style_t* style, ogc_brush_t* brush) {
    if (style && brush) {
        reinterpret_cast<DrawStyle*>(style)->brush = *reinterpret_cast<Brush*>(brush);
    }
}

ogc_font_t* ogc_draw_style_get_font(ogc_draw_style_t* style) {
    if (style) {
        return reinterpret_cast<ogc_font_t*>(&reinterpret_cast<DrawStyle*>(style)->font);
    }
    return nullptr;
}

void ogc_draw_style_set_font(ogc_draw_style_t* style, ogc_font_t* font) {
    if (style && font) {
        reinterpret_cast<DrawStyle*>(style)->font = *reinterpret_cast<Font*>(font);
    }
}

ogc_image_t* ogc_image_create(int width, int height, int channels) {
    return reinterpret_cast<ogc_image_t*>(new Image(width, height, channels));
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
        return reinterpret_cast<const Image*>(image)->SaveToFile(SafeString(filepath)) ? 1 : 0;
    }
    return 0;
}

ogc_image_t* ogc_image_load(const char* filepath) {
    if (filepath) {
        auto* img = new Image();
        if (img->LoadFromFile(SafeString(filepath))) {
            return reinterpret_cast<ogc_image_t*>(img);
        }
        delete img;
    }
    return nullptr;
}


ogc_draw_device_t* ogc_draw_device_create(ogc_device_type_e type, int width, int height) {
    switch (type) {
        case OGC_DEVICE_TYPE_RASTER_IMAGE:
            return reinterpret_cast<ogc_draw_device_t*>(new RasterImageDevice(width, height));
        case OGC_DEVICE_TYPE_SVG:
            return reinterpret_cast<ogc_draw_device_t*>(new SvgDevice("output.svg", width, height));
        case OGC_DEVICE_TYPE_TILE:
            return reinterpret_cast<ogc_draw_device_t*>(new TileDevice(width, height));
        default:
            return reinterpret_cast<ogc_draw_device_t*>(new RasterImageDevice(width, height));
    }
}

void ogc_draw_device_destroy(ogc_draw_device_t* device) {
    delete reinterpret_cast<DrawDevice*>(device);
}

int ogc_draw_device_get_width(const ogc_draw_device_t* device) {
    if (device) {
        return reinterpret_cast<const DrawDevice*>(device)->GetWidth();
    }
    return 0;
}

int ogc_draw_device_get_height(const ogc_draw_device_t* device) {
    if (device) {
        return reinterpret_cast<const DrawDevice*>(device)->GetHeight();
    }
    return 0;
}

ogc_image_t* ogc_draw_device_get_image(ogc_draw_device_t* device) {
    if (device) {
        auto* drawDevice = reinterpret_cast<DrawDevice*>(device);
        return reinterpret_cast<ogc_image_t*>(new Image(drawDevice->GetWidth(), drawDevice->GetHeight(), 4));
    }
    return nullptr;
}

ogc_draw_engine_t* ogc_draw_engine_create(ogc_engine_type_e type) {
    (void)type;
    return nullptr;
}

void ogc_draw_engine_destroy(ogc_draw_engine_t* engine) {
    delete reinterpret_cast<DrawEngine*>(engine);
}

int ogc_draw_engine_initialize(ogc_draw_engine_t* engine, ogc_draw_device_t* device) {
    if (engine && device) {
        (void)device;
        return reinterpret_cast<DrawEngine*>(engine)->Begin() == DrawResult::kSuccess ? 1 : 0;
    }
    return 0;
}

void ogc_draw_engine_shutdown(ogc_draw_engine_t* engine) {
    if (engine) {
        reinterpret_cast<DrawEngine*>(engine)->End();
    }
}

ogc_draw_context_t* ogc_draw_context_create(ogc_draw_device_t* device, ogc_draw_engine_t* engine) {
    if (device) {
        (void)engine;
        auto ctx = DrawContext::Create(reinterpret_cast<DrawDevice*>(device));
        return reinterpret_cast<ogc_draw_context_t*>(ctx.release());
    }
    return nullptr;
}

void ogc_draw_context_destroy(ogc_draw_context_t* ctx) {
    delete reinterpret_cast<DrawContext*>(ctx);
}

void ogc_draw_context_push_state(ogc_draw_context_t* ctx) {
    if (ctx) {
        reinterpret_cast<DrawContext*>(ctx)->Save();
    }
}

void ogc_draw_context_pop_state(ogc_draw_context_t* ctx) {
    if (ctx) {
        reinterpret_cast<DrawContext*>(ctx)->Restore();
    }
}

void ogc_draw_context_set_transform(ogc_draw_context_t* ctx, double* matrix) {
    if (ctx && matrix) {
        reinterpret_cast<DrawContext*>(ctx)->SetTransform(TransformMatrix(matrix[0], matrix[1], matrix[2], matrix[3], matrix[4], matrix[5]));
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
