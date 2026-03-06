#!/usr/bin/env python3
"""
aprx_parser 单元测试
"""

import unittest
import sys
import os
import tempfile
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from aprx_parser import parse_aprx, generate_xml

class TestAPRXParser(unittest.TestCase):
    def test_parse_aprx_with_mock(self):
        """测试模拟数据解析（当arcpy不可用时）"""
        # 创建一个临时.aprx文件（内容无关紧要，因为arcpy不可用）
        with tempfile.NamedTemporaryFile(suffix='.aprx', delete=False) as f:
            test_aprx = f.name
        
        try:
            # 解析应该成功（返回模拟数据）
            data = parse_aprx(test_aprx)
            
            # 验证数据结构
            self.assertIn('project_path', data)
            self.assertIn('project_name', data)
            self.assertIn('maps', data)
            self.assertIsInstance(data['maps'], list)
            
            # 验证模拟数据内容
            self.assertEqual(data['project_path'], test_aprx)
            self.assertEqual(data['project_name'], os.path.splitext(os.path.basename(test_aprx))[0])
            
            # 应该至少有一个地图
            self.assertGreater(len(data['maps']), 0)
            
            # 验证地图结构
            for map_info in data['maps']:
                self.assertIn('name', map_info)
                self.assertIn('layers', map_info)
                self.assertIsInstance(map_info['layers'], list)
                
                # 验证图层结构
                for layer_info in map_info['layers']:
                    self.assertIn('name', layer_info)
                    self.assertIn('data_source', layer_info)
                    self.assertIn('layer_type', layer_info)
        finally:
            if os.path.exists(test_aprx):
                os.unlink(test_aprx)
    
    def test_parse_nonexistent_file(self):
        """测试不存在的文件（应引发FileNotFoundError）"""
        # 模拟arcpy可用但文件不存在的情况
        # 由于arcpy不可用，我们将使用模拟数据路径
        # 但parse_aprx会首先检查文件是否存在
        with tempfile.TemporaryDirectory() as tmpdir:
            non_existent = os.path.join(tmpdir, "does_not_exist.aprx")
            # 文件不存在，应该引发FileNotFoundError
            with self.assertRaises(FileNotFoundError):
                parse_aprx(non_existent)
    
    def test_generate_xml(self):
        """测试XML生成"""
        test_data = {
            'project_path': r'C:\test.aprx',
            'project_name': 'TestProject',
            'maps': [
                {
                    'name': 'Map1',
                    'layers': [
                        {'name': 'Roads', 'data_source': r'C:\data\roads.shp', 'layer_type': 'FeatureLayer'},
                        {'name': 'Buildings', 'data_source': r'C:\data\buildings.gdb', 'layer_type': 'FeatureLayer'}
                    ]
                },
                {
                    'name': 'Map2',
                    'layers': [
                        {'name': 'Rivers', 'data_source': r'C:\data\rivers.shp', 'layer_type': 'FeatureLayer'}
                    ]
                }
            ]
        }
        
        # 生成XML字符串
        xml_str = generate_xml(test_data)
        
        # 验证XML包含关键元素
        self.assertIn('<ArcGISProject', xml_str)
        self.assertIn('name="TestProject"', xml_str)
        self.assertIn('<Map', xml_str)
        self.assertIn('<Layer', xml_str)
        self.assertIn('Roads', xml_str)
        self.assertIn('Buildings', xml_str)
        self.assertIn('Rivers', xml_str)
        
        # 验证XML结构（基本检查）
        self.assertTrue(xml_str.startswith('<?xml'))
        
        # 测试保存到文件
        with tempfile.NamedTemporaryFile(mode='w', suffix='.xml', delete=False) as f:
            xml_file = f.name
        
        try:
            # 生成XML并保存
            returned_xml = generate_xml(test_data, xml_file)
            
            # 验证文件存在且内容正确
            self.assertTrue(os.path.exists(xml_file))
            with open(xml_file, 'r', encoding='utf-8') as f:
                file_content = f.read()
                self.assertIn('TestProject', file_content)
                self.assertIn('Roads', file_content)
            
            # 验证返回的XML与文件内容相同
            self.assertEqual(returned_xml, file_content)
        finally:
            if os.path.exists(xml_file):
                os.unlink(xml_file)
    
    def test_generate_xml_invalid_data(self):
        """测试无效数据输入"""
        # 缺少'maps'键
        invalid_data = {
            'project_path': r'C:\test.aprx',
            'project_name': 'TestProject'
        }
        
        with self.assertRaises(ValueError):
            generate_xml(invalid_data)

if __name__ == '__main__':
    unittest.main()