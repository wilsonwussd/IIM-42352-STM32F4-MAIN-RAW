#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
检测日志查看器 - 独立脚本
用于查看和分析检测历史记录
"""

import os
import sys
from datetime import datetime

def print_colored(text, color_code):
    """打印彩色文本"""
    print(f"\033[{color_code}m{text}\033[0m")

def analyze_log_file(filename="detection_history.log"):
    """分析日志文件"""
    if not os.path.exists(filename):
        print(f"❌ 日志文件 {filename} 不存在")
        return
    
    print("🔍 智能挖掘检测系统 - 日志分析报告")
    print("=" * 60)
    
    # 统计数据
    stats = {
        'total_lines': 0,
        'coarse_detections': 0,
        'coarse_triggered': 0,
        'fine_detections': 0,
        'mining_events': 0,
        'uncertain_events': 0,
        'normal_events': 0,
        'system_status': 0,
        'errors': 0
    }
    
    # 存储重要事件
    important_events = []
    
    try:
        with open(filename, 'r', encoding='utf-8') as f:
            lines = f.readlines()
            stats['total_lines'] = len(lines)
            
            for line in lines:
                line = line.strip()
                if not line:
                    continue
                
                # 分析不同类型的日志
                if "粗检测" in line:
                    stats['coarse_detections'] += 1
                    if "状态: 触发" in line:
                        stats['coarse_triggered'] += 1
                        important_events.append(('粗检测触发', line))
                
                elif "细检测" in line:
                    stats['fine_detections'] += 1
                    if "结果: mining" in line:
                        stats['mining_events'] += 1
                        important_events.append(('挖掘检测', line))
                    elif "结果: uncertain" in line:
                        stats['uncertain_events'] += 1
                        important_events.append(('不确定检测', line))
                    elif "结果: normal" in line:
                        stats['normal_events'] += 1
                
                elif "挖掘事件检测" in line:
                    important_events.append(('挖掘事件', line))
                
                elif "系统状态" in line:
                    stats['system_status'] += 1
                    important_events.append(('系统状态', line))
                
                elif "ERROR" in line or "失败" in line:
                    stats['errors'] += 1
                    important_events.append(('错误', line))
    
    except Exception as e:
        print(f"❌ 读取日志文件失败: {e}")
        return
    
    # 打印统计报告
    print("\n📊 统计摘要:")
    print(f"  总日志条数: {stats['total_lines']}")
    print(f"  粗检测次数: {stats['coarse_detections']} (触发: {stats['coarse_triggered']})")
    print(f"  细检测次数: {stats['fine_detections']}")
    print(f"    - 挖掘事件: {stats['mining_events']}")
    print(f"    - 不确定: {stats['uncertain_events']}")
    print(f"    - 正常: {stats['normal_events']}")
    print(f"  系统状态记录: {stats['system_status']}")
    print(f"  错误记录: {stats['errors']}")
    
    # 计算检测率
    if stats['fine_detections'] > 0:
        mining_rate = (stats['mining_events'] / stats['fine_detections']) * 100
        print(f"  挖掘检出率: {mining_rate:.1f}%")
    
    # 显示重要事件
    if important_events:
        print("\n🚨 重要事件记录:")
        print("-" * 60)
        
        # 按类型分组显示
        event_types = ['挖掘检测', '挖掘事件', '粗检测触发', '不确定检测', '系统状态', '错误']
        
        for event_type in event_types:
            type_events = [e for e in important_events if e[0] == event_type]
            if type_events:
                if event_type == '挖掘检测':
                    print_colored(f"\n🔴 {event_type} ({len(type_events)}条):", "91")  # 红色
                elif event_type == '粗检测触发':
                    print_colored(f"\n🟠 {event_type} ({len(type_events)}条):", "93")  # 黄色
                elif event_type == '不确定检测':
                    print_colored(f"\n🟡 {event_type} ({len(type_events)}条):", "93")  # 黄色
                elif event_type == '错误':
                    print_colored(f"\n❌ {event_type} ({len(type_events)}条):", "91")  # 红色
                else:
                    print(f"\n📋 {event_type} ({len(type_events)}条):")
                
                # 显示最近的几条记录
                for i, (_, event_line) in enumerate(type_events[-5:]):  # 最近5条
                    print(f"  {i+1}. {event_line}")
                
                if len(type_events) > 5:
                    print(f"  ... 还有 {len(type_events) - 5} 条记录")
    
    print("\n" + "=" * 60)
    print("💡 提示: 运行主程序时可以点击'查看日志'按钮查看完整的实时日志")

def show_recent_logs(filename="detection_history.log", lines=20):
    """显示最近的日志"""
    if not os.path.exists(filename):
        print(f"❌ 日志文件 {filename} 不存在")
        return
    
    print(f"\n📋 最近 {lines} 条日志记录:")
    print("-" * 60)
    
    try:
        with open(filename, 'r', encoding='utf-8') as f:
            all_lines = f.readlines()
            recent_lines = all_lines[-lines:] if len(all_lines) > lines else all_lines
            
            for line in recent_lines:
                line = line.strip()
                if not line:
                    continue
                
                # 根据日志级别着色
                if "ERROR" in line:
                    print_colored(line, "91")  # 红色
                elif "WARNING" in line:
                    print_colored(line, "93")  # 黄色
                elif "CRITICAL" in line:
                    print_colored(line, "95")  # 紫色
                elif "mining" in line:
                    print_colored(line, "91")  # 红色
                elif "uncertain" in line:
                    print_colored(line, "93")  # 黄色
                else:
                    print(line)
    
    except Exception as e:
        print(f"❌ 读取日志文件失败: {e}")

def main():
    """主函数"""
    if len(sys.argv) > 1:
        if sys.argv[1] == "--recent":
            lines = int(sys.argv[2]) if len(sys.argv) > 2 else 20
            show_recent_logs(lines=lines)
        elif sys.argv[1] == "--help":
            print("检测日志查看器使用说明:")
            print("  python view_detection_logs.py          # 显示完整分析报告")
            print("  python view_detection_logs.py --recent [N]  # 显示最近N条日志")
            print("  python view_detection_logs.py --help   # 显示帮助信息")
        else:
            print("未知参数，使用 --help 查看帮助")
    else:
        # 默认显示分析报告
        analyze_log_file()
        show_recent_logs(lines=10)

if __name__ == "__main__":
    main()
