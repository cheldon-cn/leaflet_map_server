"""
迁移任务路由
"""
from typing import List
from fastapi import APIRouter, Depends, HTTPException, status
from sqlalchemy.ext.asyncio import AsyncSession
from sqlalchemy import select, func

from app.core.database import get_db
from app.models.task import MigrationTask
from app.schemas.task import TaskCreate, TaskResponse, TaskUpdate, TaskProgressResponse, TaskLogResponse
from app.schemas.common import PaginatedResponse
from app.services.task_service import TaskService

router = APIRouter()


@router.get("/", response_model=PaginatedResponse[TaskResponse])
async def list_tasks(
    skip: int = 0,
    limit: int = 100,
    db: AsyncSession = Depends(get_db),
    current_user_id: str = "demo_user_id"  # 临时演示
):
    """
    获取任务列表
    """
    service = TaskService(db)
    tasks = await service.get_user_tasks(current_user_id, skip, limit)
    
    # 获取总数
    result = await db.execute(
        select(func.count()).select_from(MigrationTask).where(MigrationTask.user_id == current_user_id)
    )
    total = result.scalar()
    
    return PaginatedResponse(
        items=tasks,
        total=total,
        page=skip // limit + 1,
        size=limit,
        pages=(total + limit - 1) // limit
    )


@router.post("/", response_model=TaskResponse, status_code=status.HTTP_201_CREATED)
async def create_task(
    task_data: TaskCreate,
    db: AsyncSession = Depends(get_db),
    current_user_id: str = "demo_user_id"  # 临时演示
):
    """
    创建迁移任务
    """
    service = TaskService(db)
    task = await service.create_task(current_user_id, task_data)
    return task


@router.get("/{task_id}", response_model=TaskResponse)
async def get_task(
    task_id: str,
    db: AsyncSession = Depends(get_db)
):
    """
    获取任务详情
    """
    service = TaskService(db)
    task = await service.get_task(task_id)
    if not task:
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail="任务未找到"
        )
    
    return task


@router.put("/{task_id}", response_model=TaskResponse)
async def update_task(
    task_id: str,
    task_data: TaskUpdate,
    db: AsyncSession = Depends(get_db)
):
    """
    更新任务
    """
    service = TaskService(db)
    task = await service.update_task(task_id, task_data)
    return task


@router.delete("/{task_id}", status_code=status.HTTP_204_NO_CONTENT)
async def delete_task(
    task_id: str,
    db: AsyncSession = Depends(get_db)
):
    """
    删除任务
    """
    service = TaskService(db)
    success = await service.delete_task(task_id)
    if not success:
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail="任务未找到"
        )


@router.post("/{task_id}/start", response_model=TaskResponse)
async def start_task(
    task_id: str,
    db: AsyncSession = Depends(get_db)
):
    """
    开始执行任务
    """
    service = TaskService(db)
    task = await service.start_task(task_id)
    return task


@router.post("/{task_id}/cancel", response_model=TaskResponse)
async def cancel_task(
    task_id: str,
    db: AsyncSession = Depends(get_db)
):
    """
    取消任务
    """
    service = TaskService(db)
    task = await service.cancel_task(task_id)
    return task


@router.get("/{task_id}/progress", response_model=TaskProgressResponse)
async def get_task_progress(
    task_id: str,
    db: AsyncSession = Depends(get_db)
):
    """
    获取任务进度
    """
    service = TaskService(db)
    task = await service.get_task(task_id)
    if not task:
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail="任务未找到"
        )
    
    # 获取步骤
    from app.models.task import TaskStep
    result = await db.execute(
        select(TaskStep)
        .where(TaskStep.task_id == task_id)
        .order_by(TaskStep.step_order)
    )
    steps = result.scalars().all()
    
    return TaskProgressResponse(
        task_id=task_id,
        status=task.status,
        progress=task.progress,
        current_step=steps[-1].step_name if steps else None,
        steps=steps
    )


@router.get("/{task_id}/logs", response_model=TaskLogResponse)
async def get_task_logs(
    task_id: str,
    db: AsyncSession = Depends(get_db)
):
    """
    获取任务日志
    """
    service = TaskService(db)
    task = await service.get_task(task_id)
    if not task:
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail="任务未找到"
        )
    
    return TaskLogResponse(
        task_id=task_id,
        logs=task.logs or "",
        last_updated=task.updated_at
    )