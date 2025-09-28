#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试挖掘检测触发STM32报警功能
"""

import serial
import time
import threading
import struct
import numpy as np

class MiningDetectionAlarmTester:
    def __init__(self, port="COM8", baudrate=115200):
        self.port = port
        self.baudrate = baudrate
        self.ser = None
        self.running = False
        self.alarm_commands_received = 0
        
    def connect(self):
        try:
            self.ser = serial.Serial(self.port, self.baudrate, timeout=1)
            time.sleep(2)
            print(f"✅ 已连接到 {self.port}")
            return True
        except Exception as e:
            print(f"❌ 连接失败: {e}")
            return False
    
    def disconnect(self):
        if self.ser and self.ser.is_open:
            self.ser.close()
            print("🔌 已断开连接")
    
    def start_monitoring(self):
        """启动数据监控线程"""
        self.running = True
        monitor_thread = threading.Thread(target=self._monitor_data)
        monitor_thread.daemon = True
        monitor_thread.start()
    
    def stop_monitoring(self):
        """停止数据监控"""
        self.running = False
    
    def _monitor_data(self):
        """监控串口数据"""
        while self.running and self.ser and self.ser.is_open:
            try:
                if self.ser.in_waiting > 0:
                    data = self.ser.read(min(100, self.ser.in_waiting))
                    
                    # 检查是否收到0x10命令（触发报警）
                    for byte in data:
                        if byte == 0x10:
                            self.alarm_commands_received += 1
                            print(f"🚨 收到STM32报警触发命令 #{self.alarm_commands_received}: 0x{byte:02X}")
                    
                    # 尝试解析为文本响应
                    try:
                        text = data.decode('utf-8', errors='ignore').strip()
                        if text and any(c.isprintable() for c in text):
                            # 过滤掉长FFT数据
                            if len(text) < 100 and any(keyword in text for keyword in 
                                ['挖掘', '检测', '触发', '报警', 'STM32', 'mining', 'detection']):
                                print(f"📥 上位机消息: {text}")
                    except:
                        pass
                
                time.sleep(0.05)
            except Exception as e:
                if self.running:
                    print(f"❌ 监控错误: {e}")
                break
    
    def send_simulated_vibration_data(self, intensity="high"):
        """发送模拟振动数据到上位机"""
        if not self.ser or not self.ser.is_open:
            print("❌ 串口未连接")
            return False
        
        try:
            print(f"📡 发送模拟{intensity}强度振动数据...")
            
            # 根据强度设置不同的振动参数
            if intensity == "high":
                # 高强度振动，容易触发检测
                base_amplitude = 0.05  # 50mg
                noise_level = 0.01     # 10mg噪声
                frequency = 25         # 25Hz主频
            elif intensity == "medium":
                # 中等强度振动
                base_amplitude = 0.02  # 20mg
                noise_level = 0.005    # 5mg噪声
                frequency = 35         # 35Hz主频
            else:  # low
                # 低强度振动，不太容易触发
                base_amplitude = 0.005 # 5mg
                noise_level = 0.002    # 2mg噪声
                frequency = 50         # 50Hz主频
            
            # 生成模拟数据包
            for i in range(50):  # 发送50个数据包
                # 生成三轴加速度数据
                t = i * 0.001  # 1ms间隔
                
                # 主信号 + 噪声
                x_accel = base_amplitude * np.sin(2 * np.pi * frequency * t) + np.random.normal(0, noise_level)
                y_accel = base_amplitude * np.sin(2 * np.pi * frequency * t + np.pi/3) + np.random.normal(0, noise_level)
                z_accel = base_amplitude * np.sin(2 * np.pi * frequency * t + 2*np.pi/3) + np.random.normal(0, noise_level)
                
                # 构造数据包 (模拟STM32发送的格式)
                # 帧头: 0xAA 0x55
                # 数据类型: 0x01 (原始加速度)
                # 数据长度: 12字节 (3个float)
                # 数据: x, y, z (float32)
                # 校验和: 简单累加
                
                packet = bytearray()
                packet.extend([0xAA, 0x55])  # 帧头
                packet.append(0x01)          # 数据类型
                packet.append(12)            # 数据长度
                
                # 添加加速度数据
                packet.extend(struct.pack('<f', x_accel))  # X轴
                packet.extend(struct.pack('<f', y_accel))  # Y轴
                packet.extend(struct.pack('<f', z_accel))  # Z轴
                
                # 计算校验和
                checksum = sum(packet[2:]) & 0xFF
                packet.append(checksum)
                
                # 发送数据包
                self.ser.write(packet)
                time.sleep(0.01)  # 10ms间隔
            
            print(f"✅ 已发送{intensity}强度模拟振动数据")
            return True
            
        except Exception as e:
            print(f"❌ 发送模拟数据失败: {e}")
            return False
    
    def test_mining_detection_alarm(self):
        """测试挖掘检测触发STM32报警功能"""
        print("\n" + "="*60)
        print("🔧 挖掘检测触发STM32报警功能测试")
        print("="*60)
        print("📋 测试说明:")
        print("1. 此测试模拟STM32向上位机发送振动数据")
        print("2. 上位机检测到挖掘事件后会发送0x10命令")
        print("3. 我们监控串口，统计收到的报警触发命令")
        print("4. 请确保上位机软件已启动并连接到同一串口")
        print()
        
        if not self.connect():
            return
        
        # 启动数据监控
        self.start_monitoring()
        time.sleep(1)
        
        try:
            # 测试1: 发送高强度振动数据
            print("\n🚨 测试1: 发送高强度振动数据（应该触发检测）")
            self.send_simulated_vibration_data("high")
            print("   等待上位机检测和响应...")
            time.sleep(5)
            
            # 测试2: 发送中等强度振动数据
            print("\n📊 测试2: 发送中等强度振动数据（可能触发检测）")
            self.send_simulated_vibration_data("medium")
            print("   等待上位机检测和响应...")
            time.sleep(5)
            
            # 测试3: 发送低强度振动数据
            print("\n📉 测试3: 发送低强度振动数据（不太可能触发检测）")
            self.send_simulated_vibration_data("low")
            print("   等待上位机检测和响应...")
            time.sleep(5)
            
            # 测试4: 连续发送高强度数据
            print("\n🔥 测试4: 连续发送高强度振动数据（测试连续检测）")
            for i in range(3):
                print(f"   发送第{i+1}轮高强度数据...")
                self.send_simulated_vibration_data("high")
                time.sleep(3)
            
            print("   等待最终响应...")
            time.sleep(5)
            
            # 结果总结
            print("\n" + "="*60)
            print("📊 测试结果总结:")
            print(f"   收到STM32报警触发命令总数: {self.alarm_commands_received}")
            
            if self.alarm_commands_received > 0:
                print("🎉 测试成功! 上位机能够正确检测挖掘事件并发送报警命令给STM32!")
                print("✅ 挖掘检测→STM32报警触发功能正常工作")
            else:
                print("⚠️ 未收到报警触发命令，可能的原因:")
                print("   1. 上位机软件未启动或未连接")
                print("   2. 检测参数设置过于严格")
                print("   3. STM32报警触发功能被禁用")
                print("   4. 模拟数据强度不足以触发检测")
            
            print("\n💡 建议:")
            print("1. 确保上位机软件已启动并连接到串口")
            print("2. 在上位机中启用'STM32报警'选项")
            print("3. 调整检测灵敏度为'高'或'中等'")
            print("4. 观察上位机界面的检测状态显示")
            
        except KeyboardInterrupt:
            print("\n⏹️ 用户中断测试")
        
        finally:
            self.stop_monitoring()
            time.sleep(1)
            self.disconnect()

def main():
    print("🔧 挖掘检测触发STM32报警功能测试工具")
    print("请确保:")
    print("1. 上位机软件(vibration_analyzer_chinese.py)已启动")
    print("2. 上位机已连接到串口")
    print("3. 上位机中'STM32报警'选项已启用")
    print("4. 检测功能已启用")
    
    input("\n按回车键开始测试...")
    
    # 可以修改串口号
    tester = MiningDetectionAlarmTester("COM8")
    tester.test_mining_detection_alarm()

if __name__ == "__main__":
    main()
