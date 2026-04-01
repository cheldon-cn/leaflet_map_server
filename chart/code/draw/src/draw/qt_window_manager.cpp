#include "ogc/draw/qt_window_manager.h"

#ifdef DRAW_HAS_QT

#include <QApplication>
#include <QDebug>

namespace ogc {
namespace draw {

QtWindowManager& QtWindowManager::Instance() {
    static QtWindowManager instance;
    return instance;
}

QtWindowManager::QtWindowManager()
    : QObject(nullptr)
{
}

QtWindowManager::~QtWindowManager() {
    Finalize();
}

bool QtWindowManager::Initialize() {
    if (m_initialized) {
        return true;
    }
    
    m_initialized = true;
    return true;
}

void QtWindowManager::Finalize() {
    if (!m_initialized) {
        return;
    }
    
    CloseAllWindows();
    m_windows.clear();
    m_factories.clear();
    m_mainWindow = nullptr;
    m_initialized = false;
}

int QtWindowManager::RegisterWindow(WindowPtr window) {
    if (!window) {
        return -1;
    }
    
    int id = GenerateWindowId();
    m_windows[id] = window;
    emit WindowCreated(id);
    return id;
}

void QtWindowManager::UnregisterWindow(int windowId) {
    auto it = m_windows.find(windowId);
    if (it != m_windows.end()) {
        emit WindowDestroyed(windowId);
        m_windows.erase(it);
    }
}

void QtWindowManager::UnregisterAllWindows() {
    auto ids = GetAllWindowIds();
    for (int id : ids) {
        UnregisterWindow(id);
    }
}

QtWindowManager::WindowPtr QtWindowManager::GetWindow(int windowId) const {
    auto it = m_windows.find(windowId);
    return it != m_windows.end() ? it->second : nullptr;
}

int QtWindowManager::CreateMainWindow(const QString& title, const QRect& geometry) {
    auto window = std::make_shared<QMainWindow>();
    window->setWindowTitle(title);
    window->setGeometry(geometry);
    
    int id = RegisterWindow(window);
    m_mainWindow = window.get();
    return id;
}

int QtWindowManager::CreateDialog(const QString& title, const QRect& geometry, QWidget* parent) {
    auto dialog = std::make_shared<QDialog>(parent ? parent : m_mainWindow);
    dialog->setWindowTitle(title);
    dialog->setGeometry(geometry);
    
    return RegisterWindow(dialog);
}

int QtWindowManager::CreateWidget(const QRect& geometry, QWidget* parent) {
    auto widget = std::make_shared<QWidget>(parent ? parent : m_mainWindow);
    widget->setGeometry(geometry);
    
    return RegisterWindow(widget);
}

void QtWindowManager::ShowWindow(int windowId) {
    auto window = GetWindow(windowId);
    if (window) {
        window->show();
    }
}

void QtWindowManager::HideWindow(int windowId) {
    auto window = GetWindow(windowId);
    if (window) {
        window->hide();
    }
}

void QtWindowManager::CloseWindow(int windowId) {
    auto window = GetWindow(windowId);
    if (window) {
        window->close();
        UnregisterWindow(windowId);
    }
}

void QtWindowManager::CloseAllWindows() {
    auto ids = GetAllWindowIds();
    for (int id : ids) {
        auto window = GetWindow(id);
        if (window) {
            window->close();
        }
    }
    m_windows.clear();
}

void QtWindowManager::SetWindowTitle(int windowId, const QString& title) {
    auto window = GetWindow(windowId);
    if (window) {
        window->setWindowTitle(title);
    }
}

QString QtWindowManager::GetWindowTitle(int windowId) const {
    auto window = GetWindow(windowId);
    return window ? window->windowTitle() : QString();
}

void QtWindowManager::SetWindowGeometry(int windowId, const QRect& geometry) {
    auto window = GetWindow(windowId);
    if (window) {
        window->setGeometry(geometry);
        emit WindowGeometryChanged(windowId, geometry);
    }
}

QRect QtWindowManager::GetWindowGeometry(int windowId) const {
    auto window = GetWindow(windowId);
    return window ? window->geometry() : QRect();
}

void QtWindowManager::SetWindowSize(int windowId, const QSize& size) {
    auto window = GetWindow(windowId);
    if (window) {
        window->resize(size);
    }
}

QSize QtWindowManager::GetWindowSize(int windowId) const {
    auto window = GetWindow(windowId);
    return window ? window->size() : QSize();
}

void QtWindowManager::SetWindowPosition(int windowId, const QPoint& pos) {
    auto window = GetWindow(windowId);
    if (window) {
        window->move(pos);
    }
}

QPoint QtWindowManager::GetWindowPosition(int windowId) const {
    auto window = GetWindow(windowId);
    return window ? window->pos() : QPoint();
}

void QtWindowManager::MaximizeWindow(int windowId) {
    auto window = GetWindow(windowId);
    if (window) {
        window->showMaximized();
    }
}

void QtWindowManager::MinimizeWindow(int windowId) {
    auto window = GetWindow(windowId);
    if (window) {
        window->showMinimized();
    }
}

void QtWindowManager::RestoreWindow(int windowId) {
    auto window = GetWindow(windowId);
    if (window) {
        window->showNormal();
    }
}

bool QtWindowManager::IsWindowMaximized(int windowId) const {
    auto window = GetWindow(windowId);
    return window ? window->isMaximized() : false;
}

bool QtWindowManager::IsWindowMinimized(int windowId) const {
    auto window = GetWindow(windowId);
    return window ? window->isMinimized() : false;
}

void QtWindowManager::SetWindowFlag(int windowId, Qt::WindowFlags flags, bool on) {
    auto window = GetWindow(windowId);
    if (window) {
        if (on) {
            window->setWindowFlags(window->windowFlags() | flags);
        } else {
            window->setWindowFlags(window->windowFlags() & ~flags);
        }
    }
}

Qt::WindowFlags QtWindowManager::GetWindowFlags(int windowId) const {
    auto window = GetWindow(windowId);
    return window ? window->windowFlags() : Qt::WindowFlags();
}

int QtWindowManager::GetActiveWindowId() const {
    return m_activeWindowId;
}

void QtWindowManager::SetActiveWindow(int windowId) {
    if (ValidateWindowId(windowId)) {
        if (m_activeWindowId != -1) {
            emit WindowDeactivated(m_activeWindowId);
        }
        m_activeWindowId = windowId;
        emit WindowActivated(windowId);
    }
}

std::vector<int> QtWindowManager::GetAllWindowIds() const {
    std::vector<int> ids;
    ids.reserve(m_windows.size());
    for (const auto& pair : m_windows) {
        ids.push_back(pair.first);
    }
    return ids;
}

int QtWindowManager::GetWindowCount() const {
    return static_cast<int>(m_windows.size());
}

void QtWindowManager::RegisterWindowFactory(const QString& type, WindowFactory factory) {
    m_factories[type] = factory;
}

QtWindowManager::WindowPtr QtWindowManager::CreateWindow(const QString& type) {
    auto it = m_factories.find(type);
    if (it != m_factories.end() && it->second) {
        auto window = it->second();
        if (window) {
            RegisterWindow(window);
            return window;
        }
    }
    return nullptr;
}

int QtWindowManager::GenerateWindowId() {
    return m_nextWindowId++;
}

bool QtWindowManager::ValidateWindowId(int windowId) const {
    return m_windows.find(windowId) != m_windows.end();
}

}  
}  

#endif
