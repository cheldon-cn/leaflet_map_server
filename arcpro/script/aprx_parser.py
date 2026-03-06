"""
ArcGIS Pro .aprx 工程文件解析模块
用于提取地图和图层信息
"""
import os
import xml.etree.ElementTree as ET
import xml.dom.minidom as minidom
from typing import Dict, Optional

try:
    import arcpy
    ARC_AVAILABLE = True
except ImportError:
    ARC_AVAILABLE = False
    print("警告: arcpy 模块不可用。请确保在 ArcGIS Pro 的 Python 环境中运行此脚本。")
    print("将使用模拟数据用于测试。")


def parse_aprx(aprx_path: str) -> Dict:
    """
    解析 .aprx 工程文件，提取地图和图层信息
    
    Args:
        aprx_path: .aprx 文件的完整路径
        
    Returns:
        包含工程信息的字典，结构如下：
        {
            'project_path': aprx_path,
            'project_name': 工程名称,
            'maps': [
                {
                    'name': 地图名称,
                    'layers': [
                        {
                            'name': 图层名称,
                            'data_source': 数据源路径,
                            'layer_type': 图层类型
                        },
                        ...
                    ]
                },
                ...
            ]
        }
    
    Raises:
        FileNotFoundError: 如果 .aprx 文件不存在
        RuntimeError: 如果 arcpy 不可用或解析失败
    """
    if not os.path.exists(aprx_path):
        raise FileNotFoundError(f"工程文件不存在: {aprx_path}")
    
    if not ARC_AVAILABLE:
        # 模拟数据用于测试
        return _create_mock_data(aprx_path)
    
    try:
        # 打开工程文件
        aprx = arcpy.mp.ArcGISProject(aprx_path)
        
        project_info = {
            'project_path': aprx_path,
            'project_name': os.path.splitext(os.path.basename(aprx_path))[0],
            'maps': []
        }
        
        # 遍历所有地图
        for map_obj in aprx.listMaps():
            map_info = {
                'name': map_obj.name,
                'layers': []
            }
            
            # 遍历地图中的所有图层
            for layer in map_obj.listLayers():
                layer_info = {
                    'name': layer.name,
                    'data_source': layer.dataSource if hasattr(layer, 'dataSource') else 'N/A',
                    'layer_type': layer.__class__.__name__
                }
                map_info['layers'].append(layer_info)
            
            project_info['maps'].append(map_info)
        
        # 释放资源
        del aprx
        
        return project_info
        
    except Exception as e:
        raise RuntimeError(f"解析 .aprx 文件失败: {str(e)}")


def generate_xml(project_data: Dict, output_path: Optional[str] = None) -> str:
    """
    将工程数据生成为 XML 格式
    
    Args:
        project_data: parse_aprx 返回的工程数据字典
        output_path: 输出 XML 文件路径，如果为 None 则只返回 XML 字符串
        
    Returns:
        XML 字符串
        
    Raises:
        ValueError: 如果 project_data 格式不正确
    """
    # 验证数据结构
    if 'maps' not in project_data:
        raise ValueError("project_data 必须包含 'maps' 键")
    
    # 创建根元素
    root = ET.Element('ArcGISProject')
    root.set('name', project_data.get('project_name', 'Unknown'))
    root.set('path', project_data.get('project_path', ''))
    
    # 添加地图和图层信息
    for map_info in project_data['maps']:
        map_elem = ET.SubElement(root, 'Map')
        map_elem.set('name', map_info.get('name', 'Unknown'))
        
        for layer_info in map_info.get('layers', []):
            layer_elem = ET.SubElement(map_elem, 'Layer')
            layer_elem.set('name', layer_info.get('name', 'Unknown'))
            layer_elem.set('dataSource', str(layer_info.get('data_source', 'N/A')))
    
    # 生成格式化的 XML 字符串
    xml_str = ET.tostring(root, encoding='unicode', method='xml')
    
    # 使用 minidom 美化输出
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


def _create_mock_data(aprx_path: str) -> Dict:
    """
    创建模拟数据用于测试（当 arcpy 不可用时）
    """
    project_name = os.path.splitext(os.path.basename(aprx_path))[0]
    
    return {
        'project_path': aprx_path,
        'project_name': project_name,
        'maps': [
            {
                'name': 'Map1',
                'layers': [
                    {
                        'name': 'Roads',
                        'data_source': r'C:\Data\Roads.shp',
                        'layer_type': 'FeatureLayer'
                    },
                    {
                        'name': 'Buildings',
                        'data_source': r'C:\Data\Buildings.gdb\Buildings',
                        'layer_type': 'FeatureLayer'
                    }
                ]
            },
            {
                'name': 'Map2',
                'layers': [
                    {
                        'name': 'Rivers',
                        'data_source': r'C:\Data\Rivers.shp',
                        'layer_type': 'FeatureLayer'
                    }
                ]
            }
        ]
    }


if __name__ == '__main__':
    # 模块测试代码
    print("aprx_parser 模块测试")
    
    # 创建一个测试工程路径
    test_aprx = r"test_project.aprx"
    
    try:
        # 解析测试工程
        data = parse_aprx(test_aprx)
        print(f"成功解析工程: {data['project_name']}")
        print(f"包含 {len(data['maps'])} 个地图")
        
        # 生成 XML
        xml_output = generate_xml(data)
        print("\n生成的 XML:")
        print(xml_output)
        
    except Exception as e:
        print(f"测试失败: {e}")