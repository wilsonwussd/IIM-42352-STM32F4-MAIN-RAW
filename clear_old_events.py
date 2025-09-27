#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
清除旧的事件数据，重新开始测试
"""

import os
import json
import sys

def clear_old_events():
    """清除旧的事件数据文件"""
    files_to_clear = [
        "mining_events.json",
        "detection_log.txt"
    ]
    
    cleared_files = []
    
    for filename in files_to_clear:
        if os.path.exists(filename):
            try:
                os.remove(filename)
                cleared_files.append(filename)
                print(f"✅ 已删除: {filename}")
            except Exception as e:
                print(f"❌ 删除失败 {filename}: {e}")
        else:
            print(f"ℹ️ 文件不存在: {filename}")
    
    if cleared_files:
        print(f"\n🎉 成功清除 {len(cleared_files)} 个旧数据文件")
        print("现在可以重新运行程序，获得干净的检测结果")
    else:
        print("\n📝 没有找到需要清除的旧数据文件")

def create_test_event():
    """创建一个测试事件来验证显示格式"""
    test_event = {
        'timestamp': 1695800000.0,
        'datetime': '2025-09-27 11:20:00',
        'detection_type': 'fine_detection',
        'confidence': 0.750,  # 正常的置信度
        'intensity': '中等',
        'sensor_data': {
            'raw_accel': [],
            'rms': 0.0125,  # 正常的RMS值 (g)
            'peak_factor': 2.35,  # 正常的峰值因子
            'baseline_rms': 0.005
        },
        'frequency_analysis': {
            'dominant_freq': 18.5,
            'spectral_centroid': 45.2,
            'low_freq_energy': 0.35,
            'mid_freq_energy': 0.45,
            'high_freq_energy': 0.15,
            'ultra_high_freq_energy': 0.05
        },
        'location': '传感器位置',
        'duration': 2.5,
        'notes': '测试事件 - 验证显示格式'
    }
    
    # 保存测试事件
    with open("mining_events.json", "w", encoding="utf-8") as f:
        json.dump([test_event], f, ensure_ascii=False, indent=2)
    
    print("✅ 已创建测试事件文件")
    print("测试事件数据:")
    print(f"  置信度: {test_event['confidence']:.3f}")
    print(f"  RMS: {test_event['sensor_data']['rms']:.4f}g")
    print(f"  峰值因子: {test_event['sensor_data']['peak_factor']:.2f}")
    print(f"  主频: {test_event['frequency_analysis']['dominant_freq']:.1f}Hz")

if __name__ == "__main__":
    print("🧹 清除旧事件数据工具")
    print("=" * 40)
    
    if len(sys.argv) > 1 and sys.argv[1] == "--test":
        print("📝 创建测试事件...")
        create_test_event()
    else:
        print("🗑️ 清除旧事件数据...")
        clear_old_events()
        
        print("\n" + "=" * 40)
        print("💡 使用说明:")
        print("1. 运行 'python clear_old_events.py' 清除旧数据")
        print("2. 运行 'python clear_old_events.py --test' 创建测试事件")
        print("3. 重新启动主程序测试新的检测结果")
