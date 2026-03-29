#ifndef OGC_DRAW_SVG_ENGINE_H
#define OGC_DRAW_SVG_ENGINE_H

#include "ogc/draw/vector_engine.h"
#include "ogc/draw/svg_device.h"
#include "ogc/draw/export.h"
#include <sstream>

namespace ogc {
namespace draw {

class OGC_DRAW_API SvgEngine : public VectorEngine {
public:
    explicit SvgEngine(SvgDevice* device);
    ~SvgEngine() override;

    std::string GetName() const override { return "SvgEngine"; }

    DrawResult Begin() override;
    void End() override;

protected:
    void WritePath(const std::vector<Point>& points, bool closed) override;
    void WriteFill(const Color& color, FillRule rule) override;
    void WriteStroke(const Pen& pen) override;
    void WriteText(double x, double y, const std::string& text,
                  const Font& font, const Color& color) override;
    void WriteImage(double x, double y, const Image& image,
                   double scaleX, double scaleY) override;
    void DoSave() override;
    void DoRestore() override;
    void DoSetTransform(const TransformMatrix& matrix) override;
    void DoSetClip(const Region& region) override;

private:
    std::string ColorToSvg(const Color& color) const;
    std::string PointsToPath(const std::vector<Point>& points, bool closed) const;
    std::string EncodeBase64(const unsigned char* data, size_t len) const;

    SvgDevice* m_svgDevice;
    std::stringstream m_content;
    std::vector<std::string> m_stateStack;
    int m_clipId;
};

} // namespace draw
} // namespace ogc

#endif // OGC_DRAW_SVG_ENGINE_H
