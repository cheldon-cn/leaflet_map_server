#ifndef OGC_DRAW_SCOPE_GUARD_H
#define OGC_DRAW_SCOPE_GUARD_H

#include "ogc/draw/export.h"
#include "ogc/draw/draw_context.h"
#include "ogc/draw/draw_result.h"
#include "ogc/draw/draw_style.h"
#include "ogc/draw/geometry.h"
#include <functional>

namespace ogc {
namespace draw {

class OGC_DRAW_API DrawScopeGuard {
public:
    explicit DrawScopeGuard(DrawContext* context);
    ~DrawScopeGuard();

    DrawScopeGuard(const DrawScopeGuard&) = delete;
    DrawScopeGuard& operator=(const DrawScopeGuard&) = delete;
    DrawScopeGuard(DrawScopeGuard&& other) noexcept;
    DrawScopeGuard& operator=(DrawScopeGuard&& other) noexcept;

    DrawResult DrawGeometry(const Geometry* geometry);
    DrawResult DrawGeometry(const Geometry* geometry, const DrawStyle& style);
    DrawResult DrawPoint(double x, double y);
    DrawResult DrawLine(double x1, double y1, double x2, double y2);
    DrawResult DrawPolygon(const double* x, const double* y, int count, bool fill = true);
    DrawResult DrawRect(double x, double y, double w, double h, bool fill = true);
    DrawResult DrawCircle(double cx, double cy, double radius, bool fill = true);
    DrawResult DrawText(double x, double y, const std::string& text);
    DrawResult DrawImage(double x, double y, const Image& image, double scaleX = 1.0, double scaleY = 1.0);

    bool Ok() const { return m_lastError == DrawResult::kSuccess; }
    DrawResult GetLastError() const { return m_lastError; }
    void Commit() { m_committed = true; }
    void Rollback();

    void SetRollbackCallback(std::function<void()> callback);

private:
    DrawContext* m_context;
    DrawResult m_lastError;
    bool m_committed;
    bool m_active;
    std::function<void()> m_rollbackCallback;
};

class OGC_DRAW_API DrawSession {
public:
    DrawSession(DrawContext* context);
    ~DrawSession();

    DrawSession(const DrawSession&) = delete;
    DrawSession& operator=(const DrawSession&) = delete;
    DrawSession(DrawSession&& other) noexcept;
    DrawSession& operator=(DrawSession&& other) noexcept;

    DrawResult Begin();
    void End();
    void Cancel();

    DrawResult DrawGeometry(const Geometry* geometry);
    DrawResult DrawGeometry(const Geometry* geometry, const DrawStyle& style);
    DrawResult DrawPoint(double x, double y);
    DrawResult DrawLine(double x1, double y1, double x2, double y2);
    DrawResult DrawPolygon(const double* x, const double* y, int count, bool fill = true);
    DrawResult DrawRect(double x, double y, double w, double h, bool fill = true);
    DrawResult DrawCircle(double cx, double cy, double radius, bool fill = true);
    DrawResult DrawText(double x, double y, const std::string& text);
    DrawResult DrawImage(double x, double y, const Image& image, double scaleX = 1.0, double scaleY = 1.0);

    DrawResult GetResult() const { return m_result; }
    bool IsActive() const { return m_active; }
    bool Ok() const { return m_result == DrawResult::kSuccess; }

    DrawContext* GetContext() const { return m_context; }

private:
    DrawContext* m_context;
    DrawResult m_result;
    bool m_active;
    bool m_begun;
};

}  
}  

#endif  
