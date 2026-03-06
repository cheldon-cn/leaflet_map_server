"""
增强版ArcGIS Pro工程解析器
使用数据模型，提取更丰富的元数据信息
"""

import os
import sys
from typing import Optional, List, Dict, Any
import datetime

from data_models import (
    MigrationProject, ProjectMetadata, MapInfo, LayerInfo, 
    DataSourceInfo, CoordinateSystem, FieldInfo, Extent
)


class EnhancedAPRXParser:
    """增强版APRX解析器"""
    
    def __init__(self, use_arcpy: bool = True):
        """
        初始化解析器
        
        Args:
            use_arcpy: 是否使用arcpy（如果为False，则使用模拟数据）
        """
        self.use_arcpy = use_arcpy
        self.arc_available = False
        
        if use_arcpy:
            try:
                import arcpy
                self.arc_available = True
                self.arcpy = arcpy
            except ImportError:
                print("警告: arcpy 模块不可用。请确保在 ArcGIS Pro 的 Python 环境中运行此脚本。")
                print("将使用模拟数据用于测试。")
                self.arc_available = False
    
    def parse_aprx(self, aprx_path: str) -> MigrationProject:
        """
        解析.aprx工程文件，返回完整的迁移工程数据
        
        Args:
            aprx_path: .aprx文件路径
            
        Returns:
            MigrationProject对象
            
        Raises:
            FileNotFoundError: 如果.aprx文件不存在
            RuntimeError: 如果解析失败
        """
        # 仅当使用arcpy时才检查文件存在性
        if self.arc_available and self.use_arcpy:
            if not os.path.exists(aprx_path):
                raise FileNotFoundError(f"工程文件不存在: {aprx_path}")
        
        if not self.arc_available or not self.use_arcpy:
            return self._create_mock_migration_project(aprx_path)
        
        try:
            # 打开工程文件
            aprx = self.arcpy.mp.ArcGISProject(aprx_path)
            
            # 创建工程元数据
            metadata = self._extract_project_metadata(aprx, aprx_path)
            
            # 提取地图信息
            maps = self._extract_maps(aprx)
            
            # 提取所有图层（去重）
            all_layers = self._extract_all_layers(maps)
            
            # 创建迁移工程对象
            migration_project = MigrationProject(
                version="1.0",
                source_arcgis_version=self._get_arcgis_version(),
                target_qgis_version="3.34",
                metadata=metadata,
                maps=maps,
                layers=all_layers
            )
            
            # 释放资源
            del aprx
            
            return migration_project
            
        except Exception as e:
            raise RuntimeError(f"解析 .aprx 文件失败: {str(e)}")
    
    def _extract_project_metadata(self, aprx, aprx_path: str) -> ProjectMetadata:
        """提取工程元数据"""
        metadata = ProjectMetadata()
        
        # 基本元数据
        metadata.project_name = os.path.splitext(os.path.basename(aprx_path))[0]
        metadata.original_project_path = aprx_path
        metadata.save_time = datetime.datetime.now()
        
        try:
            # 尝试获取更多元数据
            if hasattr(aprx, 'metadata'):
                md = aprx.metadata
                if hasattr(md, 'title') and md.title:
                    metadata.project_name = md.title
                if hasattr(md, 'version') and md.version:
                    metadata.project_version = md.version
                if hasattr(md, 'author') and md.author:
                    metadata.author = md.author
                if hasattr(md, 'description') and md.description:
                    metadata.description = md.description
        except:
            # 如果获取元数据失败，使用默认值
            pass
        
        return metadata
    
    def _extract_maps(self, aprx) -> List[MapInfo]:
        """提取地图信息"""
        maps = []
        
        try:
            for map_obj in aprx.listMaps():
                map_info = MapInfo()
                map_info.name = map_obj.name
                map_info.map_id = map_obj.name  # 使用名称作为ID
                
                # 尝试获取坐标系
                try:
                    if hasattr(map_obj, 'spatialReference') and map_obj.spatialReference:
                        cs = self._extract_coordinate_system(map_obj.spatialReference)
                        map_info.coordinate_system = cs
                except:
                    pass
                
                # 提取图层
                map_info.layers = self._extract_layers_from_map(map_obj)
                
                maps.append(map_info)
        except Exception as e:
            print(f"提取地图信息时出错: {e}")
        
        return maps
    
    def _extract_layers_from_map(self, map_obj) -> List[LayerInfo]:
        """从地图中提取图层信息"""
        layers = []
        
        try:
            for layer in map_obj.listLayers():
                layer_info = self._extract_layer_info(layer)
                if layer_info:
                    layers.append(layer_info)
        except Exception as e:
            print(f"提取图层信息时出错: {e}")
        
        return layers
    
    def _extract_layer_info(self, layer) -> Optional[LayerInfo]:
        """提取单个图层信息"""
        try:
            layer_info = LayerInfo()
            layer_info.name = layer.name
            layer_info.layer_id = layer.name  # 使用名称作为ID
            
            # 图层类型
            if hasattr(layer, 'isFeatureLayer') and layer.isFeatureLayer:
                layer_info.layer_type = "FeatureLayer"
            elif hasattr(layer, 'isRasterLayer') and layer.isRasterLayer:
                layer_info.layer_type = "RasterLayer"
            elif hasattr(layer, 'isGroupLayer') and layer.isGroupLayer:
                layer_info.layer_type = "GroupLayer"
            else:
                layer_info.layer_type = layer.__class__.__name__
            
            # 数据源
            if hasattr(layer, 'dataSource'):
                ds_info = self._extract_data_source_info(layer.dataSource)
                layer_info.data_source = ds_info
            
            # 可见性
            if hasattr(layer, 'visible'):
                layer_info.visibility = layer.visible
            
            # 透明度
            if hasattr(layer, 'transparency'):
                layer_info.transparency = layer.transparency
            
            # 定义查询
            if hasattr(layer, 'definitionQuery') and layer.definitionQuery:
                layer_info.definition_query = layer.definitionQuery
            
            # 尝试获取字段信息
            try:
                if hasattr(layer, 'fields'):
                    fields = self._extract_field_info(layer.fields)
                    layer_info.fields = fields
            except:
                pass
            
            # 尝试获取坐标系
            try:
                if hasattr(layer, 'spatialReference') and layer.spatialReference:
                    cs = self._extract_coordinate_system(layer.spatialReference)
                    layer_info.coordinate_system = cs
            except:
                pass
            
            return layer_info
            
        except Exception as e:
            print(f"提取图层 '{layer.name if hasattr(layer, 'name') else 'unknown'}' 信息时出错: {e}")
            return None
    
    def _extract_data_source_info(self, data_source: str) -> DataSourceInfo:
        """提取数据源信息"""
        ds_info = DataSourceInfo()
        
        if not data_source or data_source == 'N/A':
            ds_info.data_source_type = "Unknown"
            return ds_info
        
        # 简单判断数据源类型
        data_source_lower = data_source.lower()
        
        if data_source_lower.endswith('.shp'):
            ds_info.data_source_type = "Shapefile"
            ds_info.file_path = data_source
        elif '.gdb' in data_source_lower:
            ds_info.data_source_type = "FileGeodatabase"
            ds_info.file_path = data_source
        elif data_source_lower.startswith('postgresql') or 'postgresql' in data_source_lower:
            ds_info.data_source_type = "PostgreSQL"
            ds_info.connection_string = data_source
        elif data_source_lower.startswith('http://') or data_source_lower.startswith('https://'):
            ds_info.data_source_type = "WebService"
            ds_info.service_url = data_source
        elif data_source_lower.endswith('.tif') or data_source_lower.endswith('.tiff') or \
             data_source_lower.endswith('.jpg') or data_source_lower.endswith('.png'):
            ds_info.data_source_type = "Raster"
            ds_info.file_path = data_source
        else:
            ds_info.data_source_type = "Other"
            ds_info.file_path = data_source
        
        return ds_info
    
    def _extract_coordinate_system(self, spatial_ref) -> Optional[CoordinateSystem]:
        """提取坐标系信息"""
        try:
            cs = CoordinateSystem()
            
            if hasattr(spatial_ref, 'name'):
                cs.name = spatial_ref.name
            
            if hasattr(spatial_ref, 'factoryCode'):
                cs.epsg = spatial_ref.factoryCode
            
            if hasattr(spatial_ref, 'exportToString'):
                try:
                    wkt = spatial_ref.exportToString()
                    if wkt:
                        cs.wkt = wkt
                except:
                    pass
            
            return cs
            
        except Exception as e:
            print(f"提取坐标系信息时出错: {e}")
            return None
    
    def _extract_field_info(self, fields) -> List[FieldInfo]:
        """提取字段信息"""
        field_list = []
        
        try:
            for field in fields:
                field_info = FieldInfo()
                
                if hasattr(field, 'name'):
                    field_info.name = field.name
                
                if hasattr(field, 'type'):
                    field_info.field_type = field.type
                
                if hasattr(field, 'length'):
                    field_info.length = field.length
                
                if hasattr(field, 'precision'):
                    field_info.precision = field.precision
                
                if hasattr(field, 'scale'):
                    field_info.scale = field.scale
                
                if hasattr(field, 'isNullable'):
                    field_info.is_nullable = field.isNullable
                
                if hasattr(field, 'aliasName'):
                    field_info.alias = field.aliasName
                
                field_list.append(field_info)
        except Exception as e:
            print(f"提取字段信息时出错: {e}")
        
        return field_list
    
    def _extract_all_layers(self, maps: List[MapInfo]) -> List[LayerInfo]:
        """从所有地图中提取所有图层（去重）"""
        all_layers = []
        seen_layer_ids = set()
        
        for map_info in maps:
            for layer in map_info.layers:
                layer_id = layer.layer_id or layer.name
                if layer_id not in seen_layer_ids:
                    seen_layer_ids.add(layer_id)
                    all_layers.append(layer)
        
        return all_layers
    
    def _get_arcgis_version(self) -> str:
        """获取ArcGIS版本"""
        try:
            if self.arc_available:
                import arcpy
                if hasattr(arcpy, 'GetInstallInfo'):
                    info = arcpy.GetInstallInfo()
                    if 'Version' in info:
                        return info['Version']
        except:
            pass
        return "3.2"  # 默认版本
    
    def _create_mock_migration_project(self, aprx_path: str) -> MigrationProject:
        """创建模拟迁移工程（用于测试）"""
        import random
        
        # 创建多个坐标系
        cs_wgs84 = CoordinateSystem(
            epsg=4326,
            name="WGS 84",
            wkt='GEOGCS["WGS 84",DATUM["WGS_1984",SPHEROID["WGS 84",6378137,298.257223563]],PRIMEM["Greenwich",0],UNIT["degree",0.0174532925199433]]'
        )
        
        cs_web_mercator = CoordinateSystem(
            epsg=3857,
            name="WGS 84 / Pseudo-Mercator",
            proj_string="+proj=merc +a=6378137 +b=6378137 +lat_ts=0 +lon_0=0 +x_0=0 +y_0=0 +k=1 +units=m +nadgrids=@null +wktext +no_defs"
        )
        
        cs_local = CoordinateSystem(
            epsg=32650,
            name="WGS 84 / UTM zone 50N",
            wkt='PROJCS["WGS 84 / UTM zone 50N",GEOGCS["WGS 84",DATUM["WGS_1984",SPHEROID["WGS 84",6378137,298.257223563]],PRIMEM["Greenwich",0],UNIT["degree",0.0174532925199433]],PROJECTION["Transverse_Mercator"],PARAMETER["latitude_of_origin",0],PARAMETER["central_meridian",117],PARAMETER["scale_factor",0.9996],PARAMETER["false_easting",500000],PARAMETER["false_northing",0],UNIT["metre",1]]'
        )
        
        # 创建多种数据源
        data_sources = [
            DataSourceInfo(
                data_source_type="Shapefile",
                file_path=r"C:\Data\Roads.shp"
            ),
            DataSourceInfo(
                data_source_type="FileGeodatabase",
                file_path=r"C:\Data\Landuse.gdb",
                database_info={"workspace": "landuse", "version": "10.8"}
            ),
            DataSourceInfo(
                data_source_type="PostgreSQL",
                connection_string="postgresql://user:pass@localhost:5432/gis_db",
                database_info={
                    "host": "localhost",
                    "port": 5432,
                    "database": "gis_db",
                    "schema": "public",
                    "table": "buildings"
                }
            ),
            DataSourceInfo(
                data_source_type="WebService",
                service_url="https://services.arcgis.com/example/arcgis/rest/services/World_Imagery/MapServer"
            ),
            DataSourceInfo(
                data_source_type="Raster",
                file_path=r"C:\Data\dem.tif"
            )
        ]
        
        # 创建多种字段定义
        field_sets = {
            "roads": [
                FieldInfo(name="OBJECTID", field_type="OID", is_nullable=False),
                FieldInfo(name="NAME", field_type="String", length=50, alias="Road Name"),
                FieldInfo(name="TYPE", field_type="String", length=20, domain="RoadType"),
                FieldInfo(name="LENGTH", field_type="Double", precision=10, scale=2),
                FieldInfo(name="SPEED_LIMIT", field_type="Integer", default_value=50),
                FieldInfo(name="ONE_WAY", field_type="Boolean", is_nullable=True)
            ],
            "buildings": [
                FieldInfo(name="FID", field_type="OID", is_nullable=False),
                FieldInfo(name="BUILDING_ID", field_type="String", length=20, alias="Building ID"),
                FieldInfo(name="HEIGHT", field_type="Double", precision=8, scale=2),
                FieldInfo(name="FLOORS", field_type="Integer"),
                FieldInfo(name="CONSTRUCTION_YEAR", field_type="Date"),
                FieldInfo(name="ADDRESS", field_type="String", length=100)
            ],
            "landuse": [
                FieldInfo(name="ID", field_type="Integer", is_nullable=False),
                FieldInfo(name="LANDUSE_TYPE", field_type="String", length=30, domain="LandUseType"),
                FieldInfo(name="AREA_HA", field_type="Double", precision=12, scale=4),
                FieldInfo(name="MANAGER", field_type="String", length=50),
                FieldInfo(name="LAST_UPDATE", field_type="Date")
            ]
        }
        
        # 创建多个图层
        layers = []
        layer_names = ["Roads", "Buildings", "Land Use", "Water Bodies", "Elevation", "Satellite Imagery"]
        layer_types = ["FeatureLayer", "FeatureLayer", "FeatureLayer", "FeatureLayer", "RasterLayer", "RasterLayer"]
        
        for i, (layer_name, layer_type) in enumerate(zip(layer_names, layer_types)):
            # 选择数据源
            ds_idx = i % len(data_sources)
            data_source = data_sources[ds_idx]
            
            # 选择字段集
            if layer_name == "Roads":
                fields = field_sets["roads"]
            elif layer_name == "Buildings":
                fields = field_sets["buildings"]
            elif layer_name == "Land Use":
                fields = field_sets["landuse"]
            else:
                fields = []
            
            # 选择坐标系
            if i % 3 == 0:
                cs = cs_wgs84
            elif i % 3 == 1:
                cs = cs_web_mercator
            else:
                cs = cs_local
            
            # 创建范围
            extent = None
            if layer_type == "FeatureLayer":
                # 随机生成范围
                base_x = 100 + i * 50
                base_y = 200 + i * 30
                extent = Extent(
                    xmin=base_x,
                    ymin=base_y,
                    xmax=base_x + 1000,
                    ymax=base_y + 800
                )
            
            layer = LayerInfo(
                layer_id=f"layer_{i+1:03d}",
                name=layer_name,
                layer_type=layer_type,
                data_source=data_source,
                coordinate_system=cs,
                fields=fields,
                visibility=i != 2,  # 第三个图层不可见
                transparency=0 if i == 0 else random.randint(0, 50),
                extent=extent,
                definition_query=None if i % 2 == 0 else "TYPE = 'MAJOR'"
            )
            layers.append(layer)
        
        # 创建多个地图
        maps = []
        map_names = ["Main Map", "Overview Map", "Analysis Map", "Presentation Map"]
        
        for i, map_name in enumerate(map_names):
            # 为每个地图分配部分图层
            start_idx = i * 2
            end_idx = min(start_idx + 3, len(layers))
            map_layers = layers[start_idx:end_idx]
            
            if not map_layers:
                map_layers = [layers[0]]  # 至少一个图层
            
            map_info = MapInfo(
                name=map_name,
                map_id=f"map_{i+1:03d}",
                coordinate_system=cs_wgs84 if i % 2 == 0 else cs_web_mercator,
                layers=map_layers,
                description=f"{map_name} for demonstration purposes",
                extent=Extent(xmin=0, ymin=0, xmax=1000, ymax=1000) if i == 0 else None
            )
            maps.append(map_info)
        
        # 创建工程元数据
        project_base_name = os.path.splitext(os.path.basename(aprx_path))[0]
        metadata = ProjectMetadata(
            project_name=f"Test Project: {project_base_name}",
            project_version=f"{random.randint(1, 3)}.{random.randint(0, 9)}",
            original_project_path=aprx_path,
            author="GIS Department",
            description=f"Comprehensive test project for ArcGIS Pro migration. Contains {len(layers)} layers and {len(maps)} maps.",
            coordinate_system=cs_wgs84,
            save_time=datetime.datetime.now()
        )
        
        # 创建迁移工程
        migration_project = MigrationProject(
            version="1.0",
            source_arcgis_version="3.2",
            target_qgis_version="3.34",
            metadata=metadata,
            maps=maps,
            layers=layers
        )
        
        return migration_project


def main():
    """主函数（测试用）"""
    parser = EnhancedAPRXParser(use_arcpy=False)  # 测试时使用模拟数据
    
    test_aprx = r"test_project.aprx"
    
    try:
        print(f"开始解析工程文件: {test_aprx}")
        migration_project = parser.parse_aprx(test_aprx)
        
        print(f"工程名称: {migration_project.metadata.project_name}")
        print(f"地图数量: {len(migration_project.maps)}")
        print(f"图层总数: {len(migration_project.get_all_layers())}")
        
        # 显示详细信息
        for i, map_info in enumerate(migration_project.maps):
            print(f"\n地图 {i+1}: {map_info.name}")
            print(f"  图层数量: {len(map_info.layers)}")
            for j, layer in enumerate(map_info.layers):
                print(f"  图层 {j+1}: {layer.name} ({layer.layer_type})")
                print(f"    数据源: {layer.data_source.data_source_type}")
                if layer.data_source.file_path:
                    print(f"    文件路径: {layer.data_source.file_path}")
        
        return migration_project
        
    except Exception as e:
        print(f"解析失败: {e}")
        return None


if __name__ == '__main__':
    main()