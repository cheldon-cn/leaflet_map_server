"""
迁移任务模型
"""
import uuid
from datetime import datetime
from sqlalchemy import Column, String, Text, Float, ForeignKey, DateTime
from sqlalchemy.dialects.postgresql import UUID, JSONB
from sqlalchemy.orm import relationship

from app.core.database import Base


class MigrationTask(Base):
    """迁移任务表"""
    __tablename__ = "migration_tasks"
    
    id = Column(UUID(as_uuid=True), primary_key=True, default=uuid.uuid4, index=True)
    project_id = Column(UUID(as_uuid=True), ForeignKey("projects.id", ondelete="CASCADE"), nullable=False, index=True)
    user_id = Column(UUID(as_uuid=True), ForeignKey("users.id", ondelete="SET NULL"), nullable=True, index=True)
    name = Column(String(255), nullable=False)
    description = Column(Text)
    config = Column(JSONB, nullable=False)  # 迁移配置
    status = Column(String(50), default="pending")  # pending, running, completed, failed, cancelled
    progress = Column(Float, default=0.0)
    error_message = Column(Text)
    result_path = Column(String(500))  # 结果文件路径
    logs = Column(Text)  # 任务日志
    started_at = Column(DateTime(timezone=True))
    completed_at = Column(DateTime(timezone=True))
    created_at = Column(DateTime(timezone=True), default=datetime.utcnow, index=True)
    updated_at = Column(DateTime(timezone=True), default=datetime.utcnow, onupdate=datetime.utcnow)
    
    # 关系
    project = relationship("Project", back_populates="tasks")
    user = relationship("User", back_populates="tasks")
    steps = relationship("TaskStep", back_populates="task", cascade="all, delete-orphan")
    
    def __repr__(self):
        return f"<MigrationTask(id={self.id}, name={self.name}, status={self.status}, progress={self.progress})>"


class TaskStep(Base):
    """任务步骤表"""
    __tablename__ = "task_steps"
    
    id = Column(UUID(as_uuid=True), primary_key=True, default=uuid.uuid4)
    task_id = Column(UUID(as_uuid=True), ForeignKey("migration_tasks.id", ondelete="CASCADE"), nullable=False, index=True)
    step_name = Column(String(100), nullable=False)
    step_order = Column(Integer, nullable=False)
    status = Column(String(50), default="pending")
    progress = Column(Float, default=0.0)
    started_at = Column(DateTime(timezone=True))
    completed_at = Column(DateTime(timezone=True))
    details = Column(JSONB)
    created_at = Column(DateTime(timezone=True), default=datetime.utcnow)
    
    # 关系
    task = relationship("MigrationTask", back_populates="steps")
    
    def __repr__(self):
        return f"<TaskStep(id={self.id}, step_name={self.step_name}, status={self.status})>"