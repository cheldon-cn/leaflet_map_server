# 部署与集成步骤总结

## 项目概述
ArcGIS Pro到QGIS迁移系统是一个自动化迁移工具，支持将ArcGIS Pro工程文件(.aprx)及其关联数据、地图、符号、布局等内容迁移到QGIS环境。

## 部署目标
1. 在测试环境部署迁移工具
2. 集成到现有GIS工作流
3. 提供命令行接口和配置管理

## 环境要求
- Python 3.8+
- PostgreSQL 10+（支持逻辑复制）
- ArcGIS Pro 2.x/3.x（仅迁移工作站需要）
- QGIS 3.34+（仅目标环境需要）

## 部署步骤

### 步骤1：环境准备
1. **操作系统**: Windows 10/11（推荐）或Linux
2. **Python环境**: 安装Python 3.8或更高版本
3. **虚拟环境**: 建议使用venv或conda创建隔离环境

### 步骤2：依赖安装
创建requirements.txt文件并安装依赖：

```bash
# 创建虚拟环境
python -m venv venv
source venv/bin/activate  # Linux/Mac
venv\Scripts\activate     # Windows

# 安装依赖
pip install -r requirements.txt
```

### 步骤3：配置文件设置
创建环境变量配置文件：

1. **数据库连接配置**：
   - 设置`PGPASSWORD`环境变量存储密码
   - 或使用`PGPASSWORD_{USERNAME}`格式

2. **路径配置**：
   - 设置工作目录路径
   - 配置输入/输出目录

### 步骤4：工具验证
运行基本功能测试：

```bash
# 测试导入
python script/check_imports.py

# 运行性能基准测试
python script/benchmark.py --mode quick

# 测试单个工程迁移
python script/migration_tool.py --help
```

### 步骤5：部署方式选择

#### 方案A：本地部署
1. 将代码复制到目标机器
2. 配置环境变量
3. 创建启动脚本
4. 设置定时任务（如需批量处理）

#### 方案B：容器化部署（Docker）
1. 创建Dockerfile
2. 构建Docker镜像
3. 配置Docker Compose（如需数据库）
4. 部署到容器平台

#### 方案C：云原生部署
1. 准备Kubernetes部署文件
2. 配置持久化存储
3. 设置监控和日志

### 步骤6：集成到GIS工作流

#### 命令行集成
```bash
# 单个工程迁移
python script/migration_tool.py --input /path/to/project.aprx --output /path/to/output

# 批量处理
python script/migration_tool.py --batch /path/to/projects.txt --output /path/to/output_dir
```

#### 脚本集成
在现有GIS处理流程中调用迁移工具：

```python
import subprocess
import os

def migrate_project(aprx_path, output_dir):
    cmd = [
        "python", "script/migration_tool.py",
        "--input", aprx_path,
        "--output", output_dir
    ]
    result = subprocess.run(cmd, capture_output=True, text=True)
    return result.returncode == 0
```

#### API集成（未来扩展）
1. 创建RESTful API服务
2. 提供Web界面
3. 支持远程调用

## 配置文件示例

### requirements.txt
```
psycopg2-binary>=2.9.5
pyyaml>=6.0
lxml>=4.9.0
# 可选：arcpy（仅在ArcGIS Pro环境需要）
# 可选：qgis（仅在QGIS环境需要）
```

### .env 环境变量示例
```
# 数据库配置
PGHOST=localhost
PGPORT=5432
PGDATABASE=migration_db
PGUSER=migration_user
PGPASSWORD=secure_password

# 路径配置
WORK_DIR=/opt/migration_tool
INPUT_DIR=/data/input
OUTPUT_DIR=/data/output
```

### config.yaml 示例
```yaml
database:
  source:
    host: "source.db.example.com"
    port: 5432
    database: "source_gis"
    username: "source_user"
    # 密码从环境变量读取
  target:
    host: "target.db.example.com"
    port: 5432
    database: "target_gis"
    username: "target_user"

paths:
  workspace: "/opt/migration_tool"
  input: "/data/input"
  output: "/data/output"
  temp: "/tmp/migration"

performance:
  batch_size: 10000
  max_workers: 4
  timeout_seconds: 3600
```

## 验证与测试

### 功能验证
1. **工程解析测试**: 验证.aprx文件解析功能
2. **数据转换测试**: 验证Shapefile/FileGDB到GeoPackage转换
3. **PostgreSQL迁移测试**: 验证数据库表迁移功能
4. **样式转换测试**: 验证符号系统映射

### 性能验证
1. **30秒迁移目标**: 使用benchmark.py验证单个工程迁移时间
2. **批量处理能力**: 测试200个工程批量处理效率
3. **资源使用**: 监控CPU、内存、磁盘I/O

### 兼容性验证
1. **版本兼容性**: 使用compatibility_checker.py检查GIS软件版本
2. **数据格式兼容性**: 验证不同数据格式支持

## 监控与维护

### 日志配置
- 应用日志：记录迁移过程和错误
- 系统日志：记录资源使用情况
- 审计日志：记录用户操作

### 健康检查
1. 定期检查数据库连接
2. 监控磁盘空间
3. 检查任务队列状态

### 故障恢复
1. 实现任务重试机制
2. 配置告警通知
3. 定期备份配置和数据

## 安全考虑

### 访问控制
1. 限制数据库访问权限
2. 实现用户认证（如需）
3. 文件系统权限最小化

### 数据安全
1. 敏感信息环境变量化
2. 日志中排除敏感数据
3. 数据传输加密

### 代码安全
1. 定期更新依赖包
2. 安全扫描工具检查
3. 代码审计

## 扩展与定制

### 插件系统
使用plugin_system.py扩展功能：
1. 数据转换插件
2. 样式转换插件
3. 验证器插件

### 配置扩展
支持多种配置方式：
1. 环境变量
2. YAML配置文件
3. 命令行参数
4. API配置

### 部署扩展
支持多种部署场景：
1. 单机部署
2. 分布式部署
3. 云原生部署

## 已创建部署文件

### 核心部署文件
1. **requirements.txt** - Python依赖包列表
2. **Dockerfile** - Docker容器构建配置
3. **docker-compose.yml** - Docker Compose服务配置
4. **.env.example** - 环境变量配置示例
5. **config.yaml.example** - YAML配置文件示例
6. **run_migration.bat** - Windows启动脚本
7. **run_migration.sh** - Linux/Mac启动脚本
8. **deploy_test.py** - 部署测试脚本
9. **integration_example.py** - 集成示例脚本
10. **user_manual.md** - 用户操作手册

### 文件说明
| 文件 | 用途 | 部署场景 |
|------|------|----------|
| requirements.txt | Python依赖管理 | 所有部署方式 |
| Dockerfile | 容器镜像构建 | Docker/Kubernetes部署 |
| docker-compose.yml | 多服务容器编排 | 本地开发/测试环境 |
| .env.example | 环境变量配置模板 | 所有部署方式 |
| config.yaml.example | 应用配置模板 | 所有部署方式 |
| run_migration.bat | Windows启动脚本 | Windows服务器部署 |
| run_migration.sh | Linux/Mac启动脚本 | Linux服务器部署 |
| deploy_test.py | 部署环境验证 | 部署后验证 |
| integration_example.py | 集成示例代码 | 现有系统集成参考 |
| user_manual.md | 用户操作手册 | 所有部署方式 |

### 部署验证结果

### 测试执行
运行部署测试脚本验证环境准备情况：

```bash
python deploy_test.py
```

### 测试结果摘要
| 测试项目 | 状态 | 说明 |
|----------|------|------|
| Python版本检查 (3.8+) | ✅ 通过 | Python 3.8.10满足要求 |
| 依赖包安装验证 | ✅ 通过 | psycopg2-binary, PyYAML, lxml, psutil已安装 |
| 模块导入测试 | ⚠️ 部分通过 | 核心模块可导入，data_models导入需要PYTHONPATH设置 |
| 配置文件检查 | ✅ 通过 | 所有配置文件存在且格式正确 |
| 脚本文件验证 | ⚠️ 部分通过 | 启动脚本存在，权限需检查 |
| 目录结构检查 | ✅ 通过 | 所需目录结构完整 |
| 迁移工具基本功能 | ✅ 通过 | 命令行帮助功能正常 |
| 性能基准测试验证 | ⚠️ 部分通过 | 基准测试框架存在，部分依赖需调整 |

### 关键发现
1. **Python依赖**：已成功安装核心依赖包
2. **配置管理**：配置文件完整，包含多种部署方案
3. **模块导入**：需要正确设置PYTHONPATH或使用提供的启动脚本
4. **数据目录**：已自动创建输入/输出/临时目录

### 解决方案
1. **PYTHONPATH设置**：
   ```bash
   # Windows
   set PYTHONPATH=script
   
   # Linux/Mac
   export PYTHONPATH=script
   ```

2. **使用启动脚本**（推荐）：
   ```bash
   # Windows
   run_migration.bat --help
   
   # Linux/Mac
   chmod +x run_migration.sh
   ./run_migration.sh --help
   ```

3. **依赖安装**：
   ```bash
   pip install -r requirements.txt
   ```

## 部署执行步骤

### 1. 本地部署（推荐）
```bash
# 1. 克隆代码
git clone <repository_url>
cd arcpro

# 2. 创建虚拟环境
python -m venv venv
venv\Scripts\activate  # Windows
source venv/bin/activate  # Linux/Mac

# 3. 安装依赖
pip install -r requirements.txt

# 4. 配置环境
copy .env.example .env
# 编辑.env文件，设置数据库连接等参数

# 5. 运行测试
python deploy_test.py

# 6. 使用迁移工具
python script/migration_tool.py --help
```

### 2. Docker部署
```bash
# 1. 构建镜像
docker build -t arcpro-migration .

# 2. 运行容器
docker run -it --rm arcpro-migration

# 3. 使用Docker Compose（包含PostgreSQL）
docker-compose up -d
```

### 3. 集成到现有工作流
#### 命令行集成
```bash
# 单个工程迁移
python script/migration_tool.py --input project.aprx --output ./output

# 批量处理
python script/migration_tool.py --batch projects.txt --output ./output
```

#### Python脚本集成
```python
import subprocess
import os

def migrate_aprx(aprx_path, output_dir):
    """迁移单个APRX工程"""
    cmd = [
        "python", "script/migration_tool.py",
        "--input", aprx_path,
        "--output", output_dir,
        "--config", "config.yaml"
    ]
    result = subprocess.run(cmd, capture_output=True, text=True)
    return result.returncode == 0
```

## 监控和维护

### 日志管理
- 应用日志：`/var/log/migration_tool.log`（可配置）
- 错误日志：通过标准错误输出捕获
- 审计日志：记录所有迁移操作

### 性能监控
- 使用`benchmark.py`定期运行性能测试
- 监控CPU、内存、磁盘I/O使用情况
- 设置告警阈值（通过环境变量配置）

### 健康检查
```bash
# 检查数据库连接
python -c "from script.postgresql_migrator import DatabaseConfig; print('Database OK')"

# 检查工具功能
python script/migration_tool.py --version
```

## 故障排除

### 常见问题
1. **Python依赖安装失败**
   - 确保Python版本≥3.8
   - 使用`pip install --upgrade pip`
   - 尝试使用`requirements.txt`中的精确版本

2. **数据库连接失败**
   - 检查环境变量配置
   - 验证网络连接和防火墙设置
   - 确认数据库用户权限

3. **文件权限问题**
   - 确保输入/输出目录有读写权限
   - 检查临时目录空间充足

4. **性能问题**
   - 调整`config.yaml`中的性能参数
   - 增加批量处理大小
   - 优化数据库连接池

### 获取帮助
1. 查看详细日志：`tail -f /var/log/migration_tool.log`
2. 运行诊断：`python script/check_imports.py`
3. 性能分析：`python script/benchmark.py --mode detailed`

## 后续步骤

### 短期行动
1. [x] 创建requirements.txt文件
2. [x] 编写Dockerfile
3. [x] 创建启动脚本
4. [x] 配置环境变量示例
5. [ ] 在实际测试环境部署验证
6. [x] 创建用户操作手册

### 中期计划
1. [ ] 实现Web管理界面
2. [ ] 添加RESTful API支持
3. [ ] 集成到CI/CD流水线
4. [ ] 开发更多插件扩展

### 长期规划
1. [ ] 支持更多GIS平台（ArcMap, AutoCAD Map等）
2. [ ] 云原生架构优化（Kubernetes部署）
3. [ ] 机器学习辅助迁移质量评估
4. [ ] 实时迁移监控仪表板

---

**文档版本**: 2.1  
**更新日期**: 2026年3月4日  
**负责人**: 部署团队  
**测试状态**: 已通过基本部署测试  
**新增内容**: 用户操作手册 (user_manual.md) 已创建