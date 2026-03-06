"""
工程管理路由
"""
from typing import List
from fastapi import APIRouter, Depends, UploadFile, File, Form, HTTPException, status
from sqlalchemy.ext.asyncio import AsyncSession
from sqlalchemy import select, func

from app.core.database import get_db
from app.models.project import Project
from app.schemas.project import ProjectCreate, ProjectResponse, ProjectUpdate, ProjectParseResponse
from app.schemas.common import PaginatedResponse
from app.services.project_service import ProjectService

router = APIRouter()


@router.get("/", response_model=PaginatedResponse[ProjectResponse])
async def list_projects(
    skip: int = 0,
    limit: int = 100,
    db: AsyncSession = Depends(get_db),
    # TODO: 添加用户认证
    current_user_id: str = "demo_user_id"  # 临时演示
):
    """
    获取工程列表
    """
    service = ProjectService(db)
    projects = await service.get_user_projects(current_user_id, skip, limit)
    
    # 获取总数
    from sqlalchemy import select, func
    result = await db.execute(
        select(func.count()).select_from(Project).where(Project.user_id == current_user_id)
    )
    total = result.scalar()
    
    return PaginatedResponse(
        items=projects,
        total=total,
        page=skip // limit + 1,
        size=limit,
        pages=(total + limit - 1) // limit
    )


@router.post("/", response_model=ProjectResponse, status_code=status.HTTP_201_CREATED)
async def create_project(
    name: str = Form(...),
    description: str = Form(None),
    file: UploadFile = File(...),
    db: AsyncSession = Depends(get_db),
    current_user_id: str = "demo_user_id"  # 临时演示
):
    """
    创建工程（上传文件）
    """
    # 保存文件
    from app.services.file_service import FileService
    file_service = FileService(db)
    file_info = await file_service.save_upload_file(file, current_user_id)
    
    # 创建工程记录
    project_data = ProjectCreate(
        name=name,
        description=description,
        original_filename=file_info["filename"],
        file_size=file_info["file_size"]
    )
    
    service = ProjectService(db)
    project = await service.create_project(current_user_id, project_data)
    
    # 更新文件路径
    project.file_path = file_info["file_path"]
    await db.commit()
    await db.refresh(project)
    
    return project


@router.get("/{project_id}", response_model=ProjectResponse)
async def get_project(
    project_id: str,
    db: AsyncSession = Depends(get_db)
):
    """
    获取工程详情
    """
    service = ProjectService(db)
    project = await service.get_project(project_id)
    if not project:
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail="工程未找到"
        )
    
    return project


@router.put("/{project_id}", response_model=ProjectResponse)
async def update_project(
    project_id: str,
    project_data: ProjectUpdate,
    db: AsyncSession = Depends(get_db)
):
    """
    更新工程信息
    """
    service = ProjectService(db)
    project = await service.update_project(project_id, project_data)
    return project


@router.delete("/{project_id}", status_code=status.HTTP_204_NO_CONTENT)
async def delete_project(
    project_id: str,
    db: AsyncSession = Depends(get_db)
):
    """
    删除工程
    """
    service = ProjectService(db)
    success = await service.delete_project(project_id)
    if not success:
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail="工程未找到"
        )


@router.post("/{project_id}/parse", response_model=ProjectParseResponse)
async def parse_project(
    project_id: str,
    db: AsyncSession = Depends(get_db)
):
    """
    解析工程文件
    """
    service = ProjectService(db)
    result = await service.parse_project(project_id)
    return result