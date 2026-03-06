"""
工程服务
"""
from typing import Optional, List, Dict, Any
from sqlalchemy.ext.asyncio import AsyncSession
from sqlalchemy import select, desc
from app.models.project import Project
from app.schemas.project import ProjectCreate, ProjectUpdate
from app.core.exceptions import NotFoundError, ConflictError


class ProjectService:
    """工程服务"""
    
    def __init__(self, db: AsyncSession):
        self.db = db
    
    async def get_project(self, project_id: str) -> Optional[Project]:
        """
        根据ID获取工程
        """
        result = await self.db.execute(
            select(Project).where(Project.id == project_id)
        )
        return result.scalar_one_or_none()
    
    async def get_user_projects(
        self, 
        user_id: str, 
        skip: int = 0, 
        limit: int = 100
    ) -> List[Project]:
        """
        获取用户的工程列表
        """
        result = await self.db.execute(
            select(Project)
            .where(Project.user_id == user_id)
            .order_by(desc(Project.created_at))
            .offset(skip)
            .limit(limit)
        )
        return result.scalars().all()
    
    async def create_project(self, user_id: str, project_data: ProjectCreate) -> Project:
        """
        创建工程
        """
        project = Project(
            user_id=user_id,
            name=project_data.name,
            description=project_data.description,
            original_filename=project_data.original_filename,
            file_size=project_data.file_size,
            status="uploaded"
        )
        
        self.db.add(project)
        await self.db.commit()
        await self.db.refresh(project)
        
        return project
    
    async def update_project(self, project_id: str, project_data: ProjectUpdate) -> Project:
        """
        更新工程
        """
        project = await self.get_project(project_id)
        if not project:
            raise NotFoundError(detail="工程未找到")
        
        # 更新字段
        update_data = project_data.dict(exclude_unset=True)
        for field, value in update_data.items():
            setattr(project, field, value)
        
        await self.db.commit()
        await self.db.refresh(project)
        
        return project
    
    async def delete_project(self, project_id: str) -> bool:
        """
        删除工程
        """
        project = await self.get_project(project_id)
        if not project:
            raise NotFoundError(detail="工程未找到")
        
        await self.db.delete(project)
        await self.db.commit()
        
        return True
    
    async def parse_project(self, project_id: str) -> Dict[str, Any]:
        """
        解析工程文件
        """
        project = await self.get_project(project_id)
        if not project:
            raise NotFoundError(detail="工程未找到")
        
        # TODO: 调用现有迁移工具的解析功能
        # 这里应该调用script/enhanced_aprx_parser.py
        
        # 模拟解析结果
        project.status = "parsed"
        project.metadata = {
            "project_name": "示例工程",
            "map_count": 3,
            "layer_count": 15,
            "coordinate_system": "WGS84"
        }
        
        await self.db.commit()
        
        return {
            "success": True,
            "message": "工程解析成功",
            "metadata": project.metadata
        }