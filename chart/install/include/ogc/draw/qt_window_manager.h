#ifndef OGC_DRAW_QT_WINDOW_MANAGER_H
#define OGC_DRAW_QT_WINDOW_MANAGER_H

#include "ogc/draw/export.h"

#ifdef DRAW_HAS_QT

#include <QObject>
#include <QMainWindow>
#include <QWidget>
#include <QDialog>
#include <QString>
#include <QRect>
#include <QSize>
#include <memory>
#include <map>
#include <functional>

namespace ogc {
namespace draw {

class OGC_DRAW_API QtWindowManager : public QObject {
    Q_OBJECT

public:
    using WindowPtr = std::shared_ptr<QWidget>;
    using WindowFactory = std::function<WindowPtr()>;

    static QtWindowManager& Instance();

    bool Initialize();
    void Finalize();
    bool IsInitialized() const { return m_initialized; }

    int RegisterWindow(WindowPtr window);
    void UnregisterWindow(int windowId);
    void UnregisterAllWindows();
    
    WindowPtr GetWindow(int windowId) const;
    QWidget* GetMainWindow() const { return m_mainWindow; }
    void SetMainWindow(QWidget* window) { m_mainWindow = window; }

    int CreateMainWindow(const QString& title, const QRect& geometry);
    int CreateDialog(const QString& title, const QRect& geometry, QWidget* parent = nullptr);
    int CreateWidget(const QRect& geometry, QWidget* parent = nullptr);

    void ShowWindow(int windowId);
    void HideWindow(int windowId);
    void CloseWindow(int windowId);
    void CloseAllWindows();

    void SetWindowTitle(int windowId, const QString& title);
    QString GetWindowTitle(int windowId) const;

    void SetWindowGeometry(int windowId, const QRect& geometry);
    QRect GetWindowGeometry(int windowId) const;

    void SetWindowSize(int windowId, const QSize& size);
    QSize GetWindowSize(int windowId) const;

    void SetWindowPosition(int windowId, const QPoint& pos);
    QPoint GetWindowPosition(int windowId) const;

    void MaximizeWindow(int windowId);
    void MinimizeWindow(int windowId);
    void RestoreWindow(int windowId);
    bool IsWindowMaximized(int windowId) const;
    bool IsWindowMinimized(int windowId) const;

    void SetWindowFlag(int windowId, Qt::WindowFlags flags, bool on = true);
    Qt::WindowFlags GetWindowFlags(int windowId) const;

    int GetActiveWindowId() const;
    void SetActiveWindow(int windowId);

    std::vector<int> GetAllWindowIds() const;
    int GetWindowCount() const;

    void RegisterWindowFactory(const QString& type, WindowFactory factory);
    WindowPtr CreateWindow(const QString& type);

signals:
    void WindowCreated(int windowId);
    void WindowDestroyed(int windowId);
    void WindowActivated(int windowId);
    void WindowDeactivated(int windowId);
    void WindowGeometryChanged(int windowId, const QRect& geometry);

private:
    QtWindowManager();
    ~QtWindowManager() override;
    QtWindowManager(const QtWindowManager&) = delete;
    QtWindowManager& operator=(const QtWindowManager&) = delete;

    int GenerateWindowId();
    bool ValidateWindowId(int windowId) const;

    bool m_initialized = false;
    QWidget* m_mainWindow = nullptr;
    std::map<int, WindowPtr> m_windows;
    std::map<QString, WindowFactory> m_factories;
    int m_nextWindowId = 1;
    int m_activeWindowId = -1;
};

}  
}  

#endif

#endif
