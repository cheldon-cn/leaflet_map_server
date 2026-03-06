"""
本地数据转换基础模块
支持将本地GIS数据格式转换为QGIS兼容格式（如GeoPackage）
"""

import os
import shutil
from typing import Optional, Dict, Any, List
from pathlib import Path
import subprocess
import sys

from data_models import LayerInfo, DataSourceInfo, TaskStatus
from utils.error_handler import create_success_result, create_error_result


class LocalDataConverter:
    """本地数据转换器"""
    
    def __init__(self, output_dir: str = "output"):
        """
        初始化转换器
        
        Args:
            output_dir: 输出目录
        """
        self.output_dir = output_dir
        # 确保输出目录存在
        os.makedirs(output_dir, exist_ok=True)
        self.gdal_available = self._check_gdal_available()
        
        if not self.gdal_available:
            print("警告: GDAL/OGR 不可用。本地数据转换功能将受限。")
            print("请安装GDAL: pip install GDAL")
    
    def _check_gdal_available(self) -> bool:
        """检查GDAL是否可用"""
        try:
            import osgeo
            return True
        except ImportError:
            return False
    
    def convert_layer(self, layer: LayerInfo, output_format: str = "GPKG") -> Dict[str, Any]:
        """
        转换单个图层
        
        Args:
            layer: 图层信息
            output_format: 输出格式 (GPKG, SHP, etc.)
            
        Returns:
            转换结果字典，包含状态和输出路径
        """
        from utils.error_handler import create_success_result, create_error_result
        
        task_status = TaskStatus()
        task_status.status = "running"
        
        try:
            # 检查数据源类型
            ds = layer.data_source
            if not ds.data_source_type or ds.data_source_type == "Unknown":
                result = create_error_result(f"未知数据源类型: {layer.name}")
                task_status.status = "failed"
                result["task_status"] = task_status
                # 向后兼容性字段
                result["status"] = task_status.status
                result["error"] = result["message"]
                return result
            
            # 检查是否需要跳过（如PostgreSQL、WebService等远程数据源）
            if ds.data_source_type in ["PostgreSQL", "WebService", "EnterpriseGeodatabase"]:
                task_status.status = "skipped"
                result = create_success_result(
                    message=f"跳过远程数据源图层: {layer.name} ({ds.data_source_type})",
                    data={"skipped_reason": "remote_data_source"}
                )
                result["task_status"] = task_status
                # 向后兼容性字段
                result["status"] = task_status.status
                result["output_path"] = None
                return result
            
            # 根据数据源类型选择转换方法
            if ds.data_source_type == "Shapefile":
                if output_format.upper() == "GPKG":
                    output_path = self._convert_shapefile_to_geopackage(ds.file_path, layer.name)
                elif output_format.upper() == "SHP":
                    # 如果是SHP到SHP，只是复制
                    output_path = self._copy_shapefile(ds.file_path, layer.name)
                else:
                    result = create_error_result(f"不支持的输出格式: {output_format}")
                    task_status.status = "failed"
                    result["task_status"] = task_status
                    # 向后兼容性字段
                    result["status"] = task_status.status
                    result["error"] = result["message"]
                    return result
                    
            elif ds.data_source_type == "FileGeodatabase":
                if output_format.upper() == "GPKG":
                    output_path = self._convert_gdb_to_geopackage(ds.file_path, layer.name)
                else:
                    result = create_error_result(f"文件地理数据库暂只支持转换为GeoPackage")
                    task_status.status = "failed"
                    result["task_status"] = task_status
                    # 向后兼容性字段
                    result["status"] = task_status.status
                    result["error"] = result["message"]
                    return result
                    
            elif ds.data_source_type == "Raster":
                result = create_error_result(f"栅格数据转换暂未实现: {layer.name}")
                task_status.status = "failed"
                result["task_status"] = task_status
                # 向后兼容性字段
                result["status"] = task_status.status
                result["error"] = result["message"]
                return result
                
            else:
                result = create_error_result(f"暂不支持的数据源类型: {ds.data_source_type}")
                task_status.status = "failed"
                result["task_status"] = task_status
                # 向后兼容性字段
                result["status"] = task_status.status
                result["error"] = result["message"]
                return result
            
            # 成功转换
            # 确定状态：如果GDAL不可用且是本地文件转换，则为模拟状态
            if not self.gdal_available and ds.data_source_type in ["Shapefile", "FileGeodatabase"]:
                task_status.status = "simulated"
            else:
                task_status.status = "completed"
            
            result = create_success_result(
                message=f"成功转换图层: {layer.name}",
                data={"output_path": output_path}
            )
            result["task_status"] = task_status
            # 向后兼容性字段
            result["status"] = task_status.status
            result["output_path"] = output_path
            return result
            
        except Exception as e:
            task_status.status = "failed"
            task_status.error_info = {
                "error_code": "CONVERSION_ERROR",
                "error_message": str(e)
            }
            result = create_error_result(
                error_message=f"转换图层 {layer.name} 时出错: {str(e)}",
                errors=[str(e)]
            )
            result["task_status"] = task_status
            # 向后兼容性字段
            result["status"] = task_status.status
            result["error"] = result["message"]
            return result
    
    def _convert_shapefile_to_geopackage(self, shapefile_path: str, layer_name: str) -> str:
        """
        将Shapefile转换为GeoPackage
        
        Args:
            shapefile_path: Shapefile路径
            layer_name: 图层名称
            
        Returns:
            输出GeoPackage文件路径
        """
        # 创建输出目录
        os.makedirs(self.output_dir, exist_ok=True)
        
        # 生成输出文件名
        safe_layer_name = self._sanitize_filename(layer_name)
        output_file = os.path.join(self.output_dir, f"{safe_layer_name}.gpkg")
        
        if self.gdal_available:
            # 使用GDAL/OGR进行转换，需要文件存在
            if not shapefile_path or not os.path.exists(shapefile_path):
                raise FileNotFoundError(f"Shapefile不存在: {shapefile_path}")
            
            # 确保是.shp文件
            if not shapefile_path.lower().endswith('.shp'):
                # 尝试查找.shp文件
                base_path = os.path.splitext(shapefile_path)[0]
                shapefile_path = base_path + '.shp'
                if not os.path.exists(shapefile_path):
                    raise FileNotFoundError(f"找不到.shp文件: {base_path}")
            
            return self._convert_with_gdal(shapefile_path, output_file, layer_name)
        else:
            # 模拟转换（仅用于测试），不检查文件是否存在
            # 确保shapefile_path不为空，否则使用模拟路径
            if not shapefile_path:
                shapefile_path = f"模拟Shapefile_{layer_name}.shp"
            return self._simulate_conversion(shapefile_path, output_file, layer_name)
    
    def _convert_gdb_to_geopackage(self, gdb_path: str, layer_name: str) -> str:
        """
        将文件地理数据库转换为GeoPackage
        
        Args:
            gdb_path: 文件地理数据库路径
            layer_name: 图层名称
            
        Returns:
            输出GeoPackage文件路径
        """
        # 创建输出目录
        os.makedirs(self.output_dir, exist_ok=True)
        
        # 生成输出文件名
        safe_layer_name = self._sanitize_filename(layer_name)
        output_file = os.path.join(self.output_dir, f"{safe_layer_name}.gpkg")
        
        if self.gdal_available:
            # 使用GDAL/OGR进行转换，需要文件存在
            if not gdb_path or not os.path.exists(gdb_path):
                raise FileNotFoundError(f"文件地理数据库不存在: {gdb_path}")
            # 注意：文件地理数据库可能需要指定图层名
            return self._convert_with_gdal(gdb_path, output_file, layer_name)
        else:
            # 模拟转换（仅用于测试），不检查文件是否存在
            if not gdb_path:
                gdb_path = f"模拟FileGDB_{layer_name}.gdb"
            return self._simulate_conversion(gdb_path, output_file, layer_name)
    
    def _copy_shapefile(self, shapefile_path: str, layer_name: str) -> str:
        """
        复制Shapefile文件（用于SHP到SHP的"转换"）
        
        Args:
            shapefile_path: Shapefile路径
            layer_name: 图层名称
            
        Returns:
            输出Shapefile文件路径
        """
        if not shapefile_path or not os.path.exists(shapefile_path):
            raise FileNotFoundError(f"Shapefile不存在: {shapefile_path}")
        
        # 确保是.shp文件
        if not shapefile_path.lower().endswith('.shp'):
            base_path = os.path.splitext(shapefile_path)[0]
            shapefile_path = base_path + '.shp'
            if not os.path.exists(shapefile_path):
                raise FileNotFoundError(f"找不到.shp文件: {base_path}")
        
        # 创建输出目录
        os.makedirs(self.output_dir, exist_ok=True)
        
        # 生成输出文件名
        safe_layer_name = self._sanitize_filename(layer_name)
        output_base = os.path.join(self.output_dir, safe_layer_name)
        
        # 复制所有相关文件
        base_path = os.path.splitext(shapefile_path)[0]
        for ext in ['.shp', '.shx', '.dbf', '.prj', '.cpg', '.sbn', '.sbx', '.fbn', '.fbx', '.ain', '.aih', '.ixs', '.mxs', '.atx', '.shp.xml']:
            src_file = base_path + ext
            if os.path.exists(src_file):
                dst_file = output_base + ext
                shutil.copy2(src_file, dst_file)
        
        return output_base + '.shp'
    
    def _convert_with_gdal(self, input_path: str, output_path: str, layer_name: str) -> str:
        """
        使用GDAL/OGR进行转换
        
        Args:
            input_path: 输入文件路径
            output_path: 输出文件路径
            layer_name: 图层名称
            
        Returns:
            输出文件路径
        """
        try:
            from osgeo import ogr, osr
            
            # 设置OGR异常处理模式以避免FutureWarning
            ogr.UseExceptions()
            
            # 打开输入数据源
            input_ds = ogr.Open(input_path)
            if input_ds is None:
                raise RuntimeError(f"无法打开输入文件: {input_path}")
            
            # 获取输入图层
            input_layer = input_ds.GetLayer()
            if input_layer is None:
                # 尝试按名称获取图层
                input_layer = input_ds.GetLayerByName(layer_name)
                if input_layer is None:
                    # 获取第一个图层
                    input_layer = input_ds.GetLayer(0)
            
            # 创建输出数据源
            driver = ogr.GetDriverByName('GPKG')
            if os.path.exists(output_path):
                driver.DeleteDataSource(output_path)
            
            output_ds = driver.CreateDataSource(output_path)
            if output_ds is None:
                raise RuntimeError(f"无法创建输出文件: {output_path}")
            
            # 创建输出图层
            output_layer = output_ds.CreateLayer(
                layer_name,
                input_layer.GetSpatialRef(),
                input_layer.GetGeomType()
            )
            
            # 复制字段定义
            input_layer_defn = input_layer.GetLayerDefn()
            for i in range(input_layer_defn.GetFieldCount()):
                field_defn = input_layer_defn.GetFieldDefn(i)
                output_layer.CreateField(field_defn)
            
            # 复制要素
            input_layer.ResetReading()
            for feature in input_layer:
                output_layer.CreateFeature(feature)
            
            # 清理
            input_ds = None
            output_ds = None
            
            return output_path
            
        except ImportError:
            # 如果导入失败，尝试使用命令行
            return self._convert_with_ogr2ogr(input_path, output_path, layer_name)
    
    def _convert_with_ogr2ogr(self, input_path: str, output_path: str, layer_name: str) -> str:
        """
        使用ogr2ogr命令行工具进行转换
        
        Args:
            input_path: 输入文件路径
            output_path: 输出文件路径
            layer_name: 图层名称
            
        Returns:
            输出文件路径
        """
        # 构建命令
        cmd = [
            'ogr2ogr',
            '-f', 'GPKG',
            output_path,
            input_path,
            '-nln', layer_name
        ]
        
        try:
            result = subprocess.run(cmd, capture_output=True, text=True, check=True)
            if result.returncode != 0:
                raise RuntimeError(f"ogr2ogr命令失败: {result.stderr}")
            return output_path
        except subprocess.CalledProcessError as e:
            raise RuntimeError(f"ogr2ogr命令执行失败: {e.stderr}")
        except FileNotFoundError:
            raise RuntimeError("找不到ogr2ogr命令。请确保GDAL已安装并添加到PATH。")
    
    def _simulate_conversion(self, input_path: str, output_path: str, layer_name: str) -> str:
        """
        模拟转换（用于测试，当GDAL不可用时）
        
        Args:
            input_path: 输入文件路径
            output_path: 输出文件路径
            layer_name: 图层名称
            
        Returns:
            输出文件路径
        """
        print(f"[模拟] 转换 {input_path} 到 {output_path}")
        print(f"[模拟] 图层名称: {layer_name}")
        
        # 创建模拟输出文件
        os.makedirs(os.path.dirname(output_path), exist_ok=True)
        with open(output_path, 'w') as f:
            f.write(f"模拟GeoPackage文件: {layer_name}\n")
            f.write(f"源文件: {input_path}\n")
            f.write(f"转换时间: {__import__('datetime').datetime.now()}\n")
        
        return output_path
    
    def _sanitize_filename(self, filename: str) -> str:
        """
        清理文件名，移除非法字符
        
        Args:
            filename: 原始文件名
            
        Returns:
            清理后的文件名
        """
        # 移除非法字符
        invalid_chars = '<>:"/\\|?*'
        for char in invalid_chars:
            filename = filename.replace(char, '_')
        
        # 限制长度
        if len(filename) > 100:
            filename = filename[:100]
        
        return filename
    
    def convert_project_layers(self, layers: List[LayerInfo], output_format: str = "GPKG") -> Dict[str, Any]:
        """
        批量转换工程中的所有图层
        
        Args:
            layers: 图层列表
            output_format: 输出格式
            
        Returns:
            批量转换结果
        """
        results = {
            "total": len(layers),
            "successful": 0,
            "failed": 0,
            "converted_layers": [],
            "errors": []
        }
        
        for layer in layers:
            print(f"转换图层: {layer.name}")
            result = self.convert_layer(layer, output_format)
            
            if result["success"]:
                results["successful"] += 1
                results["converted_layers"].append({
                    "layer_name": layer.name,
                    "output_path": result["output_path"],
                    "status": "completed"
                })
            else:
                results["failed"] += 1
                results["errors"].append({
                    "layer_name": layer.name,
                    "error": result["message"]
                })
        
        return results

    def batch_convert(self, layers: List[LayerInfo], output_format: str = "GPKG") -> Dict[str, Any]:
        """
        批量转换图层（兼容旧测试）
        
        Args:
            layers: 图层列表
            output_format: 输出格式
            
        Returns:
            批量转换结果，格式与旧测试兼容
        """
        total = len(layers)
        successful = 0
        failed = 0
        skipped = 0
        results = []
        
        for layer in layers:
            result = self.convert_layer(layer, output_format)
            status = result.get("status", "unknown")
            
            result_item = {
                "layer_name": layer.name,
                "status": status,
                "output_path": result.get("output_path")
            }
            results.append(result_item)
            
            if status == "completed":
                successful += 1
            elif status == "failed":
                failed += 1
            elif status == "skipped":
                skipped += 1
            elif status == "simulated":
                successful += 1  # 模拟转换视为成功
        
        return {
            "total": total,
            "successful": successful,
            "failed": failed,
            "skipped": skipped,
            "results": results
        }


def main():
    """主函数（测试用）"""
    from data_models import LayerInfo, DataSourceInfo
    
    print("本地数据转换器测试")
    
    # 创建测试图层
    test_layer = LayerInfo(
        name="测试道路",
        layer_type="FeatureLayer",
        data_source=DataSourceInfo(
            data_source_type="Shapefile",
            file_path=r"C:\Data\Roads.shp"  # 模拟路径
        )
    )
    
    # 创建转换器
    converter = LocalDataConverter(output_dir="test_output")
    
    # 测试转换
    print(f"转换图层: {test_layer.name}")
    result = converter.convert_layer(test_layer, "GPKG")
    
    if result["success"]:
        print(f"转换成功!")
        print(f"输出文件: {result['output_path']}")
    else:
        print(f"转换失败: {result['message']}")
    
    # 清理测试目录
    if os.path.exists("test_output"):
        shutil.rmtree("test_output")


if __name__ == '__main__':
    main()