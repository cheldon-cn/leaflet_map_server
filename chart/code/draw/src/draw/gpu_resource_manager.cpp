#include <ogc/draw/gpu_resource_manager.h>
#include <cstring>
#include <algorithm>

namespace ogc {
namespace draw {

GPUResourceBase::GPUResourceBase(GPUHandle handle, size_t size)
    : m_handle(handle)
    , m_size(size)
{
}

GPUResourceBase::GPUResourceBase(GPUResourceBase&& other) noexcept
    : m_handle(other.m_handle)
    , m_size(other.m_size)
{
    other.m_handle = INVALID_GPU_HANDLE;
    other.m_size = 0;
}

GPUResourceBase& GPUResourceBase::operator=(GPUResourceBase&& other) noexcept
{
    if (this != &other) {
        Reset();
        m_handle = other.m_handle;
        m_size = other.m_size;
        other.m_handle = INVALID_GPU_HANDLE;
        other.m_size = 0;
    }
    return *this;
}

GPUResourceBase::~GPUResourceBase()
{
    Reset();
}

void GPUResourceBase::Reset()
{
    if (m_handle != INVALID_GPU_HANDLE) {
        GPUResourceManager::Instance().DestroyResource(*this);
        m_handle = INVALID_GPU_HANDLE;
        m_size = 0;
    }
}

GPUHandle GPUResourceBase::Release()
{
    GPUHandle handle = m_handle;
    m_handle = INVALID_GPU_HANDLE;
    m_size = 0;
    return handle;
}

GPUResourceManager& GPUResourceManager::Instance()
{
    static GPUResourceManager instance;
    return instance;
}

GPUResourceManager::GPUResourceManager()
    : m_available(false)
{
}

GPUResourceManager::~GPUResourceManager()
{
    ReleaseAll();
}

GPUHandle GPUResourceManager::AllocateHandle()
{
    return m_nextHandle.fetch_add(1);
}

void GPUResourceManager::RegisterResource(GPUHandle handle, size_t size)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_resourceSizes[handle] = size;
    m_totalMemory += size;
    m_resourceCount++;
}

void GPUResourceManager::UnregisterResource(GPUHandle handle)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_resourceSizes.find(handle);
    if (it != m_resourceSizes.end()) {
        m_totalMemory -= it->second;
        m_resourceCount--;
        m_resourceSizes.erase(it);
    }
    m_bufferTypes.erase(handle);
}

VertexBuffer GPUResourceManager::CreateVertexBuffer(size_t size, const void* data)
{
    if (!m_available || m_totalMemory + size > m_maxMemory) {
        return VertexBuffer();
    }
    
    GPUHandle handle = AllocateHandle();
    RegisterResource(handle, size);
    m_bufferTypes[handle] = BufferType::kVertex;
    
    return VertexBuffer(handle, size);
}

IndexBuffer GPUResourceManager::CreateIndexBuffer(size_t size, const void* data)
{
    if (!m_available || m_totalMemory + size > m_maxMemory) {
        return IndexBuffer();
    }
    
    GPUHandle handle = AllocateHandle();
    RegisterResource(handle, size);
    m_bufferTypes[handle] = BufferType::kIndex;
    
    return IndexBuffer(handle, size);
}

UniformBuffer GPUResourceManager::CreateUniformBuffer(size_t size, const void* data)
{
    if (!m_available || m_totalMemory + size > m_maxMemory) {
        return UniformBuffer();
    }
    
    GPUHandle handle = AllocateHandle();
    RegisterResource(handle, size);
    m_bufferTypes[handle] = BufferType::kUniform;
    
    return UniformBuffer(handle, size);
}

Texture GPUResourceManager::CreateTexture2D(int width, int height, TextureFormat format, const void* data)
{
    if (!m_available) {
        return Texture();
    }
    
    size_t size = static_cast<size_t>(width) * height * 4;
    if (m_totalMemory + size > m_maxMemory) {
        return Texture();
    }
    
    GPUHandle handle = AllocateHandle();
    RegisterResource(handle, size);
    
    TextureDesc desc;
    desc.width = width;
    desc.height = height;
    desc.format = format;
    
    return Texture(handle, size, desc);
}

Texture GPUResourceManager::CreateTexture2DFromImage(const uint8_t* pixelData, int width, int height, int channels)
{
    TextureFormat format = (channels == 4) ? TextureFormat::kRGBA8 : TextureFormat::kRGB8;
    return CreateTexture2D(width, height, format, pixelData);
}

Shader GPUResourceManager::CreateShader(ShaderType type, const std::string& source)
{
    if (!m_available || source.empty()) {
        return Shader();
    }
    
    GPUHandle handle = AllocateHandle();
    RegisterResource(handle, 0);
    
    return Shader(handle, type);
}

ShaderProgram GPUResourceManager::CreateShaderProgram(const Shader& vertexShader, const Shader& fragmentShader)
{
    if (!m_available || !vertexShader.IsValid() || !fragmentShader.IsValid()) {
        return ShaderProgram();
    }
    
    GPUHandle handle = AllocateHandle();
    RegisterResource(handle, 0);
    
    return ShaderProgram(handle);
}

ShaderProgram GPUResourceManager::CreateShaderProgram(const std::string& vertexSource, const std::string& fragmentSource)
{
    Shader vertexShader = CreateShader(ShaderType::kVertex, vertexSource);
    Shader fragmentShader = CreateShader(ShaderType::kFragment, fragmentSource);
    
    if (!vertexShader.IsValid() || !fragmentShader.IsValid()) {
        return ShaderProgram();
    }
    
    return CreateShaderProgram(vertexShader, fragmentShader);
}

Framebuffer GPUResourceManager::CreateFramebuffer(int width, int height, bool hasDepth, bool hasStencil)
{
    if (!m_available || width <= 0 || height <= 0) {
        return Framebuffer();
    }
    
    size_t size = static_cast<size_t>(width) * height * 4;
    if (m_totalMemory + size > m_maxMemory) {
        return Framebuffer();
    }
    
    GPUHandle handle = AllocateHandle();
    RegisterResource(handle, size);
    
    FramebufferDesc desc;
    desc.width = width;
    desc.height = height;
    desc.hasDepth = hasDepth;
    desc.hasStencil = hasStencil;
    
    return Framebuffer(handle, desc);
}

void GPUResourceManager::UpdateBuffer(VertexBuffer& buffer, const void* data, size_t size, size_t offset)
{
    if (!buffer.IsValid() || !data || size == 0) {
        return;
    }
}

void GPUResourceManager::UpdateBuffer(IndexBuffer& buffer, const void* data, size_t size, size_t offset)
{
    if (!buffer.IsValid() || !data || size == 0) {
        return;
    }
}

void GPUResourceManager::UpdateBuffer(UniformBuffer& buffer, const void* data, size_t size, size_t offset)
{
    if (!buffer.IsValid() || !data || size == 0) {
        return;
    }
}

void GPUResourceManager::UpdateTexture(Texture& texture, const void* data, int x, int y, int width, int height)
{
    if (!texture.IsValid() || !data) {
        return;
    }
}

void GPUResourceManager::DestroyResource(GPUResourceBase& resource)
{
    if (!resource.IsValid()) {
        return;
    }
    
    GPUHandle handle = resource.GetHandle();
    UnregisterResource(handle);
}

void GPUResourceManager::GarbageCollect()
{
}

void GPUResourceManager::ReleaseAll()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_resourceSizes.clear();
    m_bufferTypes.clear();
    m_totalMemory = 0;
    m_resourceCount = 0;
}

GPUResourceStats GPUResourceManager::GetStats() const
{
    GPUResourceStats stats;
    stats.totalMemory = m_totalMemory.load();
    stats.totalResourceCount = m_resourceCount.load();
    
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));
    for (const auto& pair : m_bufferTypes) {
        switch (pair.second) {
            case BufferType::kVertex:
                stats.vertexBufferCount++;
                break;
            case BufferType::kIndex:
                stats.indexBufferCount++;
                break;
            case BufferType::kUniform:
                stats.uniformBufferCount++;
                break;
            default:
                break;
        }
    }
    
    return stats;
}

}
}
