# STM32智慧地钉独立运行系统架构设计文档

## 📋 文档信息

| 项目名称 | STM32智慧地钉独立运行系统 |
|---------|------------------------|
| 文档版本 | v1.0 |
| 创建日期 | 2025-01-05 |
| 更新日期 | 2025-01-05 |
| 文档类型 | 系统架构设计 |
| 目标版本 | v4.0 (智能检测算法STM32移植版) |

## 🎯 项目概述

### 项目背景
智慧地钉产品从原有的"STM32数据采集+上位机分析"架构升级为"STM32独立智能检测"架构，实现完全自主的震动检测和分析能力，无需依赖外部计算设备。

### 核心目标
1. **完全独立运行**：STM32作为完整的智能检测终端，本地完成所有分析决策
2. **极致功耗优化**：分级功耗管理，平均功耗<5mA，电池寿命延长5倍以上
3. **实时智能检测**：本地运行两级检测算法，响应时间<3秒
4. **无线业务通信**：LoRa作为主要业务通道，支持远程监控和配置

### 技术指标要求
| 指标类型 | 目标值 | 备注 |
|---------|--------|------|
| 检测准确率 | ≥90% | 与baseline版本对比 |
| 响应时间 | <3秒 | 从震动开始到LoRa发送 |
| 平均功耗 | <5mA | 正常监测模式 |
| 误报率 | <5% | 实际部署环境 |
| 连续运行 | >30天 | 无重启稳定运行 |

## 🏗️ 系统架构设计

### 整体架构概览

```
┌─────────────────────────────────────────────────────────┐
│                   智慧地钉业务层                          │
│  ┌─────────────────┐  ┌─────────────────┐              │
│  │   挖掘检测业务   │  │   设备管理服务   │              │
│  │   逻辑控制      │  │   远程配置      │              │
│  └─────────────────┘  └─────────────────┘              │
├─────────────────────────────────────────────────────────┤
│                   智能检测算法层                          │
│  ┌─────────────────┐  ┌─────────────────┐              │
│  │   粗检测引擎     │  │   细检测引擎     │              │
│  │   (时域分析)    │  │   (频域分析)    │              │
│  │   低功耗运行    │  │   按需启动      │              │
│  └─────────────────┘  └─────────────────┘              │
├─────────────────────────────────────────────────────────┤
│                   数据处理引擎层                          │
│  ┌─────────────────┐  ┌─────────────────┐              │
│  │   数据预处理     │  │   FFT处理引擎   │              │
│  │   高通滤波      │  │   动态启停      │              │
│  └─────────────────┘  └─────────────────┘              │
├─────────────────────────────────────────────────────────┤
│                   系统控制管理层                          │
│  ┌─────────────────┐  ┌─────────────────┐              │
│  │   功耗管理器     │  │   主状态机      │              │
│  │   四级功耗      │  │   流程控制      │              │
│  └─────────────────┘  └─────────────────┘              │
├─────────────────────────────────────────────────────────┤
│                   通信服务层                              │
│  ┌─────────────────┐  ┌─────────────────┐              │
│  │   LoRa业务通信  │  │   调试串口      │              │
│  │   主通信通道    │  │   状态跟踪      │              │
│  └─────────────────┘  └─────────────────┘              │
├─────────────────────────────────────────────────────────┤
│                   硬件抽象层                              │
│  ┌─────────────────┐  ┌─────────────────┐              │
│  │   传感器驱动     │  │   外设驱动      │              │
│  │   IIM-42352     │  │   UART/SPI/I2C  │              │
│  └─────────────────┘  └─────────────────┘              │
└─────────────────────────────────────────────────────────┘
```

### 核心架构变化

| 架构层面 | 原架构 | 新架构 | 变化说明 |
|---------|--------|--------|----------|
| **系统定位** | 数据采集节点 | 智能检测终端 | 从中继到终端的转变 |
| **计算模式** | 数据传输+远程计算 | 本地智能计算 | 边缘智能化 |
| **功耗策略** | 持续高功耗运行 | 分级动态功耗 | 功耗优化70% |
| **通信角色** | 调试为主 | 业务为主 | LoRa成为主通道 |
| **处理方式** | 持续FFT处理 | 按需FFT处理 | 智能资源调度 |

## ⚡ 分级功耗管理系统

### 四级功耗状态设计

#### Level 0: 深度休眠模式
```c
typedef struct {
    uint32_t sleep_duration_ms;      // 休眠持续时间: 10-60秒
    uint8_t wakeup_source;           // 唤醒源: RTC定时器
    float32_t power_consumption;     // 功耗: <10μA
    uint8_t peripheral_state;        // 外设状态: 最小化
} deep_sleep_config_t;
```

**适用场景**：长期无震动活动，夜间或静止环境
**进入条件**：连续30分钟无任何震动检测
**唤醒条件**：RTC定时器唤醒或外部中断

#### Level 1: 监测模式
```c
typedef struct {
    uint32_t cpu_frequency_hz;       // CPU频率: 21MHz
    uint32_t sampling_rate_hz;       // 采样率: 1000Hz
    float32_t power_consumption;     // 功耗: ~2mA
    uint8_t active_peripherals;      // 活跃外设: 传感器+基础外设
} monitoring_config_t;
```

**适用场景**：日常监测状态，运行粗检测算法
**运行内容**：传感器采样、高通滤波、RMS计算、峰值因子分析
**状态转换**：检测到异常震动→Level 2

#### Level 2: 分析模式
```c
typedef struct {
    uint32_t cpu_frequency_hz;       // CPU频率: 84MHz
    uint32_t fft_buffer_size;        // FFT缓冲区: 512点
    float32_t power_consumption;     // 功耗: ~15mA
    uint32_t analysis_timeout_ms;    // 分析超时: 5秒
} analysis_config_t;
```

**适用场景**：震动分析状态，运行细检测算法
**运行内容**：FFT计算、频域特征提取、震动分类
**状态转换**：分析完成→Level 1或Level 3

#### Level 3: 通信模式
```c
typedef struct {
    uint32_t lora_tx_power_dbm;      // LoRa发射功率: 14dBm
    uint32_t transmission_timeout;   // 传输超时: 10秒
    float32_t power_consumption;     // 功耗: ~50mA
    uint8_t retry_count;             // 重试次数: 3次
} communication_config_t;
```

**适用场景**：检测结果上报，系统状态同步
**运行内容**：LoRa数据发送、ACK等待、重传机制
**状态转换**：发送完成→Level 1

### 动态功耗调度策略

```c
typedef struct {
    uint32_t level0_threshold_ms;    // 进入Level 0阈值: 1800000ms (30分钟)
    uint32_t level1_default_time;    // Level 1默认时间: 无限制
    uint32_t level2_max_time_ms;     // Level 2最大时间: 5000ms
    uint32_t level3_max_time_ms;     // Level 3最大时间: 10000ms
    
    // 功耗统计
    uint32_t total_runtime_hours;    // 总运行时间
    float32_t average_power_ma;      // 平均功耗
    uint32_t level_switch_count[4];  // 各级别切换次数
} power_management_t;
```

## 🧠 智能检测算法移植设计

### 数据预处理模块

#### 高通滤波器实现
```c
// IIR Butterworth 5Hz高通滤波器 (4阶)
typedef struct {
    // 滤波器系数 (预计算)
    float32_t b_coeffs[5];           // 分子系数
    float32_t a_coeffs[5];           // 分母系数
    
    // 滤波器状态 (Direct Form II)
    float32_t delay_line_x[4];       // X轴延迟线
    float32_t delay_line_y[4];       // Y轴延迟线
    float32_t delay_line_z[4];       // Z轴延迟线
    
    // 性能参数
    float32_t cutoff_frequency;      // 截止频率: 5.0Hz
    uint8_t filter_order;            // 滤波器阶数: 4
    float32_t sampling_rate;         // 采样率: 1000Hz
} highpass_filter_t;

// 滤波器初始化
void init_highpass_filter(highpass_filter_t* filter);

// 单点滤波处理
void apply_highpass_filter(highpass_filter_t* filter, 
                          float32_t input[3], 
                          float32_t output[3]);
```

#### 数据缓存管理
```c
typedef struct {
    // 原始数据缓冲区
    float32_t raw_buffer_x[256];     // X轴原始数据
    float32_t raw_buffer_y[256];     // Y轴原始数据
    float32_t raw_buffer_z[256];     // Z轴原始数据
    uint16_t buffer_write_index;     // 写入索引
    
    // 滤波后数据缓冲区
    float32_t filtered_buffer_x[256]; // X轴滤波数据
    float32_t filtered_buffer_y[256]; // Y轴滤波数据
    float32_t filtered_buffer_z[256]; // Z轴滤波数据
    
    // 缓冲区状态
    uint8_t buffer_full_flag;        // 缓冲区满标志
    uint32_t sample_count;           // 样本计数
} data_buffer_t;
```

### 粗检测引擎设计

#### 核心数据结构
```c
typedef struct {
    // RMS滑动窗口计算
    float32_t rms_window[200];       // 200ms滑动窗口
    uint16_t window_index;           // 窗口索引
    float32_t window_sum_squares;    // 平方和
    float32_t current_rms;           // 当前RMS值
    
    // 峰值因子计算
    float32_t peak_value;            // 当前峰值
    float32_t peak_factor;           // 峰值因子
    
    // 检测状态
    float32_t baseline_rms;          // 基线RMS: 0.003g
    float32_t trigger_threshold;     // 触发阈值
    uint32_t trigger_start_time;     // 触发开始时间
    uint32_t trigger_duration;       // 触发持续时间
    uint8_t trigger_state;           // 触发状态
    
    // 检测参数
    float32_t rms_multiplier;        // RMS倍数: 3.0
    float32_t peak_factor_threshold; // 峰值因子阈值: 2.0
    uint32_t duration_threshold_ms;  // 持续时间阈值: 2000ms
    uint32_t cooldown_time_ms;       // 冷却时间: 10000ms
    
    // 统计信息
    uint32_t total_triggers;         // 总触发次数
    uint32_t false_triggers;         // 误触发次数
} coarse_detector_t;
```

#### 核心算法实现
```c
// 粗检测主函数
detection_result_t coarse_detection_process(coarse_detector_t* detector, 
                                           float32_t accel_data[3]);

// RMS滑动窗口更新
void update_rms_window(coarse_detector_t* detector, float32_t magnitude);

// 峰值因子计算
float32_t calculate_peak_factor(coarse_detector_t* detector, 
                               float32_t current_sample);

// 触发状态管理
uint8_t manage_trigger_state(coarse_detector_t* detector);
```

### 细检测引擎设计

#### 核心数据结构
```c
typedef struct {
    // FFT处理
    uint8_t fft_engine_active;       // FFT引擎激活状态
    float32_t fft_input_buffer[512]; // FFT输入缓冲区
    float32_t fft_output_buffer[514];// FFT输出缓冲区 (复数)
    float32_t magnitude_spectrum[257]; // 幅度谱
    
    // 频域特征
    float32_t low_freq_energy;       // 低频能量 (5-15Hz)
    float32_t mid_freq_energy;       // 中频能量 (15-30Hz)
    float32_t high_freq_energy;      // 高频能量 (30-100Hz)
    float32_t dominant_frequency;    // 主频
    float32_t spectral_centroid;     // 频谱重心
    float32_t energy_concentration;  // 能量集中度
    
    // 分类规则参数
    float32_t low_freq_threshold;    // 低频阈值: 0.4
    float32_t mid_freq_threshold;    // 中频阈值: 0.2
    float32_t dominant_freq_max;     // 主频上限: 50Hz
    float32_t centroid_max;          // 重心上限: 80Hz
    float32_t concentration_min;     // 集中度下限: 0.3
    
    // 决策输出
    float32_t confidence_score;      // 置信度分数
    uint8_t classification_result;   // 分类结果: 0=正常, 1=挖掘
    uint32_t analysis_timestamp;     // 分析时间戳
    
    // 性能统计
    uint32_t fft_computation_time_us; // FFT计算时间
    uint32_t feature_extraction_time_us; // 特征提取时间
    uint32_t total_analysis_time_us;  // 总分析时间
} fine_detector_t;
```

#### 特征提取算法
```c
// 频域特征提取主函数
void extract_frequency_features(fine_detector_t* detector);

// 能量分布计算
void calculate_energy_distribution(fine_detector_t* detector);

// 主频检测
float32_t find_dominant_frequency(float32_t* spectrum, uint16_t length);

// 频谱重心计算
float32_t calculate_spectral_centroid(float32_t* spectrum, 
                                     float32_t* frequencies, 
                                     uint16_t length);

// 能量集中度计算
float32_t calculate_energy_concentration(float32_t* spectrum, uint16_t length);
```

#### 分类决策算法
```c
// 简化规则分类器
typedef struct {
    float32_t rule_weights[5];       // 规则权重
    float32_t confidence_threshold;  // 置信度阈值: 0.7
} rule_classifier_t;

// 震动分类主函数
uint8_t classify_vibration(fine_detector_t* detector, 
                          rule_classifier_t* classifier);

// 置信度计算
float32_t calculate_confidence_score(fine_detector_t* detector,
                                    rule_classifier_t* classifier);
```

## 🔄 主控状态机设计

### 状态定义
```c
typedef enum {
    STATE_SYSTEM_INIT = 0,           // 系统初始化
    STATE_IDLE_SLEEP,                // 深度休眠
    STATE_MONITORING,                // 监测模式
    STATE_COARSE_TRIGGERED,          // 粗检测触发
    STATE_FINE_ANALYSIS,             // 细检测分析
    STATE_MINING_DETECTED,           // 挖掘检测
    STATE_ALARM_SENDING,             // 报警发送
    STATE_ALARM_COMPLETE,            // 报警完成
    STATE_ERROR_HANDLING,            // 错误处理
    STATE_SYSTEM_RESET               // 系统重置
} system_state_t;
```

### 状态转换逻辑
```c
typedef struct {
    system_state_t current_state;    // 当前状态
    system_state_t previous_state;   // 前一状态
    uint32_t state_enter_time;       // 状态进入时间
    uint32_t state_duration;         // 状态持续时间
    
    // 状态转换条件
    uint8_t coarse_trigger_flag;     // 粗检测触发标志
    uint8_t fine_analysis_result;    // 细检测结果
    uint8_t alarm_send_status;       // 报警发送状态
    uint8_t error_code;              // 错误代码
    
    // 状态统计
    uint32_t state_count[10];        // 各状态计数
    uint32_t transition_count;       // 状态转换计数
} state_machine_t;
```

### 状态处理函数
```c
// 主状态机处理函数
void system_state_machine_process(state_machine_t* sm);

// 各状态处理函数
void handle_system_init(state_machine_t* sm);
void handle_idle_sleep(state_machine_t* sm);
void handle_monitoring(state_machine_t* sm);
void handle_coarse_triggered(state_machine_t* sm);
void handle_fine_analysis(state_machine_t* sm);
void handle_mining_detected(state_machine_t* sm);
void handle_alarm_sending(state_machine_t* sm);
void handle_alarm_complete(state_machine_t* sm);
void handle_error_handling(state_machine_t* sm);

// 状态转换函数
void transition_to_state(state_machine_t* sm, system_state_t new_state);
```

## 📡 通信系统重新设计

### LoRa业务通信协议

#### 数据包格式设计
```c
// LoRa业务数据包结构
typedef struct {
    // 包头 (8字节)
    uint16_t sync_word;              // 同步字: 0xABCD
    uint8_t packet_type;             // 包类型: 0x01=检测结果, 0x02=状态上报
    uint8_t device_id[4];            // 设备ID
    uint8_t sequence_number;         // 序列号
    
    // 载荷 (可变长度)
    union {
        detection_report_t detection; // 检测结果报告
        status_report_t status;       // 状态报告
        config_request_t config;      // 配置请求
    } payload;
    
    // 包尾 (2字节)
    uint16_t crc16;                  // CRC16校验
} lora_packet_t;
```

#### 检测结果报告
```c
typedef struct {
    uint32_t timestamp;              // 时间戳
    uint8_t vibration_type;          // 震动类型: 0=正常, 1=疑似, 2=确认
    float32_t confidence_score;      // 置信度: 0.0-1.0
    float32_t trigger_intensity;     // 触发强度 (RMS值)
    float32_t dominant_frequency;    // 主频
    float32_t duration_seconds;      // 持续时间
    
    // 位置信息 (可选)
    float32_t gps_latitude;          // GPS纬度
    float32_t gps_longitude;         // GPS经度
    
    // 环境信息
    float32_t temperature;           // 温度
    float32_t battery_voltage;       // 电池电压
} detection_report_t;
```

#### 系统状态报告
```c
typedef struct {
    uint32_t uptime_hours;           // 运行时间 (小时)
    float32_t battery_percentage;    // 电池电量百分比
    float32_t average_power_ma;      // 平均功耗
    uint8_t current_power_level;     // 当前功耗级别
    
    // 检测统计
    uint32_t total_detections;       // 总检测次数
    uint32_t false_positives;        // 误报次数
    uint32_t true_positives;         // 正确检测次数
    float32_t detection_accuracy;    // 检测准确率
    
    // 系统健康
    uint8_t system_health_score;     // 系统健康分数 (0-100)
    uint8_t error_count;             // 错误计数
    uint32_t last_reset_reason;      // 上次重启原因
} status_report_t;
```

### 调试串口协议

#### 命令格式
```c
// 串口命令格式
typedef struct {
    uint8_t command_id;              // 命令ID
    uint8_t parameter_count;         // 参数个数
    float32_t parameters[8];         // 参数数组
    uint8_t checksum;                // 校验和
} uart_command_t;

// 命令ID定义
#define CMD_GET_STATUS          0x01  // 获取系统状态
#define CMD_SET_THRESHOLD       0x02  // 设置检测阈值
#define CMD_GET_STATISTICS      0x03  // 获取统计信息
#define CMD_RESET_SYSTEM        0x04  // 系统重置
#define CMD_ENTER_TEST_MODE     0x05  // 进入测试模式
#define CMD_SET_POWER_LEVEL     0x06  // 设置功耗级别
#define CMD_CALIBRATE_SENSOR    0x07  // 传感器校准
#define CMD_UPDATE_FIRMWARE     0x08  // 固件升级
```

#### 响应格式
```c
typedef struct {
    uint8_t response_id;             // 响应ID
    uint8_t status_code;             // 状态码: 0=成功, 其他=错误
    uint8_t data_length;             // 数据长度
    uint8_t data[64];                // 响应数据
    uint8_t checksum;                // 校验和
} uart_response_t;
```

## 💾 内存管理和优化

### 内存布局设计
```c
// 系统内存布局
typedef struct {
    // 静态分配区域 (总计: ~8KB)
    coarse_detector_t coarse_detector;        // 粗检测器: ~1KB
    highpass_filter_t highpass_filter;       // 高通滤波器: ~200B
    data_buffer_t data_buffer;               // 数据缓冲区: ~4KB
    state_machine_t state_machine;           // 状态机: ~200B
    power_management_t power_manager;        // 功耗管理: ~200B
    
    // 动态分配区域 (按需分配)
    fine_detector_t* fine_detector;          // 细检测器: ~3KB
    
    // 共享缓冲区 (复用)
    union {
        float32_t fft_temp_buffer[512];      // FFT临时缓冲
        float32_t feature_temp_buffer[256];  // 特征计算缓冲
        uint8_t comm_temp_buffer[1024];      // 通信临时缓冲
    } shared_buffer;                         // 共享缓冲区: ~2KB
    
    // 系统配置区域
    system_config_t system_config;          // 系统配置: ~500B
    calibration_data_t calibration_data;    // 校准数据: ~300B
} system_memory_t;
```

### 内存优化策略
```c
// 内存池管理
typedef struct {
    uint8_t* memory_pool;            // 内存池起始地址
    uint32_t pool_size;              // 内存池大小
    uint32_t allocated_size;         // 已分配大小
    uint32_t free_size;              // 剩余大小
    
    // 分配记录
    struct {
        void* address;               // 分配地址
        uint32_t size;               // 分配大小
        uint8_t in_use;              // 使用标志
    } allocation_table[16];          // 分配表
} memory_pool_t;

// 内存管理函数
void* smart_malloc(memory_pool_t* pool, uint32_t size);
void smart_free(memory_pool_t* pool, void* ptr);
uint32_t get_memory_usage(memory_pool_t* pool);
void defragment_memory(memory_pool_t* pool);
```

## 🔧 开发实施计划

### 基于现有代码的渐进式开发路线图

> **开发策略**: 基于现有STM32代码进行渐进式重构，保持系统稳定性，复用已验证的代码模块

#### 现有代码基础分析

**可复用的核心模块**:
- ✅ **硬件抽象层**: 84MHz时钟配置、SPI/UART初始化已完善
- ✅ **传感器驱动**: IIM-42352驱动稳定，1000Hz采样可靠
- ✅ **FFT处理引擎**: 512点FFT优化完成，性能良好
- ✅ **通信框架**: 自定义协议、LoRa报警状态机已实现
- ✅ **中断处理**: 数据采集中断机制成熟稳定

**需要新增的功能模块**:
- 🔄 **数据预处理**: 高通滤波器集成到现有数据流
- 🆕 **粗检测算法**: RMS计算、触发逻辑
- 🔄 **智能FFT控制**: 从自动处理改为按需触发
- 🆕 **细检测算法**: 频域特征提取、分类决策
- 🆕 **功耗管理**: 分级功耗控制系统
- 🔄 **主控状态机**: 重构现有主循环逻辑

#### 阶段1: 数据预处理模块集成 (1周)
**基于现有**: `HandleInvDeviceFifoPacket()` 函数和数据处理流程
```c
// 现有代码位置: Core/Src/example-raw-data.c:180-250
void HandleInvDeviceFifoPacket(inv_iim423xx_sensor_event_t * event)
{
    // 现有: 数据格式转换和坐标变换
    // 新增: 高通滤波器处理
    // 保持: FFT_AddSample()调用和原始数据发送
}
```

**开发任务**:
- [ ] 创建 `highpass_filter.c/.h` 模块
- [ ] 在现有数据处理回调中集成滤波器
- [ ] 实现IIR Butterworth 5Hz高通滤波器
- [ ] 保持现有数据输出，添加滤波后数据对比
- [ ] 验证滤波器频率响应和相位特性

**验证标准**:
- 滤波器截止频率准确性: ±0.1Hz
- 相位延迟: <10ms
- 数据完整性: 无丢包，时序正确
- 与MATLAB/Python滤波结果一致性: >99%

#### 阶段2: 粗检测算法集成 (1.5周)
**基于现有**: 数据缓冲机制和 `raw_data_counter` 计数器
```c
// 现有代码位置: Core/Src/example-raw-data.c:233-240
static uint32_t raw_data_counter = 0;
raw_data_counter++;
if (raw_data_counter >= 100) {  // 现有10Hz原始数据发送逻辑
    // 新增: 粗检测算法调用
    // 保持: 原始数据发送
}
```

**开发任务**:
- [ ] 创建 `coarse_detector.c/.h` 模块
- [ ] 实现RMS滑动窗口算法 (200ms窗口)
- [ ] 实现峰值因子计算和触发判断逻辑
- [ ] 集成到现有数据处理流程
- [ ] 添加触发状态标志，影响后续FFT处理

**验证标准**:
- RMS计算精度: 与上位机算法误差<1%
- 触发响应时间: <100ms
- 误触发率: <2% (静态环境测试)
- 检测灵敏度: 与baseline版本一致性>95%

#### 阶段3: 智能FFT控制重构 (1周)
**基于现有**: `FFT_Process()` 函数和自动处理机制
```c
// 现有代码位置: Core/Src/fft_processor.c:107-189
int FFT_Process(void)
{
    // 现有: FFT计算和结果处理
    // 修改: 添加触发条件检查
    // 保持: FFT核心算法不变
    // 新增: 按需启动控制逻辑
}
```

**开发任务**:
- [ ] 修改FFT自动处理逻辑为触发式处理
- [ ] 在 `fft_processor.c` 中添加触发控制接口
- [ ] 保持FFT核心算法和性能不变
- [ ] 修改 `FFT_SendFullSpectrumViaProtocol()` 调用条件
- [ ] 添加FFT引擎休眠/唤醒机制

**验证标准**:
- FFT计算精度: 与原版本完全一致
- 触发响应: 粗检测触发后<50ms启动FFT
- 功耗降低: 空闲时FFT模块功耗<1mA
- 处理时间: FFT计算时间保持~36μs

#### 阶段4: 细检测算法开发 (2周)
**基于现有**: FFT输出数据和协议发送机制
```c
// 现有代码位置: Core/Src/fft_processor.c:172-180
// 在FFT_Process()函数末尾添加
if (fft_processor.state == FFT_STATE_COMPLETE) {
    // 现有: FFT_SendFullSpectrumViaProtocol()
    // 新增: 细检测特征提取和分类
    // 新增: 决策结果输出
}
```

**开发任务**:
- [ ] 创建 `fine_detector.c/.h` 模块
- [ ] 实现频域特征提取算法 (5维特征)
- [ ] 实现简化规则分类器
- [ ] 集成到现有FFT处理流程
- [ ] 添加置信度计算和决策输出

**验证标准**:
- 特征提取精度: 与上位机算法误差<2%
- 分类准确率: ≥90% (与baseline对比)
- 处理时间: 特征提取+分类<200μs
- 内存使用: 新增内存<2KB

#### 阶段5: 主控状态机重构 (1.5周)
**基于现有**: `main.c` 主循环和中断处理机制
```c
// 现有代码位置: Core/Src/main.c:274-328
do {
    // 现有: 中断处理和命令处理
    // 重构: 添加智能检测状态机
    // 保持: 中断驱动机制不变
    // 集成: Process_Alarm_State_Machine()
} while(1);
```

**开发任务**:
- [ ] 创建 `system_state_machine.c/.h` 模块
- [ ] 重构主循环，集成智能检测流程
- [ ] 保持现有中断处理机制不变
- [ ] 集成现有的 `Process_Alarm_State_Machine()`
- [ ] 添加状态转换逻辑和错误处理

**验证标准**:
- 状态转换时间: <50μs
- 系统响应性: 中断响应时间不变
- 稳定性: 连续运行>24小时无异常
- 兼容性: 保持现有命令处理功能

#### 阶段6: 功耗管理和通信优化 (1.5周)
**基于现有**: `SystemClock_Config()` 和通信协议
```c
// 现有代码位置: Core/Src/main.c:343-400
void SystemClock_Config(void)
{
    // 现有: 84MHz时钟配置
    // 新增: 动态时钟调节支持
    // 保持: 基础时钟配置不变
}
```

**开发任务**:
- [ ] 创建 `power_manager.c/.h` 模块
- [ ] 实现分级功耗管理 (4级功耗状态)
- [ ] 修改现有时钟配置支持动态调频
- [ ] 优化现有通信协议，减少不必要数据发送
- [ ] 调整LoRa报警逻辑，支持业务数据上报

**验证标准**:
- 功耗降低: 平均功耗<5mA (相比现有版本)
- 时钟切换时间: <1ms
- 通信可靠性: 保持现有通信成功率
- 响应时间: 满足<3秒检测响应要求

#### 阶段7: 系统集成和优化 (1.5周)
**基于现有**: 完整系统测试和验证框架

**开发任务**:
- [ ] 整体系统集成测试
- [ ] 与baseline版本性能对比验证
- [ ] 长期稳定性测试 (>72小时)
- [ ] 实际震动环境测试
- [ ] 系统参数调优和错误处理完善

**验证标准**:
- 检测准确率: ≥90% (与baseline对比)
- 功耗优化: 平均功耗<5mA
- 响应时间: <3秒 (震动到LoRa发送)
- 稳定性: 连续运行>30天无重启
- 兼容性: 支持现有上位机调试功能

### 关键里程碑验证 (基于现有代码基础)

#### 里程碑1: 数据预处理验证 (第1周)
- **验证目标**: 高通滤波器成功集成到现有数据流
- **验证标准**:
  - 滤波器频率响应准确性: 5Hz截止频率±0.1Hz
  - 与现有数据流兼容性: 无数据丢失，时序正确
  - 滤波效果验证: 与MATLAB/Python结果一致性>99%
- **验证方法**:
  - 频率扫描测试 (1-100Hz正弦波输入)
  - 与上位机滤波结果对比验证
  - 现有FFT处理流程完整性测试
- **风险控制**: 保持原始数据输出，可随时对比验证

#### 里程碑2: 粗检测算法验证 (第2.5周)
- **验证目标**: 粗检测算法成功集成，触发逻辑正确
- **验证标准**:
  - RMS计算精度: 与上位机算法误差<1%
  - 触发响应时间: <100ms
  - 静态环境误触发率: <2%
  - 与baseline版本一致性: >95%
- **验证方法**:
  - 标准震动信号测试 (已知RMS值)
  - 静态环境长期监测 (24小时)
  - 与现有上位机粗检测结果对比
- **风险控制**: 使用条件编译开关，可禁用粗检测功能

#### 里程碑3: 智能FFT控制验证 (第3.5周)
- **验证目标**: FFT按需处理机制工作正常，性能保持
- **验证标准**:
  - FFT计算精度: 与原版本完全一致
  - 触发响应延迟: 粗检测触发后<50ms启动FFT
  - 空闲功耗降低: FFT模块空闲时<1mA
  - FFT处理时间: 保持~36μs不变
- **验证方法**:
  - FFT结果精度对比测试
  - 功耗测试仪实时监测
  - 触发响应时间测量
- **风险控制**: 保持原有自动FFT模式作为备选

#### 里程碑4: 细检测算法验证 (第5.5周)
- **验证目标**: 细检测算法移植成功，分类准确率达标
- **验证标准**:
  - 特征提取精度: 与上位机算法误差<2%
  - 分类准确率: ≥90% (与baseline版本对比)
  - 处理时间: 特征提取+分类<200μs
  - 内存占用: 新增内存<2KB
- **验证方法**:
  - 标准震动样本库测试 (>1000个样本)
  - 实际挖掘环境测试
  - 与上位机细检测结果对比
- **风险控制**: 分步实现特征提取，逐个验证

#### 里程碑5: 系统集成验证 (第7周)
- **验证目标**: 完整智能检测流程工作正常，状态机稳定
- **验证标准**:
  - 状态转换时间: <50μs
  - 系统响应性: 中断响应时间不变
  - 检测流程完整性: 粗检测→细检测→决策输出
  - 连续运行稳定性: >24小时无异常
- **验证方法**:
  - 完整检测流程端到端测试
  - 长期稳定性测试
  - 异常情况恢复测试
- **风险控制**: 保持现有中断处理机制不变

#### 里程碑6: 功耗优化验证 (第8.5周)
- **验证目标**: 分级功耗管理生效，整体功耗显著降低
- **验证标准**:
  - 平均功耗: <5mA (相比现有版本降低>70%)
  - 时钟切换时间: <1ms
  - 各功耗级别切换正常: Level 0-3正常工作
  - 检测响应时间: 仍满足<3秒要求
- **验证方法**:
  - 功耗测试仪长期监测
  - 各功耗级别单独测试
  - 动态功耗切换测试
- **风险控制**: 保持84MHz固定频率作为备选模式

#### 里程碑7: 最终产品验证 (第9周)
- **验证目标**: 产品级稳定性，可部署使用
- **验证标准**:
  - 检测准确率: ≥90% (与baseline版本对比)
  - 平均功耗: <5mA
  - 响应时间: <3秒 (震动开始到LoRa发送)
  - 长期稳定性: 连续运行>72小时无重启
  - 通信成功率: >99%
- **验证方法**:
  - 实际部署环境测试
  - 与baseline版本全面对比
  - 长期稳定性压力测试
  - 用户验收测试
- **交付标准**: 通过所有里程碑验证，性能指标达标

### 风险评估和应对策略 (基于现有代码基础)

#### 技术风险评估
| 风险项 | 风险等级 | 影响 | 现有代码缓解因素 | 应对策略 |
|-------|---------|------|------------------|----------|
| 算法移植精度损失 | 中 (降低) | 检测准确率下降 | 现有FFT引擎已优化验证 | 分步验证，保持原算法核心不变 |
| 功耗优化效果不佳 | 中 | 电池寿命不达标 | 84MHz配置已是功耗优化版本 | 基于现有配置进一步优化 |
| 现有系统稳定性影响 | 高 (新增) | 破坏现有功能 | 现有系统已稳定运行 | 渐进式重构，保持向后兼容 |
| 实时性能不足 | 低 (降低) | 响应时间超标 | 现有中断机制高效稳定 | 保持现有中断处理不变 |
| 内存资源不足 | 低 | 系统不稳定 | 现有内存使用已优化 | 复用现有缓冲区，动态分配 |

#### 进度风险评估
| 风险项 | 风险等级 | 影响 | 现有代码优势 | 应对策略 |
|-------|---------|------|---------------|----------|
| 硬件抽象层开发 | 无 | - | 硬件驱动已完善 | 直接复用，无需重新开发 |
| FFT算法实现 | 无 | - | FFT引擎已优化完成 | 只需修改触发逻辑 |
| 通信协议开发 | 低 (降低) | 协议兼容性问题 | 协议框架已建立 | 在现有协议基础上扩展 |
| 集成测试复杂 | 低 (降低) | 系统不稳定 | 现有系统架构清晰 | 分模块集成，保持现有测试方法 |
| 调试环境搭建 | 无 | - | 调试环境已成熟 | 直接使用现有调试工具 |

#### 基于现有代码的风险控制策略

**1. 渐进式开发策略**
```c
// 使用条件编译控制新功能
#define ENABLE_INTELLIGENT_DETECTION  1
#define ENABLE_POWER_MANAGEMENT       1
#define ENABLE_COARSE_DETECTION       1
#define ENABLE_FINE_DETECTION         1

#if ENABLE_INTELLIGENT_DETECTION
    // 新增智能检测代码
#else
    // 保持原有处理逻辑
#endif
```

**2. 向后兼容保证**
- 保持现有UART命令处理功能
- 保持现有数据输出格式
- 保持现有LoRa报警机制
- 保持现有调试接口

**3. 分支开发管理**
```bash
# 主分支保持稳定
git checkout master  # 现有稳定版本

# 创建开发分支
git checkout -b intelligent-detection-dev

# 各阶段创建子分支
git checkout -b stage1-data-preprocessing
git checkout -b stage2-coarse-detection
# ... 其他阶段分支
```

**4. 实时回退机制**
- 每个阶段完成后合并到开发分支
- 保持主分支随时可用
- 关键节点创建稳定版本标签
- 支持快速回退到任意稳定版本

**5. 质量保证措施**
- 复用现有测试用例和测试方法
- 每个模块开发完成后立即集成测试
- 保持现有性能基准不降低
- 新功能必须通过现有系统验证

#### 特殊风险应对

**风险: 现有系统功能破坏**
- **检测方法**: 每次修改后运行完整回归测试
- **应对策略**:
  - 修改前备份关键函数
  - 使用函数指针支持动态切换
  - 保持原有函数接口不变
  - 新增功能通过参数控制启用/禁用

**风险: 性能回归**
- **检测方法**: 每个阶段完成后性能基准测试
- **应对策略**:
  - 保持现有FFT处理时间不变
  - 保持现有中断响应时间不变
  - 新增功能的性能开销<10%
  - 关键路径代码优化

**风险: 内存使用超标**
- **检测方法**: 实时内存使用监控
- **应对策略**:
  - 复用现有缓冲区结构
  - 使用内存池管理动态分配
  - 按需分配细检测模块内存
  - 内存使用增长<20%

#### 应急预案

**预案1: 算法性能不达标**
- 回退到现有FFT持续处理模式
- 保持上位机分析架构
- 仅实现功耗优化部分

**预案2: 开发进度严重延期**
- 优先实现核心功能 (粗检测+细检测)
- 暂缓功耗管理功能
- 保持现有通信协议不变

**预案3: 系统稳定性问题**
- 立即回退到最近稳定版本
- 分析问题根因
- 单独修复问题模块后重新集成

通过基于现有代码的渐进式开发策略，技术风险和进度风险都得到了显著降低，项目成功率大幅提升。

## 📊 性能指标和验收标准

### 功能性指标
| 指标名称 | 目标值 | 测试方法 | 验收标准 |
|---------|--------|----------|----------|
| 检测准确率 | ≥90% | 标准样本测试 | 与baseline对比 |
| 误报率 | <5% | 长期运行测试 | 实际环境验证 |
| 响应时间 | <3秒 | 实时性能测试 | 从震动到LoRa发送 |
| 检测范围 | 0.001g-4g | 动态范围测试 | 覆盖全量程 |

### 性能指标
| 指标名称 | 目标值 | 测试方法 | 验收标准 |
|---------|--------|----------|----------|
| 平均功耗 | <5mA | 功耗测试仪 | 监测模式下测量 |
| 峰值功耗 | <60mA | 功耗测试仪 | 通信模式下测量 |
| 内存使用 | <16KB | 内存分析工具 | 峰值内存占用 |
| CPU使用率 | <80% | 性能分析工具 | 分析模式下测量 |

### 可靠性指标
| 指标名称 | 目标值 | 测试方法 | 验收标准 |
|---------|--------|----------|----------|
| 连续运行时间 | >30天 | 长期稳定性测试 | 无重启运行 |
| 通信成功率 | >99% | 通信压力测试 | LoRa发送成功率 |
| 错误恢复时间 | <10秒 | 故障注入测试 | 自动恢复能力 |
| 温度适应性 | -20°C~+70°C | 环境测试 | 全温度范围工作 |

## 🔍 详细技术规格

### 硬件资源需求分析

#### STM32F407VGT6资源评估
```c
// 硬件资源需求
#define FLASH_REQUIREMENT_KB        256     // Flash需求: 256KB
#define RAM_REQUIREMENT_KB          64      // RAM需求: 64KB
#define CPU_FREQUENCY_MAX_MHZ       84      // 最大CPU频率
#define CPU_FREQUENCY_MIN_MHZ       21      // 最小CPU频率

// 外设资源需求
#define REQUIRED_TIMERS             4       // 定时器需求
#define REQUIRED_UART_CHANNELS      2       // UART通道需求
#define REQUIRED_SPI_CHANNELS       1       // SPI通道需求
#define REQUIRED_ADC_CHANNELS       2       // ADC通道需求 (电池监测)
#define REQUIRED_GPIO_PINS          12      // GPIO引脚需求
```

#### 传感器接口规格
```c
// IIM-42352传感器配置
typedef struct {
    // SPI接口配置
    uint32_t spi_frequency_hz;      // SPI频率: 1MHz
    uint8_t spi_mode;               // SPI模式: Mode 0
    uint8_t cs_pin;                 // 片选引脚

    // 传感器配置
    uint8_t accel_range;            // 加速度量程: ±4g
    uint16_t sample_rate_hz;        // 采样率: 1000Hz
    uint8_t filter_config;          // 滤波器配置: AAF优化
    uint8_t power_mode;             // 功耗模式: 低噪声模式

    // 中断配置
    uint8_t int1_pin;               // 中断1引脚
    uint8_t int2_pin;               // 中断2引脚
    uint8_t int_config;             // 中断配置
} sensor_config_t;
```

### 算法参数详细配置

#### 高通滤波器系数
```c
// Butterworth 5Hz高通滤波器系数 (4阶, Fs=1000Hz)
static const float32_t HIGHPASS_B_COEFFS[5] = {
    0.9565436765f,   // b0
    -3.8261747058f,  // b1
    5.7392620587f,   // b2
    -3.8261747058f,  // b3
    0.9565436765f    // b4
};

static const float32_t HIGHPASS_A_COEFFS[5] = {
    1.0000000000f,   // a0
    -3.8371173094f,  // a1
    5.7743740112f,   // a2
    -3.9375833143f,  // a3
    0.9565436765f    // a4
};

// 滤波器性能参数
#define FILTER_CUTOFF_HZ            5.0f    // 截止频率
#define FILTER_STOPBAND_ATTEN_DB    40.0f   // 阻带衰减
#define FILTER_PASSBAND_RIPPLE_DB   0.1f    // 通带纹波
#define FILTER_TRANSITION_WIDTH_HZ  2.0f    // 过渡带宽度
```

#### 粗检测算法参数
```c
// 粗检测参数配置表
typedef struct {
    // 基础参数
    float32_t baseline_rms_g;           // 基线RMS: 0.003g
    float32_t rms_multiplier;           // RMS倍数: 3.0
    float32_t peak_factor_threshold;    // 峰值因子阈值: 2.0
    uint32_t duration_threshold_ms;     // 持续时间阈值: 2000ms
    uint32_t cooldown_time_ms;          // 冷却时间: 10000ms

    // 高级参数
    uint16_t rms_window_size;           // RMS窗口大小: 200
    float32_t peak_decay_factor;        // 峰值衰减因子: 0.95
    uint32_t trigger_confirm_time_ms;   // 触发确认时间: 500ms
    float32_t noise_floor_g;            // 噪声底限: 0.0005g

    // 自适应参数
    uint8_t adaptive_enabled;           // 自适应使能: 0
    float32_t adaptation_rate;          // 自适应速率: 0.01
    uint32_t adaptation_window_ms;      // 自适应窗口: 300000ms (5分钟)
} coarse_detection_params_t;
```

#### 细检测算法参数
```c
// 细检测参数配置表
typedef struct {
    // FFT参数
    uint16_t fft_size;                  // FFT大小: 512
    uint16_t fft_overlap;               // FFT重叠: 256 (50%)
    uint8_t window_type;                // 窗函数类型: Hanning

    // 频带定义 (Hz)
    float32_t freq_bands[6];            // 频带边界: [0, 5, 15, 30, 100, 500]

    // 特征阈值
    float32_t low_freq_energy_threshold;    // 低频能量阈值: 0.4
    float32_t mid_freq_energy_threshold;    // 中频能量阈值: 0.2
    float32_t dominant_freq_max_hz;         // 主频上限: 50Hz
    float32_t spectral_centroid_max_hz;     // 频谱重心上限: 80Hz
    float32_t energy_concentration_min;     // 能量集中度下限: 0.3

    // 分类参数
    float32_t feature_weights[5];           // 特征权重
    float32_t confidence_threshold;         // 置信度阈值: 0.7
    uint8_t min_consistent_results;         // 最小一致结果数: 2
    uint32_t analysis_timeout_ms;           // 分析超时: 5000ms
} fine_detection_params_t;
```

### 通信协议详细定义

#### LoRa物理层参数
```c
// LoRa调制参数
typedef struct {
    uint32_t frequency_hz;              // 工作频率: 470MHz
    uint8_t spreading_factor;           // 扩频因子: SF7
    uint8_t bandwidth;                  // 带宽: 125kHz
    uint8_t coding_rate;                // 编码率: 4/5
    int8_t tx_power_dbm;                // 发射功率: 14dBm
    uint16_t preamble_length;           // 前导码长度: 8
    uint8_t sync_word;                  // 同步字: 0x12
    uint8_t crc_enabled;                // CRC使能: 1
} lora_phy_config_t;
```

#### 应用层协议栈
```c
// 协议栈层次定义
typedef enum {
    PROTOCOL_LAYER_PHY = 0,             // 物理层: LoRa调制
    PROTOCOL_LAYER_MAC,                 // MAC层: 帧格式
    PROTOCOL_LAYER_NET,                 // 网络层: 路由寻址
    PROTOCOL_LAYER_APP                  // 应用层: 业务数据
} protocol_layer_t;

// MAC层帧格式
typedef struct {
    // 帧头 (8字节)
    uint16_t frame_sync;                // 帧同步: 0xABCD
    uint8_t frame_type;                 // 帧类型
    uint8_t frame_flags;                // 帧标志
    uint32_t device_id;                 // 设备ID

    // 网络层 (4字节)
    uint16_t network_id;                // 网络ID
    uint8_t hop_count;                  // 跳数
    uint8_t ttl;                        // 生存时间

    // 应用层载荷 (可变长度, 最大200字节)
    uint8_t payload_length;             // 载荷长度
    uint8_t payload[200];               // 载荷数据

    // 帧尾 (4字节)
    uint16_t frame_crc16;               // 帧CRC16
    uint16_t frame_end;                 // 帧结束: 0x5678
} lora_frame_t;
```

### 系统配置管理

#### 配置参数存储结构
```c
// 系统配置结构
typedef struct {
    // 配置头
    uint32_t config_magic;              // 配置魔数: 0x12345678
    uint16_t config_version;            // 配置版本: 0x0100
    uint16_t config_size;               // 配置大小
    uint32_t config_crc32;              // 配置CRC32

    // 设备信息
    uint32_t device_id;                 // 设备ID
    uint8_t device_type;                // 设备类型
    uint8_t hardware_version;           // 硬件版本
    uint16_t firmware_version;          // 固件版本

    // 检测参数
    coarse_detection_params_t coarse_params;   // 粗检测参数
    fine_detection_params_t fine_params;       // 细检测参数

    // 通信参数
    lora_phy_config_t lora_config;      // LoRa配置
    uint32_t report_interval_ms;        // 上报间隔
    uint8_t retry_count;                // 重试次数

    // 功耗参数
    uint32_t sleep_threshold_ms;        // 休眠阈值
    uint8_t power_save_enabled;         // 节能使能
    float32_t battery_low_threshold_v;  // 低电压阈值

    // 校准数据
    float32_t accel_offset[3];          // 加速度偏移
    float32_t accel_scale[3];           // 加速度缩放
    float32_t temperature_offset;       // 温度偏移
} system_config_t;
```

#### 配置管理接口
```c
// 配置管理函数
typedef struct {
    // 配置加载和保存
    int (*load_config)(system_config_t* config);
    int (*save_config)(const system_config_t* config);
    int (*reset_config)(system_config_t* config);

    // 配置验证
    int (*validate_config)(const system_config_t* config);
    uint32_t (*calculate_crc)(const system_config_t* config);

    // 参数更新
    int (*update_detection_params)(const coarse_detection_params_t* coarse,
                                  const fine_detection_params_t* fine);
    int (*update_communication_params)(const lora_phy_config_t* lora);
    int (*update_power_params)(uint32_t sleep_threshold,
                              uint8_t power_save_enabled);
} config_manager_t;
```

### 错误处理和诊断系统

#### 错误代码定义
```c
// 系统错误代码
typedef enum {
    ERROR_NONE = 0x0000,                // 无错误

    // 硬件错误 (0x1000-0x1FFF)
    ERROR_SENSOR_INIT_FAILED = 0x1001,  // 传感器初始化失败
    ERROR_SENSOR_COMM_TIMEOUT = 0x1002, // 传感器通信超时
    ERROR_LORA_INIT_FAILED = 0x1003,    // LoRa初始化失败
    ERROR_LORA_TX_FAILED = 0x1004,      // LoRa发送失败
    ERROR_BATTERY_LOW = 0x1005,         // 电池电量低
    ERROR_TEMPERATURE_HIGH = 0x1006,    // 温度过高

    // 软件错误 (0x2000-0x2FFF)
    ERROR_MEMORY_ALLOCATION = 0x2001,   // 内存分配失败
    ERROR_BUFFER_OVERFLOW = 0x2002,     // 缓冲区溢出
    ERROR_INVALID_PARAMETER = 0x2003,   // 无效参数
    ERROR_CONFIG_CORRUPTED = 0x2004,    // 配置损坏
    ERROR_ALGORITHM_TIMEOUT = 0x2005,   // 算法超时
    ERROR_FFT_COMPUTATION = 0x2006,     // FFT计算错误

    // 系统错误 (0x3000-0x3FFF)
    ERROR_WATCHDOG_RESET = 0x3001,      // 看门狗复位
    ERROR_STACK_OVERFLOW = 0x3002,      // 栈溢出
    ERROR_SYSTEM_OVERLOAD = 0x3003,     // 系统过载
    ERROR_POWER_FAILURE = 0x3004,       // 电源故障
} error_code_t;
```

#### 诊断信息结构
```c
// 系统诊断信息
typedef struct {
    // 运行时统计
    uint32_t uptime_seconds;            // 运行时间
    uint32_t reset_count;               // 重启次数
    error_code_t last_error_code;       // 最后错误代码
    uint32_t error_timestamp;           // 错误时间戳

    // 性能统计
    float32_t cpu_usage_percent;        // CPU使用率
    uint32_t memory_usage_bytes;        // 内存使用量
    uint32_t max_stack_usage_bytes;     // 最大栈使用量

    // 检测统计
    uint32_t total_detections;          // 总检测次数
    uint32_t coarse_triggers;           // 粗检测触发次数
    uint32_t fine_analyses;             // 细检测分析次数
    uint32_t mining_detections;         // 挖掘检测次数
    uint32_t false_positives;           // 误报次数

    // 通信统计
    uint32_t lora_tx_count;             // LoRa发送次数
    uint32_t lora_tx_success;           // LoRa发送成功次数
    uint32_t lora_tx_failed;            // LoRa发送失败次数
    float32_t communication_success_rate; // 通信成功率

    // 功耗统计
    float32_t average_current_ma;       // 平均电流
    float32_t battery_voltage_v;        // 电池电压
    uint32_t power_level_time[4];       // 各功耗级别时间
} system_diagnostics_t;
```

### 测试验证框架

#### 单元测试框架
```c
// 测试用例结构
typedef struct {
    const char* test_name;              // 测试名称
    int (*test_function)(void);         // 测试函数
    int (*setup_function)(void);        // 设置函数
    int (*teardown_function)(void);     // 清理函数
    uint32_t timeout_ms;                // 超时时间
} test_case_t;

// 测试套件
typedef struct {
    const char* suite_name;             // 套件名称
    test_case_t* test_cases;            // 测试用例数组
    uint32_t test_count;                // 测试用例数量
    uint32_t passed_count;              // 通过数量
    uint32_t failed_count;              // 失败数量
} test_suite_t;

// 测试结果
typedef struct {
    uint32_t total_tests;               // 总测试数
    uint32_t passed_tests;              // 通过测试数
    uint32_t failed_tests;              // 失败测试数
    uint32_t execution_time_ms;         // 执行时间
    float32_t success_rate;             // 成功率
} test_result_t;
```

#### 性能基准测试
```c
// 性能基准
typedef struct {
    // 算法性能基准
    uint32_t coarse_detection_time_us;  // 粗检测时间: <100μs
    uint32_t fine_detection_time_us;    // 细检测时间: <2000μs
    uint32_t fft_computation_time_us;   // FFT计算时间: <500μs
    uint32_t feature_extraction_time_us; // 特征提取时间: <200μs

    // 系统性能基准
    uint32_t state_transition_time_us;  // 状态转换时间: <50μs
    uint32_t memory_allocation_time_us; // 内存分配时间: <10μs
    uint32_t config_save_time_ms;       // 配置保存时间: <100ms
    uint32_t lora_tx_time_ms;           // LoRa发送时间: <1000ms

    // 功耗性能基准
    float32_t idle_current_ua;          // 空闲电流: <10μA
    float32_t monitoring_current_ma;    // 监测电流: <2mA
    float32_t analysis_current_ma;      // 分析电流: <15mA
    float32_t communication_current_ma; // 通信电流: <50mA
} performance_benchmark_t;
```

## 📋 开发资源和工具链

### 开发环境要求
```
IDE: Keil MDK-ARM v5.29+ (现有项目已配置)
编译器: ARM Compiler v6.16+
调试器: ST-Link V2/V3 (现有硬件支持)
仿真器: STM32CubeMX (外设配置已完成)
版本控制: Git (现有仓库: IIM-42352-STM32F4-MAIN-RAW)
```

### 测试工具链
```
功耗测试: 功耗分析仪 (Keysight N6705C或类似)
信号发生器: 用于滤波器和算法验证
示波器: 用于时序分析和信号质量检查
频谱分析仪: 用于FFT结果验证
上位机: 现有Python分析工具 (vibration_analyzer_chinese.py)
```

### 代码质量工具
```
静态分析: PC-lint Plus (代码质量检查)
单元测试: Unity Test Framework (嵌入式单元测试)
性能分析: ARM DS-5 Performance Analysis (性能优化)
内存分析: Heap and Stack Analysis (内存使用监控)
```

### 文档和规范
```
编码规范: MISRA C 2012 (嵌入式C语言规范)
注释规范: Doxygen格式 (API文档生成)
版本管理: Semantic Versioning (语义化版本控制)
测试规范: IEEE 829 (软件测试文档标准)
```

## 🎯 项目交付物清单

### 代码交付物
- [ ] **源代码**: 完整的STM32项目源码 (基于现有代码扩展)
- [ ] **库文件**: 新增的算法库和驱动库
- [ ] **配置文件**: 系统参数配置和校准数据
- [ ] **编译脚本**: 自动化编译和部署脚本

### 文档交付物
- [ ] **系统架构文档**: 本文档 (已完成)
- [ ] **API接口文档**: 详细的函数接口说明
- [ ] **用户手册**: 系统使用和配置指南
- [ ] **测试报告**: 完整的验证测试报告
- [ ] **性能基准报告**: 与baseline版本对比分析

### 测试交付物
- [ ] **单元测试套件**: 所有模块的单元测试
- [ ] **集成测试套件**: 系统集成测试用例
- [ ] **性能测试套件**: 性能基准测试工具
- [ ] **回归测试套件**: 自动化回归测试脚本

### 工具交付物
- [ ] **调试工具**: 系统调试和诊断工具
- [ ] **配置工具**: 参数配置和校准工具
- [ ] **测试工具**: 专用测试和验证工具
- [ ] **部署工具**: 固件烧录和部署工具

## 📈 项目成功标准

### 功能成功标准
- ✅ 智能检测算法成功移植到STM32
- ✅ 检测准确率≥90% (与baseline版本对比)
- ✅ 响应时间<3秒 (从震动到LoRa发送)
- ✅ 支持完全独立运行，无需上位机

### 性能成功标准
- ✅ 平均功耗<5mA (相比现有版本降低>70%)
- ✅ 连续运行>30天无重启
- ✅ 通信成功率>99%
- ✅ 内存使用<64KB RAM, <256KB Flash

### 质量成功标准
- ✅ 代码覆盖率>90%
- ✅ 静态分析无严重缺陷
- ✅ 通过所有里程碑验证
- ✅ 符合嵌入式软件开发规范

### 商业成功标准
- ✅ 按时交付 (9周开发周期)
- ✅ 成本控制在预算范围内
- ✅ 技术方案可复制推广
- ✅ 为后续产品迭代奠定基础

---

**文档状态**: 基于现有代码分析的详细版本
**适用范围**: STM32智慧地钉独立运行系统开发 (基于现有IIM-42352项目)
**维护责任**: 系统架构师和开发团队
**更新频率**: 根据开发进度和需求变更及时更新
**版本历史**:
- v1.0: 初始架构设计
- v1.1: 基于现有代码的渐进式开发计划 (当前版本)
