"""
迁移任务相关模式
"""
from typing import Optional, Dict, Any, List
from pydantic import BaseModel
from datetime import datetime


class TaskBase(BaseModel):
    """任务基础模式"""
    name: str
    description: Optional[str] = None
    config: Dict[str, Any]


class TaskCreate(TaskBase):
    """任务创建模式"""
    project_id: str


class TaskUpdate(BaseModel):
    """任务更新模式"""
    name: Optional[str] = None
    description: Optional[str] = None
    config: Optional[Dict[str, Any]] = None
    status: Optional[str] = None
    progress: Optional[float] = None
    error_message: Optional[str] = None
    result_path: Optional[str] = None
    logs: Optional[str] = None


class TaskInDB(TaskBase):
    """数据库中的任务模式"""
    id: str
    project_id: str
    user_id: Optional[str]
    status: str
    progress: float
    error_message: Optional[str]
    result_path: Optional[str]
    logs: Optional[str]
    started_at: Optional[datetime]
    completed_at: Optional[datetime]
    created_at: datetime
    updated_at: datetime
    
    class Config:
        from_attributes = True


class TaskResponse(TaskBase):
    """任务响应模式"""
    id: str
    project_id: str
    user_id: Optional[str]
    status: str
    progress: float
    error_message: Optional[str]
    result_path: Optional[str]
    logs: Optional[str]
    started_at: Optional[datetime]
    completed_at: Optional[datetime]
    created_at: datetime
    updated_at: datetime
    
    class Config:
        from_attributes = True


class TaskStepCreate(BaseModel):
    """任务步骤创建模式"""
    step_name: str
    step_order: int
    details: Optional[Dict[str, Any]] = None


class TaskStepResponse(BaseModel):
    """任务步骤响应模式"""
    id: str
    task_id: str
    step_name: str
    step_order: int
    status: str
    progress: float
    details: Optional[Dict[str, Any]]
    started_at: Optional[datetime]
    completed_at: Optional[datetime]
    created_at: datetime
    
    class Config:
        from_attributes = True


class TaskStartRequest(BaseModel):
    """任务开始请求"""
    force: bool = False


class TaskProgressResponse(BaseModel):
    """任务进度响应"""
    task_id: str
    status: str
    progress: float
    current_step: Optional[str] = None
    steps: List[TaskStepResponse] = []
    estimated_time_remaining: Optional[int] = None  # 秒


class TaskLogResponse(BaseModel):
    """任务日志响应"""
    task_id: str
    logs: str
    last_updated: datetime