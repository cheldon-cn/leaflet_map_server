"""
系统监控路由
"""
from datetime import datetime
import psutil
from fastapi import APIRouter, Depends
from sqlalchemy.ext.asyncio import AsyncSession
from sqlalchemy import select, func

from app.core.database import get_db
from app.models.task import MigrationTask

router = APIRouter()


@router.get("/health")
async def health_check():
    """
    健康检查
    """
    return {
        "status": "healthy",
        "timestamp": datetime.utcnow().isoformat(),
        "service": "ArcGIS Pro迁移工具",
        "version": "1.0.0"
    }


@router.get("/stats")
async def system_stats():
    """
    系统统计
    """
    # 系统资源使用情况
    cpu_percent = psutil.cpu_percent(interval=1)
    memory = psutil.virtual_memory()
    disk = psutil.disk_usage("/")
    
    return {
        "timestamp": datetime.utcnow().isoformat(),
        "system": {
            "cpu_percent": cpu_percent,
            "memory_percent": memory.percent,
            "memory_total_gb": memory.total / (1024**3),
            "memory_used_gb": memory.used / (1024**3),
            "disk_percent": disk.percent,
            "disk_total_gb": disk.total / (1024**3),
            "disk_used_gb": disk.used / (1024**3)
        }
    }


@router.get("/tasks")
async def task_monitoring(
    db: AsyncSession = Depends(get_db)
):
    """
    任务监控
    """
    # 任务统计
    result = await db.execute(
        select(
            MigrationTask.status,
            func.count(MigrationTask.id).label("count")
        )
        .group_by(MigrationTask.status)
    )
    status_stats = result.all()
    
    # 最近任务
    result = await db.execute(
        select(MigrationTask)
        .order_by(MigrationTask.created_at.desc())
        .limit(10)
    )
    recent_tasks = result.scalars().all()
    
    return {
        "timestamp": datetime.utcnow().isoformat(),
        "task_stats": [
            {"status": status, "count": count}
            for status, count in status_stats
        ],
        "recent_tasks": [
            {
                "id": str(task.id),
                "name": task.name,
                "status": task.status,
                "progress": task.progress,
                "created_at": task.created_at.isoformat()
            }
            for task in recent_tasks
        ]
    }