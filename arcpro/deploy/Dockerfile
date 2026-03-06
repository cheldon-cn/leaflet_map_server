# ArcGIS Pro到QGIS迁移工具Docker镜像
# 注意：由于ArcGIS Pro需要Windows环境，此Dockerfile仅适用于无ArcGIS Pro的部署场景
# 对于需要ArcGIS Pro的环境，建议使用Windows容器或本地部署

FROM python:3.8-slim

# 设置工作目录
WORKDIR /app

# 安装系统依赖
RUN apt-get update && apt-get install -y \
    gdal-bin \
    libgdal-dev \
    postgresql-client \
    && rm -rf /var/lib/apt/lists/*

# 复制依赖文件
COPY deploy/requirements.txt .

# 安装Python依赖
RUN pip install --no-cache-dir -r requirements.txt

# 复制应用代码
COPY script/ ./script/
COPY arcgis_migration_schema.xsd .
COPY deploy/deploy_intergrate_steps_summary.md .

# 创建必要的目录
RUN mkdir -p /data/input /data/output /data/temp

# 设置环境变量
ENV PYTHONPATH=/app/script
ENV WORK_DIR=/app
ENV INPUT_DIR=/data/input
ENV OUTPUT_DIR=/data/output
ENV TEMP_DIR=/data/temp

# 设置非root用户（安全考虑）
RUN useradd -m -u 1000 migration && \
    chown -R migration:migration /app /data
USER migration

# 健康检查
HEALTHCHECK --interval=30s --timeout=3s --start-period=5s --retries=3 \
    CMD python -c "import sys; sys.exit(0)"

# 默认命令：显示帮助信息
CMD ["python", "script/migration_tool.py", "--help"]