#include "ogc/draw/svg_engine.h"
#include "ogc/draw/svg_device.h"
#include <sstream>
#include <iomanip>
#include <cmath>

namespace ogc {
namespace draw {

SvgEngine::SvgEngine(SvgDevice* device)
    : VectorEngine(device)
    , m_svgDevice(device)
    , m_clipId(0)
{
}

SvgEngine::~SvgEngine()
{
    if (m_active) {
        End();
    }
}

DrawResult SvgEngine::Begin()
{
    DrawResult result = VectorEngine::Begin();
    if (result != DrawResult::kSuccess) {
        return result;
    }

    m_content.str("");
    m_content.clear();
    m_stateStack.clear();
    m_clipId = 0;

    return DrawResult::kSuccess;
}

void SvgEngine::End()
{
    if (m_svgDevice) {
        m_svgDevice->AppendContent(m_content.str());
    }

    VectorEngine::End();
}

void SvgEngine::WritePath(const std::vector<Point>& points, bool closed)
{
    if (points.empty()) return;

    m_content << "  <path d=\"";

    m_content << "M " << std::fixed << std::setprecision(2)
              << points[0].x << " " << points[0].y << " ";

    for (size_t i = 1; i < points.size(); ++i) {
        m_content << "L " << points[i].x << " " << points[i].y << " ";
    }

    if (closed) {
        m_content << "Z";
    }

    m_content << "\" ";
}

void SvgEngine::WriteFill(const Color& color, FillRule rule)
{
    std::string fillRule = (rule == FillRule::kEvenOdd) ? "evenodd" : "nonzero";

    m_content << "fill=\"" << ColorToSvg(color) << "\" ";
    m_content << "fill-rule=\"" << fillRule << "\" ";
    m_content << "fill-opacity=\"" << (color.GetAlpha() / 255.0) << "\" ";

    if (m_opacity < 1.0) {
        m_content << "opacity=\"" << m_opacity << "\" ";
    }

    m_content << "/>\n";
}

void SvgEngine::WriteStroke(const Pen& pen)
{
    m_content << "fill=\"none\" ";
    m_content << "stroke=\"" << ColorToSvg(pen.color) << "\" ";
    m_content << "stroke-width=\"" << pen.width << "\" ";
    m_content << "stroke-opacity=\"" << (pen.color.GetAlpha() / 255.0) << "\" ";

    switch (pen.style) {
    case PenStyle::kSolid:
        break;
    case PenStyle::kDash:
        m_content << "stroke-dasharray=\"5,5\" ";
        break;
    case PenStyle::kDot:
        m_content << "stroke-dasharray=\"1,3\" ";
        break;
    case PenStyle::kDashDot:
        m_content << "stroke-dasharray=\"5,3,1,3\" ";
        break;
    case PenStyle::kDashDotDot:
        m_content << "stroke-dasharray=\"5,3,1,3,1,3\" ";
        break;
    }

    switch (pen.cap) {
    case LineCap::kFlat:
        m_content << "stroke-linecap=\"butt\" ";
        break;
    case LineCap::kRound:
        m_content << "stroke-linecap=\"round\" ";
        break;
    case LineCap::kSquare:
        m_content << "stroke-linecap=\"square\" ";
        break;
    }

    switch (pen.join) {
    case LineJoin::kMiter:
        m_content << "stroke-linejoin=\"miter\" ";
        break;
    case LineJoin::kRound:
        m_content << "stroke-linejoin=\"round\" ";
        break;
    case LineJoin::kBevel:
        m_content << "stroke-linejoin=\"bevel\" ";
        break;
    }

    if (m_opacity < 1.0) {
        m_content << "opacity=\"" << m_opacity << "\" ";
    }

    m_content << "/>\n";
}

void SvgEngine::WriteText(double x, double y, const std::string& text,
                         const Font& font, const Color& color)
{
    m_content << "  <text x=\"" << std::fixed << std::setprecision(2) << x << "\" "
              << "y=\"" << y << "\" "
              << "font-family=\"" << font.GetFamily() << "\" "
              << "font-size=\"" << font.GetSize() << "\" ";

    if (font.GetWeight() >= FontWeight::kBold) {
        m_content << "font-weight=\"bold\" ";
    }
    if (font.IsItalic()) {
        m_content << "font-style=\"italic\" ";
    }

    m_content << "fill=\"" << ColorToSvg(color) << "\" ";

    if (m_opacity < 1.0) {
        m_content << "opacity=\"" << m_opacity << "\" ";
    }

    m_content << ">";

    for (char c : text) {
        switch (c) {
        case '&': m_content << "&amp;"; break;
        case '<': m_content << "&lt;"; break;
        case '>': m_content << "&gt;"; break;
        case '"': m_content << "&quot;"; break;
        case '\'': m_content << "&apos;"; break;
        default: m_content << c; break;
        }
    }

    m_content << "</text>\n";
}

void SvgEngine::WriteImage(double x, double y, const Image& image,
                          double scaleX, double scaleY)
{
    int imgWidth = image.GetWidth();
    int imgHeight = image.GetHeight();
    int imgChannels = image.GetChannels();
    const uint8_t* imgData = image.GetData();
    size_t dataSize = image.GetDataSize();

    m_content << "  <image x=\"" << std::fixed << std::setprecision(2) << x << "\" "
              << "y=\"" << y << "\" "
              << "width=\"" << imgWidth * scaleX << "\" "
              << "height=\"" << imgHeight * scaleY << "\" ";

    std::string format = (imgChannels == 4) ? "png" : "jpeg";
    std::string base64 = EncodeBase64(imgData, dataSize);

    m_content << "href=\"data:image/" << format << ";base64," << base64 << "\" ";

    if (m_opacity < 1.0) {
        m_content << "opacity=\"" << m_opacity << "\" ";
    }

    m_content << "/>\n";
}

void SvgEngine::DoSave()
{
    std::stringstream state;
    state << "transform=\"matrix("
          << m_transform.m[0][0] << " " << m_transform.m[1][0] << " "
          << m_transform.m[0][1] << " " << m_transform.m[1][1] << " "
          << m_transform.m[0][2] << " " << m_transform.m[1][2] << ")\" ";
    m_stateStack.push_back(state.str());

    m_content << "  <g " << state.str() << ">\n";
}

void SvgEngine::DoRestore()
{
    if (!m_stateStack.empty()) {
        m_stateStack.pop_back();
    }
    m_content << "  </g>\n";
}

void SvgEngine::DoSetTransform(const TransformMatrix& matrix)
{
}

void SvgEngine::DoSetClip(const Region& region)
{
}

std::string SvgEngine::ColorToSvg(const Color& color) const
{
    std::stringstream ss;
    ss << "#"
       << std::hex << std::setfill('0')
       << std::setw(2) << static_cast<int>(color.GetRed())
       << std::setw(2) << static_cast<int>(color.GetGreen())
       << std::setw(2) << static_cast<int>(color.GetBlue());
    return ss.str();
}

std::string SvgEngine::PointsToPath(const std::vector<Point>& points, bool closed) const
{
    std::stringstream ss;
    if (points.empty()) return "";

    ss << "M " << points[0].x << " " << points[0].y << " ";
    for (size_t i = 1; i < points.size(); ++i) {
        ss << "L " << points[i].x << " " << points[i].y << " ";
    }
    if (closed) {
        ss << "Z";
    }
    return ss.str();
}

std::string SvgEngine::EncodeBase64(const unsigned char* data, size_t len) const
{
    static const char* base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    std::string result;
    result.reserve(((len + 2) / 3) * 4);

    for (size_t i = 0; i < len; i += 3) {
        unsigned int n = (data[i] << 16);
        if (i + 1 < len) n |= (data[i + 1] << 8);
        if (i + 2 < len) n |= data[i + 2];

        result.push_back(base64_chars[(n >> 18) & 0x3F]);
        result.push_back(base64_chars[(n >> 12) & 0x3F]);
        result.push_back((i + 1 < len) ? base64_chars[(n >> 6) & 0x3F] : '=');
        result.push_back((i + 2 < len) ? base64_chars[n & 0x3F] : '=');
    }

    return result;
}

} // namespace draw
} // namespace ogc
