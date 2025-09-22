#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试双视图界面的脚本
"""

import tkinter as tk
from vibration_analyzer_chinese import VibrAnalyzer
import numpy as np
import time
import threading

def simulate_data(analyzer):
    """模拟数据生成，用于测试界面"""
    print("开始模拟数据生成...")
    
    # 模拟频域数据
    frequencies = np.linspace(0, 500, 257)
    
    # 模拟原始数据
    sample_count = 0
    
    while True:
        try:
            # 模拟频域数据 (每500ms更新一次)
            if sample_count % 50 == 0:
                # 生成模拟频谱数据
                magnitudes = np.random.exponential(0.01, 257)
                # 在某些频率添加峰值
                peak_freqs = [50, 100, 200]
                for freq in peak_freqs:
                    idx = int(freq * 257 / 500)
                    if idx < 257:
                        magnitudes[idx] += np.random.uniform(0.05, 0.2)
                
                # 模拟频域数据帧
                freq_frame = {
                    'type': 'spectrum',
                    'timestamp': int(time.time() * 1000),
                    'magnitudes': magnitudes.tolist()
                }
                
                # 添加到数据队列
                if hasattr(analyzer, 'data_queue'):
                    analyzer.data_queue.put(freq_frame)
            
            # 模拟原始数据 (每100ms更新一次)
            if sample_count % 10 == 0:
                # 生成模拟三轴加速度数据
                t = time.time()
                accel_x = 0.01 * np.sin(2 * np.pi * 10 * t) + np.random.normal(0, 0.005)
                accel_y = 0.02 * np.sin(2 * np.pi * 15 * t) + np.random.normal(0, 0.005)
                accel_z = 1.0 + 0.05 * np.sin(2 * np.pi * 5 * t) + np.random.normal(0, 0.01)
                
                # 模拟原始数据帧
                raw_frame = {
                    'type': 'raw_accel',
                    'timestamp': int(t * 1000),
                    'accel_x': accel_x,
                    'accel_y': accel_y,
                    'accel_z': accel_z
                }
                
                # 添加到数据队列
                if hasattr(analyzer, 'data_queue'):
                    analyzer.data_queue.put(raw_frame)
            
            sample_count += 1
            time.sleep(0.01)  # 100Hz模拟采样率
            
        except Exception as e:
            print(f"模拟数据生成错误: {e}")
            break

def main():
    """主函数"""
    print("启动双视图界面测试...")
    
    # 创建主窗口
    root = tk.Tk()
    
    # 创建分析器实例
    analyzer = VibrAnalyzer(root)
    
    # 启动模拟数据线程
    data_thread = threading.Thread(target=simulate_data, args=(analyzer,), daemon=True)
    data_thread.start()
    
    # 模拟连接状态
    analyzer.is_connected = True
    analyzer.status_label.config(text="已连接 (模拟)", foreground="green")
    analyzer.connect_btn.config(text="断开")
    
    # 启用原始数据显示
    analyzer.show_raw_data.set(True)
    
    print("界面已启动，正在显示模拟数据...")
    print("- 上方图表: 频域分析 (振动频谱)")
    print("- 下方图表: 时域分析 (三轴加速度)")
    print("- 数据更新: 频域 2Hz, 原始数据 10Hz")
    print("- 关闭窗口退出测试")
    
    # 启动GUI主循环
    root.mainloop()

if __name__ == '__main__':
    main()
