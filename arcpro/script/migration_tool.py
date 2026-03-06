"""
ArcGIS Pro工程迁移工具 - 主程序
整合所有第一阶段和第二阶段的迁移功能
包含PostgreSQL大表迁移和样式转换映射
"""

import os
import sys
import argparse
import json
from typing import Optional, Dict, Any, List, TYPE_CHECKING
import datetime

# 添加当前目录到Python路径
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from enhanced_aprx_parser import EnhancedAPRXParser
from migration_xml_generator import MigrationXMLGenerator
from local_data_converter import LocalDataConverter
from data_models import MigrationProject

# 类型检查导入
if TYPE_CHECKING:
    from postgresql_migrator import DatabaseConfig

# 第二阶段功能模块
try:
    from postgresql_migrator import PostgreSQLMigrationEngine, DatabaseConfig
    POSTGRESQL_SUPPORT = True
except ImportError:
    POSTGRESQL_SUPPORT = False
    print("警告: PostgreSQL迁移模块未找到，相关功能将被禁用")

try:
    from style_converter import StyleConverter
    STYLE_CONVERSION_SUPPORT = True
except ImportError:
    STYLE_CONVERSION_SUPPORT = False
    print("警告: 样式转换模块未找到，相关功能将被禁用")

try:
    from performance_optimizer import PerformanceOptimizer
    PERFORMANCE_OPTIMIZATION_SUPPORT = True
except ImportError:
    PERFORMANCE_OPTIMIZATION_SUPPORT = False
    print("警告: 性能优化模块未找到，相关功能将被禁用")


class MigrationTool:
    """迁移工具主类"""
    
    def __init__(self):
        self.parser = EnhancedAPRXParser(use_arcpy=True)
        self.xml_generator = MigrationXMLGenerator()
        self.data_converter = LocalDataConverter(output_dir="migration_output")
    
    def run(self, aprx_path: str, output_xml: Optional[str] = None, 
            convert_data: bool = False, output_format: str = "GPKG",
            verbose: bool = False) -> Dict[str, Any]:
        """
        运行完整迁移流程
        
        Args:
            aprx_path: .aprx工程文件路径
            output_xml: 输出XML文件路径
            convert_data: 是否转换本地数据
            output_format: 数据输出格式
            verbose: 是否显示详细信息
            
        Returns:
            迁移结果字典
        """
        results = {
            "success": False,
            "steps": {},
            "errors": [],
            "warnings": []
        }
        
        try:
            # 步骤1: 解析工程文件
            if verbose:
                print(f"步骤1: 解析工程文件 {aprx_path}")
            
            migration_project = self.parser.parse_aprx(aprx_path)
            results["migration_project"] = migration_project
            results["steps"]["parsing"] = {
                "success": True,
                "project_name": migration_project.metadata.project_name,
                "map_count": len(migration_project.maps),
                "layer_count": len(migration_project.get_all_layers())
            }
            
            if verbose:
                print(f"  工程名称: {migration_project.metadata.project_name}")
                print(f"  地图数量: {len(migration_project.maps)}")
                print(f"  图层总数: {len(migration_project.get_all_layers())}")
            
            # 步骤2: 生成XML文件
            if verbose:
                print(f"步骤2: 生成XML文件")
            
            if output_xml is None:
                base_name = os.path.splitext(aprx_path)[0]
                output_xml = f"{base_name}_migration.xml"
            
            xml_content = self.xml_generator.generate_xml(migration_project, output_xml)
            
            results["steps"]["xml_generation"] = {
                "success": True,
                "xml_file": output_xml,
                "xml_size": len(xml_content)
            }
            
            if verbose:
                print(f"  XML文件: {output_xml}")
                print(f"  XML大小: {len(xml_content)} 字符")
            
            # 步骤3: 转换本地数据（如果启用）
            if convert_data:
                if verbose:
                    print(f"步骤3: 转换本地数据")
                
                # 获取所有图层
                all_layers = migration_project.get_all_layers()
                
                # 过滤出本地数据图层
                local_layers = []
                for layer in all_layers:
                    ds_type = layer.data_source.data_source_type
                    if ds_type in ["Shapefile", "FileGeodatabase", "Raster"]:
                        local_layers.append(layer)
                
                if local_layers:
                    conversion_results = self.data_converter.convert_project_layers(
                        local_layers, output_format
                    )
                    
                    results["steps"]["data_conversion"] = {
                        "success": conversion_results["successful"] > 0,
                        "total_local_layers": len(local_layers),
                        "successful_conversions": conversion_results["successful"],
                        "failed_conversions": conversion_results["failed"],
                        "output_dir": self.data_converter.output_dir
                    }
                    
                    if verbose:
                        print(f"  本地图层数: {len(local_layers)}")
                        print(f"  成功转换: {conversion_results['successful']}")
                        print(f"  失败转换: {conversion_results['failed']}")
                        print(f"  输出目录: {self.data_converter.output_dir}")
                else:
                    results["steps"]["data_conversion"] = {
                        "success": True,
                        "message": "没有需要转换的本地数据图层"
                    }
                    
                    if verbose:
                        print(f"  没有需要转换的本地数据图层")
            
            results["success"] = True
            results["message"] = "迁移流程完成"
            results["timestamp"] = datetime.datetime.now().isoformat()
            
            if verbose:
                print(f"\n迁移完成!")
            
        except Exception as e:
            results["success"] = False
            results["errors"].append(str(e))
            
            if verbose:
                print(f"\n迁移失败: {e}")
        
        return results
    
    def generate_report(self, results: Dict[str, Any], report_file: Optional[str] = None) -> str:
        """
        生成迁移报告
        
        Args:
            results: 迁移结果字典
            report_file: 报告文件路径
            
        Returns:
            报告内容
        """
        report_lines = []
        
        # 标题
        report_lines.append("=" * 60)
        report_lines.append("ArcGIS Pro工程迁移报告")
        report_lines.append("=" * 60)
        report_lines.append(f"生成时间: {datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        report_lines.append("")
        
        # 工程信息
        if "steps" in results and "parsing" in results["steps"]:
            parsing = results["steps"]["parsing"]
            report_lines.append("工程信息:")
            report_lines.append("-" * 40)
            if "project_name" in parsing:
                report_lines.append(f"工程名称: {parsing['project_name']}")
            if "map_count" in parsing:
                report_lines.append(f"地图数量: {parsing['map_count']}")
            if "layer_count" in parsing:
                report_lines.append(f"图层总数: {parsing['layer_count']}")
            report_lines.append("")
        
        # 总体状态
        if results["success"]:
            report_lines.append("总体状态: ✅ 成功")
        else:
            report_lines.append("总体状态: ❌ 失败")
        
        report_lines.append("")
        
        # 各步骤详情
        report_lines.append("步骤详情:")
        report_lines.append("-" * 40)
        
        for step_name, step_result in results["steps"].items():
            report_lines.append(f"{step_name}:")
            for key, value in step_result.items():
                if key != "success":
                    report_lines.append(f"  {key}: {value}")
            
            if step_result.get("success", False):
                report_lines.append("  状态: ✅ 完成")
            else:
                report_lines.append("  状态: ❌ 失败")
            report_lines.append("")
        
        # 错误信息
        if results["errors"]:
            report_lines.append("错误信息:")
            report_lines.append("-" * 40)
            for error in results["errors"]:
                report_lines.append(f"  - {error}")
            report_lines.append("")
        
        # 警告信息
        if results.get("warnings"):
            report_lines.append("警告信息:")
            report_lines.append("-" * 40)
            for warning in results["warnings"]:
                report_lines.append(f"  - {warning}")
            report_lines.append("")
        
        # 总结
        report_lines.append("=" * 60)
        report_lines.append("迁移工具 - 第一阶段功能实现")
        report_lines.append("1. 工程解析模块 ✓")
        report_lines.append("2. XML生成模块（符合XSD 1.0） ✓")
        report_lines.append("3. 本地数据转换基础 ✓")
        report_lines.append("=" * 60)
        
        report_content = "\n".join(report_lines)
        
        # 保存报告
        if report_file:
            os.makedirs(os.path.dirname(report_file), exist_ok=True)
            with open(report_file, 'w', encoding='utf-8') as f:
                f.write(report_content)
        
        return report_content


class EnhancedMigrationTool(MigrationTool):
    """增强版迁移工具 - 集成第二阶段功能"""
    
    def __init__(self, postgresql_config: Optional[Dict[str, Any]] = None, 
                 style_conversion_enabled: bool = True,
                 performance_optimization_enabled: bool = True):
        """
        初始化增强版迁移工具
        
        Args:
            postgresql_config: PostgreSQL目标数据库配置
            style_conversion_enabled: 是否启用样式转换
            performance_optimization_enabled: 是否启用性能优化
        """
        super().__init__()
        
        # PostgreSQL迁移引擎
        self.postgresql_engine = None
        if POSTGRESQL_SUPPORT:
            self.postgresql_engine = PostgreSQLMigrationEngine()
            self.postgresql_config = postgresql_config or self._get_default_postgresql_config()
        
        # 样式转换器
        self.style_converter = None
        if STYLE_CONVERSION_SUPPORT and style_conversion_enabled:
            self.style_converter = StyleConverter()
        
        # 性能优化器
        self.performance_optimizer = None
        if PERFORMANCE_OPTIMIZATION_SUPPORT and performance_optimization_enabled:
            self.performance_optimizer = PerformanceOptimizer()
    
    def _get_default_postgresql_config(self) -> "DatabaseConfig":
        """获取默认的PostgreSQL配置"""
        if POSTGRESQL_SUPPORT:
            from postgresql_migrator import DatabaseConfig
            return DatabaseConfig(
                host="localhost",
                port=5432,
                database="migrated_data",
                username="postgres",
                password="",  # 实际使用时应该从环境变量或配置文件中获取
                schema="public"
            )
        else:
            # 返回模拟配置字典
            return {
                "host": "localhost",
                "port": 5432,
                "database": "migrated_data",
                "username": "postgres",
                "password": "",
                "schema": "public",
                "ssl_mode": "disable"
            }
    
    def run(self, aprx_path: str, output_xml: Optional[str] = None, 
            convert_data: bool = False, output_format: str = "GPKG",
            verbose: bool = False, 
            migrate_postgresql: bool = False,
            convert_styles: bool = False) -> Dict[str, Any]:
        """
        运行增强版迁移流程
        
        Args:
            aprx_path: .aprx工程文件路径
            output_xml: 输出XML文件路径
            convert_data: 是否转换本地数据
            output_format: 数据输出格式
            verbose: 是否显示详细信息
            migrate_postgresql: 是否迁移PostgreSQL数据
            convert_styles: 是否转换样式
            
        Returns:
            迁移结果字典
        """
        # 启动性能监控（如果启用）
        if self.performance_optimizer:
            self.performance_optimizer.start_monitoring()
        
        # 先运行基础迁移流程
        results = super().run(
            aprx_path=aprx_path,
            output_xml=output_xml,
            convert_data=convert_data,
            output_format=output_format,
            verbose=verbose
        )
        
        if not results["success"]:
            # 基础流程失败，直接返回
            if self.performance_optimizer:
                self.performance_optimizer.stop_monitoring()
            return results
        
        try:
            # 解析工程文件（如果尚未解析）
            if "migration_project" not in results:
                migration_project = self.parser.parse_aprx(aprx_path)
                results["migration_project"] = migration_project
            
            migration_project = results["migration_project"]
            
            # 步骤4: 迁移PostgreSQL数据（如果启用）
            if migrate_postgresql and self.postgresql_engine:
                if verbose:
                    print(f"步骤4: 迁移PostgreSQL数据")
                
                postgresql_summary = self.postgresql_engine.migrate_project(
                    migration_project, self.postgresql_config
                )
                
                results["steps"]["postgresql_migration"] = {
                    "success": postgresql_summary.get('successful', 0) > 0,
                    "total_layers": postgresql_summary.get('total_layers', 0),
                    "successful_migrations": postgresql_summary.get('successful', 0),
                    "failed_migrations": postgresql_summary.get('failed', 0),
                    "total_rows_migrated": postgresql_summary.get('total_rows_migrated', 0),
                    "total_duration_seconds": postgresql_summary.get('total_duration_seconds', 0)
                }
                
                if verbose:
                    print(f"  PostgreSQL图层数: {postgresql_summary.get('total_layers', 0)}")
                    print(f"  成功迁移: {postgresql_summary.get('successful', 0)}")
                    print(f"  失败迁移: {postgresql_summary.get('failed', 0)}")
                    print(f"  迁移行数: {postgresql_summary.get('total_rows_migrated', 0):,}")
                    print(f"  迁移耗时: {postgresql_summary.get('total_duration_seconds', 0):.2f} 秒")
            
            # 步骤5: 转换样式（如果启用）
            if convert_styles and self.style_converter:
                if verbose:
                    print(f"步骤5: 转换样式")
                
                # 查找.stylx文件（通常在工程目录中）
                project_dir = os.path.dirname(aprx_path)
                stylx_files = []
                
                for root, dirs, files in os.walk(project_dir):
                    for file in files:
                        if file.endswith('.stylx'):
                            stylx_files.append(os.path.join(root, file))
                
                if stylx_files:
                    style_conversion_results = []
                    
                    for stylx_path in stylx_files:
                        if verbose:
                            print(f"  转换样式文件: {os.path.basename(stylx_path)}")
                        
                        try:
                            # 转换样式
                            base_name = os.path.splitext(os.path.basename(stylx_path))[0]
                            output_path = os.path.join(
                                project_dir, f"{base_name}_qgis.xml"
                            )
                            
                            style_library = self.style_converter.convert_stylx_to_qgis(
                                stylx_path, output_path
                            )
                            
                            style_conversion_results.append({
                                "source_file": stylx_path,
                                "output_file": output_path,
                                "symbol_count": style_library.symbol_count,
                                "success": True
                            })
                            
                        except Exception as e:
                            style_conversion_results.append({
                                "source_file": stylx_path,
                                "success": False,
                                "error": str(e)
                            })
                    
                    results["steps"]["style_conversion"] = {
                        "success": any(r["success"] for r in style_conversion_results),
                        "total_files": len(stylx_files),
                        "successful_conversions": sum(1 for r in style_conversion_results if r["success"]),
                        "failed_conversions": sum(1 for r in style_conversion_results if not r["success"]),
                        "results": style_conversion_results
                    }
                    
                    if verbose:
                        successful = sum(1 for r in style_conversion_results if r["success"])
                        print(f"  样式文件数: {len(stylx_files)}")
                        print(f"  成功转换: {successful}")
                        print(f"  失败转换: {len(stylx_files) - successful}")
                else:
                    results["steps"]["style_conversion"] = {
                        "success": True,
                        "message": "未找到.stylx样式文件"
                    }
                    
                    if verbose:
                        print(f"  未找到.stylx样式文件")
            
            # 获取性能报告
            if self.performance_optimizer:
                performance_report = self.performance_optimizer.get_performance_report()
                results["performance_report"] = performance_report
                
                if verbose:
                    monitor_summary = performance_report.get('monitoring', {})
                    latest = monitor_summary.get('latest', {})
                    print(f"\n性能报告:")
                    print(f"  CPU使用率: {latest.get('cpu_percent', 0):.1f}%")
                    print(f"  内存使用率: {latest.get('memory_percent', 0):.1f}%")
                    print(f"  活动线程数: {latest.get('active_threads', 0)}")
        
        except Exception as e:
            results["success"] = False
            results["errors"].append(str(e))
            
            if verbose:
                print(f"\n增强功能失败: {e}")
        
        finally:
            # 停止性能监控
            if self.performance_optimizer:
                self.performance_optimizer.stop_monitoring()
        
        return results
    
    def generate_enhanced_report(self, results: Dict[str, Any], report_file: Optional[str] = None) -> str:
        """
        生成增强版迁移报告
        
        Args:
            results: 迁移结果字典（包含第二阶段功能）
            report_file: 报告文件路径
            
        Returns:
            报告内容
        """
        # 生成基础报告
        report_lines = super().generate_report(results, None).split('\n')
        
        # 添加第二阶段功能信息
        enhanced_lines = []
        
        # 查找总体状态部分
        for i, line in enumerate(report_lines):
            enhanced_lines.append(line)
            
            if line.startswith("总体状态:"):
                # 在后面添加第二阶段功能状态
                enhanced_lines.append("")
                enhanced_lines.append("第二阶段功能状态:")
                enhanced_lines.append("-" * 40)
                
                # PostgreSQL迁移状态
                if "postgresql_migration" in results.get("steps", {}):
                    pg_step = results["steps"]["postgresql_migration"]
                    status = "✅ 完成" if pg_step.get("success", False) else "❌ 失败"
                    enhanced_lines.append(f"PostgreSQL数据迁移: {status}")
                    if pg_step.get("success", False):
                        enhanced_lines.append(f"  迁移图层: {pg_step.get('successful_migrations', 0)}/{pg_step.get('total_layers', 0)}")
                        enhanced_lines.append(f"  迁移行数: {pg_step.get('total_rows_migrated', 0):,}")
                        enhanced_lines.append(f"  迁移耗时: {pg_step.get('total_duration_seconds', 0):.2f} 秒")
                
                # 样式转换状态
                if "style_conversion" in results.get("steps", {}):
                    style_step = results["steps"]["style_conversion"]
                    status = "✅ 完成" if style_step.get("success", False) else "❌ 失败"
                    enhanced_lines.append(f"样式转换: {status}")
                    if style_step.get("success", False):
                        enhanced_lines.append(f"  转换文件: {style_step.get('successful_conversions', 0)}/{style_step.get('total_files', 0)}")
                
                # 性能优化状态
                if "performance_report" in results:
                    perf_report = results.get("performance_report", {})
                    enhanced_lines.append(f"性能优化: ✅ 已监控")
                    monitor_summary = perf_report.get('monitoring', {})
                    averages = monitor_summary.get('averages', {})
                    enhanced_lines.append(f"  平均CPU: {averages.get('cpu_percent', 0):.1f}%")
                    enhanced_lines.append(f"  平均内存: {averages.get('memory_percent', 0):.1f}%")
                
                enhanced_lines.append("")
        
        # 更新总结部分
        for i in range(len(enhanced_lines)):
            if enhanced_lines[i].startswith("迁移工具 - 第一阶段功能实现"):
                # 替换为两阶段功能实现
                enhanced_lines[i] = "=" * 60
                enhanced_lines.insert(i + 1, "迁移工具 - 两阶段功能实现")
                enhanced_lines.insert(i + 2, "第一阶段功能:")
                enhanced_lines.insert(i + 3, "1. 工程解析模块 ✓")
                enhanced_lines.insert(i + 4, "2. XML生成模块（符合XSD 1.0） ✓")
                enhanced_lines.insert(i + 5, "3. 本地数据转换基础 ✓")
                enhanced_lines.insert(i + 6, "")
                enhanced_lines.insert(i + 7, "第二阶段功能:")
                enhanced_lines.insert(i + 8, "4. PostgreSQL大表迁移 ✓" if "postgresql_migration" in results.get("steps", {}) else "4. PostgreSQL大表迁移 (未启用)")
                enhanced_lines.insert(i + 9, "5. 样式转换映射 ✓" if "style_conversion" in results.get("steps", {}) else "5. 样式转换映射 (未启用)")
                enhanced_lines.insert(i + 10, "6. 性能优化框架 ✓" if "performance_report" in results else "6. 性能优化框架 (未启用)")
                enhanced_lines.insert(i + 11, "=" * 60)
                break
        
        report_content = "\n".join(enhanced_lines)
        
        # 保存报告
        if report_file:
            os.makedirs(os.path.dirname(report_file), exist_ok=True)
            with open(report_file, 'w', encoding='utf-8') as f:
                f.write(report_content)
        
        return report_content
    
    def cleanup(self):
        """清理资源"""
        if self.postgresql_engine:
            self.postgresql_engine.cleanup()
        
        if self.performance_optimizer:
            self.performance_optimizer.stop_monitoring()


def main():
    """命令行入口点"""
    parser = argparse.ArgumentParser(
        description='ArcGIS Pro工程迁移工具 - 第一阶段功能实现',
        epilog='''
示例:
  python migration_tool.py example.aprx
  python migration_tool.py example.aprx --output migration.xml --convert-data
  python migration_tool.py example.aprx --verbose --report migration_report.txt
        '''
    )
    
    parser.add_argument(
        'input_aprx',
        help='输入 .aprx 工程文件路径'
    )
    
    parser.add_argument(
        '-o', '--output',
        help='输出 XML 文件路径（默认：与输入文件同目录，扩展名为 _migration.xml）',
        default=None
    )
    
    parser.add_argument(
        '-c', '--convert-data',
        help='转换本地数据（Shapefile/FileGDB 到 GeoPackage）',
        action='store_true'
    )
    
    parser.add_argument(
        '-f', '--format',
        help='数据输出格式（默认: GPKG）',
        choices=['GPKG', 'SHP'],
        default='GPKG'
    )
    
    parser.add_argument(
        '-r', '--report',
        help='生成迁移报告文件',
        default=None
    )
    
    parser.add_argument(
        '-v', '--verbose',
        help='显示详细输出信息',
        action='store_true'
    )
    
    args = parser.parse_args()
    
    # 检查输入文件是否存在
    if not os.path.exists(args.input_aprx):
        print(f"错误: 输入文件不存在: {args.input_aprx}")
        sys.exit(1)
    
    # 创建迁移工具实例
    tool = MigrationTool()
    
    # 运行迁移
    print(f"ArcGIS Pro工程迁移工具 - 第一阶段功能")
    print(f"工程文件: {args.input_aprx}")
    print(f"开始时间: {datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
    print("-" * 60)
    
    results = tool.run(
        aprx_path=args.input_aprx,
        output_xml=args.output,
        convert_data=args.convert_data,
        output_format=args.format,
        verbose=args.verbose
    )
    
    # 生成报告
    report_content = tool.generate_report(results, args.report)
    
    # 显示报告摘要
    if not args.verbose:
        print("\n迁移摘要:")
        print(f"  总体状态: {'成功' if results['success'] else '失败'}")
        
        for step_name, step_result in results.get("steps", {}).items():
            step_name_display = step_name.replace('_', ' ').title()
            status = "完成" if step_result.get("success", False) else "失败"
            print(f"  {step_name_display}: {status}")
        
        if args.output:
            print(f"  XML文件: {args.output}")
        
        if args.convert_data:
            print(f"  数据输出目录: migration_output")
        
        if args.report:
            print(f"  报告文件: {args.report}")
    
    print("-" * 60)
    print(f"完成时间: {datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
    
    # 返回适当的退出代码
    sys.exit(0 if results['success'] else 1)


if __name__ == '__main__':
    main()