"""
数据模型定义
用于ArcGIS Pro工程迁移系统的数据结构
"""

from dataclasses import dataclass, field
from typing import List, Optional, Dict, Any
from datetime import datetime


@dataclass
class CoordinateSystem:
    """坐标系信息"""
    wkt: Optional[str] = None
    proj_string: Optional[str] = None
    epsg: Optional[int] = None
    name: str = ""


@dataclass
class Extent:
    """空间范围"""
    xmin: float = 0.0
    ymin: float = 0.0
    xmax: float = 0.0
    ymax: float = 0.0


@dataclass
class FieldInfo:
    """字段信息"""
    name: str = ""
    field_type: str = ""
    length: Optional[int] = None
    precision: Optional[int] = None
    scale: Optional[int] = None
    is_nullable: bool = True
    default_value: Optional[str] = None
    alias: Optional[str] = None
    domain: Optional[str] = None


@dataclass
class DataSourceInfo:
    """数据源信息"""
    data_source_type: str = ""  # Shapefile, FileGeodatabase, EnterpriseGeodatabase, PostgreSQL, etc.
    connection_string: Optional[str] = None
    file_path: Optional[str] = None
    database_info: Optional[Dict[str, Any]] = None
    service_url: Optional[str] = None


@dataclass
class LayerInfo:
    """图层信息"""
    layer_id: str = ""
    name: str = ""
    layer_type: str = "FeatureLayer"  # FeatureLayer, RasterLayer, Table, GroupLayer
    data_source: DataSourceInfo = field(default_factory=DataSourceInfo)
    coordinate_system: Optional[CoordinateSystem] = None
    fields: List[FieldInfo] = field(default_factory=list)
    definition_query: Optional[str] = None
    transparency: Optional[int] = None
    visibility: bool = True
    extent: Optional[Extent] = None


@dataclass
class MapInfo:
    """地图信息"""
    name: str = ""
    map_id: str = ""
    description: Optional[str] = None
    coordinate_system: Optional[CoordinateSystem] = None
    extent: Optional[Extent] = None
    layers: List[LayerInfo] = field(default_factory=list)
    properties: Dict[str, Any] = field(default_factory=dict)


@dataclass
class ProjectMetadata:
    """工程元数据"""
    project_name: str = ""
    project_version: Optional[str] = None
    save_time: Optional[datetime] = None
    original_project_path: str = ""
    author: Optional[str] = None
    description: Optional[str] = None
    coordinate_system: Optional[CoordinateSystem] = None
    maps: List[MapInfo] = field(default_factory=list)
    
    @property
    def main_folder(self) -> str:
        """获取工程主文件夹路径"""
        import os
        return os.path.dirname(self.original_project_path) if self.original_project_path else ""


@dataclass
class MigrationProject:
    """迁移工程完整数据"""
    version: str = "1.0"
    source_arcgis_version: str = ""
    target_qgis_version: str = "3.34"
    metadata: ProjectMetadata = field(default_factory=ProjectMetadata)
    maps: List[MapInfo] = field(default_factory=list)
    layers: List[LayerInfo] = field(default_factory=list)
    
    def get_all_layers(self) -> List[LayerInfo]:
        """获取所有图层（包括地图中的图层）"""
        all_layers = self.layers.copy()
        for map_info in self.maps:
            all_layers.extend(map_info.layers)
        return all_layers


@dataclass
class DataSample:
    """数据采样结果"""
    total_features: int = 0
    sample_size: int = 0
    sampled_features: List[Dict[str, Any]] = field(default_factory=list)
    fields: List[FieldInfo] = field(default_factory=list)
    geometry_type: str = ""


@dataclass
class TaskStatus:
    """任务状态"""
    status: str = "pending"  # pending, running, completed, failed, cancelled
    start_time: Optional[datetime] = None
    end_time: Optional[datetime] = None
    error_info: Optional[Dict[str, Any]] = None
    retry_count: int = 0
    progress: float = 0.0  # 0.0 to 1.0