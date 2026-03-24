#ifndef OGC_DRAW_PDF_DEVICE_H
#define OGC_DRAW_PDF_DEVICE_H

#include "ogc/draw/draw_device.h"
#include <vector>
#include <map>
#include <sstream>

namespace ogc {
namespace draw {

struct PdfPage {
    int width = 0;
    int height = 0;
    std::stringstream content;
    std::map<int, std::string> fonts;
    std::map<int, std::string> images;
    
    PdfPage() = default;
    PdfPage(int w, int h) : width(w), height(h) {}
    PdfPage(const PdfPage&) = delete;
    PdfPage& operator=(const PdfPage&) = delete;
    PdfPage(PdfPage&& other) noexcept
        : width(other.width), height(other.height),
          content(std::move(other.content)),
          fonts(std::move(other.fonts)),
          images(std::move(other.images)) {}
    PdfPage& operator=(PdfPage&& other) noexcept {
        if (this != &other) {
            width = other.width;
            height = other.height;
            content = std::move(other.content);
            fonts = std::move(other.fonts);
            images = std::move(other.images);
        }
        return *this;
    }
};

class OGC_GRAPH_API PdfDevice : public DrawDevice {
public:
    PdfDevice();
    PdfDevice(int width, int height, double dpi = 72.0);
    ~PdfDevice() override;
    
    PdfDevice(const PdfDevice&) = delete;
    PdfDevice& operator=(const PdfDevice&) = delete;
    PdfDevice(PdfDevice&&) = default;
    PdfDevice& operator=(PdfDevice&&) = default;
    
    DeviceType GetType() const override { return DeviceType::kPdf; }
    std::string GetName() const override { return "PdfDevice"; }
    
    DrawResult Initialize() override;
    DrawResult Finalize() override;
    
    DeviceState GetState() const override;
    bool IsReady() const override;
    
    DrawResult BeginDraw(const DrawParams& params) override;
    DrawResult EndDraw() override;
    DrawResult AbortDraw() override;
    
    bool IsDrawing() const override;
    
    void SetTransform(const TransformMatrix& transform) override;
    TransformMatrix GetTransform() const override;
    void ResetTransform() override;
    
    void SetClipRect(double x, double y, double width, double height) override;
    void ClearClipRect() override;
    bool HasClipRect() const override;
    
    DrawResult DrawPoint(double x, double y, const DrawStyle& style) override;
    DrawResult DrawLine(double x1, double y1, double x2, double y2, const DrawStyle& style) override;
    DrawResult DrawRect(double x, double y, double width, double height, const DrawStyle& style) override;
    DrawResult DrawCircle(double cx, double cy, double radius, const DrawStyle& style) override;
    DrawResult DrawEllipse(double cx, double cy, double rx, double ry, const DrawStyle& style) override;
    DrawResult DrawArc(double cx, double cy, double rx, double ry, double startAngle, double sweepAngle, const DrawStyle& style) override;
    DrawResult DrawPolygon(const double* x, const double* y, int count, const DrawStyle& style) override;
    DrawResult DrawPolyline(const double* x, const double* y, int count, const DrawStyle& style) override;
    
    DrawResult DrawGeometry(const Geometry* geometry, const DrawStyle& style) override;
    
    DrawResult DrawText(double x, double y, const std::string& text, const Font& font, const Color& color) override;
    DrawResult DrawTextWithBackground(double x, double y, const std::string& text, const Font& font, const Color& textColor, const Color& bgColor) override;
    
    DrawResult DrawImage(double x, double y, double width, double height, const uint8_t* data, int dataWidth, int dataHeight, int channels) override;
    DrawResult DrawImageRegion(double destX, double destY, double destWidth, double destHeight, const uint8_t* data, int dataWidth, int dataHeight, int channels, int srcX, int srcY, int srcWidth, int srcHeight) override;
    
    void Clear(const Color& color) override;
    void Fill(const Color& color) override;
    
    int GetWidth() const override;
    int GetHeight() const override;
    double GetDpi() const override;
    
    void SetDpi(double dpi) override;
    
    DrawParams GetDrawParams() const override;
    
    void SetAntialiasing(bool enable) override;
    bool IsAntialiasingEnabled() const override;
    
    void SetOpacity(double opacity) override;
    double GetOpacity() const override;
    
    void SetTitle(const std::string& title);
    void SetAuthor(const std::string& author);
    void SetSubject(const std::string& subject);
    void SetCreator(const std::string& creator);
    
    bool SaveToFile(const std::string& filepath);
    
    void NewPage(int width, int height);
    int GetCurrentPage() const;
    int GetPageCount() const;
    
    static std::shared_ptr<PdfDevice> Create(int width, int height, double dpi = 72.0);

private:
    void TransformPoint(double x, double y, double& outX, double& outY) const;
    double PdfY(double y) const;
    std::string ColorToPdf(const Color& color) const;
    std::string StyleToPdf(const DrawStyle& style) const;
    int AddFont(const Font& font);
    int AddImage(int width, int height, int channels, const uint8_t* data);
    std::string GeneratePdfContent();
    void WritePdfHeader(std::ostream& os);
    void WritePdfPages(std::ostream& os);
    void WritePdfResources(std::ostream& os);
    void WritePdfXref(std::ostream& os);
    void WritePdfTrailer(std::ostream& os, int objectCount);
    
    int m_width;
    int m_height;
    double m_dpi;
    DeviceState m_state;
    TransformMatrix m_transform;
    bool m_drawing;
    bool m_antialiasing;
    double m_opacity;
    DrawParams m_params;
    
    std::string m_title;
    std::string m_author;
    std::string m_subject;
    std::string m_creator;
    
    std::vector<std::unique_ptr<PdfPage>> m_pages;
    int m_currentPage;
    
    double m_clipX, m_clipY, m_clipWidth, m_clipHeight;
    bool m_hasClipRect;
    
    int m_nextObjectId;
    std::map<std::string, int> m_fontMap;
    std::map<size_t, int> m_imageMap;
};

using PdfDevicePtr = std::shared_ptr<PdfDevice>;

}
}

#endif
