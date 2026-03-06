"""
用户服务
"""
from typing import Optional
from sqlalchemy.ext.asyncio import AsyncSession
from sqlalchemy import select
from app.models.user import User
from app.schemas.user import UserCreate, UserUpdate
from app.core.security import get_password_hash, verify_password
from app.core.exceptions import NotFoundError, ConflictError


class UserService:
    """用户服务"""
    
    def __init__(self, db: AsyncSession):
        self.db = db
    
    async def get_user(self, user_id: str) -> Optional[User]:
        """
        根据ID获取用户
        """
        result = await self.db.execute(
            select(User).where(User.id == user_id)
        )
        return result.scalar_one_or_none()
    
    async def get_user_by_username(self, username: str) -> Optional[User]:
        """
        根据用户名获取用户
        """
        result = await self.db.execute(
            select(User).where(User.username == username)
        )
        return result.scalar_one_or_none()
    
    async def get_user_by_email(self, email: str) -> Optional[User]:
        """
        根据邮箱获取用户
        """
        result = await self.db.execute(
            select(User).where(User.email == email)
        )
        return result.scalar_one_or_none()
    
    async def create_user(self, user_data: UserCreate) -> User:
        """
        创建用户
        """
        # 检查用户名是否已存在
        existing_user = await self.get_user_by_username(user_data.username)
        if existing_user:
            raise ConflictError(detail="用户名已存在")
        
        # 检查邮箱是否已存在
        existing_email = await self.get_user_by_email(user_data.email)
        if existing_email:
            raise ConflictError(detail="邮箱已存在")
        
        # 创建用户
        hashed_password = get_password_hash(user_data.password)
        user = User(
            username=user_data.username,
            email=user_data.email,
            hashed_password=hashed_password,
            full_name=user_data.full_name,
            is_active=user_data.is_active,
            is_superuser=user_data.is_superuser
        )
        
        self.db.add(user)
        await self.db.commit()
        await self.db.refresh(user)
        
        return user
    
    async def update_user(self, user_id: str, user_data: UserUpdate) -> User:
        """
        更新用户
        """
        user = await self.get_user(user_id)
        if not user:
            raise NotFoundError(detail="用户未找到")
        
        # 更新字段
        update_data = user_data.dict(exclude_unset=True)
        
        if "password" in update_data:
            update_data["hashed_password"] = get_password_hash(update_data.pop("password"))
        
        for field, value in update_data.items():
            setattr(user, field, value)
        
        await self.db.commit()
        await self.db.refresh(user)
        
        return user
    
    async def delete_user(self, user_id: str) -> bool:
        """
        删除用户
        """
        user = await self.get_user(user_id)
        if not user:
            raise NotFoundError(detail="用户未找到")
        
        await self.db.delete(user)
        await self.db.commit()
        
        return True
    
    async def authenticate_user(self, username: str, password: str) -> Optional[User]:
        """
        验证用户凭证
        """
        user = await self.get_user_by_username(username)
        if not user:
            return None
        
        if not verify_password(password, user.hashed_password):
            return None
        
        if not user.is_active:
            return None
        
        return user