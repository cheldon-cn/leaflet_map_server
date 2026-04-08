#ifndef OGC_DRAW_GPU_RESOURCE_MANAGER_H
#define OGC_DRAW_GPU_RESOURCE_MANAGER_H

#include "ogc/draw/export.h"
#include "ogc/draw/draw_types.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <functional>

namespace ogc {
namespace draw {

using GPUHandle = uint32_t;

constexpr GPUHandle INVALID_GPU_HANDLE = 0;

enum class BufferType {
    kVertex = 0,
    kIndex = 1,
    kUniform = 2,
    kPixelPack = 3,
    kPixelUnpack = 4
};

enum class TextureFormat {
    kRGB8 = 0,
    kRGBA8 = 1,
    kRGB16F = 2,
    kRGBA16F = 3,
    kRGB32F = 4,
    kRGBA32F = 5,
    kDepth24 = 6,
    kDepth32F = 7,
    kDepth24Stencil8 = 8
};

enum class ShaderType {
    kVertex = 0,
    kFragment = 1,
    kGeometry = 2,
    kCompute = 3
};

enum class MemoryUsage {
    kStatic = 0,
    kDynamic = 1,
    kStream = 2
};

struct BufferDesc {
    size_t size = 0;
    BufferType type = BufferType::kVertex;
    MemoryUsage usage = MemoryUsage::kDynamic;
};

struct TextureDesc {
    int width = 0;
    int height = 0;
    int depth = 1;
    TextureFormat format = TextureFormat::kRGBA8;
    bool generateMipmaps = false;
};

struct ShaderDesc {
    ShaderType type = ShaderType::kVertex;
    std::string source;
    std::string entryPoint = "main";
};

struct FramebufferDesc {
    int width = 0;
    int height = 0;
    int samples = 1;
    bool hasDepth = true;
    bool hasStencil = false;
};

class OGC_DRAW_API GPUResourceBase {
public:
    GPUResourceBase() : m_handle(INVALID_GPU_HANDLE), m_size(0) {}
    explicit GPUResourceBase(GPUHandle handle, size_t size = 0);
    
    GPUResourceBase(const GPUResourceBase&) = delete;
    GPUResourceBase& operator=(const GPUResourceBase&) = delete;
    
    GPUResourceBase(GPUResourceBase&& other) noexcept;
    GPUResourceBase& operator=(GPUResourceBase&& other) noexcept;
    
    virtual ~GPUResourceBase();
    
    GPUHandle GetHandle() const { return m_handle; }
    bool IsValid() const { return m_handle != INVALID_GPU_HANDLE; }
    size_t GetSize() const { return m_size; }
    
    void Reset();
    GPUHandle Release();
    
protected:
    GPUHandle m_handle;
    size_t m_size;
};

class OGC_DRAW_API VertexBuffer : public GPUResourceBase {
public:
    VertexBuffer() = default;
    explicit VertexBuffer(GPUHandle handle, size_t size) : GPUResourceBase(handle, size) {}
};

class OGC_DRAW_API IndexBuffer : public GPUResourceBase {
public:
    IndexBuffer() = default;
    explicit IndexBuffer(GPUHandle handle, size_t size) : GPUResourceBase(handle, size) {}
};

class OGC_DRAW_API UniformBuffer : public GPUResourceBase {
public:
    UniformBuffer() = default;
    explicit UniformBuffer(GPUHandle handle, size_t size) : GPUResourceBase(handle, size) {}
};

class OGC_DRAW_API Texture : public GPUResourceBase {
public:
    Texture() = default;
    explicit Texture(GPUHandle handle, size_t size, const TextureDesc& desc) 
        : GPUResourceBase(handle, size), m_desc(desc) {}
    
    int GetWidth() const { return m_desc.width; }
    int GetHeight() const { return m_desc.height; }
    TextureFormat GetFormat() const { return m_desc.format; }
    
private:
    TextureDesc m_desc;
};

class OGC_DRAW_API Shader : public GPUResourceBase {
public:
    Shader() = default;
    explicit Shader(GPUHandle handle, ShaderType type) 
        : GPUResourceBase(handle, 0), m_type(type) {}
    
    ShaderType GetType() const { return m_type; }
    
private:
    ShaderType m_type;
};

class OGC_DRAW_API ShaderProgram : public GPUResourceBase {
public:
    ShaderProgram() = default;
    explicit ShaderProgram(GPUHandle handle) : GPUResourceBase(handle, 0) {}
};

class OGC_DRAW_API Framebuffer : public GPUResourceBase {
public:
    Framebuffer() = default;
    explicit Framebuffer(GPUHandle handle, const FramebufferDesc& desc) 
        : GPUResourceBase(handle, 0), m_desc(desc) {}
    
    int GetWidth() const { return m_desc.width; }
    int GetHeight() const { return m_desc.height; }
    
private:
    FramebufferDesc m_desc;
};

struct GPUResourceStats {
    size_t totalMemory = 0;
    size_t vertexBufferCount = 0;
    size_t indexBufferCount = 0;
    size_t uniformBufferCount = 0;
    size_t textureCount = 0;
    size_t shaderCount = 0;
    size_t framebufferCount = 0;
    size_t totalResourceCount = 0;
};

class OGC_DRAW_API GPUResourceManager {
public:
    static GPUResourceManager& Instance();
    
    VertexBuffer CreateVertexBuffer(size_t size, const void* data = nullptr);
    IndexBuffer CreateIndexBuffer(size_t size, const void* data = nullptr);
    UniformBuffer CreateUniformBuffer(size_t size, const void* data = nullptr);
    
    Texture CreateTexture2D(int width, int height, TextureFormat format, const void* data = nullptr);
    Texture CreateTexture2DFromImage(const uint8_t* pixelData, int width, int height, int channels);
    
    Shader CreateShader(ShaderType type, const std::string& source);
    ShaderProgram CreateShaderProgram(const Shader& vertexShader, const Shader& fragmentShader);
    ShaderProgram CreateShaderProgram(const std::string& vertexSource, const std::string& fragmentSource);
    
    Framebuffer CreateFramebuffer(int width, int height, bool hasDepth = true, bool hasStencil = false);
    
    void UpdateBuffer(VertexBuffer& buffer, const void* data, size_t size, size_t offset = 0);
    void UpdateBuffer(IndexBuffer& buffer, const void* data, size_t size, size_t offset = 0);
    void UpdateBuffer(UniformBuffer& buffer, const void* data, size_t size, size_t offset = 0);
    
    void UpdateTexture(Texture& texture, const void* data, int x = 0, int y = 0, 
                       int width = 0, int height = 0);
    
    void DestroyResource(GPUResourceBase& resource);
    
    void GarbageCollect();
    void ReleaseAll();
    
    GPUResourceStats GetStats() const;
    size_t GetTotalMemoryUsage() const { return m_totalMemory.load(); }
    size_t GetResourceCount() const { return m_resourceCount.load(); }
    
    bool IsAvailable() const { return m_available; }
    void SetAvailable(bool available) { m_available = available; }
    
    void SetMaxMemory(size_t maxMemory) { m_maxMemory = maxMemory; }
    size_t GetMaxMemory() const { return m_maxMemory; }
    
private:
    GPUResourceManager();
    ~GPUResourceManager();
    
    GPUResourceManager(const GPUResourceManager&) = delete;
    GPUResourceManager& operator=(const GPUResourceManager&) = delete;
    
    GPUHandle AllocateHandle();
    void RegisterResource(GPUHandle handle, size_t size);
    void UnregisterResource(GPUHandle handle);
    
    bool CreateOpenGLBuffer(GPUHandle handle, BufferType type, size_t size, const void* data);
    void DestroyOpenGLBuffer(GPUHandle handle, BufferType type);
    
    bool CreateOpenGLTexture(GPUHandle handle, int width, int height, TextureFormat format, const void* data);
    void DestroyOpenGLTexture(GPUHandle handle);
    
    bool CreateOpenGLShader(GPUHandle handle, ShaderType type, const std::string& source);
    void DestroyOpenGLShader(GPUHandle handle);
    
    bool CreateOpenGLProgram(GPUHandle handle, GPUHandle vertexShader, GPUHandle fragmentShader);
    void DestroyOpenGLProgram(GPUHandle handle);
    
    bool CreateOpenGLFramebuffer(GPUHandle handle, int width, int height, bool hasDepth, bool hasStencil);
    void DestroyOpenGLFramebuffer(GPUHandle handle);
    
    std::atomic<size_t> m_totalMemory{0};
    std::atomic<size_t> m_resourceCount{0};
    std::atomic<GPUHandle> m_nextHandle{1};
    
    std::mutex m_mutex;
    std::unordered_map<GPUHandle, size_t> m_resourceSizes;
    std::unordered_map<GPUHandle, BufferType> m_bufferTypes;
    
    bool m_available = false;
    size_t m_maxMemory = 1024 * 1024 * 1024;
};

}
}

#endif
