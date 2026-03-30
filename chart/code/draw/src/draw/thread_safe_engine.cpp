#include "ogc/draw/thread_safe_engine.h"

namespace ogc {
namespace draw {

ThreadSafeEngine::ThreadSafeEngine(std::unique_ptr<DrawEngine> engine)
    : m_engine(std::move(engine))
{
}

ThreadSafeEngine::~ThreadSafeEngine() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_engine && m_engine->IsActive()) {
        m_engine->End();
    }
}

std::string ThreadSafeEngine::GetName() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_engine ? m_engine->GetName() : "";
}

EngineType ThreadSafeEngine::GetType() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_engine ? m_engine->GetType() : EngineType::kSimple2D;
}

DrawDevice* ThreadSafeEngine::GetDevice() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_engine ? m_engine->GetDevice() : nullptr;
}

DrawResult ThreadSafeEngine::Begin() {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_engine ? m_engine->Begin() : DrawResult::kDeviceError;
}

void ThreadSafeEngine::End() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_engine) {
        m_engine->End();
    }
}

bool ThreadSafeEngine::IsActive() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_engine ? m_engine->IsActive() : false;
}

DrawResult ThreadSafeEngine::DrawPoints(const double* x, const double* y, int count, const DrawStyle& style) {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_engine ? m_engine->DrawPoints(x, y, count, style) : DrawResult::kDeviceError;
}

DrawResult ThreadSafeEngine::DrawLines(const double* x1, const double* y1, const double* x2, const double* y2, int count, const DrawStyle& style) {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_engine ? m_engine->DrawLines(x1, y1, x2, y2, count, style) : DrawResult::kDeviceError;
}

DrawResult ThreadSafeEngine::DrawLineString(const double* x, const double* y, int count, const DrawStyle& style) {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_engine ? m_engine->DrawLineString(x, y, count, style) : DrawResult::kDeviceError;
}

DrawResult ThreadSafeEngine::DrawPolygon(const double* x, const double* y, int count, const DrawStyle& style, bool fill) {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_engine ? m_engine->DrawPolygon(x, y, count, style, fill) : DrawResult::kDeviceError;
}

DrawResult ThreadSafeEngine::DrawRect(double x, double y, double width, double height, const DrawStyle& style, bool fill) {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_engine ? m_engine->DrawRect(x, y, width, height, style, fill) : DrawResult::kDeviceError;
}

DrawResult ThreadSafeEngine::DrawCircle(double cx, double cy, double r, const DrawStyle& style, bool fill) {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_engine ? m_engine->DrawCircle(cx, cy, r, style, fill) : DrawResult::kDeviceError;
}

DrawResult ThreadSafeEngine::DrawEllipse(double cx, double cy, double rx, double ry, const DrawStyle& style, bool fill) {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_engine ? m_engine->DrawEllipse(cx, cy, rx, ry, style, fill) : DrawResult::kDeviceError;
}

DrawResult ThreadSafeEngine::DrawArc(double cx, double cy, double rx, double ry, double startAngle, double sweepAngle, const DrawStyle& style) {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_engine ? m_engine->DrawArc(cx, cy, rx, ry, startAngle, sweepAngle, style) : DrawResult::kDeviceError;
}

DrawResult ThreadSafeEngine::DrawText(double x, double y, const std::string& text, const Font& font, const Color& color) {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_engine ? m_engine->DrawText(x, y, text, font, color) : DrawResult::kDeviceError;
}

DrawResult ThreadSafeEngine::DrawImage(double x, double y, const Image& image, double scaleX, double scaleY) {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_engine ? m_engine->DrawImage(x, y, image, scaleX, scaleY) : DrawResult::kDeviceError;
}

DrawResult ThreadSafeEngine::DrawImageRect(double x, double y, double w, double h, const Image& image) {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_engine ? m_engine->DrawImageRect(x, y, w, h, image) : DrawResult::kDeviceError;
}

DrawResult ThreadSafeEngine::DrawGeometry(const Geometry* geometry, const DrawStyle& style) {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_engine ? m_engine->DrawGeometry(geometry, style) : DrawResult::kDeviceError;
}

void ThreadSafeEngine::SetTransform(const TransformMatrix& matrix) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_engine) {
        m_engine->SetTransform(matrix);
    }
}

TransformMatrix ThreadSafeEngine::GetTransform() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_engine ? m_engine->GetTransform() : TransformMatrix::Identity();
}

void ThreadSafeEngine::ResetTransform() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_engine) {
        m_engine->ResetTransform();
    }
}

void ThreadSafeEngine::SetClipRect(double x, double y, double width, double height) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_engine) {
        m_engine->SetClipRect(x, y, width, height);
    }
}

void ThreadSafeEngine::SetClipRegion(const Region& region) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_engine) {
        m_engine->SetClipRegion(region);
    }
}

Region ThreadSafeEngine::GetClipRegion() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_engine ? m_engine->GetClipRegion() : Region();
}

void ThreadSafeEngine::ResetClip() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_engine) {
        m_engine->ResetClip();
    }
}

void ThreadSafeEngine::SetOpacity(double opacity) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_engine) {
        m_engine->SetOpacity(opacity);
    }
}

double ThreadSafeEngine::GetOpacity() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_engine ? m_engine->GetOpacity() : 1.0;
}

void ThreadSafeEngine::SetTolerance(double tolerance) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_engine) {
        m_engine->SetTolerance(tolerance);
    }
}

double ThreadSafeEngine::GetTolerance() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_engine ? m_engine->GetTolerance() : 0.1;
}

void ThreadSafeEngine::SetAntialiasingEnabled(bool enabled) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_engine) {
        m_engine->SetAntialiasingEnabled(enabled);
    }
}

bool ThreadSafeEngine::IsAntialiasingEnabled() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_engine ? m_engine->IsAntialiasingEnabled() : true;
}

void ThreadSafeEngine::Save(StateFlags flags) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_engine) {
        m_engine->Save(flags);
    }
}

void ThreadSafeEngine::Restore() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_engine) {
        m_engine->Restore();
    }
}

TextMetrics ThreadSafeEngine::MeasureText(const std::string& text, const Font& font) {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_engine ? m_engine->MeasureText(text, font) : TextMetrics();
}

void ThreadSafeEngine::Clear(const Color& color) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_engine) {
        m_engine->Clear(color);
    }
}

void ThreadSafeEngine::Flush() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_engine) {
        m_engine->Flush();
    }
}

bool ThreadSafeEngine::SupportsFeature(const std::string& featureName) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_engine ? m_engine->SupportsFeature(featureName) : false;
}

void ThreadSafeEngine::Lock() {
    m_mutex.lock();
}

void ThreadSafeEngine::Unlock() {
    m_mutex.unlock();
}

bool ThreadSafeEngine::TryLock() {
    return m_mutex.try_lock();
}

}
}
