"""
性能优化模块测试
"""

import os
import sys
import time
import threading
from pathlib import Path

# 添加父目录到路径
sys.path.insert(0, str(Path(__file__).parent.parent))

from performance_optimizer import (
    PerformanceMetrics,
    PerformanceMonitor,
    PerformanceOptimizer,
    MigrationTask,
    BatchScheduler
)


class TestPerformanceMetrics:
    """测试性能指标类"""
    
    def test_capture_metrics(self):
        """测试捕获性能指标"""
        metrics = PerformanceMetrics.capture()
        
        # 验证指标存在且类型正确
        assert hasattr(metrics, 'timestamp')
        assert hasattr(metrics, 'cpu_percent')
        assert hasattr(metrics, 'memory_percent')
        assert hasattr(metrics, 'disk_io_read_mb')
        assert hasattr(metrics, 'disk_io_write_mb')
        assert hasattr(metrics, 'network_sent_mb')
        assert hasattr(metrics, 'network_recv_mb')
        assert hasattr(metrics, 'active_threads')
        assert hasattr(metrics, 'active_processes')
        
        # 验证指标值在合理范围内
        assert 0 <= metrics.cpu_percent <= 100
        assert 0 <= metrics.memory_percent <= 100
        assert metrics.disk_io_read_mb >= 0
        assert metrics.disk_io_write_mb >= 0
        assert metrics.network_sent_mb >= 0
        assert metrics.network_recv_mb >= 0
        assert metrics.active_threads >= 0
        assert metrics.active_processes >= 0
    
    def test_metrics_serialization(self):
        """测试指标序列化"""
        metrics = PerformanceMetrics.capture()
        
        # 转换为字典
        metrics_dict = metrics.__dict__
        
        # 验证字典包含所有字段
        expected_fields = [
            'timestamp', 'cpu_percent', 'memory_percent',
            'disk_io_read_mb', 'disk_io_write_mb',
            'network_sent_mb', 'network_recv_mb',
            'active_threads', 'active_processes'
        ]
        
        for field in expected_fields:
            assert field in metrics_dict


class TestPerformanceMonitor:
    """测试性能监控器"""
    
    def test_monitor_initialization(self):
        """测试监控器初始化"""
        monitor = PerformanceMonitor(monitoring_interval=0.1, max_history_size=50)
        
        assert monitor.monitoring_interval == 0.1
        assert monitor.max_history_size == 50
        assert monitor.metrics_history == []
        assert monitor.is_monitoring is False
        assert monitor.monitoring_thread is None
    
    def test_start_stop_monitoring(self):
        """测试启动和停止监控"""
        monitor = PerformanceMonitor(monitoring_interval=0.05, max_history_size=10)
        
        # 启动监控
        monitor.start_monitoring()
        assert monitor.is_monitoring is True
        assert monitor.monitoring_thread is not None
        assert monitor.monitoring_thread.is_alive()
        
        # 等待一小段时间收集数据
        time.sleep(0.2)
        
        # 停止监控
        monitor.stop_monitoring()
        time.sleep(0.1)  # 等待线程停止
        
        assert monitor.is_monitoring is False
        assert len(monitor.metrics_history) > 0
    
    def test_history_size_limit(self):
        """测试历史记录大小限制"""
        monitor = PerformanceMonitor(monitoring_interval=0.01, max_history_size=5)
        
        monitor.start_monitoring()
        time.sleep(0.1)  # 收集一些数据点
        monitor.stop_monitoring()
        
        # 历史记录不应超过最大值
        assert len(monitor.metrics_history) <= monitor.max_history_size
    
    def test_get_metrics_history(self):
        """测试获取历史记录"""
        monitor = PerformanceMonitor(monitoring_interval=0.05, max_history_size=10)
        
        # 手动添加一些测试数据
        test_metrics = [
            PerformanceMetrics.capture(),
            PerformanceMetrics.capture()
        ]
        monitor.metrics_history.extend(test_metrics)
        
        history = monitor.get_metrics_history()
        assert len(history) == 2
        assert history[0].timestamp <= history[1].timestamp
    
    def test_get_summary_statistics(self):
        """测试获取统计摘要"""
        monitor = PerformanceMonitor(monitoring_interval=0.05, max_history_size=10)
        
        # 手动添加测试数据
        for _ in range(3):
            monitor.metrics_history.append(PerformanceMetrics.capture())
        
        summary = monitor.get_summary_statistics()
        
        # 验证摘要包含所需字段
        assert 'cpu_stats' in summary
        assert 'memory_stats' in summary
        assert 'disk_io_stats' in summary
        assert 'network_stats' in summary
        
        cpu_stats = summary['cpu_stats']
        assert 'min' in cpu_stats
        assert 'max' in cpu_stats
        assert 'avg' in cpu_stats
    
    def test_context_manager(self):
        """测试上下文管理器用法"""
        with PerformanceMonitor(monitoring_interval=0.05, max_history_size=5) as monitor:
            # 在上下文中监控应该激活
            assert monitor.is_monitoring is True
            time.sleep(0.1)
        
        # 退出上下文后监控应该停止
        assert monitor.is_monitoring is False
        assert len(monitor.metrics_history) > 0


class TestPerformanceOptimizer:
    """测试性能优化器"""
    
    def test_optimizer_initialization(self):
        """测试优化器初始化"""
        # 无配置初始化
        optimizer = PerformanceOptimizer()
        assert optimizer.monitor is not None
        
        # 带配置初始化
        config = {
            'performance': {
                'monitoring_interval': 0.5,
                'max_history_size': 100
            }
        }
        optimizer_with_config = PerformanceOptimizer(config)
        assert optimizer_with_config.monitor is not None
    
    def test_analyze_performance(self):
        """测试性能分析"""
        optimizer = PerformanceOptimizer()
        
        # 启动监控
        optimizer.monitor.start_monitoring()
        time.sleep(0.1)
        optimizer.monitor.stop_monitoring()
        
        # 分析性能
        analysis = optimizer.analyze_performance()
        
        # 验证分析结果包含所需信息
        assert 'summary' in analysis
        assert 'recommendations' in analysis
        assert isinstance(analysis['recommendations'], list)
    
    def test_optimize_migration_task(self):
        """测试迁移任务优化"""
        optimizer = PerformanceOptimizer()
        
        # 创建模拟任务
        task_config = {
            'project_path': '/fake/path.aprx',
            'output_dir': '/fake/output',
            'data_migration_strategy': 'full_copy'
        }
        
        # 测试优化（模拟）
        optimized_config = optimizer.optimize_migration_task(task_config)
        
        # 验证优化后的配置包含原始字段
        assert 'project_path' in optimized_config
        assert optimized_config['project_path'] == '/fake/path.aprx'
        
        # 验证添加了优化字段
        assert 'optimized' in optimized_config
        assert optimized_config['optimized'] is True
    
    def test_generate_performance_report(self):
        """测试生成性能报告"""
        optimizer = PerformanceOptimizer()
        
        # 收集一些数据
        optimizer.monitor.start_monitoring()
        time.sleep(0.1)
        optimizer.monitor.stop_monitoring()
        
        # 生成报告
        report = optimizer.generate_performance_report()
        
        # 验证报告结构
        assert 'timestamp' in report
        assert 'performance_metrics' in report
        assert 'analysis' in report
        assert 'recommendations' in report


class TestMigrationTask:
    """测试迁移任务类"""
    
    def test_task_initialization(self):
        """测试任务初始化"""
        task = MigrationTask(
            task_id="test_task_001",
            project_path="/test/project.aprx",
            output_dir="/test/output"
        )
        
        assert task.task_id == "test_task_001"
        assert task.project_path == "/test/project.aprx"
        assert task.output_dir == "/test/output"
        assert task.status == "pending"
        assert task.priority == 1
    
    def test_task_execution_simulation(self):
        """测试任务执行模拟"""
        task = MigrationTask(
            task_id="test_task_002",
            project_path="/test/project2.aprx"
        )
        
        # 模拟执行
        task.execute()
        
        # 验证状态更新
        assert task.status == "completed"
        assert task.completion_time is not None
        assert task.execution_duration > 0
    
    def test_task_priority(self):
        """测试任务优先级"""
        task1 = MigrationTask(task_id="t1", priority=3)
        task2 = MigrationTask(task_id="t2", priority=1)
        task3 = MigrationTask(task_id="t3", priority=2)
        
        tasks = [task1, task2, task3]
        sorted_tasks = sorted(tasks, key=lambda t: t.priority)
        
        # 验证按优先级排序（数字越小优先级越高）
        assert sorted_tasks[0].task_id == "t2"  # 优先级1
        assert sorted_tasks[1].task_id == "t3"  # 优先级2
        assert sorted_tasks[2].task_id == "t1"  # 优先级3


class TestBatchScheduler:
    """测试批量调度器"""
    
    def test_scheduler_initialization(self):
        """测试调度器初始化"""
        scheduler = BatchScheduler(max_workers=2, batch_size=5)
        
        assert scheduler.max_workers == 2
        assert scheduler.batch_size == 5
        assert scheduler.pending_tasks == []
        assert scheduler.running_tasks == []
        assert scheduler.completed_tasks == []
    
    def test_add_tasks(self):
        """测试添加任务"""
        scheduler = BatchScheduler()
        
        tasks = [
            MigrationTask(task_id=f"task_{i}", project_path=f"/path/{i}.aprx")
            for i in range(3)
        ]
        
        for task in tasks:
            scheduler.add_task(task)
        
        assert len(scheduler.pending_tasks) == 3
        assert all(task.status == "pending" for task in scheduler.pending_tasks)
    
    def test_schedule_batch(self):
        """测试调度批次"""
        scheduler = BatchScheduler(max_workers=2, batch_size=2)
        
        # 添加4个任务
        for i in range(4):
            scheduler.add_task(MigrationTask(task_id=f"task_{i}"))
        
        # 调度批次
        scheduled = scheduler.schedule_batch()
        
        # 验证调度结果
        assert len(scheduled) == 2  # batch_size=2
        assert len(scheduler.running_tasks) == 2
        assert len(scheduler.pending_tasks) == 2
    
    def test_complete_task(self):
        """测试完成任务"""
        scheduler = BatchScheduler()
        
        task = MigrationTask(task_id="test_task")
        scheduler.add_task(task)
        scheduler.schedule_batch()  # 将任务移到运行中
        
        # 完成任务
        scheduler.complete_task(task.task_id)
        
        assert len(scheduler.running_tasks) == 0
        assert len(scheduler.completed_tasks) == 1
        assert scheduler.completed_tasks[0].task_id == "test_task"
        assert scheduler.completed_tasks[0].status == "completed"
    
    def test_get_progress(self):
        """测试获取进度"""
        scheduler = BatchScheduler()
        
        # 添加10个任务
        for i in range(10):
            scheduler.add_task(MigrationTask(task_id=f"task_{i}"))
        
        # 调度5个
        scheduler.max_workers = 5
        scheduled = scheduler.schedule_batch()
        
        # 完成2个
        for task in scheduled[:2]:
            scheduler.complete_task(task.task_id)
        
        progress = scheduler.get_progress()
        
        assert progress['total'] == 10
        assert progress['completed'] == 2
        assert progress['running'] == 3
        assert progress['pending'] == 5
        assert 0 <= progress['percentage'] <= 100


def test_integration_scenario():
    """测试集成场景"""
    # 创建调度器
    scheduler = BatchScheduler(max_workers=3, batch_size=3)
    
    # 添加多个任务
    for i in range(9):
        task = MigrationTask(
            task_id=f"project_{i}",
            project_path=f"/data/projects/project_{i}.aprx",
            output_dir=f"/output/migration_{i}",
            priority=i % 3 + 1  # 优先级1-3
        )
        scheduler.add_task(task)
    
    # 调度并执行任务
    completed_count = 0
    while scheduler.pending_tasks or scheduler.running_tasks:
        # 调度新批次
        if scheduler.pending_tasks:
            scheduled = scheduler.schedule_batch()
            for task in scheduled:
                # 模拟执行
                task.execute()
                scheduler.complete_task(task.task_id)
                completed_count += 1
        
        time.sleep(0.01)  # 短暂延迟
    
    # 验证所有任务完成
    assert completed_count == 9
    assert len(scheduler.completed_tasks) == 9
    assert all(task.status == "completed" for task in scheduler.completed_tasks)
    
    # 验证进度为100%
    progress = scheduler.get_progress()
    assert progress['percentage'] == 100.0