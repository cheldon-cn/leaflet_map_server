#include "ogc/draw/gpu_accelerated_engine.h"
#include "ogc/draw/draw_device.h"
#include <cmath>
#include <algorithm>

namespace ogc {
namespace draw {

namespace {
const char* kBasicVertexShader = R"(
#version 330 core
layout(location = 0) in vec2 a_position;
layout(location = 1) in vec4 a_color;
uniform mat3 u_transform;
out vec4 v_color;
void main() {
    vec3 pos = u_transform * vec3(a_position, 1.0);
    gl_Position = vec4(pos.xy, 0.0, 1.0);
    v_color = a_color;
}
)";

const char* kBasicFragmentShader = R"(
#version 330 core
in vec4 v_color;
out vec4 FragColor;
void main() {
    FragColor = v_color;
}
)";

const char* kLineVertexShader = R"(
#version 330 core
layout(location = 0) in vec2 a_position;
layout(location = 1) in vec4 a_color;
uniform mat3 u_transform;
uniform float u_lineWidth;
out vec4 v_color;
void main() {
    vec3 pos = u_transform * vec3(a_position, 1.0);
    gl_Position = vec4(pos.xy, 0.0, 1.0);
    v_color = a_color;
}
)";

const char* kLineFragmentShader = R"(
#version 330 core
in vec4 v_color;
out vec4 FragColor;
void main() {
    FragColor = v_color;
}
)";
}

GPUAcceleratedEngine::GPUAcceleratedEngine(DrawDevice* device)
    : m_device(device)
    , m_active(false)
    , m_opacity(1.0)
    , m_tolerance(0.1)
    , m_antialiasing(true)
    , m_initialized(false)
{
}

GPUAcceleratedEngine::~GPUAcceleratedEngine() {
    if (m_active) {
        End();
    }
    CleanupResources();
}

DrawResult GPUAcceleratedEngine::Begin() {
    if (!m_device) {
        return DrawResult::kDeviceError;
    }
    
    if (!m_initialized) {
        InitializeShaders();
        InitializeBuffers();
        m_initialized = true;
    }
    
    m_active = true;
    m_transform = TransformMatrix::Identity();
    m_clipRegion = Region();
    m_opacity = 1.0;
    m_tolerance = 0.1;
    m_antialiasing = true;
    
    while (!m_stateStack.empty()) {
        m_stateStack.pop();
    }
    
    m_currentBatch = DrawBatch();
    m_batches.clear();
    
    SetupDefaultShader();
    
    return DrawResult::kSuccess;
}

void GPUAcceleratedEngine::End() {
    if (m_active) {
        FlushBatch();
        m_active = false;
    }
}

DrawResult GPUAcceleratedEngine::DrawPoints(const double* x, const double* y, int count,
                                            const DrawStyle& style) {
    if (!m_active) return DrawResult::kInvalidState;
    if (!x || !y || count <= 0) return DrawResult::kInvalidParameter;
    
    m_currentStyle = style;
    
    double r = style.pen.width / 2.0;
    const int segments = 16;
    
    for (int i = 0; i < count; ++i) {
        Point pt = TransformPoint(x[i], y[i]);
        
        std::vector<float> vertices;
        std::vector<uint32_t> indices;
        
        uint32_t baseIndex = 0;
        for (int j = 0; j < segments; ++j) {
            double angle = 2.0 * 3.14159265358979323846 * j / segments;
            double px = pt.x + r * std::cos(angle);
            double py = pt.y + r * std::sin(angle);
            
            AddVertex(vertices, px, py, style.pen.color, m_opacity);
            baseIndex++;
        }
        
        for (int j = 0; j < segments - 2; ++j) {
            indices.push_back(0);
            indices.push_back(j + 1);
            indices.push_back(j + 2);
        }
        
        m_currentBatch.vertices.insert(m_currentBatch.vertices.end(), 
                                       vertices.begin(), vertices.end());
        m_currentBatch.indices.insert(m_currentBatch.indices.end(),
                                      indices.begin(), indices.end());
    }
    
    m_currentBatch.style = style;
    m_currentBatch.primitiveType = 4;
    
    return DrawResult::kSuccess;
}

DrawResult GPUAcceleratedEngine::DrawLines(const double* x1, const double* y1,
                                           const double* x2, const double* y2, int count,
                                           const DrawStyle& style) {
    if (!m_active) return DrawResult::kInvalidState;
    if (!x1 || !y1 || !x2 || !y2 || count <= 0) return DrawResult::kInvalidParameter;
    
    m_currentStyle = style;
    
    for (int i = 0; i < count; ++i) {
        Point p1 = TransformPoint(x1[i], y1[i]);
        Point p2 = TransformPoint(x2[i], y2[i]);
        
        AddVertex(m_currentBatch.vertices, p1.x, p1.y, style.pen.color, m_opacity);
        AddVertex(m_currentBatch.vertices, p2.x, p2.y, style.pen.color, m_opacity);
        
        uint32_t baseIdx = static_cast<uint32_t>(m_currentBatch.vertices.size() / 6 - 2);
        m_currentBatch.indices.push_back(baseIdx);
        m_currentBatch.indices.push_back(baseIdx + 1);
    }
    
    m_currentBatch.style = style;
    m_currentBatch.primitiveType = 1;
    
    return DrawResult::kSuccess;
}

DrawResult GPUAcceleratedEngine::DrawLineString(const double* x, const double* y, int count,
                                                const DrawStyle& style) {
    if (!m_active) return DrawResult::kInvalidState;
    if (!x || !y || count < 2) return DrawResult::kInvalidParameter;
    
    m_currentStyle = style;
    
    for (int i = 0; i < count; ++i) {
        Point pt = TransformPoint(x[i], y[i]);
        AddVertex(m_currentBatch.vertices, pt.x, pt.y, style.pen.color, m_opacity);
        m_currentBatch.indices.push_back(static_cast<uint32_t>(i));
    }
    
    m_currentBatch.style = style;
    m_currentBatch.primitiveType = 2;
    
    return DrawResult::kSuccess;
}

DrawResult GPUAcceleratedEngine::DrawPolygon(const double* x, const double* y, int count,
                                             const DrawStyle& style, bool fill) {
    if (!m_active) return DrawResult::kInvalidState;
    if (!x || !y || count < 3) return DrawResult::kInvalidParameter;
    
    m_currentStyle = style;
    
    for (int i = 0; i < count; ++i) {
        Point pt = TransformPoint(x[i], y[i]);
        AddVertex(m_currentBatch.vertices, pt.x, pt.y, 
                  fill ? style.brush.color : style.pen.color, m_opacity);
    }
    
    if (fill) {
        for (int i = 1; i < count - 1; ++i) {
            m_currentBatch.indices.push_back(0);
            m_currentBatch.indices.push_back(i);
            m_currentBatch.indices.push_back(i + 1);
        }
        m_currentBatch.primitiveType = 4;
    } else {
        for (int i = 0; i < count; ++i) {
            m_currentBatch.indices.push_back(i);
        }
        m_currentBatch.indices.push_back(0);
        m_currentBatch.primitiveType = 2;
    }
    
    return DrawResult::kSuccess;
}

DrawResult GPUAcceleratedEngine::DrawRect(double x, double y, double w, double h,
                                          const DrawStyle& style, bool fill) {
    double coords[] = {x, y, x + w, y, x + w, y + h, x, y + h};
    return DrawPolygon(coords, coords + 1, 4, style, fill);
}

DrawResult GPUAcceleratedEngine::DrawCircle(double cx, double cy, double radius,
                                            const DrawStyle& style, bool fill) {
    return DrawEllipse(cx, cy, radius, radius, style, fill);
}

DrawResult GPUAcceleratedEngine::DrawEllipse(double cx, double cy, double rx, double ry,
                                             const DrawStyle& style, bool fill) {
    if (!m_active) return DrawResult::kInvalidState;
    if (rx <= 0 || ry <= 0) return DrawResult::kInvalidParameter;
    
    m_currentStyle = style;
    
    const int segments = 64;
    uint32_t baseIndex = static_cast<uint32_t>(m_currentBatch.vertices.size() / 6);
    
    for (int i = 0; i < segments; ++i) {
        double angle = 2.0 * 3.14159265358979323846 * i / segments;
        double px = cx + rx * std::cos(angle);
        double py = cy + ry * std::sin(angle);
        
        Point pt = TransformPoint(px, py);
        AddVertex(m_currentBatch.vertices, pt.x, pt.y,
                  fill ? style.brush.color : style.pen.color, m_opacity);
    }
    
    if (fill) {
        for (int i = 1; i < segments - 1; ++i) {
            m_currentBatch.indices.push_back(baseIndex);
            m_currentBatch.indices.push_back(baseIndex + i);
            m_currentBatch.indices.push_back(baseIndex + i + 1);
        }
        m_currentBatch.primitiveType = 4;
    } else {
        for (int i = 0; i < segments; ++i) {
            m_currentBatch.indices.push_back(baseIndex + i);
        }
        m_currentBatch.indices.push_back(baseIndex);
        m_currentBatch.primitiveType = 2;
    }
    
    return DrawResult::kSuccess;
}

DrawResult GPUAcceleratedEngine::DrawArc(double cx, double cy, double rx, double ry,
                                         double startAngle, double sweepAngle,
                                         const DrawStyle& style) {
    if (!m_active) return DrawResult::kInvalidState;
    if (rx <= 0 || ry <= 0) return DrawResult::kInvalidParameter;
    
    m_currentStyle = style;
    
    const int segments = 32;
    uint32_t baseIndex = static_cast<uint32_t>(m_currentBatch.vertices.size() / 6);
    
    for (int i = 0; i <= segments; ++i) {
        double angle = startAngle + sweepAngle * i / segments;
        double px = cx + rx * std::cos(angle);
        double py = cy + ry * std::sin(angle);
        
        Point pt = TransformPoint(px, py);
        AddVertex(m_currentBatch.vertices, pt.x, pt.y, style.pen.color, m_opacity);
        m_currentBatch.indices.push_back(baseIndex + i);
    }
    
    m_currentBatch.primitiveType = 2;
    
    return DrawResult::kSuccess;
}

DrawResult GPUAcceleratedEngine::DrawText(double x, double y, const std::string& text,
                                          const Font& font, const Color& color) {
    if (!m_active) return DrawResult::kInvalidState;
    
    (void)text;
    (void)font;
    (void)color;
    (void)x;
    (void)y;
    
    return DrawResult::kNotImplemented;
}

DrawResult GPUAcceleratedEngine::DrawImage(double x, double y, const Image& image,
                                           double scaleX, double scaleY) {
    if (!m_active) return DrawResult::kInvalidState;
    if (!image.IsValid()) return DrawResult::kInvalidParameter;
    
    return DrawImageRect(x, y, image.GetWidth() * scaleX, image.GetHeight() * scaleY, image);
}

DrawResult GPUAcceleratedEngine::DrawImageRect(double x, double y, double w, double h,
                                               const Image& image) {
    if (!m_active) return DrawResult::kInvalidState;
    if (!image.IsValid()) return DrawResult::kInvalidParameter;
    
    (void)w;
    (void)h;
    
    return DrawResult::kNotImplemented;
}

DrawResult GPUAcceleratedEngine::DrawGeometry(const Geometry* geometry,
                                              const DrawStyle& style) {
    if (!m_active) return DrawResult::kInvalidState;
    if (!geometry) return DrawResult::kInvalidParameter;
    
    (void)geometry;
    (void)style;
    
    return DrawResult::kNotImplemented;
}

void GPUAcceleratedEngine::SetTransform(const TransformMatrix& matrix) {
    m_transform = matrix;
    UpdateUniforms();
}

void GPUAcceleratedEngine::ResetTransform() {
    m_transform = TransformMatrix::Identity();
    UpdateUniforms();
}

void GPUAcceleratedEngine::SetClipRect(double x, double y, double w, double h) {
    m_clipRegion = Region(Rect(x, y, w, h));
}

void GPUAcceleratedEngine::SetClipRegion(const Region& region) {
    m_clipRegion = region;
}

void GPUAcceleratedEngine::ResetClip() {
    m_clipRegion = Region();
}

void GPUAcceleratedEngine::Save(StateFlags flags) {
    EngineState state;
    
    if (flags & static_cast<StateFlags>(StateFlag::kTransform)) {
        state.transform = m_transform;
    }
    if (flags & static_cast<StateFlags>(StateFlag::kClip)) {
        state.clipRegion = m_clipRegion;
    }
    if (flags & static_cast<StateFlags>(StateFlag::kStyle)) {
        state.style = m_currentStyle;
    }
    if (flags & static_cast<StateFlags>(StateFlag::kOpacity)) {
        state.opacity = m_opacity;
    }
    
    m_stateStack.push(state);
}

void GPUAcceleratedEngine::Restore() {
    if (m_stateStack.empty()) return;
    
    EngineState state = m_stateStack.top();
    m_stateStack.pop();
    
    m_transform = state.transform;
    m_clipRegion = state.clipRegion;
    m_currentStyle = state.style;
    m_opacity = state.opacity;
    
    UpdateUniforms();
}

void GPUAcceleratedEngine::SetOpacity(double opacity) {
    m_opacity = std::max(0.0, std::min(1.0, opacity));
}

void GPUAcceleratedEngine::SetTolerance(double tolerance) {
    m_tolerance = std::max(0.001, std::min(1.0, tolerance));
}

void GPUAcceleratedEngine::SetAntialiasingEnabled(bool enabled) {
    m_antialiasing = enabled;
}

TextMetrics GPUAcceleratedEngine::MeasureText(const std::string& text, const Font& font) {
    TextMetrics metrics;
    double fontSize = font.GetSize();
    metrics.width = text.length() * fontSize * 0.6;
    metrics.height = fontSize;
    metrics.ascent = fontSize * 0.8;
    metrics.descent = fontSize * 0.2;
    metrics.lineHeight = fontSize * 1.2;
    return metrics;
}

void GPUAcceleratedEngine::Clear(const Color& color) {
    if (!m_active) return;
    
    (void)color;
}

void GPUAcceleratedEngine::Flush() {
    FlushBatch();
}

bool GPUAcceleratedEngine::SupportsFeature(const std::string& featureName) const {
    if (featureName == "gpu_acceleration") return true;
    if (featureName == "batch_rendering") return true;
    if (featureName == "shader_programs") return true;
    return false;
}

void GPUAcceleratedEngine::FlushBatch() {
    if (m_currentBatch.vertices.empty()) return;
    
    m_batches.push_back(m_currentBatch);
    m_currentBatch = DrawBatch();
}

void GPUAcceleratedEngine::SetupDefaultShader() {
    if (m_basicShader && m_basicShader->IsValid()) {
        m_basicShader.get();
    }
}

void GPUAcceleratedEngine::UpdateUniforms() {
}

Point GPUAcceleratedEngine::TransformPoint(double x, double y) const {
    double outX, outY;
    m_transform.TransformPoint(x, y, outX, outY);
    return Point(outX, outY);
}

void GPUAcceleratedEngine::AddVertex(std::vector<float>& vertices, double x, double y,
                                     const Color& color, double opacity) {
    vertices.push_back(static_cast<float>(x));
    vertices.push_back(static_cast<float>(y));
    vertices.push_back(color.GetRed() / 255.0f);
    vertices.push_back(color.GetGreen() / 255.0f);
    vertices.push_back(color.GetBlue() / 255.0f);
    vertices.push_back(color.GetAlpha() / 255.0f * static_cast<float>(opacity));
}

bool GPUAcceleratedEngine::InitializeShaders() {
    auto& mgr = GPUResourceManager::Instance();
    if (!mgr.IsAvailable()) {
        return true;
    }
    
    m_basicShader = std::unique_ptr<ShaderProgram>(
        new ShaderProgram(mgr.CreateShaderProgram(kBasicVertexShader, kBasicFragmentShader)));
    
    m_lineShader = std::unique_ptr<ShaderProgram>(
        new ShaderProgram(mgr.CreateShaderProgram(kLineVertexShader, kLineFragmentShader)));
    
    return true;
}

bool GPUAcceleratedEngine::InitializeBuffers() {
    auto& pool = RenderMemoryPool::Instance();
    if (!pool.IsAvailable()) {
        return true;
    }
    
    const size_t initialBufferSize = 1024 * 1024;
    
    BufferHandle vbHandle = pool.AllocateVertexBuffer(initialBufferSize);
    BufferHandle ibHandle = pool.AllocateIndexBuffer(initialBufferSize);
    
    m_vertexBuffer = std::unique_ptr<VertexBuffer>(new VertexBuffer(vbHandle, initialBufferSize));
    m_indexBuffer = std::unique_ptr<IndexBuffer>(new IndexBuffer(ibHandle, initialBufferSize));
    
    return true;
}

void GPUAcceleratedEngine::CleanupResources() {
    m_vertexBuffer.reset();
    m_indexBuffer.reset();
    m_basicShader.reset();
    m_lineShader.reset();
}

}
}
