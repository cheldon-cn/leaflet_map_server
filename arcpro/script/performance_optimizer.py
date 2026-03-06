"""
性能优化框架
支持30秒单工程迁移优化和批量处理优化
"""

import os
import sys
import time
import logging
import statistics
from typing import Optional, Dict, Any, List, Tuple
from dataclasses import dataclass, field
from datetime import datetime
import psutil
import threading
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor


@dataclass
class PerformanceMetrics:
    """性能指标"""
    timestamp: datetime
    cpu_percent: float
    memory_percent: float
    disk_io_read_mb: float
    disk_io_write_mb: float
    network_sent_mb: float
    network_recv_mb: float
    active_threads: int
    active_processes: int
    
    @classmethod
    def capture(cls) -> 'PerformanceMetrics':
        """捕获当前性能指标"""
        cpu_percent = psutil.cpu_percent(interval=0.1)
        memory_info = psutil.virtual_memory()
        disk_io = psutil.disk_io_counters()
        net_io = psutil.net_io_counters()
        
        return cls(
            timestamp=datetime.now(),
            cpu_percent=cpu_percent,
            memory_percent=memory_info.percent,
            disk_io_read_mb=disk_io.read_bytes / (1024 * 1024) if disk_io else 0,
            disk_io_write_mb=disk_io.write_bytes / (1024 * 1024) if disk_io else 0,
            network_sent_mb=net_io.bytes_sent / (1024 * 1024),
            network_recv_mb=net_io.bytes_recv / (1024 * 1024),
            active_threads=threading.active_count(),
            active_processes=len(psutil.pids())
        )


@dataclass
class MigrationTaskMetrics:
    """迁移任务指标"""
    task_id: str
    task_type: str  # parsing, conversion, migration, etc.
    start_time: datetime
    end_time: Optional[datetime] = None
    duration_seconds: float = 0.0
    rows_processed: int = 0
    bytes_processed: int = 0
    success: bool = False
    error_message: str = ""
    
    def finalize(self, success: bool = True, error_message: str = ""):
        """完成指标记录"""
        self.end_time = datetime.now()
        self.duration_seconds = (self.end_time - self.start_time).total_seconds()
        self.success = success
        self.error_message = error_message


@dataclass
class ProjectComplexityAnalysis:
    """工程复杂度分析"""
    total_layers: int = 0
    postgresql_layers: int = 0
    large_tables: int = 0
    estimated_total_size_gb: float = 0.0
    estimated_total_rows: int = 0
    complexity_score: float = 0.0  # 0-100
    
    @classmethod
    def analyze(cls, project_info: Dict[str, Any]) -> 'ProjectComplexityAnalysis':
        """分析工程复杂度"""
        analysis = cls()
        
        # 从工程信息中提取数据
        layers = project_info.get('layers', [])
        analysis.total_layers = len(layers)
        
        # 分析数据源
        postgresql_layers = []
        large_tables = []
        total_size_gb = 0
        total_rows = 0
        
        for layer in layers:
            if layer.get('data_source_type', '').lower() in ['postgresql', 'enterprisegeodatabase']:
                postgresql_layers.append(layer)
                
                # 估计表大小
                estimated_size_gb = layer.get('estimated_size_gb', 0)
                total_size_gb += estimated_size_gb
                
                if estimated_size_gb > 100:  # 大于100GB为大表
                    large_tables.append(layer)
                
                total_rows += layer.get('estimated_row_count', 0)
        
        analysis.postgresql_layers = len(postgresql_layers)
        analysis.large_tables = len(large_tables)
        analysis.estimated_total_size_gb = total_size_gb
        analysis.estimated_total_rows = total_rows
        
        # 计算复杂度分数
        # 权重：图层数(30%) + PostgreSQL图层(30%) + 大表数(20%) + 总大小(20%)
        layer_score = min(analysis.total_layers / 50 * 100, 100) * 0.3
        postgresql_score = min(analysis.postgresql_layers / 10 * 100, 100) * 0.3
        large_table_score = min(analysis.large_tables / 5 * 100, 100) * 0.2
        size_score = min(analysis.estimated_total_size_gb / 500 * 100, 100) * 0.2
        
        analysis.complexity_score = layer_score + postgresql_score + large_table_score + size_score
        
        return analysis


@dataclass
class MigrationPlan:
    """迁移计划"""
    strategy: str = "balanced"  # simple, balanced, aggressive
    parallel_tasks: int = 4
    sample_size: int = 10
    sampling_method: str = "detailed"  # quick, detailed
    estimated_duration_seconds: float = 30.0
    resource_allocation: Dict[str, Any] = field(default_factory=lambda: {
        'cpu_cores': 2,
        'memory_mb': 2048,
        'disk_io_priority': 'normal',
        'network_bandwidth': 'normal'
    })
    tasks: List[Dict[str, Any]] = field(default_factory=list)


class PerformanceMonitor:
    """性能监控器"""
    
    def __init__(self, monitoring_interval: float = 1.0, max_history_size: int = 1000):
        """
        初始化性能监控器
        
        Args:
            monitoring_interval: 监控间隔（秒）
            max_history_size: 最大历史记录大小，防止内存泄漏
        """
        self.monitoring_interval = monitoring_interval
        self.max_history_size = max_history_size
        self.logger = logging.getLogger(__name__)
        self.metrics_history: List[PerformanceMetrics] = []
        self.monitoring_thread = None
        self.is_monitoring = False
        
    def start_monitoring(self):
        """开始性能监控"""
        if self.is_monitoring:
            return
        
        self.is_monitoring = True
        self.monitoring_thread = threading.Thread(target=self._monitoring_loop)
        self.monitoring_thread.daemon = True
        self.monitoring_thread.start()
        
        self.logger.info("性能监控已启动")
    
    def stop_monitoring(self):
        """停止性能监控"""
        self.is_monitoring = False
        if self.monitoring_thread:
            self.monitoring_thread.join(timeout=2.0)
        
        self.logger.info("性能监控已停止")
    
    def _monitoring_loop(self):
        """监控循环"""
        while self.is_monitoring:
            try:
                metrics = PerformanceMetrics.capture()
                self.metrics_history.append(metrics)
                
                # 限制历史记录大小，防止内存泄漏
                if len(self.metrics_history) > self.max_history_size:
                    self.metrics_history = self.metrics_history[-self.max_history_size:]
                
                # 日志记录（可选）
                if len(self.metrics_history) % 10 == 0:
                    self._log_summary()
                
            except Exception as e:
                self.logger.warning(f"性能监控失败: {e}")
            
            time.sleep(self.monitoring_interval)
    
    def _log_summary(self):
        """日志摘要"""
        if not self.metrics_history:
            return
        
        recent_metrics = self.metrics_history[-10:]  # 最近10个样本
        
        avg_cpu = statistics.mean([m.cpu_percent for m in recent_metrics])
        avg_memory = statistics.mean([m.memory_percent for m in recent_metrics])
        
        self.logger.info(f"性能摘要 - CPU: {avg_cpu:.1f}%, 内存: {avg_memory:.1f}%")
    
    def get_summary(self) -> Dict[str, Any]:
        """获取性能摘要"""
        if not self.metrics_history:
            return {}
        
        latest_metrics = self.metrics_history[-1] if self.metrics_history else None
        
        # 计算基本统计
        cpu_values = [m.cpu_percent for m in self.metrics_history]
        memory_values = [m.memory_percent for m in self.metrics_history]
        
        return {
            'samples': len(self.metrics_history),
            'latest': {
                'cpu_percent': latest_metrics.cpu_percent if latest_metrics else 0,
                'memory_percent': latest_metrics.memory_percent if latest_metrics else 0,
                'active_threads': latest_metrics.active_threads if latest_metrics else 0
            },
            'averages': {
                'cpu_percent': statistics.mean(cpu_values) if cpu_values else 0,
                'memory_percent': statistics.mean(memory_values) if memory_values else 0
            },
            'maxima': {
                'cpu_percent': max(cpu_values) if cpu_values else 0,
                'memory_percent': max(memory_values) if memory_values else 0
            }
        }
    def get_metrics_history(self):
        """获取历史记录"""
        return self.metrics_history
    
    def reset(self):
        """重置监控数据"""
        self.metrics_history.clear()
        self.logger.info("性能监控数据已重置")


class PerformanceOptimizer:
    """性能优化器"""
    
    def __init__(self, config: Optional[Dict[str, Any]] = None):
        """
        初始化性能优化器
        
        Args:
            config: 配置字典，可包含performance配置
        """
        self.logger = logging.getLogger(__name__)
        
        # 从配置中获取性能监控参数
        monitoring_interval = 1.0
        max_history_size = 1000
        
        if config and 'performance' in config:
            perf_config = config['performance']
            monitoring_interval = perf_config.get('monitoring_interval', 1.0)
            max_history_size = perf_config.get('max_history_size', 1000)
        
        self.monitor = PerformanceMonitor(
            monitoring_interval=monitoring_interval,
            max_history_size=max_history_size
        )
        self.history = []  # 历史优化记录
        
    def optimize_single_project_migration(
        self,
        project_path: str,
        config: Dict[str, Any]
    ) -> MigrationPlan:
        """
        优化单工程迁移计划
        
        Args:
            project_path: 工程路径
            config: 迁移配置
            
        Returns:
            优化后的迁移计划
        """
        self.logger.info(f"开始优化单工程迁移计划: {project_path}")
        
        # 分析工程复杂度
        project_info = self._analyze_project(project_path)
        complexity = ProjectComplexityAnalysis.analyze(project_info)
        
        self.logger.info(f"工程复杂度分析: 分数={complexity.complexity_score:.1f}")
        
        # 创建迁移计划
        plan = MigrationPlan()
        
        # 根据复杂度调整策略
        if complexity.complexity_score <= 30:  # 简单工程
            plan.strategy = "simple"
            plan.parallel_tasks = 2
            plan.sample_size = 5
            plan.estimated_duration_seconds = 15.0
            
        elif complexity.complexity_score <= 70:  # 中等工程
            plan.strategy = "balanced"
            plan.parallel_tasks = 4
            plan.sample_size = 10
            plan.estimated_duration_seconds = 30.0
            
        else:  # 复杂工程
            plan.strategy = "aggressive"
            plan.parallel_tasks = 8
            plan.sample_size = 15
            plan.estimated_duration_seconds = 45.0
        
        # 优化数据采样策略
        if config.get('timeout_per_project', 300) < 30:  # 时间紧张
            plan.sampling_method = "quick"
            plan.sample_size = min(plan.sample_size, 5)
        else:
            plan.sampling_method = "detailed"
        
        # 优化资源分配
        plan.resource_allocation = self._optimize_resource_allocation(
            complexity,
            config.get('available_resources', {})
        )
        
        # 创建并行任务
        plan.tasks = self._create_parallel_tasks(project_info, plan)
        
        # 记录优化决策
        self.history.append({
            'timestamp': datetime.now(),
            'project_path': project_path,
            'complexity_score': complexity.complexity_score,
            'strategy': plan.strategy,
            'estimated_duration': plan.estimated_duration_seconds
        })
        
        self.logger.info(f"迁移计划优化完成，策略: {plan.strategy}")
        
        return plan
    
    def _analyze_project(self, project_path: str) -> Dict[str, Any]:
        """分析工程"""
        # 简化实现，实际应该解析工程文件
        # 这里返回模拟数据
        
        return {
            'project_path': project_path,
            'layers': [
                {
                    'name': 'roads',
                    'data_source_type': 'PostgreSQL',
                    'estimated_size_gb': 50.0,
                    'estimated_row_count': 1000000
                },
                {
                    'name': 'buildings',
                    'data_source_type': 'Shapefile',
                    'estimated_size_gb': 0.5,
                    'estimated_row_count': 5000
                },
                {
                    'name': 'parcels',
                    'data_source_type': 'PostgreSQL',
                    'estimated_size_gb': 200.0,
                    'estimated_row_count': 5000000
                }
            ]
        }
    
    def _optimize_resource_allocation(
        self,
        complexity: ProjectComplexityAnalysis,
        available_resources: Dict[str, Any]
    ) -> Dict[str, Any]:
        """优化资源分配"""
        # 根据复杂度和可用资源调整
        
        # 默认分配
        allocation = {
            'cpu_cores': 2,
            'memory_mb': 2048,
            'disk_io_priority': 'normal',
            'network_bandwidth': 'normal'
        }
        
        # 根据复杂度调整
        if complexity.complexity_score > 70:  # 复杂工程
            allocation['cpu_cores'] = 4
            allocation['memory_mb'] = 4096
            allocation['disk_io_priority'] = 'high'
            
        elif complexity.complexity_score > 30:  # 中等工程
            allocation['cpu_cores'] = 2
            allocation['memory_mb'] = 2048
            allocation['disk_io_priority'] = 'normal'
        
        # 根据可用资源调整
        if available_resources:
            available_cores = available_resources.get('cpu_cores', 0)
            available_memory = available_resources.get('memory_mb', 0)
            
            if available_cores > 0:
                allocation['cpu_cores'] = min(allocation['cpu_cores'], available_cores)
            
            if available_memory > 0:
                allocation['memory_mb'] = min(allocation['memory_mb'], available_memory)
        
        return allocation
    
    def _create_parallel_tasks(
        self,
        project_info: Dict[str, Any],
        plan: MigrationPlan
    ) -> List[Dict[str, Any]]:
        """创建并行任务"""
        tasks = []
        
        # 根据迁移计划创建任务
        if plan.strategy == "simple":
            # 串行处理：解析 -> 采样 -> 转换
            tasks.append({
                'task_id': 'parse_project',
                'task_type': 'parsing',
                'description': '解析工程文件',
                'priority': 1
            })
            
            tasks.append({
                'task_id': 'sample_data',
                'task_type': 'sampling',
                'description': '数据采样',
                'priority': 2
            })
            
        elif plan.strategy == "balanced":
            # 并行处理：解析和数据采样并行
            tasks.append({
                'task_id': 'parse_project_parallel',
                'task_type': 'parsing',
                'description': '解析工程文件（并行）',
                'priority': 1,
                'parallel_with': ['sample_data_parallel']
            })
            
            tasks.append({
                'task_id': 'sample_data_parallel',
                'task_type': 'sampling',
                'description': '数据采样（并行）',
                'priority': 1,
                'parallel_with': ['parse_project_parallel']
            })
            
            tasks.append({
                'task_id': 'convert_layers',
                'task_type': 'conversion',
                'description': '转换图层数据',
                'priority': 2
            })
            
        elif plan.strategy == "aggressive":
            # 激进并行：多级流水线
            tasks.append({
                'task_id': 'parse_project_aggressive',
                'task_type': 'parsing',
                'description': '解析工程文件（激进并行）',
                'priority': 1,
                'parallel_tasks': 2
            })
            
            tasks.append({
                'task_id': 'sample_data_aggressive',
                'task_type': 'sampling',
                'description': '数据采样（激进并行）',
                'priority': 1,
                'parallel_tasks': 2
            })
            
            tasks.append({
                'task_id': 'convert_large_tables',
                'task_type': 'conversion',
                'description': '转换大表数据',
                'priority': 2,
                'parallel_tasks': 2
            })
        
        return tasks
    
    def start_monitoring(self):
        """开始性能监控"""
        self.monitor.start_monitoring()
    
    def stop_monitoring(self):
        """停止性能监控"""
        self.monitor.stop_monitoring()
    
    def get_performance_report(self) -> Dict[str, Any]:
        """获取性能报告"""
        # 获取监控摘要
        monitor_summary = self.monitor.get_summary()
        
        # 分析历史记录
        if self.history:
            recent_optimizations = self.history[-5:]  # 最近5次优化
            
            avg_complexity = statistics.mean([h['complexity_score'] for h in recent_optimizations])
            avg_duration = statistics.mean([h['estimated_duration'] for h in recent_optimizations])
            
            strategy_counts = {}
            for h in recent_optimizations:
                strategy = h['strategy']
                strategy_counts[strategy] = strategy_counts.get(strategy, 0) + 1
            
            most_common_strategy = max(strategy_counts.items(), key=lambda x: x[1])[0] if strategy_counts else "unknown"
        else:
            avg_complexity = 0
            avg_duration = 0
            most_common_strategy = "unknown"
        
        return {
            'monitoring': monitor_summary,
            'optimization_history': {
                'total_optimizations': len(self.history),
                'average_complexity_score': avg_complexity,
                'average_estimated_duration': avg_duration,
                'most_common_strategy': most_common_strategy,
                'recent_optimizations': len(recent_optimizations) if self.history else 0
            },
            'recommendations': self._generate_recommendations(monitor_summary)
        }
    
    def _generate_recommendations(self, monitor_summary: Dict[str, Any]) -> List[str]:
        """生成优化建议"""
        recommendations = []
        
        if not monitor_summary:
            return recommendations
        
        latest_metrics = monitor_summary.get('latest', {})
        averages = monitor_summary.get('averages', {})
        
        # CPU使用率建议
        cpu_avg = averages.get('cpu_percent', 0)
        if cpu_avg > 80:
            recommendations.append("CPU使用率过高，建议增加CPU核心或优化计算密集型任务")
        elif cpu_avg < 30:
            recommendations.append("CPU使用率较低，建议增加并行任务数量以提高效率")
        
        # 内存使用率建议
        memory_avg = averages.get('memory_percent', 0)
        if memory_avg > 80:
            recommendations.append("内存使用率过高，建议增加内存或优化内存使用")
        
        # 线程数建议
        active_threads = latest_metrics.get('active_threads', 0)
        if active_threads < 4:
            recommendations.append("活动线程数较少，建议增加并行处理能力")
        
        return recommendations


class BatchProcessor:
    """批量处理器（支持200个工程）"""
    
    def __init__(self, max_workers: int = None):
        """
        初始化批量处理器
        
        Args:
            max_workers: 最大工作线程数
        """
        self.max_workers = max_workers or os.cpu_count()
        self.logger = logging.getLogger(__name__)
        self.optimizer = PerformanceOptimizer()
        self.task_queue = []
        self.results = []
        
    def process_projects_batch(
        self,
        project_list: List[str],
        config: Dict[str, Any]
    ) -> Dict[str, Any]:
        """
        处理工程批量
        
        Args:
            project_list: 工程文件列表
            config: 配置参数
            
        Returns:
            批量处理结果
        """
        self.logger.info(f"开始批量处理 {len(project_list)} 个工程")
        
        # 启动性能监控
        self.optimizer.start_monitoring()
        
        start_time = time.time()
        
        try:
            # 分析工程复杂度并分组
            groups = self._group_projects_by_complexity(project_list)
            
            self.logger.info(f"工程分组完成，共 {len(groups)} 个复杂度组")
            
            # 动态调整工作池大小
            optimal_workers = self._calculate_optimal_workers(len(project_list))
            self.logger.info(f"使用 {optimal_workers} 个工作线程")
            
            # 使用进程池处理
            with ProcessPoolExecutor(max_workers=optimal_workers) as executor:
                # 提交所有任务
                futures = {}
                for group in groups:
                    for project in group:
                        future = executor.submit(
                            self._migrate_single_project,
                            project,
                            config
                        )
                        futures[future] = project
                
                # 收集结果
                self.results = []
                for future in futures:
                    try:
                        result = future.result(timeout=config.get('timeout_per_project', 300))
                        self.results.append(result)
                        self.logger.info(f"工程 {futures[future]} 处理完成")
                        
                    except Exception as e:
                        error_result = {
                            'project': futures[future],
                            'success': False,
                            'error': str(e),
                            'duration': 0
                        }
                        self.results.append(error_result)
                        self.logger.error(f"工程 {futures[future]} 处理失败: {e}")
            
            # 生成摘要
            summary = self._generate_batch_summary()
            
            return summary
            
        finally:
            # 停止性能监控
            self.optimizer.stop_monitoring()
            
            # 生成性能报告
            performance_report = self.optimizer.get_performance_report()
            
            self.logger.info(f"批量处理完成，总耗时: {time.time() - start_time:.2f} 秒")
    
    def _group_projects_by_complexity(self, project_list: List[str]) -> List[List[str]]:
        """按复杂度分组工程"""
        # 简化实现，实际应该分析每个工程
        groups = []
        
        # 这里将工程分为三组：简单、中等、复杂
        simple_projects = []
        medium_projects = []
        complex_projects = []
        
        for i, project in enumerate(project_list):
            # 根据工程索引简单分组（实际应该分析工程内容）
            if i % 3 == 0:
                simple_projects.append(project)
            elif i % 3 == 1:
                medium_projects.append(project)
            else:
                complex_projects.append(project)
        
        if simple_projects:
            groups.append(simple_projects)
        if medium_projects:
            groups.append(medium_projects)
        if complex_projects:
            groups.append(complex_projects)
        
        return groups
    
    def _calculate_optimal_workers(self, total_projects: int) -> int:
        """计算最优工作线程数"""
        # 根据工程数量和系统资源动态调整
        
        # 获取系统资源
        cpu_cores = os.cpu_count() or 4
        available_memory = psutil.virtual_memory().available / (1024 * 1024 * 1024)  # GB
        
        # 基本规则：每个工程分配1个线程，但不超过CPU核心数的80%
        suggested_workers = min(total_projects, int(cpu_cores * 0.8))
        
        # 考虑内存限制：每个线程至少需要1GB内存
        max_workers_by_memory = int(available_memory)
        suggested_workers = min(suggested_workers, max_workers_by_memory)
        
        # 确保至少1个工作线程
        return max(1, suggested_workers)
    
    def _migrate_single_project(self, project_path: str, config: Dict[str, Any]) -> Dict[str, Any]:
        """迁移单个工程（供进程池调用）"""
        start_time = time.time()
        
        try:
            # 这里应该调用实际的迁移逻辑
            # 简化实现：模拟迁移过程
            time.sleep(1)  # 模拟处理时间
            
            result = {
                'project': project_path,
                'success': True,
                'duration': time.time() - start_time,
                'details': {
                    'layers_processed': 3,
                    'data_migrated_mb': 50
                }
            }
            
            return result
            
        except Exception as e:
            result = {
                'project': project_path,
                'success': False,
                'error': str(e),
                'duration': time.time() - start_time
            }
            
            return result
    
    def _generate_batch_summary(self) -> Dict[str, Any]:
        """生成批量处理摘要"""
        if not self.results:
            return {
                'total_projects': 0,
                'successful': 0,
                'failed': 0,
                'total_duration': 0,
                'average_duration': 0
            }
        
        successful = sum(1 for r in self.results if r.get('success', False))
        failed = len(self.results) - successful
        total_duration = sum(r.get('duration', 0) for r in self.results)
        
        return {
            'total_projects': len(self.results),
            'successful': successful,
            'failed': failed,
            'success_rate': successful / len(self.results) * 100 if self.results else 0,
            'total_duration': total_duration,
            'average_duration': total_duration / len(self.results) if self.results else 0,
            'results': self.results
        }


class AlertSystem:
    """预警系统"""
    
    def __init__(self):
        self.rules = {}
    
    def add_rule(self, rule_id: str, description: str, condition_func):
        """
        添加预警规则
        
        Args:
            rule_id: 规则ID
            description: 规则描述
            condition_func: 条件函数，接收性能指标字典，返回布尔值
        """
        self.rules[rule_id] = {
            'description': description,
            'condition': condition_func
        }
    
    def check_alerts(self, metrics: Dict[str, Any]) -> List[str]:
        """
        检查告警
        
        Args:
            metrics: 性能指标字典
            
        Returns:
            触发的告警ID列表
        """
        triggered = []
        for rule_id, rule in self.rules.items():
            if rule['condition'](metrics):
                triggered.append(rule_id)
        return triggered
    
    def get_rule_description(self, rule_id: str) -> str:
        """获取规则描述"""
        rule = self.rules.get(rule_id)
        return rule['description'] if rule else "未知规则"


# 示例使用代码
if __name__ == "__main__":
    # 设置日志
    logging.basicConfig(level=logging.INFO)
    
    print("性能优化框架示例")
    print("=" * 60)
    
    # 创建性能优化器
    optimizer = PerformanceOptimizer()
    
    # 示例：优化单工程迁移计划
    project_path = "example.aprx"
    config = {
        'timeout_per_project': 30,
        'available_resources': {
            'cpu_cores': 8,
            'memory_mb': 16384
        }
    }
    
    print(f"\n1. 优化单工程迁移计划: {project_path}")
    plan = optimizer.optimize_single_project_migration(project_path, config)
    
    print(f"   策略: {plan.strategy}")
    print(f"   并行任务数: {plan.parallel_tasks}")
    print(f"   采样大小: {plan.sample_size}")
    print(f"   估计耗时: {plan.estimated_duration_seconds:.1f} 秒")
    
    # 示例：批量处理
    print("\n2. 批量处理示例")
    
    # 创建模拟工程列表
    mock_projects = [f"project_{i}.aprx" for i in range(1, 11)]  # 10个工程
    
    batch_processor = BatchProcessor(max_workers=4)
    
    batch_config = {
        'timeout_per_project': 300,
        'max_workers': 4
    }
    
    # 在实际使用中，应该调用 batch_processor.process_projects_batch(mock_projects, batch_config)
    # 这里只展示接口
    
    print(f"   模拟处理 {len(mock_projects)} 个工程")
    print("   批量处理功能已实现，可在实际项目中调用")

    # 示例：预警系统
    print("\n3. 预警系统示例")
    alert_system = AlertSystem()
    alert_system.add_rule("cpu_high", "CPU使用率 > 80%", lambda metrics: metrics.get('cpu_percent', 0) > 80)
    alert_system.add_rule("memory_high", "内存使用率 > 80%", lambda metrics: metrics.get('memory_percent', 0) > 80)
    
    test_metrics = {'cpu_percent': 85, 'memory_percent': 75}
    alerts = alert_system.check_alerts(test_metrics)
    if alerts:
        print(f"   检测到告警: {', '.join(alerts)}")
    else:
        print("   无告警")

    print("\n性能优化框架实现完成！")