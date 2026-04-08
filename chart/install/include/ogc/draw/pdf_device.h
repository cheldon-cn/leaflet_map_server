#ifndef OGC_DRAW_PDF_DEVICE_H
#define OGC_DRAW_PDF_DEVICE_H

#include <ogc/draw/export.h>
#include <ogc/draw/draw_device.h>
#include <ogc/draw/draw_style.h>
#include <ogc/draw/transform_matrix.h>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <memory>

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

class OGC_DRAW_API PdfDevice : public DrawDevice {
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
    
    int GetWidth() const override { return m_width; }
    int GetHeight() const override { return m_height; }
    double GetDpi() const override { return m_dpi; }
    int GetColorDepth() const override { return 24; }
    
    DrawResult Initialize() override;
    DrawResult Finalize() override;
    
    DeviceState GetState() const override { return m_state; }
    bool IsReady() const override { return m_state == DeviceState::kReady; }
    
    std::unique_ptr<DrawEngine> CreateEngine() override;
    std::vector<EngineType> GetSupportedEngineTypes() const override;
    void SetPreferredEngineType(EngineType type) override;
    EngineType GetPreferredEngineType() const override;
    
    DeviceCapabilities QueryCapabilities() const override;
    bool IsFeatureAvailable(const std::string& featureName) const override;
    
    bool IsDeviceLost() const override { return false; }
    DrawResult RecoverDevice() override { return DrawResult::kSuccess; }
    
    void SetDpi(double dpi) override;
    
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
    std::string ColorToPdf(uint32_t color) const;
    std::string StyleToPdf(const DrawStyle& style) const;
    int AddFont(const std::string& fontFamily, double fontSize);
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
    EngineType m_preferredEngine;
    
    std::string m_title;
    std::string m_author;
    std::string m_subject;
    std::string m_creator;
    
    std::vector<std::unique_ptr<PdfPage>> m_pages;
    int m_currentPage;
    
    int m_nextObjectId;
    std::map<std::string, int> m_fontMap;
    std::map<size_t, int> m_imageMap;
};

using PdfDevicePtr = std::shared_ptr<PdfDevice>;

}
}

#endif
