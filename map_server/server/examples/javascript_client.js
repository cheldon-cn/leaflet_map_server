/**
 * Cycle Map Server JavaScript/Node.js 客户端示例
 * 
 * 这是一个完整的Node.js客户端实现，展示了如何与Cycle Map Server API进行交互。
 * 适用于服务器端Node.js应用或浏览器端JavaScript应用。
 */

class CycleMapClient {
    /**
     * 初始化客户端
     * @param {string} baseUrl - 服务器基础URL (例如: http://localhost:8080)
     * @param {string} username - 用户名
     * @param {string} password - 密码
     * @param {Object} options - 配置选项
     */
    constructor(baseUrl, username, password, options = {}) {
        this.baseUrl = baseUrl.replace(/\/$/, '');
        this.username = username;
        this.password = password;
        this.options = {
            timeout: 30000,
            retryAttempts: 3,
            retryDelay: 1000,
            ...options
        };
        
        this.token = null;
        this.headers = {
            'Content-Type': 'application/json'
        };
        
        // 自动登录
        this.login();
    }

    /**
     * 发送HTTP请求
     * @param {string} method - HTTP方法
     * @param {string} endpoint - API端点
     * @param {Object} data - 请求数据
     * @param {Object} params - 查询参数
     * @returns {Promise<Object>}
     */
    async _request(method, endpoint, data = null, params = {}) {
        const url = new URL(`${this.baseUrl}${endpoint}`);
        
        // 添加查询参数
        Object.keys(params).forEach(key => {
            url.searchParams.append(key, params[key]);
        });

        const config = {
            method: method,
            headers: this.headers,
            timeout: this.options.timeout
        };

        if (data && (method === 'POST' || method === 'PUT')) {
            config.body = JSON.stringify(data);
        }

        for (let attempt = 1; attempt <= this.options.retryAttempts; attempt++) {
            try {
                const response = await fetch(url.toString(), config);
                
                if (!response.ok) {
                    const errorData = await response.json().catch(() => ({}));
                    throw new Error(
                        `HTTP ${response.status}: ${errorData.error?.message || response.statusText}`
                    );
                }

                // 处理不同的响应类型
                const contentType = response.headers.get('content-type') || '';
                
                if (contentType.includes('application/json')) {
                    return await response.json();
                } else if (contentType.includes('image/')) {
                    return await response.blob();
                } else {
                    return await response.text();
                }

            } catch (error) {
                if (attempt === this.options.retryAttempts) {
                    throw error;
                }
                
                console.warn(`请求失败，第${attempt}次重试...`, error.message);
                await this._delay(this.options.retryDelay);
            }
        }
    }

    /**
     * 延迟函数
     * @param {number} ms - 延迟毫秒数
     */
    _delay(ms) {
        return new Promise(resolve => setTimeout(resolve, ms));
    }

    /**
     * 用户登录
     */
    async login() {
        try {
            const data = await this._request('POST', '/auth/login', {
                username: this.username,
                password: this.password
            });

            this.token = data.token;
            this.headers['Authorization'] = `Bearer ${this.token}`;

            console.log(`登录成功，用户: ${data.user.username}`);
            return data;

        } catch (error) {
            throw new Error(`登录失败: ${error.message}`);
        }
    }

    /**
     * 获取地图瓦片
     * @param {number} z - 缩放级别
     * @param {number} x - 瓦片X坐标
     * @param {number} y - 瓦片Y坐标
     * @param {string} format - 图像格式
     * @param {number} dpi - DPI设置
     * @returns {Promise<Blob>}
     */
    async getTile(z, x, y, format = 'png32', dpi = 96) {
        try {
            return await this._request(
                'GET', 
                `/tile/${z}/${x}/${y}/${format}`,
                null,
                { dpi }
            );
        } catch (error) {
            throw new Error(`获取瓦片失败: ${error.message}`);
        }
    }

    /**
     * 获取瓦片边界信息
     * @param {number} z - 缩放级别
     * @param {number} x - 瓦片X坐标
     * @param {number} y - 瓦片Y坐标
     * @returns {Promise<Object>}
     */
    async getTileBounds(z, x, y) {
        try {
            const data = await this._request('GET', `/tile/bounds/${z}/${x}/${y}/png32`);
            return data.bounds;
        } catch (error) {
            throw new Error(`获取瓦片边界失败: ${error.message}`);
        }
    }

    /**
     * 批量生成瓦片
     * @param {Object} bounds - 边界框
     * @param {number[]} zoomLevels - 缩放级别数组
     * @param {string} format - 图像格式
     * @param {number} dpi - DPI设置
     * @param {boolean} asyncMode - 是否异步模式
     * @returns {Promise<string>} 任务ID
     */
    async batchGenerateTiles(bounds, zoomLevels, format = 'png32', dpi = 96, asyncMode = true) {
        try {
            const payload = {
                bounds,
                zoom_levels: zoomLevels,
                format,
                dpi,
                async: asyncMode
            };

            const data = await this._request('POST', '/tile/generate', payload);
            console.log(`批量任务已创建，任务ID: ${data.task_id}`);
            return data.task_id;

        } catch (error) {
            throw new Error(`创建批量任务失败: ${error.message}`);
        }
    }

    /**
     * 获取服务器性能指标
     * @returns {Promise<Object>}
     */
    async getMetrics() {
        try {
            return await this._request('GET', '/metrics');
        } catch (error) {
            throw new Error(`获取指标失败: ${error.message}`);
        }
    }

    /**
     * 获取服务器健康状态
     * @returns {Promise<Object>}
     */
    async getHealth() {
        try {
            return await this._request('GET', '/health');
        } catch (error) {
            throw new Error(`健康检查失败: ${error.message}`);
        }
    }

    /**
     * 获取缓存统计信息
     * @returns {Promise<Object>}
     */
    async getCacheStats() {
        try {
            return await this._request('GET', '/cache/stats');
        } catch (error) {
            throw new Error(`获取缓存统计失败: ${error.message}`);
        }
    }

    /**
     * 清空缓存
     * @param {string} cacheType - 缓存类型
     * @returns {Promise<Object>}
     */
    async clearCache(cacheType = 'all') {
        try {
            return await this._request('DELETE', '/cache/clear', null, { type: cacheType });
        } catch (error) {
            throw new Error(`清空缓存失败: ${error.message}`);
        }
    }

    /**
     * 获取服务器配置
     * @returns {Promise<Object>}
     */
    async getConfig() {
        try {
            return await this._request('GET', '/config');
        } catch (error) {
            throw new Error(`获取配置失败: ${error.message}`);
        }
    }

    /**
     * 刷新JWT令牌
     * @returns {Promise<string>}
     */
    async refreshToken() {
        try {
            const data = await this._request('POST', '/auth/refresh');
            this.token = data.token;
            this.headers['Authorization'] = `Bearer ${this.token}`;
            
            console.log('令牌刷新成功');
            return this.token;

        } catch (error) {
            throw new Error(`令牌刷新失败: ${error.message}`);
        }
    }

    /**
     * 用户登出
     */
    async logout() {
        try {
            await this._request('POST', '/auth/logout');
            console.log('登出成功');
        } catch (error) {
            console.warn(`登出失败: ${error.message}`);
        } finally {
            this.token = null;
            delete this.headers['Authorization'];
        }
    }

    /**
     * 保存Blob到文件（Node.js环境）
     * @param {Blob} blob - Blob对象
     * @param {string} filePath - 文件路径
     */
    async saveBlobToFile(blob, filePath) {
        if (typeof window === 'undefined') {
            // Node.js环境
            const fs = await import('fs').then(m => m.promises);
            const buffer = Buffer.from(await blob.arrayBuffer());
            await fs.writeFile(filePath, buffer);
            console.log(`文件已保存: ${filePath}`);
        } else {
            // 浏览器环境
            const url = URL.createObjectURL(blob);
            const a = document.createElement('a');
            a.href = url;
            a.download = filePath.split('/').pop();
            a.click();
            URL.revokeObjectURL(url);
        }
    }
}

/**
 * 浏览器端使用示例
 */
async function browserExample() {
    const client = new CycleMapClient(
        'http://localhost:8080',
        'user',
        'password'
    );

    try {
        // 获取瓦片并在页面显示
        const tileBlob = await client.getTile(10, 512, 256, 'png32');
        const imgUrl = URL.createObjectURL(tileBlob);
        
        const img = document.createElement('img');
        img.src = imgUrl;
        img.alt = '地图瓦片';
        document.body.appendChild(img);
        
        // 获取健康状态
        const health = await client.getHealth();
        console.log('服务器状态:', health.status);
        
    } catch (error) {
        console.error('浏览器示例错误:', error);
    }
}

/**
 * Node.js端使用示例
 */
async function nodeExample() {
    const client = new CycleMapClient(
        'http://localhost:8080',
        'user',
        'password'
    );

    try {
        console.log('=== Cycle Map Server Node.js客户端示例 ===\n');

        // 1. 健康检查
        console.log('1. 服务器健康检查:');
        const health = await client.getHealth();
        console.log(`   状态: ${health.status}`);
        console.log(`   版本: ${health.version}`);
        console.log();

        // 2. 获取性能指标
        console.log('2. 服务器性能指标:');
        const metrics = await client.getMetrics();
        console.log(`   总请求数: ${metrics.requests.total}`);
        console.log(`   缓存命中率: ${(metrics.cache.memory_hit_rate * 100).toFixed(2)}%`);
        console.log();

        // 3. 获取单个瓦片
        console.log('3. 获取地图瓦片:');
        const tileBlob = await client.getTile(10, 512, 256, 'png32');
        console.log(`   瓦片大小: ${tileBlob.size} 字节`);
        
        // 保存瓦片到文件
        await client.saveBlobToFile(tileBlob, './tiles/tile_10_512_256.png');
        
        // 获取瓦片边界信息
        const bounds = await client.getTileBounds(10, 512, 256);
        console.log(`   瓦片边界:`, bounds);
        console.log();

        // 4. 批量生成瓦片示例
        console.log('4. 批量生成瓦片:');
        const boundsExample = {
            minX: -74.1,
            minY: 40.6,
            maxX: -73.8,
            maxY: 40.8
        };

        const taskId = await client.batchGenerateTiles(
            boundsExample,
            [10, 11],
            'png32',
            96,
            true
        );
        console.log(`   批量任务ID: ${taskId}`);
        console.log();

        // 5. 缓存管理
        console.log('5. 缓存统计:');
        const cacheStats = await client.getCacheStats();
        const memoryCache = cacheStats.memory_cache;
        console.log(`   内存缓存使用: ${(memoryCache.used / 1024 / 1024).toFixed(1)} MB / ${(memoryCache.size / 1024 / 1024).toFixed(1)} MB`);
        console.log(`   内存缓存命中率: ${(memoryCache.hit_rate * 100).toFixed(2)}%`);
        console.log();

        // 6. 获取服务器配置
        console.log('6. 服务器配置:');
        const config = await client.getConfig();
        console.log(`   数据库类型: ${config.database.type}`);
        console.log(`   服务器端口: ${config.server.port}`);
        console.log(`   缓存启用: ${config.cache.enabled}`);
        console.log();

        // 7. 令牌刷新
        console.log('7. 刷新认证令牌:');
        const newToken = await client.refreshToken();
        console.log(`   新令牌长度: ${newToken.length} 字符`);
        console.log();

        console.log('=== 示例执行完成 ===');

        // 登出
        await client.logout();

    } catch (error) {
        console.error('错误:', error);
    }
}

// 根据环境执行相应的示例
if (typeof window === 'undefined') {
    // Node.js环境
    nodeExample().catch(console.error);
} else {
    // 浏览器环境
    browserExample().catch(console.error);
}

module.exports = CycleMapClient;