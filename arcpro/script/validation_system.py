"""
验证系统模块
提供多层次验证体系，确保ArcGIS Pro到QGIS迁移的准确性和完整性
"""

import logging
from typing import Dict, Any, List, Optional, Tuple
from dataclasses import dataclass, field
from enum import Enum
import math

from .data_models import MigrationProject, LayerInfo, FieldInfo, CoordinateSystem, Extent
from .utils.error_handler import StandardResult, create_success_result, create_error_result


class ValidationLevel(Enum):
    """验证级别枚举"""
    BASIC = "basic"          # 基本结构验证
    STANDARD = "standard"    # 标准验证（结构+属性）
    COMPREHENSIVE = "comprehensive"  # 全面验证（结构+属性+几何+符号+布局）


class ValidationResultType(Enum):
    """验证结果类型枚举"""
    PASS = "pass"
    WARNING = "warning"
    FAIL = "fail"
    SKIPPED = "skipped"


@dataclass
class ValidationResult:
    """单个验证结果"""
    validation_type: str
    result_type: ValidationResultType
    message: str
    details: Dict[str, Any] = field(default_factory=dict)
    score: float = 0.0  # 0.0 to 1.0
    tolerance: Optional[float] = None
    
    def to_dict(self) -> Dict[str, Any]:
        """转换为字典"""
        return {
            'validation_type': self.validation_type,
            'result_type': self.result_type.value,
            'message': self.message,
            'details': self.details,
            'score': self.score,
            'tolerance': self.tolerance
        }


@dataclass
class ValidationReport:
    """验证报告"""
    overall_score: float = 0.0
    results: List[ValidationResult] = field(default_factory=list)
    summary: Dict[str, int] = field(default_factory=dict)
    recommendations: List[str] = field(default_factory=list)
    
    def add_result(self, result: ValidationResult) -> None:
        """添加验证结果"""
        self.results.append(result)
    
    def calculate_overall_score(self) -> None:
        """计算总体评分"""
        if not self.results:
            self.overall_score = 0.0
            return
        
        # 计算加权平均分（不同类型的验证可能有不同权重）
        total_weight = 0
        weighted_sum = 0
        
        for result in self.results:
            if result.result_type == ValidationResultType.SKIPPED:
                continue
                
            # 根据验证类型分配权重
            weight = self._get_weight_for_type(result.validation_type)
            weighted_sum += result.score * weight
            total_weight += weight
        
        self.overall_score = weighted_sum / total_weight if total_weight > 0 else 0.0
    
    def generate_summary(self) -> Dict[str, int]:
        """生成结果摘要"""
        summary = {
            'total': len(self.results),
            'pass': 0,
            'warning': 0,
            'fail': 0,
            'skipped': 0
        }
        
        for result in self.results:
            summary[result.result_type.value] += 1
        
        self.summary = summary
        return summary
    
    def _get_weight_for_type(self, validation_type: str) -> float:
        """获取验证类型权重"""
        # 从环境变量或配置文件中读取权重配置
        weights_config = os.environ.get('VALIDATION_WEIGHTS', '')
        if weights_config:
            try:
                # 格式: structure=0.3,attribute=0.2,geometry=0.2,symbology=0.15,layout=0.15
                weights = {}
                for item in weights_config.split(','):
                    if '=' in item:
                        key, value = item.split('=', 1)
                        weights[key.strip()] = float(value.strip())
            except Exception:
                # 解析失败时使用默认值
                weights = {}
        else:
            weights = {}
        
        # 默认权重
        default_weights = {
            'structure': 0.3,
            'attribute': 0.2,
            'geometry': 0.2,
            'symbology': 0.15,
            'layout': 0.15
        }
        
        # 使用配置的权重，如果没有则使用默认值
        return weights.get(validation_type, default_weights.get(validation_type, 0.1))


@dataclass
class ValidationToleranceConfig:
    """验证容差配置"""
    geometry_tolerance: float = 0.001      # 几何容差（地图单位）
    attribute_tolerance: float = 0.0       # 属性容差（必须完全一致）
    visual_tolerance: float = 0.05         # 视觉容差（5%差异）
    layout_tolerance: float = 1.0          # 布局容差（1像素）
    time_tolerance: float = 2.0            # 时间容差（2秒）
    data_volume_tolerance: float = 0.01    # 数据量容差（1%）


class BaseValidator:
    """验证器基类"""
    
    def __init__(self, tolerance_config: ValidationToleranceConfig):
        self.tolerance = tolerance_config
        self.logger = logging.getLogger(__name__)
    
    def validate(self, source_data: Any, target_data: Any) -> ValidationResult:
        """执行验证（子类必须实现）"""
        raise NotImplementedError


class StructureValidator(BaseValidator):
    """结构验证器"""
    
    def validate(self, source_project: MigrationProject, target_project: Any) -> ValidationResult:
        """验证工程结构一致性"""
        self.logger.info("开始结构验证")
        
        # 这里简化实现，实际需要比较两个工程的结构
        # 假设target_project是类似MigrationProject的结构
        
        issues = []
        score = 1.0
        
        # 1. 验证地图数量
        source_map_count = len(source_project.maps)
        target_map_count = len(target_project.maps) if hasattr(target_project, 'maps') else 0
        
        if source_map_count != target_map_count:
            issues.append(f"地图数量不一致: 源={source_map_count}, 目标={target_map_count}")
            score -= 0.2
        
        # 2. 验证图层数量
        source_layers = source_project.get_all_layers()
        target_layers = target_project.get_all_layers() if hasattr(target_project, 'get_all_layers') else []
        
        source_layer_count = len(source_layers)
        target_layer_count = len(target_layers)
        
        if source_layer_count != target_layer_count:
            issues.append(f"图层数量不一致: 源={source_layer_count}, 目标={target_layer_count}")
            score -= 0.2
        
        # 3. 验证图层名称匹配
        source_layer_names = {layer.name for layer in source_layers}
        target_layer_names = {layer.name for layer in target_layers}
        
        missing_in_target = source_layer_names - target_layer_names
        extra_in_target = target_layer_names - source_layer_names
        
        if missing_in_target:
            issues.append(f"目标中缺少图层: {', '.join(missing_in_target)}")
            score -= 0.1 * len(missing_in_target) / max(len(source_layer_names), 1)
        
        if extra_in_target:
            issues.append(f"目标中多出图层: {', '.join(extra_in_target)}")
            score -= 0.05 * len(extra_in_target) / max(len(target_layer_names), 1)
        
        # 确定结果类型
        if score >= 0.9:
            result_type = ValidationResultType.PASS
        elif score >= 0.7:
            result_type = ValidationResultType.WARNING
        else:
            result_type = ValidationResultType.FAIL
        
        return ValidationResult(
            validation_type='structure',
            result_type=result_type,
            message=f"结构验证完成，评分: {score:.2f}",
            details={
                'issues': issues,
                'source_map_count': source_map_count,
                'target_map_count': target_map_count,
                'source_layer_count': source_layer_count,
                'target_layer_count': target_layer_count,
                'missing_layers': list(missing_in_target),
                'extra_layers': list(extra_in_target)
            },
            score=score
        )


class AttributeValidator(BaseValidator):
    """属性验证器"""
    
    def validate(self, source_samples: List[Dict[str, Any]], target_samples: List[Dict[str, Any]]) -> ValidationResult:
        """验证属性数据一致性"""
        self.logger.info("开始属性验证")
        
        if not source_samples and not target_samples:
            return ValidationResult(
                validation_type='attribute',
                result_type=ValidationResultType.SKIPPED,
                message="属性验证跳过（无采样数据）",
                score=1.0
            )
        
        issues = []
        score = 1.0
        
        # 比较采样数据
        source_sample_count = len(source_samples)
        target_sample_count = len(target_samples)
        
        if source_sample_count != target_sample_count:
            issues.append(f"采样数量不一致: 源={source_sample_count}, 目标={target_sample_count}")
            score -= 0.3
        
        # 比较字段定义
        if source_samples and target_samples:
            # 获取字段名列表（假设第一个样本包含所有字段）
            source_fields = set(source_samples[0].keys()) if source_samples else set()
            target_fields = set(target_samples[0].keys()) if target_samples else set()
            
            missing_fields = source_fields - target_fields
            extra_fields = target_fields - source_fields
            
            if missing_fields:
                issues.append(f"目标中缺少字段: {', '.join(missing_fields)}")
                score -= 0.2
            
            if extra_fields:
                issues.append(f"目标中多出字段: {', '.join(extra_fields)}")
                score -= 0.1
        
        # 比较属性值（简化实现，仅比较前几个样本）
        matched_samples = min(source_sample_count, target_sample_count)
        if matched_samples > 0:
            mismatch_count = 0
            for i in range(matched_samples):
                source_attrs = source_samples[i]
                target_attrs = target_samples[i]
                
                for key in source_attrs.keys():
                    if key in target_attrs:
                        if source_attrs[key] != target_attrs[key]:
                            mismatch_count += 1
                            if mismatch_count <= 3:  # 只记录前3个不匹配
                                issues.append(f"样本{i}字段'{key}'值不匹配: 源='{source_attrs[key]}', 目标='{target_attrs[key]}'")
            
            if mismatch_count > 0:
                mismatch_rate = mismatch_count / (matched_samples * len(source_fields)) if source_fields else 0
                score -= mismatch_rate * 0.5
                issues.append(f"属性值不匹配率: {mismatch_rate:.2%}")
        
        # 确定结果类型
        if score >= 0.95:
            result_type = ValidationResultType.PASS
        elif score >= 0.8:
            result_type = ValidationResultType.WARNING
        else:
            result_type = ValidationResultType.FAIL
        
        return ValidationResult(
            validation_type='attribute',
            result_type=result_type,
            message=f"属性验证完成，评分: {score:.2f}",
            details={
                'issues': issues,
                'source_sample_count': source_sample_count,
                'target_sample_count': target_sample_count,
                'missing_fields': list(missing_fields) if 'missing_fields' in locals() else [],
                'extra_fields': list(extra_fields) if 'extra_fields' in locals() else []
            },
            score=score
        )


class GeometryValidator(BaseValidator):
    """几何验证器"""
    
    def validate(self, source_geometries: List[str], target_geometries: List[str]) -> ValidationResult:
        """验证几何数据一致性"""
        self.logger.info("开始几何验证")
        
        if not source_geometries and not target_geometries:
            return ValidationResult(
                validation_type='geometry',
                result_type=ValidationResultType.SKIPPED,
                message="几何验证跳过（无几何数据）",
                score=1.0
            )
        
        issues = []
        score = 1.0
        
        source_count = len(source_geometries)
        target_count = len(target_geometries)
        
        if source_count != target_count:
            issues.append(f"几何数量不一致: 源={source_count}, 目标={target_count}")
            score -= 0.3
        
        # 简化几何比较（实际应使用GIS库进行精确比较）
        matched_count = min(source_count, target_count)
        if matched_count > 0:
            mismatch_count = 0
            
            for i in range(matched_count):
                source_geom = source_geometries[i]
                target_geom = target_geometries[i]
                
                # 简单比较WKT字符串（实际应进行空间比较）
                if source_geom != target_geom:
                    mismatch_count += 1
                    if mismatch_count <= 3:
                        issues.append(f"几何{i}不匹配")
            
            if mismatch_count > 0:
                mismatch_rate = mismatch_count / matched_count
                score -= mismatch_rate * 0.7
                issues.append(f"几何不匹配率: {mismatch_rate:.2%}")
        
        # 确定结果类型
        if score >= 0.95:
            result_type = ValidationResultType.PASS
        elif score >= 0.8:
            result_type = ValidationResultType.WARNING
        else:
            result_type = ValidationResultType.FAIL
        
        return ValidationResult(
            validation_type='geometry',
            result_type=result_type,
            message=f"几何验证完成，评分: {score:.2f}",
            details={
                'issues': issues,
                'source_count': source_count,
                'target_count': target_count,
                'mismatch_count': mismatch_count if 'mismatch_count' in locals() else 0,
                'tolerance': self.tolerance.geometry_tolerance
            },
            score=score
        )


class SymbologyValidator(BaseValidator):
    """符号验证器"""
    
    def validate(self, source_symbols: List[Dict[str, Any]], target_symbols: List[Dict[str, Any]]) -> ValidationResult:
        """验证符号系统一致性"""
        self.logger.info("开始符号验证")
        
        if not source_symbols and not target_symbols:
            return ValidationResult(
                validation_type='symbology',
                result_type=ValidationResultType.SKIPPED,
                message="符号验证跳过（无符号数据）",
                score=1.0
            )
        
        issues = []
        score = 1.0
        
        source_count = len(source_symbols)
        target_count = len(target_symbols)
        
        if source_count != target_count:
            issues.append(f"符号数量不一致: 源={source_count}, 目标={target_count}")
            score -= 0.2
        
        # 简化符号比较
        if source_count > 0 and target_count > 0:
            # 比较符号类型分布
            source_types = {}
            for symbol in source_symbols:
                sym_type = symbol.get('type', 'Unknown')
                source_types[sym_type] = source_types.get(sym_type, 0) + 1
            
            target_types = {}
            for symbol in target_symbols:
                sym_type = symbol.get('type', 'Unknown')
                target_types[sym_type] = target_types.get(sym_type, 0) + 1
            
            # 检查类型匹配
            for sym_type, count in source_types.items():
                target_count = target_types.get(sym_type, 0)
                if target_count != count:
                    issues.append(f"符号类型'{sym_type}'数量不一致: 源={count}, 目标={target_count}")
                    score -= 0.1 * abs(count - target_count) / max(count, 1)
            
            # 检查颜色匹配（简化）
            source_colors = [s.get('color') for s in source_symbols if s.get('color')]
            target_colors = [s.get('color') for s in target_symbols if s.get('color')]
            
            if len(source_colors) != len(target_colors):
                issues.append(f"颜色定义数量不一致: 源={len(source_colors)}, 目标={len(target_colors)}")
                score -= 0.1
        
        # 确定结果类型
        if score >= 0.9:
            result_type = ValidationResultType.PASS
        elif score >= 0.7:
            result_type = ValidationResultType.WARNING
        else:
            result_type = ValidationResultType.FAIL
        
        return ValidationResult(
            validation_type='symbology',
            result_type=result_type,
            message=f"符号验证完成，评分: {score:.2f}",
            details={
                'issues': issues,
                'source_count': source_count,
                'target_count': target_count,
                'tolerance': self.tolerance.visual_tolerance
            },
            score=score
        )


class LayoutValidator(BaseValidator):
    """布局验证器"""
    
    def validate(self, source_layouts: List[Dict[str, Any]], target_layouts: List[Dict[str, Any]]) -> ValidationResult:
        """验证布局一致性"""
        self.logger.info("开始布局验证")
        
        if not source_layouts and not target_layouts:
            return ValidationResult(
                validation_type='layout',
                result_type=ValidationResultType.SKIPPED,
                message="布局验证跳过（无布局数据）",
                score=1.0
            )
        
        issues = []
        score = 1.0
        
        source_count = len(source_layouts)
        target_count = len(target_layouts)
        
        if source_count != target_count:
            issues.append(f"布局数量不一致: 源={source_count}, 目标={target_count}")
            score -= 0.3
        
        # 简化布局元素比较
        if source_count > 0 and target_count > 0:
            # 比较布局元素类型
            source_elements = []
            for layout in source_layouts:
                elements = layout.get('elements', [])
                source_elements.extend(elements)
            
            target_elements = []
            for layout in target_layouts:
                elements = layout.get('elements', [])
                target_elements.extend(elements)
            
            if len(source_elements) != len(target_elements):
                issues.append(f"布局元素数量不一致: 源={len(source_elements)}, 目标={len(target_elements)}")
                score -= 0.2
            
            # 检查关键元素类型
            source_element_types = [e.get('type') for e in source_elements if e.get('type')]
            target_element_types = [e.get('type') for e in target_elements if e.get('type')]
            
            missing_types = set(source_element_types) - set(target_element_types)
            if missing_types:
                issues.append(f"目标中缺少布局元素类型: {', '.join(missing_types)}")
                score -= 0.1 * len(missing_types)
        
        # 确定结果类型
        if score >= 0.9:
            result_type = ValidationResultType.PASS
        elif score >= 0.7:
            result_type = ValidationResultType.WARNING
        else:
            result_type = ValidationResultType.FAIL
        
        return ValidationResult(
            validation_type='layout',
            result_type=result_type,
            message=f"布局验证完成，评分: {score:.2f}",
            details={
                'issues': issues,
                'source_count': source_count,
                'target_count': target_count,
                'tolerance': self.tolerance.layout_tolerance
            },
            score=score
        )


class DataValidator:
    """数据验证器（主入口）"""
    
    def __init__(self, tolerance_config: Optional[ValidationToleranceConfig] = None):
        self.tolerance = tolerance_config or ValidationToleranceConfig()
        self.logger = logging.getLogger(__name__)
        
        self.validators = {
            'structure': StructureValidator(self.tolerance),
            'attribute': AttributeValidator(self.tolerance),
            'geometry': GeometryValidator(self.tolerance),
            'symbology': SymbologyValidator(self.tolerance),
            'layout': LayoutValidator(self.tolerance)
        }
    
    def validate_migration(
        self,
        source_info: Dict[str, Any],
        target_info: Dict[str, Any],
        validation_level: ValidationLevel = ValidationLevel.COMPREHENSIVE
    ) -> ValidationReport:
        """验证迁移结果"""
        report = ValidationReport()
        
        self.logger.info(f"开始迁移验证，级别: {validation_level.value}")
        
        # 结构验证（总是执行）
        if 'source_project' in source_info and 'target_project' in target_info:
            structure_result = self.validators['structure'].validate(
                source_info['source_project'],
                target_info['target_project']
            )
            report.add_result(structure_result)
        
        # 属性验证
        if validation_level in [ValidationLevel.STANDARD, ValidationLevel.COMPREHENSIVE]:
            if 'attribute_samples' in source_info and 'attribute_samples' in target_info:
                attribute_result = self.validators['attribute'].validate(
                    source_info['attribute_samples'],
                    target_info['attribute_samples']
                )
                report.add_result(attribute_result)
        
        # 几何验证
        if validation_level in [ValidationLevel.STANDARD, ValidationLevel.COMPREHENSIVE]:
            if 'geometry_samples' in source_info and 'geometry_samples' in target_info:
                geometry_result = self.validators['geometry'].validate(
                    source_info['geometry_samples'],
                    target_info['geometry_samples']
                )
                report.add_result(geometry_result)
        
        # 符号验证
        if validation_level == ValidationLevel.COMPREHENSIVE:
            if 'symbols' in source_info and 'symbols' in target_info:
                symbology_result = self.validators['symbology'].validate(
                    source_info['symbols'],
                    target_info['symbols']
                )
                report.add_result(symbology_result)
        
        # 布局验证
        if validation_level == ValidationLevel.COMPREHENSIVE:
            if 'layouts' in source_info and 'layouts' in target_info:
                layout_result = self.validators['layout'].validate(
                    source_info['layouts'],
                    target_info['layouts']
                )
                report.add_result(layout_result)
        
        # 生成报告
        report.calculate_overall_score()
        report.generate_summary()
        
        # 生成建议
        self._generate_recommendations(report)
        
        self.logger.info(f"迁移验证完成，总体评分: {report.overall_score:.2f}")
        
        return report
    
    def _generate_recommendations(self, report: ValidationReport) -> None:
        """生成优化建议"""
        recommendations = []
        
        for result in report.results:
            if result.result_type == ValidationResultType.FAIL:
                if result.validation_type == 'structure':
                    recommendations.append("工程结构不一致，请检查地图和图层配置")
                elif result.validation_type == 'attribute':
                    recommendations.append("属性数据不一致，请检查字段定义和数据转换")
                elif result.validation_type == 'geometry':
                    recommendations.append("几何数据不一致，请检查坐标系和几何转换")
                elif result.validation_type == 'symbology':
                    recommendations.append("符号系统不一致，请检查样式转换映射")
                elif result.validation_type == 'layout':
                    recommendations.append("布局不一致，请检查布局元素转换")
            elif result.result_type == ValidationResultType.WARNING:
                if result.validation_type == 'structure':
                    recommendations.append("工程结构有轻微差异，建议手动检查")
        
        # 根据总体评分添加建议
        if report.overall_score < 0.7:
            recommendations.append("迁移质量较低，建议进行全面检查和调整")
        elif report.overall_score < 0.9:
            recommendations.append("迁移质量良好，建议对警告项进行优化")
        else:
            recommendations.append("迁移质量优秀，可以投入生产使用")
        
        report.recommendations = recommendations


# 工具函数
def create_validation_report(source_data: Dict[str, Any], target_data: Dict[str, Any]) -> StandardResult:
    """创建验证报告（兼容现有接口）"""
    try:
        validator = DataValidator()
        report = validator.validate_migration(
            source_data,
            target_data,
            ValidationLevel.COMPREHENSIVE
        )
        
        return create_success_result(
            message="验证完成",
            data={
                'report': report,
                'overall_score': report.overall_score,
                'summary': report.summary,
                'results': [r.to_dict() for r in report.results],
                'recommendations': report.recommendations
            }
        )
    except Exception as e:
        return create_error_result(
            message=f"验证失败: {str(e)}",
            errors=[str(e)]
        )


# 示例使用
if __name__ == "__main__":
    # 设置日志
    logging.basicConfig(level=logging.INFO)
    
    print("验证系统示例")
    print("=" * 60)
    
    # 创建模拟数据
    source_info = {
        'source_project': None,  # 实际应为MigrationProject实例
        'attribute_samples': [
            {'id': 1, 'name': 'Feature 1', 'value': 100},
            {'id': 2, 'name': 'Feature 2', 'value': 200}
        ],
        'geometry_samples': ['POINT(10 20)', 'POINT(30 40)'],
        'symbols': [
            {'type': 'SimpleMarkerSymbol', 'color': '#FF0000', 'size': 5},
            {'type': 'SimpleLineSymbol', 'color': '#00FF00', 'width': 2}
        ],
        'layouts': [
            {'name': 'Main Layout', 'elements': ['title', 'map', 'legend']}
        ]
    }
    
    target_info = {
        'target_project': None,
        'attribute_samples': [
            {'id': 1, 'name': 'Feature 1', 'value': 100},
            {'id': 2, 'name': 'Feature 2', 'value': 200}
        ],
        'geometry_samples': ['POINT(10 20)', 'POINT(30 40)'],
        'symbols': [
            {'type': 'SimpleMarkerSymbol', 'color': '#FF0000', 'size': 5},
            {'type': 'SimpleLineSymbol', 'color': '#00FF00', 'width': 2}
        ],
        'layouts': [
            {'name': 'Main Layout', 'elements': ['title', 'map', 'legend']}
        ]
    }
    
    # 执行验证
    validator = DataValidator()
    report = validator.validate_migration(
        source_info,
        target_info,
        ValidationLevel.COMPREHENSIVE
    )
    
    print(f"总体评分: {report.overall_score:.2f}")
    print(f"验证结果摘要: {report.summary}")
    print("\n详细结果:")
    for result in report.results:
        print(f"  - {result.validation_type}: {result.result_type.value} (评分: {result.score:.2f})")
    
    print("\n建议:")
    for rec in report.recommendations:
        print(f"  - {rec}")
    
    print("\n验证系统实现完成！")