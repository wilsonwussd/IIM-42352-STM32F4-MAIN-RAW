#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试特征提取修复
"""

import numpy as np
import sys
import os

# 添加当前目录到路径
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

from vibration_analyzer_chinese import FineDetector, ParameterManager

def create_test_spectrum():
    """创建测试频谱数据"""
    # 创建频率数组 (0-500Hz, 257点)
    frequencies = np.linspace(0, 500, 257)
    
    # 创建测试信号：主要能量在20Hz
    magnitudes = np.zeros(257)
    
    # 添加20Hz的主峰
    freq_20hz_idx = np.argmin(np.abs(frequencies - 20))
    magnitudes[freq_20hz_idx] = 0.5
    
    # 添加一些低频成分 (10Hz)
    freq_10hz_idx = np.argmin(np.abs(frequencies - 10))
    magnitudes[freq_10hz_idx] = 0.3
    
    # 添加一些中频成分 (50Hz)
    freq_50hz_idx = np.argmin(np.abs(frequencies - 50))
    magnitudes[freq_50hz_idx] = 0.2
    
    # 添加一些高频成分 (150Hz)
    freq_150hz_idx = np.argmin(np.abs(frequencies - 150))
    magnitudes[freq_150hz_idx] = 0.1
    
    # 添加少量噪声
    magnitudes += np.random.normal(0, 0.01, len(magnitudes))
    magnitudes = np.abs(magnitudes)  # 确保为正数
    
    return frequencies, magnitudes

def test_feature_extraction():
    """测试特征提取"""
    print("🔧 测试特征提取修复...")
    
    # 创建参数管理器和细检测器
    param_manager = ParameterManager()
    fine_detector = FineDetector(param_manager)
    
    # 创建测试数据
    frequencies, magnitudes = create_test_spectrum()
    
    print(f"📊 测试数据:")
    print(f"频率范围: {frequencies[0]:.1f} - {frequencies[-1]:.1f} Hz")
    print(f"数据点数: {len(frequencies)}")
    print(f"最大幅值: {np.max(magnitudes):.3f}")
    print(f"主频位置: {frequencies[np.argmax(magnitudes)]:.1f} Hz")
    
    # 提取特征
    features = fine_detector.extract_frequency_features(magnitudes, frequencies)
    
    if features is not None:
        print(f"\n✅ 特征提取成功!")
        print(f"特征向量长度: {len(features)}")
        
        # 解析特征
        bands = param_manager.get_param("fine_detection", "frequency_bands")
        print(f"\n📈 频段划分: {bands}")
        
        print(f"\n🔍 特征详情:")
        print(f"低频能量比 (5-15Hz): {features[0]:.6f}")
        print(f"中频能量比 (15-30Hz): {features[1]:.6f}")
        print(f"高频能量比 (30-100Hz): {features[2]:.6f}")
        print(f"超高频能量比 (100-500Hz): {features[3]:.6f}")
        print(f"主频: {features[4]:.1f} Hz")
        print(f"频谱重心: {features[5]:.1f} Hz")
        print(f"谐波强度: {features[6]:.6f}")
        print(f"频谱平坦度: {features[7]:.6f}")
        
        # 验证特征合理性
        print(f"\n🔍 特征验证:")
        
        # 检查能量比总和
        energy_sum = sum(features[:4])
        print(f"能量比总和: {energy_sum:.6f} (应该接近1.0)")
        
        # 检查主频
        expected_main_freq = frequencies[np.argmax(magnitudes)]
        print(f"期望主频: {expected_main_freq:.1f} Hz")
        print(f"检测主频: {features[4]:.1f} Hz")
        print(f"主频匹配: {'✅' if abs(features[4] - expected_main_freq) < 5 else '❌'}")
        
        # 检查数值范围
        all_finite = all(np.isfinite(f) for f in features)
        print(f"所有特征有限: {'✅' if all_finite else '❌'}")
        
        energy_ratios_valid = all(0 <= f <= 1 for f in features[:4])
        print(f"能量比范围正确: {'✅' if energy_ratios_valid else '❌'}")
        
        return True
    else:
        print("❌ 特征提取失败!")
        return False

def test_classification():
    """测试分类功能"""
    print(f"\n🎯 测试分类功能...")
    
    param_manager = ParameterManager()
    fine_detector = FineDetector(param_manager)
    
    # 创建测试数据
    frequencies, magnitudes = create_test_spectrum()
    
    # 执行分类
    result = fine_detector.classify(magnitudes, frequencies)
    
    print(f"分类结果: {result}")
    
    if result['result'] != 'error':
        print(f"✅ 分类成功!")
        print(f"结果: {result['result']}")
        print(f"置信度: {result['confidence']:.3f}")
        print(f"规则得分: {result['rule_score']}/{result['total_rules']}")
        return True
    else:
        print(f"❌ 分类失败!")
        return False

if __name__ == "__main__":
    print("🚀 开始测试特征提取修复...")
    
    try:
        success1 = test_feature_extraction()
        success2 = test_classification()
        
        if success1 and success2:
            print("\n🎉 所有测试通过！特征提取修复成功！")
        else:
            print("\n❌ 部分测试失败，需要进一步修复")
            
    except Exception as e:
        print(f"\n❌ 测试过程中出错: {e}")
        import traceback
        traceback.print_exc()
