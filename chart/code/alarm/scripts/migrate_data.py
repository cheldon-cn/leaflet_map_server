#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
数据迁移脚本
用于将 alarm 模块数据迁移到 alert 模块格式

版本: v1.0
日期: 2026-04-06
"""

import json
import sqlite3
import os
from datetime import datetime
from typing import Dict, List, Optional, Any

class DataMigrator:
    """数据迁移工具类"""
    
    def __init__(self, source_db: str, target_db: str):
        self.source_db = source_db
        self.target_db = target_db
        self.stats = {
            'alerts_migrated': 0,
            'configs_migrated': 0,
            'errors': []
        }
    
    def migrate_all(self) -> bool:
        """执行完整迁移"""
        print("开始数据迁移...")
        
        if not os.path.exists(self.source_db):
            print(f"错误: 源数据库不存在: {self.source_db}")
            return False
        
        try:
            self._migrate_alerts()
            self._migrate_configs()
            self._print_summary()
            return True
        except Exception as e:
            print(f"迁移失败: {str(e)}")
            self.stats['errors'].append(str(e))
            return False
    
    def _migrate_alerts(self):
        """迁移预警数据"""
        print("\n迁移预警数据...")
        
        source_conn = sqlite3.connect(self.source_db)
        target_conn = sqlite3.connect(self.target_db)
        
        source_cursor = source_conn.cursor()
        target_cursor = target_conn.cursor()
        
        self._create_alert_tables(target_cursor)
        
        source_cursor.execute("""
            SELECT alert_id, alert_type, alert_level, status, title, message,
                   position_lon, position_lat, issue_time, expire_time,
                   acknowledged, acknowledge_time, action_taken
            FROM alerts
        """)
        
        for row in source_cursor.fetchall():
            try:
                alert_data = self._convert_alert(row)
                self._insert_alert(target_cursor, alert_data)
                self.stats['alerts_migrated'] += 1
            except Exception as e:
                self.stats['errors'].append(f"Alert {row[0]}: {str(e)}")
        
        target_conn.commit()
        source_conn.close()
        target_conn.close()
        
        print(f"  迁移预警: {self.stats['alerts_migrated']} 条")
    
    def _migrate_configs(self):
        """迁移配置数据"""
        print("\n迁移配置数据...")
        
        source_conn = sqlite3.connect(self.source_db)
        target_conn = sqlite3.connect(self.target_db)
        
        source_cursor = source_conn.cursor()
        target_cursor = target_conn.cursor()
        
        self._create_config_tables(target_cursor)
        
        source_cursor.execute("""
            SELECT user_id, depth_threshold_l1, depth_threshold_l2, 
                   depth_threshold_l3, depth_threshold_l4,
                   collision_threshold_l1, collision_threshold_l2,
                   collision_threshold_l3, collision_threshold_l4,
                   push_methods
            FROM alert_configs
        """)
        
        for row in source_cursor.fetchall():
            try:
                config_data = self._convert_config(row)
                self._insert_config(target_cursor, config_data)
                self.stats['configs_migrated'] += 1
            except Exception as e:
                self.stats['errors'].append(f"Config {row[0]}: {str(e)}")
        
        target_conn.commit()
        source_conn.close()
        target_conn.close()
        
        print(f"  迁移配置: {self.stats['configs_migrated']} 条")
    
    def _convert_alert(self, row: tuple) -> Dict[str, Any]:
        """转换预警数据格式"""
        alert_id, alert_type, alert_level, status, title, message, \
            position_lon, position_lat, issue_time, expire_time, \
            acknowledged, acknowledge_time, action_taken = row
        
        alert_type_map = {
            'kDepthAlert': 'kDepth',
            'kCollisionAlert': 'kCollision',
            'kWeatherAlert': 'kWeather',
            'kChannelAlert': 'kChannel'
        }
        
        alert_level_map = {
            'kLevel1_Critical': 'kLevel1',
            'kLevel2_Severe': 'kLevel2',
            'kLevel3_Moderate': 'kLevel3',
            'kLevel4_Minor': 'kLevel4'
        }
        
        status_map = {
            'kPending': 'kPending',
            'kActive': 'kActive',
            'kPushed': 'kPushed',
            'kAcknowledged': 'kAcknowledged',
            'kCleared': 'kCleared',
            'kExpired': 'kExpired'
        }
        
        return {
            'alert_id': alert_id,
            'alert_type': alert_type_map.get(alert_type, 'kUnknown'),
            'alert_level': alert_level_map.get(alert_level, 'kNone'),
            'status': status_map.get(status, 'kActive'),
            'content': json.dumps({
                'title': title,
                'message': message
            }),
            'position': json.dumps({
                'longitude': position_lon,
                'latitude': position_lat
            }),
            'issue_time': issue_time,
            'expire_time': expire_time,
            'acknowledge_required': True,
            'acknowledged': bool(acknowledged),
            'acknowledge_time': acknowledge_time,
            'action_taken': action_taken
        }
    
    def _convert_config(self, row: tuple) -> Dict[str, Any]:
        """转换配置数据格式"""
        user_id, depth_l1, depth_l2, depth_l3, depth_l4, \
            collision_l1, collision_l2, collision_l3, collision_l4, \
            push_methods = row
        
        return {
            'user_id': user_id,
            'depth_alert_enabled': True,
            'collision_alert_enabled': True,
            'weather_alert_enabled': True,
            'channel_alert_enabled': True,
            'depth_threshold': json.dumps({
                'level1_threshold': depth_l1,
                'level2_threshold': depth_l2,
                'level3_threshold': depth_l3,
                'level4_threshold': depth_l4
            }),
            'collision_threshold': json.dumps({
                'level1_threshold': collision_l1,
                'level2_threshold': collision_l2,
                'level3_threshold': collision_l3,
                'level4_threshold': collision_l4
            }),
            'push_methods': push_methods
        }
    
    def _create_alert_tables(self, cursor):
        """创建预警表"""
        cursor.execute("""
            CREATE TABLE IF NOT EXISTS alerts (
                alert_id TEXT PRIMARY KEY,
                alert_type TEXT NOT NULL,
                alert_level TEXT NOT NULL,
                status TEXT NOT NULL,
                content TEXT,
                position TEXT,
                issue_time TEXT,
                expire_time TEXT,
                acknowledge_required INTEGER DEFAULT 1,
                acknowledged INTEGER DEFAULT 0,
                acknowledge_time TEXT,
                action_taken TEXT,
                created_at TEXT DEFAULT CURRENT_TIMESTAMP,
                updated_at TEXT DEFAULT CURRENT_TIMESTAMP
            )
        """)
    
    def _create_config_tables(self, cursor):
        """创建配置表"""
        cursor.execute("""
            CREATE TABLE IF NOT EXISTS alert_configs (
                user_id TEXT PRIMARY KEY,
                depth_alert_enabled INTEGER DEFAULT 1,
                collision_alert_enabled INTEGER DEFAULT 1,
                weather_alert_enabled INTEGER DEFAULT 1,
                channel_alert_enabled INTEGER DEFAULT 1,
                deviation_alert_enabled INTEGER DEFAULT 1,
                speed_alert_enabled INTEGER DEFAULT 1,
                restricted_area_alert_enabled INTEGER DEFAULT 1,
                depth_threshold TEXT,
                collision_threshold TEXT,
                deviation_threshold TEXT,
                speed_threshold TEXT,
                push_methods TEXT,
                created_at TEXT DEFAULT CURRENT_TIMESTAMP,
                updated_at TEXT DEFAULT CURRENT_TIMESTAMP
            )
        """)
    
    def _insert_alert(self, cursor, data: Dict[str, Any]):
        """插入预警数据"""
        cursor.execute("""
            INSERT OR REPLACE INTO alerts 
            (alert_id, alert_type, alert_level, status, content, position,
             issue_time, expire_time, acknowledge_required, acknowledged,
             acknowledge_time, action_taken)
            VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
        """, (
            data['alert_id'], data['alert_type'], data['alert_level'],
            data['status'], data['content'], data['position'],
            data['issue_time'], data['expire_time'], data['acknowledge_required'],
            data['acknowledged'], data['acknowledge_time'], data['action_taken']
        ))
    
    def _insert_config(self, cursor, data: Dict[str, Any]):
        """插入配置数据"""
        cursor.execute("""
            INSERT OR REPLACE INTO alert_configs
            (user_id, depth_alert_enabled, collision_alert_enabled,
             weather_alert_enabled, channel_alert_enabled,
             depth_threshold, collision_threshold, push_methods)
            VALUES (?, ?, ?, ?, ?, ?, ?, ?)
        """, (
            data['user_id'], data['depth_alert_enabled'],
            data['collision_alert_enabled'], data['weather_alert_enabled'],
            data['channel_alert_enabled'], data['depth_threshold'],
            data['collision_threshold'], data['push_methods']
        ))
    
    def _print_summary(self):
        """打印迁移摘要"""
        print("\n" + "=" * 50)
        print("迁移完成摘要")
        print("=" * 50)
        print(f"预警数据迁移: {self.stats['alerts_migrated']} 条")
        print(f"配置数据迁移: {self.stats['configs_migrated']} 条")
        
        if self.stats['errors']:
            print(f"\n错误数量: {len(self.stats['errors'])}")
            for error in self.stats['errors'][:10]:
                print(f"  - {error}")
        else:
            print("\n无错误")
        
        print("=" * 50)


def main():
    """主函数"""
    import argparse
    
    parser = argparse.ArgumentParser(description='数据迁移脚本')
    parser.add_argument('--source', required=True, help='源数据库路径')
    parser.add_argument('--target', required=True, help='目标数据库路径')
    parser.add_argument('--dry-run', action='store_true', help='试运行模式')
    
    args = parser.parse_args()
    
    migrator = DataMigrator(args.source, args.target)
    
    if args.dry_run:
        print("试运行模式 - 不实际写入数据")
        print(f"源数据库: {args.source}")
        print(f"目标数据库: {args.target}")
    else:
        success = migrator.migrate_all()
        exit(0 if success else 1)


if __name__ == '__main__':
    main()
