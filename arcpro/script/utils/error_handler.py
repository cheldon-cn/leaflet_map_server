"""
统一错误处理接口
提供标准化的错误处理函数和返回格式
"""

from typing import Dict, Any, Optional, List, Union
from dataclasses import dataclass
from datetime import datetime
import traceback


@dataclass
class StandardResult:
    """标准化结果格式"""
    success: bool
    message: str
    data: Optional[Dict[str, Any]] = None
    errors: List[str] = None
    warnings: List[str] = None
    timestamp: datetime = None
    execution_time_ms: Optional[int] = None
    
    def __post_init__(self):
        if self.errors is None:
            self.errors = []
        if self.warnings is None:
            self.warnings = []
        if self.timestamp is None:
            self.timestamp = datetime.now()
    
    def to_dict(self) -> Dict[str, Any]:
        """转换为字典格式"""
        return {
            "success": self.success,
            "message": self.message,
            "data": self.data or {},
            "errors": self.errors,
            "warnings": self.warnings,
            "timestamp": self.timestamp.isoformat(),
            "execution_time_ms": self.execution_time_ms
        }
    
    @classmethod
    def success_result(cls, message: str = "操作成功", data: Optional[Dict[str, Any]] = None,
                      warnings: List[str] = None) -> "StandardResult":
        """创建成功结果"""
        return cls(
            success=True,
            message=message,
            data=data,
            warnings=warnings or []
        )
    
    @classmethod
    def error_result(cls, message: str = "操作失败", errors: List[str] = None,
                    data: Optional[Dict[str, Any]] = None, warnings: List[str] = None) -> "StandardResult":
        """创建错误结果"""
        if errors is None:
            errors = [message]
        else:
            errors = errors.copy()
            if message not in errors:
                errors.insert(0, message)
        
        return cls(
            success=False,
            message=message,
            data=data,
            errors=errors,
            warnings=warnings or []
        )
    
    @classmethod
    def from_exception(cls, exception: Exception, message: str = None,
                      data: Optional[Dict[str, Any]] = None) -> "StandardResult":
        """从异常创建结果"""
        error_msg = str(exception)
        if message:
            error_msg = f"{message}: {error_msg}"
        
        return cls.error_result(
            message=error_msg,
            errors=[error_msg, traceback.format_exc()],
            data=data
        )


def handle_exception(exception: Exception, context: str = "", 
                    data: Optional[Dict[str, Any]] = None) -> StandardResult:
    """
    统一异常处理
    
    Args:
        exception: 捕获的异常
        context: 上下文信息，描述异常发生的位置
        data: 相关数据
        
    Returns:
        StandardResult对象
    """
    message = f"{context}: {str(exception)}" if context else str(exception)
    return StandardResult.from_exception(exception, message, data)


def validate_required_fields(data: Dict[str, Any], required_fields: List[str], 
                           context: str = "") -> Optional[StandardResult]:
    """
    验证必需字段
    
    Args:
        data: 待验证的数据字典
        required_fields: 必需字段列表
        context: 上下文信息
        
    Returns:
        如果验证失败返回StandardResult，否则返回None
    """
    missing_fields = []
    for field in required_fields:
        if field not in data or data[field] is None:
            missing_fields.append(field)
    
    if missing_fields:
        message = f"缺少必需字段: {', '.join(missing_fields)}"
        if context:
            message = f"{context}: {message}"
        return StandardResult.error_result(message=message, data={"missing_fields": missing_fields})
    
    return None


def wrap_function(func):
    """
    装饰器：将函数包装为返回StandardResult
    
    Usage:
        @wrap_function
        def my_function(arg1, arg2):
            # 正常返回数据
            return {"result": "data"}
            
        # 调用函数
        result = my_function(arg1, arg2)
        if result.success:
            data = result.data
    """
    def wrapper(*args, **kwargs):
        try:
            start_time = datetime.now()
            func_result = func(*args, **kwargs)
            end_time = datetime.now()
            execution_time_ms = int((end_time - start_time).total_seconds() * 1000)
            
            # 如果函数已经返回StandardResult，直接返回
            if isinstance(func_result, StandardResult):
                func_result.execution_time_ms = execution_time_ms
                return func_result
            
            # 否则包装为成功结果
            return StandardResult.success_result(
                message="函数执行成功",
                data={"result": func_result} if func_result is not None else {},
                execution_time_ms=execution_time_ms
            )
        except Exception as e:
            return handle_exception(e, f"函数 {func.__name__} 执行失败")
    
    return wrapper


# 兼容性函数，用于逐步迁移现有代码
def create_success_result(data: Optional[Dict[str, Any]] = None, message: str = "成功") -> Dict[str, Any]:
    """创建成功结果字典（兼容旧格式）"""
    return {
        "success": True,
        "message": message,
        "data": data or {},
        "errors": [],
        "warnings": []
    }


def create_error_result(error_message: str, errors: List[str] = None, 
                       data: Optional[Dict[str, Any]] = None) -> Dict[str, Any]:
    """创建错误结果字典（兼容旧格式）"""
    if errors is None:
        errors = [error_message]
    
    return {
        "success": False,
        "message": error_message,
        "data": data or {},
        "errors": errors,
        "warnings": []
    }