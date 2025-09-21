#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Professional Vibration Analyzer - Real Data Display
Supports Y-axis scaling and resolution adjustment
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
import queue
from collections import deque

class ProfessionalProtocolParser:
    """Professional protocol parser - maintains original data precision"""
    
    def __init__(self):
        self.buffer = bytearray()
        
    def add_data(self, data):
        """Add data and parse frames"""
        self.buffer.extend(data)
        frames = []
        
        while len(self.buffer) >= 10:
            # Find frame header
            frame_start = -1
            for i in range(len(self.buffer) - 1):
                if self.buffer[i] == 0xAA and self.buffer[i+1] == 0x55:
                    frame_start = i
                    break
            
            if frame_start == -1:
                self.buffer.clear()
                break
            
            # Remove data before frame header
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
                
                # Verify checksum
                calc_checksum = cmd
                for byte in payload:
                    calc_checksum ^= byte
                calc_checksum ^= length & 0xFF
                calc_checksum ^= (length >> 8) & 0xFF
                calc_checksum &= 0xFF
                
                if calc_checksum != checksum:
                    self.buffer = self.buffer[1:]
                    continue
                
                # Parse payload
                frame = self._parse_payload(cmd, payload)
                if frame:
                    frames.append(frame)
                
                self.buffer = self.buffer[5+length+2:]
                
            except Exception:
                self.buffer = self.buffer[1:]
        
        return frames
    
    def _parse_payload(self, cmd, payload):
        """Parse payload data - maintain original precision"""
        try:
            if cmd == 0x01 and len(payload) >= 88:  # 21-point data
                timestamp = struct.unpack('<I', payload[:4])[0]
                magnitudes = []
                
                for i in range(4, 88, 4):
                    mag = struct.unpack('<f', payload[i:i+4])[0]
                    magnitudes.append(mag)  # Keep original data, no scaling
                
                frequencies = [i * 25.0 for i in range(21)]
                
                return {
                    'type': 'spectrum',
                    'timestamp': timestamp,
                    'frequencies': frequencies,
                    'magnitudes': magnitudes,
                    'data_points': 21
                }
                
            elif cmd == 0x04 and len(payload) >= 1032:  # 257-point data
                timestamp = struct.unpack('<I', payload[:4])[0]
                magnitudes = []
                
                for i in range(4, 1032, 4):
                    mag = struct.unpack('<f', payload[i:i+4])[0]
                    magnitudes.append(mag)  # Keep original data, no scaling
                
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

class ProfessionalVibrAnalyzer:
    """Professional vibration analyzer main class"""
    
    def __init__(self, root):
        self.root = root
        self.root.title("Professional Vibration Analyzer v3.0 - Real Data Display")
        self.root.geometry("1200x800")
        
        # Data related
        self.serial_conn = None
        self.parser = ProfessionalProtocolParser()
        self.data_queue = queue.Queue()
        self.running = False
        self.data_buffer = deque(maxlen=100)
        
        # Display control parameters
        self.y_scale_mode = tk.StringVar(value="auto")  # auto, manual, log
        self.y_min = tk.DoubleVar(value=0.0)
        self.y_max = tk.DoubleVar(value=1.0)
        self.y_unit = tk.StringVar(value="g")  # g, mg, μg
        self.freq_range_max = tk.DoubleVar(value=500.0)
        self.show_grid = tk.BooleanVar(value=True)
        self.show_peak_labels = tk.BooleanVar(value=True)
        
        # Statistics
        self.frame_count = 0
        self.update_rate = 0.0
        self.last_update_time = time.time()
        
        self.setup_ui()
        self.setup_plot()
        
    def setup_ui(self):
        """Setup user interface"""
        # Main frame
        main_frame = ttk.Frame(self.root)
        main_frame.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
        
        # Control panel
        control_frame = ttk.LabelFrame(main_frame, text="Control Panel", padding=5)
        control_frame.pack(fill=tk.X, pady=(0, 5))
        
        # Serial control
        serial_frame = ttk.Frame(control_frame)
        serial_frame.pack(fill=tk.X, pady=(0, 5))
        
        ttk.Label(serial_frame, text="Port:").pack(side=tk.LEFT)
        self.port_combo = ttk.Combobox(serial_frame, width=15)
        self.port_combo.pack(side=tk.LEFT, padx=(5, 10))
        
        self.connect_btn = ttk.Button(serial_frame, text="Connect", command=self.toggle_connection)
        self.connect_btn.pack(side=tk.LEFT, padx=(0, 10))
        
        self.status_label = ttk.Label(serial_frame, text="Disconnected", foreground="red")
        self.status_label.pack(side=tk.LEFT)
        
        # Y-axis control panel
        y_control_frame = ttk.LabelFrame(control_frame, text="Y-Axis Display Control", padding=5)
        y_control_frame.pack(fill=tk.X, pady=(5, 0))
        
        # Y-axis scaling mode
        scale_frame = ttk.Frame(y_control_frame)
        scale_frame.pack(fill=tk.X, pady=(0, 5))
        
        ttk.Label(scale_frame, text="Scale Mode:").pack(side=tk.LEFT)
        ttk.Radiobutton(scale_frame, text="Auto", variable=self.y_scale_mode, 
                       value="auto", command=self.update_plot_settings).pack(side=tk.LEFT, padx=5)
        ttk.Radiobutton(scale_frame, text="Manual", variable=self.y_scale_mode, 
                       value="manual", command=self.update_plot_settings).pack(side=tk.LEFT, padx=5)
        ttk.Radiobutton(scale_frame, text="Log", variable=self.y_scale_mode, 
                       value="log", command=self.update_plot_settings).pack(side=tk.LEFT, padx=5)
        
        # Manual range setting
        range_frame = ttk.Frame(y_control_frame)
        range_frame.pack(fill=tk.X, pady=(0, 5))
        
        ttk.Label(range_frame, text="Y Range:").pack(side=tk.LEFT)
        ttk.Label(range_frame, text="Min:").pack(side=tk.LEFT, padx=(10, 0))
        self.y_min_entry = ttk.Entry(range_frame, textvariable=self.y_min, width=10)
        self.y_min_entry.pack(side=tk.LEFT, padx=5)
        self.y_min_entry.bind('<Return>', lambda e: self.update_plot_settings())
        
        ttk.Label(range_frame, text="Max:").pack(side=tk.LEFT, padx=(10, 0))
        self.y_max_entry = ttk.Entry(range_frame, textvariable=self.y_max, width=10)
        self.y_max_entry.pack(side=tk.LEFT, padx=5)
        self.y_max_entry.bind('<Return>', lambda e: self.update_plot_settings())
        
        # Unit and display options
        options_frame = ttk.Frame(y_control_frame)
        options_frame.pack(fill=tk.X)
        
        ttk.Label(options_frame, text="Unit:").pack(side=tk.LEFT)
        unit_combo = ttk.Combobox(options_frame, textvariable=self.y_unit, 
                                 values=["g", "mg", "ug"], width=8, state="readonly")
        unit_combo.pack(side=tk.LEFT, padx=5)
        unit_combo.bind('<<ComboboxSelected>>', lambda e: self.update_plot_settings())
        
        ttk.Checkbutton(options_frame, text="Show Grid", variable=self.show_grid,
                       command=self.update_plot_settings).pack(side=tk.LEFT, padx=(20, 5))
        ttk.Checkbutton(options_frame, text="Show Peak Labels", variable=self.show_peak_labels,
                       command=self.update_plot_settings).pack(side=tk.LEFT, padx=5)
        
        # Quick scale buttons
        quick_frame = ttk.Frame(y_control_frame)
        quick_frame.pack(fill=tk.X, pady=(5, 0))
        
        ttk.Label(quick_frame, text="Quick Scale:").pack(side=tk.LEFT)
        ttk.Button(quick_frame, text="Micro (0-1mg)", 
                  command=lambda: self.set_quick_scale(0, 0.001)).pack(side=tk.LEFT, padx=2)
        ttk.Button(quick_frame, text="Small (0-10mg)", 
                  command=lambda: self.set_quick_scale(0, 0.01)).pack(side=tk.LEFT, padx=2)
        ttk.Button(quick_frame, text="Medium (0-100mg)", 
                  command=lambda: self.set_quick_scale(0, 0.1)).pack(side=tk.LEFT, padx=2)
        ttk.Button(quick_frame, text="Large (0-1g)", 
                  command=lambda: self.set_quick_scale(0, 1.0)).pack(side=tk.LEFT, padx=2)
        
        # Plot frame
        plot_frame = ttk.LabelFrame(main_frame, text="Frequency Domain Analysis", padding=5)
        plot_frame.pack(fill=tk.BOTH, expand=True)
        
        self.plot_container = plot_frame
        
        # Status bar
        status_frame = ttk.Frame(main_frame)
        status_frame.pack(fill=tk.X, pady=(5, 0))
        
        self.info_label = ttk.Label(status_frame, text="Update Rate: 0.0 Hz | Data Count: 0 | Peak Freq: -- Hz | Peak Amp: -- | Max Value: --")
        self.info_label.pack(side=tk.LEFT)
        
        # Refresh port list
        self.refresh_ports()
        
    def setup_plot(self):
        """Setup plotting"""
        self.fig = Figure(figsize=(10, 6), dpi=100)
        self.ax = self.fig.add_subplot(111)
        
        self.canvas = FigureCanvasTkAgg(self.fig, self.plot_container)
        self.canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True)
        
        # Initialize empty plot
        self.ax.set_xlabel('Frequency (Hz)')
        self.ax.set_ylabel('Amplitude (g)')
        self.ax.set_title('Vibration Frequency Analysis - Real Data Display')
        self.ax.grid(True, alpha=0.3)
        self.ax.set_xlim(0, 500)
        self.ax.set_ylim(0, 1)
        
        self.canvas.draw()
        
    def refresh_ports(self):
        """Refresh serial port list"""
        ports = [port.device for port in serial.tools.list_ports.comports()]
        self.port_combo['values'] = ports
        if ports and not self.port_combo.get():
            self.port_combo.set(ports[0])
            
    def set_quick_scale(self, y_min, y_max):
        """Quick set Y-axis range"""
        self.y_scale_mode.set("manual")
        self.y_min.set(y_min)
        self.y_max.set(y_max)
        self.update_plot_settings()
        
    def update_plot_settings(self):
        """Update plot settings"""
        if hasattr(self, 'ax'):
            # Update grid
            self.ax.grid(self.show_grid.get(), alpha=0.3)
            
            # Update Y-axis label
            unit = self.y_unit.get()
            self.ax.set_ylabel(f'Amplitude ({unit})')
            
            # If there's data, redraw
            if hasattr(self, 'current_data') and self.current_data:
                self.update_plot(self.current_data)
            else:
                self.canvas.draw()
                
    def convert_units(self, data_g):
        """Convert units"""
        unit = self.y_unit.get()
        if unit == "mg":
            return np.array(data_g) * 1000
        elif unit == "ug":
            return np.array(data_g) * 1000000
        else:  # g
            return np.array(data_g)
            
    def toggle_connection(self):
        """Toggle connection state"""
        if self.running:
            self.disconnect()
        else:
            self.connect()
            
    def connect(self):
        """Connect to serial port"""
        port = self.port_combo.get()
        if not port:
            messagebox.showerror("Error", "Please select a port")
            return
            
        try:
            self.serial_conn = serial.Serial(port, 115200, timeout=1)
            self.running = True
            self.connect_btn.config(text="Disconnect")
            self.status_label.config(text="Connected", foreground="green")
            
            # Start data receiving thread
            self.receive_thread = threading.Thread(target=self.receive_data, daemon=True)
            self.receive_thread.start()
            
            # Start data processing
            self.process_data()
            
        except Exception as e:
            messagebox.showerror("Connection Error", f"Cannot connect to {port}: {str(e)}")
            
    def disconnect(self):
        """Disconnect"""
        self.running = False
        if self.serial_conn:
            self.serial_conn.close()
            self.serial_conn = None
        
        self.connect_btn.config(text="Connect")
        self.status_label.config(text="Disconnected", foreground="red")
        
    def receive_data(self):
        """Data receiving thread"""
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
                    print(f"Data receiving error: {e}")
                break
                
    def process_data(self):
        """Process data"""
        try:
            while not self.data_queue.empty():
                frame = self.data_queue.get_nowait()
                self.data_buffer.append(frame)
                self.frame_count += 1
                
                # Calculate update rate
                current_time = time.time()
                if current_time - self.last_update_time >= 1.0:
                    self.update_rate = self.frame_count / (current_time - self.last_update_time)
                    self.frame_count = 0
                    self.last_update_time = current_time
                
                # Update display
                self.update_plot(frame)
                
        except queue.Empty:
            pass
        
        if self.running:
            self.root.after(50, self.process_data)
            
    def update_plot(self, frame):
        """Update plot - display real data"""
        self.current_data = frame
        
        frequencies = frame['frequencies']
        magnitudes_g = frame['magnitudes']  # Original g values
        
        # Convert units
        magnitudes_display = self.convert_units(magnitudes_g)
        
        # Clear previous plot
        self.ax.clear()
        
        # Set basic properties
        unit = self.y_unit.get()
        self.ax.set_xlabel('Frequency (Hz)')
        self.ax.set_ylabel(f'Amplitude ({unit})')
        self.ax.set_title('Vibration Frequency Analysis - Real Data Display')
        
        if self.show_grid.get():
            self.ax.grid(True, alpha=0.3)
        
        # Draw spectrum
        if frame['data_points'] == 21:
            # 21-point bar chart
            self.ax.bar(frequencies, magnitudes_display, width=20, alpha=0.7, color='blue')
        else:
            # 257-point continuous curve
            self.ax.fill_between(frequencies, magnitudes_display, alpha=0.6, color='lightblue')
            self.ax.plot(frequencies, magnitudes_display, linewidth=1.5, color='blue')
        
        # Set X-axis range
        self.ax.set_xlim(0, max(frequencies))
        
        # Set Y-axis range
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
        
        # Peak detection and annotation
        if len(magnitudes_display) > 1:
            ac_magnitudes = magnitudes_display[1:]  # Exclude DC component
            ac_frequencies = frequencies[1:]
            
            if len(ac_magnitudes) > 0:
                max_idx = np.argmax(ac_magnitudes)
                peak_freq = ac_frequencies[max_idx]
                peak_mag = ac_magnitudes[max_idx]
                
                # Annotate peak
                if self.show_peak_labels.get():
                    self.ax.plot(peak_freq, peak_mag, 'ro', markersize=8)
                    self.ax.annotate(f'{peak_freq:.1f}Hz\n{peak_mag:.6f}{unit}', 
                                   xy=(peak_freq, peak_mag), 
                                   xytext=(10, 10), textcoords='offset points',
                                   bbox=dict(boxstyle='round,pad=0.3', facecolor='yellow', alpha=0.7),
                                   arrowprops=dict(arrowstyle='->', connectionstyle='arc3,rad=0'))
                
                # Update status info
                max_val = max(magnitudes_display)
                info_text = (f"Update Rate: {self.update_rate:.1f} Hz | "
                           f"Data Count: {len(self.data_buffer)} | "
                           f"Peak Freq: {peak_freq:.1f} Hz | "
                           f"Peak Amp: {peak_mag:.6f} {unit} | "
                           f"Max Value: {max_val:.6f} {unit}")
                self.info_label.config(text=info_text)
        
        self.canvas.draw()

def main():
    """Main function"""
    root = tk.Tk()
    app = ProfessionalVibrAnalyzer(root)
    
    def on_closing():
        app.disconnect()
        root.destroy()
    
    root.protocol("WM_DELETE_WINDOW", on_closing)
    root.mainloop()

if __name__ == '__main__':
    main()
