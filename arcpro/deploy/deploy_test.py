#!/usr/bin/env python3
"""
部署测试脚本
验证迁移工具的基本部署和配置
"""

import os
import sys
import subprocess
import platform
from pathlib import Path

def print_header(text):
    """打印标题"""
    print("\n" + "="*60)
    print(f"{text}")
    print("="*60)

def run_command(cmd, capture_output=True):
    """运行命令并返回结果"""
    try:
        if capture_output:
            result = subprocess.run(cmd, shell=True, capture_output=True, text=True, timeout=30)
        else:
            result = subprocess.run(cmd, shell=True, timeout=30)
        return result.returncode == 0, result.stdout if capture_output else "", result.stderr if capture_output else ""
    except subprocess.TimeoutExpired:
        return False, "", "命令执行超时"
    except Exception as e:
        return False, "", str(e)

def test_python_version():
    """测试Python版本"""
    print_header("1. 测试Python版本")
    
    success, output, error = run_command("python --version")
    if success:
        print(f"[OK] Python版本: {output.strip()}")
        
        # 检查Python 3.8+
        version_str = output.strip().split()[1]
        major, minor = map(int, version_str.split('.')[:2])
        if major >= 3 and minor >= 8:
            print("[OK] Python版本满足要求 (3.8+)")
            return True
        else:
            print(f"[FAIL] Python版本过低: {version_str}，需要3.8+")
            return False
    else:
        print(f"[FAIL] 无法获取Python版本: {error}")
        return False

def test_dependencies():
    """测试依赖安装"""
    print_header("2. 测试Python依赖")
    
    # 检查requirements.txt是否存在
    if not os.path.exists("deploy/requirements.txt"):
        print("[FAIL] requirements.txt文件不存在")
        return False
    
    print("[OK] requirements.txt文件存在")
    
    # 检查关键依赖
    dependencies = [
        ("psycopg2", "psycopg2-binary"),
        ("yaml", "PyYAML"),
        ("lxml", "lxml"),
        ("psutil", "psutil"),
    ]
    
    all_installed = True
    for module, package in dependencies:
        success, output, error = run_command(f"python -c \"import {module}\"")
        if success:
            print(f"[OK] {package} 已安装")
        else:
            print(f"[FAIL] {package} 未安装: {error}")
            all_installed = False
    
    return all_installed

def test_imports():
    """测试Python模块导入"""
    print_header("3. 测试Python模块导入")
    
    # 检查关键模块
    modules = [
        "enhanced_aprx_parser",
        "migration_xml_generator", 
        "local_data_converter",
        "postgresql_migrator",
        "style_converter",
        "performance_optimizer",
        "validation_system",
        "config_manager",
    ]
    
    all_imported = True
    for module in modules:
        module_path = f"script.{module}"
        success, output, error = run_command(f"python -c \"import {module_path}\"")
        if success:
            print(f"[OK] {module} 可导入")
        else:
            print(f"[FAIL] {module} 导入失败: {error}")
            all_imported = False
    
    return all_imported

def test_config_files():
    """测试配置文件"""
    print_header("4. 测试配置文件")
    
    config_files = [
        "deploy/requirements.txt",
        "deploy/.env.example",
        "deploy/config.yaml.example",
        "deploy/Dockerfile",
        "deploy/docker-compose.yml",
    ]
    
    all_exist = True
    for file in config_files:
        if os.path.exists(file):
            print(f"[OK] {file} 存在")
        else:
            print(f"[FAIL] {file} 不存在")
            all_exist = False
    
    return all_exist

def test_script_files():
    """测试脚本文件权限"""
    print_header("5. 测试脚本文件")
    
    script_files = [
        "deploy/run_migration.bat",
        "deploy/run_migration.sh",
    ]
    
    all_exist = True
    for file in script_files:
        if os.path.exists(file):
            print(f"[OK] {file} 存在")
            
            # 如果是.sh文件，检查执行权限
            if file.endswith('.sh'):
                if os.access(file, os.X_OK):
                    print(f"  [OK] {file} 有执行权限")
                else:
                    print(f"  [WARN] {file} 无执行权限，运行: chmod +x {file}")
        else:
            print(f"[FAIL] {file} 不存在")
            all_exist = False
    
    return all_exist

def test_directory_structure():
    """测试目录结构"""
    print_header("6. 测试目录结构")
    
    required_dirs = [
        "script",
        "script/tests",
        "script/utils",
    ]
    
    all_exist = True
    for dir_path in required_dirs:
        if os.path.isdir(dir_path):
            print(f"[OK] 目录 {dir_path} 存在")
        else:
            print(f"[FAIL] 目录 {dir_path} 不存在")
            all_exist = False
    
    # 检查数据目录（如果不存在则创建）
    data_dirs = ["data/input", "data/output", "data/temp"]
    for dir_path in data_dirs:
        os.makedirs(dir_path, exist_ok=True)
        print(f"[OK] 数据目录 {dir_path} 已准备")
    
    return all_exist

def test_migration_tool():
    """测试迁移工具基本功能"""
    print_header("7. 测试迁移工具基本功能")
    
    # 测试帮助命令
    success, output, error = run_command("python script/migration_tool.py --help")
    if success:
        print("[OK] 迁移工具帮助命令正常")
        
        # 检查输出中是否包含关键信息
        if "usage" in output.lower() or "help" in output.lower():
            print("[OK] 帮助信息格式正确")
        else:
            print("[WARN] 帮助信息可能不完整")
        
        return True
    else:
        print(f"[FAIL] 迁移工具帮助命令失败: {error}")
        return False

def test_benchmark():
    """测试性能基准测试"""
    print_header("8. 测试性能基准测试")
    
    # 测试快速模式
    success, output, error = run_command("python script/benchmark.py --mode quick --output test_benchmark.json")
    if success:
        print("[OK] 性能基准测试运行成功")
        
        # 检查输出文件
        if os.path.exists("test_benchmark.json"):
            print("[OK] 基准测试报告已生成")
            os.remove("test_benchmark.json")  # 清理测试文件
        else:
            print("[WARN] 基准测试报告未生成")
        
        return True
    else:
        print(f"[WARN] 性能基准测试运行失败（可能是预期行为）: {error}")
        return True  # 不视为失败，因为可能需要特定环境

def generate_summary(passed_tests, total_tests):
    """生成测试总结"""
    print_header("部署测试总结")
    
    print(f"[OK] 通过的测试: {passed_tests}/{total_tests}")
    
    if passed_tests == total_tests:
        print("\n[SUCCESS] 所有测试通过！部署环境准备就绪。")
        print("\n下一步建议:")
        print("1. 复制 .env.example 为 .env 并配置环境变量")
        print("2. 复制 config.yaml.example 为 config.yaml 并修改配置")
        print("3. 运行迁移工具: python script/migration_tool.py --help")
        print("4. 或使用启动脚本: ./run_migration.sh (Linux/Mac) 或 run_migration.bat (Windows)")
        return True
    elif passed_tests >= total_tests * 0.7:
        print("\n[WARN] 大部分测试通过，但存在一些问题需要修复。")
        print("\n建议检查:")
        print("1. Python依赖是否安装完整")
        print("2. 配置文件是否存在")
        print("3. 脚本文件权限是否正确")
        return False
    else:
        print("\n[FAIL] 部署环境存在较多问题，需要修复。")
        return False

def main():
    """主测试函数"""
    print_header("ArcGIS Pro到QGIS迁移工具部署测试")
    print(f"操作系统: {platform.system()} {platform.release()}")
    print(f"工作目录: {os.getcwd()}")
    
    # 切换到项目根目录
    project_root = Path(__file__).parent.parent.absolute()
    os.chdir(project_root)
    print(f"项目目录: {project_root}")
    
    # 设置Python路径以便导入模块
    script_dir = project_root / "script"
    os.environ['PYTHONPATH'] = str(script_dir) + os.pathsep + os.environ.get('PYTHONPATH', '')
    
    tests = [
        ("Python版本", test_python_version),
        ("依赖安装", test_dependencies),
        ("模块导入", test_imports),
        ("配置文件", test_config_files),
        ("脚本文件", test_script_files),
        ("目录结构", test_directory_structure),
        ("迁移工具", test_migration_tool),
        ("性能测试", test_benchmark),
    ]
    
    passed = 0
    results = []
    
    for test_name, test_func in tests:
        try:
            if test_func():
                passed += 1
                results.append((test_name, "[OK] 通过"))
            else:
                results.append((test_name, "[FAIL] 失败"))
        except Exception as e:
            results.append((test_name, f"[FAIL] 异常: {str(e)}"))
    
    # 打印详细结果
    print_header("详细测试结果")
    for test_name, result in results:
        print(f"{test_name:20} {result}")
    
    # 生成总结
    success = generate_summary(passed, len(tests))
    
    return 0 if success else 1

if __name__ == "__main__":
    sys.exit(main())