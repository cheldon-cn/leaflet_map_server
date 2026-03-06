#!/usr/bin/env python3
"""
migration_xml_generator 单元测试
"""

import unittest
import sys
import os
import tempfile
import xml.etree.ElementTree as ET
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from migration_xml_generator import MigrationXMLGenerator
from data_models import (
    MigrationProject, ProjectMetadata, MapInfo, LayerInfo,
    DataSourceInfo, CoordinateSystem, FieldInfo, Extent
)

class TestMigrationXMLGenerator(unittest.TestCase):
    def setUp(self):
        """设置测试环境"""
        self.generator = MigrationXMLGenerator(validate_xsd=False)
        
        # 创建测试迁移项目
        self.test_project = MigrationProject(
            version="1.0",
            source_arcgis_version="3.2",
            target_qgis_version="3.34"
        )
        
        # 设置元数据
        self.test_project.metadata = ProjectMetadata(
            project_name="Test Project",
            original_project_path=r"C:\test.aprx",
            author="Test Author",
            description="Test migration project"
        )
        
        # 添加坐标系
        self.test_project.metadata.coordinate_system = CoordinateSystem(
            epsg=4326,
            name="WGS84",
            wkt='GEOGCS["WGS 84",DATUM["WGS_1984",SPHEROID["WGS 84",6378137,298.257223563]],PRIMEM["Greenwich",0],UNIT["degree",0.0174532925199433]]'
        )
        
        # 添加地图
        map_info = MapInfo(
            name="Test Map",
            map_id="map1",
            coordinate_system=CoordinateSystem(epsg=3857, name="Web Mercator")
        )
        
        # 添加图层
        layer = LayerInfo(
            layer_id="layer1",
            name="Test Layer",
            layer_type="FeatureLayer",
            data_source=DataSourceInfo(
                data_source_type="Shapefile",
                file_path=r"C:\data\test.shp"
            ),
            coordinate_system=CoordinateSystem(epsg=4326, name="WGS84"),
            fields=[
                FieldInfo(name="id", field_type="Integer", length=10),
                FieldInfo(name="name", field_type="String", length=50)
            ],
            extent=Extent(xmin=0, ymin=0, xmax=10, ymax=10)
        )
        
        map_info.layers.append(layer)
        self.test_project.maps.append(map_info)
    
    def test_generate_xml_string(self):
        """测试生成XML字符串"""
        xml_str = self.generator.generate_xml(self.test_project)
        
        # 验证XML字符串非空
        self.assertIsNotNone(xml_str)
        self.assertGreater(len(xml_str), 0)
        
        # 验证XML格式正确
        try:
            root = ET.fromstring(xml_str)
        except ET.ParseError as e:
            self.fail(f"生成的XML无法解析: {e}")
        
        # 验证根元素
        self.assertEqual(root.tag, 'MigrationProject')
        self.assertEqual(root.get('version'), '1.0')
        self.assertEqual(root.get('sourceArcGISVersion'), '3.2')
        self.assertEqual(root.get('targetQGISVersion'), '3.34')
        
        # 验证元数据
        metadata_elem = root.find('ProjectMetadata')
        self.assertIsNotNone(metadata_elem)
        self.assertEqual(metadata_elem.findtext('ProjectName'), 'Test Project')
        self.assertEqual(metadata_elem.findtext('Author'), 'Test Author')
    
    def test_generate_xml_file(self):
        """测试生成XML文件"""
        with tempfile.NamedTemporaryFile(mode='w', suffix='.xml', delete=False) as f:
            xml_file = f.name
        
        try:
            # 生成XML文件
            returned_xml = self.generator.generate_xml(self.test_project, xml_file)
            
            # 验证文件存在
            self.assertTrue(os.path.exists(xml_file))
            
            # 验证文件内容与返回的XML相同
            with open(xml_file, 'r', encoding='utf-8') as f:
                file_content = f.read()
                self.assertEqual(returned_xml, file_content)
            
            # 验证文件内容可解析
            tree = ET.parse(xml_file)
            root = tree.getroot()
            self.assertEqual(root.tag, 'MigrationProject')
            
        finally:
            if os.path.exists(xml_file):
                os.unlink(xml_file)
    
    def test_generate_xml_empty_project(self):
        """测试生成空项目的XML"""
        empty_project = MigrationProject()
        
        xml_str = self.generator.generate_xml(empty_project)
        
        # 应该成功生成XML
        self.assertIsNotNone(xml_str)
        
        # 验证基本结构
        root = ET.fromstring(xml_str)
        self.assertEqual(root.tag, 'MigrationProject')
        self.assertEqual(root.get('version'), '1.0')
        self.assertEqual(root.get('targetQGISVersion'), '3.34')
        
        # 应该包含元数据元素
        metadata_elem = root.find('ProjectMetadata')
        self.assertIsNotNone(metadata_elem)
    
    def test_xml_structure_completeness(self):
        """测试XML结构的完整性"""
        xml_str = self.generator.generate_xml(self.test_project)
        root = ET.fromstring(xml_str)
        
        # 检查所有必需的元素
        required_elements = [
            'ProjectMetadata',
            'ProjectMetadata/ProjectName',
            'ProjectMetadata/OriginalProjectPath',
            'MapList',
            'MapList/Map',
            'MapList/Map/MapName',
            'MapList/Map/LayerReferences',
            'LayerDefinitions',
            'LayerDefinitions/Layer'
        ]
        
        for elem_path in required_elements:
            elem = root.find(elem_path)
            self.assertIsNotNone(f"缺少必需元素: {elem_path}", elem)
        
        # 验证图层详细信息 - 在LayerDefinitions中查找
        layer_elem = root.find('LayerDefinitions/Layer')
        self.assertIsNotNone(layer_elem)
        
        # 检查图层属性
        self.assertEqual(layer_elem.findtext('LayerName'), 'Test Layer')
        self.assertEqual(layer_elem.get('type'), 'FeatureLayer')
        
        # 检查数据源
        ds_elem = layer_elem.find('DataSource')
        self.assertIsNotNone(ds_elem)
        self.assertEqual(ds_elem.findtext('DataSourceType'), 'Shapefile')
        
        # 检查字段
        fields_elem = layer_elem.find('FieldDefinitions')
        self.assertIsNotNone(fields_elem)
        field_elems = fields_elem.findall('Field')
        self.assertEqual(len(field_elems), 2)
        
        # 检查坐标系
        cs_elem = layer_elem.find('CoordinateSystem')
        self.assertIsNotNone(cs_elem)
        self.assertEqual(cs_elem.findtext('EPSG'), '4326')
    
    def test_special_characters_handling(self):
        """测试特殊字符处理"""
        # 创建包含特殊字符的项目
        special_project = MigrationProject()
        special_project.metadata = ProjectMetadata(
            project_name='Test & "Special" <Chars>',
            author="Author & Co.",
            description="Test with 'quotes' & <xml> tags"
        )
        
        xml_str = self.generator.generate_xml(special_project)
        
        # XML应该有效且特殊字符被正确转义
        try:
            root = ET.fromstring(xml_str)
            
            # 验证转义后的字符
            metadata_elem = root.find('ProjectMetadata')
            project_name = metadata_elem.findtext('ProjectName')
            author = metadata_elem.findtext('Author')
            
            # 检查特殊字符被正确转义
            # 应该包含转义序列，而不是原始字符
            self.assertIn('&amp;', project_name)  # & 应该被转义为 &amp;
            self.assertIn('&lt;', project_name)   # < 应该被转义为 &lt;
            self.assertIn('&gt;', project_name)   # > 应该被转义为 &gt;
            self.assertIn('&quot;', project_name) # " 应该被转义为 &quot;
            
            # 确保原始字符不作为独立字符出现（除了作为转义序列的一部分）
            # 通过检查转义序列后的字符位置来间接验证
            # 如果字符串中包含独立的 '&'，那么它后面应该跟着有效的转义序列
            # 简单检查：替换转义序列后不应再包含特殊字符
            cleaned = project_name.replace('&amp;', '').replace('&lt;', '').replace('&gt;', '').replace('&quot;', '').replace('&apos;', '')
            # 清理后不应包含 &, <, >, ", '
            self.assertNotIn('&', cleaned, "发现未转义的 & 字符")
            self.assertNotIn('<', cleaned, "发现未转义的 < 字符")
            self.assertNotIn('>', cleaned, "发现未转义的 > 字符")
            self.assertNotIn('"', cleaned, "发现未转义的 \" 字符")
            
            # 验证XML解析成功（已经通过ET.fromstring验证）
            
        except ET.ParseError as e:
            self.fail(f"包含特殊字符的XML解析失败: {e}")

if __name__ == '__main__':
    unittest.main()