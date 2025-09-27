#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
STM32 LoRa通信系统 - 二进制命令测试工具
"""

import serial
import time
import threading

class STM32LoRaTester:
    def __init__(self, port="COM8", baudrate=115200):
        self.port = port
        self.baudrate = baudrate
        self.ser = None
        self.running = False
        self.response_received = False
        
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
                    
                    # 尝试解析为文本响应
                    try:
                        text = data.decode('utf-8', errors='ignore').strip()
                        if text and any(c.isprintable() for c in text):
                            if any(keyword in text for keyword in ['ALARM_TRIGGERED', 'STATUS_OK', 'RESPONSE']):
                                print(f"📨 STM32响应: {text}")
                                self.response_received = True
                            elif len(text) < 50:  # 过滤长FFT数据
                                print(f"📥 数据: {text}")
                    except:
                        pass
                    
                    # 显示原始十六进制（仅前20字节）
                    if len(data) <= 20:
                        hex_str = data.hex().upper()
                        print(f"🔍 原始数据: {hex_str}")
                
                time.sleep(0.05)
            except Exception as e:
                if self.running:
                    print(f"❌ 监控错误: {e}")
                break
    
    def send_command(self, command_byte, description):
        """发送二进制命令"""
        if not self.ser or not self.ser.is_open:
            print("❌ 串口未连接")
            return False
        
        try:
            print(f"\n🚀 发送命令: {description}")
            print(f"   命令字节: 0x{command_byte:02X}")
            
            # 清空接收缓冲区
            if self.ser.in_waiting > 0:
                discarded = self.ser.read_all()
                print(f"   清空缓冲区: {len(discarded)} 字节")
            
            # 发送命令
            self.response_received = False
            self.ser.write(bytes([command_byte]))
            print(f"   ✅ 命令已发送")
            
            # 等待响应
            print("   ⏳ 等待响应...")
            timeout = 10  # 10秒超时
            start_time = time.time()
            
            while time.time() - start_time < timeout:
                if self.response_received:
                    print("   ✅ 收到响应!")
                    return True
                time.sleep(0.1)
            
            print("   ⚠️ 响应超时")
            return False
            
        except Exception as e:
            print(f"   ❌ 发送失败: {e}")
            return False
    
    def test_lora_communication(self):
        """测试LoRa通信功能"""
        print("\n" + "="*50)
        print("🔧 STM32 LoRa通信系统测试")
        print("="*50)
        
        if not self.connect():
            return
        
        # 启动数据监控
        self.start_monitoring()
        time.sleep(1)
        
        try:
            # 测试1: 查询状态
            print("\n📋 测试1: 查询系统状态")
            success1 = self.send_command(0x11, "查询状态命令")
            time.sleep(2)
            
            # 测试2: 触发报警
            print("\n🚨 测试2: 触发LoRa报警")
            success2 = self.send_command(0x10, "触发报警命令")
            time.sleep(5)  # 等待报警周期完成
            
            # 测试3: 再次查询状态
            print("\n📋 测试3: 再次查询状态")
            success3 = self.send_command(0x11, "查询状态命令")
            time.sleep(2)
            
            # 结果总结
            print("\n" + "="*50)
            print("📊 测试结果总结:")
            print(f"   状态查询1: {'✅ 成功' if success1 else '❌ 失败'}")
            print(f"   报警触发:   {'✅ 成功' if success2 else '❌ 失败'}")
            print(f"   状态查询2: {'✅ 成功' if success3 else '❌ 失败'}")
            
            if success1 and success2 and success3:
                print("🎉 所有测试通过! LoRa通信系统工作正常!")
            else:
                print("⚠️ 部分测试失败，请检查系统配置")
            
        except KeyboardInterrupt:
            print("\n⏹️ 用户中断测试")
        
        finally:
            self.stop_monitoring()
            time.sleep(1)
            self.disconnect()

def main():
    tester = STM32LoRaTester("COM8")
    tester.test_lora_communication()

if __name__ == "__main__":
    main()
