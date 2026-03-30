#include "ogc/draw/draw_scope_guard.h"
#include "ogc/draw/draw_context.h"
#include "ogc/draw/draw_engine.h"
#include "ogc/draw/draw_device.h"

namespace ogc {
namespace draw {

DrawScopeGuard::DrawScopeGuard(DrawContext* context)
    : m_context(context)
    , m_lastError(DrawResult::kSuccess)
    , m_committed(false)
    , m_active(true)
    , m_rollbackCallback(nullptr)
{
    if (m_context) {
        m_context->Save();
    }
}

DrawScopeGuard::~DrawScopeGuard() {
    if (m_active && !m_committed) {
        Rollback();
    }
}

DrawScopeGuard::DrawScopeGuard(DrawScopeGuard&& other) noexcept
    : m_context(other.m_context)
    , m_lastError(other.m_lastError)
    , m_committed(other.m_committed)
    , m_active(other.m_active)
    , m_rollbackCallback(std::move(other.m_rollbackCallback))
{
    other.m_context = nullptr;
    other.m_active = false;
}

DrawScopeGuard& DrawScopeGuard::operator=(DrawScopeGuard&& other) noexcept {
    if (this != &other) {
        if (m_active && !m_committed) {
            Rollback();
        }
        m_context = other.m_context;
        m_lastError = other.m_lastError;
        m_committed = other.m_committed;
        m_active = other.m_active;
        m_rollbackCallback = std::move(other.m_rollbackCallback);
        other.m_context = nullptr;
        other.m_active = false;
    }
    return *this;
}

DrawResult DrawScopeGuard::DrawGeometry(const Geometry* geometry) {
    if (!m_context || !m_active) {
        m_lastError = DrawResult::kInvalidState;
        return m_lastError;
    }
    m_lastError = m_context->DrawGeometry(geometry);
    return m_lastError;
}

DrawResult DrawScopeGuard::DrawGeometry(const Geometry* geometry, const DrawStyle& style) {
    if (!m_context || !m_active) {
        m_lastError = DrawResult::kInvalidState;
        return m_lastError;
    }
    m_lastError = m_context->DrawGeometry(geometry, style);
    return m_lastError;
}

DrawResult DrawScopeGuard::DrawPoint(double x, double y) {
    if (!m_context || !m_active) {
        m_lastError = DrawResult::kInvalidState;
        return m_lastError;
    }
    m_lastError = m_context->DrawPoint(x, y);
    return m_lastError;
}

DrawResult DrawScopeGuard::DrawLine(double x1, double y1, double x2, double y2) {
    if (!m_context || !m_active) {
        m_lastError = DrawResult::kInvalidState;
        return m_lastError;
    }
    m_lastError = m_context->DrawLine(x1, y1, x2, y2);
    return m_lastError;
}

DrawResult DrawScopeGuard::DrawPolygon(const double* x, const double* y, int count, bool fill) {
    if (!m_context || !m_active) {
        m_lastError = DrawResult::kInvalidState;
        return m_lastError;
    }
    m_lastError = m_context->DrawPolygon(x, y, count, fill);
    return m_lastError;
}

DrawResult DrawScopeGuard::DrawRect(double x, double y, double w, double h, bool fill) {
    if (!m_context || !m_active) {
        m_lastError = DrawResult::kInvalidState;
        return m_lastError;
    }
    m_lastError = m_context->DrawRect(x, y, w, h, fill);
    return m_lastError;
}

DrawResult DrawScopeGuard::DrawCircle(double cx, double cy, double radius, bool fill) {
    if (!m_context || !m_active) {
        m_lastError = DrawResult::kInvalidState;
        return m_lastError;
    }
    m_lastError = m_context->DrawCircle(cx, cy, radius, fill);
    return m_lastError;
}

DrawResult DrawScopeGuard::DrawText(double x, double y, const std::string& text) {
    if (!m_context || !m_active) {
        m_lastError = DrawResult::kInvalidState;
        return m_lastError;
    }
    m_lastError = m_context->DrawText(x, y, text);
    return m_lastError;
}

DrawResult DrawScopeGuard::DrawImage(double x, double y, const Image& image, double scaleX, double scaleY) {
    if (!m_context || !m_active) {
        m_lastError = DrawResult::kInvalidState;
        return m_lastError;
    }
    m_lastError = m_context->DrawImage(x, y, image, scaleX, scaleY);
    return m_lastError;
}

void DrawScopeGuard::Rollback() {
    if (m_context && m_active) {
        m_context->Restore();
        if (m_rollbackCallback) {
            m_rollbackCallback();
        }
    }
    m_active = false;
}

void DrawScopeGuard::SetRollbackCallback(std::function<void()> callback) {
    m_rollbackCallback = std::move(callback);
}

DrawSession::DrawSession(DrawContext* context)
    : m_context(context)
    , m_result(DrawResult::kSuccess)
    , m_active(false)
    , m_begun(false)
{
}

DrawSession::~DrawSession() {
    if (m_active) {
        End();
    }
}

DrawSession::DrawSession(DrawSession&& other) noexcept
    : m_context(other.m_context)
    , m_result(other.m_result)
    , m_active(other.m_active)
    , m_begun(other.m_begun)
{
    other.m_context = nullptr;
    other.m_active = false;
    other.m_begun = false;
}

DrawSession& DrawSession::operator=(DrawSession&& other) noexcept {
    if (this != &other) {
        if (m_active) {
            End();
        }
        m_context = other.m_context;
        m_result = other.m_result;
        m_active = other.m_active;
        m_begun = other.m_begun;
        other.m_context = nullptr;
        other.m_active = false;
        other.m_begun = false;
    }
    return *this;
}

DrawResult DrawSession::Begin() {
    if (!m_context) {
        m_result = DrawResult::kInvalidState;
        return m_result;
    }
    if (m_begun) {
        m_result = DrawResult::kInvalidState;
        return m_result;
    }
    m_result = m_context->Begin();
    if (m_result == DrawResult::kSuccess) {
        m_active = true;
        m_begun = true;
    }
    return m_result;
}

void DrawSession::End() {
    if (m_context && m_active) {
        m_context->End();
    }
    m_active = false;
    m_begun = false;
}

void DrawSession::Cancel() {
    if (m_context && m_active) {
        m_context->End();
    }
    m_result = DrawResult::kCancelled;
    m_active = false;
    m_begun = false;
}

DrawResult DrawSession::DrawGeometry(const Geometry* geometry) {
    if (!m_context || !m_active) {
        m_result = DrawResult::kInvalidState;
        return m_result;
    }
    m_result = m_context->DrawGeometry(geometry);
    return m_result;
}

DrawResult DrawSession::DrawGeometry(const Geometry* geometry, const DrawStyle& style) {
    if (!m_context || !m_active) {
        m_result = DrawResult::kInvalidState;
        return m_result;
    }
    m_result = m_context->DrawGeometry(geometry, style);
    return m_result;
}

DrawResult DrawSession::DrawPoint(double x, double y) {
    if (!m_context || !m_active) {
        m_result = DrawResult::kInvalidState;
        return m_result;
    }
    m_result = m_context->DrawPoint(x, y);
    return m_result;
}

DrawResult DrawSession::DrawLine(double x1, double y1, double x2, double y2) {
    if (!m_context || !m_active) {
        m_result = DrawResult::kInvalidState;
        return m_result;
    }
    m_result = m_context->DrawLine(x1, y1, x2, y2);
    return m_result;
}

DrawResult DrawSession::DrawPolygon(const double* x, const double* y, int count, bool fill) {
    if (!m_context || !m_active) {
        m_result = DrawResult::kInvalidState;
        return m_result;
    }
    m_result = m_context->DrawPolygon(x, y, count, fill);
    return m_result;
}

DrawResult DrawSession::DrawRect(double x, double y, double w, double h, bool fill) {
    if (!m_context || !m_active) {
        m_result = DrawResult::kInvalidState;
        return m_result;
    }
    m_result = m_context->DrawRect(x, y, w, h, fill);
    return m_result;
}

DrawResult DrawSession::DrawCircle(double cx, double cy, double radius, bool fill) {
    if (!m_context || !m_active) {
        m_result = DrawResult::kInvalidState;
        return m_result;
    }
    m_result = m_context->DrawCircle(cx, cy, radius, fill);
    return m_result;
}

DrawResult DrawSession::DrawText(double x, double y, const std::string& text) {
    if (!m_context || !m_active) {
        m_result = DrawResult::kInvalidState;
        return m_result;
    }
    m_result = m_context->DrawText(x, y, text);
    return m_result;
}

DrawResult DrawSession::DrawImage(double x, double y, const Image& image, double scaleX, double scaleY) {
    if (!m_context || !m_active) {
        m_result = DrawResult::kInvalidState;
        return m_result;
    }
    m_result = m_context->DrawImage(x, y, image, scaleX, scaleY);
    return m_result;
}

}  
}  
