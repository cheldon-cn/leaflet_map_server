"""
版本兼容性检查模块
检查ArcGIS Pro和QGIS版本兼容性，确保迁移功能正常工作
"""

import os
import sys
import logging
import re
from typing import Dict, Any, List, Optional, Tuple
from dataclasses import dataclass, field
from enum import Enum


class CompatibilityLevel(Enum):
    """兼容性级别"""
    FULLY_COMPATIBLE = "fully_compatible"  # 完全兼容
    PARTIALLY_COMPATIBLE = "partially_compatible"  # 部分兼容
    LIMITED_COMPATIBLE = "limited_compatible"  # 有限兼容
    NOT_COMPATIBLE = "not_compatible"  # 不兼容


class CompatibilityIssueSeverity(Enum):
    """兼容性问题严重性"""
    INFO = "info"  # 信息
    WARNING = "warning"  # 警告
    ERROR = "error"  # 错误
    CRITICAL = "critical"  # 严重


@dataclass
class CompatibilityIssue:
    """兼容性问题"""
    component: str  # 组件名称
    severity: CompatibilityIssueSeverity
    description: str
    details: Dict[str, Any] = field(default_factory=dict)
    recommendation: Optional[str] = None
    affected_features: List[str] = field(default_factory=list)


@dataclass
class CompatibilityReport:
    """兼容性报告"""
    source_arcgis_version: str
    target_qgis_version: str
    compatibility_level: CompatibilityLevel
    issues: List[CompatibilityIssue] = field(default_factory=list)
    summary: Dict[str, Any] = field(default_factory=dict)
    recommendations: List[str] = field(default_factory=list)
    
    def add_issue(self, issue: CompatibilityIssue) -> None:
        """添加兼容性问题"""
        self.issues.append(issue)
    
    def generate_summary(self) -> Dict[str, Any]:
        """生成报告摘要"""
        severity_counts = {
            "info": 0,
            "warning": 0,
            "error": 0,
            "critical": 0
        }
        
        component_counts = {}
        
        for issue in self.issues:
            severity_counts[issue.severity.value] += 1
            
            if issue.component not in component_counts:
                component_counts[issue.component] = 0
            component_counts[issue.component] += 1
        
        self.summary = {
            "total_issues": len(self.issues),
            "severity_counts": severity_counts,
            "component_counts": component_counts,
            "compatibility_level": self.compatibility_level.value
        }
        
        return self.summary
    
    def to_dict(self) -> Dict[str, Any]:
        """转换为字典"""
        return {
            "source_arcgis_version": self.source_arcgis_version,
            "target_qgis_version": self.target_qgis_version,
            "compatibility_level": self.compatibility_level.value,
            "issues": [
                {
                    "component": issue.component,
                    "severity": issue.severity.value,
                    "description": issue.description,
                    "details": issue.details,
                    "recommendation": issue.recommendation,
                    "affected_features": issue.affected_features
                }
                for issue in self.issues
            ],
            "summary": self.summary,
            "recommendations": self.recommendations
        }


class VersionCompatibilityChecker:
    """版本兼容性检查器"""
    
    # ArcGIS Pro版本兼容性矩阵
    ARCGIS_COMPATIBILITY_MATRIX = {
        # ArcGIS Pro版本: (最小兼容QGIS版本, 最大兼容QGIS版本, 兼容性备注)
        "2.0": ("3.4", "3.22", "基本功能支持，部分高级符号化可能不支持"),
        "2.1": ("3.4", "3.22", "基本功能支持"),
        "2.2": ("3.6", "3.24", "改进的符号化支持"),
        "2.3": ("3.8", "3.24", "更好的布局转换支持"),
        "2.4": ("3.10", "3.26", "增强的数据源支持"),
        "2.5": ("3.12", "3.28", "改进的3D功能支持"),
        "2.6": ("3.14", "3.30", "更好的网络分析支持"),
        "2.7": ("3.16", "3.30", "增强的时态数据支持"),
        "2.8": ("3.18", "3.32", "改进的栅格处理支持"),
        "2.9": ("3.20", "3.34", "更好的地图系列支持"),
        "3.0": ("3.22", "3.36", "增强的编辑功能支持"),
        "3.1": ("3.24", "3.36", "改进的符号系统转换"),
        "3.2": ("3.26", "3.38", "更好的布局元素支持"),
        "3.3": ("3.28", "3.38", "增强的3D场景支持"),
    }
    
    # 已知的不兼容功能
    INCOMPATIBLE_FEATURES = {
        "arcgis": [
            "Arcade表达式",  # QGIS不支持Arcade
            "ArcGIS Online特定符号",  # 需要转换
            "ArcGIS Pro独占工具",  # 需要QGIS等效工具
            "Enterprise Geodatabase高级功能",  # 有限支持
            "特定坐标系",  # 可能需要在QGIS中定义
        ],
        "qgis": [
            "QGIS表达式语言",  # 与Arcade不兼容
            "QGIS特定插件",  # 无ArcGIS对应
            "某些数据提供者",  # 可能无对应
        ]
    }
    
    def __init__(self):
        self.logger = logging.getLogger(__name__)
    
    def parse_version(self, version_str: str) -> Tuple[int, int, Optional[int]]:
        """
        解析版本字符串
        
        Args:
            version_str: 版本字符串，如"3.2.1"或"2.9"
            
        Returns:
            (主版本, 次版本, 修订版本)
        """
        if not version_str:
            return (0, 0, None)
        
        # 提取数字部分
        match = re.search(r'(\d+)\.(\d+)(?:\.(\d+))?', version_str)
        if not match:
            # 尝试其他格式
            match = re.search(r'(\d+)[^\d]*(\d+)', version_str)
            if not match:
                return (0, 0, None)
        
        major = int(match.group(1))
        minor = int(match.group(2))
        patch = int(match.group(3)) if match.group(3) else None
        
        return (major, minor, patch)
    
    def compare_versions(self, version1: str, version2: str) -> int:
        """
        比较两个版本
        
        Args:
            version1: 版本1
            version2: 版本2
            
        Returns:
            -1: version1 < version2
            0: version1 == version2
            1: version1 > version2
        """
        v1 = self.parse_version(version1)
        v2 = self.parse_version(version2)
        
        # 比较主版本
        if v1[0] < v2[0]:
            return -1
        elif v1[0] > v2[0]:
            return 1
        
        # 比较次版本
        if v1[1] < v2[1]:
            return -1
        elif v1[1] > v2[1]:
            return 1
        
        # 比较修订版本（如果都存在）
        if v1[2] is not None and v2[2] is not None:
            if v1[2] < v2[2]:
                return -1
            elif v1[2] > v2[2]:
                return 1
        
        return 0
    
    def check_version_compatibility(self, arcgis_version: str, qgis_version: str) -> CompatibilityReport:
        """
        检查ArcGIS Pro和QGIS版本兼容性
        
        Args:
            arcgis_version: ArcGIS Pro版本
            qgis_version: QGIS版本
            
        Returns:
            兼容性报告
        """
        report = CompatibilityReport(
            source_arcgis_version=arcgis_version,
            target_qgis_version=qgis_version,
            compatibility_level=CompatibilityLevel.FULLY_COMPATIBLE
        )
        
        # 检查ArcGIS Pro版本是否在兼容矩阵中
        arcgis_major_minor = self._get_major_minor_version(arcgis_version)
        qgis_major_minor = self._get_major_minor_version(qgis_version)
        
        if arcgis_major_minor not in self.ARCGIS_COMPATIBILITY_MATRIX:
            # 未知版本，添加警告
            report.add_issue(CompatibilityIssue(
                component="版本检测",
                severity=CompatibilityIssueSeverity.WARNING,
                description=f"未知的ArcGIS Pro版本: {arcgis_version}",
                details={"arcgis_version": arcgis_version},
                recommendation="请检查ArcGIS Pro版本是否受支持，或联系技术支持"
            ))
            
            # 尝试找到最接近的版本
            closest_version = self._find_closest_version(arcgis_version, list(self.ARCGIS_COMPATIBILITY_MATRIX.keys()))
            if closest_version:
                arcgis_major_minor = closest_version
                report.add_issue(CompatibilityIssue(
                    component="版本检测",
                    severity=CompatibilityIssueSeverity.INFO,
                    description=f"使用最接近的兼容版本进行验证: {closest_version}",
                    details={"original_version": arcgis_version, "used_version": closest_version}
                ))
        
        # 检查QGIS版本是否兼容
        if arcgis_major_minor in self.ARCGIS_COMPATIBILITY_MATRIX:
            min_qgis, max_qgis, notes = self.ARCGIS_COMPATIBILITY_MATRIX[arcgis_major_minor]
            
            if self.compare_versions(qgis_version, min_qgis) < 0:
                # QGIS版本太低
                report.add_issue(CompatibilityIssue(
                    component="版本兼容性",
                    severity=CompatibilityIssueSeverity.ERROR,
                    description=f"QGIS版本 {qgis_version} 低于最低要求 {min_qgis}",
                    details={
                        "arcgis_version": arcgis_version,
                        "qgis_version": qgis_version,
                        "min_required_qgis": min_qgis
                    },
                    recommendation=f"请升级QGIS到 {min_qgis} 或更高版本",
                    affected_features=["所有功能"]
                ))
                report.compatibility_level = CompatibilityLevel.NOT_COMPATIBLE
            
            elif self.compare_versions(qgis_version, max_qgis) > 0:
                # QGIS版本太高
                report.add_issue(CompatibilityIssue(
                    component="版本兼容性",
                    severity=CompatibilityIssueSeverity.WARNING,
                    description=f"QGIS版本 {qgis_version} 高于测试的最高版本 {max_qgis}",
                    details={
                        "arcgis_version": arcgis_version,
                        "qgis_version": qgis_version,
                        "max_tested_qgis": max_qgis
                    },
                    recommendation=f"某些功能可能未经测试，建议使用 {max_qgis} 或更低版本",
                    affected_features=["高级功能"]
                ))
                report.compatibility_level = CompatibilityLevel.PARTIALLY_COMPATIBLE
            
            else:
                # 版本在兼容范围内
                report.add_issue(CompatibilityIssue(
                    component="版本兼容性",
                    severity=CompatibilityIssueSeverity.INFO,
                    description=f"版本兼容性检查通过: ArcGIS Pro {arcgis_version} → QGIS {qgis_version}",
                    details={
                        "compatibility_range": f"QGIS {min_qgis} - {max_qgis}",
                        "notes": notes
                    }
                ))
        
        # 检查已知的不兼容功能
        self._check_incompatible_features(report)
        
        # 检查特定功能兼容性
        self._check_feature_compatibility(report)
        
        # 生成报告摘要
        report.generate_summary()
        
        # 根据问题严重性调整兼容性级别
        if not report.issues:
            report.compatibility_level = CompatibilityLevel.FULLY_COMPATIBLE
        else:
            critical_errors = any(
                issue.severity == CompatibilityIssueSeverity.CRITICAL 
                for issue in report.issues
            )
            errors = any(
                issue.severity == CompatibilityIssueSeverity.ERROR 
                for issue in report.issues
            )
            
            if critical_errors:
                report.compatibility_level = CompatibilityLevel.NOT_COMPATIBLE
            elif errors:
                report.compatibility_level = CompatibilityLevel.LIMITED_COMPATIBLE
            elif any(issue.severity == CompatibilityIssueSeverity.WARNING for issue in report.issues):
                report.compatibility_level = CompatibilityLevel.PARTIALLY_COMPATIBLE
        
        # 生成建议
        report.recommendations = self._generate_recommendations(report)
        
        return report
    
    def _get_major_minor_version(self, version_str: str) -> str:
        """获取主版本.次版本格式"""
        match = re.search(r'(\d+\.\d+)', version_str)
        if match:
            return match.group(1)
        return version_str
    
    def _find_closest_version(self, target: str, versions: List[str]) -> Optional[str]:
        """找到最接近的版本"""
        if not versions:
            return None
        
        target_major_minor = self._get_major_minor_version(target)
        for version in sorted(versions, key=lambda v: self.parse_version(v)):
            if self.compare_versions(version, target_major_minor) >= 0:
                return version
        
        return versions[-1]
    
    def _check_incompatible_features(self, report: CompatibilityReport) -> None:
        """检查已知的不兼容功能"""
        # ArcGIS特定功能
        for feature in self.INCOMPATIBLE_FEATURES["arcgis"]:
            report.add_issue(CompatibilityIssue(
                component="功能兼容性",
                severity=CompatibilityIssueSeverity.WARNING,
                description=f"ArcGIS功能 '{feature}' 在QGIS中可能不完全支持",
                details={"feature": feature, "source": "arcgis"},
                recommendation="可能需要手动调整或使用替代方案",
                affected_features=[feature]
            ))
    
    def _check_feature_compatibility(self, report: CompatibilityReport) -> None:
        """检查特定功能兼容性"""
        arcgis_version = report.source_arcgis_version
        
        # 根据ArcGIS版本检查特定功能
        if self.compare_versions(arcgis_version, "3.0") >= 0:
            # ArcGIS Pro 3.0+ 引入的新功能
            report.add_issue(CompatibilityIssue(
                component="功能兼容性",
                severity=CompatibilityIssueSeverity.INFO,
                description="ArcGIS Pro 3.0+ 的增强编辑功能在QGIS中可能有限支持",
                details={"arcgis_version": arcgis_version},
                affected_features=["增强编辑", "拓扑编辑"]
            ))
        
        if self.compare_versions(arcgis_version, "2.9") >= 0:
            # 地图系列功能
            report.add_issue(CompatibilityIssue(
                component="功能兼容性",
                severity=CompatibilityIssueSeverity.WARNING,
                description="ArcGIS Pro地图系列功能在QGIS中需要手动设置图集",
                details={"arcgis_version": arcgis_version},
                recommendation="使用QGIS的图集功能手动配置",
                affected_features=["地图系列", "批量出图"]
            ))
    
    def _generate_recommendations(self, report: CompatibilityReport) -> List[str]:
        """生成建议列表"""
        recommendations = []
        
        # 基于兼容性级别的建议
        if report.compatibility_level == CompatibilityLevel.NOT_COMPATIBLE:
            recommendations.append("当前版本组合不兼容，请升级或降级软件版本")
            recommendations.append("考虑使用中间格式进行数据迁移")
        
        elif report.compatibility_level == CompatibilityLevel.LIMITED_COMPATIBLE:
            recommendations.append("有限兼容，建议测试核心功能后再进行完整迁移")
            recommendations.append("准备手动调整不兼容的功能")
        
        elif report.compatibility_level == CompatibilityLevel.PARTIALLY_COMPATIBLE:
            recommendations.append("部分兼容，大多数功能可用，但某些高级功能可能需要调整")
            recommendations.append("建议创建迁移测试计划")
        
        elif report.compatibility_level == CompatibilityLevel.FULLY_COMPATIBLE:
            recommendations.append("完全兼容，可以安全进行迁移")
            recommendations.append("建议仍然进行小规模测试以确保所有功能正常工作")
        
        # 基于具体问题的建议
        for issue in report.issues:
            if issue.recommendation and issue.recommendation not in recommendations:
                recommendations.append(issue.recommendation)
        
        # 通用建议
        recommendations.append("在迁移前备份所有原始数据")
        recommendations.append("使用测试环境进行迁移验证")
        recommendations.append("记录所有迁移过程中遇到的问题和解决方案")
        
        return recommendations
    
    def check_project_compatibility(self, migration_project: Any) -> CompatibilityReport:
        """
        检查迁移工程的兼容性
        
        Args:
            migration_project: 迁移工程对象
            
        Returns:
            兼容性报告
        """
        # 这里需要根据实际的migration_project结构进行调整
        # 假设migration_project有source_arcgis_version和target_qgis_version属性
        try:
            arcgis_version = getattr(migration_project, 'source_arcgis_version', '')
            qgis_version = getattr(migration_project, 'target_qgis_version', '')
            
            if not arcgis_version:
                arcgis_version = "3.0"  # 默认值
            
            if not qgis_version:
                qgis_version = "3.34"  # 默认值
            
            return self.check_version_compatibility(arcgis_version, qgis_version)
            
        except Exception as e:
            self.logger.error(f"检查工程兼容性失败: {e}")
            # 返回基本报告
            return CompatibilityReport(
                source_arcgis_version="unknown",
                target_qgis_version="unknown",
                compatibility_level=CompatibilityLevel.PARTIALLY_COMPATIBLE,
                issues=[
                    CompatibilityIssue(
                        component="兼容性检查",
                        severity=CompatibilityIssueSeverity.ERROR,
                        description=f"兼容性检查失败: {e}",
                        details={"error": str(e)}
                    )
                ]
            )


# 实用函数
def check_compatibility(arcgis_version: str, qgis_version: str) -> Dict[str, Any]:
    """
    快速检查兼容性（简化接口）
    
    Args:
        arcgis_version: ArcGIS Pro版本
        qgis_version: QGIS版本
        
    Returns:
        兼容性信息字典
    """
    checker = VersionCompatibilityChecker()
    report = checker.check_version_compatibility(arcgis_version, qgis_version)
    return report.to_dict()


def get_supported_arcgis_versions() -> List[str]:
    """获取支持的ArcGIS Pro版本列表"""
    checker = VersionCompatibilityChecker()
    return list(checker.ARCGIS_COMPATIBILITY_MATRIX.keys())


def get_recommended_qgis_version(arcgis_version: str) -> Optional[str]:
    """获取推荐的QGIS版本"""
    checker = VersionCompatibilityChecker()
    arcgis_major_minor = checker._get_major_minor_version(arcgis_version)
    
    if arcgis_major_minor in checker.ARCGIS_COMPATIBILITY_MATRIX:
        min_qgis, max_qgis, _ = checker.ARCGIS_COMPATIBILITY_MATRIX[arcgis_major_minor]
        return max_qgis  # 返回测试的最高版本
    
    return None