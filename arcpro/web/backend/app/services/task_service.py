"""
迁移任务服务
"""
from typing import Optional, List, Dict, Any
from sqlalchemy.ext.asyncio import AsyncSession
from sqlalchemy import select, desc
from datetime import datetime
from app.models.task import MigrationTask, TaskStep
from app.schemas.task import TaskCreate, TaskUpdate, TaskStepCreate
from app.core.exceptions import NotFoundError, ConflictError


class TaskService:
    """迁移任务服务"""
    
    def __init__(self, db: AsyncSession):
        self.db = db
    
    async def get_task(self, task_id: str) -> Optional[MigrationTask]:
        """
        根据ID获取任务
        """
        result = await self.db.execute(
            select(MigrationTask).where(MigrationTask.id == task_id)
        )
        return result.scalar_one_or_none()
    
    async def get_user_tasks(
        self, 
        user_id: str, 
        skip: int = 0, 
        limit: int = 100
    ) -> List[MigrationTask]:
        """
        获取用户的任务列表
        """
        result = await self.db.execute(
            select(MigrationTask)
            .where(MigrationTask.user_id == user_id)
            .order_by(desc(MigrationTask.created_at))
            .offset(skip)
            .limit(limit)
        )
        return result.scalars().all()
    
    async def get_project_tasks(
        self, 
        project_id: str, 
        skip: int = 0, 
        limit: int = 100
    ) -> List[MigrationTask]:
        """
        获取工程的任务列表
        """
        result = await self.db.execute(
            select(MigrationTask)
            .where(MigrationTask.project_id == project_id)
            .order_by(desc(MigrationTask.created_at))
            .offset(skip)
            .limit(limit)
        )
        return result.scalars().all()
    
    async def create_task(self, user_id: str, task_data: TaskCreate) -> MigrationTask:
        """
        创建迁移任务
        """
        task = MigrationTask(
            project_id=task_data.project_id,
            user_id=user_id,
            name=task_data.name,
            description=task_data.description,
            config=task_data.config,
            status="pending"
        )
        
        self.db.add(task)
        await self.db.commit()
        await self.db.refresh(task)
        
        return task
    
    async def update_task(self, task_id: str, task_data: TaskUpdate) -> MigrationTask:
        """
        更新任务
        """
        task = await self.get_task(task_id)
        if not task:
            raise NotFoundError(detail="任务未找到")
        
        # 更新字段
        update_data = task_data.dict(exclude_unset=True)
        for field, value in update_data.items():
            setattr(task, field, value)
        
        await self.db.commit()
        await self.db.refresh(task)
        
        return task
    
    async def delete_task(self, task_id: str) -> bool:
        """
        删除任务
        """
        task = await self.get_task(task_id)
        if not task:
            raise NotFoundError(detail="任务未找到")
        
        await self.db.delete(task)
        await self.db.commit()
        
        return True
    
    async def start_task(self, task_id: str) -> MigrationTask:
        """
        开始执行任务
        """
        task = await self.get_task(task_id)
        if not task:
            raise NotFoundError(detail="任务未找到")
        
        if task.status not in ["pending", "failed"]:
            raise ConflictError(detail="任务状态不允许开始")
        
        task.status = "running"
        task.started_at = datetime.utcnow()
        
        # TODO: 这里应该触发异步任务执行
        # 调用现有迁移工具
        
        await self.db.commit()
        await self.db.refresh(task)
        
        return task
    
    async def cancel_task(self, task_id: str) -> MigrationTask:
        """
        取消任务
        """
        task = await self.get_task(task_id)
        if not task:
            raise NotFoundError(detail="任务未找到")
        
        if task.status not in ["pending", "running"]:
            raise ConflictError(detail="任务状态不允许取消")
        
        task.status = "cancelled"
        task.completed_at = datetime.utcnow()
        
        await self.db.commit()
        await self.db.refresh(task)
        
        return task
    
    async def update_task_progress(
        self, 
        task_id: str, 
        progress: float, 
        step_name: Optional[str] = None,
        details: Optional[Dict[str, Any]] = None
    ) -> MigrationTask:
        """
        更新任务进度
        """
        task = await self.get_task(task_id)
        if not task:
            raise NotFoundError(detail="任务未找到")
        
        task.progress = progress
        
        if progress >= 1.0:
            task.status = "completed"
            task.completed_at = datetime.utcnow()
        
        # 记录步骤
        if step_name:
            step = TaskStep(
                task_id=task_id,
                step_name=step_name,
                step_order=len(task.steps) + 1,
                progress=progress,
                details=details,
                started_at=datetime.utcnow()
            )
            
            if progress >= 1.0:
                step.status = "completed"
                step.completed_at = datetime.utcnow()
            else:
                step.status = "running"
            
            self.db.add(step)
        
        await self.db.commit()
        await self.db.refresh(task)
        
        return task