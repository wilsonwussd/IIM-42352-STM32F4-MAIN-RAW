#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
专业振动分析仪 - 中文版 (字体修复版)
支持Y轴缩放和分辨率调节功能
"""

import tkinter as tk
from tkinter import ttk, messagebox
import serial
import serial.tools.list_ports
import threading
import time
import struct
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.figure import Figure
import matplotlib
# 配置中文字体 - 使用系统检测到的字体
matplotlib.rcParams['font.sans-serif'] = ['Microsoft JhengHei', 'Microsoft YaHei', 'SimHei', 'SimSun', 'Arial Unicode MS', 'DejaVu Sans']
matplotlib.rcParams['axes.unicode_minus'] = False
import queue
from collections import deque

class ProtocolParser:
    """协议解析器 - 保持原始数据精度"""
    
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
                
            except Exception:
                self.buffer = self.buffer[1:]
        
        return frames
    
    def _parse_payload(self, cmd, payload):
        """解析载荷数据 - 保持原始精度"""
        try:
            if cmd == 0x01 and len(payload) >= 88:  # 21点数据
                timestamp = struct.unpack('<I', payload[:4])[0]
                magnitudes = []
                
                for i in range(4, 88, 4):
                    mag = struct.unpack('<f', payload[i:i+4])[0]
                    magnitudes.append(mag)  # 保持原始数据
                
                frequencies = [i * 25.0 for i in range(21)]
                
                return {
                    'type': 'spectrum',
                    'timestamp': timestamp,
                    'frequencies': frequencies,
                    'magnitudes': magnitudes,
                    'data_points': 21
                }
                
            elif cmd == 0x04 and len(payload) >= 1032:  # 257点数据
                timestamp = struct.unpack('<I', payload[:4])[0]
                magnitudes = []
                
                for i in range(4, 1032, 4):
                    mag = struct.unpack('<f', payload[i:i+4])[0]
                    magnitudes.append(mag)  # 保持原始数据
                
                frequencies = [i * 1000.0 / 512 for i in range(257)]
                
                return {
                    'type': 'spectrum_full',
                    'timestamp': timestamp,
                    'frequencies': frequencies,
                    'magnitudes': magnitudes,
                    'data_points': 257
                }
                
        except Exception:
            pass
        
        return None

class VibrAnalyzer:
    """振动分析仪主类"""
    
    def __init__(self, root):
        self.root = root
        self.root.title("专业振动分析仪 v3.0 - 真实数据显示")
        self.root.geometry("1200x800")
        
        # 数据相关
        self.serial_conn = None
        self.parser = ProtocolParser()
        self.data_queue = queue.Queue()
        self.running = False
        self.data_buffer = deque(maxlen=100)
        
        # 显示控制参数
        self.y_scale_mode = tk.StringVar(value="auto")  # auto, manual, log
        self.y_min = tk.DoubleVar(value=0.0)
        self.y_max = tk.DoubleVar(value=1.0)
        self.y_unit = tk.StringVar(value="g")  # g, mg, μg
        self.show_grid = tk.BooleanVar(value=True)
        self.show_peak_labels = tk.BooleanVar(value=True)
        
        # 统计信息
        self.frame_count = 0
        self.update_rate = 0.0
        self.last_update_time = time.time()
        
        self.setup_ui()
        self.setup_plot()
        
    def setup_ui(self):
        """设置用户界面"""
        # 主框架
        main_frame = ttk.Frame(self.root)
        main_frame.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
        
        # 控制面板
        control_frame = ttk.LabelFrame(main_frame, text="控制面板", padding=5)
        control_frame.pack(fill=tk.X, pady=(0, 5))
        
        # 串口控制
        serial_frame = ttk.Frame(control_frame)
        serial_frame.pack(fill=tk.X, pady=(0, 5))
        
        ttk.Label(serial_frame, text="串口:").pack(side=tk.LEFT)
        self.port_combo = ttk.Combobox(serial_frame, width=15)
        self.port_combo.pack(side=tk.LEFT, padx=(5, 10))
        
        self.connect_btn = ttk.Button(serial_frame, text="连接", command=self.toggle_connection)
        self.connect_btn.pack(side=tk.LEFT, padx=(0, 10))
        
        self.status_label = ttk.Label(serial_frame, text="未连接", foreground="red")
        self.status_label.pack(side=tk.LEFT)
        
        # Y轴控制面板
        y_control_frame = ttk.LabelFrame(control_frame, text="Y轴显示控制", padding=5)
        y_control_frame.pack(fill=tk.X, pady=(5, 0))
        
        # Y轴缩放模式
        scale_frame = ttk.Frame(y_control_frame)
        scale_frame.pack(fill=tk.X, pady=(0, 5))
        
        ttk.Label(scale_frame, text="缩放模式:").pack(side=tk.LEFT)
        ttk.Radiobutton(scale_frame, text="自动", variable=self.y_scale_mode, 
                       value="auto", command=self.update_plot_settings).pack(side=tk.LEFT, padx=5)
        ttk.Radiobutton(scale_frame, text="手动", variable=self.y_scale_mode, 
                       value="manual", command=self.update_plot_settings).pack(side=tk.LEFT, padx=5)
        ttk.Radiobutton(scale_frame, text="对数", variable=self.y_scale_mode, 
                       value="log", command=self.update_plot_settings).pack(side=tk.LEFT, padx=5)
        
        # 手动范围设置
        range_frame = ttk.Frame(y_control_frame)
        range_frame.pack(fill=tk.X, pady=(0, 5))
        
        ttk.Label(range_frame, text="Y轴范围:").pack(side=tk.LEFT)
        ttk.Label(range_frame, text="最小值:").pack(side=tk.LEFT, padx=(10, 0))
        self.y_min_entry = ttk.Entry(range_frame, textvariable=self.y_min, width=10)
        self.y_min_entry.pack(side=tk.LEFT, padx=5)
        self.y_min_entry.bind('<Return>', lambda e: self.update_plot_settings())
        
        ttk.Label(range_frame, text="最大值:").pack(side=tk.LEFT, padx=(10, 0))
        self.y_max_entry = ttk.Entry(range_frame, textvariable=self.y_max, width=10)
        self.y_max_entry.pack(side=tk.LEFT, padx=5)
        self.y_max_entry.bind('<Return>', lambda e: self.update_plot_settings())
        
        # 单位和显示选项
        options_frame = ttk.Frame(y_control_frame)
        options_frame.pack(fill=tk.X)
        
        ttk.Label(options_frame, text="单位:").pack(side=tk.LEFT)
        unit_combo = ttk.Combobox(options_frame, textvariable=self.y_unit, 
                                 values=["g", "mg", "μg"], width=8, state="readonly")
        unit_combo.pack(side=tk.LEFT, padx=5)
        unit_combo.bind('<<ComboboxSelected>>', lambda e: self.update_plot_settings())
        
        ttk.Checkbutton(options_frame, text="显示网格", variable=self.show_grid,
                       command=self.update_plot_settings).pack(side=tk.LEFT, padx=(20, 5))
        ttk.Checkbutton(options_frame, text="显示峰值标注", variable=self.show_peak_labels,
                       command=self.update_plot_settings).pack(side=tk.LEFT, padx=5)
        
        # 快速缩放按钮
        quick_frame = ttk.Frame(y_control_frame)
        quick_frame.pack(fill=tk.X, pady=(5, 0))
        
        ttk.Label(quick_frame, text="快速缩放:").pack(side=tk.LEFT)
        ttk.Button(quick_frame, text="微振动(0-1mg)", 
                  command=lambda: self.set_quick_scale(0, 0.001)).pack(side=tk.LEFT, padx=2)
        ttk.Button(quick_frame, text="小振动(0-10mg)", 
                  command=lambda: self.set_quick_scale(0, 0.01)).pack(side=tk.LEFT, padx=2)
        ttk.Button(quick_frame, text="中振动(0-100mg)", 
                  command=lambda: self.set_quick_scale(0, 0.1)).pack(side=tk.LEFT, padx=2)
        ttk.Button(quick_frame, text="大振动(0-1g)", 
                  command=lambda: self.set_quick_scale(0, 1.0)).pack(side=tk.LEFT, padx=2)
        
        # 图表框架
        plot_frame = ttk.LabelFrame(main_frame, text="频域分析", padding=5)
        plot_frame.pack(fill=tk.BOTH, expand=True)
        
        self.plot_container = plot_frame
        
        # 状态栏
        status_frame = ttk.Frame(main_frame)
        status_frame.pack(fill=tk.X, pady=(5, 0))
        
        self.info_label = ttk.Label(status_frame, text="更新频率: 0.0 Hz | 接收数据: 0 | 峰值频率: -- Hz | 峰值幅度: -- | 最大值: --")
        self.info_label.pack(side=tk.LEFT)
        
        # 刷新串口列表
        self.refresh_ports()
        
    def setup_plot(self):
        """设置绘图"""
        self.fig = Figure(figsize=(10, 6), dpi=100)
        self.ax = self.fig.add_subplot(111)
        
        self.canvas = FigureCanvasTkAgg(self.fig, self.plot_container)
        self.canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True)
        
        # 初始化空图
        self.ax.set_xlabel('频率 (Hz)')
        self.ax.set_ylabel('幅度 (g)')
        self.ax.set_title('振动频域分析 - 真实数据显示')
        self.ax.grid(True, alpha=0.3)
        self.ax.set_xlim(0, 500)
        self.ax.set_ylim(0, 1)
        
        self.canvas.draw()
        
    def refresh_ports(self):
        """刷新串口列表"""
        ports = [port.device for port in serial.tools.list_ports.comports()]
        self.port_combo['values'] = ports
        if ports and not self.port_combo.get():
            self.port_combo.set(ports[0])
            
    def set_quick_scale(self, y_min, y_max):
        """快速设置Y轴范围"""
        self.y_scale_mode.set("manual")
        self.y_min.set(y_min)
        self.y_max.set(y_max)
        self.update_plot_settings()
        
    def update_plot_settings(self):
        """更新绘图设置"""
        if hasattr(self, 'ax'):
            self.ax.grid(self.show_grid.get(), alpha=0.3)
            unit = self.y_unit.get()
            self.ax.set_ylabel(f'幅度 ({unit})')
            
            if hasattr(self, 'current_data') and self.current_data:
                self.update_plot(self.current_data)
            else:
                self.canvas.draw()
                
    def convert_units(self, data_g):
        """转换单位"""
        unit = self.y_unit.get()
        if unit == "mg":
            return np.array(data_g) * 1000
        elif unit == "μg":
            return np.array(data_g) * 1000000
        else:  # g
            return np.array(data_g)
            
    def toggle_connection(self):
        """切换连接状态"""
        if self.running:
            self.disconnect()
        else:
            self.connect()
            
    def connect(self):
        """连接串口"""
        port = self.port_combo.get()
        if not port:
            messagebox.showerror("错误", "请选择串口")
            return
            
        try:
            self.serial_conn = serial.Serial(port, 115200, timeout=1)
            self.running = True
            self.connect_btn.config(text="断开")
            self.status_label.config(text="已连接", foreground="green")
            
            # 启动数据接收线程
            self.receive_thread = threading.Thread(target=self.receive_data, daemon=True)
            self.receive_thread.start()
            
            # 启动数据处理
            self.process_data()
            
        except Exception as e:
            messagebox.showerror("连接错误", f"无法连接到 {port}: {str(e)}")
            
    def disconnect(self):
        """断开连接"""
        self.running = False
        if self.serial_conn:
            self.serial_conn.close()
            self.serial_conn = None
        
        self.connect_btn.config(text="连接")
        self.status_label.config(text="未连接", foreground="red")
        
    def receive_data(self):
        """接收数据线程"""
        while self.running:
            try:
                if self.serial_conn and self.serial_conn.in_waiting > 0:
                    data = self.serial_conn.read(self.serial_conn.in_waiting)
                    frames = self.parser.add_data(data)
                    
                    for frame in frames:
                        self.data_queue.put(frame)
                        
                time.sleep(0.01)
                
            except Exception as e:
                if self.running:
                    print(f"接收数据错误: {e}")
                break
                
    def process_data(self):
        """处理数据"""
        try:
            while not self.data_queue.empty():
                frame = self.data_queue.get_nowait()
                self.data_buffer.append(frame)
                self.frame_count += 1
                
                # 计算更新频率
                current_time = time.time()
                if current_time - self.last_update_time >= 1.0:
                    self.update_rate = self.frame_count / (current_time - self.last_update_time)
                    self.frame_count = 0
                    self.last_update_time = current_time
                
                # 更新显示
                self.update_plot(frame)
                
        except queue.Empty:
            pass
        
        if self.running:
            self.root.after(50, self.process_data)
            
    def update_plot(self, frame):
        """更新绘图 - 显示真实数据"""
        self.current_data = frame
        
        frequencies = frame['frequencies']
        magnitudes_g = frame['magnitudes']  # 原始g值数据
        
        # 转换单位
        magnitudes_display = self.convert_units(magnitudes_g)
        
        # 清除之前的绘图
        self.ax.clear()
        
        # 设置基本属性
        unit = self.y_unit.get()
        self.ax.set_xlabel('频率 (Hz)')
        self.ax.set_ylabel(f'幅度 ({unit})')
        self.ax.set_title('振动频域分析 - 真实数据显示')
        
        if self.show_grid.get():
            self.ax.grid(True, alpha=0.3)
        
        # 绘制频谱
        if frame['data_points'] == 21:
            # 21点柱状图
            self.ax.bar(frequencies, magnitudes_display, width=20, alpha=0.7, color='blue')
        else:
            # 257点连续曲线
            self.ax.fill_between(frequencies, magnitudes_display, alpha=0.6, color='lightblue')
            self.ax.plot(frequencies, magnitudes_display, linewidth=1.5, color='blue')
        
        # 设置X轴范围
        self.ax.set_xlim(0, max(frequencies))
        
        # 设置Y轴范围
        scale_mode = self.y_scale_mode.get()
        if scale_mode == "auto":
            if len(magnitudes_display) > 0:
                y_max = max(magnitudes_display) * 1.1
                y_min = min(0, min(magnitudes_display))
                self.ax.set_ylim(y_min, y_max)
        elif scale_mode == "manual":
            y_min_val = self.convert_units([self.y_min.get()])[0]
            y_max_val = self.convert_units([self.y_max.get()])[0]
            self.ax.set_ylim(y_min_val, y_max_val)
        elif scale_mode == "log":
            self.ax.set_yscale('log')
            positive_mags = [m for m in magnitudes_display if m > 0]
            if positive_mags:
                self.ax.set_ylim(min(positive_mags) * 0.1, max(positive_mags) * 10)
        
        # 峰值检测和标注
        if len(magnitudes_display) > 1:
            ac_magnitudes = magnitudes_display[1:]  # 排除DC分量
            ac_frequencies = frequencies[1:]
            
            if len(ac_magnitudes) > 0:
                max_idx = np.argmax(ac_magnitudes)
                peak_freq = ac_frequencies[max_idx]
                peak_mag = ac_magnitudes[max_idx]
                
                # 标注峰值
                if self.show_peak_labels.get():
                    self.ax.plot(peak_freq, peak_mag, 'ro', markersize=8)
                    self.ax.annotate(f'{peak_freq:.1f}Hz\n{peak_mag:.6f}{unit}', 
                                   xy=(peak_freq, peak_mag), 
                                   xytext=(10, 10), textcoords='offset points',
                                   bbox=dict(boxstyle='round,pad=0.3', facecolor='yellow', alpha=0.7),
                                   arrowprops=dict(arrowstyle='->', connectionstyle='arc3,rad=0'))
                
                # 更新状态信息
                max_val = max(magnitudes_display)
                info_text = (f"更新频率: {self.update_rate:.1f} Hz | "
                           f"接收数据: {len(self.data_buffer)} | "
                           f"峰值频率: {peak_freq:.1f} Hz | "
                           f"峰值幅度: {peak_mag:.6f} {unit} | "
                           f"最大值: {max_val:.6f} {unit}")
                self.info_label.config(text=info_text)
        
        self.canvas.draw()

def main():
    """主函数"""
    root = tk.Tk()
    app = VibrAnalyzer(root)
    
    def on_closing():
        app.disconnect()
        root.destroy()
    
    root.protocol("WM_DELETE_WINDOW", on_closing)
    root.mainloop()

if __name__ == '__main__':
    main()
