"""
依赖注入框架
提供简单的依赖注入容器和服务注册功能
"""

import inspect
from typing import Dict, Any, Type, Optional, Callable, List
from dataclasses import dataclass, field
import logging


@dataclass
class ServiceDescriptor:
    """服务描述符"""
    service_type: Type
    implementation: Any
    is_singleton: bool = True
    instance: Optional[Any] = None
    dependencies: List[str] = field(default_factory=list)


class DependencyContainer:
    """依赖注入容器"""
    
    def __init__(self):
        self._services: Dict[str, ServiceDescriptor] = {}
        self._logger = logging.getLogger(__name__)
    
    def register_singleton(self, service_type: Type, implementation: Optional[Any] = None) -> None:
        """
        注册单例服务
        
        Args:
            service_type: 服务类型（接口或抽象类）
            implementation: 实现类，如果为None则使用service_type自身
        """
        if implementation is None:
            implementation = service_type
        
        service_name = service_type.__name__
        self._services[service_name] = ServiceDescriptor(
            service_type=service_type,
            implementation=implementation,
            is_singleton=True
        )
        self._logger.debug(f"注册单例服务: {service_name}")
    
    def register_transient(self, service_type: Type, implementation: Optional[Any] = None) -> None:
        """
        注册瞬时服务（每次请求创建新实例）
        
        Args:
            service_type: 服务类型（接口或抽象类）
            implementation: 实现类，如果为None则使用service_type自身
        """
        if implementation is None:
            implementation = service_type
        
        service_name = service_type.__name__
        self._services[service_name] = ServiceDescriptor(
            service_type=service_type,
            implementation=implementation,
            is_singleton=False
        )
        self._logger.debug(f"注册瞬时服务: {service_name}")
    
    def register_instance(self, service_type: Type, instance: Any) -> None:
        """
        注册已存在的实例
        
        Args:
            service_type: 服务类型
            instance: 已存在的实例
        """
        service_name = service_type.__name__
        self._services[service_name] = ServiceDescriptor(
            service_type=service_type,
            implementation=type(instance),
            is_singleton=True,
            instance=instance
        )
        self._logger.debug(f"注册实例服务: {service_name}")
    
    def get_service(self, service_type: Type) -> Any:
        """
        获取服务实例
        
        Args:
            service_type: 服务类型
            
        Returns:
            服务实例
            
        Raises:
            KeyError: 如果服务未注册
        """
        service_name = service_type.__name__
        
        if service_name not in self._services:
            raise KeyError(f"服务未注册: {service_name}")
        
        descriptor = self._services[service_name]
        
        # 如果已有实例且是单例，直接返回
        if descriptor.instance is not None and descriptor.is_singleton:
            return descriptor.instance
        
        # 创建新实例
        implementation = descriptor.implementation
        
        # 解析构造函数参数
        try:
            signature = inspect.signature(implementation.__init__)
            parameters = signature.parameters
            
            # 收集依赖参数
            dependencies = {}
            for param_name, param in parameters.items():
                if param_name == 'self':
                    continue
                
                # 尝试从容器中解析参数类型
                param_type = param.annotation
                if param_type != inspect.Parameter.empty:
                    try:
                        dependencies[param_name] = self.get_service(param_type)
                    except KeyError:
                        # 如果依赖未注册，尝试使用默认值
                        if param.default != inspect.Parameter.empty:
                            dependencies[param_name] = param.default
                        else:
                            self._logger.warning(f"无法解析依赖: {param_name}: {param_type}")
                            dependencies[param_name] = None
                elif param.default != inspect.Parameter.empty:
                    dependencies[param_name] = param.default
            
            # 创建实例
            instance = implementation(**dependencies)
            
        except Exception as e:
            self._logger.error(f"创建服务实例失败 {service_name}: {e}")
            # 如果依赖注入失败，尝试无参数构造函数
            try:
                instance = implementation()
            except Exception as e2:
                raise RuntimeError(f"无法创建服务实例 {service_name}: {e2}")
        
        # 如果是单例，保存实例
        if descriptor.is_singleton:
            descriptor.instance = instance
        
        return instance
    
    def resolve_dependencies(self, obj: Any) -> None:
        """
        解析对象的依赖并注入
        
        Args:
            obj: 需要注入依赖的对象
        """
        obj_type = type(obj)
        
        # 检查对象是否有需要注入的属性
        if hasattr(obj, '__di_inject__'):
            inject_config = getattr(obj, '__di_inject__')
            for attr_name, service_type in inject_config.items():
                try:
                    service_instance = self.get_service(service_type)
                    setattr(obj, attr_name, service_instance)
                except KeyError:
                    self._logger.warning(f"无法注入依赖 {attr_name} 到 {obj_type.__name__}")
    
    def create_with_dependencies(self, cls: Type) -> Any:
        """
        创建类实例并自动注入依赖
        
        Args:
            cls: 要创建的类
            
        Returns:
            创建的实例
        """
        try:
            # 首先尝试使用容器解析构造函数参数
            signature = inspect.signature(cls.__init__)
            parameters = signature.parameters
            
            dependencies = {}
            for param_name, param in parameters.items():
                if param_name == 'self':
                    continue
                
                param_type = param.annotation
                if param_type != inspect.Parameter.empty:
                    try:
                        dependencies[param_name] = self.get_service(param_type)
                    except KeyError:
                        if param.default != inspect.Parameter.empty:
                            dependencies[param_name] = param.default
                        else:
                            dependencies[param_name] = None
                elif param.default != inspect.Parameter.empty:
                    dependencies[param_name] = param.default
            
            instance = cls(**dependencies)
            
        except Exception as e:
            self._logger.error(f"依赖注入创建实例失败 {cls.__name__}: {e}")
            # 回退到无参数构造函数
            try:
                instance = cls()
            except Exception as e2:
                raise RuntimeError(f"无法创建实例 {cls.__name__}: {e2}")
        
        # 解析额外的依赖注入
        self.resolve_dependencies(instance)
        
        return instance


# 全局容器实例
_container: Optional[DependencyContainer] = None


def get_container() -> DependencyContainer:
    """获取全局容器实例"""
    global _container
    if _container is None:
        _container = DependencyContainer()
    return _container


def register_singleton(service_type: Type, implementation: Optional[Any] = None) -> None:
    """注册单例服务（全局容器）"""
    get_container().register_singleton(service_type, implementation)


def register_transient(service_type: Type, implementation: Optional[Any] = None) -> None:
    """注册瞬时服务（全局容器）"""
    get_container().register_transient(service_type, implementation)


def register_instance(service_type: Type, instance: Any) -> None:
    """注册实例（全局容器）"""
    get_container().register_instance(service_type, instance)


def get_service(service_type: Type) -> Any:
    """获取服务实例（全局容器）"""
    return get_container().get_service(service_type)


def create_with_dependencies(cls: Type) -> Any:
    """创建类实例并自动注入依赖（全局容器）"""
    return get_container().create_with_dependencies(cls)


def inject(*args, **kwargs):
    """
    依赖注入装饰器
    
    用法:
    @inject
    class MyService:
        def __init__(self, config_service: ConfigService):
            self.config_service = config_service
    
    或:
    class MyService:
        __di_inject__ = {'config_service': ConfigService}
        
        def __init__(self):
            pass  # 依赖将在构造后注入
    """
    if len(args) == 1 and len(kwargs) == 0 and callable(args[0]):
        # 用作无参数装饰器
        cls = args[0]
        return _inject_class(cls)
    else:
        # 用作带参数装饰器
        def decorator(cls):
            return _inject_class(cls, **kwargs)
        return decorator


def _inject_class(cls, **dependencies):
    """内部函数：注入类依赖"""
    original_init = cls.__init__
    
    def new_init(self, *args, **kwargs):
        # 首先调用原始构造函数
        original_init(self, *args, **kwargs)
        
        # 然后注入依赖
        container = get_container()
        for attr_name, service_type in dependencies.items():
            try:
                service_instance = container.get_service(service_type)
                setattr(self, attr_name, service_instance)
            except KeyError:
                logging.getLogger(__name__).warning(
                    f"无法注入依赖 {attr_name} 到 {cls.__name__}"
                )
    
    cls.__init__ = new_init
    return cls


# 示例服务接口（用于类型提示）
class ConfigService:
    """配置服务接口"""
    pass


class DatabaseService:
    """数据库服务接口"""
    pass


class MigrationService:
    """迁移服务接口"""
    pass