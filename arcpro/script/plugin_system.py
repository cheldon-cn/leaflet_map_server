"""
插件系统架构
提供可扩展的插件框架，支持动态加载和卸载功能模块
"""

import os
import sys
import importlib
import inspect
import logging
from typing import Dict, Any, List, Optional, Type, Callable, Set
from dataclasses import dataclass, field
from enum import Enum
from pathlib import Path
import pkgutil


class PluginType(Enum):
    """插件类型"""
    DATA_CONVERTER = "data_converter"  # 数据转换插件
    STYLE_CONVERTER = "style_converter"  # 样式转换插件
    VALIDATOR = "validator"  # 验证插件
    OPTIMIZER = "optimizer"  # 优化插件
    EXPORTER = "exporter"  # 导出插件
    IMPORTER = "importer"  # 导入插件
    CUSTOM = "custom"  # 自定义插件


class PluginStatus(Enum):
    """插件状态"""
    REGISTERED = "registered"  # 已注册
    LOADED = "loaded"  # 已加载
    ACTIVE = "active"  # 活动状态
    DISABLED = "disabled"  # 已禁用
    ERROR = "error"  # 错误状态


@dataclass
class PluginInfo:
    """插件信息"""
    plugin_id: str
    name: str
    version: str
    plugin_type: PluginType
    description: str = ""
    author: str = ""
    dependencies: List[str] = field(default_factory=list)
    settings: Dict[str, Any] = field(default_factory=dict)
    status: PluginStatus = PluginStatus.REGISTERED
    error_message: Optional[str] = None
    load_time: Optional[float] = None
    
    def to_dict(self) -> Dict[str, Any]:
        """转换为字典"""
        return {
            "plugin_id": self.plugin_id,
            "name": self.name,
            "version": self.version,
            "plugin_type": self.plugin_type.value,
            "description": self.description,
            "author": self.author,
            "dependencies": self.dependencies,
            "status": self.status.value,
            "error_message": self.error_message,
            "load_time": self.load_time
        }


@dataclass
class PluginContext:
    """插件上下文"""
    config: Dict[str, Any]
    logger: logging.Logger
    data_dir: str
    temp_dir: str
    shared_data: Dict[str, Any] = field(default_factory=dict)
    
    def get_setting(self, key: str, default: Any = None) -> Any:
        """获取设置"""
        return self.config.get(key, default)
    
    def set_shared_data(self, key: str, value: Any) -> None:
        """设置共享数据"""
        self.shared_data[key] = value
    
    def get_shared_data(self, key: str, default: Any = None) -> Any:
        """获取共享数据"""
        return self.shared_data.get(key, default)


class BasePlugin:
    """插件基类"""
    
    # 插件元数据（子类应覆盖这些属性）
    PLUGIN_ID: str = ""
    PLUGIN_NAME: str = ""
    PLUGIN_VERSION: str = "1.0.0"
    PLUGIN_TYPE: PluginType = PluginType.CUSTOM
    PLUGIN_DESCRIPTION: str = ""
    PLUGIN_AUTHOR: str = ""
    PLUGIN_DEPENDENCIES: List[str] = field(default_factory=list)
    
    def __init__(self, context: PluginContext):
        """初始化插件"""
        self.context = context
        self.logger = context.logger.getChild(self.PLUGIN_ID)
        self.settings = context.config.get("plugins", {}).get(self.PLUGIN_ID, {})
        self._initialized = False
    
    def initialize(self) -> bool:
        """初始化插件（子类可以覆盖）"""
        self._initialized = True
        self.logger.info(f"插件 {self.PLUGIN_NAME} 初始化完成")
        return True
    
    def cleanup(self) -> None:
        """清理插件资源（子类可以覆盖）"""
        self.logger.info(f"插件 {self.PLUGIN_NAME} 清理完成")
    
    def get_info(self) -> PluginInfo:
        """获取插件信息"""
        return PluginInfo(
            plugin_id=self.PLUGIN_ID,
            name=self.PLUGIN_NAME,
            version=self.PLUGIN_VERSION,
            plugin_type=self.PLUGIN_TYPE,
            description=self.PLUGIN_DESCRIPTION,
            author=self.PLUGIN_AUTHOR,
            dependencies=self.PLUGIN_DEPENDENCIES,
            settings=self.settings
        )
    
    def validate_dependencies(self) -> List[str]:
        """验证依赖（返回缺失的依赖列表）"""
        missing = []
        for dep in self.PLUGIN_DEPENDENCIES:
            try:
                importlib.import_module(dep)
            except ImportError:
                missing.append(dep)
        return missing


class DataConverterPlugin(BasePlugin):
    """数据转换插件基类"""
    PLUGIN_TYPE = PluginType.DATA_CONVERTER
    
    def convert(self, source_path: str, target_path: str, options: Dict[str, Any] = None) -> Dict[str, Any]:
        """转换数据（子类必须实现）"""
        raise NotImplementedError("子类必须实现convert方法")
    
    def get_supported_formats(self) -> Dict[str, List[str]]:
        """获取支持的格式（子类可以覆盖）"""
        return {
            "input": [],  # 支持的输入格式
            "output": []  # 支持的输出格式
        }


class StyleConverterPlugin(BasePlugin):
    """样式转换插件基类"""
    PLUGIN_TYPE = PluginType.STYLE_CONVERTER
    
    def convert_style(self, source_style: Dict[str, Any], target_type: str, options: Dict[str, Any] = None) -> Dict[str, Any]:
        """转换样式（子类必须实现）"""
        raise NotImplementedError("子类必须实现convert_style方法")
    
    def get_supported_style_types(self) -> List[str]:
        """获取支持的样式类型（子类可以覆盖）"""
        return []


class ValidatorPlugin(BasePlugin):
    """验证插件基类"""
    PLUGIN_TYPE = PluginType.VALIDATOR
    
    def validate(self, data: Any, rules: Dict[str, Any] = None) -> Dict[str, Any]:
        """验证数据（子类必须实现）"""
        raise NotImplementedError("子类必须实现validate方法")
    
    def get_validation_rules(self) -> Dict[str, Any]:
        """获取验证规则（子类可以覆盖）"""
        return {}


class PluginManager:
    """插件管理器"""
    
    def __init__(self, config: Dict[str, Any] = None):
        """初始化插件管理器"""
        self.config = config or {}
        self.logger = logging.getLogger(__name__)
        self.plugins: Dict[str, BasePlugin] = {}
        self.plugin_info: Dict[str, PluginInfo] = {}
        self.plugin_context = PluginContext(
            config=self.config,
            logger=self.logger,
            data_dir=self.config.get("data_dir", "./data"),
            temp_dir=self.config.get("temp_dir", "./temp")
        )
        
        # 插件目录
        self.plugin_dirs = self.config.get("plugin_dirs", ["./plugins"])
        
        # 已加载的模块
        self.loaded_modules: Set[str] = set()
    
    def register_plugin(self, plugin_class: Type[BasePlugin]) -> bool:
        """注册插件类"""
        try:
            # 创建插件实例
            plugin = plugin_class(self.plugin_context)
            plugin_id = plugin.PLUGIN_ID
            
            if not plugin_id:
                self.logger.error("插件ID不能为空")
                return False
            
            if plugin_id in self.plugins:
                self.logger.warning(f"插件 {plugin_id} 已注册，跳过")
                return False
            
            # 验证依赖
            missing_deps = plugin.validate_dependencies()
            if missing_deps:
                self.logger.error(f"插件 {plugin_id} 缺少依赖: {missing_deps}")
                return False
            
            # 存储插件
            self.plugins[plugin_id] = plugin
            self.plugin_info[plugin_id] = plugin.get_info()
            
            self.logger.info(f"插件注册成功: {plugin.PLUGIN_NAME} ({plugin_id})")
            return True
            
        except Exception as e:
            self.logger.error(f"注册插件失败: {e}")
            return False
    
    def load_plugin(self, plugin_id: str) -> bool:
        """加载插件"""
        if plugin_id not in self.plugins:
            self.logger.error(f"插件未注册: {plugin_id}")
            return False
        
        plugin = self.plugins[plugin_id]
        plugin_info = self.plugin_info[plugin_id]
        
        try:
            # 初始化插件
            if plugin.initialize():
                plugin_info.status = PluginStatus.ACTIVE
                plugin_info.load_time = os.times().elapsed
                self.logger.info(f"插件加载成功: {plugin_id}")
                return True
            else:
                plugin_info.status = PluginStatus.ERROR
                plugin_info.error_message = "初始化失败"
                self.logger.error(f"插件初始化失败: {plugin_id}")
                return False
                
        except Exception as e:
            plugin_info.status = PluginStatus.ERROR
            plugin_info.error_message = str(e)
            self.logger.error(f"插件加载失败 {plugin_id}: {e}")
            return False
    
    def unload_plugin(self, plugin_id: str) -> bool:
        """卸载插件"""
        if plugin_id not in self.plugins:
            self.logger.error(f"插件未注册: {plugin_id}")
            return False
        
        plugin = self.plugins[plugin_id]
        plugin_info = self.plugin_info[plugin_id]
        
        try:
            # 清理插件
            plugin.cleanup()
            plugin_info.status = PluginStatus.REGISTERED
            plugin_info.error_message = None
            
            self.logger.info(f"插件卸载成功: {plugin_id}")
            return True
            
        except Exception as e:
            self.logger.error(f"插件卸载失败 {plugin_id}: {e}")
            return False
    
    def discover_plugins(self, directory: str = None) -> List[str]:
        """发现目录中的插件"""
        if directory is None:
            directories = self.plugin_dirs
        else:
            directories = [directory]
        
        discovered = []
        
        for plugin_dir in directories:
            plugin_dir_path = Path(plugin_dir)
            if not plugin_dir_path.exists():
                self.logger.warning(f"插件目录不存在: {plugin_dir}")
                continue
            
            # 查找Python模块
            for finder, name, ispkg in pkgutil.iter_modules([str(plugin_dir_path)]):
                if name.startswith('plugin_'):
                    module_path = f"{plugin_dir_path.name}.{name}"
                    discovered.append((module_path, str(plugin_dir_path)))
            
            # 查找Python文件
            for py_file in plugin_dir_path.glob("plugin_*.py"):
                module_name = py_file.stem
                discovered.append((module_name, str(plugin_dir_path)))
        
        return discovered
    
    def load_plugins_from_directory(self, directory: str = None) -> List[str]:
        """从目录加载所有插件"""
        discovered = self.discover_plugins(directory)
        loaded = []
        
        for module_name, module_path in discovered:
            try:
                # 添加路径到sys.path
                if module_path not in sys.path:
                    sys.path.insert(0, module_path)
                
                # 导入模块
                module = importlib.import_module(module_name)
                
                # 查找插件类
                for name, obj in inspect.getmembers(module):
                    if (inspect.isclass(obj) and 
                        issubclass(obj, BasePlugin) and 
                        obj != BasePlugin):
                        
                        # 注册插件
                        if self.register_plugin(obj):
                            loaded.append(obj.PLUGIN_ID)
                
                self.loaded_modules.add(module_name)
                
            except Exception as e:
                self.logger.error(f"加载插件模块失败 {module_name}: {e}")
        
        return loaded
    
    def get_plugin(self, plugin_id: str) -> Optional[BasePlugin]:
        """获取插件实例"""
        return self.plugins.get(plugin_id)
    
    def get_plugin_info(self, plugin_id: str) -> Optional[PluginInfo]:
        """获取插件信息"""
        return self.plugin_info.get(plugin_id)
    
    def get_all_plugins(self) -> List[PluginInfo]:
        """获取所有插件信息"""
        return list(self.plugin_info.values())
    
    def get_plugins_by_type(self, plugin_type: PluginType) -> List[PluginInfo]:
        """按类型获取插件"""
        return [
            info for info in self.plugin_info.values()
            if info.plugin_type == plugin_type
        ]
    
    def execute_plugin_method(self, plugin_id: str, method_name: str, *args, **kwargs) -> Any:
        """执行插件方法"""
        plugin = self.get_plugin(plugin_id)
        if not plugin:
            raise ValueError(f"插件未找到: {plugin_id}")
        
        if not hasattr(plugin, method_name):
            raise ValueError(f"插件 {plugin_id} 没有方法 {method_name}")
        
        method = getattr(plugin, method_name)
        return method(*args, **kwargs)
    
    def shutdown(self) -> None:
        """关闭插件管理器"""
        for plugin_id in list(self.plugins.keys()):
            self.unload_plugin(plugin_id)
        
        self.logger.info("插件管理器已关闭")


# 示例插件实现
class ExampleDataConverterPlugin(DataConverterPlugin):
    """示例数据转换插件"""
    
    PLUGIN_ID = "example_data_converter"
    PLUGIN_NAME = "示例数据转换器"
    PLUGIN_DESCRIPTION = "用于演示的数据转换插件"
    PLUGIN_AUTHOR = "迁移系统团队"
    PLUGIN_DEPENDENCIES = ["os", "shutil"]
    
    def convert(self, source_path: str, target_path: str, options: Dict[str, Any] = None) -> Dict[str, Any]:
        """转换数据"""
        options = options or {}
        
        self.logger.info(f"转换数据: {source_path} -> {target_path}")
        
        # 模拟转换过程
        result = {
            "success": True,
            "source_path": source_path,
            "target_path": target_path,
            "converted_size": 1024,  # 模拟大小
            "conversion_time": 1.5,  # 模拟时间
            "message": "转换成功"
        }
        
        return result
    
    def get_supported_formats(self) -> Dict[str, List[str]]:
        """获取支持的格式"""
        return {
            "input": [".shp", ".geojson", ".kml"],
            "output": [".gpkg", ".shp", ".geojson"]
        }


class ExampleStyleConverterPlugin(StyleConverterPlugin):
    """示例样式转换插件"""
    
    PLUGIN_ID = "example_style_converter"
    PLUGIN_NAME = "示例样式转换器"
    PLUGIN_DESCRIPTION = "用于演示的样式转换插件"
    PLUGIN_AUTHOR = "迁移系统团队"
    
    def convert_style(self, source_style: Dict[str, Any], target_type: str, options: Dict[str, Any] = None) -> Dict[str, Any]:
        """转换样式"""
        options = options or {}
        
        self.logger.info(f"转换样式到: {target_type}")
        
        # 模拟转换过程
        result = {
            "success": True,
            "original_style": source_style,
            "converted_style": {
                "type": target_type,
                "properties": {
                    "color": source_style.get("color", "#000000"),
                    "size": source_style.get("size", 1),
                    "opacity": source_style.get("opacity", 1.0)
                }
            },
            "message": "样式转换成功"
        }
        
        return result
    
    def get_supported_style_types(self) -> List[str]:
        """获取支持的样式类型"""
        return ["qml", "sld", "lyr"]


# 插件管理器单例
_plugin_manager: Optional[PluginManager] = None


def get_plugin_manager(config: Dict[str, Any] = None) -> PluginManager:
    """获取插件管理器单例"""
    global _plugin_manager
    if _plugin_manager is None:
        _plugin_manager = PluginManager(config)
    return _plugin_manager


def register_plugin(plugin_class: Type[BasePlugin]) -> bool:
    """注册插件（使用全局管理器）"""
    return get_plugin_manager().register_plugin(plugin_class)


def load_plugin(plugin_id: str) -> bool:
    """加载插件（使用全局管理器）"""
    return get_plugin_manager().load_plugin(plugin_id)


def get_plugin(plugin_id: str) -> Optional[BasePlugin]:
    """获取插件（使用全局管理器）"""
    return get_plugin_manager().get_plugin(plugin_id)


def get_all_plugins() -> List[PluginInfo]:
    """获取所有插件（使用全局管理器）"""
    return get_plugin_manager().get_all_plugins()