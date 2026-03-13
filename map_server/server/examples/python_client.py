#!/usr/bin/env python3
"""
Cycle Map Server Python 客户端示例

这是一个完整的Python客户端实现，展示了如何与Cycle Map Server API进行交互。
"""

import requests
import json
from typing import Dict, List, Optional, Any
import time
from pathlib import Path


class CycleMapClient:
    """Cycle Map Server Python客户端"""
    
    def __init__(self, base_url: str, username: str, password: str, 
                 verify_ssl: bool = True, timeout: int = 30):
        """
        初始化客户端
        
        Args:
            base_url: 服务器基础URL (例如: http://localhost:8080)
            username: 用户名
            password: 密码
            verify_ssl: 是否验证SSL证书
            timeout: 请求超时时间(秒)
        """
        self.base_url = base_url.rstrip('/')
        self.session = requests.Session()
        self.session.verify = verify_ssl
        self.timeout = timeout
        self.token = None
        
        # 登录获取令牌
        self._login(username, password)
    
    def _login(self, username: str, password: str):
        """用户登录"""
        try:
            response = self.session.post(
                f"{self.base_url}/auth/login",
                json={"username": username, "password": password},
                timeout=self.timeout
            )
            response.raise_for_status()
            
            data = response.json()
            self.token = data["token"]
            
            # 设置认证头
            self.session.headers.update({
                "Authorization": f"Bearer {self.token}",
                "Content-Type": "application/json"
            })
            
            print(f"登录成功，用户: {data['user']['username']}")
            
        except requests.exceptions.RequestException as e:
            raise Exception(f"登录失败: {e}")
    
    def get_tile(self, z: int, x: int, y: int, format: str = "png32", 
                 dpi: int = 96, save_path: Optional[str] = None) -> bytes:
        """
        获取地图瓦片
        
        Args:
            z: 缩放级别 (0-22)
            x: 瓦片X坐标
            y: 瓦片Y坐标
            format: 图像格式 (png8, png32, webp)
            dpi: DPI设置 (72-600)
            save_path: 保存路径，如果提供则保存文件
            
        Returns:
            瓦片图像二进制数据
        """
        try:
            response = self.session.get(
                f"{self.base_url}/tile/{z}/{x}/{y}/{format}",
                params={"dpi": dpi},
                timeout=self.timeout
            )
            response.raise_for_status()
            
            tile_data = response.content
            
            if save_path:
                Path(save_path).parent.mkdir(parents=True, exist_ok=True)
                with open(save_path, 'wb') as f:
                    f.write(tile_data)
                print(f"瓦片已保存: {save_path}")
            
            return tile_data
            
        except requests.exceptions.RequestException as e:
            raise Exception(f"获取瓦片失败: {e}")
    
    def get_tile_bounds(self, z: int, x: int, y: int) -> Dict[str, float]:
        """获取瓦片边界框信息"""
        try:
            response = self.session.get(
                f"{self.base_url}/tile/bounds/{z}/{x}/{y}/png32",
                timeout=self.timeout
            )
            response.raise_for_status()
            return response.json()["bounds"]
            
        except requests.exceptions.RequestException as e:
            raise Exception(f"获取瓦片边界失败: {e}")
    
    def batch_generate_tiles(self, bounds: Dict[str, float], 
                            zoom_levels: List[int], format: str = "png32",
                            dpi: int = 96, async_mode: bool = True) -> str:
        """批量生成瓦片"""
        try:
            payload = {
                "bounds": bounds,
                "zoom_levels": zoom_levels,
                "format": format,
                "dpi": dpi,
                "async": async_mode
            }
            
            response = self.session.post(
                f"{self.base_url}/tile/generate",
                json=payload,
                timeout=self.timeout
            )
            response.raise_for_status()
            
            data = response.json()
            task_id = data["task_id"]
            print(f"批量任务已创建，任务ID: {task_id}")
            
            return task_id
            
        except requests.exceptions.RequestException as e:
            raise Exception(f"创建批量任务失败: {e}")
    
    def get_metrics(self) -> Dict[str, Any]:
        """获取服务器性能指标"""
        try:
            response = self.session.get(
                f"{self.base_url}/metrics",
                timeout=self.timeout
            )
            response.raise_for_status()
            return response.json()
            
        except requests.exceptions.RequestException as e:
            raise Exception(f"获取指标失败: {e}")
    
    def get_health(self) -> Dict[str, Any]:
        """获取服务器健康状态"""
        try:
            response = self.session.get(
                f"{self.base_url}/health",
                timeout=self.timeout
            )
            response.raise_for_status()
            return response.json()
            
        except requests.exceptions.RequestException as e:
            raise Exception(f"健康检查失败: {e}")
    
    def get_cache_stats(self) -> Dict[str, Any]:
        """获取缓存统计信息"""
        try:
            response = self.session.get(
                f"{self.base_url}/cache/stats",
                timeout=self.timeout
            )
            response.raise_for_status()
            return response.json()
            
        except requests.exceptions.RequestException as e:
            raise Exception(f"获取缓存统计失败: {e}")
    
    def clear_cache(self, cache_type: str = "all") -> Dict[str, Any]:
        """清空缓存"""
        try:
            response = self.session.delete(
                f"{self.base_url}/cache/clear",
                params={"type": cache_type},
                timeout=self.timeout
            )
            response.raise_for_status()
            return response.json()
            
        except requests.exceptions.RequestException as e:
            raise Exception(f"清空缓存失败: {e}")
    
    def get_config(self) -> Dict[str, Any]:
        """获取服务器配置"""
        try:
            response = self.session.get(
                f"{self.base_url}/config",
                timeout=self.timeout
            )
            response.raise_for_status()
            return response.json()
            
        except requests.exceptions.RequestException as e:
            raise Exception(f"获取配置失败: {e}")
    
    def refresh_token(self) -> str:
        """刷新JWT令牌"""
        try:
            response = self.session.post(
                f"{self.base_url}/auth/refresh",
                timeout=self.timeout
            )
            response.raise_for_status()
            
            data = response.json()
            self.token = data["token"]
            
            # 更新认证头
            self.session.headers.update({
                "Authorization": f"Bearer {self.token}"
            })
            
            print("令牌刷新成功")
            return self.token
            
        except requests.exceptions.RequestException as e:
            raise Exception(f"令牌刷新失败: {e}")
    
    def logout(self):
        """用户登出"""
        try:
            response = self.session.post(
                f"{self.base_url}/auth/logout",
                timeout=self.timeout
            )
            response.raise_for_status()
            print("登出成功")
            
        except requests.exceptions.RequestException as e:
            print(f"登出失败: {e}")
        finally:
            self.token = None
            self.session.close()


def main():
    """使用示例"""
    
    # 客户端配置
    BASE_URL = "http://localhost:8080"
    USERNAME = "user"
    PASSWORD = "password"
    
    try:
        # 创建客户端
        client = CycleMapClient(BASE_URL, USERNAME, PASSWORD)
        
        print("=== Cycle Map Server Python客户端示例 ===\n")
        
        # 1. 健康检查
        print("1. 服务器健康检查:")
        health = client.get_health()
        print(f"   状态: {health['status']}")
        print(f"   版本: {health['version']}")
        print()
        
        # 2. 获取性能指标
        print("2. 服务器性能指标:")
        metrics = client.get_metrics()
        print(f"   总请求数: {metrics['requests']['total']}")
        print(f"   缓存命中率: {metrics['cache']['memory_hit_rate']:.2%}")
        print()
        
        # 3. 获取单个瓦片
        print("3. 获取地图瓦片:")
        tile_data = client.get_tile(10, 512, 256, "png32", save_path="./tiles/tile_10_512_256.png")
        print(f"   瓦片大小: {len(tile_data)} 字节")
        
        # 获取瓦片边界信息
        bounds = client.get_tile_bounds(10, 512, 256)
        print(f"   瓦片边界: {bounds}")
        print()
        
        # 4. 批量生成瓦片示例
        print("4. 批量生成瓦片:")
        bounds_example = {
            "minX": -74.1,
            "minY": 40.6,
            "maxX": -73.8,
            "maxY": 40.8
        }
        
        task_id = client.batch_generate_tiles(
            bounds=bounds_example,
            zoom_levels=[10, 11],
            format="png32",
            async_mode=True
        )
        print(f"   批量任务ID: {task_id}")
        print()
        
        # 5. 缓存管理
        print("5. 缓存统计:")
        cache_stats = client.get_cache_stats()
        memory_cache = cache_stats['memory_cache']
        print(f"   内存缓存使用: {memory_cache['used'] / 1024 / 1024:.1f} MB / {memory_cache['size'] / 1024 / 1024:.1f} MB")
        print(f"   内存缓存命中率: {memory_cache['hit_rate']:.2%}")
        print()
        
        # 6. 获取服务器配置
        print("6. 服务器配置:")
        config = client.get_config()
        print(f"   数据库类型: {config['database']['type']}")
        print(f"   服务器端口: {config['server']['port']}")
        print(f"   缓存启用: {config['cache']['enabled']}")
        print()
        
        # 7. 令牌刷新
        print("7. 刷新认证令牌:")
        new_token = client.refresh_token()
        print(f"   新令牌长度: {len(new_token)} 字符")
        print()
        
        # 8. 清空缓存（可选）
        # print("8. 清空缓存:")
        # result = client.clear_cache("memory")
        # print(f"   释放内存: {result['freed_memory'] / 1024 / 1024:.1f} MB")
        # print()
        
        print("=== 示例执行完成 ===")
        
        # 登出
        client.logout()
        
    except Exception as e:
        print(f"错误: {e}")


if __name__ == "__main__":
    main()