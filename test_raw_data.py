#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试原始数据功能的简单脚本
"""

import serial
import serial.tools.list_ports
import struct
import time

class SimpleProtocolParser:
    """简单的协议解析器用于测试"""
    
    def __init__(self):
        self.buffer = bytearray()
        
    def add_data(self, data):
        """添加数据并解析帧"""
        self.buffer.extend(data)
        frames = []
        
        while len(self.buffer) >= 10:
            # 查找帧头
            frame_start = -1
            for i in range(len(self.buffer) - 1):
                if self.buffer[i] == 0xAA and self.buffer[i+1] == 0x55:
                    frame_start = i
                    break
            
            if frame_start == -1:
                self.buffer.clear()
                break
            
            if frame_start > 0:
                self.buffer = self.buffer[frame_start:]
            
            if len(self.buffer) < 10:
                break
            
            try:
                cmd = self.buffer[2]
                length = struct.unpack('<H', self.buffer[3:5])[0]
                
                if len(self.buffer) < 5 + length + 2:
                    break
                
                payload = self.buffer[5:5+length]
                checksum = self.buffer[5+length]
                frame_end = self.buffer[5+length+1]
                
                if frame_end != 0x0D:
                    self.buffer = self.buffer[1:]
                    continue
                
                # 验证校验和
                calc_checksum = cmd
                for byte in payload:
                    calc_checksum ^= byte
                calc_checksum ^= length & 0xFF
                calc_checksum ^= (length >> 8) & 0xFF
                calc_checksum &= 0xFF
                
                if calc_checksum != checksum:
                    self.buffer = self.buffer[1:]
                    continue
                
                # 解析载荷
                frame = self._parse_payload(cmd, payload)
                if frame:
                    frames.append(frame)
                
                self.buffer = self.buffer[5+length+2:]
                
            except Exception as e:
                print(f"解析错误: {e}")
                self.buffer = self.buffer[1:]
        
        return frames
    
    def _parse_payload(self, cmd, payload):
        """解析载荷数据"""
        try:
            if cmd == 0x02 and len(payload) >= 16:  # 原始加速度数据
                timestamp = struct.unpack('<I', payload[:4])[0]
                accel_x = struct.unpack('<f', payload[4:8])[0]
                accel_y = struct.unpack('<f', payload[8:12])[0]
                accel_z = struct.unpack('<f', payload[12:16])[0]
                
                return {
                    'type': 'raw_accel',
                    'timestamp': timestamp,
                    'accel_x': accel_x,
                    'accel_y': accel_y,
                    'accel_z': accel_z
                }
            elif cmd == 0x04:  # 频域数据
                return {
                    'type': 'spectrum',
                    'cmd': cmd,
                    'length': len(payload)
                }
                
        except Exception as e:
            print(f"载荷解析错误: {e}")
        
        return None

def main():
    """主测试函数"""
    print("原始数据接收测试")
    print("==================")
    
    # 列出可用串口
    ports = list(serial.tools.list_ports.comports())
    if not ports:
        print("未找到可用串口")
        return
    
    print("可用串口:")
    for i, port in enumerate(ports):
        print(f"{i}: {port.device} - {port.description}")
    
    # 选择串口
    try:
        port_index = int(input("请选择串口编号: "))
        selected_port = ports[port_index].device
    except (ValueError, IndexError):
        print("无效的串口选择")
        return
    
    # 连接串口
    try:
        ser = serial.Serial(selected_port, 115200, timeout=1)
        print(f"已连接到 {selected_port}")
    except Exception as e:
        print(f"连接失败: {e}")
        return
    
    parser = SimpleProtocolParser()
    raw_data_count = 0
    spectrum_count = 0
    
    print("\n开始接收数据... (按Ctrl+C停止)")
    print("时间戳\t\tX轴(g)\t\tY轴(g)\t\tZ轴(g)")
    print("-" * 60)
    
    try:
        while True:
            if ser.in_waiting > 0:
                data = ser.read(ser.in_waiting)
                frames = parser.add_data(data)
                
                for frame in frames:
                    if frame['type'] == 'raw_accel':
                        raw_data_count += 1
                        print(f"{frame['timestamp']}\t{frame['accel_x']:.4f}\t\t{frame['accel_y']:.4f}\t\t{frame['accel_z']:.4f}")
                    elif frame['type'] == 'spectrum':
                        spectrum_count += 1
                        if spectrum_count % 10 == 0:  # 每10个频域数据包打印一次
                            print(f"[频域数据] 命令: 0x{frame['cmd']:02X}, 长度: {frame['length']} 字节")
            
            time.sleep(0.01)
            
    except KeyboardInterrupt:
        print(f"\n\n测试结束")
        print(f"接收到原始数据包: {raw_data_count}")
        print(f"接收到频域数据包: {spectrum_count}")
    
    finally:
        ser.close()
        print("串口已关闭")

if __name__ == '__main__':
    main()
