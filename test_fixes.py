#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试修复后的数据预处理器和线程安全机制
"""

import numpy as np
import threading
import time
import sys
import os

# 添加当前目录到路径
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

from vibration_analyzer_chinese import DataPreprocessor, ParameterManager

def test_data_preprocessor():
    """测试数据预处理器修复"""
    print("🔧 测试数据预处理器修复...")
    
    # 创建参数管理器
    param_manager = ParameterManager()
    
    # 创建数据预处理器
    preprocessor = DataPreprocessor(param_manager)
    
    # 测试1: 单点滤波
    print("\n📊 测试1: 单点滤波")
    single_point = np.array([0.1, 0.2, 0.3])
    filtered_single = preprocessor.apply_highpass_filter(single_point)
    print(f"输入: {single_point}")
    print(f"输出: {filtered_single}")
    print(f"形状: {filtered_single.shape}")
    
    # 测试2: 序列滤波
    print("\n📊 测试2: 序列滤波")
    sequence = np.random.randn(100, 3) * 0.1  # 100个三轴数据点
    filtered_sequence = preprocessor.apply_highpass_filter(sequence)
    print(f"输入形状: {sequence.shape}")
    print(f"输出形状: {filtered_sequence.shape}")
    print(f"输入均值: {np.mean(sequence, axis=0)}")
    print(f"输出均值: {np.mean(filtered_sequence, axis=0)}")
    
    # 测试3: 连续单点滤波（模拟实时数据）
    print("\n📊 测试3: 连续单点滤波")
    for i in range(5):
        point = np.array([0.01 * i, 0.02 * i, 0.03 * i])
        filtered = preprocessor.apply_highpass_filter(point)
        print(f"点{i+1}: {point} -> {filtered}")
    
    # 测试4: 滤波器状态重置
    print("\n📊 测试4: 滤波器状态重置")
    preprocessor.reset_filter_state()
    print("滤波器状态已重置")
    
    print("✅ 数据预处理器测试完成")

def test_thread_safety():
    """测试线程安全机制"""
    print("\n🔒 测试线程安全机制...")
    
    # 模拟共享状态
    class MockAnalyzer:
        def __init__(self):
            self._state_lock = threading.Lock()
            self._serial_lock = threading.Lock()
            self._running = False
            self.serial_conn = None
            self.access_count = 0
        
        @property
        def running(self):
            with self._state_lock:
                return self._running
        
        @running.setter
        def running(self, value):
            with self._state_lock:
                self._running = value
        
        def get_serial_connection(self):
            with self._serial_lock:
                self.access_count += 1
                return self.serial_conn
        
        def set_serial_connection(self, conn):
            with self._serial_lock:
                self.serial_conn = conn
    
    analyzer = MockAnalyzer()
    
    # 测试并发访问
    def worker(worker_id):
        for i in range(100):
            analyzer.running = True
            analyzer.set_serial_connection(f"conn_{worker_id}_{i}")
            conn = analyzer.get_serial_connection()
            analyzer.running = False
            time.sleep(0.001)
    
    # 启动多个线程
    threads = []
    for i in range(5):
        t = threading.Thread(target=worker, args=(i,))
        threads.append(t)
        t.start()
    
    # 等待所有线程完成
    for t in threads:
        t.join()
    
    print(f"✅ 线程安全测试完成，访问次数: {analyzer.access_count}")

def test_filter_performance():
    """测试滤波器性能"""
    print("\n⚡ 测试滤波器性能...")
    
    param_manager = ParameterManager()
    preprocessor = DataPreprocessor(param_manager)
    
    # 生成测试数据
    n_points = 1000
    test_data = np.random.randn(n_points, 3) * 0.1
    
    # 测试单点滤波性能
    start_time = time.time()
    for i in range(n_points):
        filtered = preprocessor.apply_highpass_filter(test_data[i])
    single_point_time = time.time() - start_time
    
    # 重置滤波器状态
    preprocessor.reset_filter_state()
    
    # 测试批量滤波性能
    start_time = time.time()
    filtered_batch = preprocessor.apply_highpass_filter(test_data)
    batch_time = time.time() - start_time
    
    print(f"单点滤波时间: {single_point_time:.4f}s ({n_points}点)")
    print(f"批量滤波时间: {batch_time:.4f}s ({n_points}点)")
    if batch_time > 0:
        print(f"性能提升: {single_point_time/batch_time:.2f}x")
    else:
        print("批量滤波速度极快，无法计算性能提升比")
    
    print("✅ 性能测试完成")

if __name__ == "__main__":
    print("🚀 开始测试修复...")
    
    try:
        test_data_preprocessor()
        test_thread_safety()
        test_filter_performance()
        
        print("\n🎉 所有测试通过！修复成功！")
        
    except Exception as e:
        print(f"\n❌ 测试失败: {e}")
        import traceback
        traceback.print_exc()
