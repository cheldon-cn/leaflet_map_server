"""
迁移任务工作器
"""
import os
import sys
import json
import subprocess
from pathlib import Path
from datetime import datetime
import logging

# 添加项目根目录到Python路径
sys.path.insert(0, str(Path(__file__).parent.parent.parent))

from app.core.config import settings
from app.core.database import AsyncSessionLocal
from app.models.task import MigrationTask, TaskStep
from app.services.task_service import TaskService

# 设置日志
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

class MigrationWorker:
    """迁移任务工作器"""
    
    def __init__(self):
        self.script_path = Path(settings.MIGRATION_SCRIPT_PATH)
        self.timeout = settings.MIGRATION_TIMEOUT
    
    async def process_task(self, task_id: str):
        """
        处理迁移任务
        """
        logger.info(f"开始处理任务: {task_id}")
        
        async with AsyncSessionLocal() as db:
            try:
                # 获取任务
                task_service = TaskService(db)
                task = await task_service.get_task(task_id)
                
                if not task:
                    logger.error(f"任务未找到: {task_id}")
                    return
                
                # 更新任务状态为运行中
                task.status = "running"
                task.started_at = datetime.utcnow()
                await db.commit()
                
                # 查找对应的工程
                from app.models.project import Project
                result = await db.execute(
                    f"SELECT * FROM projects WHERE id = '{task.project_id}'"
                )
                project = result.first()
                
                if not project:
                    logger.error(f"工程未找到: {task.project_id}")
                    task.status = "failed"
                    task.error_message = "关联的工程未找到"
                    await db.commit()
                    return
                
                # 构建迁移命令
                config = task.config
                cmd = [
                    "python",
                    str(self.script_path / "migration_tool.py"),
                    "--aprx", project.file_path,
                    "--output", f"./uploads/results/{task_id}",
                    "--config", json.dumps(config),
                    "--verbose", "true"
                ]
                
                # 记录步骤开始
                step = TaskStep(
                    task_id=task_id,
                    step_name="开始迁移",
                    step_order=1,
                    status="running",
                    progress=0.1,
                    started_at=datetime.utcnow(),
                    details={"command": " ".join(cmd)}
                )
                db.add(step)
                await db.commit()
                
                # 开始执行迁移
                logger.info(f"执行命令: {' '.join(cmd)}")
                
                try:
                    # 创建结果目录
                    result_dir = Path(f"./uploads/results/{task_id}")
                    result_dir.mkdir(parents=True, exist_ok=True)
                    
                    # 执行迁移
                    process = subprocess.run(
                        cmd,
                        capture_output=True,
                        text=True,
                        timeout=self.timeout,
                        cwd=str(self.script_path.parent)
                    )
                    
                    # 记录迁移结果
                    # 更新步骤状态
                    step.status = "completed"
                    step.completed_at = datetime.utcnow()
                    step.progress = 1.0
                    step.details = {
                        "exit_code": process.returncode,
                        "stdout": process.stdout[:1000] if process.stdout else "",
                        "stderr": process.stderr[:1000] if process.stderr else ""
                    }
                    
                    # 更新任务状态
                    if process.returncode == 0:
                        task.status = "completed"
                        task.progress = 1.0
                        
                        # 查找生成的QGS文件
                        qgs_files = list(result_dir.glob("*.qgs"))
                        if qgs_files:
                            task.result_path = str(qgs_files[0])
                        
                        # 记录任务日志
                        task.logs = f"迁移成功\n标准输出:\n{process.stdout[:5000]}\n错误输出:\n{process.stderr[:5000]}"
                    else:
                        task.status = "failed"
                        task.error_message = f"迁移失败，退出码: {process.returncode}"
                        task.logs = f"迁移失败\n标准输出:\n{process.stdout[:5000]}\n错误输出:\n{process.stderr[:5000]}"
                    
                except subprocess.TimeoutExpired:
                    logger.error(f"任务超时: {task_id}")
                    task.status = "failed"
                    task.error_message = "迁移任务超时"
                    step.status = "failed"
                
                except Exception as e:
                    logger.error(f"任务执行异常: {e}")
                    task.status = "failed"
                    task.error_message = str(e)
                    step.status = "failed"
                
                finally:
                    task.completed_at = datetime.utcnow()
                    await db.commit()
                    logger.info(f"任务处理完成: {task_id}")
            
            except Exception as e:
                logger.error(f"处理任务时发生异常: {e}")
                import traceback
                logger.error(traceback.format_exc())


async def main():
    """主函数"""
    if len(sys.argv) < 2:
        print("用法: python -m app.workers.migration_worker <task_id>")
        sys.exit(1)
    
    task_id = sys.argv[1]
    worker = MigrationWorker()
    await worker.process_task(task_id)


if __name__ == "__main__":
    import asyncio
    asyncio.run(main())