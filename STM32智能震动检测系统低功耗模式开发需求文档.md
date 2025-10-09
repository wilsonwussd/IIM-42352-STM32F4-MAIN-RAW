# STM32智能震动检测系统低功耗模式开发需求文档

## 📋 **项目概述**

### **项目名称**
STM32智能震动检测系统v4.0低功耗模式扩展（阶段6功能）

### **开发目标**
在现有v4.0架构基础上，新增RTC定时唤醒的低功耗模式，实现功耗降低70%以上，电池寿命从天级延长到月级，同时保持现有的检测精度和功能完整性。

### **基础架构**
- **硬件平台**: STM32F407VGT6 + IIM-42352传感器
- **现有架构**: 5阶段渐进式设计（数据预处理、粗检测、智能FFT、细检测、状态机）
- **扩展方向**: 在现有架构上增加功耗管理层，实现阶段6功能

## 🎯 **功能需求详述**

### **核心功能流程**

**重要说明**: 完全保持现有v4.0检测算法逻辑不变，只是将运行方式从"连续运行"改为"定时唤醒运行"

**现有v4.0检测流程**:
```c
// 现有的完整检测流程 (Core/Src/example-raw-data.c)
1. 连续采样 IIM-42352 传感器 (1000Hz)
2. 5Hz高通滤波处理 (去除DC分量)
3. 粗检测算法:
   - RMS滑动窗口 (200样本 = 200ms)
   - 峰值因子计算 (current_rms / baseline_rms)
   - 触发判断 (peak_factor > 1.5x)
4. 智能FFT触发:
   - 如果粗检测未触发: 跳过FFT处理
   - 如果粗检测触发: 自动激活FFT处理
5. 细检测算法 (仅在FFT激活时):
   - 512点FFT频域分析
   - 5维特征提取
   - 智能分类判断
6. LoRa报警 (仅在检测到挖掘振动时)
```

#### **情况一：无振动检测**
```
Sleep模式 → RTC 2s唤醒 → 启动现有检测流程 → 粗检测[不触发] → 回到Sleep模式
```
*说明：粗检测RMS滑动窗口计算，峰值因子<1.5x，FFT_SetTriggerState(false)，跳过FFT处理*

#### **情况二：正常振动检测**
```
Sleep模式 → RTC 2s唤醒 → 启动现有检测流程 → 粗检测[触发]
→ 自动进入FFT处理 → 细检测[正常振动] → 回到Sleep模式
```
*说明：峰值因子≥1.5x，FFT_SetTriggerState(true)，自动激活512点FFT和5维特征提取*

#### **情况三：非法挖掘检测**
```
Sleep模式 → RTC 2s唤醒 → 启动现有检测流程 → 粗检测[触发]
→ 自动进入FFT处理 → 细检测[挖掘振动] → LoRa报警 → 回到Sleep模式
```
*说明：细检测分类结果为FINE_DETECTION_MINING，触发现有LoRa报警状态机*

### **功能特性要求**
1. **RTC定时唤醒**: 2秒周期唤醒，可通过宏配置
2. **现有算法100%复用**: 完全保持v4.0的检测逻辑不变
   - 粗检测：使用加速度数据，RMS滑动窗口，峰值因子判断
   - 智能FFT触发：只有粗检测达阈值才运行FFT
   - 细检测：5维特征提取和分类算法
   - LoRa报警：完全按现有逻辑执行
3. **运行方式改变**: 从连续运行改为定时唤醒运行
4. **调试支持**: Sleep期间保持串口调试功能
5. **向后兼容**: 可配置选择连续模式或低功耗模式

## 🏗️ **技术架构设计**

### **系统架构扩展**
```
现有v4.0架构
├── 阶段1: 数据预处理 (5Hz高通滤波) ✅
├── 阶段2: 粗检测算法 (RMS滑动窗口) ✅
├── 阶段3: 智能FFT控制 (按需触发) ✅
├── 阶段4: 细检测算法 (5维特征提取) ✅
├── 阶段5: 系统状态机 (10状态管理) ✅
└── 阶段6: 功耗管理 (RTC低功耗模式) 🆕
    ├── RTC定时唤醒控制
    ├── Sleep模式管理
    ├── 快速采样控制
    └── 功耗状态监控
```

### **关键宏定义**
```c
// 低功耗模式配置
#define ENABLE_LOW_POWER_MODE           1       // 使能低功耗模式
#define RTC_WAKEUP_PERIOD_SEC           2       // RTC唤醒周期(秒)
#define LOW_POWER_DEBUG_ENABLED         1       // 低功耗调试使能

// 功耗管理参数
#define SLEEP_ENTRY_DELAY_MS            100     // 进入Sleep前延时
#define WAKEUP_STABILIZE_DELAY_MS       50      // 唤醒后稳定延时
#define SENSOR_STARTUP_DELAY_MS         10      // 传感器启动延时

// 注意：使用现有v4.0的精确参数定义
// 现有定义：RMS_WINDOW_SIZE = 200, FFT_SIZE = 512
// 现有粗检测参数：BASELINE_RMS_THRESHOLD = 0.001f, TRIGGER_MULTIPLIER = 1.5f
```

### **状态机扩展**
```c
typedef enum {
    // 现有状态保持不变
    SYSTEM_INIT,
    SYSTEM_MONITORING,
    SYSTEM_COARSE_TRIGGERED,
    SYSTEM_FINE_ANALYSIS,
    SYSTEM_MINING_DETECTED,
    SYSTEM_ALARM_SENDING,
    SYSTEM_ALARM_COMPLETE,
    SYSTEM_ERROR,
    SYSTEM_IDLE,
    SYSTEM_RESET,
    
    // 新增低功耗状态
    SYSTEM_SLEEP_PREPARE,       // 准备进入Sleep
    SYSTEM_SLEEP_MODE,          // Sleep模式
    SYSTEM_WAKEUP,              // RTC唤醒
    SYSTEM_DETECTION_ACTIVE,    // 检测活跃状态(运行现有检测流程)
    SYSTEM_POWER_ANALYSIS,      // 功耗分析状态
    
    STATE_COUNT                 // 状态总数
} system_state_t;
```

## 🔧 **接口定义和数据结构**

### **低功耗管理结构**
```c
typedef struct {
    // 配置参数
    uint32_t wakeup_period_sec;         // 唤醒周期
    uint8_t debug_enabled;              // 调试使能

    // 运行状态
    uint8_t low_power_enabled;          // 低功耗模式使能
    uint32_t sleep_count;               // Sleep次数统计
    uint32_t wakeup_count;              // 唤醒次数统计
    uint32_t detection_count;           // 检测次数统计
    uint32_t coarse_trigger_count;      // 粗检测触发次数
    uint32_t fine_analysis_count;       // 细检测分析次数
    uint32_t alarm_count;               // 报警次数

    // 功耗统计
    uint32_t total_sleep_time_sec;      // 总Sleep时间
    uint32_t total_active_time_ms;      // 总活跃时间
    float32_t average_power_ma;         // 平均功耗

    // 状态标志
    uint8_t is_sleeping;                // 当前是否在Sleep
    uint8_t wakeup_source;              // 唤醒源标识
    uint32_t last_wakeup_time;          // 上次唤醒时间
    uint32_t next_sleep_time;           // 下次Sleep时间
} low_power_manager_t;
```

### **核心API接口**
```c
// 低功耗管理初始化
int LowPower_Init(void);

// 进入Sleep模式
int LowPower_EnterSleep(void);

// 处理RTC唤醒
int LowPower_HandleWakeup(void);

// 启动现有检测流程（完全复用v4.0逻辑）
int LowPower_StartDetectionProcess(void);

// 检测完成判断
bool LowPower_IsDetectionComplete(void);

// 功耗统计更新
void LowPower_UpdatePowerStats(void);

// 获取功耗统计
low_power_manager_t* LowPower_GetStats(void);
```

## ⚡ **功耗管理策略**

### **功耗模式定义**
```c
typedef enum {
    POWER_MODE_CONTINUOUS,      // 连续模式(现有v4.0模式)
    POWER_MODE_LOW_POWER,       // 低功耗模式(新增)
    POWER_MODE_ULTRA_LOW,       // 超低功耗模式(预留)
} power_mode_t;
```

### **功耗优化策略**
1. **Sleep模式**: STM32 Sleep模式，保持RTC和RAM，关闭其他外设
2. **传感器管理**: 快速启动IIM-42352，采样完成后可选择性休眠
3. **时钟管理**: Sleep期间降低系统时钟，唤醒后恢复84MHz
4. **外设控制**: 按需启动SPI、UART等外设
5. **调试保持**: UART1保持低速运行，支持调试输出

### **预期功耗指标**
```
Sleep模式:     <0.1mA  (RTC运行，外设关闭)
检测活跃:     ~2-20mA (运行现有v4.0检测流程，时间0.2-3秒)
  - 仅粗检测:  ~2mA × 0.2秒 (RMS计算，未触发FFT)
  - 含FFT分析: ~10mA × 0.5秒 (FFT + 细检测)
  - 含LoRa通信: ~30mA × 2秒 (报警发送)
平均功耗:     <0.6mA  (相比现有2mA，降低70%)
```

## 📝 **实现优先级和阶段划分**

### **Phase 1: 基础框架 (优先级: 高)**
- [ ] RTC配置和定时唤醒功能
- [ ] 基本Sleep/Wake状态机
- [ ] 低功耗管理数据结构
- [ ] 核心API接口定义

### **Phase 2: 检测流程集成 (优先级: 高)**
- [ ] 现有v4.0检测流程启动控制
- [ ] 检测完成状态判断
- [ ] 现有算法100%复用验证
- [ ] 状态转换逻辑

### **Phase 3: 功耗优化 (优先级: 中)**
- [ ] 传感器低功耗管理
- [ ] 外设按需控制
- [ ] 时钟动态调整
- [ ] 功耗统计功能

### **Phase 4: 调试和测试 (优先级: 中)**
- [ ] 调试接口保持
- [ ] 功耗测试工具
- [ ] 性能验证
- [ ] 稳定性测试

### **Phase 5: 优化和完善 (优先级: 低)**
- [ ] 参数可配置化
- [ ] 异常处理完善
- [ ] 性能调优
- [ ] 文档完善

## 🧪 **测试验证要求**

### **功能测试**
1. **RTC唤醒测试**: 验证2秒周期唤醒的准确性
2. **检测流程测试**: 验证现有v4.0检测流程在低功耗模式下的正确性
3. **算法复用测试**: 验证粗检测、FFT触发、细检测、报警逻辑100%不变
4. **状态转换测试**: 验证Sleep/Wake状态转换的正确性
5. **三种情况测试**: 验证无振动、正常振动、挖掘振动三种情况的正确处理

### **性能测试**
1. **功耗测试**: 实际测量各模式下的功耗
2. **响应时间测试**: 测量唤醒到检测完成的时间
3. **检测精度测试**: 验证低功耗模式下的检测精度
4. **稳定性测试**: 长期运行稳定性验证
5. **电池寿命测试**: 实际电池供电寿命测试

### **兼容性测试**
1. **向后兼容测试**: 验证连续模式功能不受影响
2. **配置切换测试**: 验证模式切换的可靠性
3. **调试功能测试**: 验证调试接口在低功耗模式下的可用性

## 📊 **性能指标和验收标准**

### **功耗指标**
- **平均功耗**: <0.6mA (目标降低70%)
- **Sleep功耗**: <0.1mA
- **唤醒响应**: <100ms
- **电池寿命**: >30天 (2000mAh电池)

### **功能指标**
- **检测精度**: 保持75-85%置信度
- **响应时间**: 完整检测流程<3秒
- **唤醒准确性**: RTC唤醒误差<±50ms
- **系统稳定性**: 连续运行>7天无异常

### **兼容性指标**
- **现有功能**: 100%保持现有v4.0功能
- **代码复用**: >90%复用现有算法代码
- **配置灵活性**: 支持运行时模式切换
- **调试支持**: 100%保持调试功能

## ⚠️ **风险评估和注意事项**

### **技术风险**
1. **RTC精度**: 长期运行可能存在时钟漂移
2. **传感器启动**: IIM-42352快速启动的稳定性
3. **状态同步**: Sleep/Wake状态转换的数据一致性
4. **功耗测量**: 实际功耗可能与理论值存在差异

### **实现注意事项**
1. **保持现有架构**: 不破坏现有v4.0的稳定性
2. **渐进式开发**: 分阶段实现，每阶段充分测试
3. **调试友好**: 确保开发过程中的调试便利性
4. **文档同步**: 及时更新技术文档和用户手册

### **验收条件**
1. **功能完整**: 三种检测情况全部正确实现
2. **性能达标**: 所有性能指标达到要求
3. **稳定可靠**: 通过长期稳定性测试
4. **文档齐全**: 完整的技术文档和使用说明

## 💻 **具体实现指导**

### **RTC配置实现**
```c
// RTC初始化配置
void LowPower_RTC_Init(void) {
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};

    // 配置RTC时钟源为LSE
    hrtc.Instance = RTC;
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
    hrtc.Init.AsynchPrediv = 127;
    hrtc.Init.SynchPrediv = 255;
    hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
    hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

    if (HAL_RTC_Init(&hrtc) != HAL_OK) {
        Error_Handler();
    }

    // 配置Wakeup Timer
    if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc,
                                   RTC_WAKEUP_PERIOD_SEC * 2048 - 1,
                                   RTC_WAKEUPCLOCK_RTCCLK_DIV16) != HAL_OK) {
        Error_Handler();
    }
}
```

### **Sleep模式实现**
```c
// 进入Sleep模式
void LowPower_EnterSleepMode(void) {
    // 保存当前状态
    LowPower_SaveSystemState();

    // 关闭不必要的外设
    LowPower_DisablePeripherals();

    // 配置唤醒源
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);

    // 进入Sleep模式
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);

    // 唤醒后恢复
    LowPower_RestoreSystemState();
    LowPower_EnablePeripherals();
}
```

### **检测流程控制实现**
```c
// 启动现有检测流程（完全复用v4.0逻辑）
int LowPower_StartDetectionProcess(void) {
    // 启动传感器
    IIM423xx_WakeUp();
    HAL_Delay(SENSOR_STARTUP_DELAY_MS);

    // 直接调用现有的主检测循环
    // 这里会按照现有逻辑执行：
    // 1. 连续采样和滤波
    // 2. RMS滑动窗口计算
    // 3. 粗检测判断
    // 4. 如果触发，自动进入FFT和细检测
    // 5. 如果检测到挖掘，自动发送LoRa报警

    return ProcessSensorData(); // 调用现有的数据处理函数
}

// 检测完成判断
bool LowPower_IsDetectionComplete(void) {
    system_state_t current_state = GetSystemState();

    // 检测完成的条件：
    // 1. 粗检测未触发，回到MONITORING状态
    // 2. 细检测完成，回到MONITORING状态
    // 3. 报警发送完成，回到MONITORING状态
    return (current_state == SYSTEM_MONITORING ||
            current_state == SYSTEM_IDLE);
}
```

### **现有代码集成策略**
```c
// 集成现有算法的适配层
typedef struct {
    // 现有算法实例
    highpass_filter_t* filter;
    coarse_detector_t* coarse_detector;
    fine_detection_features_t* fine_detector;
    system_state_machine_t* state_machine;

    // 低功耗模式适配
    low_power_manager_t* power_manager;
    uint8_t quick_mode;         // 快速模式标志
    uint8_t detailed_mode;      // 详细模式标志
} low_power_adapter_t;

// 适配器初始化
int LowPowerAdapter_Init(low_power_adapter_t* adapter) {
    // 复用现有算法初始化
    adapter->filter = GetHighpassFilter();
    adapter->coarse_detector = GetCoarseDetector();
    adapter->fine_detector = GetFineDetector();
    adapter->state_machine = GetSystemStateMachine();

    // 初始化功耗管理
    adapter->power_manager = malloc(sizeof(low_power_manager_t));
    memset(adapter->power_manager, 0, sizeof(low_power_manager_t));

    return 0;
}
```

## 📁 **文件结构规划**

### **新增文件**
```
Core/Inc/
├── low_power_manager.h         # 低功耗管理头文件
├── rtc_wakeup.h               # RTC唤醒控制头文件
└── power_adapter.h            # 现有算法适配头文件

Core/Src/
├── low_power_manager.c         # 低功耗管理实现
├── rtc_wakeup.c               # RTC唤醒控制实现
└── power_adapter.c            # 现有算法适配实现
```

### **修改文件**
```
Core/Src/main.c                # 主循环集成低功耗模式
Core/Inc/main.h                # 添加低功耗相关定义
Core/Src/example-raw-data.c    # 状态机扩展
Core/Inc/example-raw-data.h    # 状态定义扩展
```

## 🔄 **开发流程建议**

### **Step 1: 环境准备**
1. 备份现有v4.0代码
2. 创建低功耗开发分支
3. 准备功耗测试设备
4. 搭建调试环境

### **Step 2: 基础框架开发**
1. 实现RTC配置和唤醒功能
2. 实现基本Sleep/Wake控制
3. 扩展状态机定义
4. 实现核心API接口

### **Step 3: 算法集成**
1. 实现快速采样控制
2. 集成现有粗检测算法
3. 实现详细采样控制
4. 集成现有FFT和细检测算法

### **Step 4: 功能测试**
1. 单元测试各个模块
2. 集成测试完整流程
3. 功耗测试和优化
4. 稳定性测试

### **Step 5: 优化完善**
1. 性能调优
2. 异常处理完善
3. 文档更新
4. 代码审查

## 📋 **开发检查清单**

### **Phase 1 完成标准**
- [ ] RTC配置正确，2秒唤醒准确
- [ ] Sleep模式正常进入和退出
- [ ] 基本状态机扩展完成
- [ ] 核心数据结构定义完成
- [ ] 基础API接口实现完成

### **Phase 2 完成标准**
- [ ] 200样本快速采样正常
- [ ] 512样本详细采样正常
- [ ] 现有粗检测算法正确集成
- [ ] 现有FFT和细检测算法正确集成
- [ ] 三种检测情况流程正确

### **Phase 3 完成标准**
- [ ] 实际功耗测试达标(<0.6mA平均)
- [ ] 传感器快速启动正常
- [ ] 外设按需控制正常
- [ ] 功耗统计功能正常

### **Phase 4 完成标准**
- [ ] 调试功能在低功耗模式下正常
- [ ] 所有功能测试通过
- [ ] 长期稳定性测试通过(>24小时)
- [ ] 性能指标全部达标

### **最终验收标准**
- [ ] 三种检测情况100%正确实现
- [ ] 功耗降低70%以上
- [ ] 检测精度保持75-85%
- [ ] 现有v4.0功能100%兼容
- [ ] 文档完整，代码规范

---

**开发依据**: 本文档作为STM32智能震动检测系统v4.0低功耗模式开发的唯一技术依据，所有开发活动应严格按照本文档执行，确保不偏离设计目标。

**版本**: v1.0
**日期**: 2025-01-08
**状态**: 待开发实现
**预计工期**: 2-3周（按Phase分阶段实现）
