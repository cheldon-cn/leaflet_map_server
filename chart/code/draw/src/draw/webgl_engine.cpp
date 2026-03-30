#ifdef __EMSCRIPTEN__

#include "ogc/draw/webgl_engine.h"
#include <cmath>
#include <vector>

namespace ogc {
namespace draw {

const char* WebGLEngine::s_vertexShaderSource = R"(
    attribute vec2 a_position;
    uniform mat3 u_transform;
    
    void main() {
        vec3 pos = u_transform * vec3(a_position, 1.0);
        gl_Position = vec4(pos.xy, 0.0, 1.0);
    }
)";

const char* WebGLEngine::s_fragmentShaderSource = R"(
    precision mediump float;
    uniform vec4 u_color;
    
    void main() {
        gl_FragColor = u_color;
    }
)";

WebGLEngine::WebGLEngine() {
    m_transform = TransformMatrix::Identity();
}

WebGLEngine::WebGLEngine(DrawDevice* device) 
    : DrawEngine(device) {
    m_transform = TransformMatrix::Identity();
}

WebGLEngine::~WebGLEngine() {
    if (m_vao) {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
    if (m_vbo) {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }
    if (m_shaderProgram) {
        glDeleteProgram(m_shaderProgram);
        m_shaderProgram = 0;
    }
}

bool WebGLEngine::Initialize() {
    if (!CreateShaderProgram()) {
        return false;
    }
    
    glGenBuffers(1, &m_vbo);
    if (!m_vbo) {
        return false;
    }
    
    glGenVertexArrays(1, &m_vao);
    if (!m_vao) {
        return false;
    }
    
    return true;
}

bool WebGLEngine::CreateShaderProgram() {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    if (!vertexShader) {
        return false;
    }
    
    if (!CompileShader(vertexShader, s_vertexShaderSource)) {
        glDeleteShader(vertexShader);
        return false;
    }
    
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    if (!fragmentShader) {
        glDeleteShader(vertexShader);
        return false;
    }
    
    if (!CompileShader(fragmentShader, s_fragmentShaderSource)) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }
    
    m_shaderProgram = glCreateProgram();
    if (!m_shaderProgram) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }
    
    glAttachShader(m_shaderProgram, vertexShader);
    glAttachShader(m_shaderProgram, fragmentShader);
    
    glBindAttribLocation(m_shaderProgram, 0, "a_position");
    
    glLinkProgram(m_shaderProgram);
    
    GLint linkStatus;
    glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &linkStatus);
    if (!linkStatus) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(m_shaderProgram);
        m_shaderProgram = 0;
        return false;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return true;
}

bool WebGLEngine::CompileShader(GLuint shader, const std::string& source) {
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    
    GLint compileStatus;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
    
    return compileStatus != 0;
}

bool WebGLEngine::Begin() {
    if (!m_shaderProgram) {
        if (!Initialize()) {
            return false;
        }
    }
    
    glUseProgram(m_shaderProgram);
    
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    return true;
}

void WebGLEngine::End() {
    glFlush();
}

void WebGLEngine::SetViewport(int width, int height) {
    m_viewportWidth = width;
    m_viewportHeight = height;
    glViewport(0, 0, width, height);
}

void WebGLEngine::ApplyTransform() {
    if (!m_shaderProgram) {
        return;
    }
    
    GLint transformLoc = glGetUniformLocation(m_shaderProgram, "u_transform");
    
    float transformMatrix[9] = {
        static_cast<float>(m_transform.m11), static_cast<float>(m_transform.m12), 0.0f,
        static_cast<float>(m_transform.m21), static_cast<float>(m_transform.m22), 0.0f,
        static_cast<float>(m_transform.dx), static_cast<float>(m_transform.dy), 1.0f
    };
    
    glUniformMatrix3fv(transformLoc, 1, GL_FALSE, transformMatrix);
}

void WebGLEngine::ApplyStyle(const DrawStyle& style) {
    GLint colorLoc = glGetUniformLocation(m_shaderProgram, "u_color");
    
    float r = style.pen.color.r / 255.0f;
    float g = style.pen.color.g / 255.0f;
    float b = style.pen.color.b / 255.0f;
    float a = style.pen.color.a / 255.0f;
    
    glUniform4f(colorLoc, r, g, b, a);
}

DrawResult WebGLEngine::DrawPoints(const double* x, const double* y, int count, const DrawStyle& style) {
    if (!m_shaderProgram || count <= 0) {
        return DrawResult::InvalidParameter;
    }
    
    ApplyTransform();
    ApplyStyle(style);
    
    glPointSize(static_cast<float>(style.pen.width > 0 ? style.pen.width : 1.0));
    
    std::vector<float> vertices;
    vertices.reserve(count * 2);
    
    for (int i = 0; i < count; ++i) {
        vertices.push_back(static_cast<float>(x[i]));
        vertices.push_back(static_cast<float>(y[i]));
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    
    glDrawArrays(GL_POINTS, 0, count);
    
    return DrawResult::Success;
}

DrawResult WebGLEngine::DrawLines(const double* x1, const double* y1, const double* x2, const double* y2, int count, const DrawStyle& style) {
    if (!m_shaderProgram || count <= 0) {
        return DrawResult::InvalidParameter;
    }
    
    ApplyTransform();
    ApplyStyle(style);
    
    glLineWidth(static_cast<float>(style.pen.width > 0 ? style.pen.width : 1.0));
    
    std::vector<float> vertices;
    vertices.reserve(count * 4);
    
    for (int i = 0; i < count; ++i) {
        vertices.push_back(static_cast<float>(x1[i]));
        vertices.push_back(static_cast<float>(y1[i]));
        vertices.push_back(static_cast<float>(x2[i]));
        vertices.push_back(static_cast<float>(y2[i]));
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    
    glDrawArrays(GL_LINES, 0, count * 2);
    
    return DrawResult::Success;
}

DrawResult WebGLEngine::DrawPolygon(const double* x, const double* y, int count, const DrawStyle& style, bool fill) {
    if (!m_shaderProgram || count < 3) {
        return DrawResult::InvalidParameter;
    }
    
    ApplyTransform();
    
    std::vector<float> vertices;
    vertices.reserve(count * 2);
    
    for (int i = 0; i < count; ++i) {
        vertices.push_back(static_cast<float>(x[i]));
        vertices.push_back(static_cast<float>(y[i]));
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    
    if (fill && style.brush.style != BrushStyle::Null) {
        GLint colorLoc = glGetUniformLocation(m_shaderProgram, "u_color");
        float r = style.brush.color.r / 255.0f;
        float g = style.brush.color.g / 255.0f;
        float b = style.brush.color.b / 255.0f;
        float a = style.brush.color.a / 255.0f;
        glUniform4f(colorLoc, r, g, b, a);
        
        glDrawArrays(GL_TRIANGLE_FAN, 0, count);
    }
    
    if (style.pen.style != PenStyle::Null) {
        ApplyStyle(style);
        glLineWidth(static_cast<float>(style.pen.width > 0 ? style.pen.width : 1.0));
        glDrawArrays(GL_LINE_LOOP, 0, count);
    }
    
    return DrawResult::Success;
}

DrawResult WebGLEngine::DrawText(double x, double y, const std::string& text, const Font& font, const Color& color) {
    return DrawResult::NotImplemented;
}

DrawResult WebGLEngine::DrawImage(double x, double y, const Image& image, double scaleX, double scaleY) {
    return DrawResult::NotImplemented;
}

void WebGLEngine::SetTransform(const TransformMatrix& matrix) {
    m_transform = matrix;
    ApplyTransform();
}

void WebGLEngine::SetClipRegion(const Region& region) {
    m_clipRegion = region;
    
    if (region.rects.empty()) {
        glDisable(GL_SCISSOR_TEST);
        return;
    }
    
    glEnable(GL_SCISSOR_TEST);
    
    if (!region.rects.empty()) {
        const auto& rect = region.rects[0];
        glScissor(static_cast<int>(rect.x), 
                  static_cast<int>(m_viewportHeight - rect.y - rect.height),
                  static_cast<int>(rect.width), 
                  static_cast<int>(rect.height));
    }
}

void WebGLEngine::PushState() {
    m_stateStack.push_back(m_transform);
}

void WebGLEngine::PopState() {
    if (!m_stateStack.empty()) {
        m_transform = m_stateStack.back();
        m_stateStack.pop_back();
        ApplyTransform();
    }
}

}
}

#endif
