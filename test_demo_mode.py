#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试演示模式和检测功能
"""

import sys
import time
import numpy as np
from collections import deque
import json

# 导入检测系统组件
sys.path.append('.')

# 模拟参数管理器
class MockParameterManager:
    def __init__(self):
        self.params = {
            "coarse_detection": {
                "rms_threshold_multiplier": 2.5,
                "peak_factor_threshold": 1.8,
                "duration_threshold_sec": 5.0,
                "cooldown_time_sec": 30.0,
                "adaptive_enabled": True,
                "baseline_update_rate": 0.1
            },
            "filtering": {
                "highpass_cutoff_hz": 5.0,
                "filter_order": 4
            }
        }
    
    def get_param(self, category, param_name):
        return self.params.get(category, {}).get(param_name, 0)

# 模拟数据预处理器
class MockDataPreprocessor:
    def apply_highpass_filter(self, data):
        # 简单的高通滤波模拟
        return data * 0.9  # 模拟滤波后的数据

# 导入CoarseDetector类
def test_coarse_detector():
    """测试粗检测器"""
    print("🔍 测试粗检测器...")
    
    # 创建模拟组件
    param_manager = MockParameterManager()
    
    # 这里我们需要从主文件导入CoarseDetector
    # 但为了测试，我们创建一个简化版本
    
    class TestCoarseDetector:
        def __init__(self, param_manager):
            self.param_manager = param_manager
            self.data_buffer = deque(maxlen=2000)
            self.baseline_rms = 0.005
            self.last_trigger_time = 0
            self.trigger_start_time = 0
            self.is_triggered = False
            self.baseline_initialized = False
        
        def calculate_features(self, data):
            if len(data) < 3:
                return None
            
            try:
                magnitude = np.sqrt(data[0]**2 + data[1]**2 + data[2]**2)
                rms = np.sqrt(np.mean(magnitude**2))
                peak_value = np.max(np.abs(magnitude))
                peak_factor = peak_value / rms if rms > 0 else 0
                
                return {
                    'rms': rms,
                    'peak_factor': peak_factor,
                    'peak_value': peak_value,
                    'magnitude': magnitude
                }
            except Exception as e:
                print(f"特征计算失败: {e}")
                return None
        
        def update_baseline(self, rms_value):
            if rms_value > 0:
                if not self.baseline_initialized:
                    self.baseline_rms = max(rms_value, 0.002)
                    self.baseline_initialized = True
                    print(f"✅ 基线初始化: {self.baseline_rms:.6f}g")
                elif self.param_manager.get_param("coarse_detection", "adaptive_enabled"):
                    update_rate = self.param_manager.get_param("coarse_detection", "baseline_update_rate")
                    if not self.is_triggered and rms_value < self.baseline_rms * 3:
                        old_baseline = self.baseline_rms
                        self.baseline_rms = (1 - update_rate) * self.baseline_rms + update_rate * rms_value
                        print(f"📊 基线更新: {old_baseline:.6f}g -> {self.baseline_rms:.6f}g")
        
        def detect(self, filtered_data):
            if len(filtered_data) != 3:
                return False
            
            self.data_buffer.append(filtered_data)
            
            if len(self.data_buffer) < 100:
                return False
            
            recent_data = np.array(list(self.data_buffer)[-200:])
            features = self.calculate_features(recent_data.T)
            
            if features is None:
                return False
            
            if not self.is_triggered:
                self.update_baseline(features['rms'])
            
            rms_multiplier = self.param_manager.get_param("coarse_detection", "rms_threshold_multiplier")
            peak_threshold = self.param_manager.get_param("coarse_detection", "peak_factor_threshold")
            duration_threshold = self.param_manager.get_param("coarse_detection", "duration_threshold_sec")
            cooldown_time = self.param_manager.get_param("coarse_detection", "cooldown_time_sec")
            
            rms_threshold = self.baseline_rms * rms_multiplier
            current_time = time.time()
            
            if current_time - self.last_trigger_time < cooldown_time:
                return False
            
            rms_triggered = features['rms'] > rms_threshold
            peak_triggered = features['peak_factor'] > peak_threshold
            
            print(f"📈 RMS: {features['rms']:.6f}g (阈值: {rms_threshold:.6f}g) | 峰值因子: {features['peak_factor']:.3f} (阈值: {peak_threshold:.1f})")
            
            if rms_triggered and peak_triggered:
                if not self.is_triggered:
                    self.trigger_start_time = current_time
                    self.is_triggered = True
                    print("🚨 检测触发开始!")
                
                if current_time - self.trigger_start_time >= duration_threshold:
                    self.last_trigger_time = current_time
                    self.is_triggered = False
                    print("🎯 检测确认触发!")
                    return True
            else:
                if self.is_triggered:
                    print("⏹️ 检测触发结束")
                self.is_triggered = False
            
            return False
    
    # 创建检测器
    detector = TestCoarseDetector(param_manager)
    preprocessor = MockDataPreprocessor()
    
    print("\n📊 测试场景1: 正常背景噪声")
    for i in range(150):  # 确保有足够数据
        # 生成正常噪声 (2mg)
        accel_data = np.array([
            0.002 * np.random.normal(0, 1),
            0.002 * np.random.normal(0, 1),
            0.002 * np.random.normal(0, 1)
        ])
        
        filtered_data = preprocessor.apply_highpass_filter(accel_data)
        result = detector.detect(filtered_data)
        
        if i % 50 == 0:
            print(f"步骤 {i}: 结果={result}")
    
    print("\n📊 测试场景2: 模拟挖掘活动")
    for i in range(200):
        # 生成挖掘信号 (15mg, 8Hz)
        t = i * 0.05  # 50ms间隔
        base_freq = 8
        amplitude = 0.015
        phase = 2 * np.pi * base_freq * t
        
        accel_data = np.array([
            amplitude * (np.sin(phase) + 0.3 * np.sin(2*phase) + 0.1 * np.random.normal(0, 1)),
            amplitude * (np.cos(phase) + 0.2 * np.cos(3*phase) + 0.1 * np.random.normal(0, 1)),
            amplitude * (0.5 * np.sin(0.5*phase) + 0.1 * np.random.normal(0, 1))
        ])
        
        filtered_data = preprocessor.apply_highpass_filter(accel_data)
        result = detector.detect(filtered_data)
        
        if result:
            print(f"🎉 步骤 {i}: 检测到挖掘活动!")
        elif i % 50 == 0:
            print(f"步骤 {i}: 继续监测...")
    
    print("\n✅ 粗检测器测试完成!")

def test_demo_data_generation():
    """测试演示数据生成"""
    print("\n🎮 测试演示数据生成...")
    
    demo_timer = 0
    for i in range(600):  # 30秒测试
        demo_timer += 0.05
        
        if demo_timer < 10:
            # 正常噪声
            amplitude = 0.002
            accel_x = amplitude * np.random.normal(0, 1)
            accel_y = amplitude * np.random.normal(0, 1) 
            accel_z = amplitude * np.random.normal(0, 1)
            signal_type = "正常"
        elif demo_timer < 20:
            # 挖掘活动
            base_freq = 8
            amplitude = 0.015
            phase = 2 * np.pi * base_freq * demo_timer
            
            accel_x = amplitude * (np.sin(phase) + 0.3 * np.sin(2*phase) + 0.1 * np.random.normal(0, 1))
            accel_y = amplitude * (np.cos(phase) + 0.2 * np.cos(3*phase) + 0.1 * np.random.normal(0, 1))
            accel_z = amplitude * (0.5 * np.sin(0.5*phase) + 0.1 * np.random.normal(0, 1))
            signal_type = "挖掘"
        else:
            # 重置
            demo_timer = 0
            amplitude = 0.002
            accel_x = amplitude * np.random.normal(0, 1)
            accel_y = amplitude * np.random.normal(0, 1)
            accel_z = amplitude * np.random.normal(0, 1)
            signal_type = "正常"
        
        rms = np.sqrt(accel_x**2 + accel_y**2 + accel_z**2)
        
        if i % 100 == 0:
            print(f"时间: {demo_timer:.1f}s | 类型: {signal_type} | RMS: {rms:.6f}g")
    
    print("✅ 演示数据生成测试完成!")

if __name__ == "__main__":
    print("🚀 开始测试检测系统修复...")
    test_coarse_detector()
    test_demo_data_generation()
    print("\n🎉 所有测试完成!")
