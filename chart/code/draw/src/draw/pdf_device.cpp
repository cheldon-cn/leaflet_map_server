#include "ogc/draw/pdf_device.h"
#include "ogc/draw/pdf_engine.h"
#include <fstream>
#include <cmath>
#include <iomanip>

namespace ogc {
namespace draw {

PdfDevice::PdfDevice()
    : m_width(612)
    , m_height(792)
    , m_dpi(72.0)
    , m_state(DeviceState::kUninitialized)
    , m_preferredEngine(EngineType::kVector)
    , m_currentPage(-1)
    , m_nextObjectId(1)
{
}

PdfDevice::PdfDevice(int width, int height, double dpi)
    : m_width(width)
    , m_height(height)
    , m_dpi(dpi)
    , m_state(DeviceState::kUninitialized)
    , m_preferredEngine(EngineType::kVector)
    , m_currentPage(-1)
    , m_nextObjectId(1)
{
}

PdfDevice::~PdfDevice() {
    Finalize();
}

DrawResult PdfDevice::Initialize() {
    if (m_state == DeviceState::kReady) {
        return DrawResult::kSuccess;
    }
    
    m_pages.clear();
    m_fontMap.clear();
    m_imageMap.clear();
    m_nextObjectId = 1;
    
    NewPage(m_width, m_height);
    
    m_state = DeviceState::kReady;
    return DrawResult::kSuccess;
}

DrawResult PdfDevice::Finalize() {
    m_pages.clear();
    m_state = DeviceState::kUninitialized;
    return DrawResult::kSuccess;
}

std::unique_ptr<DrawEngine> PdfDevice::CreateEngine() {
    return std::unique_ptr<DrawEngine>(new PdfEngine(this));
}

std::vector<EngineType> PdfDevice::GetSupportedEngineTypes() const {
    return { EngineType::kVector, EngineType::kSimple2D };
}

void PdfDevice::SetPreferredEngineType(EngineType type) {
    m_preferredEngine = type;
}

EngineType PdfDevice::GetPreferredEngineType() const {
    return m_preferredEngine;
}

DeviceCapabilities PdfDevice::QueryCapabilities() const {
    DeviceCapabilities caps;
    caps.supportsGPU = false;
    caps.supportsMultithreading = false;
    caps.supportsVSync = false;
    caps.supportsDoubleBuffer = false;
    caps.supportsPartialUpdate = false;
    caps.supportsAlpha = true;
    caps.supportsAntialiasing = true;
    caps.maxTextureSize = 0;
    caps.maxRenderTargets = 1;
    caps.supportedFormats = { "pdf" };
    return caps;
}

bool PdfDevice::IsFeatureAvailable(const std::string& featureName) const {
    if (featureName == "multipage") return true;
    if (featureName == "metadata") return true;
    if (featureName == "vector") return true;
    return false;
}

void PdfDevice::SetDpi(double dpi) {
    m_dpi = dpi;
}

void PdfDevice::SetTitle(const std::string& title) {
    m_title = title;
}

void PdfDevice::SetAuthor(const std::string& author) {
    m_author = author;
}

void PdfDevice::SetSubject(const std::string& subject) {
    m_subject = subject;
}

void PdfDevice::SetCreator(const std::string& creator) {
    m_creator = creator;
}

void PdfDevice::NewPage(int width, int height) {
    auto page = std::make_unique<PdfPage>(width, height);
    m_pages.push_back(std::move(page));
    m_currentPage = static_cast<int>(m_pages.size()) - 1;
}

int PdfDevice::GetCurrentPage() const {
    return m_currentPage;
}

int PdfDevice::GetPageCount() const {
    return static_cast<int>(m_pages.size());
}

std::shared_ptr<PdfDevice> PdfDevice::Create(int width, int height, double dpi) {
    auto device = std::make_shared<PdfDevice>(width, height, dpi);
    device->Initialize();
    return device;
}

void PdfDevice::TransformPoint(double x, double y, double& outX, double& outY) const {
    outX = x;
    outY = y;
}

double PdfDevice::PdfY(double y) const {
    if (m_currentPage >= 0 && m_currentPage < static_cast<int>(m_pages.size())) {
        return m_pages[m_currentPage]->height - y;
    }
    return m_height - y;
}

std::string PdfDevice::ColorToPdf(uint32_t color) const {
    double r = ((color >> 16) & 0xFF) / 255.0;
    double g = ((color >> 8) & 0xFF) / 255.0;
    double b = (color & 0xFF) / 255.0;
    
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(3);
    oss << r << " " << g << " " << b << " rg\n";
    return oss.str();
}

std::string PdfDevice::StyleToPdf(const DrawStyle&) const {
    std::ostringstream oss;
    return oss.str();
}

int PdfDevice::AddFont(const std::string& fontFamily, double fontSize) {
    std::string key = fontFamily + "_" + std::to_string(static_cast<int>(fontSize));
    auto it = m_fontMap.find(key);
    if (it != m_fontMap.end()) {
        return it->second;
    }
    
    int id = static_cast<int>(m_fontMap.size()) + 1;
    m_fontMap[key] = id;
    return id;
}

int PdfDevice::AddImage(int width, int height, int channels, const uint8_t* data) {
    size_t hash = 0;
    for (int i = 0; i < std::min(100, width * height * channels); ++i) {
        hash = hash * 31 + data[i];
    }
    
    auto it = m_imageMap.find(hash);
    if (it != m_imageMap.end()) {
        return it->second;
    }
    
    int id = static_cast<int>(m_imageMap.size()) + 1;
    m_imageMap[hash] = id;
    return id;
}

bool PdfDevice::SaveToFile(const std::string& filepath) {
    std::string content = GeneratePdfContent();
    
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    file.close();
    
    return true;
}

std::string PdfDevice::GeneratePdfContent() {
    std::ostringstream oss;
    
    WritePdfHeader(oss);
    WritePdfPages(oss);
    WritePdfResources(oss);
    WritePdfXref(oss);
    
    return oss.str();
}

void PdfDevice::WritePdfHeader(std::ostream& os) {
    os << "%PDF-1.4\n";
    os << "%\xE2\xE3\xCF\xD3\n";
}

void PdfDevice::WritePdfPages(std::ostream& os) {
    for (const auto& page : m_pages) {
        os << m_nextObjectId << " 0 obj\n";
        os << "<< /Type /Page /Parent 2 0 R ";
        os << "/MediaBox [0 0 " << page->width << " " << page->height << "] ";
        os << "/Contents " << (m_nextObjectId + 1) << " 0 R ";
        os << "/Resources << /Font << >> >> >>\n";
        os << "endobj\n";
        m_nextObjectId++;
        
        std::string content = page->content.str();
        os << m_nextObjectId << " 0 obj\n";
        os << "<< /Length " << content.size() << " >>\n";
        os << "stream\n";
        os << content;
        os << "endstream\n";
        os << "endobj\n";
        m_nextObjectId++;
    }
}

void PdfDevice::WritePdfResources(std::ostream& os) {
    os << "2 0 obj\n";
    os << "<< /Type /Pages /Kids [ ";
    for (size_t i = 0; i < m_pages.size(); ++i) {
        os << (3 + static_cast<int>(i) * 2) << " 0 R ";
    }
    os << "] /Count " << m_pages.size() << " >>\n";
    os << "endobj\n";
}

void PdfDevice::WritePdfXref(std::ostream& os) {
    os << "xref\n";
    os << "0 " << (m_nextObjectId) << "\n";
    os << "0000000000 65535 f \n";
}

void PdfDevice::WritePdfTrailer(std::ostream& os, int objectCount) {
    os << "trailer\n";
    os << "<< /Size " << objectCount << " /Root 1 0 R ";
    if (!m_title.empty()) {
        os << "/Info << /Title (" << m_title << ") ";
        if (!m_author.empty()) os << "/Author (" << m_author << ") ";
        if (!m_subject.empty()) os << "/Subject (" << m_subject << ") ";
        if (!m_creator.empty()) os << "/Creator (" << m_creator << ") ";
        os << ">> ";
    }
    os << ">>\n";
    os << "startxref\n";
    os << "0\n";
    os << "%%EOF\n";
}

}
}
