#include "ogc/draw/gpu_resource_wrapper.h"
#include "ogc/draw/gpu_resource_manager.h"

namespace ogc {
namespace draw {

ScopedVertexBuffer::ScopedVertexBuffer(size_t size)
    : m_size(size)
{
    auto& mgr = GPUResourceManager::Instance();
    m_buffer = mgr.CreateVertexBuffer(size);
}

ScopedVertexBuffer::~ScopedVertexBuffer() {
    Reset();
}

ScopedVertexBuffer::ScopedVertexBuffer(ScopedVertexBuffer&& other) noexcept
    : m_buffer(std::move(other.m_buffer)), m_size(other.m_size)
{
    other.m_size = 0;
}

ScopedVertexBuffer& ScopedVertexBuffer::operator=(ScopedVertexBuffer&& other) noexcept {
    if (this != &other) {
        Reset();
        m_buffer = std::move(other.m_buffer);
        m_size = other.m_size;
        other.m_size = 0;
    }
    return *this;
}

void ScopedVertexBuffer::Update(const void* data, size_t size, size_t offset) {
    auto& mgr = GPUResourceManager::Instance();
    mgr.UpdateBuffer(m_buffer, data, size, offset);
}

void ScopedVertexBuffer::Reset() {
    if (m_buffer.IsValid()) {
        auto& mgr = GPUResourceManager::Instance();
        mgr.DestroyResource(m_buffer);
        m_buffer = VertexBuffer();
        m_size = 0;
    }
}

ScopedIndexBuffer::ScopedIndexBuffer(size_t size)
    : m_size(size)
{
    auto& mgr = GPUResourceManager::Instance();
    m_buffer = mgr.CreateIndexBuffer(size);
}

ScopedIndexBuffer::~ScopedIndexBuffer() {
    Reset();
}

ScopedIndexBuffer::ScopedIndexBuffer(ScopedIndexBuffer&& other) noexcept
    : m_buffer(std::move(other.m_buffer)), m_size(other.m_size)
{
    other.m_size = 0;
}

ScopedIndexBuffer& ScopedIndexBuffer::operator=(ScopedIndexBuffer&& other) noexcept {
    if (this != &other) {
        Reset();
        m_buffer = std::move(other.m_buffer);
        m_size = other.m_size;
        other.m_size = 0;
    }
    return *this;
}

void ScopedIndexBuffer::Update(const void* data, size_t size, size_t offset) {
    auto& mgr = GPUResourceManager::Instance();
    mgr.UpdateBuffer(m_buffer, data, size, offset);
}

void ScopedIndexBuffer::Reset() {
    if (m_buffer.IsValid()) {
        auto& mgr = GPUResourceManager::Instance();
        mgr.DestroyResource(m_buffer);
        m_buffer = IndexBuffer();
        m_size = 0;
    }
}

ScopedTexture::ScopedTexture(int width, int height, TextureFormat format)
    : m_width(width), m_height(height)
{
    auto& mgr = GPUResourceManager::Instance();
    m_texture = mgr.CreateTexture2D(width, height, format);
}

ScopedTexture::~ScopedTexture() {
    Reset();
}

ScopedTexture::ScopedTexture(ScopedTexture&& other) noexcept
    : m_texture(std::move(other.m_texture)), m_width(other.m_width), m_height(other.m_height)
{
    other.m_width = 0;
    other.m_height = 0;
}

ScopedTexture& ScopedTexture::operator=(ScopedTexture&& other) noexcept {
    if (this != &other) {
        Reset();
        m_texture = std::move(other.m_texture);
        m_width = other.m_width;
        m_height = other.m_height;
        other.m_width = 0;
        other.m_height = 0;
    }
    return *this;
}

void ScopedTexture::Update(const void* data, int x, int y, int width, int height) {
    auto& mgr = GPUResourceManager::Instance();
    mgr.UpdateTexture(m_texture, data, x, y, width, height);
}

void ScopedTexture::Reset() {
    if (m_texture.IsValid()) {
        auto& mgr = GPUResourceManager::Instance();
        mgr.DestroyResource(m_texture);
        m_texture = Texture();
        m_width = 0;
        m_height = 0;
    }
}

ScopedFramebuffer::ScopedFramebuffer(int width, int height, bool hasDepth, bool hasStencil)
    : m_width(width), m_height(height)
{
    auto& mgr = GPUResourceManager::Instance();
    m_framebuffer = mgr.CreateFramebuffer(width, height, hasDepth, hasStencil);
}

ScopedFramebuffer::~ScopedFramebuffer() {
    Reset();
}

ScopedFramebuffer::ScopedFramebuffer(ScopedFramebuffer&& other) noexcept
    : m_framebuffer(std::move(other.m_framebuffer)), m_width(other.m_width), m_height(other.m_height)
{
    other.m_width = 0;
    other.m_height = 0;
}

ScopedFramebuffer& ScopedFramebuffer::operator=(ScopedFramebuffer&& other) noexcept {
    if (this != &other) {
        Reset();
        m_framebuffer = std::move(other.m_framebuffer);
        m_width = other.m_width;
        m_height = other.m_height;
        other.m_width = 0;
        other.m_height = 0;
    }
    return *this;
}

void ScopedFramebuffer::Reset() {
    if (m_framebuffer.IsValid()) {
        auto& mgr = GPUResourceManager::Instance();
        mgr.DestroyResource(m_framebuffer);
        m_framebuffer = Framebuffer();
        m_width = 0;
        m_height = 0;
    }
}

ScopedShader::ScopedShader(ShaderType type, const std::string& source) {
    auto& mgr = GPUResourceManager::Instance();
    m_shader = mgr.CreateShader(type, source);
}

ScopedShader::~ScopedShader() {
    Reset();
}

ScopedShader::ScopedShader(ScopedShader&& other) noexcept
    : m_shader(std::move(other.m_shader))
{
}

ScopedShader& ScopedShader::operator=(ScopedShader&& other) noexcept {
    if (this != &other) {
        Reset();
        m_shader = std::move(other.m_shader);
    }
    return *this;
}

void ScopedShader::Reset() {
    if (m_shader.IsValid()) {
        auto& mgr = GPUResourceManager::Instance();
        mgr.DestroyResource(m_shader);
        m_shader = Shader();
    }
}

ScopedShaderProgram::ScopedShaderProgram(const std::string& vertexSource, const std::string& fragmentSource) {
    auto& mgr = GPUResourceManager::Instance();
    m_program = mgr.CreateShaderProgram(vertexSource, fragmentSource);
}

ScopedShaderProgram::~ScopedShaderProgram() {
    Reset();
}

ScopedShaderProgram::ScopedShaderProgram(ScopedShaderProgram&& other) noexcept
    : m_program(std::move(other.m_program))
{
}

ScopedShaderProgram& ScopedShaderProgram::operator=(ScopedShaderProgram&& other) noexcept {
    if (this != &other) {
        Reset();
        m_program = std::move(other.m_program);
    }
    return *this;
}

void ScopedShaderProgram::Reset() {
    if (m_program.IsValid()) {
        auto& mgr = GPUResourceManager::Instance();
        mgr.DestroyResource(m_program);
        m_program = ShaderProgram();
    }
}

}
}
