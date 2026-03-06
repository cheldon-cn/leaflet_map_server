"""
通用模式
"""
from typing import TypeVar, Generic, List, Optional
from pydantic import BaseModel
from datetime import datetime

T = TypeVar('T')


class PaginatedResponse(BaseModel, Generic[T]):
    """分页响应"""
    items: List[T]
    total: int
    page: int
    size: int
    pages: int


class ErrorResponse(BaseModel):
    """错误响应"""
    error: str
    message: str
    detail: Optional[str] = None
    timestamp: datetime


class MessageResponse(BaseModel):
    """消息响应"""
    message: str


class StatusResponse(BaseModel):
    """状态响应"""
    status: str
    message: Optional[str] = None