#pragma once

#include <string>
#include <vector>
#include <queue>
#include <mutex>
#include <memory>
#include "RenderEngine.h"
// 前向声明 SQLite 类型
struct sqlite3;

namespace cycle {

// 边界框结构（已在 RenderEngine.h 中定义，这里重新声明以保持独立）
struct BoundingBox2 {
    double m_dMinX = 0.0;
    double m_dMinY = 0.0;
    double m_dMaxX = 0.0;
    double m_dMaxY = 0.0;
    
    BoundingBox2() = default;
    BoundingBox2(double dMinX, double dMinY, double dMaxX, double dMaxY)
        : m_dMinX(dMinX), m_dMinY(dMinY), m_dMaxX(dMaxX), m_dMaxY(dMaxY) {}
    
    bool IsValid() const {
        return m_dMinX < m_dMaxX && m_dMinY < m_dMaxY;
    }
    
    double Width() const { return m_dMaxX - m_dMinX; }
    double Height() const { return m_dMaxY - m_dMinY; }
};

// 图层信息结构
struct LayerInfo {
    std::string id;
    std::string name;
    std::string type; // "vector", "raster", "tile"
    std::string description;
    BoundingBox bounds;
    std::vector<std::string> attributes;
    
    LayerInfo() = default;
    LayerInfo(const std::string& id_, const std::string& name_, 
              const std::string& type_, const std::string& desc_,
              const BoundingBox& bounds_ = BoundingBox(),
              const std::vector<std::string>& attrs_ = {})
        : id(id_), name(name_), type(type_), description(desc_), bounds(bounds_), attributes(attrs_) {}
};

// 数据库管理器类
class DatabaseManager {
public:
    DatabaseManager(const std::string& dbPath, int poolSize = 5);
    ~DatabaseManager();
    
    // 禁止拷贝
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
    
    // 允许移动
    DatabaseManager(DatabaseManager&& other) noexcept;
    DatabaseManager& operator=(DatabaseManager&& other) noexcept;
    
    // 获取所有图层
    std::vector<LayerInfo> GetAllLayers();
    
    // 根据ID获取图层
    LayerInfo GetLayerById(const std::string& layerId);
    
    // 检查数据库连接是否健康
    bool IsHealthy();
    
    // 获取数据库路径
    const std::string& GetDatabasePath() const { return m_dbPath; }
    
private:
    // 获取数据库连接（从连接池）
    sqlite3* GetConnection();
    
    // 释放连接（返回到连接池）
    void ReleaseConnection(sqlite3* conn);
    
    // 初始化连接池
    void InitializePool();
    
    // 清理连接池
    void CleanupPool();
    
    // 执行查询并处理结果
    bool ExecuteQuery(sqlite3* conn, const std::string& query, 
                     std::vector<LayerInfo>& results);
    
private:
    std::string m_dbPath;
    int m_poolSize;
    std::queue<sqlite3*> m_connectionPool;
    std::mutex m_poolMutex;
    bool m_initialized = false;
};

} // namespace cycle