#include "DatabaseManager.h"
#include <sqlite3.h>
#include <iostream>
#include <sstream>
#include <algorithm>

namespace cycle {

DatabaseManager::DatabaseManager(const std::string& dbPath, int poolSize) 
    : m_dbPath(dbPath), m_poolSize(poolSize > 0 ? poolSize : 5) {
    InitializePool();
}

DatabaseManager::~DatabaseManager() {
    CleanupPool();
}

DatabaseManager::DatabaseManager(DatabaseManager&& other) noexcept
    : m_dbPath(std::move(other.m_dbPath)),
      m_poolSize(other.m_poolSize),
      m_connectionPool(std::move(other.m_connectionPool)),
      m_initialized(other.m_initialized) {
    other.m_initialized = false;
    other.m_poolSize = 0;
}

DatabaseManager& DatabaseManager::operator=(DatabaseManager&& other) noexcept {
    if (this != &other) {
        CleanupPool();
        m_dbPath = std::move(other.m_dbPath);
        m_poolSize = other.m_poolSize;
        m_connectionPool = std::move(other.m_connectionPool);
        m_initialized = other.m_initialized;
        
        other.m_initialized = false;
        other.m_poolSize = 0;
    }
    return *this;
}

void DatabaseManager::InitializePool() {
    std::lock_guard<std::mutex> lock(m_poolMutex);
    
    for (int i = 0; i < m_poolSize; ++i) {
        sqlite3* conn = nullptr;
        int rc = sqlite3_open_v2(m_dbPath.c_str(), &conn, 
            SQLITE_OPEN_READWRITE | SQLITE_OPEN_FULLMUTEX,
                                 nullptr);
        
        if (rc != SQLITE_OK) {
            std::cerr << "Failed to open database connection " << i 
                      << ": " << sqlite3_errmsg(conn) << std::endl;
            if (conn) sqlite3_close(conn);
            continue;
        }
        
        // 设置一些优化参数
        sqlite3_exec(conn, "PRAGMA journal_mode = MEMORY;", nullptr, nullptr, nullptr);
        sqlite3_exec(conn, "PRAGMA synchronous = OFF;", nullptr, nullptr, nullptr);
        
        m_connectionPool.push(conn);
    }
    
    m_initialized = !m_connectionPool.empty();
    if (m_initialized) {
        std::cout << "Database connection pool initialized with " 
                  << m_connectionPool.size() << " connections" << std::endl;
    } else {
        std::cerr << "Warning: Database connection pool initialization failed" << std::endl;
    }
}

void DatabaseManager::CleanupPool() {
    std::lock_guard<std::mutex> lock(m_poolMutex);
    
    while (!m_connectionPool.empty()) {
        sqlite3* conn = m_connectionPool.front();
        m_connectionPool.pop();
        
        if (conn) {
            sqlite3_close(conn);
        }
    }
    
    m_initialized = false;
}

sqlite3* DatabaseManager::GetConnection() {
    std::lock_guard<std::mutex> lock(m_poolMutex);
    
    if (m_connectionPool.empty()) {
        // 尝试创建新的连接（紧急情况）
        sqlite3* conn = nullptr;
        int rc = sqlite3_open_v2(m_dbPath.c_str(), &conn, 
                                 SQLITE_OPEN_READONLY | SQLITE_OPEN_FULLMUTEX, 
                                 nullptr);
        
        if (rc != SQLITE_OK) {
            std::cerr << "Emergency connection creation failed: " 
                      << sqlite3_errmsg(conn) << std::endl;
            if (conn) sqlite3_close(conn);
            return nullptr;
        }
        
        return conn;
    }
    
    sqlite3* conn = m_connectionPool.front();
    m_connectionPool.pop();
    return conn;
}

void DatabaseManager::ReleaseConnection(sqlite3* conn) {
    if (!conn) return;
    
    std::lock_guard<std::mutex> lock(m_poolMutex);
    
    // 重置连接状态
    sqlite3_reset(nullptr); // 清除任何待处理的语句
    
    m_connectionPool.push(conn);
}

std::vector<LayerInfo> DatabaseManager::GetAllLayers() {
    std::vector<LayerInfo> layers;
    
    // 如果数据库不可用，返回空列表
    if (!m_initialized) {
        std::cerr << "Database not initialized, returning empty layer list" << std::endl;
        return layers;
    }
    
    sqlite3* conn = GetConnection();
    if (!conn) {
        std::cerr << "Failed to get database connection" << std::endl;
        return layers;
    }
    
    // 尝试查询图层表
    const std::string query = "SELECT id, name, type, description, "
                              "minx, miny, maxx, maxy "
                              "FROM layers ORDER BY name;";
    
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(conn, query.c_str(), -1, &stmt, nullptr);
    
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare query: " << sqlite3_errmsg(conn) << std::endl;
        
        // 如果图层表不存在，返回模拟数据
        std::cout << "Layers table not found, returning mock data" << std::endl;
        layers.push_back(LayerInfo("roads", "Roads", "vector", 
                                   "Road network including highways, streets, and paths"));
        layers.push_back(LayerInfo("buildings", "Buildings", "vector", 
                                   "Building footprints with architectural details"));
        layers.push_back(LayerInfo("points", "Points of Interest", "vector", 
                                   "Points of interest including landmarks, businesses, and facilities"));
        
        ReleaseConnection(conn);
        return layers;
    }
    
    // 处理查询结果
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        LayerInfo layer;
        
        // 读取基本字段
        const char* id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        const char* type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        const char* desc = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        
        layer.id = id ? id : "";
        layer.name = name ? name : "";
        layer.type = type ? type : "vector";
        layer.description = desc ? desc : "";
        
        // 读取边界框（如果有）
        if (sqlite3_column_type(stmt, 4) != SQLITE_NULL &&
            sqlite3_column_type(stmt, 5) != SQLITE_NULL &&
            sqlite3_column_type(stmt, 6) != SQLITE_NULL &&
            sqlite3_column_type(stmt, 7) != SQLITE_NULL) {
            
            double minx = sqlite3_column_double(stmt, 4);
            double miny = sqlite3_column_double(stmt, 5);
            double maxx = sqlite3_column_double(stmt, 6);
            double maxy = sqlite3_column_double(stmt, 7);
            
            layer.bounds = BoundingBox(minx, miny, maxx, maxy);
        }
        
        layers.push_back(layer);
    }
    
    sqlite3_finalize(stmt);
    ReleaseConnection(conn);
    
    // 如果查询结果为空，返回模拟数据
    if (layers.empty()) {
        std::cout << "No layers found in database, returning mock data" << std::endl;
        layers.push_back(LayerInfo("roads", "Roads", "vector", 
                                   "Road network including highways, streets, and paths"));
        layers.push_back(LayerInfo("buildings", "Buildings", "vector", 
                                   "Building footprints with architectural details"));
        layers.push_back(LayerInfo("points", "Points of Interest", "vector", 
                                   "Points of interest including landmarks, businesses, and facilities"));
    }
    
    return layers;
}

LayerInfo DatabaseManager::GetLayerById(const std::string& layerId) {
    // 如果数据库不可用，返回空图层信息
    if (!m_initialized) {
        std::cerr << "Database not initialized, returning empty layer info" << std::endl;
        return LayerInfo();
    }
    
    sqlite3* conn = GetConnection();
    if (!conn) {
        std::cerr << "Failed to get database connection" << std::endl;
        return LayerInfo();
    }
    
    // 尝试查询特定图层
    const std::string query = "SELECT id, name, type, description, "
                              "minx, miny, maxx, maxy "
                              "FROM layers WHERE id = ?;";
    
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(conn, query.c_str(), -1, &stmt, nullptr);
    
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare query: " << sqlite3_errmsg(conn) << std::endl;
        ReleaseConnection(conn);
        return LayerInfo();
    }
    
    // 绑定参数
    sqlite3_bind_text(stmt, 1, layerId.c_str(), -1, SQLITE_STATIC);
    
    LayerInfo layer;
    
    // 处理查询结果
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        // 读取基本字段
        const char* id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        const char* type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        const char* desc = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        
        layer.id = id ? id : "";
        layer.name = name ? name : "";
        layer.type = type ? type : "vector";
        layer.description = desc ? desc : "";
        
        // 读取边界框（如果有）
        if (sqlite3_column_type(stmt, 4) != SQLITE_NULL &&
            sqlite3_column_type(stmt, 5) != SQLITE_NULL &&
            sqlite3_column_type(stmt, 6) != SQLITE_NULL &&
            sqlite3_column_type(stmt, 7) != SQLITE_NULL) {
            
            double minx = sqlite3_column_double(stmt, 4);
            double miny = sqlite3_column_double(stmt, 5);
            double maxx = sqlite3_column_double(stmt, 6);
            double maxy = sqlite3_column_double(stmt, 7);
            
            layer.bounds = BoundingBox(minx, miny, maxx, maxy);
        }
    } else {
        // 如果没有找到图层，返回模拟数据
        if (layerId == "roads") {
            layer = LayerInfo("roads", "Roads", "vector", 
                             "Road network including highways, streets, and paths",
                             BoundingBox(-180, -90, 180, 90),
                             {"name", "type", "length", "width"});
        } else if (layerId == "buildings") {
            layer = LayerInfo("buildings", "Buildings", "vector", 
                             "Building footprints with architectural details",
                             BoundingBox(-180, -90, 180, 90),
                             {"name", "height", "area", "type"});
        } else if (layerId == "points") {
            layer = LayerInfo("points", "Points of Interest", "vector", 
                             "Points of interest including landmarks, businesses, and facilities",
                             BoundingBox(-180, -90, 180, 90),
                             {"name", "category", "rating", "address"});
        }
        // 否则返回空LayerInfo
    }
    
    sqlite3_finalize(stmt);
    ReleaseConnection(conn);
    
    return layer;
}

bool DatabaseManager::IsHealthy() {
    if (!m_initialized) return false;
    
    sqlite3* conn = GetConnection();
    if (!conn) return false;
    
    // 执行简单的查询来测试连接
    bool healthy = false;
    sqlite3_stmt* stmt = nullptr;
    const char* testQuery = "SELECT 1;";
    
    if (sqlite3_prepare_v2(conn, testQuery, -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            healthy = true;
        }
        sqlite3_finalize(stmt);
    }
    
    ReleaseConnection(conn);
    return healthy;
}

} // namespace cycle