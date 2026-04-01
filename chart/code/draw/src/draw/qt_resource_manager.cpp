#include "ogc/draw/qt_resource_manager.h"

#ifdef DRAW_HAS_QT

#include <QFileInfo>
#include <QDirIterator>
#include <QTextStream>
#include <QResource>
#include <QDebug>

namespace ogc {
namespace draw {

QtResourceManager& QtResourceManager::Instance() {
    static QtResourceManager instance;
    return instance;
}

QtResourceManager::QtResourceManager()
    : QObject(nullptr)
{
    m_pixmapCache.setMaxCost(m_pixmapCacheSize);
    m_imageCache.setMaxCost(m_imageCacheSize);
}

QtResourceManager::~QtResourceManager() {
    Finalize();
}

bool QtResourceManager::Initialize(const QString& resourcePath) {
    if (m_initialized) {
        return true;
    }
    
    if (!resourcePath.isEmpty()) {
        SetResourcePath(resourcePath);
    }
    
    m_initialized = true;
    return true;
}

void QtResourceManager::Finalize() {
    if (!m_initialized) {
        return;
    }
    
    ClearAllCaches();
    m_aliases.clear();
    m_searchPaths.clear();
    m_resourcePath.clear();
    m_initialized = false;
}

void QtResourceManager::SetResourcePath(const QString& path) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_resourcePath = path;
    if (!m_resourcePath.isEmpty() && !m_searchPaths.contains(path)) {
        m_searchPaths.prepend(path);
    }
}

void QtResourceManager::AddSearchPath(const QString& path) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!path.isEmpty() && !m_searchPaths.contains(path)) {
        m_searchPaths.append(path);
    }
}

void QtResourceManager::RemoveSearchPath(const QString& path) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_searchPaths.removeAll(path);
}

QStringList QtResourceManager::GetSearchPaths() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_searchPaths;
}

QPixmap QtResourceManager::LoadPixmap(const QString& name) {
    QString resolvedName = ResolveAlias(name);
    
    QPixmap* cached = m_pixmapCache.object(resolvedName);
    if (cached) {
        return *cached;
    }
    
    QString filePath = FindResourceFile(resolvedName);
    if (filePath.isEmpty()) {
        emit ResourceLoadFailed(name, "File not found");
        return QPixmap();
    }
    
    QPixmap pixmap;
    if (!pixmap.load(filePath)) {
        emit ResourceLoadFailed(name, "Failed to load pixmap");
        return QPixmap();
    }
    
    m_pixmapCache.insert(resolvedName, new QPixmap(pixmap));
    emit ResourceLoaded(name);
    return pixmap;
}

QImage QtResourceManager::LoadImage(const QString& name) {
    QString resolvedName = ResolveAlias(name);
    
    QImage* cached = m_imageCache.object(resolvedName);
    if (cached) {
        return *cached;
    }
    
    QString filePath = FindResourceFile(resolvedName);
    if (filePath.isEmpty()) {
        emit ResourceLoadFailed(name, "File not found");
        return QImage();
    }
    
    QImage image;
    if (!image.load(filePath)) {
        emit ResourceLoadFailed(name, "Failed to load image");
        return QImage();
    }
    
    m_imageCache.insert(resolvedName, new QImage(image));
    emit ResourceLoaded(name);
    return image;
}

QIcon QtResourceManager::LoadIcon(const QString& name) {
    QPixmap pixmap = LoadPixmap(name);
    return QIcon(pixmap);
}

QFont QtResourceManager::LoadFont(const QString& name, int pointSize) {
    QString filePath = FindResourceFile(name);
    if (filePath.isEmpty()) {
        return QFont();
    }
    
    int id = QFontDatabase::addApplicationFont(filePath);
    if (id < 0) {
        emit ResourceLoadFailed(name, "Failed to load font");
        return QFont();
    }
    
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    QFont font(family);
    if (pointSize > 0) {
        font.setPointSize(pointSize);
    }
    
    emit ResourceLoaded(name);
    return font;
}

QColor QtResourceManager::LoadColor(const QString& name) {
    QString filePath = FindResourceFile(name);
    if (filePath.isEmpty()) {
        return QColor();
    }
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit ResourceLoadFailed(name, "Failed to open color file");
        return QColor();
    }
    
    QTextStream in(&file);
    QString colorStr = in.readLine().trimmed();
    file.close();
    
    QColor color(colorStr);
    if (!color.isValid()) {
        color = QColor::fromString(colorStr);
    }
    
    emit ResourceLoaded(name);
    return color;
}

QString QtResourceManager::LoadTextFile(const QString& name) {
    QString filePath = FindResourceFile(name);
    if (filePath.isEmpty()) {
        emit ResourceLoadFailed(name, "File not found");
        return QString();
    }
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit ResourceLoadFailed(name, "Failed to open file");
        return QString();
    }
    
    QTextStream in(&file);
    QString content = in.readAll();
    file.close();
    
    emit ResourceLoaded(name);
    return content;
}

QByteArray QtResourceManager::LoadBinaryFile(const QString& name) {
    QString filePath = FindResourceFile(name);
    if (filePath.isEmpty()) {
        emit ResourceLoadFailed(name, "File not found");
        return QByteArray();
    }
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        emit ResourceLoadFailed(name, "Failed to open file");
        return QByteArray();
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    emit ResourceLoaded(name);
    return data;
}

bool QtResourceManager::SavePixmap(const QString& name, const QPixmap& pixmap, const char* format) {
    QString filePath = GetResourcePath(name);
    if (!EnsureDirectoryExists(QFileInfo(filePath).path())) {
        return false;
    }
    
    return pixmap.save(filePath, format);
}

bool QtResourceManager::SaveImage(const QString& name, const QImage& image, const char* format) {
    QString filePath = GetResourcePath(name);
    if (!EnsureDirectoryExists(QFileInfo(filePath).path())) {
        return false;
    }
    
    return image.save(filePath, format);
}

bool QtResourceManager::SaveTextFile(const QString& name, const QString& content) {
    QString filePath = GetResourcePath(name);
    if (!EnsureDirectoryExists(QFileInfo(filePath).path())) {
        return false;
    }
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    
    QTextStream out(&file);
    out << content;
    file.close();
    return true;
}

bool QtResourceManager::SaveBinaryFile(const QString& name, const QByteArray& data) {
    QString filePath = GetResourcePath(name);
    if (!EnsureDirectoryExists(QFileInfo(filePath).path())) {
        return false;
    }
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    file.write(data);
    file.close();
    return true;
}

bool QtResourceManager::ResourceExists(const QString& name) const {
    QString filePath = FindResourceFile(name);
    return !filePath.isEmpty();
}

QString QtResourceManager::GetResourcePath(const QString& name) const {
    QString resolvedName = ResolveAlias(name);
    QString filePath = FindResourceFile(resolvedName);
    return filePath.isEmpty() ? m_resourcePath + "/" + resolvedName : filePath;
}

void QtResourceManager::CachePixmap(const QString& name, const QPixmap& pixmap) {
    std::lock_guard<std::mutex> lock(m_mutex);
    QString resolvedName = ResolveAlias(name);
    m_pixmapCache.insert(resolvedName, new QPixmap(pixmap));
}

QPixmap QtResourceManager::GetCachedPixmap(const QString& name) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    QString resolvedName = ResolveAlias(name);
    QPixmap* cached = m_pixmapCache.object(resolvedName);
    return cached ? *cached : QPixmap();
}

void QtResourceManager::ClearPixmapCache() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_pixmapCache.clear();
    emit CacheCleared();
}

void QtResourceManager::SetPixmapCacheSize(int maxSize) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_pixmapCacheSize = maxSize;
    m_pixmapCache.setMaxCost(maxSize);
}

void QtResourceManager::CacheImage(const QString& name, const QImage& image) {
    std::lock_guard<std::mutex> lock(m_mutex);
    QString resolvedName = ResolveAlias(name);
    m_imageCache.insert(resolvedName, new QImage(image));
}

QImage QtResourceManager::GetCachedImage(const QString& name) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    QString resolvedName = ResolveAlias(name);
    QImage* cached = m_imageCache.object(resolvedName);
    return cached ? *cached : QImage();
}

void QtResourceManager::ClearImageCache() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_imageCache.clear();
    emit CacheCleared();
}

void QtResourceManager::SetImageCacheSize(int maxSize) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_imageCacheSize = maxSize;
    m_imageCache.setMaxCost(maxSize);
}

void QtResourceManager::ClearAllCaches() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_pixmapCache.clear();
    m_imageCache.clear();
    emit CacheCleared();
}

void QtResourceManager::RegisterResourceAlias(const QString& alias, const QString& actualPath) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_aliases[alias] = actualPath;
}

void QtResourceManager::UnregisterResourceAlias(const QString& alias) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_aliases.remove(alias);
}

QString QtResourceManager::ResolveAlias(const QString& name) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_aliases.value(name, name);
}

bool QtResourceManager::LoadResourceFile(const QString& rccPath) {
    return QResource::registerResource(rccPath);
}

void QtResourceManager::UnloadResourceFile(const QString& rccPath) {
    QResource::unregisterResource(rccPath);
}

QString QtResourceManager::FindResourceFile(const QString& name) const {
    if (QFileInfo::exists(name)) {
        return name;
    }
    
    for (const QString& path : m_searchPaths) {
        QString fullPath = path + "/" + name;
        if (QFileInfo::exists(fullPath)) {
            return fullPath;
        }
    }
    
    if (QFile::exists(":/")) {
        QString resourcePath = ":" + (name.startsWith("/") ? name : "/" + name);
        if (QFileInfo::exists(resourcePath)) {
            return resourcePath;
        }
    }
    
    return QString();
}

bool QtResourceManager::EnsureDirectoryExists(const QString& path) {
    QDir dir(path);
    if (!dir.exists()) {
        return dir.mkpath(".");
    }
    return true;
}

}  
}  

#endif
