"""
符合XSD 1.0规范的XML生成模块
用于生成ArcGIS Pro迁移系统的中间XML文件
"""

import os
import xml.etree.ElementTree as ET
import xml.dom.minidom as minidom
import datetime
import xml.sax.saxutils as saxutils
from typing import Optional, Dict, Any, List
from dataclasses import asdict

from data_models import MigrationProject, ProjectMetadata, MapInfo, LayerInfo, DataSourceInfo, CoordinateSystem, FieldInfo, Extent


class MigrationXMLGenerator:
    """迁移XML生成器"""
    
    def __init__(self, validate_xsd: bool = False):
        """
        初始化XML生成器
        
        Args:
            validate_xsd: 是否验证XSD（暂不支持，保留参数）
        """
        self.validate_xsd = validate_xsd
    
    def _escape_text(self, text: str) -> str:
        """
        转义XML文本中的特殊字符
        
        Args:
            text: 原始文本
            
        Returns:
            转义后的文本
        """
        if text is None:
            return ""
        # 转义 &, <, >，同时转义单引号和双引号
        return saxutils.escape(text, {"'": "&apos;", '"': "&quot;"})
    
    def generate_xml(self, migration_project: MigrationProject, output_path: Optional[str] = None) -> str:
        """
        生成符合XSD的XML
        
        Args:
            migration_project: 迁移工程数据
            output_path: 输出文件路径，如果为None则只返回XML字符串
            
        Returns:
            XML字符串
        """
        # 创建根元素
        root = ET.Element('MigrationProject')
        root.set('version', migration_project.version)
        root.set('sourceArcGISVersion', migration_project.source_arcgis_version)
        root.set('targetQGISVersion', migration_project.target_qgis_version)
        
        # 添加工程元数据
        self._add_project_metadata(root, migration_project.metadata)
        
        # 添加地图列表（如果有）
        if migration_project.maps:
            self._add_map_list(root, migration_project.maps)
        
        # 添加图层定义（从所有图层中提取）
        all_layers = migration_project.get_all_layers()
        if all_layers:
            self._add_layer_definitions(root, all_layers)
        
        # 生成格式化的XML字符串
        xml_str = ET.tostring(root, encoding='unicode', method='xml')
        
        # 使用minidom美化输出
        dom = minidom.parseString(xml_str)
        pretty_xml = dom.toprettyxml(indent='  ')
        
        # 移除多余的空白行
        pretty_xml = '\n'.join([line for line in pretty_xml.split('\n') if line.strip()])
        
        # 如果需要，保存到文件
        if output_path:
            os.makedirs(os.path.dirname(output_path), exist_ok=True)
            with open(output_path, 'w', encoding='utf-8') as f:
                f.write(pretty_xml)
        
        return pretty_xml
    
    def _add_project_metadata(self, root: ET.Element, metadata: ProjectMetadata) -> None:
        """添加工程元数据"""
        metadata_elem = ET.SubElement(root, 'ProjectMetadata')
        
        # 项目名称
        name_elem = ET.SubElement(metadata_elem, 'ProjectName')
        name_elem.text = self._escape_text(metadata.project_name)
        
        # 项目版本（可选）
        if metadata.project_version:
            version_elem = ET.SubElement(metadata_elem, 'ProjectVersion')
            version_elem.text = self._escape_text(metadata.project_version)
        
        # 保存时间（使用当前时间作为默认值）
        save_time = metadata.save_time or datetime.datetime.now()
        save_elem = ET.SubElement(metadata_elem, 'SaveTime')
        save_elem.text = save_time.isoformat()  # ISO格式不包含特殊字符
        
        # 原始工程路径
        path_elem = ET.SubElement(metadata_elem, 'OriginalProjectPath')
        path_elem.text = self._escape_text(metadata.original_project_path)
        
        # 主文件夹
        folder_elem = ET.SubElement(metadata_elem, 'MainFolder')
        folder_elem.text = self._escape_text(metadata.main_folder)
        
        # 作者（可选）
        if metadata.author:
            author_elem = ET.SubElement(metadata_elem, 'Author')
            author_elem.text = self._escape_text(metadata.author)
        
        # 描述（可选）
        if metadata.description:
            desc_elem = ET.SubElement(metadata_elem, 'Description')
            desc_elem.text = self._escape_text(metadata.description)
        
        # 坐标系（可选）
        if metadata.coordinate_system:
            self._add_coordinate_system(metadata_elem, metadata.coordinate_system, 'CoordinateSystem')
    
    def _add_map_list(self, root: ET.Element, maps: List[MapInfo]) -> None:
        """添加地图列表"""
        map_list_elem = ET.SubElement(root, 'MapList')
        
        for map_info in maps:
            map_elem = ET.SubElement(map_list_elem, 'Map')
            
            # 地图名称
            name_elem = ET.SubElement(map_elem, 'MapName')
            name_elem.text = self._escape_text(map_info.name)
            
            # 地图ID（可选）
            if map_info.map_id:
                id_elem = ET.SubElement(map_elem, 'MapId')
                id_elem.text = self._escape_text(map_info.map_id)
            
            # 描述（可选）
            if map_info.description:
                desc_elem = ET.SubElement(map_elem, 'Description')
                desc_elem.text = self._escape_text(map_info.description)
            
            # 坐标系
            if map_info.coordinate_system:
                self._add_coordinate_system(map_elem, map_info.coordinate_system, 'CoordinateSystem')
            
            # 地图范围（可选）
            if map_info.extent:
                self._add_extent(map_elem, map_info.extent, 'MapExtent')
            
            # 图层引用
            layer_refs_elem = ET.SubElement(map_elem, 'LayerReferences')
            for layer in map_info.layers:
                layer_ref_elem = ET.SubElement(layer_refs_elem, 'LayerRef')
                layer_ref_elem.text = self._escape_text(layer.layer_id or layer.name)
    
    def _add_layer_definitions(self, root: ET.Element, layers: List[LayerInfo]) -> None:
        """添加图层定义"""
        layer_defs_elem = ET.SubElement(root, 'LayerDefinitions')
        
        for layer in layers:
            layer_elem = ET.SubElement(layer_defs_elem, 'Layer')
            
            # 图层类型属性
            if layer.layer_type:
                layer_elem.set('type', self._escape_text(layer.layer_type))
            
            # 图层ID
            id_elem = ET.SubElement(layer_elem, 'LayerId')
            id_elem.text = self._escape_text(layer.layer_id or f"layer_{hash(layer.name) % 10000:04d}")
            
            # 图层名称
            name_elem = ET.SubElement(layer_elem, 'LayerName')
            name_elem.text = self._escape_text(layer.name)
            
            # 数据源
            self._add_data_source(layer_elem, layer.data_source)
            
            # 坐标系（可选）
            if layer.coordinate_system:
                self._add_coordinate_system(layer_elem, layer.coordinate_system, 'CoordinateSystem')
            
            # 字段定义
            self._add_field_definitions(layer_elem, layer.fields)
            
            # 定义查询（可选）
            if layer.definition_query:
                query_elem = ET.SubElement(layer_elem, 'DefinitionQuery')
                query_elem.text = self._escape_text(layer.definition_query)
            
            # 透明度（可选）
            if layer.transparency is not None:
                trans_elem = ET.SubElement(layer_elem, 'Transparency')
                trans_elem.text = str(layer.transparency)  # 数值，不需要转义
            
            # 可见性
            visibility_elem = ET.SubElement(layer_elem, 'Visibility')
            visibility_elem.text = str(layer.visibility).lower()  # 布尔值字符串，不需要转义
            
            # 图层范围（可选）
            if layer.extent:
                self._add_extent(layer_elem, layer.extent, 'DataSampling')
    
    def _add_data_source(self, parent: ET.Element, data_source: DataSourceInfo) -> None:
        """添加数据源信息"""
        ds_elem = ET.SubElement(parent, 'DataSource')
        
        # 数据源类型
        type_elem = ET.SubElement(ds_elem, 'DataSourceType')
        type_elem.text = self._escape_text(data_source.data_source_type)
        
        # 连接字符串（可选）
        if data_source.connection_string:
            conn_elem = ET.SubElement(ds_elem, 'ConnectionString')
            conn_elem.text = self._escape_text(data_source.connection_string)
        
        # 文件路径（可选）
        if data_source.file_path:
            path_elem = ET.SubElement(ds_elem, 'FilePath')
            path_elem.text = self._escape_text(data_source.file_path)
        
        # 数据库信息（可选）
        if data_source.database_info:
            db_elem = ET.SubElement(ds_elem, 'DatabaseInfo')
            for key, value in data_source.database_info.items():
                if value is not None:
                    db_item_elem = ET.SubElement(db_elem, key)  # 键作为元素标签，假设是合法的XML名称
                    db_item_elem.text = self._escape_text(str(value))
        
        # 服务URL（可选）
        if data_source.service_url:
            url_elem = ET.SubElement(ds_elem, 'ServiceUrl')
            url_elem.text = self._escape_text(data_source.service_url)
    
    def _add_coordinate_system(self, parent: ET.Element, cs: CoordinateSystem, element_name: str = 'CoordinateSystem') -> None:
        """添加坐标系信息"""
        cs_elem = ET.SubElement(parent, element_name)
        
        # WKT（可选）
        if cs.wkt:
            wkt_elem = ET.SubElement(cs_elem, 'WKT')
            wkt_elem.text = self._escape_text(cs.wkt)
        
        # Proj字符串（可选）
        if cs.proj_string:
            proj_elem = ET.SubElement(cs_elem, 'ProjString')
            proj_elem.text = self._escape_text(cs.proj_string)
        
        # EPSG（可选）
        if cs.epsg:
            epsg_elem = ET.SubElement(cs_elem, 'EPSG')
            epsg_elem.text = str(cs.epsg)  # 数值，不需要转义
        
        # 名称
        name_elem = ET.SubElement(cs_elem, 'Name')
        name_elem.text = self._escape_text(cs.name)
    
    def _add_extent(self, parent: ET.Element, extent: Extent, element_name: str = 'Extent') -> None:
        """添加范围信息"""
        extent_elem = ET.SubElement(parent, element_name)
        
        xmin_elem = ET.SubElement(extent_elem, 'XMin')
        xmin_elem.text = str(extent.xmin)
        
        ymin_elem = ET.SubElement(extent_elem, 'YMin')
        ymin_elem.text = str(extent.ymin)
        
        xmax_elem = ET.SubElement(extent_elem, 'XMax')
        xmax_elem.text = str(extent.xmax)
        
        ymax_elem = ET.SubElement(extent_elem, 'YMax')
        ymax_elem.text = str(extent.ymax)
    
    def _add_field_definitions(self, parent: ET.Element, fields: List[FieldInfo]) -> None:
        """添加字段定义"""
        fields_elem = ET.SubElement(parent, 'FieldDefinitions')
        
        for field in fields:
            field_elem = ET.SubElement(fields_elem, 'Field')
            
            # 字段名称
            name_elem = ET.SubElement(field_elem, 'FieldName')
            name_elem.text = self._escape_text(field.name)
            
            # 字段类型
            type_elem = ET.SubElement(field_elem, 'FieldType')
            type_elem.text = self._escape_text(field.field_type)
            
            # 字段长度（可选）
            if field.length is not None:
                length_elem = ET.SubElement(field_elem, 'FieldLength')
                length_elem.text = str(field.length)  # 数值，不需要转义
            
            # 精度（可选）
            if field.precision is not None:
                prec_elem = ET.SubElement(field_elem, 'FieldPrecision')
                prec_elem.text = str(field.precision)  # 数值，不需要转义
            
            # 比例（可选）
            if field.scale is not None:
                scale_elem = ET.SubElement(field_elem, 'FieldScale')
                scale_elem.text = str(field.scale)  # 数值，不需要转义
            
            # 是否可为空
            nullable_elem = ET.SubElement(field_elem, 'IsNullable')
            nullable_elem.text = str(field.is_nullable).lower()  # 布尔值字符串，不需要转义
            
            # 默认值（可选）
            if field.default_value is not None:
                default_elem = ET.SubElement(field_elem, 'DefaultValue')
                default_elem.text = self._escape_text(str(field.default_value))
            
            # 别名（可选）
            if field.alias:
                alias_elem = ET.SubElement(field_elem, 'Alias')
                alias_elem.text = self._escape_text(field.alias)
            
            # 域（可选）
            if field.domain:
                domain_elem = ET.SubElement(field_elem, 'Domain')
                domain_elem.text = self._escape_text(field.domain)


def create_example_migration_project() -> MigrationProject:
    """创建示例迁移工程用于测试"""
    # 创建坐标系
    cs = CoordinateSystem(
        epsg=4326,
        name="WGS 84"
    )
    
    # 创建数据源
    ds = DataSourceInfo(
        data_source_type="Shapefile",
        file_path=r"C:\Data\Roads.shp"
    )
    
    # 创建字段
    fields = [
        FieldInfo(name="ID", field_type="Integer", is_nullable=False),
        FieldInfo(name="NAME", field_type="String", length=50, alias="Road Name"),
        FieldInfo(name="TYPE", field_type="String", length=20),
        FieldInfo(name="LENGTH", field_type="Double", precision=10, scale=2)
    ]
    
    # 创建图层
    layer = LayerInfo(
        layer_id="layer_001",
        name="Roads",
        layer_type="FeatureLayer",
        data_source=ds,
        coordinate_system=cs,
        fields=fields,
        visibility=True
    )
    
    # 创建地图
    map_info = MapInfo(
        name="Transportation Map",
        map_id="map_001",
        coordinate_system=cs,
        layers=[layer]
    )
    
    # 创建工程元数据
    metadata = ProjectMetadata(
        project_name="示例工程",
        project_version="1.0",
        original_project_path=r"C:\Projects\example.aprx",
        author="GIS Team",
        description="示例ArcGIS Pro工程",
        coordinate_system=cs
    )
    
    # 创建迁移工程
    migration_project = MigrationProject(
        version="1.0",
        source_arcgis_version="3.2",
        target_qgis_version="3.34",
        metadata=metadata,
        maps=[map_info],
        layers=[layer]
    )
    
    return migration_project


if __name__ == '__main__':
    """模块测试"""
    print("Migration XML Generator 测试")
    
    # 创建示例数据
    project = create_example_migration_project()
    
    # 生成XML
    generator = MigrationXMLGenerator()
    xml_output = generator.generate_xml(project, output_path="example_migration.xml")
    
    print(f"成功生成XML，长度: {len(xml_output)} 字符")
    print("\nXML 预览 (前20行):")
    lines = xml_output.split('\n')
    for i, line in enumerate(lines[:20]):
        print(f"{i+1:3}: {line}")
    if len(lines) > 20:
        print("...")
    
    print("\nXML 文件已保存为: example_migration.xml")