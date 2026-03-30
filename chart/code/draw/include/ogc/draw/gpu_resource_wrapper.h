#ifndef OGC_DRAW_GPU_RESOURCE_WRAPPER_H
#define OGC_DRAW_GPU_RESOURCE_WRAPPER_H

#include <ogc/draw/export.h>
#include <ogc/draw/gpu_resource_manager.h>
#include <memory>
#include <functional>

namespace ogc {
namespace draw {

template<typename T>
class OGC_DRAW_API GPUResourcePtr {
public:
    GPUResourcePtr() : m_resource(nullptr), m_deleter(nullptr) {}
    
    explicit GPUResourcePtr(T* resource, std::function<void(T*)> deleter = nullptr)
        : m_resource(resource), m_deleter(deleter) {}
    
    GPUResourcePtr(const GPUResourcePtr&) = delete;
    GPUResourcePtr& operator=(const GPUResourcePtr&) = delete;
    
    GPUResourcePtr(GPUResourcePtr&& other) noexcept
        : m_resource(other.m_resource), m_deleter(std::move(other.m_deleter)) {
        other.m_resource = nullptr;
        other.m_deleter = nullptr;
    }
    
    GPUResourcePtr& operator=(GPUResourcePtr&& other) noexcept {
        if (this != &other) {
            Reset();
            m_resource = other.m_resource;
            m_deleter = std::move(other.m_deleter);
            other.m_resource = nullptr;
            other.m_deleter = nullptr;
        }
        return *this;
    }
    
    ~GPUResourcePtr() {
        Reset();
    }
    
    T* Get() const { return m_resource; }
    T* operator->() const { return m_resource; }
    T& operator*() const { return *m_resource; }
    
    explicit operator bool() const { return m_resource != nullptr; }
    bool IsValid() const { return m_resource != nullptr; }
    
    void Reset() {
        if (m_resource && m_deleter) {
            m_deleter(m_resource);
        }
        m_resource = nullptr;
        m_deleter = nullptr;
    }
    
    T* Release() {
        T* temp = m_resource;
        m_resource = nullptr;
        m_deleter = nullptr;
        return temp;
    }
    
private:
    T* m_resource;
    std::function<void(T*)> m_deleter;
};

class OGC_DRAW_API ScopedVertexBuffer {
public:
    ScopedVertexBuffer() : m_size(0) {}
    explicit ScopedVertexBuffer(size_t size);
    ~ScopedVertexBuffer();
    
    ScopedVertexBuffer(const ScopedVertexBuffer&) = delete;
    ScopedVertexBuffer& operator=(const ScopedVertexBuffer&) = delete;
    
    ScopedVertexBuffer(ScopedVertexBuffer&& other) noexcept;
    ScopedVertexBuffer& operator=(ScopedVertexBuffer&& other) noexcept;
    
    bool IsValid() const { return m_buffer.IsValid(); }
    GPUHandle GetHandle() const { return m_buffer.GetHandle(); }
    size_t GetSize() const { return m_size; }
    
    void Update(const void* data, size_t size, size_t offset = 0);
    void Reset();
    
private:
    VertexBuffer m_buffer;
    size_t m_size;
};

class OGC_DRAW_API ScopedIndexBuffer {
public:
    ScopedIndexBuffer() : m_size(0) {}
    explicit ScopedIndexBuffer(size_t size);
    ~ScopedIndexBuffer();
    
    ScopedIndexBuffer(const ScopedIndexBuffer&) = delete;
    ScopedIndexBuffer& operator=(const ScopedIndexBuffer&) = delete;
    
    ScopedIndexBuffer(ScopedIndexBuffer&& other) noexcept;
    ScopedIndexBuffer& operator=(ScopedIndexBuffer&& other) noexcept;
    
    bool IsValid() const { return m_buffer.IsValid(); }
    GPUHandle GetHandle() const { return m_buffer.GetHandle(); }
    size_t GetSize() const { return m_size; }
    
    void Update(const void* data, size_t size, size_t offset = 0);
    void Reset();
    
private:
    IndexBuffer m_buffer;
    size_t m_size;
};

class OGC_DRAW_API ScopedTexture {
public:
    ScopedTexture() : m_width(0), m_height(0) {}
    ScopedTexture(int width, int height, TextureFormat format);
    ~ScopedTexture();
    
    ScopedTexture(const ScopedTexture&) = delete;
    ScopedTexture& operator=(const ScopedTexture&) = delete;
    
    ScopedTexture(ScopedTexture&& other) noexcept;
    ScopedTexture& operator=(ScopedTexture&& other) noexcept;
    
    bool IsValid() const { return m_texture.IsValid(); }
    GPUHandle GetHandle() const { return m_texture.GetHandle(); }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    TextureFormat GetFormat() const { return m_texture.GetFormat(); }
    
    void Update(const void* data, int x = 0, int y = 0, int width = 0, int height = 0);
    void Reset();
    
private:
    Texture m_texture;
    int m_width;
    int m_height;
};

class OGC_DRAW_API ScopedFramebuffer {
public:
    ScopedFramebuffer() : m_width(0), m_height(0) {}
    ScopedFramebuffer(int width, int height, bool hasDepth = true, bool hasStencil = false);
    ~ScopedFramebuffer();
    
    ScopedFramebuffer(const ScopedFramebuffer&) = delete;
    ScopedFramebuffer& operator=(const ScopedFramebuffer&) = delete;
    
    ScopedFramebuffer(ScopedFramebuffer&& other) noexcept;
    ScopedFramebuffer& operator=(ScopedFramebuffer&& other) noexcept;
    
    bool IsValid() const { return m_framebuffer.IsValid(); }
    GPUHandle GetHandle() const { return m_framebuffer.GetHandle(); }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    
    void Reset();
    
private:
    Framebuffer m_framebuffer;
    int m_width;
    int m_height;
};

class OGC_DRAW_API ScopedShader {
public:
    ScopedShader() {}
    ScopedShader(ShaderType type, const std::string& source);
    ~ScopedShader();
    
    ScopedShader(const ScopedShader&) = delete;
    ScopedShader& operator=(const ScopedShader&) = delete;
    
    ScopedShader(ScopedShader&& other) noexcept;
    ScopedShader& operator=(ScopedShader&& other) noexcept;
    
    bool IsValid() const { return m_shader.IsValid(); }
    GPUHandle GetHandle() const { return m_shader.GetHandle(); }
    ShaderType GetType() const { return m_shader.GetType(); }
    
    void Reset();
    
private:
    Shader m_shader;
};

class OGC_DRAW_API ScopedShaderProgram {
public:
    ScopedShaderProgram() {}
    ScopedShaderProgram(const std::string& vertexSource, const std::string& fragmentSource);
    ~ScopedShaderProgram();
    
    ScopedShaderProgram(const ScopedShaderProgram&) = delete;
    ScopedShaderProgram& operator=(const ScopedShaderProgram&) = delete;
    
    ScopedShaderProgram(ScopedShaderProgram&& other) noexcept;
    ScopedShaderProgram& operator=(ScopedShaderProgram&& other) noexcept;
    
    bool IsValid() const { return m_program.IsValid(); }
    GPUHandle GetHandle() const { return m_program.GetHandle(); }
    
    void Reset();
    
private:
    ShaderProgram m_program;
};

}
}

#endif
