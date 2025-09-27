#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
智能挖掘检测系统启动脚本
"""

import sys
import os

# 添加当前目录到路径
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

def check_dependencies():
    """检查依赖库"""
    required_modules = [
        'tkinter',
        'numpy', 
        'matplotlib',
        'scipy',
        'serial'
    ]
    
    missing_modules = []
    
    for module in required_modules:
        try:
            __import__(module)
            print(f"✓ {module}")
        except ImportError:
            missing_modules.append(module)
            print(f"✗ {module} (缺失)")
    
    if missing_modules:
        print(f"\n缺少以下依赖库: {', '.join(missing_modules)}")
        print("请安装缺失的库后再运行程序")
        return False
    
    return True

def main():
    """主函数"""
    print("智能挖掘检测系统")
    print("=" * 50)
    
    print("\n检查依赖库...")
    if not check_dependencies():
        return
    
    print("\n启动检测系统...")
    
    try:
        # 导入并启动主程序
        from vibration_analyzer_chinese import main as start_analyzer
        start_analyzer()
        
    except KeyboardInterrupt:
        print("\n程序被用户中断")
    except Exception as e:
        print(f"\n启动失败: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()
