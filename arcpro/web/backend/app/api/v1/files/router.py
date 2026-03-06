"""
文件管理路由
"""
from fastapi import APIRouter, Depends, UploadFile, File, HTTPException, status
from fastapi.responses import FileResponse
from sqlalchemy.ext.asyncio import AsyncSession

from app.core.database import get_db
from app.services.file_service import FileService

router = APIRouter()


@router.post("/upload")
async def upload_file(
    file: UploadFile = File(...),
    db: AsyncSession = Depends(get_db),
    current_user_id: str = "demo_user_id"  # 临时演示
):
    """
    上传文件
    """
    service = FileService(db)
    file_info = await service.save_upload_file(file, current_user_id)
    
    return {
        "message": "文件上传成功",
        "file_id": file_info["unique_filename"],
        "filename": file_info["filename"],
        "file_size": file_info["file_size"],
        "content_type": file_info["content_type"]
    }


@router.get("/{file_id}/download")
async def download_file(
    file_id: str,
    db: AsyncSession = Depends(get_db)
):
    """
    下载文件
    """
    service = FileService(db)
    file_path = await service.get_file_path(file_id)
    
    if not file_path or not file_path.exists():
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail="文件未找到"
        )
    
    return FileResponse(
        path=str(file_path),
        filename=file_path.name,
        media_type="application/octet-stream"
    )


@router.delete("/{file_id}", status_code=status.HTTP_204_NO_CONTENT)
async def delete_file(
    file_id: str,
    db: AsyncSession = Depends(get_db)
):
    """
    删除文件
    """
    service = FileService(db)
    success = await service.delete_file(file_id)
    if not success:
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail="文件未找到"
        )