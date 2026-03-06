#!/usr/bin/env python3
"""
数据模型单元测试
"""

import unittest
import sys
import os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from data_models import (
    CoordinateSystem, Extent, FieldInfo, DataSourceInfo, 
    LayerInfo, MapInfo, ProjectMetadata, MigrationProject,
    DataSample, TaskStatus
)

class TestCoordinateSystem(unittest.TestCase):
    def test_creation(self):
        cs = CoordinateSystem(wkt="WKT", epsg=4326, name="WGS84")
        self.assertEqual(cs.wkt, "WKT")
        self.assertEqual(cs.epsg, 4326)
        self.assertEqual(cs.name, "WGS84")
        self.assertIsNone(cs.proj_string)
    
    def test_defaults(self):
        cs = CoordinateSystem()
        self.assertIsNone(cs.wkt)
        self.assertIsNone(cs.epsg)
        self.assertEqual(cs.name, "")

class TestExtent(unittest.TestCase):
    def test_creation(self):
        extent = Extent(xmin=0, ymin=1, xmax=10, ymax=20)
        self.assertEqual(extent.xmin, 0)
        self.assertEqual(extent.ymin, 1)
        self.assertEqual(extent.xmax, 10)
        self.assertEqual(extent.ymax, 20)

class TestFieldInfo(unittest.TestCase):
    def test_creation(self):
        field = FieldInfo(name="id", field_type="Integer", length=10, 
                         is_nullable=False, alias="Identifier")
        self.assertEqual(field.name, "id")
        self.assertEqual(field.field_type, "Integer")
        self.assertEqual(field.length, 10)
        self.assertFalse(field.is_nullable)
        self.assertEqual(field.alias, "Identifier")

class TestDataSourceInfo(unittest.TestCase):
    def test_creation(self):
        ds = DataSourceInfo(
            data_source_type="Shapefile",
            file_path=r"C:\data\test.shp",
            connection_string="host=localhost"
        )
        self.assertEqual(ds.data_source_type, "Shapefile")
        self.assertEqual(ds.file_path, r"C:\data\test.shp")
        self.assertEqual(ds.connection_string, "host=localhost")

class TestLayerInfo(unittest.TestCase):
    def test_creation(self):
        ds = DataSourceInfo(data_source_type="Shapefile")
        layer = LayerInfo(
            layer_id="layer1",
            name="Test Layer",
            layer_type="FeatureLayer",
            data_source=ds,
            visibility=False
        )
        self.assertEqual(layer.layer_id, "layer1")
        self.assertEqual(layer.name, "Test Layer")
        self.assertEqual(layer.layer_type, "FeatureLayer")
        self.assertEqual(layer.data_source.data_source_type, "Shapefile")
        self.assertFalse(layer.visibility)

class TestMapInfo(unittest.TestCase):
    def test_creation(self):
        layer = LayerInfo(name="Layer1")
        map_info = MapInfo(
            name="Test Map",
            map_id="map1",
            layers=[layer]
        )
        self.assertEqual(map_info.name, "Test Map")
        self.assertEqual(map_info.map_id, "map1")
        self.assertEqual(len(map_info.layers), 1)
        self.assertEqual(map_info.layers[0].name, "Layer1")

class TestProjectMetadata(unittest.TestCase):
    def test_creation(self):
        import datetime
        now = datetime.datetime.now()
        metadata = ProjectMetadata(
            project_name="Test Project",
            original_project_path=r"C:\test.aprx",
            author="Tester",
            save_time=now
        )
        self.assertEqual(metadata.project_name, "Test Project")
        self.assertEqual(metadata.original_project_path, r"C:\test.aprx")
        self.assertEqual(metadata.author, "Tester")
        self.assertEqual(metadata.save_time, now)
    
    def test_main_folder(self):
        metadata = ProjectMetadata(original_project_path=r"C:\projects\test.aprx")
        self.assertEqual(metadata.main_folder, r"C:\projects")

class TestMigrationProject(unittest.TestCase):
    def test_creation(self):
        project = MigrationProject(
            version="1.0",
            source_arcgis_version="3.2",
            target_qgis_version="3.34"
        )
        self.assertEqual(project.version, "1.0")
        self.assertEqual(project.source_arcgis_version, "3.2")
        self.assertEqual(project.target_qgis_version, "3.34")
        self.assertEqual(len(project.maps), 0)
        self.assertEqual(len(project.layers), 0)
    
    def test_get_all_layers(self):
        project = MigrationProject()
        layer1 = LayerInfo(name="Layer1")
        layer2 = LayerInfo(name="Layer2")
        map_layer = LayerInfo(name="MapLayer")
        map_info = MapInfo(name="Map", layers=[map_layer])
        
        project.layers = [layer1, layer2]
        project.maps = [map_info]
        
        all_layers = project.get_all_layers()
        self.assertEqual(len(all_layers), 3)
        names = {layer.name for layer in all_layers}
        self.assertSetEqual(names, {"Layer1", "Layer2", "MapLayer"})

class TestDataSample(unittest.TestCase):
    def test_creation(self):
        sample = DataSample(
            total_features=100,
            sample_size=10,
            geometry_type="Point"
        )
        self.assertEqual(sample.total_features, 100)
        self.assertEqual(sample.sample_size, 10)
        self.assertEqual(sample.geometry_type, "Point")

class TestTaskStatus(unittest.TestCase):
    def test_creation(self):
        status = TaskStatus(
            status="running",
            progress=0.5,
            retry_count=2
        )
        self.assertEqual(status.status, "running")
        self.assertEqual(status.progress, 0.5)
        self.assertEqual(status.retry_count, 2)

if __name__ == '__main__':
    unittest.main()