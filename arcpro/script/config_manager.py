"""
配置管理模块
提供统一的配置加载、验证和管理功能
"""

import os
import json
import logging
from typing import Dict, Any, Optional
from dataclasses import dataclass, field

# YAML支持可选
try:
    import yaml
    YAML_AVAILABLE = True
except ImportError:
    YAML_AVAILABLE = False


@dataclass
class DatabaseConfig:
    """数据库配置"""
    host: str = "localhost"
    port: int = 5432
    database: str = ""
    schema: str = "public"
    username: str = "postgres"
    password: str = ""
    ssl_mode: str = "disable"
    
    def __post_init__(self):
        """初始化后处理，从环境变量读取密码"""
        if not self.password and self.username:
            # 尝试从环境变量读取密码
            env_var_name = f"PGPASSWORD_{self.username.upper()}"
            self.password = os.environ.get(env_var_name, os.environ.get('PGPASSWORD', ''))
    
    @classmethod
    def from_dict(cls, config_dict: Dict[str, Any]) -> 'DatabaseConfig':
        """从字典创建配置"""
        return cls(**{
            k: v for k, v in config_dict.items() 
            if k in ['host', 'port', 'database', 'schema', 'username', 'password', 'ssl_mode']
        })


@dataclass
class MigrationConfig:
    """迁移配置"""
    timeout_per_project: int = 300
    max_workers: int = 4
    sample_size: int = 10
    validation_level: str = "comprehensive"
    performance_monitoring: bool = True
    
    @classmethod
    def from_dict(cls, config_dict: Dict[str, Any]) -> 'MigrationConfig':
        """从字典创建配置"""
        return cls(**{
            k: v for k, v in config_dict.items() 
            if k in ['timeout_per_project', 'max_workers', 'sample_size', 
                    'validation_level', 'performance_monitoring']
        })


@dataclass
class ValidationWeightsConfig:
    """验证权重配置"""
    structure: float = 0.3
    attribute: float = 0.2
    geometry: float = 0.2
    symbology: float = 0.15
    layout: float = 0.15
    
    @classmethod
    def from_dict(cls, config_dict: Dict[str, Any]) -> 'ValidationWeightsConfig':
        """从字典创建配置"""
        return cls(**{
            k: v for k, v in config_dict.items() 
            if k in ['structure', 'attribute', 'geometry', 'symbology', 'layout']
        })


class ConfigManager:
    """配置管理器"""
    
    def __init__(self, config_path: Optional[str] = None):
        self.config_path = config_path
        self.logger = logging.getLogger(__name__)
        self.config: Dict[str, Any] = {}
        
        # 默认配置
        self.default_config = {
            'database': {
                'host': 'localhost',
                'port': 5432,
                'username': 'postgres',
                'password': '',
                'ssl_mode': 'disable'
            },
            'migration': {
                'timeout_per_project': 300,
                'max_workers': 4,
                'sample_size': 10,
                'validation_level': 'comprehensive'
            },
            'validation': {
                'weights': {
                    'structure': 0.3,
                    'attribute': 0.2,
                    'geometry': 0.2,
                    'symbology': 0.15,
                    'layout': 0.15
                },
                'tolerance': {
                    'geometry': 0.001,
                    'attribute': 0.0,
                    'visual': 0.05,
                    'layout': 1.0
                }
            },
            'performance': {
                'monitoring_interval': 1.0,
                'max_history_size': 1000
            }
        }
        
        self.load_config()
    
    def load_config(self) -> None:
        """加载配置"""
        # 首先加载默认配置
        self.config = self.default_config.copy()
        
        # 从配置文件加载（如果存在）
        if self.config_path and os.path.exists(self.config_path):
            try:
                with open(self.config_path, 'r', encoding='utf-8') as f:
                    if self.config_path.endswith('.json'):
                        file_config = json.load(f)
                    elif self.config_path.endswith('.yaml') or self.config_path.endswith('.yml'):
                        file_config = yaml.safe_load(f)
                    else:
                        self.logger.warning(f"不支持的配置文件格式: {self.config_path}")
                        return
                
                # 深度合并配置
                self._merge_config(self.config, file_config)
                self.logger.info(f"已加载配置文件: {self.config_path}")
                
            except Exception as e:
                self.logger.error(f"加载配置文件失败: {e}")
        
        # 从环境变量加载
        self._load_from_env()
    
    def _merge_config(self, base: Dict[str, Any], override: Dict[str, Any]) -> None:
        """深度合并配置"""
        for key, value in override.items():
            if key in base and isinstance(base[key], dict) and isinstance(value, dict):
                self._merge_config(base[key], value)
            else:
                base[key] = value
    
    def _load_from_env(self) -> None:
        """从环境变量加载配置"""
        # 数据库配置
        if os.environ.get('DB_HOST'):
            self.config['database']['host'] = os.environ['DB_HOST']
        if os.environ.get('DB_PORT'):
            self.config['database']['port'] = int(os.environ['DB_PORT'])
        if os.environ.get('DB_USER'):
            self.config['database']['username'] = os.environ['DB_USER']
        if os.environ.get('DB_PASSWORD'):
            self.config['database']['password'] = os.environ['DB_PASSWORD']
        
        # 迁移配置
        if os.environ.get('MIGRATION_TIMEOUT'):
            self.config['migration']['timeout_per_project'] = int(os.environ['MIGRATION_TIMEOUT'])
        if os.environ.get('MIGRATION_MAX_WORKERS'):
            self.config['migration']['max_workers'] = int(os.environ['MIGRATION_MAX_WORKERS'])
        
        # 验证权重配置
        weights_str = os.environ.get('VALIDATION_WEIGHTS', '')
        if weights_str:
            try:
                weights = {}
                for item in weights_str.split(','):
                    if '=' in item:
                        key, value = item.split('=', 1)
                        weights[key.strip()] = float(value.strip())
                
                if weights:
                    self.config['validation']['weights'] = weights
            except Exception as e:
                self.logger.warning(f"解析环境变量 VALIDATION_WEIGHTS 失败: {e}")
    
    def get_database_config(self) -> DatabaseConfig:
        """获取数据库配置"""
        return DatabaseConfig.from_dict(self.config['database'])
    
    def get_migration_config(self) -> MigrationConfig:
        """获取迁移配置"""
        return MigrationConfig.from_dict(self.config['migration'])
    
    def get_validation_weights(self) -> ValidationWeightsConfig:
        """获取验证权重配置"""
        return ValidationWeightsConfig.from_dict(self.config['validation']['weights'])
    
    def get_validation_tolerance(self) -> Dict[str, float]:
        """获取验证容差配置"""
        return self.config['validation']['tolerance']
    
    def get_performance_config(self) -> Dict[str, Any]:
        """获取性能配置"""
        return self.config['performance']
    
    def save_config(self, file_path: Optional[str] = None) -> None:
        """保存配置到文件"""
        save_path = file_path or self.config_path
        if not save_path:
            self.logger.warning("未指定配置文件路径")
            return
        
        try:
            os.makedirs(os.path.dirname(save_path), exist_ok=True)
            
            with open(save_path, 'w', encoding='utf-8') as f:
                if save_path.endswith('.json'):
                    json.dump(self.config, f, indent=2, ensure_ascii=False)
                elif save_path.endswith('.yaml') or save_path.endswith('.yml'):
                    yaml.dump(self.config, f, default_flow_style=False, allow_unicode=True)
            
            self.logger.info(f"配置已保存到: {save_path}")
            
        except Exception as e:
            self.logger.error(f"保存配置失败: {e}")


def validate_file_path(file_path: str, allowed_extensions: Optional[list] = None) -> bool:
    """
    验证文件路径安全性
    
    Args:
        file_path: 文件路径
        allowed_extensions: 允许的文件扩展名列表
        
    Returns:
        是否安全
    """
    if not file_path:
        return False
    
    # 检查路径遍历
    if '..' in file_path or file_path.startswith('/') or ':\\' in file_path:
        return False
    
    # 规范化路径
    try:
        normalized = os.path.normpath(file_path)
        if normalized != file_path:
            return False
    except Exception:
        return False
    
    # 检查文件扩展名
    if allowed_extensions:
        ext = os.path.splitext(file_path)[1].lower()
        if ext not in allowed_extensions:
            return False
    
    return True


def get_config_manager(config_path: Optional[str] = None) -> ConfigManager:
    """获取配置管理器单例"""
    # 简单的单例实现
    if not hasattr(get_config_manager, '_instance'):
        get_config_manager._instance = ConfigManager(config_path)
    return get_config_manager._instance


# 示例使用
if __name__ == "__main__":
    # 设置日志
    logging.basicConfig(level=logging.INFO)
    
    print("配置管理模块示例")
    print("=" * 60)
    
    # 创建配置管理器
    config_manager = ConfigManager()
    
    # 获取配置
    db_config = config_manager.get_database_config()
    migration_config = config_manager.get_migration_config()
    
    print(f"数据库配置: {db_config}")
    print(f"迁移配置: {migration_config}")
    
    # 测试路径验证
    test_paths = [
        "正常文件.aprx",
        "../恶意路径.aprx",
        "C:\\Windows\\system32",
        "/etc/passwd"
    ]
    
    print("\n路径验证测试:")
    for path in test_paths:
        is_valid = validate_file_path(path, ['.aprx', '.xml', '.gpkg'])
        print(f"  {path}: {'有效' if is_valid else '无效'}")
    
    print("\n配置管理模块实现完成！")