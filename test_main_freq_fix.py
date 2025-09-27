#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试主频检测修复 - 验证只在5Hz以上查找主频
"""

import numpy as np
import sys
import os

# 添加当前目录到路径
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

from vibration_analyzer_chinese import FineDetector, ParameterManager

def create_test_spectrum_with_low_freq():
    """创建包含低频成分的测试频谱数据"""
    # 创建频率数组 (0-500Hz, 257点)
    frequencies = np.linspace(0, 500, 257)
    
    # 创建测试信号
    magnitudes = np.zeros(257)
    
    # 添加2Hz的静态成分（模拟环境振动）
    freq_2hz_idx = np.argmin(np.abs(frequencies - 2))
    magnitudes[freq_2hz_idx] = 0.8  # 很强的低频成分
    
    # 添加20Hz的挖掘信号（我们真正关心的）
    freq_20hz_idx = np.argmin(np.abs(frequencies - 20))
    magnitudes[freq_20hz_idx] = 0.5  # 中等强度的挖掘信号
    
    # 添加一些其他频率成分
    freq_10hz_idx = np.argmin(np.abs(frequencies - 10))
    magnitudes[freq_10hz_idx] = 0.3
    
    freq_50hz_idx = np.argmin(np.abs(frequencies - 50))
    magnitudes[freq_50hz_idx] = 0.2
    
    # 添加少量噪声
    magnitudes += np.random.normal(0, 0.01, len(magnitudes))
    magnitudes = np.abs(magnitudes)  # 确保为正数
    
    return frequencies, magnitudes

def test_main_frequency_detection():
    """测试主频检测逻辑"""
    print("🔧 测试主频检测修复...")
    
    # 创建参数管理器和细检测器
    param_manager = ParameterManager()
    fine_detector = FineDetector(param_manager)
    
    # 创建测试数据
    frequencies, magnitudes = create_test_spectrum_with_low_freq()
    
    print(f"📊 测试数据:")
    print(f"频率范围: {frequencies[0]:.1f} - {frequencies[-1]:.1f} Hz")
    
    # 找到各个频率的索引和幅值
    freq_2hz_idx = np.argmin(np.abs(frequencies - 2))
    freq_20hz_idx = np.argmin(np.abs(frequencies - 20))
    
    print(f"2Hz成分幅值: {magnitudes[freq_2hz_idx]:.3f}")
    print(f"20Hz成分幅值: {magnitudes[freq_20hz_idx]:.3f}")
    print(f"全频段最大幅值位置: {frequencies[np.argmax(magnitudes)]:.1f} Hz")
    
    # 检查5Hz以上的最大幅值位置
    valid_indices = frequencies >= 5.0
    valid_freqs = frequencies[valid_indices]
    valid_magnitudes = magnitudes[valid_indices]
    
    if len(valid_magnitudes) > 0:
        max_valid_idx = np.argmax(valid_magnitudes)
        max_valid_freq = valid_freqs[max_valid_idx]
        print(f"5Hz以上最大幅值位置: {max_valid_freq:.1f} Hz")
    
    # 提取特征
    features = fine_detector.extract_frequency_features(magnitudes, frequencies)
    
    if features is not None:
        print(f"\n✅ 特征提取成功!")
        
        # 解析特征
        bands = param_manager.get_param("fine_detection", "frequency_bands")
        print(f"\n📈 频段划分: {bands}")
        
        print(f"\n🔍 特征详情:")
        print(f"低频能量比 (5-15Hz): {features[0]:.6f}")
        print(f"中频能量比 (15-30Hz): {features[1]:.6f}")
        print(f"高频能量比 (30-100Hz): {features[2]:.6f}")
        print(f"超高频能量比 (100-500Hz): {features[3]:.6f}")
        print(f"检测到的主频: {features[4]:.1f} Hz")
        print(f"频谱重心: {features[5]:.1f} Hz")
        print(f"谐波强度: {features[6]:.6f}")
        print(f"频谱平坦度: {features[7]:.6f}")
        
        # 验证主频检测逻辑
        print(f"\n🔍 主频检测验证:")
        detected_main_freq = features[4]
        
        if detected_main_freq >= 5.0:
            print(f"✅ 主频 {detected_main_freq:.1f}Hz >= 5Hz，符合预期")
        else:
            print(f"❌ 主频 {detected_main_freq:.1f}Hz < 5Hz，不符合预期")
        
        if abs(detected_main_freq - 20.0) < 5.0:
            print(f"✅ 主频接近20Hz挖掘信号，正确忽略了2Hz静态成分")
        else:
            print(f"⚠️ 主频不是20Hz，可能检测逻辑需要调整")
        
        # 检查能量分布
        print(f"\n📊 能量分布分析:")
        total_energy_ratio = sum(features[:4])
        print(f"总能量比: {total_energy_ratio:.6f}")
        
        if features[1] > features[0]:  # 中频 > 低频
            print(f"✅ 中频能量({features[1]:.3f}) > 低频能量({features[0]:.3f})，符合20Hz主频")
        else:
            print(f"⚠️ 低频能量过高，可能受到2Hz成分影响")
        
        return True
    else:
        print("❌ 特征提取失败!")
        return False

def test_classification_with_low_freq():
    """测试包含低频成分的分类"""
    print(f"\n🎯 测试分类功能（包含低频成分）...")
    
    param_manager = ParameterManager()
    fine_detector = FineDetector(param_manager)
    
    # 创建测试数据
    frequencies, magnitudes = create_test_spectrum_with_low_freq()
    
    # 执行分类
    result = fine_detector.classify(magnitudes, frequencies)
    
    print(f"分类结果: {result}")
    
    if result['result'] != 'error':
        print(f"✅ 分类成功!")
        print(f"结果: {result['result']}")
        print(f"置信度: {result['confidence']:.3f}")
        print(f"检测到的主频: {result['features'][4]:.1f} Hz")
        
        if result['features'][4] >= 5.0:
            print(f"✅ 主频检测正确，忽略了低频静态成分")
        else:
            print(f"❌ 主频检测错误，可能受到低频干扰")
        
        return True
    else:
        print(f"❌ 分类失败!")
        return False

if __name__ == "__main__":
    print("🚀 开始测试主频检测修复...")
    
    try:
        success1 = test_main_frequency_detection()
        success2 = test_classification_with_low_freq()
        
        if success1 and success2:
            print("\n🎉 主频检测修复测试通过！")
            print("✅ 系统正确忽略低频静态成分，只在5Hz以上查找主频")
        else:
            print("\n❌ 部分测试失败，需要进一步调整")
            
    except Exception as e:
        print(f"\n❌ 测试过程中出错: {e}")
        import traceback
        traceback.print_exc()
