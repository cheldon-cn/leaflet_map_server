# Cycle Map Server API 示例

本目录包含Cycle Map Server API的完整使用示例，包括多种编程语言的客户端实现。

## 📁 文件结构

```
examples/
├── README.md                 # 本说明文件
├── python_client.py          # Python客户端示例
├── javascript_client.js      # JavaScript/Node.js客户端示例
├── config_example.json        # 配置文件示例
└── docker-compose.yml        # Docker部署示例
```

## 🐍 Python 客户端

### 功能特性
- 完整的API封装
- 自动认证管理
- 错误处理和重试机制
- 文件保存功能
- 批量操作支持

### 安装依赖
```bash
pip install requests
```

### 快速开始
```python
from python_client import CycleMapClient

# 创建客户端
client = CycleMapClient(
    base_url="http://localhost:8080",
    username="user", 
    password="password"
)

# 获取瓦片
tile_data = client.get_tile(10, 512, 256, "png32")

# 保存到文件
with open("tile.png", "wb") as f:
    f.write(tile_data)

# 获取服务器指标
metrics = client.get_metrics()
print(f"缓存命中率: {metrics['cache']['memory_hit_rate']:.2%}")

# 登出
client.logout()
```

### 完整示例
运行完整的功能演示：
```bash
python python_client.py
```

## 🌐 JavaScript/Node.js 客户端

### 功能特性
- 支持浏览器和Node.js环境
- Promise-based异步API
- 自动重试机制
- Blob数据处理

### 浏览器使用
```html
<script type="module">
    import { CycleMapClient } from './javascript_client.js';
    
    const client = new CycleMapClient(
        'http://localhost:8080',
        'user',
        'password'
    );
    
    // 获取瓦片并显示
    client.getTile(10, 512, 256, 'png32')
        .then(blob => {
            const imgUrl = URL.createObjectURL(blob);
            document.getElementById('tile').src = imgUrl;
        });
</script>
```

### Node.js使用
```bash
npm install node-fetch
```

```javascript
const CycleMapClient = require('./javascript_client.js');

async function main() {
    const client = new CycleMapClient(
        'http://localhost:8080',
        'user',
        'password'
    );
    
    const tileBlob = await client.getTile(10, 512, 256, 'png32');
    await client.saveBlobToFile(tileBlob, './tile.png');
    
    await client.logout();
}

main().catch(console.error);
```

### 完整示例
运行完整的功能演示：
```bash
node javascript_client.js
```

## 🔧 配置示例

### config_example.json
```json
{
  "database": {
    "type": "sqlite3",
    "sqlite_path": "./data/map.db"
  },
  "server": {
    "host": "0.0.0.0",
    "port": 8080,
    "enable_https": false,
    "jwt_secret": "your-secret-key",
    "token_expiry_hours": 24
  },
  "cache": {
    "enabled": true,
    "memory_cache_size": 536870912
  }
}
```

## 🐳 Docker 部署示例

### docker-compose.yml
```yaml
version: '3.8'
services:
  map-server:
    image: cycle-map-server:latest
    ports:
      - "8080:8080"
      - "8443:8443"
    volumes:
      - ./data:/app/data
      - ./cache:/app/cache
      - ./logs:/app/logs
      - ./config.json:/app/config.json
    environment:
      - CYCLE_JWT_SECRET=your-secret-key
      - CYCLE_DB_PASSWORD=your-db-password
```

启动服务：
```bash
docker-compose up -d
```

## 🔌 API 端点速查

### 认证相关
- `POST /auth/login` - 用户登录
- `POST /auth/logout` - 用户登出  
- `POST /auth/refresh` - 刷新令牌
- `GET /auth/user` - 用户信息

### 瓦片服务
- `GET /tile/{z}/{x}/{y}/{format}` - 获取瓦片
- `GET /tile/bounds/{z}/{x}/{y}/{format}` - 瓦片边界
- `POST /tile/generate` - 批量生成

### 监控管理
- `GET /metrics` - 性能指标
- `GET /health` - 健康检查
- `GET /cache/stats` - 缓存统计
- `DELETE /cache/clear` - 清空缓存

### 配置管理
- `GET /config` - 获取配置
- `PUT /config` - 更新配置
- `GET /config/security` - 安全配置

## 🚀 高级用法

### 批量瓦片生成
```python
# 定义生成区域
bounds = {
    "minX": -74.1,
    "minY": 40.6,
    "maxX": -73.8,
    "maxY": 40.8
}

# 创建批量任务
task_id = client.batch_generate_tiles(
    bounds=bounds,
    zoom_levels=[10, 11, 12],
    format="png32",
    async_mode=True
)
```

### 性能基准测试
```javascript
// 执行性能测试
const benchmark = await client.performanceBenchmark({
    test_type: "concurrent_requests",
    concurrent_users: 100,
    requests_per_user: 100
});

// 获取测试结果
const results = await client.getPerformanceResults(benchmark.benchmark_id);
```

### 安全监控
```python
# 获取安全事件
events = client.get_security_events({
    "type": "auth_failure",
    "start_time": "2024-01-01T00:00:00Z",
    "limit": 50
})

# 配置安全告警
client.configure_security_alerts({
    "failed_login_threshold": 5,
    "email_notifications": True
})
```

## 🔒 安全最佳实践

### 1. 认证管理
```python
# 定期刷新令牌
async def maintain_session(client):
    while True:
        await asyncio.sleep(3600)  # 每小时刷新一次
        try:
            await client.refresh_token()
        except Exception as e:
            print(f"令牌刷新失败: {e}")
```

### 2. 错误处理
```javascript
// 实现重试机制
async function robustRequest(client, endpoint, maxRetries = 3) {
    for (let i = 0; i < maxRetries; i++) {
        try {
            return await client._request('GET', endpoint);
        } catch (error) {
            if (i === maxRetries - 1) throw error;
            await new Promise(resolve => setTimeout(resolve, 1000 * (i + 1)));
        }
    }
}
```

### 3. 性能优化
```python
# 使用连接池
import requests
from requests.adapters import HTTPAdapter
from requests.packages.urllib3.util.retry import Retry

session = requests.Session()
retry_strategy = Retry(
    total=3,
    backoff_factor=1,
    status_forcelist=[429, 500, 502, 503, 504]
)
adapter = HTTPAdapter(max_retries=retry_strategy)
session.mount("http://", adapter)
session.mount("https://", adapter)
```

## 🐛 故障排除

### 常见问题

1. **认证失败**
   - 检查用户名密码是否正确
   - 验证JWT密钥配置
   - 检查令牌是否过期

2. **连接超时**
   - 检查服务器是否运行
   - 验证网络连接
   - 调整超时时间设置

3. **权限不足**
   - 检查用户角色和权限
   - 验证API端点访问权限

4. **性能问题**
   - 检查服务器资源使用情况
   - 优化缓存策略
   - 调整并发设置

### 调试模式
启用详细日志输出：
```python
import logging
logging.basicConfig(level=logging.DEBUG)
```

## 📞 支持

- **文档**: 查看完整的API参考文档
- **问题**: 提交GitHub Issues
- **社区**: 加入开发者社区讨论

## 📄 许可证

本项目采用MIT许可证。详见LICENSE文件。