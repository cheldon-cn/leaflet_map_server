#!/usr/bin/env python3
"""
测试所有模块的导入功能
"""

import sys
import os
import pytest
pytestmark = pytest.mark.skip(reason="这是导入检查脚本，不是单元测试")

# 添加父目录到路径
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

def test_import(module_name: str, class_name: str = None):
    """测试导入模块"""
    try:
        module = __import__(module_name)
        if class_name:
            getattr(module, class_name)
        print(f"[OK] {module_name}" + (f".{class_name}" if class_name else ""))
        return True
    except ImportError as e:
        print(f"[FAIL] {module_name}: {e}")
        return False
    except AttributeError as e:
        print(f"[FAIL] {module_name}.{class_name}: {e}")
        return False

def main():
    print("测试模块导入...")
    
    # 第一阶段模块
    modules = [
        ("data_models", "MigrationProject"),
        ("enhanced_aprx_parser", "EnhancedAPRXParser"),
        ("migration_xml_generator", "MigrationXMLGenerator"),
        ("local_data_converter", "LocalDataConverter"),
        ("migration_tool", "MigrationTool"),
        ("aprx_parser", None),
    ]
    
    # 第二阶段模块
    try:
        import psycopg2
        POSTGRESQL_AVAILABLE = True
    except ImportError:
        POSTGRESQL_AVAILABLE = False
        print("警告: psycopg2 不可用，PostgreSQL相关模块将跳过")
    
    if POSTGRESQL_AVAILABLE:
        modules.extend([
            ("postgresql_migrator", "PostgreSQLMigrationEngine"),
            ("performance_optimizer", "PerformanceOptimizer"),
            ("style_converter", "StyleConverter"),
        ])
    
    success_count = 0
    total_count = len(modules)
    
    for module_name, class_name in modules:
        if test_import(module_name, class_name):
            success_count += 1
    
    print(f"\n导入测试完成: {success_count}/{total_count} 通过")
    
    if success_count == total_count:
        print("所有模块导入成功！")
        return 0
    else:
        print("部分模块导入失败，请检查依赖。")
        return 1

if __name__ == "__main__":
    sys.exit(main())