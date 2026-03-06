"""
工程模型
"""
import uuid
from datetime import datetime
from sqlalchemy import Column, String, Text, Integer, BigInteger, ForeignKey, DateTime
from sqlalchemy.dialects.postgresql import UUID, JSONB
from sqlalchemy.orm import relationship

from app.core.database import Base


class Project(Base):
    """工程表"""
    __tablename__ = "projects"
    
    id = Column(UUID(as_uuid=True), primary_key=True, default=uuid.uuid4, index=True)
    user_id = Column(UUID(as_uuid=True), ForeignKey("users.id", ondelete="CASCADE"), nullable=False, index=True)
    name = Column(String(255), nullable=False)
    description = Column(Text)
    original_filename = Column(String(255))
    file_path = Column(String(500))  # MinIO/S3路径
    file_size = Column(BigInteger)
    status = Column(String(50), default="uploaded")  # uploaded, parsing, parsed, error
    metadata = Column(JSONB)  # 解析后的工程元数据
    created_at = Column(DateTime(timezone=True), default=datetime.utcnow)
    updated_at = Column(DateTime(timezone=True), default=datetime.utcnow, onupdate=datetime.utcnow)
    
    # 关系
    user = relationship("User", back_populates="projects")
    tasks = relationship("MigrationTask", back_populates="project", cascade="all, delete-orphan")
    
    def __repr__(self):
        return f"<Project(id={self.id}, name={self.name}, status={self.status})>"