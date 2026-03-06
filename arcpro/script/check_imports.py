"""
快速导入检查，确保所有新模块可以正确导入
"""

import sys
import os

def check_import(module_name, class_name=None):
    """检查模块导入"""
    try:
        module = __import__(module_name, fromlist=[''])
        if class_name:
            getattr(module, class_name)
        print(f"[OK] {module_name}" + (f".{class_name}" if class_name else ""))
        return True
    except Exception as e:
        print(f"[FAIL] {module_name}" + (f".{class_name}" if class_name else "") + f" - {e}")
        return False

def main():
    """主函数"""
    print("新模块导入检查")
    print("=" * 50)
    
    modules_to_check = [
        ("compatibility_checker", "VersionCompatibilityChecker"),
        ("plugin_system", "PluginManager"),
        ("di", "DependencyContainer"),
        ("benchmark", "MigrationBenchmark"),
        ("config_manager", "DatabaseConfig"),
    ]
    
    results = []
    for module_name, class_name in modules_to_check:
        success = check_import(module_name, class_name)
        results.append(success)
    
    # 检查postgresql_migrator.py中的新方法
    print("\n检查postgresql_migrator.py中的新方法...")
    try:
        from postgresql_migrator import PostgreSQLMigrationManager
        manager = PostgreSQLMigrationManager()
        
        # 检查新方法是否存在
        methods_to_check = [
            "check_postgresql_version",
            "check_logical_replication_prerequisites",
            "_migrate_logical_replication",
            "_migrate_with_batch_copy"
        ]
        
        for method_name in methods_to_check:
            if hasattr(manager, method_name):
                print(f"[OK] PostgreSQLMigrationManager.{method_name}")
            else:
                print(f"[FAIL] PostgreSQLMigrationManager.{method_name} - 不存在")
    
    except Exception as e:
        print(f"[FAIL] 导入postgresql_migrator.py失败: {e}")
    
    print("\n" + "=" * 50)
    if all(results):
        print("所有模块导入成功！")
    else:
        print("部分模块导入失败，请检查错误信息。")
    
    print("\n新功能摘要:")
    print("1. compatibility_checker.py - 版本兼容性检查")
    print("2. plugin_system.py - 插件系统架构")
    print("3. di.py - 依赖注入框架")
    print("4. benchmark.py - 性能基准测试")
    print("5. postgresql_migrator.py - 增强的逻辑复制功能")
    print("6. config_manager.py - 统一配置管理")

if __name__ == "__main__":
    main()