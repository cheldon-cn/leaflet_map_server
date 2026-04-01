#include "ogc/draw/draw_facade.h"
#include <ogc/draw/raster_image_device.h>
#include <ogc/draw/draw_context.h>
#include <unordered_map>

namespace ogc {
namespace draw {

namespace {
    std::unordered_map<std::string, DrawDevicePtr> g_devices;
    std::unordered_map<std::string, DrawEnginePtr> g_engines;
}

DrawFacade& DrawFacade::Instance() {
    static DrawFacade instance;
    return instance;
}

DrawFacade::DrawFacade()
    : m_initialized(false)
    , m_defaultDpi(96.0) {
}

DrawFacade::~DrawFacade() {
    Finalize();
}

DrawResult DrawFacade::Initialize() {
    if (m_initialized) {
        return DrawResult::kSuccess;
    }
    m_initialized = true;
    return DrawResult::kSuccess;
}

DrawResult DrawFacade::Finalize() {
    if (!m_initialized) {
        return DrawResult::kSuccess;
    }
    g_devices.clear();
    g_engines.clear();
    m_initialized = false;
    return DrawResult::kSuccess;
}

bool DrawFacade::IsInitialized() const {
    return m_initialized;
}

DrawContextPtr DrawFacade::CreateContext() {
    return DrawContext::Create(nullptr);
}

DrawContextPtr DrawFacade::CreateContext(DrawDevicePtr device) {
    return DrawContext::Create(device.get());
}

DrawContextPtr DrawFacade::CreateContext(DrawDevicePtr device, DrawEnginePtr engine) {
    (void)engine;
    return DrawContext::Create(device.get());
}

DrawDevicePtr DrawFacade::CreateDevice(DeviceType type, int width, int height) {
    switch (type) {
        case DeviceType::kRasterImage:
            return std::make_shared<RasterImageDevice>(width, height, PixelFormat::kRGBA8888);
        default:
            return nullptr;
    }
}

DrawEnginePtr DrawFacade::CreateEngine(EngineType type) {
    (void)type;
    return nullptr;
}

DrawResult DrawFacade::RegisterDevice(const std::string& name, DrawDevicePtr device) {
    g_devices[name] = device;
    return DrawResult::kSuccess;
}

DrawResult DrawFacade::RegisterEngine(const std::string& name, DrawEnginePtr engine) {
    g_engines[name] = engine;
    return DrawResult::kSuccess;
}

DrawDevicePtr DrawFacade::GetDevice(const std::string& name) {
    auto it = g_devices.find(name);
    return it != g_devices.end() ? it->second : nullptr;
}

DrawEnginePtr DrawFacade::GetEngine(const std::string& name) {
    auto it = g_engines.find(name);
    return it != g_engines.end() ? it->second : nullptr;
}

void DrawFacade::SetDefaultDrawStyle(const DrawStyle& style) {
    m_defaultStyle = style;
}

DrawStyle DrawFacade::GetDefaultDrawStyle() const {
    return m_defaultStyle;
}

void DrawFacade::SetDefaultFont(const Font& font) {
    m_defaultFont = font;
}

Font DrawFacade::GetDefaultFont() const {
    return m_defaultFont;
}

void DrawFacade::SetDefaultColor(const Color& color) {
    m_defaultColor = color;
}

Color DrawFacade::GetDefaultColor() const {
    return m_defaultColor;
}

void DrawFacade::SetDefaultDpi(double dpi) {
    m_defaultDpi = dpi;
}

double DrawFacade::GetDefaultDpi() const {
    return m_defaultDpi;
}

void DrawFacade::SetDefaultBackground(const Color& color) {
    m_defaultBackground = color;
}

Color DrawFacade::GetDefaultBackground() const {
    return m_defaultBackground;
}

DrawStyle DrawFacade::CreateStrokeStyle(uint32_t color, double width) {
    DrawStyle style;
    style.pen.color = Color::FromHexWithAlpha(color);
    style.pen.width = width;
    style.brush = Brush::NoBrush();
    return style;
}

DrawStyle DrawFacade::CreateFillStyle(uint32_t color) {
    DrawStyle style;
    style.pen = Pen::NoPen();
    style.brush.color = Color::FromHexWithAlpha(color);
    return style;
}

DrawStyle DrawFacade::CreateStrokeFillStyle(uint32_t strokeColor, double strokeWidth, uint32_t fillColor) {
    DrawStyle style;
    style.pen.color = Color::FromHexWithAlpha(strokeColor);
    style.pen.width = strokeWidth;
    style.brush.color = Color::FromHexWithAlpha(fillColor);
    return style;
}

Font DrawFacade::CreateFont(const std::string& family, double size, bool bold, bool italic) {
    return Font(family, size, bold ? (italic ? FontStyle::kBoldItalic : FontStyle::kBold) : (italic ? FontStyle::kItalic : FontStyle::kNormal));
}

Color DrawFacade::CreateColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return Color(r, g, b, a);
}

Color DrawFacade::CreateColorFromHex(const std::string& hex) {
    return Color::FromHexString(hex);
}

DrawParams DrawFacade::CreateDrawParams(const Envelope& extent, int width, int height, double dpi) {
    return DrawParams(extent, width, height, dpi);
}

std::string DrawFacade::GetVersion() {
    return "1.0.0";
}

std::string DrawFacade::GetBuildInfo() {
    return "OGC Graphics Drawing Library - Build 1.0.0";
}

}  
}  
