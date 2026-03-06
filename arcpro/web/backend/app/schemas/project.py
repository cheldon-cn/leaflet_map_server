"""
工程相关模式
"""
from typing import Optional, Dict, Any
from pydantic import BaseModel
from datetime import datetime


class ProjectBase(BaseModel):
    """工程基础模式"""
    name: str
    description: Optional[str] = None


class ProjectCreate(ProjectBase):
    """工程创建模式"""
    original_filename: Optional[str] = None
    file_size: Optional[int] = None


class ProjectUpdate(BaseModel):
    """工程更新模式"""
    name: Optional[str] = None
    description: Optional[str] = None
    status: Optional[str] = None
    metadata: Optional[Dict[str, Any]] = None


class ProjectInDB(ProjectBase):
    """数据库中的工程模式"""
    id: str
    user_id: str
    original_filename: Optional[str]
    file_path: Optional[str]
    file_size: Optional[int]
    status: str
    metadata: Optional[Dict[str, Any]]
    created_at: datetime
    updated_at: datetime
    
    class Config:
        from_attributes = True


class ProjectResponse(ProjectBase):
    """工程响应模式"""
    id: str
    user_id: str
    original_filename: Optional[str]
    file_size: Optional[int]
    status: str
    metadata: Optional[Dict[str, Any]]
    created_at: datetime
    updated_at: datetime
    
    class Config:
        from_attributes = True


class ProjectParseRequest(BaseModel):
    """工程解析请求"""
    force: bool = False


class ProjectParseResponse(BaseModel):
    """工程解析响应"""
    success: bool
    message: str
    metadata: Optional[Dict[str, Any]] = None