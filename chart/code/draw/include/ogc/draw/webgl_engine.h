#ifndef OGC_DRAW_WEBGL_ENGINE_H
#define OGC_DRAW_WEBGL_ENGINE_H

#include "ogc/draw/export.h"
#include "ogc/draw/draw_engine.h"

#ifdef __EMSCRIPTEN__

#include <emscripten.h>
#include <GLES3/gl3.h>
#include <string>
#include <memory>
#include <vector>

namespace ogc {
namespace draw {

class OGC_DRAW_API WebGLEngine : public DrawEngine {
public:
    WebGLEngine();
    explicit WebGLEngine(DrawDevice* device);
    ~WebGLEngine() override;
    
    std::string GetName() const override { return "WebGLEngine"; }
    
    bool Begin() override;
    void End() override;
    
    DrawResult DrawPoints(const double* x, const double* y, int count, const DrawStyle& style) override;
    DrawResult DrawLines(const double* x1, const double* y1, const double* x2, const double* y2, int count, const DrawStyle& style) override;
    DrawResult DrawPolygon(const double* x, const double* y, int count, const DrawStyle& style, bool fill = true) override;
    DrawResult DrawText(double x, double y, const std::string& text, const Font& font, const Color& color) override;
    DrawResult DrawImage(double x, double y, const Image& image, double scaleX, double scaleY) override;
    
    void SetTransform(const TransformMatrix& matrix) override;
    void SetClipRegion(const Region& region) override;
    
    void PushState() override;
    void PopState() override;
    
    bool IsGPUAccelerated() const override { return true; }
    
    bool Initialize();
    void SetViewport(int width, int height);
    
    GLuint GetShaderProgram() const { return m_shaderProgram; }

private:
    GLuint m_shaderProgram = 0;
    GLuint m_vbo = 0;
    GLuint m_vao = 0;
    
    TransformMatrix m_transform;
    Region m_clipRegion;
    std::vector<TransformMatrix> m_stateStack;
    
    int m_viewportWidth = 0;
    int m_viewportHeight = 0;
    
    bool CreateShaderProgram();
    bool CompileShader(GLuint shader, const std::string& source);
    void ApplyTransform();
    void ApplyStyle(const DrawStyle& style);
    
    static const char* s_vertexShaderSource;
    static const char* s_fragmentShaderSource;
};

}
}

#endif

#endif
