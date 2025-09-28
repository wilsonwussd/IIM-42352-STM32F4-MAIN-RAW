# 挖掘检测触发STM32报警功能说明

## 🎯 功能概述

在振动分析上位机软件中添加了自动触发STM32报警的功能。当检测到非法挖掘事件时，上位机会自动通过串口发送触发报警协议给STM32，STM32收到命令后会启动LoRa报警周期，将报警信号发送到云端。

## ✅ 实现的功能

### 1. 自动报警触发
- **粗检测触发**: 当粗检测模式检测到挖掘事件时，自动发送0x10命令给STM32
- **细检测触发**: 当细检测模式检测到挖掘事件时，自动发送0x10命令给STM32
- **两级检测触发**: 在两级检测模式下，细检测确认挖掘事件后触发报警

### 2. 用户控制界面
- **STM32报警开关**: 在检测控制面板中添加了"STM32报警"复选框
- **状态显示**: 开关状态变化会在控制台和日志中记录
- **默认启用**: 系统启动时默认启用STM32报警功能

### 3. 线程安全通信
- **串口保护**: 使用线程锁保护串口通信，避免冲突
- **错误处理**: 完善的异常处理和错误日志记录
- **状态检查**: 发送前检查串口连接状态和功能开关状态

## 🔧 代码修改详情

### 1. 添加的变量
```python
# 在VibrAnalyzer类的__init__方法中
self.stm32_alarm_enabled = tk.BooleanVar(value=True)  # STM32报警触发开关
```

### 2. 新增的函数
```python
def send_alarm_trigger_to_stm32(self):
    """发送触发报警命令给STM32"""
    # 检查功能开关
    if not self.stm32_alarm_enabled.get():
        return False
    
    # 线程安全的串口访问
    serial_conn = self.get_serial_connection()
    if serial_conn and serial_conn.is_open:
        command = bytes([0x10])  # 发送0x10命令
        serial_conn.write(command)
        return True
    return False

def on_stm32_alarm_changed(self):
    """STM32报警开关改变回调"""
    enabled = self.stm32_alarm_enabled.get()
    status = "启用" if enabled else "禁用"
    print(f"STM32报警触发功能{status}")
```

### 3. UI界面修改
```python
# 在setup_detection_controls函数中添加
ttk.Checkbutton(mode_frame, text="STM32报警", variable=self.stm32_alarm_enabled,
               command=self.on_stm32_alarm_changed).pack(side=tk.LEFT, padx=(10, 0))
```

### 4. 触发点集成
```python
# 在粗检测触发时
if self.detection_mode.get() == "粗检测":
    self.record_detection_result("mining", 0.7, "coarse_only")
    self.record_coarse_mining_event(log_data)
    self.send_alarm_trigger_to_stm32()  # 新增

# 在细检测触发时
if result['result'] == 'mining':
    self.detection_logger.log_mining_event(mining_event)
    self.event_recorder.record_mining_event(mining_event)
    self.send_alarm_trigger_to_stm32()  # 新增
```

## 🎛️ 使用说明

### 1. 启动系统
1. 确保STM32已连接并运行LoRa通信固件
2. 启动上位机软件: `python vibration_analyzer_chinese.py`
3. 连接到正确的串口（与STM32通信的串口）

### 2. 配置检测参数
1. **检测模式**: 选择"粗检测"、"细检测"或"两级检测"
2. **灵敏度**: 根据环境选择合适的灵敏度级别
3. **启用检测**: 确保检测功能已启用
4. **STM32报警**: 确保"STM32报警"复选框已勾选

### 3. 监控和测试
1. **实时监控**: 观察检测状态显示和日志输出
2. **手动测试**: 可以制造振动来测试检测功能
3. **日志查看**: 点击"查看日志"按钮查看详细检测记录

## 🧪 测试工具

### 测试脚本: `test_mining_detection_alarm.py`
```bash
python test_mining_detection_alarm.py
```

**功能**:
- 模拟STM32发送不同强度的振动数据
- 监控上位机发送的0x10报警触发命令
- 统计报警触发次数
- 验证整个检测→报警流程

**测试流程**:
1. 发送高强度振动数据（应该触发检测）
2. 发送中等强度振动数据（可能触发检测）
3. 发送低强度振动数据（不太可能触发）
4. 连续发送高强度数据（测试连续检测）

## 📊 工作流程

```
振动传感器数据 → 上位机接收 → 挖掘检测算法 → 检测到挖掘事件
                                                    ↓
云端接收报警 ← LoRa通信 ← STM32报警状态机 ← 0x10命令 ← 上位机发送
```

### 详细步骤
1. **数据采集**: STM32采集振动传感器数据并发送给上位机
2. **检测分析**: 上位机运行挖掘检测算法分析数据
3. **事件确认**: 检测算法确认发现挖掘事件
4. **命令发送**: 上位机发送0x10命令给STM32
5. **报警启动**: STM32收到命令后启动LoRa报警周期
6. **云端通信**: STM32通过LoRa将报警信号发送到云端

## ⚙️ 配置选项

### 检测模式说明
- **粗检测**: 仅使用时域特征，响应快但可能误报
- **细检测**: 仅使用频域特征，准确但计算量大
- **两级检测**: 先粗检测筛选，再细检测确认（推荐）

### 灵敏度级别
- **高**: 容易触发，适合安静环境
- **中等**: 平衡模式，适合大多数场景
- **低**: 不易触发，适合嘈杂环境

### STM32报警控制
- **启用**: 检测到挖掘事件时自动发送0x10命令
- **禁用**: 仅记录检测结果，不发送命令给STM32

## 🔍 故障排除

### 常见问题
1. **未收到报警命令**
   - 检查串口连接是否正常
   - 确认"STM32报警"选项已启用
   - 验证检测功能是否正常工作

2. **检测不触发**
   - 调整灵敏度设置
   - 检查振动数据是否正常接收
   - 查看检测参数配置

3. **STM32无响应**
   - 确认STM32固件支持0x10命令
   - 检查STM32串口通信设置
   - 验证LoRa模块连接状态

### 调试方法
1. **控制台输出**: 观察实时日志信息
2. **日志文件**: 查看详细的检测和通信记录
3. **测试工具**: 使用测试脚本验证功能
4. **手动命令**: 可以手动发送0x10命令测试STM32响应

## 📝 日志记录

系统会记录以下信息:
- 检测事件的详细数据
- STM32命令发送状态
- 串口通信错误
- 功能开关状态变化

日志文件位置: `detection_history.log`

## 🎉 总结

该功能实现了完整的"检测→报警→云端"自动化流程:
- ✅ 自动检测挖掘事件
- ✅ 自动触发STM32报警
- ✅ 用户可控制启用/禁用
- ✅ 完善的错误处理和日志记录
- ✅ 线程安全的串口通信
- ✅ 提供测试工具验证功能

这样就实现了您要求的功能：当上位机检测到非法挖掘事件时，自动通过调试串口发送触发报警协议给STM32。
