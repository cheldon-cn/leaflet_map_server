"""
文件服务
"""
from typing import Optional, BinaryIO
import os
from pathlib import Path
from fastapi import UploadFile
from sqlalchemy.ext.asyncio import AsyncSession

from app.core.config import settings
from app.core.exceptions import NotFoundError, ValidationError


class FileService:
    """文件服务"""
    
    def __init__(self, db: AsyncSession):
        self.db = db
        self.upload_dir = Path(settings.UPLOAD_DIR)
        self.upload_dir.mkdir(parents=True, exist_ok=True)
    
    async def save_upload_file(self, file: UploadFile, user_id: str) -> Dict[str, Any]:
        """
        保存上传文件
        """
        # 验证文件扩展名
        file_ext = Path(file.filename).suffix.lower()
        if file_ext not in settings.ALLOWED_EXTENSIONS:
            raise ValidationError(detail=f"不支持的文件类型: {file_ext}")
        
        # 验证文件大小
        file.file.seek(0, 2)  # 移动到文件末尾
        file_size = file.file.tell()
        file.file.seek(0)  # 重置文件指针
        
        if file_size > settings.MAX_UPLOAD_SIZE:
            raise ValidationError(detail=f"文件太大，最大支持 {settings.MAX_UPLOAD_SIZE // 1024 // 1024}MB")
        
        # 生成唯一文件名
        import uuid
        unique_filename = f"{uuid.uuid4()}{file_ext}"
        file_path = self.upload_dir / unique_filename
        
        # 保存文件
        with open(file_path, "wb") as buffer:
            content = await file.read()
            buffer.write(content)
        
        return {
            "filename": file.filename,
            "unique_filename": unique_filename,
            "file_path": str(file_path),
            "file_size": file_size,
            "content_type": file.content_type
        }
    
    async def get_file_path(self, file_id: str) -> Optional[Path]:
        """
        获取文件路径
        """
        # TODO: 从数据库查询文件信息
        file_path = self.upload_dir / file_id
        if file_path.exists():
            return file_path
        return None
    
    async def delete_file(self, file_id: str) -> bool:
        """
        删除文件
        """
        file_path = self.upload_dir / file_id
        if file_path.exists():
            file_path.unlink()
            return True
        return False