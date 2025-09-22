# IIM-42352 STM32F4 振动分析系统 v3.1

## 📋 **项目概述**

基于STM32F4和IIM-42352传感器的专业振动分析系统，支持高分辨率频域分析和实时原始数据显示。

### **主要特性**
- ✅ **高分辨率频谱分析**: 257点FFT，1.953Hz频率分辨率
- ✅ **实时原始数据显示**: 三轴加速度实时波形 (新增功能)
- ✅ **同屏双视图界面**: 频域+时域同一界面显示，方便对比分析
- ✅ **真实数据显示**: 输出真实物理量，无人为放大
- ✅ **灵活的Y轴控制**: 自动/手动/对数缩放，支持g/mg/μg单位转换
- ✅ **专业上位机**: 中英文双版本，完整的显示控制功能
- ✅ **向后兼容**: 同时支持21点和257点数据格式

### **技术规格**
- **传感器**: IIM-42352 (±4g, 1000Hz采样)
- **处理器**: STM32F4系列
- **FFT分辨率**: 512点，0-500Hz频率范围
- **通信接口**: UART (115200bps)
- **数据精度**: 32位浮点数

## 🏗️ **系统架构**

```
传感器采集 → 数据处理 → 协议封装 → 串口传输 → 上位机显示
    ↓           ↓         ↓         ↓         ↓
IIM-42352 → STM32F4 → 自定义协议 → UART → Python GUI
           (FFT+原始)   (双协议)            (双视图)
```

### **数据流程**
- **频域数据**: 1000Hz采样 → 512点FFT → 257点频谱 → 2Hz更新 → 上方图表
- **原始数据**: 1000Hz采样 → 三轴加速度 → 10Hz更新 → 下方图表

## 📁 **项目结构**

```
IIM-42352-STM32F4/
├── Core/
│   ├── Src/
│   │   ├── main.c                    # 主程序
│   │   ├── fft_processor.c           # FFT处理模块
│   │   ├── fft_test.c                # FFT测试模块
│   │   ├── example-raw-data.c        # 传感器配置和原始数据发送
│   │   └── clock_config_84mhz.c      # 84MHz时钟配置验证
│   └── Inc/
│       ├── fft_processor.h
│       ├── fft_test.h
│       ├── example-raw-data.h
│       ├── clock_config_84mhz.h      # 84MHz时钟配置定义
│       └── main.h
├── Iim423xx/                         # 传感器驱动库
├── LCD/                              # LCD显示模块
├── delay/                            # 延时函数
├── sys/                              # 系统配置
├── MDK-ARM/                          # Keil工程文件
├── vibration_analyzer_chinese.py     # 中文版上位机 (推荐)
├── vibration_analyzer_pro_en.py      # 英文版上位机
├── test_raw_data.py                  # 原始数据测试工具
├── run_analyzer.bat                  # 一键启动脚本
├── UPGRADE_NOTES.md                  # 升级说明文档
└── README.md                         # 本文档
```

## 🔧 **STM32配置总结**

### **硬件配置**
- **MCU**: STM32F4系列 (推荐STM32F407VGT6)
- **时钟**: 84MHz主频 (优化功耗配置，降低50%功耗)
- **内存**: 192KB RAM, 1MB Flash
- **接口**: SPI (传感器), UART (通信)

### **84MHz低功耗配置** ⚡
- **系统时钟**: 84MHz (从168MHz降频50%)
- **功耗优化**: 降低约50%功耗，延长电池寿命
- **性能保证**: FFT处理时间36μs，完全满足实时性要求
- **验证功能**: 自动时钟配置验证和性能测试
- **配置文件**: `Core/Inc/clock_config_84mhz.h`, `Core/Src/clock_config_84mhz.c`

### **传感器接口配置**
```c
// SPI配置
SPI_InitTypeDef SPI_InitStruct;
SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;
SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;
SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
```

### **UART配置**
```c
// UART配置 (115200bps)
UART_HandleTypeDef huart;
huart.Instance = USART1;
huart.Init.BaudRate = 115200;
huart.Init.WordLength = UART_WORDLENGTH_8B;
huart.Init.StopBits = UART_STOPBITS_1;
huart.Init.Parity = UART_PARITY_NONE;
huart.Init.Mode = UART_MODE_TX_RX;
```

### **FFT配置**
```c
// FFT参数
#define FFT_SIZE 512
#define SAMPLE_RATE 1000.0f
#define FREQ_RESOLUTION (SAMPLE_RATE / FFT_SIZE)  // 1.953Hz
```

## 📡 **传感器配置总结**

### **IIM-42352配置参数**
```c
// 基本配置
采样率: IIM423XX_ACCEL_CONFIG0_ODR_1_KHZ     // 1000Hz
量程:   IIM423XX_ACCEL_CONFIG0_FS_SEL_4g     // ±4g
模式:   IS_LOW_NOISE_MODE                    // 低噪声模式
分辨率: IS_HIGH_RES_MODE                     // 高分辨率模式
```

### **振动优化滤波器**
```c
// 专用振动分析滤波器配置
rc |= inv_iim423xx_set_reg_bank(&icm_driver, 2);
data = 0x7E;  // ACCEL_AAF_DELT: 63
rc |= inv_iim423xx_write_reg(&icm_driver, MPUREG_ACCEL_CONFIG_STATIC2_B2, 1, &data);
data = 0x80;  // ACCEL_AAF_DELTSQR: 3968
rc |= inv_iim423xx_write_reg(&icm_driver, MPUREG_ACCEL_CONFIG_STATIC3_B2, 1, &data);
data = 0x3F;  // ACCEL_AAF_BITSHIFT: 3
rc |= inv_iim423xx_write_reg(&icm_driver, MPUREG_ACCEL_CONFIG_STATIC4_B2, 1, &data);
```

### **数据处理流程**
1. **原始数据采集**: 16位ADC → 浮点数转换
2. **FFT预处理**: 窗函数 → 512点FFT
3. **幅度计算**: 复数 → 幅度谱
4. **物理量转换**: ADC值 → 真实g值 (0.001f缩放)

## 📨 **通信协议总结**

### **协议格式**
```
帧头(2) + 命令(1) + 长度(2) + 载荷(N) + 校验(1) + 帧尾(1)
0xAA55  + CMD    + LEN     + DATA   + CHK    + 0x0D
```

### **命令定义**
| 命令码 | 数据类型 | 载荷长度 | 描述 | 更新频率 |
|--------|----------|----------|------|----------|
| 0x01 | 21点频谱 | 88字节 | 时间戳(4) + 21个浮点数(84) | ~2Hz |
| 0x02 | 原始加速度 | 16字节 | 时间戳(4) + X/Y/Z轴(12) | 10Hz |
| 0x04 | 257点频谱 | 1032字节 | 时间戳(4) + 257个浮点数(1028) | ~2Hz |

### **数据格式**
```c
// 频谱数据载荷结构
typedef struct {
    uint32_t timestamp;           // 时间戳
    float magnitudes[257];        // 幅度数据 (真实g值)
} spectrum_data_t;

// 原始加速度数据载荷结构
typedef struct {
    uint32_t timestamp;           // 时间戳
    float accel_x;                // X轴加速度 (g)
    float accel_y;                // Y轴加速度 (g)
    float accel_z;                // Z轴加速度 (g)
} raw_accel_data_t;
```

### **校验算法**
```c
// XOR校验
uint8_t checksum = cmd;
for(int i = 0; i < payload_len; i++) {
    checksum ^= payload[i];
}
checksum ^= (payload_len & 0xFF);
checksum ^= ((payload_len >> 8) & 0xFF);
```

## 🖥️ **上位机配置总结**

### **版本对比**
| 版本 | 文件名 | 语言 | 特点 |
|------|--------|------|------|
| 中文专业版 | `vibration_analyzer_chinese.py` | 中文 | 完整功能，双视图显示，字体已修复 |
| 英文专业版 | `vibration_analyzer_pro_en.py` | 英文 | 完整功能，无字体问题 |
| 测试工具 | `test_raw_data.py` | 中文 | 原始数据接收测试 |

### **核心功能模块**
```python
# 主要类结构
class ProtocolParser:           # 协议解析 (支持频域+原始数据)
class VibrAnalyzer:            # 主界面
    - setup_ui()               # 界面初始化 (双视图)
    - setup_plot()             # 图表初始化 (频域+时域)
    - receive_data()           # 数据接收
    - update_plot()            # 频域显示更新
    - update_raw_data()        # 原始数据显示更新
```

### **显示控制功能**
- **同屏双视图**: 频域分析 + 原始数据同一界面显示，方便对比分析
- **Y轴缩放**: 自动/手动/对数三种模式
- **单位转换**: g/mg/μg实时转换
- **快速缩放**: 微振动(0-1mg)/小振动(0-10mg)/中振动(0-100mg)/大振动(0-1g)
- **显示选项**: 网格开关、峰值标注、原始数据开关、实时统计
- **时域显示**: 三轴加速度实时波形，最近10秒数据

## 📊 **参数总结**

### **关键参数配置**
```c
// STM32端参数
#define SAMPLE_RATE 1000.0f           // 采样率
#define FFT_SIZE 512                  // FFT点数
#define SCALING_FACTOR 0.001f         // 物理量缩放系数
#define SENSOR_RANGE_G 4.0f           // 传感器量程

// 频率参数
频率分辨率: 1.953Hz (1000Hz/512)
频率范围: 0-500Hz
数据点数: 257点 (单边谱)
```

### **性能参数**
```
频域数据更新率: ~2Hz
原始数据更新率: 10Hz
传输带宽: ~2.5KB/s
延迟: <100ms
精度: 32位浮点数 (~7位有效数字)
动态范围: 0.0001g - 4g
显示窗口: 最近10秒原始数据
```

## 🎯 **功能总结**

### **STM32端功能**
- ✅ **传感器数据采集**: 1000Hz高速采集
- ✅ **实时FFT处理**: 512点FFT，窗函数优化
- ✅ **原始数据发送**: 三轴加速度实时传输
- ✅ **双协议支持**: 频域数据 + 原始数据协议
- ✅ **协议封装**: 自定义高效协议
- ✅ **串口通信**: 115200bps稳定传输
- ✅ **多模式支持**: 21点兼容模式 + 257点高分辨率模式

### **上位机功能**
- ✅ **同屏双视图**: 频域+时域同一界面显示，方便对比分析
- ✅ **实时频谱显示**: 连续曲线/柱状图双模式
- ✅ **原始数据波形**: 三轴加速度实时波形显示
- ✅ **灵活显示控制**: 多种缩放模式和单位转换
- ✅ **峰值自动检测**: 实时标注最大峰值
- ✅ **数据统计**: 更新率、峰值频率、幅度统计、原始数据状态
- ✅ **中英文支持**: 双语言界面，字体优化

### **数据处理功能**
- ✅ **真实物理量**: 输出真实g值，无人为放大
- ✅ **高精度显示**: 支持微振动显示 (0.0001g级别)
- ✅ **多单位支持**: g/mg/μg自动转换
- ✅ **时域分析**: 三轴加速度实时监测
- ✅ **智能缓存**: 自动管理数据缓存，避免内存泄漏
- ✅ **实时验证**: 数据完整性检查和验证工具

## 📖 **使用方法**

### **1. 硬件连接**
```
STM32F4 ←→ IIM-42352 (SPI接口)
STM32F4 ←→ PC (USB转串口)
```

### **2. 软件部署**
```bash
# 编译烧录STM32代码
1. 打开Keil MDK或STM32CubeIDE
2. 导入项目
3. 编译 (Build All)
4. 烧录 (Run/Debug)

# 安装Python依赖
pip install matplotlib numpy pyserial tkinter

# 运行上位机
python vibration_analyzer_chinese.py  # 中文版 (推荐)
# 或
python vibration_analyzer_pro_en.py   # 英文版
# 或使用一键启动
run_analyzer.bat                       # Windows一键启动
```

### **3. 操作步骤**
```bash
1. 连接硬件设备
2. 选择串口 (通常COM6)
3. 点击"连接"按钮
4. 观察双视图显示:
   - 上方图表: 频域分析 (振动频谱)
   - 下方图表: 时域分析 (三轴加速度实时波形)
5. 调整显示设置:
   - 如果数据太小: 点击"微振动(0-1mg)"
   - 或切换单位到"mg"/"μg"
   - 勾选"显示原始数据"查看时域波形
6. 观察实时数据显示
```

## 🧪 **测试方法**

### **1. 功能验证测试**
```bash
# 运行原始数据测试
python test_raw_data.py

# 检查项目:
- 协议解析功能 ✅ (频域+原始数据)
- 频率映射精度 ✅
- 峰值检测算法 ✅
- 数据缩放处理 ✅
- 原始数据显示 ✅
- GUI组件功能 ✅
```

### **2. 数据真实性测试**
```bash
# 静态测试
1. 保持传感器完全静止
2. 记录基线噪声 (应 < 0.001g)
3. 观察频谱稳定性

# 动态测试  
1. 轻敲传感器附近表面
2. 观察频谱变化
3. 验证峰值检测准确性

# 频率验证
1. 用手机播放50Hz/100Hz音频
2. 将手机靠近传感器
3. 验证对应频率峰值出现
```

### **3. 性能测试**
```bash
# 数据传输测试
1. 检查频域更新频率 (目标: ~2Hz)
2. 检查原始数据更新频率 (目标: 10Hz)
3. 检查数据完整性
4. 长时间稳定性测试 (>1小时)

# 精度测试
1. 对比已知振动源
2. 验证幅度测量精度
3. 验证频率测量精度
4. 验证原始数据精度
```

### **4. 显示功能测试**
```bash
# 频域显示测试
1. 测试自动缩放功能
2. 测试手动范围设置
3. 测试对数显示模式

# 时域显示测试
1. 验证三轴数据显示
2. 测试10秒滚动窗口
3. 验证实时更新

# 单位转换测试
1. 验证g/mg/μg转换正确性
2. 测试快速缩放按钮
3. 验证数值显示精度
```

## 🔧 **故障排除**

### **常见问题**
| 问题 | 原因 | 解决方案 |
|------|------|----------|
| 串口连接失败 | 端口被占用/驱动问题 | 检查设备管理器，重新插拔USB |
| 数据显示为0 | 缩放设置不当 | 点击"微振动(0-1mg)"或切换到mg单位 |
| 只有频域数据，无原始数据 | 原始数据功能未启用 | 勾选"显示原始数据"复选框 |
| 原始数据更新缓慢 | 串口通信问题 | 检查串口连接，重启程序 |
| 中文字体乱码 | matplotlib字体问题 | 使用英文版或运行字体修复工具 |
| FFT结果异常 | 传感器配置错误 | 检查传感器初始化代码 |

### **调试工具**
```bash
python test_raw_data.py           # 原始数据接收测试
python vibration_analyzer_pro_en.py  # 英文版（无字体问题）
run_analyzer.bat                  # 一键启动脚本
```

## 📈 **项目成果**

### **技术突破**
- ✅ **高分辨率升级**: 从21点提升到257点 (12.8倍分辨率提升)
- ✅ **同屏双视图**: 频域+时域同一界面显示，方便对比分析
- ✅ **实时原始数据**: 三轴加速度实时波形监测
- ✅ **真实数据显示**: 摒弃人为放大，显示真实物理量
- ✅ **专业显示控制**: 灵活的Y轴缩放和单位转换
- ✅ **中文界面优化**: 完美解决字体显示问题

### **应用价值**
- 🎯 **精密设备监测**: 可检测0.0001g级别的微振动
- 🎯 **机械故障诊断**: 高分辨率频谱分析 + 时域波形分析
- 🎯 **振动测试平台**: 专业的测试和分析工具
- 🎯 **冲击检测**: 实时监测瞬态振动和冲击事件
- 🎯 **教学研究**: 完整的振动分析系统示例

## 📄 **许可证**

本项目采用 MIT 许可证 - 详见 [LICENSE](LICENSE) 文件

## 👥 **贡献**

欢迎提交 Issue 和 Pull Request 来改进项目

## 📞 **联系方式**

如有问题或建议，请通过 GitHub Issues 联系

## 🆕 **v3.1 新增功能详解**

### **实时原始加速度数据显示**

#### **功能特点**
- **同屏显示**: 频域+时域同一界面显示，方便对比分析
- **三轴同步显示**: X/Y/Z轴加速度实时波形，不同颜色区分
- **滚动窗口**: 显示最近10秒数据，自动滚动更新
- **智能缩放**: 自动调整Y轴范围适应数据幅度
- **高效传输**: 10Hz更新频率，不影响频域分析性能

#### **技术实现**
```python
# 原始数据缓存结构
self.raw_accel_buffer = {
    'x': deque(maxlen=1000),  # X轴数据
    'y': deque(maxlen=1000),  # Y轴数据
    'z': deque(maxlen=1000),  # Z轴数据
    'time': deque(maxlen=1000) # 时间戳
}
```

#### **协议扩展**
```c
// 新增命令码 0x02 - 原始加速度数据
typedef struct {
    uint8_t header[2];        // 0xAA 0x55
    uint8_t cmd;              // 0x02
    uint16_t length;          // 16 (小端序)
    uint32_t timestamp;       // 时间戳
    float accel_x;            // X轴加速度 (g)
    float accel_y;            // Y轴加速度 (g)
    float accel_z;            // Z轴加速度 (g)
    uint8_t checksum;         // XOR校验
    uint8_t tail;             // 0x0D
} raw_accel_frame_t;
```

#### **使用场景**
- **瞬态分析**: 观察冲击、碰撞等瞬态事件
- **振动模式**: 识别周期性振动模式
- **基线监测**: 监测设备静态时的噪声水平
- **故障诊断**: 结合频域和时域信息进行综合分析

### **升级说明**
详细的升级说明请参考 [UPGRADE_NOTES.md](UPGRADE_NOTES.md)

---

**专业振动分析系统 v3.1 - 真实数据，精确分析，全面监测** 🚀
