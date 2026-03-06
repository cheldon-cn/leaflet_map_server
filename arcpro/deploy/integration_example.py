#!/usr/bin/env python3
"""
ArcGIS Pro到QGIS迁移工具集成示例
展示如何将迁移工具集成到现有GIS工作流中
"""

import os
import sys
import subprocess
import json
import yaml
from pathlib import Path
from typing import List, Dict, Any

def setup_environment():
    """设置运行环境"""
    # 添加script目录到Python路径
    script_dir = Path(__file__).parent.parent / "script"
    sys.path.insert(0, str(script_dir))
    
    # 设置环境变量
    os.environ["PYTHONPATH"] = str(script_dir)
    
    print("[OK] 环境设置完成")

def load_config(config_file: str = "config.yaml") -> Dict[str, Any]:
    """加载配置文件"""
    config_path = Path(config_file)
    
    if config_path.exists():
        with open(config_path, 'r', encoding='utf-8') as f:
            config = yaml.safe_load(f)
        print(f"[OK] 配置文件加载成功: {config_file}")
        return config
    else:
        # 使用默认配置
        default_config = {
            "database": {
                "source": {
                    "host": "localhost",
                    "port": 5432,
                    "database": "source_gis",
                    "username": "source_user"
                },
                "target": {
                    "host": "localhost",
                    "port": 5432,
                    "database": "target_gis",
                    "username": "target_user"
                }
            },
            "paths": {
                "input": "./data/input",
                "output": "./data/output",
                "temp": "./data/temp"
            }
        }
        print(f"[WARN] 配置文件不存在，使用默认配置")
        return default_config

def run_migration_single(aprx_path: str, output_dir: str, config: Dict[str, Any]) -> bool:
    """
    运行单个工程迁移
    
    Args:
        aprx_path: .aprx文件路径
        output_dir: 输出目录
        config: 配置字典
        
    Returns:
        bool: 迁移是否成功
    """
    print(f"\n[INFO] 开始迁移工程: {aprx_path}")
    
    # 构建命令
    cmd = [
        sys.executable, "script/migration_tool.py",
        "--input", aprx_path,
        "--output", output_dir,
        "--verbose"
    ]
    
    try:
        # 执行迁移
        result = subprocess.run(
            cmd,
            capture_output=True,
            text=True,
            timeout=300  # 5分钟超时
        )
        
        if result.returncode == 0:
            print(f"[OK] 工程迁移成功: {aprx_path}")
            print(f"   输出目录: {output_dir}")
            
            # 检查输出文件
            output_xml = Path(output_dir) / f"{Path(aprx_path).stem}_migration.xml"
            if output_xml.exists():
                print(f"   生成XML文件: {output_xml}")
            else:
                print(f"   [WARN] XML文件未生成")
            
            return True
        else:
            print(f"[FAIL] 工程迁移失败: {aprx_path}")
            print(f"   错误信息: {result.stderr}")
            return False
            
    except subprocess.TimeoutExpired:
        print(f"[FAIL] 迁移超时: {aprx_path}")
        return False
    except Exception as e:
        print(f"[FAIL] 迁移异常: {aprx_path}")
        print(f"   异常信息: {str(e)}")
        return False

def run_migration_batch(project_list: List[str], output_base_dir: str, config: Dict[str, Any]) -> Dict[str, Any]:
    """
    批量运行工程迁移
    
    Args:
        project_list: .aprx文件路径列表
        output_base_dir: 输出基础目录
        config: 配置字典
        
    Returns:
        Dict: 批量迁移结果统计
    """
    print(f"\n[INFO] 开始批量迁移，共 {len(project_list)} 个工程")
    
    results = {
        "total": len(project_list),
        "success": 0,
        "failed": 0,
        "failed_projects": [],
        "details": {}
    }
    
    # 创建输出目录
    output_base = Path(output_base_dir)
    output_base.mkdir(parents=True, exist_ok=True)
    
    for i, aprx_path in enumerate(project_list, 1):
        aprx_path = Path(aprx_path)
        
        if not aprx_path.exists():
            print(f"[FAIL] 文件不存在: {aprx_path}")
            results["failed"] += 1
            results["failed_projects"].append(str(aprx_path))
            continue
        
        # 为每个工程创建独立的输出目录
        project_output_dir = output_base / aprx_path.stem
        project_output_dir.mkdir(exist_ok=True)
        
        print(f"\n[{i}/{len(project_list)}] 处理工程: {aprx_path.name}")
        
        # 运行迁移
        success = run_migration_single(str(aprx_path), str(project_output_dir), config)
        
        if success:
            results["success"] += 1
            results["details"][str(aprx_path)] = {
                "status": "success",
                "output_dir": str(project_output_dir)
            }
        else:
            results["failed"] += 1
            results["failed_projects"].append(str(aprx_path))
            results["details"][str(aprx_path)] = {
                "status": "failed",
                "output_dir": str(project_output_dir)
            }
    
    return results

def generate_integration_report(results: Dict[str, Any], report_file: str = "integration_report.json"):
    """生成集成报告"""
    report_path = Path(report_file)
    
    with open(report_path, 'w', encoding='utf-8') as f:
        json.dump(results, f, indent=2, ensure_ascii=False)
    
    print(f"\n[INFO] 生成集成报告: {report_path}")
    print(f"   总工程数: {results['total']}")
    print(f"   成功数: {results['success']}")
    print(f"   失败数: {results['failed']}")
    
    if results['failed'] > 0:
        print(f"   失败工程列表:")
        for project in results['failed_projects']:
            print(f"     - {project}")

def example_workflow():
    """示例工作流"""
    print("=" * 60)
    print("ArcGIS Pro到QGIS迁移工具集成示例")
    print("=" * 60)
    
    # 1. 设置环境
    setup_environment()
    
    # 2. 加载配置
    config = load_config()
    
    # 3. 准备测试数据
    # 假设在data/input目录下有测试工程文件
    input_dir = Path("data/input")
    input_dir.mkdir(parents=True, exist_ok=True)
    
    # 创建示例工程文件列表
    example_projects = [
        "project1.aprx",
        "project2.aprx",
        "project3.aprx"
    ]
    
    # 在实际使用中，应该扫描目录获取真实文件
    # 这里我们只检查存在的文件
    existing_projects = []
    for proj in example_projects:
        proj_path = input_dir / proj
        if proj_path.exists():
            existing_projects.append(str(proj_path))
        else:
            print(f"⚠️  示例工程文件不存在: {proj_path}")
            # 创建占位文件以便演示
            proj_path.touch()
            existing_projects.append(str(proj_path))
            print(f"   已创建占位文件: {proj_path}")
    
    if not existing_projects:
        print("❌ 未找到任何工程文件，请将.aprx文件放入data/input目录")
        return
    
    # 4. 运行批量迁移
    output_base = Path("data/output/migration_batch")
    results = run_migration_batch(existing_projects, str(output_base), config)
    
    # 5. 生成报告
    generate_integration_report(results)
    
    # 6. 后续处理示例
    print("\n[INFO] 后续处理示例:")
    print("   1. 验证迁移结果")
    print("   2. 发送通知邮件")
    print("   3. 更新任务状态")
    print("   4. 清理临时文件")
    
    print("\n[OK] 集成示例完成")

def integration_with_existing_system():
    """与现有系统集成的示例"""
    print("\n" + "=" * 60)
    print("与现有GIS工作流集成示例")
    print("=" * 60)
    
    # 示例1: 在数据处理流水线中集成
    print("\n📋 示例1: 数据处理流水线集成")
    print("""
# 在现有数据处理脚本中调用迁移工具
def process_gis_data(data_pipeline):
    # 1. 数据预处理
    preprocess_data(data_pipeline)
    
    # 2. 运行ArcGIS Pro到QGIS迁移
    migration_success = run_migration_single(
        aprx_path=data_pipeline['aprx_file'],
        output_dir=data_pipeline['output_dir'],
        config=data_pipeline['config']
    )
    
    # 3. 根据迁移结果决定后续流程
    if migration_success:
        # 迁移成功，继续后续处理
        postprocess_data(data_pipeline)
        update_database_status(data_pipeline, 'completed')
    else:
        # 迁移失败，记录错误并告警
        log_error(data_pipeline, 'Migration failed')
        send_alert(data_pipeline, 'Migration failure')
        update_database_status(data_pipeline, 'failed')
    
    return migration_success
    """)
    
    # 示例2: 定时批量迁移
    print("\n📋 示例2: 定时批量迁移")
    print("""
# 使用计划任务定期运行批量迁移
def scheduled_batch_migration():
    # 1. 扫描输入目录获取新工程
    new_projects = scan_for_new_projects('data/input')
    
    if new_projects:
        # 2. 加载配置
        config = load_config('config.yaml')
        
        # 3. 运行批量迁移
        results = run_migration_batch(
            project_list=new_projects,
            output_base_dir='data/output',
            config=config
        )
        
        # 4. 记录结果并发送报告
        log_migration_results(results)
        send_daily_report(results)
        
        # 5. 清理已处理的文件
        archive_processed_projects(new_projects)
        
        return results
    else:
        print("没有新工程需要处理")
        return None
    """)
    
    # 示例3: API集成
    print("\n📋 示例3: Web API集成")
    print("""
# 创建RESTful API服务（使用Flask/FastAPI示例）
from fastapi import FastAPI, File, UploadFile
import tempfile

app = FastAPI()

@app.post("/migrate/")
async def migrate_project(file: UploadFile = File(...)):
    # 1. 保存上传的文件
    with tempfile.NamedTemporaryFile(suffix='.aprx', delete=False) as tmp:
        content = await file.read()
        tmp.write(content)
        aprx_path = tmp.name
    
    try:
        # 2. 运行迁移
        output_dir = tempfile.mkdtemp()
        success = run_migration_single(
            aprx_path=aprx_path,
            output_dir=output_dir,
            config=load_config()
        )
        
        if success:
            # 3. 返回结果文件
            return {
                "status": "success",
                "message": "Migration completed",
                "download_url": f"/download/{os.path.basename(output_dir)}"
            }
        else:
            return {
                "status": "error",
                "message": "Migration failed"
            }
    finally:
        # 4. 清理临时文件
        os.unlink(aprx_path)
    """)

def main():
    """主函数"""
    try:
        # 运行示例工作流
        example_workflow()
        
        # 展示集成示例
        integration_with_existing_system()
        
        print("\n" + "=" * 60)
        print("✅ 集成示例执行完成")
        print("=" * 60)
        print("\n下一步建议:")
        print("1. 根据实际需求修改integration_example.py")
        print("2. 配置config.yaml文件设置数据库连接")
        print("3. 将迁移工具集成到现有GIS工作流中")
        print("4. 使用提供的启动脚本简化部署")
        
    except Exception as e:
        print(f"[FAIL] 集成示例执行失败: {str(e)}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    sys.exit(main())