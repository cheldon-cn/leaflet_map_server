#ifndef OGC_DRAW_QT_RESOURCE_MANAGER_H
#define OGC_DRAW_QT_RESOURCE_MANAGER_H

#include "ogc/draw/export.h"

#ifdef DRAW_HAS_QT

#include <QObject>
#include <QString>
#include <QPixmap>
#include <QImage>
#include <QIcon>
#include <QFont>
#include <QColor>
#include <QFile>
#include <QDir>
#include <QMap>
#include <QCache>
#include <memory>
#include <mutex>

namespace ogc {
namespace draw {

class OGC_DRAW_API QtResourceManager : public QObject {
    Q_OBJECT

public:
    static QtResourceManager& Instance();

    bool Initialize(const QString& resourcePath = QString());
    void Finalize();
    bool IsInitialized() const { return m_initialized; }

    void SetResourcePath(const QString& path);
    QString GetResourcePath() const { return m_resourcePath; }

    void AddSearchPath(const QString& path);
    void RemoveSearchPath(const QString& path);
    QStringList GetSearchPaths() const;

    QPixmap LoadPixmap(const QString& name);
    QImage LoadImage(const QString& name);
    QIcon LoadIcon(const QString& name);
    QFont LoadFont(const QString& name, int pointSize = -1);
    QColor LoadColor(const QString& name);
    QString LoadTextFile(const QString& name);
    QByteArray LoadBinaryFile(const QString& name);

    bool SavePixmap(const QString& name, const QPixmap& pixmap, const char* format = "PNG");
    bool SaveImage(const QString& name, const QImage& image, const char* format = "PNG");
    bool SaveTextFile(const QString& name, const QString& content);
    bool SaveBinaryFile(const QString& name, const QByteArray& data);

    bool ResourceExists(const QString& name) const;
    QString GetResourcePath(const QString& name) const;

    void CachePixmap(const QString& name, const QPixmap& pixmap);
    QPixmap GetCachedPixmap(const QString& name) const;
    void ClearPixmapCache();
    void SetPixmapCacheSize(int maxSize);

    void CacheImage(const QString& name, const QImage& image);
    QImage GetCachedImage(const QString& name) const;
    void ClearImageCache();
    void SetImageCacheSize(int maxSize);

    void ClearAllCaches();

    void RegisterResourceAlias(const QString& alias, const QString& actualPath);
    void UnregisterResourceAlias(const QString& alias);
    QString ResolveAlias(const QString& name) const;

    bool LoadResourceFile(const QString& rccPath);
    void UnloadResourceFile(const QString& rccPath);

signals:
    void ResourceLoaded(const QString& name);
    void ResourceLoadFailed(const QString& name, const QString& error);
    void CacheCleared();

private:
    QtResourceManager();
    ~QtResourceManager() override;
    QtResourceManager(const QtResourceManager&) = delete;
    QtResourceManager& operator=(const QtResourceManager&) = delete;

    QString FindResourceFile(const QString& name) const;
    bool EnsureDirectoryExists(const QString& path);

    bool m_initialized = false;
    QString m_resourcePath;
    QStringList m_searchPaths;
    QMap<QString, QString> m_aliases;
    
    mutable std::mutex m_mutex;
    QCache<QString, QPixmap> m_pixmapCache;
    QCache<QString, QImage> m_imageCache;
    
    int m_pixmapCacheSize = 100;
    int m_imageCacheSize = 100;
};

}  
}  

#endif

#endif
