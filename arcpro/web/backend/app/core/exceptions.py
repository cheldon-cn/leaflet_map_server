"""
自定义异常类
"""
from typing import Optional, Dict, Any
from fastapi import HTTPException, status
from datetime import datetime


class AppException(HTTPException):
    """应用基础异常"""
    
    def __init__(
        self,
        status_code: int = status.HTTP_400_BAD_REQUEST,
        detail: str = "应用错误",
        headers: Optional[Dict[str, Any]] = None
    ):
        super().__init__(status_code=status_code, detail=detail, headers=headers)


class AuthenticationError(AppException):
    """认证错误"""
    
    def __init__(self, detail: str = "认证失败"):
        super().__init__(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail=detail,
            headers={"WWW-Authenticate": "Bearer"}
        )


class AuthorizationError(AppException):
    """授权错误"""
    
    def __init__(self, detail: str = "权限不足"):
        super().__init__(
            status_code=status.HTTP_403_FORBIDDEN,
            detail=detail
        )


class NotFoundError(AppException):
    """资源未找到"""
    
    def __init__(self, detail: str = "资源未找到"):
        super().__init__(
            status_code=status.HTTP_404_NOT_FOUND,
            detail=detail
        )


class ValidationError(AppException):
    """验证错误"""
    
    def __init__(self, detail: str = "验证失败"):
        super().__init__(
            status_code=status.HTTP_422_UNPROCESSABLE_ENTITY,
            detail=detail
        )


class ConflictError(AppException):
    """资源冲突"""
    
    def __init__(self, detail: str = "资源已存在"):
        super().__init__(
            status_code=status.HTTP_409_CONFLICT,
            detail=detail
        )


class ServiceError(AppException):
    """服务错误"""
    
    def __init__(self, detail: str = "服务内部错误"):
        super().__init__(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=detail
        )


def create_error_response(
    error_type: str,
    message: str,
    detail: Optional[str] = None,
    status_code: int = status.HTTP_400_BAD_REQUEST
) -> Dict[str, Any]:
    """
    创建标准化错误响应
    """
    return {
        "error": error_type,
        "message": message,
        "detail": detail,
        "timestamp": datetime.utcnow().isoformat(),
        "status_code": status_code
    }