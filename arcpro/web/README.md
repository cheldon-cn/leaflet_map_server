# ArcGIS Pro到QGIS迁移工具 - Web管理界面

基于RESTful API的Web管理界面，用于ArcGIS Pro工程文件(.aprx)迁移到QGIS的工具。

## 功能特性

### 前端功能
- **用户认证**：登录、注册、JWT令牌管理
- **工程管理**：上传、查看、编辑、删除ArcGIS Pro工程文件
- **迁移任务**：创建、启动、监控、取消迁移任务
- **系统监控**：实时监控系统资源使用情况和任务状态
- **系统设置**：配置数据库、存储、安全等系统参数
- **响应式设计**：支持桌面和移动设备

### 后端功能
- **RESTful API**：完整的API接口，遵循RESTful设计原则
- **用户管理**：用户注册、登录、权限管理
- **文件管理**：文件上传、下载、存储管理
- **任务队列**：异步任务处理，支持并发执行
- **数据库管理**：PostgreSQL数据库，支持数据持久化
- **监控系统**：系统健康检查、性能监控

## 技术栈

### 前端
- **框架**：React 18 + TypeScript
- **路由**：React Router 6
- **状态管理**：Zustand + React Query
- **UI组件库**：Ant Design 5
- **HTTP客户端**：Axios
- **构建工具**：Vite 5

### 后端
- **框架**：FastAPI + Python 3.11
- **数据库**：PostgreSQL 15 + SQLAlchemy
- **缓存/队列**：Redis + RQ
- **文件存储**：MinIO（兼容S3）
- **认证**：JWT + bcrypt
- **API文档**：自动生成OpenAPI文档

### 部署
- **容器化**：Docker + Docker Compose
- **反向代理**：Nginx
- **监控**：健康检查、日志收集

## 快速开始

### 环境要求
- Docker 20.10+
- Docker Compose 2.0+
- Node.js 18+（仅开发需要）
- Python 3.11+（仅开发需要）

### 使用Docker Compose部署（推荐）

1. **克隆项目并进入web目录**
   ```bash
   cd e:/pro/search/arcpro/web
   ```

2. **启动所有服务**
   ```bash
   docker-compose up -d
   ```

3. **访问应用**
   - 前端界面：http://localhost:3000
   - 后端API：http://localhost:8000
   - API文档：http://localhost:8000/docs
   - MinIO管理：http://localhost:9001 (用户名: minioadmin, 密码: minioadmin)
   - PostgreSQL：localhost:5432 (用户: postgres, 密码: password)

4. **停止服务**
   ```bash
   docker-compose down
   ```

### 开发环境设置

#### 前端开发
```bash
cd frontend
npm install
npm run dev
```

#### 后端开发
```bash
cd backend
python -m venv venv
source venv/bin/activate  # Linux/Mac
# 或
venv\Scripts\activate  # Windows

pip install -r requirements.txt
uvicorn app.main:app --reload
```

## 项目结构

```
web/
├── frontend/                 # 前端React应用
│   ├── src/
│   │   ├── components/      # 可复用组件
│   │   ├── pages/          # 页面组件
│   │   ├── services/       # API服务
│   │   └── types/          # TypeScript类型定义
│   ├── public/             # 静态资源
│   └── package.json        # 依赖配置
│
├── backend/                 # 后端FastAPI应用
│   ├── app/
│   │   ├── api/            # API路由
│   │   ├── core/           # 核心配置
│   │   ├── models/         # 数据库模型
│   │   ├── schemas/        # Pydantic模式
│   │   ├── services/       # 业务服务
│   │   └── workers/        # 任务工作器
│   └── requirements.txt    # Python依赖
│
├── docker-compose.yml      # 多服务编排
├── architecture_design.md  # 架构设计文档
└── README.md              # 本文档
```

## API文档

启动后端服务后，访问以下地址查看API文档：
- Swagger UI：http://localhost:8000/docs
- ReDoc：http://localhost:8000/redoc

### 主要API端点

#### 认证
- `POST /api/v1/auth/login` - 用户登录
- `POST /api/v1/auth/register` - 用户注册
- `GET /api/v1/auth/me` - 获取当前用户

#### 工程管理
- `GET /api/v1/projects` - 获取工程列表
- `POST /api/v1/projects` - 创建工程（上传文件）
- `GET /api/v1/projects/{id}` - 获取工程详情
- `PUT /api/v1/projects/{id}` - 更新工程
- `DELETE /api/v1/projects/{id}` - 删除工程
- `POST /api/v1/projects/{id}/parse` - 解析工程文件

#### 迁移任务
- `GET /api/v1/tasks` - 获取任务列表
- `POST /api/v1/tasks` - 创建迁移任务
- `POST /api/v1/tasks/{id}/start` - 开始执行任务
- `POST /api/v1/tasks/{id}/cancel` - 取消任务
- `GET /api/v1/tasks/{id}/progress` - 获取任务进度

#### 文件管理
- `POST /api/v1/files/upload` - 上传文件
- `GET /api/v1/files/{id}/download` - 下载文件
- `DELETE /api/v1/files/{id}` - 删除文件

#### 系统监控
- `GET /api/v1/monitoring/health` - 健康检查
- `GET /api/v1/monitoring/stats` - 系统统计
- `GET /api/v1/monitoring/tasks` - 任务监控

## 数据库设计

### 核心表
1. **users** - 用户表
2. **projects** - 工程表
3. **migration_tasks** - 迁移任务表
4. **task_steps** - 任务步骤表
5. **system_configs** - 系统配置表

详细数据库设计见 `architecture_design.md`。

## 与现有迁移工具集成

Web管理界面通过以下方式与现有Python迁移工具集成：

1. **文件上传**：用户通过Web界面上传.aprx文件
2. **任务创建**：用户配置迁移参数并创建任务
3. **异步执行**：任务被添加到Redis队列，由工作器异步执行
4. **调用迁移工具**：工作器调用现有的 `script/migration_tool.py`
5. **进度跟踪**：实时更新任务进度和状态
6. **结果下载**：用户可下载迁移结果文件

## 配置说明

### 环境变量

后端服务支持以下环境变量配置：

```bash
# 数据库配置
DATABASE_URL=postgresql+asyncpg://postgres:password@localhost:5432/migration_db

# Redis配置
REDIS_URL=redis://localhost:6379/0

# MinIO配置
MINIO_ENDPOINT=localhost:9000
MINIO_ACCESS_KEY=minioadmin
MINIO_SECRET_KEY=minioadmin

# JWT配置
SECRET_KEY=your-secret-key-change-in-production
ACCESS_TOKEN_EXPIRE_MINUTES=30

# 应用配置
API_V1_STR=/api/v1
BACKEND_CORS_ORIGINS=["http://localhost:3000"]
DEBUG=true
```

### 配置文件

- `backend/.env` - 环境变量配置文件
- `backend/app/core/config.py` - 应用配置类
- `frontend/.env` - 前端环境变量

## 开发指南

### 添加新API端点

1. 在 `backend/app/api/v1/` 下创建新的路由模块
2. 在 `backend/app/schemas/` 下定义请求/响应模式
3. 在 `backend/app/services/` 下实现业务逻辑
4. 在 `backend/app/api/v1/api.py` 中注册路由
5. 更新API文档注释

### 添加新前端页面

1. 在 `frontend/src/pages/` 下创建页面组件
2. 在 `frontend/src/App.tsx` 中添加路由
3. 在 `frontend/src/services/api.ts` 中添加API调用
4. 在 `frontend/src/types/` 下添加TypeScript类型定义

### 数据库迁移

使用Alembic进行数据库迁移：

```bash
cd backend
alembic revision --autogenerate -m "描述"
alembic upgrade head
```

## 故障排除

### 常见问题

1. **服务启动失败**
   - 检查端口是否被占用
   - 检查Docker容器日志：`docker-compose logs [service_name]`
   - 检查环境变量配置

2. **数据库连接失败**
   - 检查PostgreSQL服务状态
   - 验证数据库连接字符串
   - 检查网络连接

3. **文件上传失败**
   - 检查文件大小限制
   - 检查文件类型限制
   - 检查存储服务状态

4. **任务执行失败**
   - 检查迁移工具路径配置
   - 检查Python环境
   - 查看任务日志

### 日志查看

```bash
# 查看所有服务日志
docker-compose logs -f

# 查看特定服务日志
docker-compose logs -f backend
docker-compose logs -f frontend
docker-compose logs -f db
```

## 性能优化

### 数据库优化
- 使用索引优化查询性能
- 配置连接池减少连接开销
- 使用读写分离（生产环境）

### 缓存策略
- 使用Redis缓存热点数据
- 配置HTTP缓存头
- 使用CDN加速静态资源

### 前端优化
- 代码分割和懒加载
- 图片压缩和WebP格式
- 服务端渲染（SSR）

### 后端优化
- 异步处理IO密集型任务
- 使用Gzip压缩响应
- 配置负载均衡

## 安全考虑

### 认证和授权
- 使用JWT进行无状态认证
- 密码使用bcrypt哈希存储
- 基于角色的访问控制（RBAC）

### 数据安全
- 输入验证和清理
- SQL注入防护
- 文件上传安全检查
- HTTPS传输加密

### 网络安全
- CORS配置限制
- 速率限制防止DDoS攻击
- 安全HTTP头配置

## 部署到生产环境

### 准备工作
1. 配置生产环境变量
2. 设置SSL证书
3. 配置域名和DNS
4. 设置监控和告警

### 部署步骤
1. 构建Docker镜像
2. 配置生产环境docker-compose.yml
3. 启动服务
4. 运行数据库迁移
5. 验证服务状态

### 监控和维护
1. 配置日志收集和分析
2. 设置性能监控
3. 定期备份数据
4. 安全更新和补丁

## 贡献指南

1. Fork项目
2. 创建功能分支
3. 提交更改
4. 推送到分支
5. 创建Pull Request

## 许可证

本项目基于MIT许可证 - 查看LICENSE文件了解详情。

## 支持

如有问题或建议，请：
1. 查看FAQ和文档
2. 提交Issue
3. 联系开发团队