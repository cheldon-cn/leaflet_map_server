#!/usr/bin/env python3
"""
local_data_converter 单元测试
"""

import unittest
import sys
import os
import tempfile
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from local_data_converter import LocalDataConverter
from data_models import LayerInfo, DataSourceInfo

class TestLocalDataConverter(unittest.TestCase):
    def setUp(self):
        """设置测试环境"""
        self.test_output_dir = tempfile.mkdtemp()
        self.converter = LocalDataConverter(output_dir=self.test_output_dir)
    
    def tearDown(self):
        """清理测试环境"""
        import shutil
        if os.path.exists(self.test_output_dir):
            shutil.rmtree(self.test_output_dir)
    
    def test_initialization(self):
        """测试转换器初始化"""
        self.assertIsInstance(self.converter, LocalDataConverter)
        self.assertEqual(self.converter.output_dir, self.test_output_dir)
        
        # GDAL可用性检查应该执行
        self.assertIn('gdal_available', self.converter.__dict__)
        
        # 输出目录应该存在
        self.assertTrue(os.path.exists(self.converter.output_dir))
    
    def test_convert_layer_shapefile_mock(self):
        """测试Shapefile图层转换（模拟模式）"""
        # 创建Shapefile图层
        layer = LayerInfo(
            name="Test Shapefile",
            layer_type="FeatureLayer",
            data_source=DataSourceInfo(
                data_source_type="Shapefile",
                file_path=r"C:\data\test.shp"
            )
        )
        
        # 转换图层（模拟模式）
        result = self.converter.convert_layer(layer, output_format="GPKG")
        
        # 验证结果结构
        self.assertIsInstance(result, dict)
        self.assertIn('status', result)
        self.assertIn('output_path', result)
        self.assertIn('message', result)
        
        # 由于GDAL可能不可用，状态可能是"simulated"或"completed"
        self.assertIn(result['status'], ['simulated', 'completed', 'failed'])
        
        if result['status'] == 'simulated':
            # 模拟模式下，输出路径应该存在
            self.assertTrue(result['output_path'].endswith('.gpkg'))
    
    def test_convert_layer_filegdb_mock(self):
        """测试FileGDB图层转换（模拟模式）"""
        # 创建FileGDB图层
        layer = LayerInfo(
            name="Test FileGDB",
            layer_type="FeatureLayer",
            data_source=DataSourceInfo(
                data_source_type="FileGeodatabase",
                file_path=r"C:\data\test.gdb\featureclass"
            )
        )
        
        result = self.converter.convert_layer(layer, output_format="GPKG")
        
        self.assertIsInstance(result, dict)
        self.assertIn('status', result)
        
        # 验证输出路径格式
        if 'output_path' in result and result['output_path']:
            self.assertTrue(result['output_path'].endswith('.gpkg'))
    
    def test_convert_layer_postgresql(self):
        """测试PostgreSQL图层转换（应该跳过）"""
        # PostgreSQL图层不应该被本地转换器处理
        layer = LayerInfo(
            name="Test PostgreSQL",
            layer_type="FeatureLayer",
            data_source=DataSourceInfo(
                data_source_type="PostgreSQL",
                connection_string="host=localhost dbname=test"
            )
        )
        
        result = self.converter.convert_layer(layer, output_format="GPKG")
        
        # 应该返回跳过状态
        self.assertEqual(result['status'], 'skipped')
        self.assertIn('PostgreSQL', result['message'])
    
    def test_batch_convert(self):
        """测试批量转换"""
        layers = [
            LayerInfo(
                name=f"Layer{i}",
                data_source=DataSourceInfo(
                    data_source_type="Shapefile",
                    file_path=rf"C:\data\layer{i}.shp"
                )
            )
            for i in range(3)
        ]
        
        results = self.converter.batch_convert(layers, output_format="GPKG")
        
        # 验证结果
        self.assertIsInstance(results, dict)
        self.assertIn('total', results)
        self.assertIn('successful', results)
        self.assertIn('failed', results)
        self.assertIn('skipped', results)
        self.assertIn('results', results)
        
        # 应该有三个结果
        self.assertEqual(results['total'], 3)
        self.assertEqual(len(results['results']), 3)
        
        # 验证每个结果
        for i, result in enumerate(results['results']):
            self.assertIn('layer_name', result)
            self.assertIn('status', result)
            self.assertEqual(result['layer_name'], f"Layer{i}")
    
    def test_convert_with_nonexistent_source(self):
        """测试不存在的源文件转换"""
        layer = LayerInfo(
            name="Non-existent",
            data_source=DataSourceInfo(
                data_source_type="Shapefile",
                file_path=r"C:\non_existent\file.shp"
            )
        )
        
        result = self.converter.convert_layer(layer, output_format="GPKG")
        
        # 应该失败或模拟
        self.assertIn(result['status'], ['failed', 'simulated'])
        
        if result['status'] == 'failed':
            self.assertIn('error', result)
    
    def test_output_directory_creation(self):
        """测试输出目录创建"""
        # 使用不存在的输出目录
        new_output_dir = os.path.join(self.test_output_dir, "subdir", "deep")
        converter = LocalDataConverter(output_dir=new_output_dir)
        
        # 目录应该被创建
        self.assertTrue(os.path.exists(new_output_dir))
        
        # 清理
        import shutil
        shutil.rmtree(new_output_dir, ignore_errors=True)
    
    def test_supported_formats(self):
        """测试支持的输出格式"""
        layer = LayerInfo(
            name="Test",
            data_source=DataSourceInfo(
                data_source_type="Shapefile",
                file_path=r"C:\data\test.shp"
            )
        )
        
        # 测试支持的格式
        supported_formats = ["GPKG", "SHP", "GeoJSON", "KML"]
        
        for fmt in supported_formats:
            result = self.converter.convert_layer(layer, output_format=fmt)
            self.assertIsInstance(result, dict)
            self.assertIn('status', result)
            
            if 'output_path' in result and result['output_path']:
                # 验证文件扩展名
                if fmt == "GPKG":
                    self.assertTrue(result['output_path'].endswith('.gpkg'))
                elif fmt == "SHP":
                    self.assertTrue(result['output_path'].endswith('.shp'))
                elif fmt == "GeoJSON":
                    self.assertTrue(result['output_path'].endswith('.geojson'))
                elif fmt == "KML":
                    self.assertTrue(result['output_path'].endswith('.kml'))
    
    def test_unsupported_format(self):
        """测试不支持的输出格式"""
        layer = LayerInfo(
            name="Test",
            data_source=DataSourceInfo(
                data_source_type="Shapefile",
                file_path=r"C:\data\test.shp"
            )
        )
        
        # 不支持的格式应该引发错误或返回失败
        result = self.converter.convert_layer(layer, output_format="UNSUPPORTED")
        
        # 应该返回失败状态
        self.assertEqual(result['status'], 'failed')
        self.assertIn('不支持', result['message'])

if __name__ == '__main__':
    unittest.main()