#include "ogc/draw/pdf_device.h"
#include "ogc/point.h"
#include "ogc/linestring.h"
#include "ogc/polygon.h"
#include "ogc/common.h"
#include <fstream>
#include <cmath>
#include <iomanip>

namespace ogc {
namespace draw {

PdfDevice::PdfDevice()
    : m_width(612)
    , m_height(792)
    , m_dpi(72.0)
    , m_state(DeviceState::kCreated)
    , m_drawing(false)
    , m_antialiasing(true)
    , m_opacity(1.0)
    , m_currentPage(-1)
    , m_hasClipRect(false)
    , m_nextObjectId(1)
{
    m_transform = TransformMatrix::Identity();
}

PdfDevice::PdfDevice(int width, int height, double dpi)
    : m_width(width)
    , m_height(height)
    , m_dpi(dpi)
    , m_state(DeviceState::kCreated)
    , m_drawing(false)
    , m_antialiasing(true)
    , m_opacity(1.0)
    , m_currentPage(-1)
    , m_hasClipRect(false)
    , m_nextObjectId(1)
{
    m_transform = TransformMatrix::Identity();
}

PdfDevice::~PdfDevice() {
    if (m_drawing) {
        EndDraw();
    }
}

DrawResult PdfDevice::Initialize() {
    if (m_state == DeviceState::kInitialized) {
        return DrawResult::kSuccess;
    }
    
    m_pages.clear();
    m_fontMap.clear();
    m_imageMap.clear();
    m_nextObjectId = 1;
    
    NewPage(m_width, m_height);
    
    m_state = DeviceState::kInitialized;
    return DrawResult::kSuccess;
}

DrawResult PdfDevice::Finalize() {
    if (m_drawing) {
        EndDraw();
    }
    
    m_pages.clear();
    m_state = DeviceState::kClosed;
    return DrawResult::kSuccess;
}

DeviceState PdfDevice::GetState() const {
    return m_state;
}

bool PdfDevice::IsReady() const {
    return m_state == DeviceState::kInitialized;
}

DrawResult PdfDevice::BeginDraw(const DrawParams& params) {
    if (m_state != DeviceState::kInitialized) {
        return DrawResult::kDeviceNotReady;
    }
    
    if (m_drawing) {
        return DrawResult::kFailed;
    }
    
    m_params = params;
    m_drawing = true;
    
    return DrawResult::kSuccess;
}

DrawResult PdfDevice::EndDraw() {
    if (!m_drawing) {
        return DrawResult::kFailed;
    }
    
    m_drawing = false;
    return DrawResult::kSuccess;
}

DrawResult PdfDevice::AbortDraw() {
    m_drawing = false;
    return DrawResult::kSuccess;
}

bool PdfDevice::IsDrawing() const {
    return m_drawing;
}

void PdfDevice::SetTransform(const TransformMatrix& transform) {
    m_transform = transform;
}

TransformMatrix PdfDevice::GetTransform() const {
    return m_transform;
}

void PdfDevice::ResetTransform() {
    m_transform = TransformMatrix::Identity();
}

void PdfDevice::SetClipRect(double x, double y, double width, double height) {
    m_clipX = x;
    m_clipY = y;
    m_clipWidth = width;
    m_clipHeight = height;
    m_hasClipRect = true;
    
    if (m_currentPage >= 0 && m_currentPage < static_cast<int>(m_pages.size())) {
        double pdfY = PdfY(y + height);
        m_pages[m_currentPage]->content << "q\n";
        m_pages[m_currentPage]->content << std::fixed << std::setprecision(2);
        m_pages[m_currentPage]->content << x << " " << pdfY << " " << width << " " << height << " re W n\n";
    }
}

void PdfDevice::ClearClipRect() {
    m_hasClipRect = false;
    
    if (m_currentPage >= 0 && m_currentPage < static_cast<int>(m_pages.size())) {
        m_pages[m_currentPage]->content << "Q\n";
    }
}

bool PdfDevice::HasClipRect() const {
    return m_hasClipRect;
}

DrawResult PdfDevice::DrawPoint(double x, double y, const DrawStyle& style) {
    if (!m_drawing || m_currentPage < 0) {
        return DrawResult::kFailed;
    }
    
    double tx, ty;
    TransformPoint(x, y, tx, ty);
    double pdfY = PdfY(ty);
    
    double radius = style.stroke.width > 0 ? style.stroke.width : 3.0;
    
    m_pages[m_currentPage]->content << "q\n";
    m_pages[m_currentPage]->content << ColorToPdf(Color(style.fill.color));
    m_pages[m_currentPage]->content << std::fixed << std::setprecision(2);
    m_pages[m_currentPage]->content << tx - radius << " " << pdfY - radius << " " << radius * 2 << " " << radius * 2 << " re f\n";
    m_pages[m_currentPage]->content << "Q\n";
    
    return DrawResult::kSuccess;
}

DrawResult PdfDevice::DrawLine(double x1, double y1, double x2, double y2, const DrawStyle& style) {
    if (!m_drawing || m_currentPage < 0) {
        return DrawResult::kFailed;
    }
    
    double tx1, ty1, tx2, ty2;
    TransformPoint(x1, y1, tx1, ty1);
    TransformPoint(x2, y2, tx2, ty2);
    
    double pdfY1 = PdfY(ty1);
    double pdfY2 = PdfY(ty2);
    
    m_pages[m_currentPage]->content << "q\n";
    m_pages[m_currentPage]->content << ColorToPdf(Color(style.stroke.color));
    m_pages[m_currentPage]->content << std::fixed << std::setprecision(2);
    m_pages[m_currentPage]->content << style.stroke.width << " w\n";
    m_pages[m_currentPage]->content << tx1 << " " << pdfY1 << " m " << tx2 << " " << pdfY2 << " l S\n";
    m_pages[m_currentPage]->content << "Q\n";
    
    return DrawResult::kSuccess;
}

DrawResult PdfDevice::DrawRect(double x, double y, double width, double height, const DrawStyle& style) {
    if (!m_drawing || m_currentPage < 0) {
        return DrawResult::kFailed;
    }
    
    double tx, ty;
    TransformPoint(x, y, tx, ty);
    double pdfY = PdfY(ty + height);
    
    m_pages[m_currentPage]->content << "q\n";
    m_pages[m_currentPage]->content << std::fixed << std::setprecision(2);
    
    if (style.fill.visible) {
        m_pages[m_currentPage]->content << ColorToPdf(Color(style.fill.color));
        m_pages[m_currentPage]->content << tx << " " << pdfY << " " << width << " " << height << " re f\n";
    }
    
    if (style.stroke.visible) {
        m_pages[m_currentPage]->content << ColorToPdf(Color(style.stroke.color));
        m_pages[m_currentPage]->content << style.stroke.width << " w\n";
        m_pages[m_currentPage]->content << tx << " " << pdfY << " " << width << " " << height << " re S\n";
    }
    
    m_pages[m_currentPage]->content << "Q\n";
    
    return DrawResult::kSuccess;
}

DrawResult PdfDevice::DrawCircle(double cx, double cy, double radius, const DrawStyle& style) {
    return DrawEllipse(cx, cy, radius, radius, style);
}

DrawResult PdfDevice::DrawEllipse(double cx, double cy, double rx, double ry, const DrawStyle& style) {
    if (!m_drawing || m_currentPage < 0) {
        return DrawResult::kFailed;
    }
    
    double tx, ty;
    TransformPoint(cx, cy, tx, ty);
    double pdfY = PdfY(ty);
    
    double k = 0.5522847498;
    
    m_pages[m_currentPage]->content << "q\n";
    m_pages[m_currentPage]->content << std::fixed << std::setprecision(2);
    
    m_pages[m_currentPage]->content << tx - rx << " " << pdfY << " m\n";
    m_pages[m_currentPage]->content << tx - rx << " " << pdfY + ry * k << " " 
                                    << tx - rx * k << " " << pdfY + ry << " " 
                                    << tx << " " << pdfY + ry << " c\n";
    m_pages[m_currentPage]->content << tx + rx * k << " " << pdfY + ry << " " 
                                    << tx + rx << " " << pdfY + ry * k << " " 
                                    << tx + rx << " " << pdfY << " c\n";
    m_pages[m_currentPage]->content << tx + rx << " " << pdfY - ry * k << " " 
                                    << tx + rx * k << " " << pdfY - ry << " " 
                                    << tx << " " << pdfY - ry << " c\n";
    m_pages[m_currentPage]->content << tx - rx * k << " " << pdfY - ry << " " 
                                    << tx - rx << " " << pdfY - ry * k << " " 
                                    << tx - rx << " " << pdfY << " c\n";
    
    if (style.fill.visible) {
        m_pages[m_currentPage]->content << ColorToPdf(Color(style.fill.color)) << "f\n";
    }
    if (style.stroke.visible) {
        m_pages[m_currentPage]->content << ColorToPdf(Color(style.stroke.color));
        m_pages[m_currentPage]->content << style.stroke.width << " w\nS\n";
    }
    
    m_pages[m_currentPage]->content << "Q\n";
    
    return DrawResult::kSuccess;
}

DrawResult PdfDevice::DrawArc(double cx, double cy, double rx, double ry, 
                               double startAngle, double sweepAngle, const DrawStyle& style) {
    if (!m_drawing || m_currentPage < 0) {
        return DrawResult::kFailed;
    }
    
    double tx, ty;
    TransformPoint(cx, cy, tx, ty);
    double pdfY = PdfY(ty);
    
    const int segments = 36;
    double angleStep = sweepAngle / segments;
    
    m_pages[m_currentPage]->content << "q\n";
    m_pages[m_currentPage]->content << std::fixed << std::setprecision(2);
    m_pages[m_currentPage]->content << ColorToPdf(Color(style.stroke.color));
    m_pages[m_currentPage]->content << style.stroke.width << " w\n";
    
    double angle = startAngle;
    double x1 = tx + rx * std::cos(angle);
    double y1 = pdfY + ry * std::sin(angle);
    m_pages[m_currentPage]->content << x1 << " " << y1 << " m\n";
    
    for (int i = 1; i <= segments; ++i) {
        angle = startAngle + angleStep * i;
        double x2 = tx + rx * std::cos(angle);
        double y2 = pdfY + ry * std::sin(angle);
        m_pages[m_currentPage]->content << x2 << " " << y2 << " l\n";
    }
    
    m_pages[m_currentPage]->content << "S\nQ\n";
    
    return DrawResult::kSuccess;
}

DrawResult PdfDevice::DrawPolygon(const double* x, const double* y, int count, const DrawStyle& style) {
    if (!m_drawing || m_currentPage < 0 || count < 3) {
        return DrawResult::kFailed;
    }
    
    m_pages[m_currentPage]->content << "q\n";
    m_pages[m_currentPage]->content << std::fixed << std::setprecision(2);
    
    double tx, ty;
    TransformPoint(x[0], y[0], tx, ty);
    m_pages[m_currentPage]->content << tx << " " << PdfY(ty) << " m\n";
    
    for (int i = 1; i < count; ++i) {
        TransformPoint(x[i], y[i], tx, ty);
        m_pages[m_currentPage]->content << tx << " " << PdfY(ty) << " l\n";
    }
    
    m_pages[m_currentPage]->content << "h\n";
    
    if (style.fill.visible) {
        m_pages[m_currentPage]->content << ColorToPdf(Color(style.fill.color)) << "f\n";
    }
    if (style.stroke.visible) {
        m_pages[m_currentPage]->content << ColorToPdf(Color(style.stroke.color));
        m_pages[m_currentPage]->content << style.stroke.width << " w\nS\n";
    }
    
    m_pages[m_currentPage]->content << "Q\n";
    
    return DrawResult::kSuccess;
}

DrawResult PdfDevice::DrawPolyline(const double* x, const double* y, int count, const DrawStyle& style) {
    if (!m_drawing || m_currentPage < 0 || count < 2) {
        return DrawResult::kFailed;
    }
    
    m_pages[m_currentPage]->content << "q\n";
    m_pages[m_currentPage]->content << std::fixed << std::setprecision(2);
    m_pages[m_currentPage]->content << ColorToPdf(Color(style.stroke.color));
    m_pages[m_currentPage]->content << style.stroke.width << " w\n";
    
    double tx, ty;
    TransformPoint(x[0], y[0], tx, ty);
    m_pages[m_currentPage]->content << tx << " " << PdfY(ty) << " m\n";
    
    for (int i = 1; i < count; ++i) {
        TransformPoint(x[i], y[i], tx, ty);
        m_pages[m_currentPage]->content << tx << " " << PdfY(ty) << " l\n";
    }
    
    m_pages[m_currentPage]->content << "S\nQ\n";
    
    return DrawResult::kSuccess;
}

DrawResult PdfDevice::DrawGeometry(const Geometry* geometry, const DrawStyle& style) {
    if (!geometry || !m_drawing || m_currentPage < 0) {
        return DrawResult::kInvalidParams;
    }
    
    auto geomType = geometry->GetGeometryType();
    
    switch (geomType) {
        case GeomType::kPoint: {
            auto point = dynamic_cast<const Point*>(geometry);
            if (point) {
                return DrawPoint(point->GetX(), point->GetY(), style);
            }
            break;
        }
        case GeomType::kLineString: {
            auto lineString = dynamic_cast<const LineString*>(geometry);
            if (lineString) {
                int numPoints = lineString->GetNumPoints();
                std::vector<double> x(numPoints), y(numPoints);
                for (int i = 0; i < numPoints; ++i) {
                    auto coord = lineString->GetCoordinateN(i);
                    x[i] = coord.x;
                    y[i] = coord.y;
                }
                return DrawPolyline(x.data(), y.data(), numPoints, style);
            }
            break;
        }
        case GeomType::kPolygon: {
            auto polygon = dynamic_cast<const Polygon*>(geometry);
            if (polygon) {
                auto exteriorRing = polygon->GetExteriorRing();
                if (exteriorRing) {
                    int numPoints = exteriorRing->GetNumPoints();
                    std::vector<double> x(numPoints), y(numPoints);
                    for (int i = 0; i < numPoints; ++i) {
                        auto coord = exteriorRing->GetCoordinateN(i);
                        x[i] = coord.x;
                        y[i] = coord.y;
                    }
                    return DrawPolygon(x.data(), y.data(), numPoints, style);
                }
            }
            break;
        }
        default:
            break;
    }
    
    return DrawResult::kSuccess;
}

DrawResult PdfDevice::DrawText(double x, double y, const std::string& text, 
                                const Font& font, const Color& color) {
    if (!m_drawing || m_currentPage < 0) {
        return DrawResult::kFailed;
    }
    
    double tx, ty;
    TransformPoint(x, y, tx, ty);
    double pdfY = PdfY(ty);
    
    int fontId = AddFont(font);
    
    m_pages[m_currentPage]->content << "q\n";
    m_pages[m_currentPage]->content << "BT\n";
    m_pages[m_currentPage]->content << "/F" << fontId << " " << font.GetSize() << " Tf\n";
    m_pages[m_currentPage]->content << ColorToPdf(color);
    m_pages[m_currentPage]->content << std::fixed << std::setprecision(2);
    m_pages[m_currentPage]->content << tx << " " << pdfY << " Td\n";
    m_pages[m_currentPage]->content << "(" << text << ") Tj\n";
    m_pages[m_currentPage]->content << "ET\n";
    m_pages[m_currentPage]->content << "Q\n";
    
    return DrawResult::kSuccess;
}

DrawResult PdfDevice::DrawTextWithBackground(double x, double y, const std::string& text, 
                                              const Font& font, const Color& textColor, 
                                              const Color& bgColor) {
    double textWidth = text.length() * font.GetSize() * 0.6;
    double textHeight = font.GetSize() * 1.2;
    
    DrawStyle bgStyle;
    bgStyle.fill.visible = true;
    bgStyle.fill.color = bgColor.GetRGBA();
    bgStyle.stroke.visible = false;
    
    DrawResult result = DrawRect(x, y - font.GetSize() * 0.2, textWidth, textHeight, bgStyle);
    if (result != DrawResult::kSuccess) {
        return result;
    }
    
    return DrawText(x, y, text, font, textColor);
}

DrawResult PdfDevice::DrawImage(double x, double y, double width, double height, 
                                 const uint8_t* data, int dataWidth, int dataHeight, int channels) {
    if (!m_drawing || m_currentPage < 0 || !data) {
        return DrawResult::kInvalidParams;
    }
    
    int imageId = AddImage(dataWidth, dataHeight, channels, data);
    
    double tx, ty;
    TransformPoint(x, y, tx, ty);
    double pdfY = PdfY(ty + height);
    
    m_pages[m_currentPage]->content << "q\n";
    m_pages[m_currentPage]->content << std::fixed << std::setprecision(2);
    m_pages[m_currentPage]->content << width << " 0 0 " << height << " " << tx << " " << pdfY << " cm\n";
    m_pages[m_currentPage]->content << "/Im" << imageId << " Do\n";
    m_pages[m_currentPage]->content << "Q\n";
    
    return DrawResult::kSuccess;
}

DrawResult PdfDevice::DrawImageRegion(double destX, double destY, double destWidth, double destHeight,
                                       const uint8_t* data, int dataWidth, int dataHeight, int channels,
                                       int srcX, int srcY, int srcWidth, int srcHeight) {
    if (!data) {
        return DrawResult::kInvalidParams;
    }
    
    std::vector<uint8_t> regionData(srcWidth * srcHeight * channels);
    
    for (int y = 0; y < srcHeight; ++y) {
        for (int x = 0; x < srcWidth; ++x) {
            int srcIdx = ((srcY + y) * dataWidth + (srcX + x)) * channels;
            int dstIdx = (y * srcWidth + x) * channels;
            for (int c = 0; c < channels; ++c) {
                regionData[dstIdx + c] = data[srcIdx + c];
            }
        }
    }
    
    return DrawImage(destX, destY, destWidth, destHeight, 
                     regionData.data(), srcWidth, srcHeight, channels);
}

void PdfDevice::Clear(const Color& color) {
    if (m_currentPage >= 0 && m_currentPage < static_cast<int>(m_pages.size())) {
        m_pages[m_currentPage]->content.str("");
        m_pages[m_currentPage]->content.clear();
        
        m_pages[m_currentPage]->content << "q\n";
        m_pages[m_currentPage]->content << ColorToPdf(color);
        m_pages[m_currentPage]->content << "0 0 " << m_width << " " << m_height << " re f\n";
        m_pages[m_currentPage]->content << "Q\n";
    }
}

void PdfDevice::Fill(const Color& color) {
    Clear(color);
}

int PdfDevice::GetWidth() const {
    return m_width;
}

int PdfDevice::GetHeight() const {
    return m_height;
}

double PdfDevice::GetDpi() const {
    return m_dpi;
}

void PdfDevice::SetDpi(double dpi) {
    m_dpi = dpi;
}

DrawParams PdfDevice::GetDrawParams() const {
    return m_params;
}

void PdfDevice::SetAntialiasing(bool enable) {
    m_antialiasing = enable;
}

bool PdfDevice::IsAntialiasingEnabled() const {
    return m_antialiasing;
}

void PdfDevice::SetOpacity(double opacity) {
    m_opacity = opacity;
}

double PdfDevice::GetOpacity() const {
    return m_opacity;
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
    auto page = std::make_unique<PdfPage>();
    page->width = width;
    page->height = height;
    m_pages.push_back(std::move(page));
    m_currentPage = static_cast<int>(m_pages.size()) - 1;
}

int PdfDevice::GetCurrentPage() const {
    return m_currentPage;
}

int PdfDevice::GetPageCount() const {
    return static_cast<int>(m_pages.size());
}

bool PdfDevice::SaveToFile(const std::string& filepath) {
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    file << "%PDF-1.4\n";
    file << "%\xE2\xE3\xCF\xD3\n";
    
    std::vector<int> objectOffsets;
    
    int catalogObj = m_nextObjectId++;
    int pagesObj = m_nextObjectId++;
    std::vector<int> pageObjects;
    
    for (size_t i = 0; i < m_pages.size(); ++i) {
        pageObjects.push_back(m_nextObjectId++);
    }
    
    objectOffsets.push_back(static_cast<int>(file.tellp()));
    file << catalogObj << " 0 obj\n";
    file << "<< /Type /Catalog /Pages " << pagesObj << " 0 R >>\n";
    file << "endobj\n";
    
    objectOffsets.push_back(static_cast<int>(file.tellp()));
    file << pagesObj << " 0 obj\n";
    file << "<< /Type /Pages /Kids [";
    for (int pageObj : pageObjects) {
        file << pageObj << " 0 R ";
    }
    file << "] /Count " << m_pages.size() << " >>\n";
    file << "endobj\n";
    
    for (size_t i = 0; i < m_pages.size(); ++i) {
        objectOffsets.push_back(static_cast<int>(file.tellp()));
        file << pageObjects[i] << " 0 obj\n";
        file << "<< /Type /Page /Parent " << pagesObj << " 0 R ";
        file << "/MediaBox [0 0 " << m_pages[i]->width << " " << m_pages[i]->height << "] ";
        file << "/Contents << /Length " << m_pages[i]->content.str().length() << " >> ";
        file << ">>\n";
        file << "endobj\n";
        
        objectOffsets.push_back(static_cast<int>(file.tellp()));
        file << "stream\n";
        file << m_pages[i]->content.str();
        file << "\nendstream\n";
    }
    
    int xrefOffset = static_cast<int>(file.tellp());
    file << "xref\n";
    file << "0 " << objectOffsets.size() + 1 << "\n";
    file << "0000000000 65535 f \n";
    for (int offset : objectOffsets) {
        file << std::setw(10) << std::setfill('0') << offset << " 00000 n \n";
    }
    
    file << "trailer\n";
    file << "<< /Size " << objectOffsets.size() + 1 << " /Root " << catalogObj << " 0 R >>\n";
    file << "startxref\n";
    file << xrefOffset << "\n";
    file << "%%EOF\n";
    
    return true;
}

std::shared_ptr<PdfDevice> PdfDevice::Create(int width, int height, double dpi) {
    auto device = std::make_shared<PdfDevice>(width, height, dpi);
    device->Initialize();
    return device;
}

void PdfDevice::TransformPoint(double x, double y, double& outX, double& outY) const {
    outX = m_transform.GetM00() * x + m_transform.GetM01() * y + m_transform.GetM02();
    outY = m_transform.GetM10() * x + m_transform.GetM11() * y + m_transform.GetM12();
}

double PdfDevice::PdfY(double y) const {
    if (m_currentPage >= 0 && m_currentPage < static_cast<int>(m_pages.size())) {
        return m_pages[m_currentPage]->height - y;
    }
    return m_height - y;
}

std::string PdfDevice::ColorToPdf(const Color& color) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(3);
    
    if (color.GetAlphaF() < 1.0) {
        oss << "/GS" << (int)(color.GetAlphaF() * 100) << " gs ";
    }
    
    oss << color.GetRedF() << " " << color.GetGreenF() << " " << color.GetBlueF() << " rg ";
    oss << color.GetRedF() << " " << color.GetGreenF() << " " << color.GetBlueF() << " RG ";
    
    return oss.str();
}

std::string PdfDevice::StyleToPdf(const DrawStyle& style) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    
    if (style.fill.visible) {
        Color fillColor(style.fill.color);
        oss << fillColor.GetRedF() << " " << fillColor.GetGreenF() << " " << fillColor.GetBlueF() << " rg ";
    }
    if (style.stroke.visible) {
        Color strokeColor(style.stroke.color);
        oss << strokeColor.GetRedF() << " " << strokeColor.GetGreenF() << " " << strokeColor.GetBlueF() << " RG ";
        oss << style.stroke.width << " w ";
    }
    
    return oss.str();
}

int PdfDevice::AddFont(const Font& font) {
    std::string fontKey = font.GetFamily() + "_" + std::to_string(font.GetSize());
    
    auto it = m_fontMap.find(fontKey);
    if (it != m_fontMap.end()) {
        return it->second;
    }
    
    int fontId = m_nextObjectId++;
    m_fontMap[fontKey] = fontId;
    return fontId;
}

int PdfDevice::AddImage(int width, int height, int channels, const uint8_t* data) {
    size_t key = reinterpret_cast<size_t>(data);
    
    auto it = m_imageMap.find(key);
    if (it != m_imageMap.end()) {
        return it->second;
    }
    
    int imageId = m_nextObjectId++;
    m_imageMap[key] = imageId;
    return imageId;
}

}
}
