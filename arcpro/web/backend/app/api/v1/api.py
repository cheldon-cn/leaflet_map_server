"""
API v1 主路由
"""
from fastapi import APIRouter

from app.api.v1.auth import router as auth_router
from app.api.v1.projects import router as projects_router
from app.api.v1.tasks import router as tasks_router
from app.api.v1.files import router as files_router
from app.api.v1.monitoring import router as monitoring_router

router = APIRouter()

# 注册子路由
router.include_router(auth_router, prefix="/auth", tags=["认证"])
router.include_router(projects_router, prefix="/projects", tags=["工程管理"])
router.include_router(tasks_router, prefix="/tasks", tags=["迁移任务"])
router.include_router(files_router, prefix="/files", tags=["文件管理"])
router.include_router(monitoring_router, prefix="/monitoring", tags=["系统监控"])