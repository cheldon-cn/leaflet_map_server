# ArcGIS Pro到QGIS迁移工具 - 用户操作手册

## 概述

ArcGIS Pro到QGIS迁移工具是一个自动化迁移系统，支持将ArcGIS Pro工程文件（.aprx）及其关联数据、地图、符号、布局等内容完整迁移到QGIS环境。本工具采用两阶段架构设计，确保迁移过程准确、高效。

### 主要功能

1. **工程解析** - 完整解析ArcGIS Pro工程文件，提取地图、图层、数据源、坐标系等元数据
2. **XML中间文件生成** - 生成符合XSD 1.0规范的标准化中间文件
3. **数据迁移** - 支持多种数据源迁移：
   - 本地文件：Shapefile、File Geodatabase、栅格数据
   - 数据库：PostgreSQL/PostGIS大表（支持逻辑复制）
4. **样式转换** - 将ArcGIS Pro样式（.stylx）转换为QGIS样式（.xml）
5. **批量处理** - 支持大规模工程批量迁移（200+工程）
6. **完整验证** - 多层次验证体系确保迁移质量
7. **性能优化** - 智能任务调度和资源管理，确保30秒内完成单工程迁移

### 适用场景

- 从ArcGIS Pro平台迁移到QGIS平台
- GIS系统升级和平台迁移项目
- 大规模GIS数据整合和标准化
- 跨平台GIS项目协作

## 系统要求

### 硬件要求

| 组件 | 最低要求 | 推荐配置 |
|------|----------|----------|
| CPU | 4核 | 8核或更高 |
| 内存 | 8 GB | 16 GB或更高 |
| 存储 | 100 GB可用空间 | 500 GB以上（根据数据量） |
| 网络 | 100 Mbps | 1 Gbps或更高 |

### 软件要求

#### 必需软件
- **Python**: 3.8或更高版本
- **PostgreSQL**: 10或更高版本（如需数据库迁移）
- **ArcGIS Pro**: 2.x或3.x（用于解析.aprx文件）
- **QGIS**: 3.34或更高版本（用于导入迁移结果）

#### 可选软件
- **Docker**: 20.10或更高版本（用于容器化部署）
- **GDAL/OGR**: 3.0或更高版本（用于数据格式转换）

### 操作系统支持

- **Windows**: 10/11（推荐）, Server 2016+
- **Linux**: Ubuntu 20.04+, CentOS 8+, RHEL 8+
- **macOS**: 10.15+

## 安装指南

### 方法一：本地安装（推荐）

#### 1. 获取代码

```bash
# 克隆代码仓库
git clone <repository_url>
cd arcpro
```

#### 2. 创建虚拟环境

```bash
# Windows
python -m venv venv
venv\Scripts\activate

# Linux/Mac
python3 -m venv venv
source venv/bin/activate
```

#### 3. 安装依赖

```bash
pip install -r requirements.txt
```

#### 4. 配置环境

```bash
# 复制环境变量配置文件
copy .env.example .env  # Windows
cp .env.example .env    # Linux/Mac

# 编辑.env文件，设置数据库连接等参数
# 推荐使用文本编辑器打开.env文件进行配置
```

#### 5. 配置YAML文件

```bash
# 复制配置文件
copy config.yaml.example config.yaml  # Windows
cp config.yaml.example config.yaml    # Linux/Mac

# 编辑config.yaml文件，根据实际需求调整配置
```

#### 6. 验证安装

```bash
# 运行部署测试
python deploy_test.py

# 测试工具基本功能
python script/migration_tool.py --help
```

### 方法二：Docker安装

#### 1. 构建镜像

```bash
docker build -t arcpro-migration .
```

#### 2. 运行容器

```bash
# 单次运行
docker run -it --rm arcpro-migration

# 后台运行
docker run -d --name migration-tool arcpro-migration

# 挂载数据卷
docker run -it --rm -v $(pwd)/data:/data arcpro-migration
```

#### 3. 使用Docker Compose（包含PostgreSQL）

```bash
# 启动所有服务
docker-compose up -d

# 查看服务状态
docker-compose ps

# 停止服务
docker-compose down
```

### 方法三：云原生部署

参考`deploy_intergrate_steps_summary.md`中的云原生部署章节，支持Kubernetes部署。

## 快速开始

### 1. 准备工程文件

将需要迁移的ArcGIS Pro工程文件（.aprx）放入`data/input`目录：

```bash
# 创建输入目录（如果不存在）
mkdir -p data/input

# 复制工程文件
cp /path/to/your/project.aprx data/input/
```

### 2. 运行基本迁移

```bash
# 使用启动脚本（推荐）
./run_migration.sh --input data/input/project.aprx --output data/output

# 或直接使用Python
python script/migration_tool.py --input project.aprx --output ./output
```

### 3. 查看迁移结果

迁移完成后，检查输出目录：

```bash
# 查看生成的文件
ls -la data/output/

# 可能包含以下文件：
# - project_migration.xml      # 中间XML文件
# - project_report.txt         # 迁移报告
# - converted_data/            # 转换后的数据文件
# - qgis_styles/               # QGIS样式文件
```

### 4. 在QGIS中导入

1. 打开QGIS
2. 使用"导入XML"功能或运行QGIS插件
3. 选择生成的`project_migration.xml`文件
4. 按照向导完成工程重建

## 详细使用说明

### 命令行参数

迁移工具提供丰富的命令行参数，支持多种使用场景：

#### 基本参数

```bash
python script/migration_tool.py --help

# 必需参数
--input <aprx文件>          # 输入.aprx工程文件路径
--output <目录>             # 输出目录路径

# 可选参数
--config <配置文件>         # 指定配置文件路径（默认：config.yaml）
--verbose                   # 显示详细输出信息
--log-level <级别>         # 设置日志级别：DEBUG, INFO, WARNING, ERROR
```

#### 数据迁移参数

```bash
# 数据转换选项
--convert-data              # 转换本地数据（Shapefile/FileGDB到GeoPackage）
--data-format <格式>        # 数据输出格式：GPKG（默认）, SHP
--skip-local-data           # 跳过本地数据转换

# PostgreSQL迁移选项
--migrate-postgresql        # 迁移PostgreSQL数据
--pg-config <配置文件>      # PostgreSQL配置（JSON/YAML文件）
--pg-strategy <策略>        # 迁移策略：full_copy, connection_update, logical_replication
--skip-large-tables         # 跳过大表迁移
```

#### 样式转换参数

```bash
# 样式转换选项
--convert-styles            # 转换ArcGIS Pro样式
--style-output <目录>       # 样式输出目录
--style-format <格式>       # 样式输出格式：qgis, sld, mapbox
--skip-style-conversion     # 跳过样式转换
```

#### 验证和报告参数

```bash
# 验证选项
--validate                  # 启用迁移验证
--validation-level <级别>   # 验证级别：basic, standard, comprehensive
--validation-tolerance <值> # 验证容差（默认：0.001）

# 报告选项
--report <文件路径>         # 生成迁移报告
--report-format <格式>      # 报告格式：html, json, text
--summary-only             # 仅生成摘要报告
```

#### 性能优化参数

```bash
# 性能选项
--batch-size <数量>         # 批量处理大小（默认：10000）
--max-workers <数量>        # 最大工作线程数（默认：4）
--timeout <秒数>           # 任务超时时间（默认：3600）
--memory-limit <MB>        # 内存限制（MB）
```

### 使用示例

#### 示例1：基本迁移（仅生成XML）

```bash
python script/migration_tool.py \
  --input project.aprx \
  --output ./migration_output \
  --verbose
```

#### 示例2：完整迁移（包含数据和样式）

```bash
python script/migration_tool.py \
  --input project.aprx \
  --output ./full_migration \
  --convert-data \
  --migrate-postgresql \
  --convert-styles \
  --validate \
  --report migration_report.html
```

#### 示例3：批量处理

```bash
# 创建工程列表文件
echo "project1.aprx" > projects.txt
echo "project2.aprx" >> projects.txt
echo "project3.aprx" >> projects.txt

# 运行批量迁移
python script/migration_tool.py \
  --batch projects.txt \
  --output ./batch_output \
  --max-workers 8 \
  --batch-size 50000
```

#### 示例4：使用配置文件

```bash
# 创建自定义配置文件
cp config.yaml.example my_config.yaml
# 编辑my_config.yaml文件

# 使用自定义配置运行
python script/migration_tool.py \
  --input project.aprx \
  --output ./output \
  --config my_config.yaml
```

## 配置文件说明

### 环境变量配置（.env）

环境变量配置文件用于设置敏感信息和系统级配置：

#### 数据库配置

```bash
# 源数据库（ArcGIS Pro数据源）
PGHOST_SOURCE=source.db.example.com
PGPORT_SOURCE=5432
PGDATABASE_SOURCE=source_gis
PGUSER_SOURCE=source_user
# 密码通过PGPASSWORD_SOURCE环境变量设置

# 目标数据库（QGIS数据源）
PGHOST_TARGET=target.db.example.com
PGPORT_TARGET=5432
PGDATABASE_TARGET=target_gis
PGUSER_TARGET=target_user
# 密码通过PGPASSWORD_TARGET环境变量设置
```

#### 路径配置

```bash
# 工作目录
WORK_DIR=/opt/migration_tool

# 输入/输出目录
INPUT_DIR=/data/input
OUTPUT_DIR=/data/output
TEMP_DIR=/tmp/migration
```

#### 性能配置

```bash
# 批量处理大小
BATCH_SIZE=10000

# 最大工作线程数
MAX_WORKERS=4

# 内存限制
MEMORY_LIMIT_MB=4096
```

### YAML配置文件（config.yaml）

YAML配置文件用于设置应用级配置，支持更复杂的配置结构：

#### 数据库配置示例

```yaml
database:
  source:
    host: "source.db.example.com"
    port: 5432
    database: "source_gis"
    username: "source_user"
    ssl_mode: "prefer"
    
  target:
    host: "target.db.example.com"
    port: 5432
    database: "target_gis"
    username: "target_user"
    ssl_mode: "prefer"
```

#### 迁移策略配置

```yaml
migration:
  default_strategy: "full_copy"
  
  strategies:
    small_table: "full_copy"
    medium_table: "connection_update"
    large_table: "logical_replication"
  
  thresholds:
    small_table: 10000
    medium_table: 100000
    large_table: 1000000
```

#### 验证配置

```yaml
validation:
  level: "standard"  # basic, standard, comprehensive
  
  tolerance:
    geometry: 0.001
    attribute: 0.0
    coordinate: 0.01
  
  sampling:
    enabled: true
    sample_size: 10
```

## 高级功能

### PostgreSQL大表迁移

#### 迁移策略

工具支持多种PostgreSQL迁移策略，根据表大小自动选择：

1. **完全复制（Full Copy）** - 适用于小表（<10,000行）
2. **连接字符串更新（Connection Update）** - 适用于中等表（10,000-100,000行）
3. **逻辑复制（Logical Replication）** - 适用于大表（>100,000行）

#### 逻辑复制配置

```yaml
logical_replication:
  enabled: true
  publication_prefix: "pub_migration_"
  subscription_prefix: "sub_migration_"
  slot_name_prefix: "slot_migration_"
  max_sync_workers: 4
```

#### 使用示例

```bash
# 启用PostgreSQL迁移
python script/migration_tool.py \
  --input project.aprx \
  --output ./output \
  --migrate-postgresql \
  --pg-strategy logical_replication

# 指定大表阈值
python script/migration_tool.py \
  --input project.aprx \
  --output ./output \
  --migrate-postgresql \
  --large-table-threshold 500000
```

### 样式转换

#### 支持的样式类型

- 点符号（Marker Symbols）
- 线符号（Line Symbols）
- 填充符号（Fill Symbols）
- 文本符号（Text Symbols）
- 栅格符号（Raster Symbols）

#### 样式映射配置

```yaml
style_conversion:
  enabled: true
  mapping_file: "style_mapping.json"
  default_symbol_size: 2.0
  color_matching_tolerance: 0.1
```

#### 使用示例

```bash
# 转换样式
python script/migration_tool.py \
  --input project.aprx \
  --output ./output \
  --convert-styles \
  --style-output ./qgis_styles

# 指定样式映射文件
python script/migration_tool.py \
  --input project.aprx \
  --output ./output \
  --convert-styles \
  --style-mapping custom_mapping.json
```

### 批量处理

#### 批量处理配置

```yaml
performance:
  batch_size: 10000
  max_workers: 4
  timeout_seconds: 3600
  
  # 监控配置
  monitor_enabled: true
  monitor_interval_seconds: 5
  alert_threshold_cpu: 80
  alert_threshold_memory: 85
```

#### 批量处理示例

```bash
# 批量处理多个工程
python script/migration_tool.py \
  --batch projects.txt \
  --output ./batch_output \
  --max-workers 8 \
  --batch-size 50000 \
  --progress

# 监控批量处理进度
python script/migration_tool.py \
  --batch projects.txt \
  --output ./batch_output \
  --monitor \
  --monitor-interval 10
```

### 验证系统

#### 验证级别

1. **基本验证（Basic）** - 验证工程结构和元数据
2. **标准验证（Standard）** - 验证结构、属性和坐标系
3. **全面验证（Comprehensive）** - 验证所有内容，包括几何、符号和布局

#### 验证配置

```yaml
validation:
  level: "standard"
  
  tolerance:
    geometry: 0.001
    attribute: 0.0
    coordinate: 0.01
  
  reporting:
    format: "html"
    include_details: true
    save_to_file: true
```

#### 验证示例

```bash
# 运行验证
python script/migration_tool.py \
  --input project.aprx \
  --output ./output \
  --validate \
  --validation-level comprehensive \
  --validation-report validation_report.html
```

## 故障排除

### 常见问题及解决方案

#### 1. Python依赖安装失败

**问题**：`pip install -r requirements.txt` 失败

**解决方案**：
```bash
# 升级pip
pip install --upgrade pip

# 使用国内镜像源
pip install -r requirements.txt -i https://pypi.tuna.tsinghua.edu.cn/simple

# 或逐个安装依赖
pip install psycopg2-binary
pip install pyyaml
pip install lxml
pip install psutil
```

#### 2. 数据库连接失败

**问题**：无法连接到PostgreSQL数据库

**解决方案**：
1. 检查数据库服务是否运行
2. 验证连接参数（主机、端口、用户名、密码）
3. 检查防火墙设置
4. 确保用户有足够的权限

```bash
# 测试数据库连接
python -c "
import psycopg2
try:
    conn = psycopg2.connect(
        host='localhost',
        port=5432,
        database='test',
        user='postgres'
    )
    print('连接成功')
except Exception as e:
    print(f'连接失败: {e}')
"
```

#### 3. ArcGIS Pro工程解析失败

**问题**：无法解析.aprx文件

**解决方案**：
1. 确保在ArcGIS Pro环境中运行
2. 检查工程文件是否损坏
3. 验证arcpy模块是否可用

```bash
# 测试arcpy是否可用
python -c "
try:
    import arcpy
    print('arcpy可用')
except ImportError:
    print('arcpy不可用，切换到模拟模式')
"
```

#### 4. 内存不足错误

**问题**：处理大文件时内存不足

**解决方案**：
1. 增加系统内存
2. 调整批量处理大小
3. 启用内存优化模式

```bash
# 减少批量处理大小
python script/migration_tool.py \
  --input project.aprx \
  --output ./output \
  --batch-size 1000 \
  --memory-limit 2048
```

#### 5. 权限错误

**问题**：无法写入输出目录

**解决方案**：
1. 检查目录权限
2. 以管理员/root权限运行
3. 更改输出目录

```bash
# 检查目录权限
ls -la /data/output/

# 更改目录权限
chmod 755 /data/output
```

### 日志和调试

#### 启用详细日志

```bash
# 设置环境变量
export LOG_LEVEL=DEBUG

# 或使用命令行参数
python script/migration_tool.py \
  --input project.aprx \
  --output ./output \
  --verbose \
  --log-level DEBUG
```

#### 查看日志文件

```bash
# 查看应用日志
tail -f /var/log/migration_tool.log

# 查看错误日志
grep -i error /var/log/migration_tool.log

# 查看性能日志
grep -i performance /var/log/migration_tool.log
```

#### 生成调试报告

```bash
# 生成详细调试报告
python script/migration_tool.py \
  --input project.aprx \
  --output ./output \
  --debug \
  --debug-report debug_info.json
```

## 性能优化

### 性能基准测试

#### 运行基准测试

```bash
# 运行快速基准测试
python script/benchmark.py --mode quick

# 运行详细基准测试
python script/benchmark.py --mode detailed

# 运行自定义基准测试
python script/benchmark.py \
  --iterations 10 \
  --concurrency 4 \
  --output benchmark_results.json
```

#### 基准测试结果解读

基准测试会生成以下指标：
- **单工程迁移时间**：目标 <30秒
- **批量处理吞吐量**：目标 ≥5工程/分钟
- **内存使用率**：目标 <80%
- **CPU使用率**：目标 <70%

### 性能调优建议

#### 针对小规模数据（<1GB）

```yaml
performance:
  batch_size: 10000
  max_workers: 2
  memory_limit_mb: 2048
  cache_enabled: true
  cache_size_mb: 256
```

#### 针对中等规模数据（1GB-10GB）

```yaml
performance:
  batch_size: 50000
  max_workers: 4
  memory_limit_mb: 4096
  cache_enabled: true
  cache_size_mb: 512
```

#### 针对大规模数据（>10GB）

```yaml
performance:
  batch_size: 100000
  max_workers: 8
  memory_limit_mb: 8192
  cache_enabled: true
  cache_size_mb: 1024
  
  # 启用磁盘缓存
  disk_cache_enabled: true
  disk_cache_path: "/tmp/migration_cache"
```

## 监控和维护

### 系统监控

#### 实时监控

```bash
# 启用性能监控
python script/migration_tool.py \
  --input project.aprx \
  --output ./output \
  --monitor \
  --monitor-interval 5

# 查看监控数据
python script/performance_monitor.py --status
```

#### 资源监控指标

- **CPU使用率**：监控每个工作线程的CPU使用
- **内存使用**：监控进程内存和系统内存
- **磁盘I/O**：监控读写速度和磁盘空间
- **网络I/O**：监控网络带宽使用
- **数据库连接**：监控活跃连接数

### 健康检查

#### 运行健康检查

```bash
# 运行完整健康检查
python script/health_check.py --full

# 检查特定组件
python script/health_check.py --component database
python script/health_check.py --component filesystem
python script/health_check.py --component network
```

#### 健康检查项目

1. **数据库连接**：验证所有数据库连接
2. **文件系统**：检查磁盘空间和权限
3. **网络连接**：验证网络连通性
4. **服务状态**：检查相关服务状态
5. **配置验证**：验证配置文件有效性

### 定期维护

#### 清理临时文件

```bash
# 清理临时文件
python script/cleanup.py --temp

# 清理旧日志文件
python script/cleanup.py --logs --days 30

# 清理缓存文件
python script/cleanup.py --cache
```

#### 数据备份

```bash
# 备份配置文件
python script/backup.py --config

# 备份迁移结果
python script/backup.py --output --destination /backup/migration

# 完整备份
python script/backup.py --full --destination /backup/full
```

## 扩展和定制

### 插件系统

#### 插件开发

工具支持插件系统，可以扩展以下功能：

1. **数据转换插件**：支持新的数据格式
2. **样式转换插件**：支持新的样式类型
3. **验证器插件**：添加自定义验证规则
4. **优化器插件**：实现新的优化算法

#### 插件配置

```yaml
plugins:
  enabled: true
  directory: "./plugins"
  auto_discover: true
  
  whitelist:
    - "data_converter"
    - "style_converter"
    - "validator"
    - "optimizer"
```

### API集成

#### Python API

```python
import sys
sys.path.append('script')

from migration_tool import EnhancedMigrationTool

# 创建迁移工具实例
tool = EnhancedMigrationTool()

# 运行迁移
results = tool.run(
    aprx_path="project.aprx",
    output_xml="migration.xml",
    convert_data=True,
    migrate_postgresql=True,
    convert_styles=True,
    verbose=True
)

# 生成报告
report = tool.generate_enhanced_report(results, "migration_report.txt")
```

#### REST API（实验性）

```bash
# 启动API服务
python script/api_server.py --host 0.0.0.0 --port 8000

# 使用API
curl -X POST http://localhost:8000/api/migrate \
  -H "Content-Type: application/json" \
  -d '{"aprx_path": "project.aprx", "output_dir": "./output"}'
```

## 常见问题（FAQ）

### Q1: 迁移工具支持哪些ArcGIS Pro版本？
**A**: 支持ArcGIS Pro 2.x和3.x版本。建议使用最新版本以获得最佳兼容性。

### Q2: 迁移过程需要多长时间？
**A**: 单个工程迁移通常在30秒内完成。具体时间取决于工程复杂度和数据量。

### Q3: 是否支持批量处理？
**A**: 是的，支持批量处理多达200个工程。批量处理时建议增加系统资源。

### Q4: 迁移后数据是否会丢失？
**A**: 工具采用多层次验证体系，确保数据完整性。迁移前后会进行数据一致性验证。

### Q5: 是否支持自定义样式映射？
**A**: 是的，可以通过编辑`style_mapping.json`文件或创建自定义映射文件来实现。

### Q6: 如何处理迁移失败的情况？
**A**: 工具提供详细的错误日志和故障恢复机制。失败的任务可以重试，支持断点续传。

### Q7: 是否需要ArcGIS Pro许可证？
**A**: 解析.aprx文件需要ArcGIS Pro环境，但可以使用模拟模式进行测试。

### Q8: 是否支持其他数据库？
**A**: 目前主要支持PostgreSQL/PostGIS。未来版本计划支持更多数据库。

### Q9: 如何验证迁移质量？
**A**: 工具提供全面的验证系统，支持基本、标准和全面三个验证级别。

### Q10: 是否提供技术支持？
**A**: 请参考项目文档和GitHub Issues。企业用户可联系技术支持团队。

## 附录

### A. 命令行参数速查表

| 参数 | 简写 | 说明 | 默认值 |
|------|------|------|--------|
| --input | -i | 输入.aprx文件 | 必需 |
| --output | -o | 输出目录 | 必需 |
| --config | -c | 配置文件 | config.yaml |
| --verbose | -v | 详细输出 | false |
| --convert-data | -d | 转换本地数据 | false |
| --migrate-postgresql | -p | 迁移PostgreSQL数据 | false |
| --convert-styles | -s | 转换样式 | false |
| --validate | -V | 启用验证 | false |
| --batch | -b | 批量处理文件 | - |
| --max-workers | -w | 最大工作线程数 | 4 |
| --batch-size | -B | 批量处理大小 | 10000 |
| --help | -h | 显示帮助信息 | - |

### B. 配置文件示例

完整配置文件示例请参考`config.yaml.example`文件。

### C. 环境变量列表

完整环境变量列表请参考`.env.example`文件。

### D. 错误代码说明

| 错误代码 | 说明 | 建议操作 |
|----------|------|----------|
| 1001 | 输入文件不存在 | 检查文件路径 |
| 1002 | 输出目录无权限 | 检查目录权限 |
| 2001 | 数据库连接失败 | 检查数据库配置 |
| 2002 | 数据库权限不足 | 检查用户权限 |
| 3001 | 工程解析失败 | 检查工程文件 |
| 3002 | 数据转换失败 | 检查数据格式 |
| 4001 | 内存不足 | 减少批量大小或增加内存 |
| 4002 | 超时错误 | 增加超时时间 |

### E. 联系和支持

- **项目仓库**: [GitHub Repository URL]
- **问题反馈**: 通过GitHub Issues提交
- **文档更新**: 定期查看项目文档
- **社区支持**: 加入GIS迁移社区

---

**文档版本**: 1.0  
**最后更新**: 2026年3月4日  
**适用版本**: 迁移工具 v1.0+  
**维护团队**: ArcGIS Pro迁移项目组