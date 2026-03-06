#!/usr/bin/env python3
"""
migration_tool 单元测试
"""

import unittest
import sys
import os
import tempfile
import json
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from migration_tool import MigrationTool, EnhancedMigrationTool
from data_models import MigrationProject

class TestMigrationTool(unittest.TestCase):
    def setUp(self):
        """设置测试环境"""
        self.test_output_dir = tempfile.mkdtemp()
        self.tool = MigrationTool()
    
    def tearDown(self):
        """清理测试环境"""
        import shutil
        if os.path.exists(self.test_output_dir):
            shutil.rmtree(self.test_output_dir)
    
    def test_initialization(self):
        """测试工具初始化"""
        self.assertIsInstance(self.tool, MigrationTool)
        
        # 验证关键属性存在
        self.assertIsNotNone(self.tool.parser)
        self.assertIsNotNone(self.tool.xml_generator)
        self.assertIsNotNone(self.tool.data_converter)
        
        # 验证解析器配置
        self.assertTrue(self.tool.parser.use_arcpy)
    
    def test_run_with_mock_data(self):
        """测试使用模拟数据运行迁移流程"""
        # 创建一个测试.aprx文件（实际不存在，但会触发模拟数据）
        with tempfile.NamedTemporaryFile(suffix='.aprx', delete=False) as f:
            test_aprx = f.name
        
        try:
            # 运行迁移工具
            results = self.tool.run(
                aprx_path=test_aprx,
                output_xml=None,  # 不输出XML文件
                convert_data=False,
                verbose=False
            )
            
            # 验证结果结构
            self.assertIsInstance(results, dict)
            self.assertIn('success', results)
            self.assertIn('message', results)
            self.assertIn('timestamp', results)
            
            # 由于使用模拟数据，应该成功
            self.assertTrue(results['success'])
            
            # 验证返回的迁移项目
            if 'migration_project' in results:
                project = results['migration_project']
                self.assertIsInstance(project, MigrationProject)
                
                # 验证基本属性
                self.assertEqual(project.version, "1.0")
                self.assertIsInstance(project.source_arcgis_version, str)
                
                # 验证模拟数据中的地图
                self.assertGreater(len(project.maps), 0)
        
        finally:
            if os.path.exists(test_aprx):
                os.unlink(test_aprx)
    
    def test_run_with_xml_output(self):
        """测试生成XML输出"""
        with tempfile.NamedTemporaryFile(suffix='.aprx', delete=False) as aprx_file:
            test_aprx = aprx_file.name
        
        with tempfile.NamedTemporaryFile(suffix='.xml', delete=False) as xml_file:
            test_xml = xml_file.name
        
        try:
            # 运行迁移工具并生成XML
            results = self.tool.run(
                aprx_path=test_aprx,
                output_xml=test_xml,
                convert_data=False,
                verbose=False
            )
            
            # 应该成功
            self.assertTrue(results['success'])
            
            # XML文件应该存在
            self.assertTrue(os.path.exists(test_xml))
            
            # 验证XML内容
            with open(test_xml, 'r', encoding='utf-8') as f:
                xml_content = f.read()
                self.assertIn('<?xml', xml_content)
                self.assertIn('<MigrationProject', xml_content)
                self.assertIn('Test Project:', xml_content)
        
        finally:
            for file_path in [test_aprx, test_xml]:
                if os.path.exists(file_path):
                    os.unlink(file_path)
    
    def test_run_with_data_conversion_mock(self):
        """测试数据转换（模拟模式）"""
        with tempfile.NamedTemporaryFile(suffix='.aprx', delete=False) as f:
            test_aprx = f.name
        
        try:
            # 运行迁移工具并启用数据转换
            results = self.tool.run(
                aprx_path=test_aprx,
                convert_data=True,
                output_format="GPKG",
                verbose=False
            )
            
            # 应该成功（模拟模式）
            self.assertTrue(results['success'])
            
            # 验证数据转换结果
            if 'data_conversion_results' in results:
                conv_results = results['data_conversion_results']
                self.assertIsInstance(conv_results, dict)
                self.assertIn('total_layers', conv_results)
        
        finally:
            if os.path.exists(test_aprx):
                os.unlink(test_aprx)
    
    def test_generate_report(self):
        """测试报告生成"""
        with tempfile.NamedTemporaryFile(suffix='.aprx', delete=False) as f:
            test_aprx = f.name
        
        with tempfile.NamedTemporaryFile(suffix='.txt', delete=False) as report_file:
            test_report = report_file.name
        
        try:
            # 运行迁移工具
            results = self.tool.run(
                aprx_path=test_aprx,
                convert_data=False,
                verbose=False
            )
            
            # 生成报告
            report_content = self.tool.generate_report(results, test_report)
            
            # 报告内容非空
            self.assertIsNotNone(report_content)
            self.assertGreater(len(report_content), 0)
            
            # 报告文件应该存在
            self.assertTrue(os.path.exists(test_report))
            
            # 验证报告内容
            with open(test_report, 'r', encoding='utf-8') as f:
                file_content = f.read()
                self.assertIn('迁移报告', file_content)
                self.assertIn('工程信息', file_content)
                self.assertIn('状态', file_content)
        
        finally:
            for file_path in [test_aprx, test_report]:
                if os.path.exists(file_path):
                    os.unlink(file_path)
    
    def test_run_with_nonexistent_file(self):
        """测试不存在的工程文件"""
        import tempfile
        non_existent = r"C:\non_existent_file.aprx"
        
        # 创建临时XML输出文件，避免权限问题
        with tempfile.NamedTemporaryFile(suffix='.xml', delete=False) as f:
            xml_output = f.name
        
        try:
            results = self.tool.run(
                aprx_path=non_existent,
                output_xml=xml_output,
                convert_data=False,
                verbose=False
            )
            
            # 模拟数据模式下应该成功（即使文件不存在）
            if not results['success']:
                print("Errors:", results.get('errors'))
                print("Message:", results.get('message'))
            self.assertTrue(results['success'])
            # 消息可能包含模拟相关的信息
            self.assertIsInstance(results['message'], str)
        finally:
            if os.path.exists(xml_output):
                os.unlink(xml_output)
    
    def test_verbose_mode(self):
        """测试详细模式"""
        with tempfile.NamedTemporaryFile(suffix='.aprx', delete=False) as f:
            test_aprx = f.name
        
        try:
            # 运行迁移工具并启用详细模式
            results = self.tool.run(
                aprx_path=test_aprx,
                convert_data=False,
                verbose=True
            )
            
            # 应该成功
            self.assertTrue(results['success'])
            
            # 详细模式应该包含步骤信息
            self.assertIn('steps', results)
            self.assertGreater(len(results['steps']), 0)
        
        finally:
            if os.path.exists(test_aprx):
                os.unlink(test_aprx)

class TestEnhancedMigrationTool(unittest.TestCase):
    def setUp(self):
        """设置测试环境"""
        self.test_output_dir = tempfile.mkdtemp()
        
        # 创建增强版工具（第二阶段功能可能不可用）
        self.tool = EnhancedMigrationTool(
            postgresql_config=None,
            style_conversion_enabled=False,
            performance_optimization_enabled=False
        )
    
    def tearDown(self):
        """清理测试环境"""
        import shutil
        if os.path.exists(self.test_output_dir):
            shutil.rmtree(self.test_output_dir)
    
    def test_initialization(self):
        """测试增强版工具初始化"""
        self.assertIsInstance(self.tool, EnhancedMigrationTool)
        self.assertIsInstance(self.tool, MigrationTool)  # 应该继承自MigrationTool
        
        # 验证第二阶段功能属性存在（即使不可用）
        self.assertIn('postgresql_engine', self.tool.__dict__)
        self.assertIn('style_converter', self.tool.__dict__)
        self.assertIn('performance_optimizer', self.tool.__dict__)
    
    def test_run_basic_functionality(self):
        """测试增强版工具基础功能（不启用第二阶段功能）"""
        with tempfile.NamedTemporaryFile(suffix='.aprx', delete=False) as f:
            test_aprx = f.name
        
        try:
            # 运行基础迁移流程（不启用第二阶段功能）
            results = self.tool.run(
                aprx_path=test_aprx,
                migrate_postgresql=False,
                convert_styles=False,
                verbose=False
            )
            
            # 应该成功
            self.assertTrue(results['success'])
        
        finally:
            if os.path.exists(test_aprx):
                os.unlink(test_aprx)
    
    def test_get_default_postgresql_config(self):
        """测试获取默认PostgreSQL配置"""
        # 这个方法应该存在，但可能返回模拟数据
        config = self.tool._get_default_postgresql_config()
        
        # 验证配置结构
        self.assertIsInstance(config, dict)
        self.assertIn('host', config)
        self.assertIn('port', config)
        self.assertIn('database', config)
        self.assertIn('username', config)

if __name__ == '__main__':
    unittest.main()