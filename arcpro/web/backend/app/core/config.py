"""
应用配置管理
"""
from typing import List, Optional
from pydantic_settings import BaseSettings
from pydantic import AnyHttpUrl, validator, PostgresDsn


class Settings(BaseSettings):
    # 应用配置
    PROJECT_NAME: str = "ArcGIS Pro迁移工具"
    VERSION: str = "1.0.0"
    API_V1_STR: str = "/api/v1"
    HOST: str = "0.0.0.0"
    PORT: int = 8000
    RELOAD: bool = True
    LOG_LEVEL: str = "INFO"
    DOCS_ENABLED: bool = True
    
    # 环境配置
    ENVIRONMENT: str = "development"  # development, testing, production
    DEBUG: bool = True
    
    # 安全配置
    SECRET_KEY: str = "your-secret-key-change-in-production"
    ALGORITHM: str = "HS256"
    ACCESS_TOKEN_EXPIRE_MINUTES: int = 30
    REFRESH_TOKEN_EXPIRE_DAYS: int = 7
    
    # CORS配置
    BACKEND_CORS_ORIGINS: List[AnyHttpUrl] = [
        "http://localhost:3000",  # 前端开发服务器
        "http://localhost:8000",  # 后端开发服务器
    ]
    
    ALLOWED_HOSTS: List[str] = ["localhost", "127.0.0.1", "0.0.0.0"]
    
    # 数据库配置
    DATABASE_URL: Optional[PostgresDsn] = "postgresql+asyncpg://postgres:password@localhost:5432/migration_db"
    DATABASE_POOL_SIZE: int = 20
    DATABASE_MAX_OVERFLOW: int = 40
    
    # Redis配置
    REDIS_URL: str = "redis://localhost:6379/0"
    REDIS_POOL_SIZE: int = 20
    
    # 文件存储配置
    MINIO_ENDPOINT: str = "localhost:9000"
    MINIO_ACCESS_KEY: str = "minioadmin"
    MINIO_SECRET_KEY: str = "minioadmin"
    MINIO_SECURE: bool = False
    MINIO_BUCKET: str = "migration-files"
    
    # 文件上传配置
    MAX_UPLOAD_SIZE: int = 104857600  # 100MB
    ALLOWED_EXTENSIONS: List[str] = [".aprx", ".zip", ".qgs", ".gpkg"]
    UPLOAD_DIR: str = "./uploads"
    
    # 迁移工具配置
    MIGRATION_SCRIPT_PATH: str = "../../script"  # 相对于backend目录
    MIGRATION_TIMEOUT: int = 3600  # 1小时超时
    MIGRATION_WORKERS: int = 4
    
    # 验证器
    @validator("BACKEND_CORS_ORIGINS", pre=True)
    def assemble_cors_origins(cls, v):
        if isinstance(v, str) and not v.startswith("["):
            return [i.strip() for i in v.split(",")]
        elif isinstance(v, (list, str)):
            return v
        raise ValueError(v)
    
    class Config:
        case_sensitive = True
        env_file = ".env"
        env_file_encoding = "utf-8"

settings = Settings()