# ogc_cache 模块 - 头文件索引

## 模块描述

ogc_cache 是 OGC 图表系统的**缓存模块**，提供瓦片缓存和离线存储能力。支持多级缓存策略（内存/磁盘）、离线地图数据管理和数据加密功能。作为数据层和渲染层之间的桥梁，有效提升地图渲染性能和离线使用能力。

## 核心特性

- **多级缓存**：支持内存缓存、磁盘缓存和多级缓存组合
- **LRU策略**：自动淘汰最近最少使用的缓存项
- **离线存储**：支持离线地图区域下载和管理
- **数据同步**：离线数据版本管理和增量更新
- **数据加密**：支持 AES-256 加密算法保护敏感数据
- **线程安全**：所有缓存操作均为线程安全

---

## 头文件清单

| File | Description | Core Classes |
|------|-------------|--------------|
| [export.h](ogc/cache/export.h) | DLL导出宏定义 | `OGC_CACHE_API` |
| [tile/tile_key.h](ogc/cache/tile/tile_key.h) | 瓦片键定义 | `TileKey` |
| [tile/tile_cache.h](ogc/cache/tile/tile_cache.h) | 瓦片缓存基类 | `TileCache`, `TileData` |
| [tile/memory_tile_cache.h](ogc/cache/tile/memory_tile_cache.h) | 内存缓存实现 | `MemoryTileCache` |
| [tile/disk_tile_cache.h](ogc/cache/tile/disk_tile_cache.h) | 磁盘缓存实现 | `DiskTileCache` |
| [tile/multi_level_tile_cache.h](ogc/cache/tile/multi_level_tile_cache.h) | 多级缓存实现 | `MultiLevelTileCache` |
| [offline/offline_storage_manager.h](ogc/cache/offline/offline_storage_manager.h) | 离线存储管理 | `OfflineStorageManager`, `OfflineRegion` |
| [offline/offline_sync_manager.h](ogc/cache/offline/offline_sync_manager.h) | 离线同步管理 | `OfflineSyncManager`, `ChartVersion` |
| [offline/data_encryption.h](ogc/cache/offline/data_encryption.h) | 数据加密 | `DataEncryption`, `EncryptionKey` |
| [cache_manager.h](ogc/cache/cache_manager.h) | 缓存管理器 | `CacheManager` |

---

## 类继承关系图

```
TileCache (抽象基类)
    ├── MemoryTileCache
    ├── DiskTileCache
    └── MultiLevelTileCache

DataEncryption (加密工具)
    └── AES-256 实现

OfflineStorageManager
    └── 离线区域管理

OfflineSyncManager
    └── 版本同步管理

CacheManager
    └── 统一缓存管理
```

---

## 依赖关系图

```
ogc_cache
    │
    ├── ogc_geometry (几何类型)
    │       └── Envelope, Coordinate
    │
    ├── ogc_draw (绘图引擎)
    │       └── DrawContext
    │
    └── ogc_layer (图层)
            └── Layer
```

---

## 文件分类

| Category | Files |
|----------|-------|
| **Tile Cache** | tile_cache.h, tile_key.h, memory_tile_cache.h, disk_tile_cache.h, multi_level_tile_cache.h |
| **Offline** | offline_storage_manager.h, offline_sync_manager.h, data_encryption.h |
| **Build** | export.h |

---

## 关键类

### TileKey (瓦片键)

**File**: [tile_key.h](ogc/cache/tile/tile_key.h)  
**Description**: 瓦片唯一标识，包含 x, y, z 三维坐标

```cpp
namespace ogc {
namespace cache {

struct TileKey {
    int x;          // 瓦片列号
    int y;          // 瓦片行号
    int z;          // 缩放级别
    
    TileKey();
    TileKey(int tileX, int tileY, int tileLevel);
    
    bool operator==(const TileKey& other) const;
    bool operator<(const TileKey& other) const;
    
    std::string ToString() const;           // "z/x/y" 格式
    static TileKey FromString(const std::string& str);
    
    uint64_t ToIndex() const;               // 转换为唯一索引
    static TileKey FromIndex(uint64_t index);
    
    TileKey GetParent() const;              // 获取父级瓦片
    TileKey GetChild(int index) const;      // 获取子级瓦片
    void GetChildren(TileKey children[4]) const;
    
    Envelope ToEnvelope() const;            // 转换为地理范围
    static TileKey FromEnvelope(const Envelope& env, int level);
};

} // namespace cache
} // namespace ogc
```

---

### TileCache (瓦片缓存基类)

**File**: [tile_cache.h](ogc/cache/tile/tile_cache.h)  
**Description**: 瓦片缓存抽象接口

```cpp
namespace ogc {
namespace cache {

struct TileData {
    TileKey key;
    std::vector<uint8_t> data;
    int64_t timestamp;
    int64_t size;
    bool valid;
    
    bool IsValid() const;
    size_t GetSize() const;
};

typedef std::shared_ptr<TileCache> TileCachePtr;

class OGC_CACHE_API TileCache {
public:
    virtual ~TileCache() = default;
    
    virtual bool HasTile(const TileKey& key) const = 0;
    virtual TileData GetTile(const TileKey& key) const = 0;
    virtual bool PutTile(const TileKey& key, const TileData& tile) = 0;
    virtual bool PutTile(const TileKey& key, const std::vector<uint8_t>& data) = 0;
    virtual bool RemoveTile(const TileKey& key) = 0;
    virtual void Clear() = 0;
    
    virtual size_t GetTileCount() const = 0;
    virtual size_t GetSize() const = 0;
    virtual size_t GetMaxSize() const = 0;
    virtual void SetMaxSize(size_t maxSize) = 0;
    virtual bool IsFull() const = 0;
    
    static TileCachePtr CreateMemoryCache(size_t maxSize = 100 * 1024 * 1024);
    static TileCachePtr CreateDiskCache(const std::string& path, size_t maxSize = 1024 * 1024 * 1024);
};

} // namespace cache
} // namespace ogc
```

---

### MemoryTileCache (内存缓存)

**File**: [memory_tile_cache.h](ogc/cache/tile/memory_tile_cache.h)  
**Description**: 基于内存的瓦片缓存，使用 LRU 淘汰策略

```cpp
class OGC_CACHE_API MemoryTileCache : public TileCache {
public:
    explicit MemoryTileCache(size_t maxSize = 100 * 1024 * 1024);
    
    // TileCache 接口实现
    bool HasTile(const TileKey& key) const override;
    TileData GetTile(const TileKey& key) const override;
    bool PutTile(const TileKey& key, const TileData& tile) override;
    bool PutTile(const TileKey& key, const std::vector<uint8_t>& data) override;
    bool RemoveTile(const TileKey& key) override;
    void Clear() override;
    
    void SetExpirationTime(int64_t seconds);
    void Cleanup();
    
    static std::shared_ptr<MemoryTileCache> Create(size_t maxSize = 100 * 1024 * 1024);
};
```

---

### MultiLevelTileCache (多级缓存)

**File**: [multi_level_tile_cache.h](ogc/cache/tile/multi_level_tile_cache.h)  
**Description**: 多级缓存组合，支持缓存层级提升

```cpp
class OGC_CACHE_API MultiLevelTileCache : public TileCache {
public:
    MultiLevelTileCache();
    explicit MultiLevelTileCache(const std::vector<TileCachePtr>& caches);
    
    void AddCache(const TileCachePtr& cache, int priority = -1);
    void RemoveCache(size_t index);
    void RemoveCache(const std::string& name);
    size_t GetCacheCount() const;
    TileCachePtr GetCache(size_t index) const;
    
    // 缓存策略
    void SetPromoteOnHit(bool promote);     // 命中时提升到更高级缓存
    void SetWriteThrough(bool writeThrough); // 写穿透
    void SetWriteBack(bool writeBack);       // 写回
    
    static std::shared_ptr<MultiLevelTileCache> Create();
    static std::shared_ptr<MultiLevelTileCache> Create(const std::vector<TileCachePtr>& caches);
};

typedef std::shared_ptr<MultiLevelTileCache> MultiLevelTileCachePtr;
```

---

### OfflineStorageManager (离线存储管理)

**File**: [offline_storage_manager.h](ogc/cache/offline/offline_storage_manager.h)  
**Description**: 离线地图区域管理

```cpp
struct OfflineRegion {
    std::string id;
    std::string name;
    double minLon, maxLon, minLat, maxLat;
    int minZoom, maxZoom;
    std::string createdAt, updatedAt;
    size_t dataSize, tileCount;
    bool isDownloading;
    double downloadProgress;
};

struct DownloadProgress {
    std::string regionId;
    int totalTiles, downloadedTiles, failedTiles;
    double progress;
    std::string status, currentTile, errorMessage;
};

class OGC_CACHE_API OfflineStorageManager {
public:
    static OfflineStorageManager& Instance();
    
    bool DownloadRegion(const OfflineRegion& region, DownloadProgressCallback callback);
    bool CancelDownload(const std::string& regionId);
    bool PauseDownload(const std::string& regionId);
    bool ResumeDownload(const std::string& regionId);
    
    bool DeleteRegion(const std::string& regionId);
    std::vector<OfflineRegion> GetRegions() const;
    OfflineRegion GetRegion(const std::string& regionId) const;
    
    StorageInfo GetStorageInfo() const;
    bool CleanupOldData();
};
```

---

### DataEncryption (数据加密)

**File**: [data_encryption.h](ogc/cache/offline/data_encryption.h)  
**Description**: 数据加密工具，支持 AES-256 算法

```cpp
struct EncryptionKey {
    std::vector<uint8_t> key;
    std::vector<uint8_t> iv;
    std::string keyId;
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point expiresAt;
    
    bool IsValid() const;
    bool IsExpired() const;
};

enum class EncryptionAlgorithm {
    kAES256_CBC = 0,
    kAES256_GCM = 1,
    kAES256_CTR = 2
};

class OGC_CACHE_API DataEncryption {
public:
    static std::unique_ptr<DataEncryption> Create();
    
    virtual bool Initialize(const std::string& keyStoragePath) = 0;
    virtual void Shutdown() = 0;
    
    virtual EncryptionKey GenerateKey() = 0;
    virtual bool SetActiveKey(const EncryptionKey& key) = 0;
    
    virtual std::vector<uint8_t> Encrypt(const std::vector<uint8_t>& data) = 0;
    virtual std::vector<uint8_t> Decrypt(const std::vector<uint8_t>& encryptedData) = 0;
};
```

---

## 类型定义

### StorageError (存储错误枚举)

```cpp
enum class StorageError {
    kNone = 0,
    kInsufficientSpace,
    kInvalidRegion,
    kDownloadFailed,
    kWriteFailed,
    kReadFailed,
    kDeleteFailed,
    kRegionExists,
    kRegionNotFound
};
```

### SyncStatus (同步状态枚举)

```cpp
enum class SyncStatus {
    kIdle = 0,
    kCheckingUpdates,
    kDownloading,
    kApplying,
    kCompleted,
    kFailed,
    kConflict
};
```

---

## 使用示例

### 内存缓存使用

```cpp
#include <ogc/cache/tile/memory_tile_cache.h>

using namespace ogc::cache;

// 创建 100MB 内存缓存
auto cache = MemoryTileCache::Create(100 * 1024 * 1024);

// 存储瓦片
TileKey key(1234, 5678, 10);
std::vector<uint8_t> tileData = LoadTileData();
cache->PutTile(key, tileData);

// 读取瓦片
if (cache->HasTile(key)) {
    TileData data = cache->GetTile(key);
    if (data.IsValid()) {
        ProcessTile(data.data);
    }
}

// 清理过期瓦片
cache->Cleanup();
```

### 多级缓存使用

```cpp
#include <ogc/cache/tile/multi_level_tile_cache.h>
#include <ogc/cache/tile/memory_tile_cache.h>
#include <ogc/cache/tile/disk_tile_cache.h>

using namespace ogc::cache;

// 创建多级缓存
auto memoryCache = MemoryTileCache::Create(50 * 1024 * 1024);
auto diskCache = DiskTileCache::Create("E:/cache/tiles", 1024 * 1024 * 1024);

auto multiCache = MultiLevelTileCache::Create();
multiCache->AddCache(memoryCache, 0);  // L1: 内存缓存
multiCache->AddCache(diskCache, 1);    // L2: 磁盘缓存

// 启用缓存提升策略
multiCache->SetPromoteOnHit(true);     // L2 命中时提升到 L1
multiCache->SetWriteThrough(true);     // 写入时同时写 L1 和 L2

// 使用缓存
TileKey key(1234, 5678, 10);
if (multiCache->HasTile(key)) {
    TileData data = multiCache->GetTile(key);
    // 数据可能来自 L1 或 L2
}
```

### 离线存储使用

```cpp
#include <ogc/cache/offline/offline_storage_manager.h>

using namespace ogc::cache;

// 定义离线区域
OfflineRegion region;
region.id = "shanghai";
region.name = "上海港";
region.minLon = 120.8;
region.maxLon = 122.2;
region.minLat = 30.6;
region.maxLat = 31.9;
region.minZoom = 5;
region.maxZoom = 15;

// 下载离线区域
auto& manager = OfflineStorageManager::Instance();
manager.DownloadRegion(region, [](const DownloadProgress& progress) {
    std::cout << "Progress: " << progress.progress * 100 << "%" << std::endl;
});

// 查看存储信息
StorageInfo info = manager.GetStorageInfo();
std::cout << "Used: " << info.usedSpace / (1024 * 1024) << " MB" << std::endl;
```

---

## 修改历史

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| v1.0 | 2026-04-06 | Team | 从 ogc_graph 拆分创建 |

---

**Generated**: 2026-04-07  
**Module Version**: v1.0  
**C++ Standard**: C++11
