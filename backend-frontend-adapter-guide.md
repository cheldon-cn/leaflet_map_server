# Leaf 项目前后端适配指南

## 概述

本文档详细分析了 `leaf` 项目中 `server` (C++ 后端) 与 `web` (JavaScript 前端) 之间的技术栈差异和接口不匹配问题，并提供了一套可操作的适配步骤与修改建议，旨在使前后端能够完全协同工作，形成一个功能完整的地图渲染服务系统。

## 1. 适配现状分析

### 1.1 技术栈对比

| 维度 | `server` (后端) | `web` (前端) |
|------|----------------|--------------|
| **语言** | C++11 | JavaScript (ES6+) |
| **HTTP库** | cpp-httplib v0.14.1 | Axios |
| **构建工具** | CMake 3.10+ | Vite 5.0.0 |
| **地图引擎** | 自定义 RenderEngine | Leaflet 1.9.4 |
| **数据库** | SQLite3 (推测) | 无直接访问 |
| **缓存策略** | 简单文件缓存 | 内存 + LocalStorage LRU 缓存 |

### 1.2 核心不匹配点

1. **API端点缺失**：前端定义的 `GET_LAYER_INFO`、`GET_CAPABILITIES`、`POST_METRICS` 在后端未实现。
2. **响应格式冲突**：前端 `MapApiClient` 默认尝试解析所有响应为 JSON，但后端的 `/generate`、`/tile` 返回二进制 PNG。
3. **响应头期望**：前端 `extractMetadata()` 期望特定响应头 (`x-render-time`, `x-cache-hit` 等)，后端未设置。
4. **参数验证差距**：前端有完整参数验证 (`validateGenerateParams`)，后端只有简易 JSON 解析。
5. **数据库交互**：前端期望通过 `/layers/{layerId}` 获取图层详情，后端需连接数据库提供真实数据。
6. **错误处理不一致**：后端错误响应格式可能与前端 `errorHandler.js` 的分类逻辑不匹配。

## 2. 详细适配步骤 (按优先级排序)

### 2.1 步骤一：统一 API 端点清单

**目标**：确保后端实现前端期望的所有端点。

| 端点 | 方法 | 前端期望 (endpoints.js) | 后端现状 (HttpServer.cpp) | 适配建议 |
|------|------|------------------------|--------------------------|----------|
| `/health` | GET | 返回健康状态 JSON | ✅ 已实现 (模拟数据) | 保持现状 |
| `/layers` | GET | 返回图层列表 JSON | ✅ 已实现 (模拟数据) | 替换为真实数据库查询 |
| `/generate` | POST | 接收 JSON 参数，返回 PNG | ✅ 已实现 (返回 PNG) | 需添加响应头 |
| `/tile/{z}/{x}/{y}.png` | GET | 返回 PNG 瓦片 | ✅ 已实现 (测试图案) | 需添加响应头，改进为真实瓦片 |
| `/layers/{layerId}` | GET | 返回图层详细信息 JSON | ❌ 未实现 | **新增端点** |
| `/capabilities` | GET | 返回服务能力 JSON (WMS-like) | ❌ 未实现 | **新增端点** |
| `/metrics` | POST | 接收客户端指标数据 | ❌ 未实现 | **新增端点** (可先记录日志) |

**操作建议**：

1. 在后端 `HttpServer::SetupRoutes()` 中添加三个缺失的路由：
   - `GET /layers/{layerId}`
   - `GET /capabilities`
   - `POST /metrics`

2. 创建对应的处理函数：
   - `void HandleLayerInfoRequest(const httplib::Request& req, httplib::Response& res)`
   - `void HandleCapabilitiesRequest(const httplib::Request& req, httplib::Response& res)`
   - `void HandleMetricsRequest(const httplib::Request& req, httplib::Response& res)`

3. 参考现有 `HandleLayersRequest` 的实现模式，返回符合前端 `responseSchema` 的 JSON。

### 2.2 步骤二：协调响应格式与头部

**目标**：确保前端能正确解析后端的二进制和 JSON 响应。

**问题分析**：
- `MapApiClient` 构造函数设置 `responseType: 'arraybuffer'`
- `listLayers()`、`getLayerInfo()` 等方法尝试用 `JSON.parse(new TextDecoder().decode(response.data))` 解析
- 后端 `/generate` 和 `/tile` 返回 PNG 二进制数据，但需要添加元数据响应头

**后端修改方案**：

1. 在 `HandleGenerateRequest` 函数中，渲染完成后添加响应头：
   ```cpp
   auto endTime = std::chrono::high_resolution_clock::now();
   auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
   
   // 设置响应头
   res.set_header("x-render-time", std::to_string(duration.count()));
   res.set_header("x-cache-hit", "false"); // 暂时硬编码，后续实现缓存后更新
   res.set_header("x-features-count", std::to_string(featureCount)); // 需要从渲染引擎获取
   res.set_header("x-timestamp", GetCurrentISOTime());
   res.set_header("x-bbox", BboxToJsonString(request.m_bbox));
   ```

2. 在 `HandleTileRequest` 函数中添加类似响应头。

3. 确保 `/layers`、`/layers/{layerId}`、`/capabilities`、`/health` 返回正确的 `Content-Type: application/json`。

**前端验证**：
检查 `MapApiClient.js` 中以下方法是否能正确处理响应：
- 第121行: `listLayers()` 中的 `JSON.parse(new TextDecoder().decode(response.data))`
- 第140行: `getLayerInfo()` 中的相同逻辑
- 第156行: `getCapabilities()`
- 第180行: `getHealth()`

### 2.3 步骤三：增强请求参数验证

**目标**：在后端实现与前端的参数验证逻辑。

**前端验证参考** (`MapApiClient.validateGenerateParams`)：
- `bbox`: 数组长度4，坐标范围有效，西<东、南<北
- `width`/`height`: 1-4096像素
- `layers`: 数组格式
- `styles`: 对象格式
- `backgroundColor`: 十六进制颜色码
- `format`: `png`/`webp`/`jpeg`
- `quality`: 0-100整数

**后端改进方案**：

1. **引入 JSON 库**：检查 `thirdparty/` 是否已有 `nlohmann/json.hpp`，或将其添加为 CMake 依赖。

2. **重构 `ParseMapRequest`**：
   ```cpp
   #include "nlohmann/json.hpp"
   
   bool HttpServer::ParseMapRequest(const std::string& strJson, MapRequest& request) {
       try {
           auto json = nlohmann::json::parse(strJson);
           
           // 解析 bbox (支持数组格式或单独字段)
           if (json.contains("bbox") && json["bbox"].is_array() && json["bbox"].size() == 4) {
               request.m_bbox.m_dMinX = json["bbox"][0];
               request.m_bbox.m_dMinY = json["bbox"][1];
               request.m_bbox.m_dMaxX = json["bbox"][2];
               request.m_bbox.m_dMaxY = json["bbox"][3];
           } else if (json.contains("minx") && json.contains("miny") && 
                      json.contains("maxx") && json.contains("maxy")) {
               request.m_bbox.m_dMinX = json["minx"];
               request.m_bbox.m_dMinY = json["miny"];
               request.m_bbox.m_dMaxX = json["maxx"];
               request.m_bbox.m_dMaxY = json["maxy"];
           } else {
               return false; // 无效的 bbox
           }
           
           // 验证 bbox 有效性
           if (!request.m_bbox.IsValid() || 
               request.m_bbox.m_dMinX >= request.m_bbox.m_dMaxX ||
               request.m_bbox.m_dMinY >= request.m_bbox.m_dMaxY) {
               return false;
           }
           
           // 解析并验证其他参数...
           request.m_nWidth = json.value("width", 1024);
           request.m_nHeight = json.value("height", 768);
           
           // 尺寸限制检查
           if (request.m_nWidth <= 0 || request.m_nHeight <= 0 ||
               request.m_nWidth > m_config.m_nMaxImageWidth ||
               request.m_nHeight > m_config.m_nMaxImageHeight) {
               return false;
           }
           
           // 解析 layers 数组
           if (json.contains("layers") && json["layers"].is_array()) {
               for (const auto& layer : json["layers"]) {
                   request.m_vecLayers.push_back(layer.get<std::string>());
               }
           }
           
           return true;
       } catch (const nlohmann::json::exception& e) {
           SetError(std::string("JSON parsing error: ") + e.what());
           return false;
       }
   }
   ```

3. **添加验证函数**：可独立创建 `ValidateMapRequestParams` 函数，集中所有验证逻辑。

4. **统一错误响应**：验证失败时返回结构化错误：
   ```cpp
   strResponse = GenerateErrorResponse("VALIDATION_ERROR", "Invalid bounding box coordinates");
   strContentType = "application/json";
   ```

### 2.4 步骤四：数据库连接与图层查询

**目标**：为 `/layers` 和 `/layers/{layerId}` 提供真实数据。

**现状分析**：
- 配置文件: `config.example.json` → `"database": {"path": "./spatial_data.db"}`
- 代码引用: `sqlite_recovery_lib`, `feature_database_writer_lib`
- 当前实现: `HandleLayersRequest` 返回硬编码模拟数据

**操作步骤**：

1. **探查数据库结构**：
   ```bash
   sqlite3 ./spatial_data.db ".schema"
   sqlite3 ./spatial_data.db "SELECT name FROM sqlite_master WHERE type='table';"
   ```

2. **创建数据访问层** (`server/src/DatabaseManager.cpp/h`)：
   ```cpp
   class DatabaseManager {
   public:
       DatabaseManager(const std::string& dbPath, int poolSize = 5);
       ~DatabaseManager();
       
       std::vector<LayerInfo> GetAllLayers();
       LayerInfo GetLayerById(const std::string& layerId);
       bool IsHealthy();
       
   private:
       sqlite3* GetConnection();
       void ReleaseConnection(sqlite3* conn);
       
       std::string m_dbPath;
       std::queue<sqlite3*> m_connectionPool;
       std::mutex m_poolMutex;
   };
   
   struct LayerInfo {
       std::string id;
       std::string name;
       std::string type; // "vector", "raster", "tile"
       std::string description;
       BoundingBox bounds;
       std::vector<std::string> attributes;
   };
   ```

3. **集成到 HttpServer**：
   - 在 `HttpServer` 构造函数中初始化 `DatabaseManager`
   - 修改 `HandleLayersRequest` 调用 `m_pDbManager->GetAllLayers()`
   - 实现 `HandleLayerInfoRequest` 调用 `m_pDbManager->GetLayerById(layerId)`

4. **错误处理**：数据库查询失败时返回适当的 HTTP 状态码和错误信息。

### 2.5 步骤五：完善服务能力端点 (`/capabilities`)

**目标**：实现类似 WMS 的 GetCapabilities 响应。

**前端期望结构**：
```json
{
  "service": {
    "name": "string",
    "version": "string",
    "title": "string",
    "abstract": "string"
  },
  "capabilities": {
    "maxWidth": "integer",
    "maxHeight": "integer",
    "formats": ["png", "webp", "jpeg"],
    "projections": ["EPSG:4326", "EPSG:3857"]
  }
}
```

**实现建议**：

1. **静态信息**：
   ```cpp
   nlohmann::json serviceInfo = {
       {"name", "map-server"},
       {"version", "1.0.0"},
       {"title", "Leaf Map Rendering Service"},
       {"abstract", "High-performance map rendering service"}
   };
   ```

2. **动态信息**：
   ```cpp
   nlohmann::json capabilities = {
       {"maxWidth", m_config.m_nMaxImageWidth},
       {"maxHeight", m_config.m_nMaxImageHeight},
       {"formats", {"png", "webp", "jpeg"}},
       {"projections", {"EPSG:4326", "EPSG:3857"}}
   };
   
   // 可选：包含图层列表
   auto layers = m_pDbManager->GetAllLayers();
   capabilities["layers"] = ConvertLayersToJson(layers);
   ```

3. **响应构造**：
   ```cpp
   nlohmann::json response = {
       {"service", serviceInfo},
       {"capabilities", capabilities}
   };
   
   strResponse = response.dump(2); // 带缩进的 JSON
   strContentType = "application/json";
   ```

### 2.6 步骤六：指标收集端点 (`/metrics`)

**目标**：接收前端提交的性能指标。

**前端提交格式**：
```json
{
  "metrics": [
    {
      "name": "render_time",
      "value": 123.45,
      "timestamp": "2024-01-01T12:00:00Z",
      "tags": {"sessionId": "abc123"}
    }
  ]
}
```

**实现方案**：

1. **基础实现** (日志记录)：
   ```cpp
   void HttpServer::HandleMetricsRequest(const httplib::Request& req, httplib::Response& res) {
       try {
           auto json = nlohmann::json::parse(req.body);
           
           if (json.contains("metrics") && json["metrics"].is_array()) {
               std::cout << "Received " << json["metrics"].size() << " metrics" << std::endl;
               
               // 简单记录到控制台
               for (const auto& metric : json["metrics"]) {
                   std::cout << "Metric: " << metric["name"] << " = " << metric["value"] << std::endl;
               }
           }
           
           res.status = 200;
           res.set_content("{\"status\":\"ok\"}", "application/json");
       } catch (...) {
           res.status = 400;
           res.set_content("{\"error\":\"Invalid JSON\"}", "application/json");
       }
   }
   ```

2. **扩展方向**：
   - 写入文件日志
   - 集成 Prometheus 客户端库
   - 存储到数据库的 metrics 表

### 2.7 步骤七：CORS 与代理配置协调

**目标**：确保开发环境跨域请求正常。

**现状检查**：
- 后端：`set_pre_routing_handler` 设置 `Access-Control-Allow-Origin: *`，处理 `OPTIONS`
- 前端：`vite.config.js` 代理 `/api` → `http://localhost:8080`
- 设置：`API_BASE_URL` 默认为 `http://localhost:8080`

**验证步骤**：

1. 确认后端服务器运行在 `0.0.0.0:8080` (配置默认值)
2. 测试代理是否工作：访问 `http://localhost:3000/api/health` 应返回后端响应
3. 检查 `MapApiClient` 的 `baseURL` 是否正确使用 `API_BASE_URL`

**建议配置**：

1. **开发环境**：保持当前代理配置，前端通过 `/api` 访问后端
2. **生产环境**：
   - 前后端部署在同一域名下
   - 或设置正确的 CORS 策略
   - `API_BASE_URL` 配置为后端服务的公开 URL

## 3. 风险与注意事项

1. **C++ JSON 处理风险**：
   - 手动字符串解析 (`GetJsonString`) 易错，不支持嵌套结构
   - **强烈建议**：引入 `nlohmann/json` 库作为依赖

2. **二进制响应安全**：
   - 确保 `res.set_content()` 传递正确的数据指针和大小
   - PNG 编码失败时返回适当错误，而非空响应

3. **数据库线程安全**：
   - 配置中 `connection_pool_size: 5`，需实现线程安全连接池
   - SQLite 连接在并发写入时需要额外同步

4. **错误传播一致性**：
   - 前端 `errorHandler.js` 依赖 `RequestError`、`ValidationError`、`TimeoutError` 分类
   - 后端错误响应需保持格式一致：`{"error": {"code": "...", "message": "..."}}`

5. **版本兼容性**：
   - 前端 `endpoints.js` 的 `responseSchema` 定义了严格结构
   - 后端修改时需确保向前兼容，新增字段应为可选

6. **性能监控**：
   - 添加响应头 `x-render-time` 会增加少量开销
   - 考虑在高频请求中抽样记录而非全量

## 4. 测试验证方案

完成每步适配后，按以下顺序验证：

### 4.1 基础连通性测试
1. **健康检查**：`GET /health` → 返回 `{"status": "healthy", ...}`
2. **CORS 验证**：从不同源的前端页面调用 API，确认无跨域错误

### 4.2 核心功能测试
1. **图层列表**：`GET /layers` → 返回数据库中的真实图层列表
2. **地图生成**：`POST /generate` 带合法参数 → 返回 PNG，响应头包含元数据
   ```json
   {
     "bbox": [-180, -90, 180, 90],
     "width": 1024,
     "height": 768,
     "layers": ["roads", "buildings"]
   }
   ```
3. **瓦片服务**：`GET /tile/10/500/300.png` → 返回有效瓦片图像

### 4.3 新增端点测试
1. **图层详情**：`GET /layers/roads` → 返回特定图层详细信息
2. **能力文档**：`GET /capabilities` → 返回完整服务描述
3. **指标提交**：`POST /metrics` 带测试数据 → 返回 200 OK，数据被记录

### 4.4 错误处理测试
1. **无效参数**：`POST /generate` 带无效 bbox → 返回 `VALIDATION_ERROR`
2. **不存在的图层**：`GET /layers/nonexistent` → 返回 `NOT_FOUND`
3. **服务端错误**：模拟数据库故障 → 返回 `INTERNAL_ERROR`

### 4.5 性能测试
1. **并发请求**：同时发起多个 `/generate` 请求，验证连接池工作
2. **大尺寸图像**：请求最大尺寸 (4096x4096)，验证内存和性能
3. **缓存命中**：重复相同请求，验证 `x-cache-hit` 头部变化

## 5. 后续优化方向

### 5.1 短期优化 (1-2周)
1. **API 文档**：使用 OpenAPI/Swagger 规范前后端接口
2. **缓存集成**：实现响应头 `x-cache-hit` 对应的磁盘/内存缓存
3. **监控仪表盘**：基础性能指标可视化

### 5.2 中期优化 (1-2月)
1. **认证授权**：添加 API 密钥或 JWT 用户认证
2. **性能监控**：集成 APM 工具 (如 Prometheus + Grafana)
3. **批量处理**：支持异步地图生成和结果查询

### 5.3 长期优化 (3-6月)
1. **容器化部署**：提供 Docker 配置和 Kubernetes 部署文件
2. **高可用架构**：多实例部署、负载均衡、数据库复制
3. **高级功能**：
   - 矢量瓦片支持 (MVT)
   - 动态样式服务 (SLD)
   - 空间分析功能

## 6. 实施时间估算

| 步骤 | 预计工时 | 优先级 | 依赖 |
|------|----------|--------|------|
| 1. 统一 API 端点清单 | 4-6小时 | P0 | 无 |
| 2. 协调响应格式与头部 | 3-4小时 | P0 | 步骤1 |
| 3. 增强请求参数验证 | 6-8小时 | P1 | 步骤2 |
| 4. 数据库连接与图层查询 | 8-12小时 | P1 | 步骤3 |
| 5. 完善服务能力端点 | 3-4小时 | P2 | 步骤4 |
| 6. 指标收集端点 | 2-3小时 | P3 | 无 |
| 7. CORS 与代理配置协调 | 1-2小时 | P0 | 无 |

**总计**: 27-39 小时

## 7. 责任分工建议

| 角色 | 负责步骤 | 所需技能 |
|------|----------|----------|
| 后端开发 (C++) | 步骤1-5, 7 | C++11, cpp-httplib, SQLite3, CMake |
| 前端开发 (JavaScript) | 步骤2验证, 6集成 | JavaScript, Axios, Vite, 调试工具 |
| DevOps/测试 | 所有步骤验证 | API 测试, 性能测试, 部署 |

---

*文档版本: 1.0*  
*最后更新: 2025-02-04*  
*适用范围: leaf 项目前后端适配*