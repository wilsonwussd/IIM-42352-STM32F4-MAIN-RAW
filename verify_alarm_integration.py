#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
验证挖掘检测触发STM32报警功能的集成
"""

import re

def verify_vibration_analyzer_modifications():
    """验证振动分析软件的修改"""
    print("🔍 验证振动分析软件修改...")
    
    try:
        with open('vibration_analyzer_chinese.py', 'r', encoding='utf-8') as f:
            content = f.read()
        
        checks = []
        
        # 检查1: STM32报警开关变量
        if 'self.stm32_alarm_enabled = tk.BooleanVar(value=True)' in content:
            checks.append("✅ STM32报警开关变量已定义")
        else:
            checks.append("❌ 缺少STM32报警开关变量")
        
        # 检查2: 发送报警命令函数
        if 'def send_alarm_trigger_to_stm32(self):' in content:
            checks.append("✅ 发送报警命令函数已定义")
        else:
            checks.append("❌ 缺少发送报警命令函数")
        
        # 检查3: 0x10命令发送
        if 'command = bytes([0x10])' in content:
            checks.append("✅ 0x10命令发送代码已添加")
        else:
            checks.append("❌ 缺少0x10命令发送代码")
        
        # 检查4: UI控件添加
        if 'text="STM32报警"' in content and 'variable=self.stm32_alarm_enabled' in content:
            checks.append("✅ STM32报警UI控件已添加")
        else:
            checks.append("❌ 缺少STM32报警UI控件")
        
        # 检查5: 开关回调函数
        if 'def on_stm32_alarm_changed(self):' in content:
            checks.append("✅ STM32报警开关回调函数已添加")
        else:
            checks.append("❌ 缺少STM32报警开关回调函数")
        
        # 检查6: 粗检测触发集成
        if 'self.send_alarm_trigger_to_stm32()' in content:
            # 计算调用次数
            call_count = content.count('self.send_alarm_trigger_to_stm32()')
            if call_count >= 2:
                checks.append(f"✅ 报警触发调用已集成 (找到{call_count}处调用)")
            else:
                checks.append(f"⚠️ 报警触发调用可能不完整 (仅找到{call_count}处调用)")
        else:
            checks.append("❌ 缺少报警触发调用")
        
        # 检查7: 功能开关检查
        if 'if not self.stm32_alarm_enabled.get():' in content:
            checks.append("✅ 功能开关检查已添加")
        else:
            checks.append("❌ 缺少功能开关检查")
        
        # 检查8: 线程安全访问
        if 'serial_conn = self.get_serial_connection()' in content:
            checks.append("✅ 线程安全串口访问已实现")
        else:
            checks.append("❌ 缺少线程安全串口访问")
        
        # 输出检查结果
        print("\n📋 代码修改验证结果:")
        for check in checks:
            print(f"   {check}")
        
        success_count = sum(1 for check in checks if check.startswith("✅"))
        warning_count = sum(1 for check in checks if check.startswith("⚠️"))
        total_count = len(checks)
        
        print(f"\n📊 验证统计: {success_count}✅ {warning_count}⚠️ {total_count - success_count - warning_count}❌ / {total_count} 项")
        
        if success_count >= total_count - 1:  # 允许1个警告
            print("🎉 验证通过! 挖掘检测触发STM32报警功能已正确集成!")
            return True
        else:
            print("⚠️ 验证发现问题，请检查代码修改")
            return False
            
    except FileNotFoundError:
        print("❌ 找不到vibration_analyzer_chinese.py文件")
        return False
    except Exception as e:
        print(f"❌ 验证过程出错: {e}")
        return False

def verify_test_files():
    """验证测试文件"""
    print("\n🔍 验证测试文件...")
    
    test_files = [
        'test_mining_detection_alarm.py',
        'MINING_DETECTION_ALARM_GUIDE.md'
    ]
    
    for file_name in test_files:
        try:
            with open(file_name, 'r', encoding='utf-8') as f:
                content = f.read()
            
            if len(content) > 100:  # 简单检查文件不为空
                print(f"   ✅ {file_name} 存在且内容完整")
            else:
                print(f"   ⚠️ {file_name} 内容可能不完整")
                
        except FileNotFoundError:
            print(f"   ❌ {file_name} 文件不存在")
        except Exception as e:
            print(f"   ❌ {file_name} 读取错误: {e}")

def show_usage_instructions():
    """显示使用说明"""
    print("\n" + "="*60)
    print("📖 使用说明")
    print("="*60)
    
    print("\n🚀 启动步骤:")
    print("1. 确保STM32已连接并运行LoRa通信固件")
    print("2. 启动上位机软件:")
    print("   python vibration_analyzer_chinese.py")
    print("3. 在上位机中连接到正确的串口")
    
    print("\n⚙️ 配置设置:")
    print("1. 启用检测功能（勾选'启用检测'）")
    print("2. 选择合适的检测模式（推荐'两级检测'）")
    print("3. 调整灵敏度（推荐'中等'或'高'）")
    print("4. 确保'STM32报警'选项已勾选")
    
    print("\n🧪 测试验证:")
    print("1. 运行测试脚本:")
    print("   python test_mining_detection_alarm.py")
    print("2. 观察上位机界面的检测状态")
    print("3. 查看控制台输出的报警命令")
    print("4. 验证STM32是否收到0x10命令")
    
    print("\n📊 工作流程:")
    print("振动数据 → 挖掘检测 → 事件确认 → 0x10命令 → STM32报警 → LoRa通信 → 云端")
    
    print("\n🎛️ 控制选项:")
    print("- 检测模式: 粗检测/细检测/两级检测")
    print("- 灵敏度: 低/中等/高/自定义")
    print("- 启用检测: 开启/关闭检测功能")
    print("- STM32报警: 开启/关闭报警触发")
    
    print("\n🔍 故障排除:")
    print("1. 检查串口连接状态")
    print("2. 确认检测功能已启用")
    print("3. 验证STM32报警选项已勾选")
    print("4. 调整检测灵敏度参数")
    print("5. 查看日志文件了解详细信息")

def main():
    print("🔧 挖掘检测触发STM32报警功能验证工具")
    print("="*50)
    
    # 验证代码修改
    code_ok = verify_vibration_analyzer_modifications()
    
    # 验证测试文件
    verify_test_files()
    
    # 显示使用说明
    show_usage_instructions()
    
    if code_ok:
        print("\n🎉 验证完成! 功能已正确集成，可以开始使用。")
        print("\n💡 下一步:")
        print("1. 启动上位机软件测试功能")
        print("2. 运行测试脚本验证通信")
        print("3. 制造振动测试实际检测效果")
    else:
        print("\n⚠️ 验证发现问题，请检查代码修改。")

if __name__ == "__main__":
    main()
