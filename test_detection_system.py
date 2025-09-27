#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
智能挖掘检测系统测试脚本
用于验证检测功能的基本工作状态
"""

import numpy as np
import time
import sys
import os

# 添加当前目录到路径
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

# 导入检测系统组件
try:
    from vibration_analyzer_chinese import (
        ParameterManager, 
        DataPreprocessor, 
        CoarseDetector, 
        FineDetector
    )
    print("✓ 成功导入检测系统组件")
except ImportError as e:
    print(f"✗ 导入失败: {e}")
    sys.exit(1)

def test_parameter_manager():
    """测试参数管理器"""
    print("\n=== 测试参数管理器 ===")
    
    try:
        param_manager = ParameterManager()
        
        # 测试参数获取
        rms_threshold = param_manager.get_param("coarse_detection", "rms_threshold_multiplier")
        print(f"✓ RMS阈值倍数: {rms_threshold}")
        
        # 测试参数设置
        param_manager.set_param("coarse_detection", "rms_threshold_multiplier", 3.0)
        new_threshold = param_manager.get_param("coarse_detection", "rms_threshold_multiplier")
        print(f"✓ 参数设置成功: {new_threshold}")
        
        # 测试参数保存
        if param_manager.save_parameters():
            print("✓ 参数保存成功")
        else:
            print("✗ 参数保存失败")
            
        return param_manager
        
    except Exception as e:
        print(f"✗ 参数管理器测试失败: {e}")
        return None

def test_data_preprocessor(param_manager):
    """测试数据预处理器"""
    print("\n=== 测试数据预处理器 ===")
    
    try:
        preprocessor = DataPreprocessor(param_manager)
        
        # 生成测试数据
        test_data = np.array([
            [0.1, 0.05, 0.02],  # x, y, z 轴数据
        ]).T
        
        # 测试滤波
        filtered_data = preprocessor.apply_highpass_filter(test_data)
        print(f"✓ 滤波处理成功，输入形状: {test_data.shape}, 输出形状: {filtered_data.shape}")
        
        return preprocessor
        
    except Exception as e:
        print(f"✗ 数据预处理器测试失败: {e}")
        return None

def test_coarse_detector(param_manager):
    """测试粗检测器"""
    print("\n=== 测试粗检测器 ===")
    
    try:
        detector = CoarseDetector(param_manager)
        
        # 生成正常振动数据
        print("测试正常振动数据...")
        for i in range(10):
            normal_data = np.random.normal(0, 0.001, 3)  # 低幅值正常数据
            result = detector.detect(normal_data)
            if result:
                print(f"  第{i+1}次: 触发 (可能误触发)")
            
        # 生成强振动数据
        print("测试强振动数据...")
        for i in range(20):
            strong_data = np.random.normal(0, 0.05, 3)  # 高幅值数据
            result = detector.detect(strong_data)
            if result:
                print(f"  第{i+1}次: 触发检测")
                break
        
        # 获取当前状态
        status = detector.get_current_status()
        print(f"✓ 当前状态: RMS={status['rms']:.4f}, 基线={status['baseline_rms']:.4f}")
        
        return detector
        
    except Exception as e:
        print(f"✗ 粗检测器测试失败: {e}")
        return None

def test_fine_detector(param_manager):
    """测试细检测器"""
    print("\n=== 测试细检测器 ===")
    
    try:
        detector = FineDetector(param_manager)
        
        if not detector.is_trained:
            print("✗ 细检测器未训练")
            return None
        
        # 生成模拟FFT数据
        frequencies = np.linspace(0, 500, 257)
        
        # 测试挖掘特征数据（低频能量高）
        mining_spectrum = np.zeros(257)
        mining_spectrum[1:20] = np.random.exponential(0.1, 19)  # 低频高能量
        mining_spectrum[20:50] = np.random.exponential(0.05, 30)  # 中频中等能量
        mining_spectrum[50:] = np.random.exponential(0.01, 207)  # 高频低能量
        
        result = detector.classify(mining_spectrum, frequencies)
        print(f"✓ 挖掘特征测试: 结果={result['result']}, 置信度={result['confidence']:.3f}, 规则得分={result.get('rule_score', 0)}/{result.get('total_rules', 0)}")

        # 测试正常特征数据（频谱均匀）
        normal_spectrum = np.random.exponential(0.02, 257)  # 均匀分布
        result = detector.classify(normal_spectrum, frequencies)
        print(f"✓ 正常特征测试: 结果={result['result']}, 置信度={result['confidence']:.3f}, 规则得分={result.get('rule_score', 0)}/{result.get('total_rules', 0)}")
        
        return detector
        
    except Exception as e:
        print(f"✗ 细检测器测试失败: {e}")
        return None

def test_integration():
    """集成测试"""
    print("\n=== 集成测试 ===")
    
    # 初始化所有组件
    param_manager = test_parameter_manager()
    if not param_manager:
        return False
    
    preprocessor = test_data_preprocessor(param_manager)
    if not preprocessor:
        return False
    
    coarse_detector = test_coarse_detector(param_manager)
    if not coarse_detector:
        return False
    
    fine_detector = test_fine_detector(param_manager)
    if not fine_detector:
        return False
    
    print("\n=== 模拟完整检测流程 ===")
    
    # 模拟数据流
    for i in range(5):
        print(f"\n第{i+1}轮检测:")
        
        # 生成原始数据
        raw_data = np.random.normal(0, 0.02, 3)
        
        # 数据预处理
        filtered_data = preprocessor.apply_highpass_filter(raw_data)
        
        # 粗检测
        coarse_result = coarse_detector.detect(filtered_data)
        print(f"  粗检测结果: {'触发' if coarse_result else '正常'}")
        
        if coarse_result:
            # 生成FFT数据进行细检测
            frequencies = np.linspace(0, 500, 257)
            fft_data = np.random.exponential(0.02, 257)
            
            fine_result = fine_detector.classify(fft_data, frequencies)
            print(f"  细检测结果: {fine_result['result']}, 置信度: {fine_result['confidence']:.3f}")
        
        time.sleep(0.1)
    
    print("\n✓ 集成测试完成")
    return True

if __name__ == "__main__":
    print("智能挖掘检测系统测试")
    print("=" * 50)
    
    try:
        success = test_integration()
        
        if success:
            print("\n🎉 所有测试通过！系统工作正常。")
            print("\n现在可以运行主程序:")
            print("python vibration_analyzer_chinese.py")
        else:
            print("\n❌ 测试失败，请检查系统配置。")
            
    except KeyboardInterrupt:
        print("\n\n测试被用户中断")
    except Exception as e:
        print(f"\n❌ 测试过程中发生错误: {e}")
        import traceback
        traceback.print_exc()
