#include "ogc/graph/render/draw_facade.h"
#include <ogc/draw/raster_image_device.h>
#include <ogc/draw/draw_context.h>
#include <unordered_map>

namespace ogc {
namespace graph {

namespace {
    std::unordered_map<std::string, ogc::draw::DrawDevicePtr> g_devices;
    std::unordered_map<std::string, ogc::draw::DrawEnginePtr> g_engines;
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

ogc::draw::DrawResult DrawFacade::Initialize() {
    if (m_initialized) {
        return ogc::draw::DrawResult::kSuccess;
    }
    m_initialized = true;
    return ogc::draw::DrawResult::kSuccess;
}

ogc::draw::DrawResult DrawFacade::Finalize() {
    if (!m_initialized) {
        return ogc::draw::DrawResult::kSuccess;
    }
    g_devices.clear();
    g_engines.clear();
    m_initialized = false;
    return ogc::draw::DrawResult::kSuccess;
}

bool DrawFacade::IsInitialized() const {
    return m_initialized;
}

ogc::draw::DrawContextPtr DrawFacade::CreateContext() {
    return ogc::draw::DrawContext::Create(nullptr);
}

ogc::draw::DrawContextPtr DrawFacade::CreateContext(ogc::draw::DrawDevicePtr device) {
    return ogc::draw::DrawContext::Create(device.get());
}

ogc::draw::DrawContextPtr DrawFacade::CreateContext(ogc::draw::DrawDevicePtr device, ogc::draw::DrawEnginePtr engine) {
    (void)engine;
    return ogc::draw::DrawContext::Create(device.get());
}

ogc::draw::DrawDevicePtr DrawFacade::CreateDevice(ogc::draw::DeviceType type, int width, int height) {
    switch (type) {
        case ogc::draw::DeviceType::kRasterImage:
            return std::make_shared<ogc::draw::RasterImageDevice>(width, height, ogc::draw::PixelFormat::kRGBA8888);
        default:
            return nullptr;
    }
}

ogc::draw::DrawEnginePtr DrawFacade::CreateEngine(ogc::draw::EngineType type) {
    (void)type;
    return nullptr;
}

ogc::draw::DrawResult DrawFacade::RegisterDevice(const std::string& name, ogc::draw::DrawDevicePtr device) {
    g_devices[name] = device;
    return ogc::draw::DrawResult::kSuccess;
}

ogc::draw::DrawResult DrawFacade::RegisterEngine(const std::string& name, ogc::draw::DrawEnginePtr engine) {
    g_engines[name] = engine;
    return ogc::draw::DrawResult::kSuccess;
}

ogc::draw::DrawDevicePtr DrawFacade::GetDevice(const std::string& name) {
    auto it = g_devices.find(name);
    return it != g_devices.end() ? it->second : nullptr;
}

ogc::draw::DrawEnginePtr DrawFacade::GetEngine(const std::string& name) {
    auto it = g_engines.find(name);
    return it != g_engines.end() ? it->second : nullptr;
}

void DrawFacade::SetDefaultDrawStyle(const ogc::draw::DrawStyle& style) {
    m_defaultStyle = style;
}

ogc::draw::DrawStyle DrawFacade::GetDefaultDrawStyle() const {
    return m_defaultStyle;
}

void DrawFacade::SetDefaultFont(const ogc::draw::Font& font) {
    m_defaultFont = font;
}

ogc::draw::Font DrawFacade::GetDefaultFont() const {
    return m_defaultFont;
}

void DrawFacade::SetDefaultColor(const ogc::draw::Color& color) {
    m_defaultColor = color;
}

ogc::draw::Color DrawFacade::GetDefaultColor() const {
    return m_defaultColor;
}

void DrawFacade::SetDefaultDpi(double dpi) {
    m_defaultDpi = dpi;
}

double DrawFacade::GetDefaultDpi() const {
    return m_defaultDpi;
}

void DrawFacade::SetDefaultBackground(const ogc::draw::Color& color) {
    m_defaultBackground = color;
}

ogc::draw::Color DrawFacade::GetDefaultBackground() const {
    return m_defaultBackground;
}

ogc::draw::DrawStyle DrawFacade::CreateStrokeStyle(uint32_t color, double width) {
    ogc::draw::DrawStyle style;
    style.pen.color = ogc::draw::Color::FromHexWithAlpha(color);
    style.pen.width = width;
    style.brush = ogc::draw::Brush::NoBrush();
    return style;
}

ogc::draw::DrawStyle DrawFacade::CreateFillStyle(uint32_t color) {
    ogc::draw::DrawStyle style;
    style.pen = ogc::draw::Pen::NoPen();
    style.brush.color = ogc::draw::Color::FromHexWithAlpha(color);
    return style;
}

ogc::draw::DrawStyle DrawFacade::CreateStrokeFillStyle(uint32_t strokeColor, double strokeWidth, uint32_t fillColor) {
    ogc::draw::DrawStyle style;
    style.pen.color = ogc::draw::Color::FromHexWithAlpha(strokeColor);
    style.pen.width = strokeWidth;
    style.brush.color = ogc::draw::Color::FromHexWithAlpha(fillColor);
    return style;
}

ogc::draw::Font DrawFacade::CreateFont(const std::string& family, double size, bool bold, bool italic) {
    return ogc::draw::Font(family, size, bold ? (italic ? ogc::draw::FontStyle::kBoldItalic : ogc::draw::FontStyle::kBold) : (italic ? ogc::draw::FontStyle::kItalic : ogc::draw::FontStyle::kNormal));
}

ogc::draw::Color DrawFacade::CreateColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return ogc::draw::Color(r, g, b, a);
}

ogc::draw::Color DrawFacade::CreateColorFromHex(const std::string& hex) {
    return ogc::draw::Color::FromHexString(hex);
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
