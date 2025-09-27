#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
十六进制数据测试工具
用于测试STM32的二进制协议通信
"""

import serial
import time

def hex_test():
    try:
        print("连接到COM8...")
        ser = serial.Serial("COM8", 115200, timeout=1)
        time.sleep(2)
        
        print("分析当前数据流格式...")
        # 先读取一些数据来分析格式
        print("读取原始数据流...")
        for i in range(10):
            if ser.in_waiting > 0:
                data = ser.read(ser.in_waiting)
                print(f"原始数据 ({len(data)} 字节): {data.hex()}")
                
                # 尝试解析为文本
                try:
                    text = data.decode('utf-8', errors='ignore')
                    if any(c.isprintable() for c in text):
                        print(f"文本内容: {repr(text)}")
                except:
                    pass
                break
            time.sleep(0.5)
        
        print("\n尝试发送二进制命令...")
        
        # 尝试发送原有的协议命令（如果存在）
        # 根据之前的分析，可能有0x02和0x04命令
        test_commands = [
            b'\x02',  # 原始数据命令
            b'\x04',  # 频域数据命令
            b'STATUS\n',  # ASCII命令
            b'TRIGGER_ALARM\n',  # ASCII命令
        ]
        
        for i, cmd in enumerate(test_commands):
            print(f"\n--- 测试命令 {i+1}: {cmd.hex() if len(cmd) < 10 else repr(cmd)} ---")
            
            # 清空缓冲区
            if ser.in_waiting > 0:
                discarded = ser.read_all()
                print(f"清空缓冲区: {len(discarded)} 字节")
            
            # 发送命令
            ser.write(cmd)
            print(f"已发送: {cmd.hex() if len(cmd) < 10 else repr(cmd)}")
            
            # 等待响应
            print("等待响应...")
            response_found = False
            
            for j in range(30):  # 等待3秒
                if ser.in_waiting > 0:
                    data = ser.read(min(100, ser.in_waiting))  # 读取最多100字节
                    
                    # 显示十六进制
                    print(f"收到数据: {data.hex()}")
                    
                    # 尝试解析为文本
                    try:
                        text = data.decode('utf-8', errors='ignore').strip()
                        if text and any(c.isprintable() for c in text):
                            print(f"文本内容: {repr(text)}")
                            
                            # 检查是否是响应
                            if "RESPONSE:" in text:
                                print("✅ 检测到响应消息!")
                                response_found = True
                                break
                    except:
                        pass
                    
                    # 检查是否是特定的二进制响应模式
                    if len(data) >= 4:
                        # 检查是否符合某种协议格式
                        if data[0] in [0x46, 0x01, 0x02, 0x04]:  # 可能的协议头
                            print("✅ 检测到可能的协议响应!")
                            response_found = True
                            break
                
                time.sleep(0.1)
            
            if not response_found:
                print("❌ 未检测到明确的响应")
            
            time.sleep(1)  # 命令间隔
        
        print("\n持续监听数据流...")
        print("观察数据模式，按Ctrl+C退出")
        
        try:
            packet_count = 0
            while packet_count < 20:  # 监听20个数据包
                if ser.in_waiting > 0:
                    data = ser.read(min(50, ser.in_waiting))
                    packet_count += 1
                    
                    print(f"数据包 {packet_count}: {data.hex()}")
                    
                    # 分析数据特征
                    if len(data) > 0:
                        print(f"  长度: {len(data)}, 首字节: 0x{data[0]:02X}")
                        
                        # 检查是否有固定的包头
                        if data[0] in [0x55, 0xAA, 0xFF, 0x00, 0x01, 0x02, 0x04]:
                            print(f"  可能的协议头: 0x{data[0]:02X}")
                
                time.sleep(0.2)
                
        except KeyboardInterrupt:
            print("\n用户中断监听")
        
        ser.close()
        print("测试完成")
        
    except Exception as e:
        print(f"错误: {e}")

if __name__ == "__main__":
    hex_test()
