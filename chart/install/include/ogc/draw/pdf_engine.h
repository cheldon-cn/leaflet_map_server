#ifndef OGC_DRAW_PDF_ENGINE_H
#define OGC_DRAW_PDF_ENGINE_H

#include <ogc/draw/export.h>
#include <ogc/draw/vector_engine.h>
#include <ogc/draw/pdf_device.h>
#include <string>

namespace ogc {
namespace draw {

class OGC_DRAW_API PdfEngine : public VectorEngine {
public:
    explicit PdfEngine(PdfDevice* device);
    ~PdfEngine() override;
    
    std::string GetName() const override { return "PdfEngine"; }
    
    DrawResult Begin() override;
    void End() override;
    
    void SetTransform(const TransformMatrix& matrix) override;
    void SetClipRegion(const Region& region) override;
    
    void AddLink(double x, double y, double w, double h, const std::string& url);
    void AddBookmark(const std::string& title, int level = 0);
    void SetLayer(const std::string& layerName);
    
protected:
    void WritePath(const std::vector<Point>& points, bool closed) override;
    void WriteFill(const Color& color, FillRule rule) override;
    void WriteStroke(const Pen& pen) override;
    void WriteText(double x, double y, const std::string& text, const Font& font, const Color& color) override;
    void WriteImage(double x, double y, const Image& image, double scaleX, double scaleY) override;
    
    void DoSave() override;
    void DoRestore() override;
    void DoSetTransform(const TransformMatrix& matrix) override;
    void DoSetClip(const Region& region) override;
    
private:
    void SetupPage();
    void ApplyCurrentStyle();
    
    PdfDevice* m_pdfDevice;
    std::string m_currentLayer;
    int m_linkCount;
    int m_bookmarkCount;
};

}
}

#endif
