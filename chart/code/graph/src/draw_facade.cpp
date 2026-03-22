#include "ogc/draw/draw_facade.h"
#include "ogc/draw/raster_image_device.h"
#include "ogc/draw/driver_manager.h"

namespace ogc {
namespace draw {

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
    DriverManager::Instance().Clear();
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
    return DrawContext::Create(device);
}

DrawContextPtr DrawFacade::CreateContext(DrawDevicePtr device, DrawEnginePtr engine) {
    return DrawContext::Create(device, engine);
}

DrawDevicePtr DrawFacade::CreateDevice(DeviceType type, int width, int height) {
    switch (type) {
        case DeviceType::kRasterImage:
            return RasterImageDevice::Create(width, height, 4);
        default:
            return nullptr;
    }
}

DrawEnginePtr DrawFacade::CreateEngine(EngineType type) {
    return DrawEngine::Create(type);
}

DrawDriverPtr DrawFacade::CreateDriver(const std::string& name) {
    return DrawDriver::Create(name);
}

DrawResult DrawFacade::RegisterDevice(const std::string& name, DrawDevicePtr device) {
    return DriverManager::Instance().RegisterDevice(name, device);
}

DrawResult DrawFacade::RegisterEngine(const std::string& name, DrawEnginePtr engine) {
    return DriverManager::Instance().RegisterEngine(name, engine);
}

DrawResult DrawFacade::RegisterDriver(const std::string& name, DrawDriverPtr driver) {
    return DriverManager::Instance().RegisterDriver(name, driver);
}

DrawDevicePtr DrawFacade::GetDevice(const std::string& name) {
    return DriverManager::Instance().GetDevice(name);
}

DrawEnginePtr DrawFacade::GetEngine(const std::string& name) {
    return DriverManager::Instance().GetEngine(name);
}

DrawDriverPtr DrawFacade::GetDriver(const std::string& name) {
    return DriverManager::Instance().GetDriver(name);
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
    style.stroke.color = color;
    style.stroke.width = width;
    style.stroke.visible = true;
    style.fill.visible = false;
    return style;
}

DrawStyle DrawFacade::CreateFillStyle(uint32_t color) {
    DrawStyle style;
    style.fill.color = color;
    style.fill.visible = true;
    style.stroke.visible = false;
    return style;
}

DrawStyle DrawFacade::CreateStrokeFillStyle(uint32_t strokeColor, double strokeWidth, uint32_t fillColor) {
    DrawStyle style;
    style.stroke.color = strokeColor;
    style.stroke.width = strokeWidth;
    style.stroke.visible = true;
    style.fill.color = fillColor;
    style.fill.visible = true;
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
