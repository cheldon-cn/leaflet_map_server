"""
新功能集成测试脚本
演示阶段3实现的所有新功能
"""

import os
import sys
import logging
from pathlib import Path

# 添加当前目录到路径
sys.path.insert(0, str(Path(__file__).parent))

# 设置日志
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)


def test_compatibility_checker():
    """测试版本兼容性检查器"""
    print("\n" + "="*60)
    print("测试: 版本兼容性检查器")
    print("="*60)
    
    try:
        from compatibility_checker import check_compatibility, get_supported_arcgis_versions
        
        # 测试兼容性检查
        test_cases = [
            ("3.2", "3.34"),  # 应该完全兼容
            ("2.5", "3.10"),  # 应该兼容
            ("3.3", "3.38"),  # 应该兼容
            ("2.0", "3.2"),   # QGIS版本太低
            ("3.3", "2.0"),   # QGIS版本不存在
        ]
        
        for arcgis_version, qgis_version in test_cases:
            result = check_compatibility(arcgis_version, qgis_version)
            print(f"\nArcGIS Pro {arcgis_version} → QGIS {qgis_version}:")
            print(f"  兼容性级别: {result.get('compatibility_level', 'unknown')}")
            print(f"  问题数量: {result.get('summary', {}).get('total_issues', 0)}")
        
        # 测试支持的ArcGIS版本
        supported_versions = get_supported_arcgis_versions()
        print(f"\n支持的ArcGIS Pro版本: {', '.join(supported_versions[:5])}...")
        
        return True
        
    except Exception as e:
        print(f"兼容性检查器测试失败: {e}")
        return False


def test_plugin_system():
    """测试插件系统"""
    print("\n" + "="*60)
    print("测试: 插件系统")
    print("="*60)
    
    try:
        from plugin_system import (
            PluginManager, ExampleDataConverterPlugin, ExampleStyleConverterPlugin
        )
        
        # 创建插件管理器
        config = {
            "data_dir": "./data",
            "temp_dir": "./temp",
            "plugin_dirs": ["."]
        }
        
        manager = PluginManager(config)
        
        # 注册示例插件
        manager.register_plugin(ExampleDataConverterPlugin)
        manager.register_plugin(ExampleStyleConverterPlugin)
        
        # 加载插件
        manager.load_plugin("example_data_converter")
        manager.load_plugin("example_style_converter")
        
        # 获取插件信息
        plugins = manager.get_all_plugins()
        print(f"已注册插件数量: {len(plugins)}")
        for plugin in plugins:
            print(f"  - {plugin.name} ({plugin.plugin_id}): {plugin.status.value}")
        
        # 执行插件方法
        data_result = manager.execute_plugin_method(
            "example_data_converter",
            "convert",
            "/test/input.shp",
            "/test/output.gpkg"
        )
        print(f"\n数据转换结果: {data_result.get('success', False)}")
        
        # 清理
        manager.shutdown()
        
        return True
        
    except Exception as e:
        print(f"插件系统测试失败: {e}")
        return False


def test_dependency_injection():
    """测试依赖注入框架"""
    print("\n" + "="*60)
    print("测试: 依赖注入框架")
    print("="*60)
    
    try:
        from di import DependencyContainer, inject
        
        # 创建服务类
        class ConfigService:
            def __init__(self):
                self.config = {"app_name": "Migration Tool"}
            
            def get_config(self):
                return self.config
        
        class DatabaseService:
            def __init__(self, config_service: ConfigService):
                self.config_service = config_service
                self.connection = None
            
            def connect(self):
                config = self.config_service.get_config()
                print(f"使用配置连接数据库: {config.get('app_name')}")
                self.connection = "模拟数据库连接"
                return True
        
        # 使用依赖注入装饰器
        @inject
        class MigrationService:
            def __init__(self, config_service: ConfigService, database_service: DatabaseService):
                self.config_service = config_service
                self.database_service = database_service
            
            def migrate(self):
                print("开始迁移...")
                self.database_service.connect()
                print("迁移完成")
                return True
        
        # 创建容器并注册服务
        container = DependencyContainer()
        container.register_singleton(ConfigService)
        container.register_singleton(DatabaseService)
        
        # 获取服务
        config_service = container.get_service(ConfigService)
        print(f"配置服务: {config_service.get_config()}")
        
        # 创建带依赖注入的实例
        migration_service = container.create_with_dependencies(MigrationService)
        result = migration_service.migrate()
        
        print(f"\n依赖注入测试结果: {result}")
        
        return True
        
    except Exception as e:
        print(f"依赖注入测试失败: {e}")
        return False


def test_performance_benchmark():
    """测试性能基准测试"""
    print("\n" + "="*60)
    print("测试: 性能基准测试")
    print("="*60)
    
    try:
        from benchmark import MigrationBenchmark
        
        # 创建基准测试器
        benchmark = MigrationBenchmark()
        
        # 运行基准测试
        print("运行基准测试套件...")
        results = benchmark.run_benchmark_suite()
        
        # 显示结果
        print(f"\n基准测试完成，共运行 {len(results)} 个测试")
        
        successful_tests = [r for r in results if r.success]
        failed_tests = [r for r in results if not r.success]
        
        print(f"成功: {len(successful_tests)}")
        print(f"失败: {len(failed_tests)}")
        
        # 检查30秒目标
        full_migration_test = next((r for r in results if r.test_name == "完整迁移流程性能测试"), None)
        if full_migration_test and full_migration_test.success:
            actual_time = full_migration_test.metrics.get("actual_time", 0)
            meets_target = full_migration_test.metrics.get("within_target", False)
            print(f"\n完整迁移时间: {actual_time:.2f}秒")
            print(f"30秒目标: {'✅ 达成' if meets_target else '❌ 未达成'}")
        
        # 生成报告
        report_path = benchmark.generate_report()
        print(f"\n详细报告已生成: {report_path}")
        
        return len(failed_tests) == 0
        
    except Exception as e:
        print(f"性能基准测试失败: {e}")
        return False


def test_logical_replication_enhancements():
    """测试逻辑复制增强功能"""
    print("\n" + "="*60)
    print("测试: 逻辑复制增强功能")
    print("="*60)
    
    try:
        from postgresql_migrator import PostgreSQLMigrationManager, DatabaseConfig
        
        # 创建迁移管理器
        manager = PostgreSQLMigrationManager()
        
        # 测试版本检查
        print("测试PostgreSQL版本检查...")
        version_result = manager.check_postgresql_version()
        print(f"  版本检查成功: {version_result.get('success', False)}")
        print(f"  PostgreSQL版本: {version_result.get('version', 'unknown')}")
        print(f"  支持逻辑复制: {version_result.get('supports_logical_replication', False)}")
        
        # 测试先决条件检查（模拟配置）
        source_config = DatabaseConfig(
            host="localhost",
            port=5432,
            database="source_db",
            username="postgres"
        )
        
        target_config = DatabaseConfig(
            host="localhost",
            port=5432,
            database="target_db",
            username="postgres"
        )
        
        print("\n测试逻辑复制先决条件检查...")
        prerequisites = manager.check_logical_replication_prerequisites(source_config, target_config)
        print(f"  先决条件检查成功: {prerequisites.get('success', False)}")
        print(f"  先决条件满足: {prerequisites.get('prerequisites_met', False)}")
        
        if not prerequisites.get("prerequisites_met", False):
            print(f"  错误: {prerequisites.get('errors', [])}")
        
        return True
        
    except Exception as e:
        print(f"逻辑复制增强测试失败: {e}")
        return False


def main():
    """主测试函数"""
    print("ArcGIS Pro到QGIS迁移系统 - 新功能集成测试")
    print("="*60)
    
    test_results = []
    
    # 运行所有测试
    test_results.append(("兼容性检查器", test_compatibility_checker()))
    test_results.append(("插件系统", test_plugin_system()))
    test_results.append(("依赖注入框架", test_dependency_injection()))
    test_results.append(("性能基准测试", test_performance_benchmark()))
    test_results.append(("逻辑复制增强", test_logical_replication_enhancements()))
    
    # 显示测试结果摘要
    print("\n" + "="*60)
    print("测试结果摘要")
    print("="*60)
    
    all_passed = True
    for test_name, passed in test_results:
        status = "✅ 通过" if passed else "❌ 失败"
        print(f"{test_name}: {status}")
        if not passed:
            all_passed = False
    
    print("\n" + "="*60)
    if all_passed:
        print("所有测试通过！新功能实现成功。")
    else:
        print("部分测试失败，请检查具体错误信息。")
    
    print("\n下一步建议:")
    print("1. 在实际环境中运行性能基准测试")
    print("2. 使用插件系统扩展迁移功能")
    print("3. 配置依赖注入框架管理服务")
    print("4. 运行完整的端到端测试")


if __name__ == "__main__":
    main()