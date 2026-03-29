#include "ogc/draw/svg_device.h"
#include "ogc/draw/draw_engine.h"
#include <fstream>
#include <iomanip>

namespace ogc {
namespace draw {

std::map<std::string, std::pair<double, double>> SvgDevice::s_pageSizes = {
    { "A4", { 595.28, 841.89 } },
    { "A3", { 841.89, 1190.55 } },
    { "Letter", { 612.0, 792.0 } },
    { "Legal", { 612.0, 1008.0 } }
};

SvgDevice::SvgDevice(const std::string& filePath, double width, double height)
    : m_filePath(filePath)
    , m_width(width)
    , m_height(height)
    , m_initialized(false)
    , m_groupDepth(0)
    , m_state(DeviceState::kUninitialized)
{
}

SvgDevice::~SvgDevice()
{
    if (m_initialized) {
        Save();
    }
}

std::unique_ptr<DrawEngine> SvgDevice::CreateEngine()
{
    return nullptr;
}

std::vector<EngineType> SvgDevice::GetSupportedEngineTypes() const
{
    return { EngineType::kVector };
}

DrawResult SvgDevice::Initialize()
{
    if (m_initialized) {
        return DrawResult::kSuccess;
    }

    m_content.str("");
    m_content.clear();
    m_groupDepth = 0;

    WriteHeader();
    m_initialized = true;
    m_state = DeviceState::kReady;
    return DrawResult::kSuccess;
}

DrawResult SvgDevice::Finalize()
{
    if (!m_initialized) {
        return DrawResult::kInvalidState;
    }
    
    m_state = DeviceState::kReady;
    return DrawResult::kSuccess;
}

DeviceCapabilities SvgDevice::QueryCapabilities() const
{
    DeviceCapabilities caps;
    caps.supportsGPU = false;
    caps.supportsMultithreading = false;
    caps.supportsVSync = false;
    caps.supportsDoubleBuffer = false;
    caps.supportsPartialUpdate = true;
    caps.supportsAlpha = true;
    caps.supportsAntialiasing = true;
    caps.maxTextureSize = 0;
    caps.maxRenderTargets = 1;
    return caps;
}

bool SvgDevice::IsFeatureAvailable(const std::string& featureName) const
{
    return featureName == "VectorOutput" ||
           featureName == "Transparency" ||
           featureName == "Transform" ||
           featureName == "Clip" ||
           featureName == "Text" ||
           featureName == "Image";
}

void SvgDevice::SetTitle(const std::string& title)
{
    m_title = title;
}

void SvgDevice::SetDescription(const std::string& desc)
{
    m_description = desc;
}

bool SvgDevice::Save()
{
    WriteFooter();

    std::ofstream file(m_filePath);
    if (!file.is_open()) {
        return false;
    }

    file << m_content.str();
    file.close();

    return true;
}

void SvgDevice::AppendContent(const std::string& content)
{
    m_content << content;
}

void SvgDevice::BeginGroup(const std::string& id)
{
    if (!id.empty()) {
        m_content << "  <g id=\"" << id << "\">\n";
    } else {
        m_content << "  <g>\n";
    }
    m_groupDepth++;
}

void SvgDevice::EndGroup()
{
    if (m_groupDepth > 0) {
        m_content << "  </g>\n";
        m_groupDepth--;
    }
}

void SvgDevice::WriteHeader()
{
    m_content << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    m_content << "<svg xmlns=\"http://www.w3.org/2000/svg\" ";
    m_content << "width=\"" << m_width << "\" ";
    m_content << "height=\"" << m_height << "\" ";
    m_content << "viewBox=\"0 0 " << m_width << " " << m_height << "\">\n";

    if (!m_title.empty()) {
        m_content << "  <title>" << m_title << "</title>\n";
    }
    if (!m_description.empty()) {
        m_content << "  <desc>" << m_description << "</desc>\n";
    }
}

void SvgDevice::WriteFooter()
{
    while (m_groupDepth > 0) {
        EndGroup();
    }
    m_content << "</svg>\n";
}

} // namespace draw
} // namespace ogc
