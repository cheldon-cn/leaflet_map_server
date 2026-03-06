#!/usr/bin/env python3
"""
enhanced_aprx_parser 单元测试
"""

import unittest
import sys
import os
import tempfile
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from enhanced_aprx_parser import EnhancedAPRXParser
from data_models import MigrationProject

class TestEnhancedAPRXParser(unittest.TestCase):
    def setUp(self):
        """设置测试环境"""
        self.parser_with_arcpy = EnhancedAPRXParser(use_arcpy=True)
        self.parser_without_arcpy = EnhancedAPRXParser(use_arcpy=False)
    
    def test_initialization(self):
        """测试解析器初始化"""
        # 测试use_arcpy=True的情况（arcpy可能不可用）
        self.assertIsInstance(self.parser_with_arcpy, EnhancedAPRXParser)
        self.assertEqual(self.parser_with_arcpy.use_arcpy, True)
        
        # 测试use_arcpy=False的情况
        self.assertIsInstance(self.parser_without_arcpy, EnhancedAPRXParser)
        self.assertEqual(self.parser_without_arcpy.use_arcpy, False)
        
        # arc_available属性应该被设置
        self.assertIn('arc_available', self.parser_with_arcpy.__dict__)
        self.assertIn('arc_available', self.parser_without_arcpy.__dict__)
    
    def test_parse_aprx_without_arcpy(self):
        """测试不使用arcpy解析（模拟模式）"""
        test_aprx = r"C:\test_project.aprx"
        
        # 解析应该成功（返回模拟数据）
        project = self.parser_without_arcpy.parse_aprx(test_aprx)
        
        # 验证返回的是MigrationProject对象
        self.assertIsInstance(project, MigrationProject)
        
        # 验证基本属性
        self.assertEqual(project.version, "1.0")
        # source_arcgis_version 可能是版本号如 "3.2"
        self.assertIsInstance(project.source_arcgis_version, str)
        self.assertEqual(project.target_qgis_version, "3.34")
        
        # 验证元数据
        self.assertIsNotNone(project.metadata)
        self.assertEqual(project.metadata.project_name, "Test Project: test_project")
        self.assertEqual(project.metadata.original_project_path, test_aprx)
        
        # 验证地图数据（模拟数据应该包含地图）
        self.assertGreater(len(project.maps), 0)
        
        # 验证地图结构
        for map_info in project.maps:
            self.assertIsNotNone(map_info.name)
            self.assertIsNotNone(map_info.map_id)
            self.assertIsInstance(map_info.layers, list)
            
            # 模拟数据中的图层应该包含基本字段
            for layer in map_info.layers:
                self.assertIsNotNone(layer.name)
                self.assertIsNotNone(layer.layer_id)
                self.assertIsNotNone(layer.layer_type)
                self.assertIsNotNone(layer.data_source)
    
    def test_parse_nonexistent_file_without_arcpy(self):
        """测试不使用arcpy解析不存在的文件（应该成功，因为使用模拟数据）"""
        with tempfile.TemporaryDirectory() as tmpdir:
            non_existent = os.path.join(tmpdir, "does_not_exist.aprx")
            
            # 应该成功（模拟数据）
            project = self.parser_without_arcpy.parse_aprx(non_existent)
            self.assertIsInstance(project, MigrationProject)
            self.assertEqual(project.metadata.original_project_path, non_existent)
    
    def test_parse_with_mock_data_structure(self):
        """测试模拟数据的结构完整性"""
        test_aprx = r"C:\sample.aprx"
        project = self.parser_without_arcpy.parse_aprx(test_aprx)
        
        # 验证所有图层都有数据源
        all_layers = project.get_all_layers()
        self.assertGreater(len(all_layers), 0)
        
        for layer in all_layers:
            self.assertIsNotNone(layer.data_source)
            self.assertIsNotNone(layer.data_source.data_source_type)
            
            # 根据数据源类型验证相应字段
            if layer.data_source.data_source_type == "Shapefile":
                self.assertIsNotNone(layer.data_source.file_path)
                self.assertIn(".shp", layer.data_source.file_path)
            elif layer.data_source.data_source_type == "FileGeodatabase":
                self.assertIsNotNone(layer.data_source.file_path)
                self.assertIn(".gdb", layer.data_source.file_path)
            elif layer.data_source.data_source_type == "PostgreSQL":
                self.assertIsNotNone(layer.data_source.connection_string)
                self.assertIsNotNone(layer.data_source.database_info)
    
    def test_parse_error_handling(self):
        """测试错误处理"""
        # 测试空路径（应该成功，因为使用模拟数据）
        project = self.parser_without_arcpy.parse_aprx("")
        self.assertIsInstance(project, MigrationProject)
        
        # 测试None路径（应该引发TypeError）
        with self.assertRaises(TypeError):
            self.parser_without_arcpy.parse_aprx(None)

if __name__ == '__main__':
    unittest.main()