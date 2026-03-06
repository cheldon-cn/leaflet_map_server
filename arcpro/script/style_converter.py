"""
样式转换模块
支持ArcGIS Pro样式到QGIS样式的转换
"""

import os
import json
import sqlite3
import logging
from typing import Optional, Dict, Any, List
from enum import Enum
from dataclasses import dataclass, field
from datetime import datetime


class StyleType(Enum):
    """样式类型枚举"""
    MARKER = "marker"  # 点符号
    LINE = "line"      # 线符号  
    FILL = "fill"      # 填充符号
    TEXT = "text"      # 文本符号
    RASTER = "raster"  # 栅格符号


@dataclass
class ColorInfo:
    """颜色信息"""
    red: int = 0
    green: int = 0
    blue: int = 0
    alpha: int = 255  # 透明度，0-255
    
    def to_hex(self) -> str:
        """转换为十六进制颜色字符串（带透明度）"""
        return f"#{self.red:02x}{self.green:02x}{self.blue:02x}{self.alpha:02x}"
    
    def to_rgb(self) -> str:
        """转换为RGB颜色字符串"""
        return f"{self.red},{self.green},{self.blue}"
    
    def to_qgis_color(self) -> str:
        """转换为QGIS颜色字符串"""
        return f"{self.red},{self.green},{self.blue},{self.alpha}"


@dataclass
class SymbolInfo:
    """符号信息"""
    symbol_id: str = ""
    name: str = ""
    style_type: StyleType = StyleType.MARKER
    category: str = ""
    tags: List[str] = field(default_factory=list)
    color: Optional[ColorInfo] = None
    size: float = 1.0
    width: float = 1.0  # 线宽或轮廓宽度
    rotation: float = 0.0
    description: str = ""
    
    # ArcGIS Pro特定属性
    arcgis_symbol_type: str = ""
    arcgis_symbol_json: Optional[Dict[str, Any]] = None
    
    # QGIS样式XML
    qgis_symbol_xml: str = ""


@dataclass
class StyleLibrary:
    """样式库"""
    name: str = ""
    version: str = "1.0"
    source_path: str = ""  # .stylx文件路径
    symbols: List[SymbolInfo] = field(default_factory=list)
    conversion_time: Optional[datetime] = None
    
    @property
    def symbol_count(self) -> int:
        """获取符号数量"""
        return len(self.symbols)
    
    @property
    def symbols_by_type(self) -> Dict[StyleType, List[SymbolInfo]]:
        """按类型分组符号"""
        grouped = {style_type: [] for style_type in StyleType}
        for symbol in self.symbols:
            grouped[symbol.style_type].append(symbol)
        return grouped


class StyleMappingTable:
    """样式映射表"""
    
    # ArcGIS Pro到QGIS符号类型映射
    SYMBOL_TYPE_MAP = {
        # 点符号
        "SimpleMarkerSymbol": "QgsSimpleMarkerSymbolLayer",
        "PictureMarkerSymbol": "QgsRasterMarkerSymbolLayer",
        "CharacterMarkerSymbol": "QgsFontMarkerSymbolLayer",
        "ArrowMarkerSymbol": "QgsSimpleMarkerSymbolLayer",
        
        # 线符号
        "SimpleLineSymbol": "QgsSimpleLineSymbolLayer",
        "CartographicLineSymbol": "QgsSimpleLineSymbolLayer",
        "HashLineSymbol": "QgsLinePatternFillSymbolLayer",
        "MarkerLineSymbol": "QgsMarkerLineSymbolLayer",
        
        # 面符号
        "SimpleFillSymbol": "QgsSimpleFillSymbolLayer",
        "GradientFillSymbol": "QgsGradientFillSymbolLayer",
        "LineFillSymbol": "QgsLinePatternFillSymbolLayer",
        "MarkerFillSymbol": "QgsPointPatternFillSymbolLayer",
        "PictureFillSymbol": "QgsRasterFillSymbolLayer",
    }
    
    # 线型映射
    LINE_STYLE_MAP = {
        "solid": "solid",
        "dash": "dash",
        "dot": "dot",
        "dash-dot": "dash dot",
        "dash-dot-dot": "dash dot dot",
        "null": "no",
        "insideFrame": "solid",  # 近似映射
        "outsideFrame": "solid", # 近似映射
    }
    
    # 填充样式映射
    FILL_STYLE_MAP = {
        "solid": "solid",
        "null": "no",
        "horizontal": "horizontal",
        "vertical": "vertical",
        "cross": "cross",
        "diagonal-cross": "diagonal_x",
        "diagonalLeft": "diagonal",
        "diagonalRight": "diagonal",
        "forwardDiagonal": "f_diagonal",
        "backwardDiagonal": "b_diagonal",
    }
    
    # 点符号形状映射
    MARKER_SHAPE_MAP = {
        "circle": "circle",
        "square": "square",
        "cross": "cross",
        "diamond": "diamond",
        "triangle": "triangle",
        "star": "star",
        "arrow": "arrow",
        "x": "x",
        "hexagon": "hexagon",
        "pentagon": "pentagon",
    }
    
    @classmethod
    def convert_symbol_type(cls, arcgis_type: str) -> str:
        """转换符号类型"""
        return cls.SYMBOL_TYPE_MAP.get(arcgis_type, "QgsSimpleMarkerSymbolLayer")
    
    @classmethod
    def convert_line_style(cls, arcgis_style: str) -> str:
        """转换线型"""
        return cls.LINE_STYLE_MAP.get(arcgis_style.lower(), "solid")
    
    @classmethod
    def convert_fill_style(cls, arcgis_style: str) -> str:
        """转换填充样式"""
        return cls.FILL_STYLE_MAP.get(arcgis_style.lower(), "solid")
    
    @classmethod
    def convert_marker_shape(cls, arcgis_shape: str) -> str:
        """转换点符号形状"""
        return cls.MARKER_SHAPE_MAP.get(arcgis_shape.lower(), "circle")


class StylxParser:
    """.stylx文件解析器"""
    
    def __init__(self, stylx_path: str):
        """
        初始化解析器
        
        Args:
            stylx_path: .stylx文件路径
        """
        self.stylx_path = stylx_path
        self.logger = self._setup_logger()
        self.conn = None
        
    def _setup_logger(self) -> logging.Logger:
        """设置日志器"""
        logger = logging.getLogger(__name__)
        if not logger.handlers:
            handler = logging.StreamHandler()
            formatter = logging.Formatter(
                '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
            )
            handler.setFormatter(formatter)
            logger.addHandler(handler)
            logger.setLevel(logging.INFO)
        return logger
    
    def open_stylx(self) -> bool:
        """打开.stylx文件（SQLite数据库）"""
        try:
            if not os.path.exists(self.stylx_path):
                self.logger.error(f"文件不存在: {self.stylx_path}")
                return False
            
            self.conn = sqlite3.connect(self.stylx_path)
            self.logger.info(f"已打开.stylx文件: {self.stylx_path}")
            return True
            
        except Exception as e:
            self.logger.error(f"打开.stylx文件失败: {e}")
            return False
    
    def get_symbols(self) -> List[Dict[str, Any]]:
        """获取所有符号"""
        symbols = []
        
        if not self.conn:
            if not self.open_stylx():
                return symbols
        
        try:
            cursor = self.conn.cursor()
            
            # .stylx文件结构：包含items表和resources表
            cursor.execute("""
                SELECT itemID, name, category, tags, key 
                FROM items 
                ORDER BY name
            """)
            
            items = cursor.fetchall()
            
            for item in items:
                item_id, name, category, tags, key = item
                
                # 获取符号数据
                cursor.execute("""
                    SELECT data 
                    FROM resources 
                    WHERE resourceID = ?
                """, (key,))
                
                resource_result = cursor.fetchone()
                if resource_result:
                    symbol_data = resource_result[0]
                    
                    try:
                        # 解析JSON数据
                        symbol_json = json.loads(symbol_data)
                        
                        symbols.append({
                            'id': item_id,
                            'name': name or f"symbol_{item_id}",
                            'category': category or '',
                            'tags': tags.split(',') if tags else [],
                            'symbol_json': symbol_json,
                            'type': symbol_json.get('type', 'Unknown')
                        })
                        
                    except json.JSONDecodeError as e:
                        self.logger.warning(f"解析符号 {item_id} 的JSON数据失败: {e}")
            
            cursor.close()
            self.logger.info(f"从.stylx文件中提取了 {len(symbols)} 个符号")
            
        except Exception as e:
            self.logger.error(f"获取符号失败: {e}")
        
        return symbols
    
    def parse_symbol_json(self, symbol_data: Dict[str, Any]) -> SymbolInfo:
        """
        解析符号JSON数据
        
        Args:
            symbol_data: 符号JSON数据
            
        Returns:
            符号信息对象
        """
        symbol = SymbolInfo()
        symbol.arcgis_symbol_json = symbol_data
        
        # 提取基本信息
        symbol.symbol_id = str(symbol_data.get('id', ''))
        symbol.name = symbol_data.get('name', '')
        symbol.category = symbol_data.get('category', '')
        
        # 解析符号类型
        symbol_type = symbol_data.get('type', '')
        symbol.arcgis_symbol_type = symbol_type
        
        # 确定样式类型
        if 'Marker' in symbol_type:
            symbol.style_type = StyleType.MARKER
        elif 'Line' in symbol_type:
            symbol.style_type = StyleType.LINE
        elif 'Fill' in symbol_type:
            symbol.style_type = StyleType.FILL
        elif 'Text' in symbol_type:
            symbol.style_type = StyleType.TEXT
        else:
            symbol.style_type = StyleType.MARKER  # 默认
        
        # 解析颜色
        color_data = symbol_data.get('color')
        if color_data:
            symbol.color = self._parse_color(color_data)
        
        # 解析大小/宽度
        symbol.size = symbol_data.get('size', 1.0)
        symbol.width = symbol_data.get('width', 1.0)
        
        # 解析标签
        tags = symbol_data.get('tags', [])
        if isinstance(tags, str):
            symbol.tags = [tag.strip() for tag in tags.split(',') if tag.strip()]
        elif isinstance(tags, list):
            symbol.tags = tags
        
        return symbol
    
    def _parse_color(self, color_data: Any) -> Optional[ColorInfo]:
        """解析颜色数据"""
        if isinstance(color_data, dict):
            # 颜色对象
            return ColorInfo(
                red=int(color_data.get('r', 0) * 255),
                green=int(color_data.get('g', 0) * 255),
                blue=int(color_data.get('b', 0) * 255),
                alpha=int(color_data.get('a', 1.0) * 255)
            )
        elif isinstance(color_data, list) and len(color_data) >= 3:
            # RGB(A)数组
            return ColorInfo(
                red=min(255, max(0, int(color_data[0]))),
                green=min(255, max(0, int(color_data[1]))),
                blue=min(255, max(0, int(color_data[2]))),
                alpha=min(255, max(0, int(color_data[3]))) if len(color_data) > 3 else 255
            )
        elif isinstance(color_data, str):
            # 十六进制颜色字符串
            try:
                color_str = color_data.lstrip('#')
                if len(color_str) == 6:
                    r = int(color_str[0:2], 16)
                    g = int(color_str[2:4], 16)
                    b = int(color_str[4:6], 16)
                    return ColorInfo(red=r, green=g, blue=b, alpha=255)
                elif len(color_str) == 8:
                    r = int(color_str[0:2], 16)
                    g = int(color_str[2:4], 16)
                    b = int(color_str[4:6], 16)
                    a = int(color_str[6:8], 16)
                    return ColorInfo(red=r, green=g, blue=b, alpha=a)
            except (ValueError, IndexError):
                pass
        
        return None
    
    def close(self):
        """关闭数据库连接"""
        if self.conn:
            self.conn.close()
            self.logger.info(f"已关闭.stylx文件连接")
            self.conn = None


class QgisStyleGenerator:
    """QGIS样式生成器"""
    
    def __init__(self):
        self.logger = logging.getLogger(__name__)
        self.mapping_table = StyleMappingTable()
    
    def generate_symbol_xml(self, symbol: SymbolInfo) -> str:
        """
        生成QGIS符号XML
        
        Args:
            symbol: 符号信息
            
        Returns:
            QGIS符号XML字符串
        """
        try:
            if symbol.style_type == StyleType.MARKER:
                return self._generate_marker_symbol_xml(symbol)
            elif symbol.style_type == StyleType.LINE:
                return self._generate_line_symbol_xml(symbol)
            elif symbol.style_type == StyleType.FILL:
                return self._generate_fill_symbol_xml(symbol)
            elif symbol.style_type == StyleType.TEXT:
                return self._generate_text_symbol_xml(symbol)
            else:
                return self._generate_default_symbol_xml(symbol)
                
        except Exception as e:
            self.logger.error(f"生成符号XML失败: {e}")
            return self._generate_default_symbol_xml(symbol)
    
    def _generate_marker_symbol_xml(self, symbol: SymbolInfo) -> str:
        """生成点符号XML"""
        # 确定QGIS符号类型
        qgis_symbol_type = self.mapping_table.convert_symbol_type(symbol.arcgis_symbol_type)
        
        # 基础属性
        color = symbol.color.to_qgis_color() if symbol.color else "0,0,0,255"
        size = max(1.0, symbol.size)
        rotation = symbol.rotation
        
        if qgis_symbol_type == "QgsSimpleMarkerSymbolLayer":
            # 简单点符号
            shape = "circle"
            if symbol.arcgis_symbol_json and 'style' in symbol.arcgis_symbol_json:
                shape = self.mapping_table.convert_marker_shape(
                    symbol.arcgis_symbol_json.get('style', 'circle')
                )
            
            xml = f'''
            <symbol name="{symbol.name}" type="marker" alpha="1" force_rhr="0">
              <layer class="QgsSimpleMarkerSymbolLayer" enabled="1" locked="0" pass="0">
                <prop k="angle" v="{rotation}"/>
                <prop k="cap_style" v="square"/>
                <prop k="color" v="{color}"/>
                <prop k="horizontal_anchor_point" v="1"/>
                <prop k="joinstyle" v="bevel"/>
                <prop k="name" v="{shape}"/>
                <prop k="offset" v="0,0"/>
                <prop k="offset_map_unit_scale" v="3x:0,0,0,0,0,0"/>
                <prop k="offset_unit" v="MM"/>
                <prop k="outline_color" v="35,35,35,255"/>
                <prop k="outline_style" v="solid"/>
                <prop k="outline_width" v="0"/>
                <prop k="outline_width_map_unit_scale" v="3x:0,0,0,0,0,0"/>
                <prop k="outline_width_unit" v="MM"/>
                <prop k="scale_method" v="diameter"/>
                <prop k="size" v="{size}"/>
                <prop k="size_map_unit_scale" v="3x:0,0,0,0,0,0"/>
                <prop k="size_unit" v="MM"/>
                <prop k="vertical_anchor_point" v="1"/>
              </layer>
            </symbol>
            '''
            
        elif qgis_symbol_type == "QgsFontMarkerSymbolLayer":
            # 字体点符号
            xml = f'''
            <symbol name="{symbol.name}" type="marker" alpha="1" force_rhr="0">
              <layer class="QgsFontMarkerSymbolLayer" enabled="1" locked="0" pass="0">
                <prop k="angle" v="{rotation}"/>
                <prop k="chr" v="A"/>
                <prop k="color" v="{color}"/>
                <prop k="font" v="Arial"/>
                <prop k="horizontal_anchor_point" v="1"/>
                <prop k="joinstyle" v="bevel"/>
                <prop k="offset" v="0,0"/>
                <prop k="offset_map_unit_scale" v="3x:0,0,0,0,0,0"/>
                <prop k="offset_unit" v="MM"/>
                <prop k="outline_color" v="35,35,35,255"/>
                <prop k="outline_width" v="0"/>
                <prop k="outline_width_map_unit_scale" v="3x:0,0,0,0,0,0"/>
                <prop k="outline_width_unit" v="MM"/>
                <prop k="size" v="{size}"/>
                <prop k="size_map_unit_scale" v="3x:0,0,0,0,0,0"/>
                <prop k="size_unit" v="MM"/>
                <prop k="vertical_anchor_point" v="1"/>
              </layer>
            </symbol>
            '''
            
        else:
            # 默认点符号
            xml = self._generate_default_symbol_xml(symbol)
        
        return xml.strip()
    
    def _generate_line_symbol_xml(self, symbol: SymbolInfo) -> str:
        """生成线符号XML"""
        # 确定QGIS符号类型
        qgis_symbol_type = self.mapping_table.convert_symbol_type(symbol.arcgis_symbol_type)
        
        # 基础属性
        color = symbol.color.to_qgis_color() if symbol.color else "0,0,0,255"
        width = max(0.1, symbol.width)
        rotation = symbol.rotation
        
        if qgis_symbol_type == "QgsSimpleLineSymbolLayer":
            # 简单线符号
            line_style = "solid"
            if symbol.arcgis_symbol_json and 'style' in symbol.arcgis_symbol_json:
                line_style = self.mapping_table.convert_line_style(
                    symbol.arcgis_symbol_json.get('style', 'solid')
                )
            
            xml = f'''
            <symbol name="{symbol.name}" type="line" alpha="1" force_rhr="0">
              <layer class="QgsSimpleLineSymbolLayer" enabled="1" locked="0" pass="0">
                <prop k="align_dash_pattern" v="0"/>
                <prop k="capstyle" v="square"/>
                <prop k="customdash" v="5;2"/>
                <prop k="customdash_map_unit_scale" v="3x:0,0,0,0,0,0"/>
                <prop k="customdash_unit" v="MM"/>
                <prop k="dash_pattern_offset" v="0"/>
                <prop k="dash_pattern_offset_map_unit_scale" v="3x:0,0,0,0,0,0"/>
                <prop k="dash_pattern_offset_unit" v="MM"/>
                <prop k="draw_inside_polygon" v="0"/>
                <prop k="joinstyle" v="bevel"/>
                <prop k="line_color" v="{color}"/>
                <prop k="line_style" v="{line_style}"/>
                <prop k="line_width" v="{width}"/>
                <prop k="line_width_unit" v="MM"/>
                <prop k="offset" v="0"/>
                <prop k="offset_map_unit_scale" v="3x:0,0,0,0,0,0"/>
                <prop k="offset_unit" v="MM"/>
                <prop k="ring_filter" v="0"/>
                <prop k="trim_distance_end" v="0"/>
                <prop k="trim_distance_end_map_unit_scale" v="3x:0,0,0,0,0,0"/>
                <prop k="trim_distance_end_unit" v="MM"/>
                <prop k="trim_distance_start" v="0"/>
                <prop k="trim_distance_start_map_unit_scale" v="3x:0,0,0,0,0,0"/>
                <prop k="trim_distance_start_unit" v="MM"/>
                <prop k="tweak_dash_pattern_on_corners" v="0"/>
                <prop k="use_custom_dash" v="0"/>
                <prop k="width_map_unit_scale" v="3x:0,0,0,0,0,0"/>
              </layer>
            </symbol>
            '''
            
        else:
            # 默认线符号
            xml = self._generate_default_symbol_xml(symbol)
        
        return xml.strip()
    
    def _generate_fill_symbol_xml(self, symbol: SymbolInfo) -> str:
        """生成填充符号XML"""
        # 确定QGIS符号类型
        qgis_symbol_type = self.mapping_table.convert_symbol_type(symbol.arcgis_symbol_type)
        
        # 基础属性
        color = symbol.color.to_qgis_color() if symbol.color else "0,0,0,255"
        width = max(0.1, symbol.width)
        
        if qgis_symbol_type == "QgsSimpleFillSymbolLayer":
            # 简单填充符号
            fill_style = "solid"
            if symbol.arcgis_symbol_json and 'style' in symbol.arcgis_symbol_json:
                fill_style = self.mapping_table.convert_fill_style(
                    symbol.arcgis_symbol_json.get('style', 'solid')
                )
            
            xml = f'''
            <symbol name="{symbol.name}" type="fill" alpha="1" force_rhr="0">
              <layer class="QgsSimpleFillSymbolLayer" enabled="1" locked="0" pass="0">
                <prop k="border_width_map_unit_scale" v="3x:0,0,0,0,0,0"/>
                <prop k="color" v="{color}"/>
                <prop k="joinstyle" v="bevel"/>
                <prop k="offset" v="0,0"/>
                <prop k="offset_map_unit_scale" v="3x:0,0,0,0,0,0"/>
                <prop k="offset_unit" v="MM"/>
                <prop k="outline_color" v="35,35,35,255"/>
                <prop k="outline_style" v="{fill_style}"/>
                <prop k="outline_width" v="0.26"/>
                <prop k="outline_width_unit" v="MM"/>
                <prop k="style" v="{fill_style}"/>
              </layer>
            </symbol>
            '''
            
        else:
            # 默认填充符号
            xml = self._generate_default_symbol_xml(symbol)
        
        return xml.strip()
    
    def _generate_text_symbol_xml(self, symbol: SymbolInfo) -> str:
        """生成文本符号XML"""
        # 基础属性
        color = symbol.color.to_qgis_color() if symbol.color else "0,0,0,255"
        size = max(1.0, symbol.size)
        
        xml = f'''
        <symbol name="{symbol.name}" type="marker" alpha="1" force_rhr="0">
          <layer class="QgsFontMarkerSymbolLayer" enabled="1" locked="0" pass="0">
            <prop k="angle" v="0"/>
            <prop k="chr" v="T"/>
            <prop k="color" v="{color}"/>
            <prop k="font" v="Arial"/>
            <prop k="horizontal_anchor_point" v="1"/>
            <prop k="joinstyle" v="bevel"/>
            <prop k="offset" v="0,0"/>
            <prop k="offset_map_unit_scale" v="3x:0,0,0,0,0,0"/>
            <prop k="offset_unit" v="MM"/>
            <prop k="outline_color" v="35,35,35,255"/>
            <prop k="outline_width" v="0"/>
            <prop k="outline_width_map_unit_scale" v="3x:0,0,0,0,0,0"/>
            <prop k="outline_width_unit" v="MM"/>
            <prop k="size" v="{size}"/>
            <prop k="size_map_unit_scale" v="3x:0,0,0,0,0,0"/>
            <prop k="size_unit" v="MM"/>
            <prop k="vertical_anchor_point" v="1"/>
          </layer>
        </symbol>
        '''
        
        return xml.strip()
    
    def _generate_default_symbol_xml(self, symbol: SymbolInfo) -> str:
        """生成默认符号XML"""
        color = symbol.color.to_qgis_color() if symbol.color else "0,0,0,255"
        
        xml = f'''
        <symbol name="{symbol.name}" type="marker" alpha="1" force_rhr="0">
          <layer class="QgsSimpleMarkerSymbolLayer" enabled="1" locked="0" pass="0">
            <prop k="angle" v="0"/>
            <prop k="cap_style" v="square"/>
            <prop k="color" v="{color}"/>
            <prop k="horizontal_anchor_point" v="1"/>
            <prop k="joinstyle" v="bevel"/>
            <prop k="name" v="circle"/>
            <prop k="offset" v="0,0"/>
            <prop k="offset_map_unit_scale" v="3x:0,0,0,0,0,0"/>
            <prop k="offset_unit" v="MM"/>
            <prop k="outline_color" v="35,35,35,255"/>
            <prop k="outline_style" v="solid"/>
            <prop k="outline_width" v="0"/>
            <prop k="outline_width_map_unit_scale" v="3x:0,0,0,0,0,0"/>
            <prop k="outline_width_unit" v="MM"/>
            <prop k="scale_method" v="diameter"/>
            <prop k="size" v="2"/>
            <prop k="size_map_unit_scale" v="3x:0,0,0,0,0,0"/>
            <prop k="size_unit" v="MM"/>
            <prop k="vertical_anchor_point" v="1"/>
          </layer>
        </symbol>
        '''
        
        return xml.strip()
    
    def generate_style_xml(self, style_library: StyleLibrary) -> str:
        """
        生成完整的QGIS样式XML
        
        Args:
            style_library: 样式库
            
        Returns:
            完整的QGIS样式XML
        """
        xml_parts = []
        
        # XML头部
        xml_parts.append('<?xml version="1.0" encoding="UTF-8"?>')
        xml_parts.append('<qgis_style version="2">')
        xml_parts.append('<symbols>')
        
        # 添加所有符号
        for symbol in style_library.symbols:
            symbol_xml = symbol.qgis_symbol_xml
            if not symbol_xml:
                symbol_xml = self.generate_symbol_xml(symbol)
                symbol.qgis_symbol_xml = symbol_xml
            
            xml_parts.append(symbol_xml)
        
        xml_parts.append('</symbols>')
        
        # 颜色色板（可选）
        xml_parts.append('<colorramps/>')
        
        # 文本格式（可选）
        xml_parts.append('<textformats/>')
        
        # 符号层（可选）
        xml_parts.append('<symbolLayerSettings/>')
        
        # 元数据
        xml_parts.append('<metadata>')
        xml_parts.append(f'<name>{style_library.name}</name>')
        xml_parts.append(f'<source>{style_library.source_path}</source>')
        xml_parts.append(f'<version>{style_library.version}</version>')
        xml_parts.append(f'<conversion_time>{style_library.conversion_time.isoformat() if style_library.conversion_time else ""}</conversion_time>')
        xml_parts.append(f'<symbol_count>{style_library.symbol_count}</symbol_count>')
        xml_parts.append('</metadata>')
        
        xml_parts.append('</qgis_style>')
        
        return '\n'.join(xml_parts)


class StyleConverter:
    """样式转换器（主类）"""
    
    def __init__(self):
        self.logger = logging.getLogger(__name__)
        self.stylx_parser = None
        self.qgis_generator = QgisStyleGenerator()
        self.mapping_table = StyleMappingTable()
    
    def convert_stylx_to_qgis(self, stylx_path: str, output_path: Optional[str] = None) -> StyleLibrary:
        """
        转换.stylx文件到QGIS样式
        
        Args:
            stylx_path: .stylx文件路径
            output_path: 输出文件路径（可选）
            
        Returns:
            样式库对象
        """
        self.logger.info(f"开始转换.stylx文件: {stylx_path}")
        
        # 创建样式库
        style_library = StyleLibrary(
            name=os.path.splitext(os.path.basename(stylx_path))[0],
            source_path=stylx_path,
            conversion_time=datetime.now()
        )
        
        # 解析.stylx文件
        self.stylx_parser = StylxParser(stylx_path)
        if not self.stylx_parser.open_stylx():
            self.logger.error(f"无法打开.stylx文件: {stylx_path}")
            return style_library
        
        try:
            # 获取所有符号
            symbols_data = self.stylx_parser.get_symbols()
            
            for symbol_data in symbols_data:
                try:
                    # 解析符号
                    symbol = self.stylx_parser.parse_symbol_json(symbol_data['symbol_json'])
                    
                    # 设置符号ID和名称
                    symbol.symbol_id = symbol_data['id']
                    if not symbol.name:
                        symbol.name = symbol_data['name']
                    
                    # 生成QGIS符号XML
                    symbol.qgis_symbol_xml = self.qgis_generator.generate_symbol_xml(symbol)
                    
                    # 添加到样式库
                    style_library.symbols.append(symbol)
                    
                except Exception as e:
                    self.logger.warning(f"处理符号 {symbol_data.get('name', 'unknown')} 失败: {e}")
            
            self.logger.info(f"成功转换 {len(style_library.symbols)} 个符号")
            
            # 生成完整的QGIS样式XML
            qgis_xml = self.qgis_generator.generate_style_xml(style_library)
            
            # 保存到文件
            if output_path:
                os.makedirs(os.path.dirname(output_path), exist_ok=True)
                with open(output_path, 'w', encoding='utf-8') as f:
                    f.write(qgis_xml)
                self.logger.info(f"QGIS样式已保存到: {output_path}")
            
            return style_library
            
        finally:
            self.stylx_parser.close()
    
    def convert_cim_to_qgis(self, cim_json: str) -> Dict[str, Any]:
        """
        转换CIM JSON到QGIS渲染器配置
        
        Args:
            cim_json: CIM JSON字符串
            
        Returns:
            QGIS渲染器配置
        """
        try:
            cim_data = json.loads(cim_json)
            return self._parse_cim_renderer(cim_data)
        except Exception as e:
            self.logger.error(f"转换CIM JSON失败: {e}")
            return {}
    
    def _parse_cim_renderer(self, cim_data: Dict[str, Any]) -> Dict[str, Any]:
        """解析CIM渲染器"""
        # 简化实现，实际需要根据CIM规范解析
        renderer_config = {
            'type': 'singleSymbol',  # 默认
            'symbol': {},
            'classification_method': 'equal',
            'classes': []
        }
        
        if 'renderer' in cim_data:
            renderer_info = cim_data['renderer']
            renderer_type = renderer_info.get('type', 'SimpleRenderer')
            
            if renderer_type == 'SimpleRenderer':
                renderer_config['type'] = 'singleSymbol'
            elif renderer_type == 'ClassBreaksRenderer':
                renderer_config['type'] = 'graduatedSymbol'
            elif renderer_type == 'UniqueValueRenderer':
                renderer_config['type'] = 'categorizedSymbol'
            
            # 解析符号信息
            if 'symbol' in renderer_info:
                renderer_config['symbol'] = self._parse_cim_symbol(renderer_info['symbol'])
        
        return renderer_config
    
    def _parse_cim_symbol(self, symbol_data: Dict[str, Any]) -> Dict[str, Any]:
        """解析CIM符号"""
        symbol_info = {
            'type': 'marker',
            'color': {'r': 0, 'g': 0, 'b': 0, 'a': 255},
            'size': 2.0,
            'width': 0.26
        }
        
        # 简化实现
        if 'color' in symbol_data:
            symbol_info['color'] = symbol_data['color']
        
        if 'size' in symbol_data:
            symbol_info['size'] = symbol_data['size']
        
        if 'width' in symbol_data:
            symbol_info['width'] = symbol_data['width']
        
        return symbol_info
    
    def create_symbol_mapping_report(self, style_library: StyleLibrary) -> str:
        """创建符号映射报告"""
        report_lines = []
        report_lines.append("=" * 60)
        report_lines.append("样式转换映射报告")
        report_lines.append("=" * 60)
        report_lines.append(f"生成时间: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        report_lines.append(f"源文件: {style_library.source_path}")
        report_lines.append(f"符号总数: {style_library.symbol_count}")
        report_lines.append("")
        
        # 按类型统计
        symbols_by_type = style_library.symbols_by_type
        report_lines.append("符号类型分布:")
        for style_type, symbols in symbols_by_type.items():
            if symbols:
                report_lines.append(f"  {style_type.value}: {len(symbols)} 个")
        
        report_lines.append("")
        
        # 详细映射信息
        report_lines.append("详细符号映射:")
        for symbol in style_library.symbols:
            report_lines.append(f"  {symbol.name}")
            report_lines.append(f"    ArcGIS Pro类型: {symbol.arcgis_symbol_type}")
            report_lines.append(f"    QGIS类型: {self.mapping_table.convert_symbol_type(symbol.arcgis_symbol_type)}")
            report_lines.append(f"    样式类型: {symbol.style_type.value}")
            
            if symbol.color:
                report_lines.append(f"    颜色: {symbol.color.to_hex()}")
            
            report_lines.append(f"    大小: {symbol.size}")
            
            if symbol.tags:
                report_lines.append(f"    标签: {', '.join(symbol.tags)}")
        
        return "\n".join(report_lines)


# 示例使用代码
if __name__ == "__main__":
    # 设置日志
    logging.basicConfig(level=logging.INFO)
    
    # 创建样式转换器
    converter = StyleConverter()
    
    # 示例：转换.stylx文件
    stylx_path = "example.stylx"  # 替换为实际路径
    
    if os.path.exists(stylx_path):
        output_path = "converted_style.xml"
        
        print(f"开始转换.stylx文件: {stylx_path}")
        style_library = converter.convert_stylx_to_qgis(stylx_path, output_path)
        
        # 生成报告
        report = converter.create_symbol_mapping_report(style_library)
        print(report)
        
        print(f"\n转换完成！共转换 {style_library.symbol_count} 个符号。")
        if os.path.exists(output_path):
            print(f"QGIS样式文件已保存到: {output_path}")
    else:
        print(f"示例文件不存在: {stylx_path}")
        print("请创建一个示例.stylx文件或指定正确的路径。")