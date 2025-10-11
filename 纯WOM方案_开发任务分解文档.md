# 纯WOM方案 - 开发任务分解文档

## 📋 文档信息

- **项目名称**: STM32智能震动检测系统 - 纯WOM方案开发
- **文档版本**: v1.0
- **创建日期**: 2025-10-11
- **开发周期**: 4周（预估）
- **文档状态**: 待审核

---

## 🎯 一、开发目标

### 1.1 核心目标

将现有RTC定时唤醒架构改造为纯WOM（Wake-on-Motion）事件驱动架构，实现：

- ✅ 功耗降低84.8%（7.29mA → 1.11mA）
- ✅ 电池寿命提升6.5倍（17.2天 → 112.6天）
- ✅ 响应速度提升（2秒延迟 → 即时响应）
- ✅ 保持现有检测算法100%兼容

### 1.2 架构转变

```
当前RTC方案：
STM32 Sleep → RTC定时唤醒(2秒) → 采集2000样本 → 阶段2-5检测 → Sleep

纯WOM方案：
STM32 Sleep → WOM中断触发 → 快速预检(200样本) → [真实振动]完整检测 / [误触发]快速退出 → Sleep
```

### 1.3 关键约束

- ❌ **不改动**：阶段2-5检测算法逻辑（粗检测、FFT、细检测、状态机）
- ✅ **新增**：WOM配置、快速预检、模式切换
- ✅ **修改**：主循环、低功耗管理、状态机扩展

---

## 📦 二、代码架构分析

### 2.1 现有代码结构

```
Core/
├── Inc/
│   ├── main.h                      # 主程序头文件
│   ├── example-raw-data.h          # 检测算法头文件（阶段1-5）
│   ├── fft_processor.h             # FFT处理器头文件
│   ├── low_power_manager.h         # 低功耗管理头文件
│   └── rtc_wakeup.h                # RTC唤醒头文件
├── Src/
│   ├── main.c                      # 主程序（初始化、主循环）
│   ├── example-raw-data.c          # 检测算法实现（阶段1-5）
│   ├── fft_processor.c             # FFT处理器实现
│   ├── low_power_manager.c         # 低功耗管理实现
│   ├── rtc_wakeup.c                # RTC唤醒实现
│   └── stm32f4xx_it.c              # 中断处理
Iim423xx/
├── Iim423xxDriver_HL.h             # 传感器驱动头文件
├── Iim423xxDriver_HL.c             # 传感器驱动实现
├── Iim423xxDriver_HL_apex.h        # APEX功能头文件（WOM）
├── Iim423xxDriver_HL_apex.c        # APEX功能实现（WOM）
└── Iim423xxDefs.h                  # 寄存器定义
```

### 2.2 关键模块依赖关系

```
main.c
  ├─> low_power_manager.c (低功耗管理)
  │     ├─> rtc_wakeup.c (RTC唤醒) [将被WOM替代]
  │     └─> example-raw-data.c (检测算法)
  ├─> example-raw-data.c (检测算法)
  │     ├─> fft_processor.c (FFT处理)
  │     └─> Iim423xxDriver_HL.c (传感器驱动)
  └─> Iim423xxDriver_HL_apex.c (WOM功能) [新增依赖]
```

### 2.3 现有状态机（10状态）

```c
typedef enum {
    STATE_SYSTEM_INIT = 0,          // 系统初始化
    STATE_IDLE_SLEEP,               // 深度睡眠（当前未使用）
    STATE_MONITORING,               // 监测模式
    STATE_COARSE_TRIGGERED,         // 粗检测触发
    STATE_FINE_ANALYSIS,            // 细检测分析
    STATE_MINING_DETECTED,          // 挖掘振动检测
    STATE_ALARM_SENDING,            // 报警发送中
    STATE_ALARM_COMPLETE,           // 报警完成
    STATE_ERROR_HANDLING,           // 错误处理
    STATE_SYSTEM_RESET              // 系统重启
} system_state_t;
```

---

## 🔨 三、开发任务分解

### 阶段1：WOM基础功能开发（第1周）

#### 任务1.1：创建WOM管理器模块

**文件**: `Core/Inc/wom_manager.h` + `Core/Src/wom_manager.c`

**功能需求**:
- WOM配置管理
- LP/LN模式切换
- WOM中断处理
- 统计信息记录

**数据结构设计**:

```c
// WOM配置参数
typedef struct {
    uint8_t wom_threshold_x;        // X轴WOM阈值 (LSB)
    uint8_t wom_threshold_y;        // Y轴WOM阈值 (LSB)
    uint8_t wom_threshold_z;        // Z轴WOM阈值 (LSB)
    uint8_t lp_odr;                 // LP模式ODR (50Hz)
    uint8_t ln_odr;                 // LN模式ODR (1000Hz)
    bool wom_enabled;               // WOM使能标志
} wom_config_t;

// WOM管理器状态
typedef struct {
    wom_config_t config;            // WOM配置
    bool is_lp_mode;                // 当前是否LP模式
    bool wom_triggered;             // WOM触发标志
    uint32_t wom_trigger_count;     // WOM触发计数
    uint32_t false_alarm_count;     // 误触发计数
    uint32_t last_trigger_time;     // 最后触发时间
} wom_manager_t;
```

**核心函数接口**:

```c
// 初始化WOM管理器
int WOM_Init(void);

// 配置WOM参数
int WOM_Configure(uint8_t threshold_x, uint8_t threshold_y, uint8_t threshold_z);

// 使能WOM模式（配置传感器为LP+WOM）
int WOM_Enable(void);

// 禁用WOM模式
int WOM_Disable(void);

// 切换到LN模式（1000Hz）
int WOM_SwitchToLNMode(void);

// 切换到LP+WOM模式（50Hz）
int WOM_SwitchToLPMode(void);

// 检查WOM是否触发
bool WOM_IsTriggered(void);

// 清除WOM触发标志
void WOM_ClearTrigger(void);

// 获取统计信息
void WOM_GetStatistics(uint32_t* trigger_count, uint32_t* false_alarm_count);
```

**实现要点**:

1. **WOM配置流程**（参考数据手册Section 8.7）:
```c
int WOM_Configure(uint8_t threshold_x, uint8_t threshold_y, uint8_t threshold_z) {
    int rc = 0;
    
    // 1. 切换到Bank 0
    rc |= inv_iim423xx_set_reg_bank(&icm_driver, 0);
    
    // 2. 设置加速度计为LP模式，50Hz
    rc |= inv_iim423xx_set_accel_mode(&icm_driver, IIM423XX_ACCEL_CONFIG0_ACCEL_MODE_LP);
    rc |= inv_iim423xx_set_accel_frequency(&icm_driver, IIM423XX_ACCEL_CONFIG0_ODR_50_HZ);
    
    // 3. 设置LP模式时钟
    // 通过INTF_CONFIG1寄存器设置ACCEL_LP_CLK_SEL = 0
    
    // 4. 切换到Bank 4
    rc |= inv_iim423xx_set_reg_bank(&icm_driver, 4);
    
    // 5. 设置WOM阈值
    rc |= inv_iim423xx_configure_smd_wom(&icm_driver, 
                                         threshold_x, 
                                         threshold_y, 
                                         threshold_z,
                                         IIM423XX_SMD_CONFIG_WOM_INT_MODE_ANDED,
                                         IIM423XX_SMD_CONFIG_WOM_MODE_CMP_PREV);
    
    // 6. 切换回Bank 0
    rc |= inv_iim423xx_set_reg_bank(&icm_driver, 0);
    
    // 7. 使能WOM中断到INT1
    rc |= inv_iim423xx_enable_wom(&icm_driver);
    
    // 8. 等待50ms
    HAL_Delay(50);
    
    return rc;
}
```

2. **模式切换流程**:
```c
int WOM_SwitchToLNMode(void) {
    int rc = 0;
    
    // 1. 禁用WOM
    rc |= inv_iim423xx_disable_wom(&icm_driver);
    
    // 2. 设置为LN模式，1000Hz
    rc |= inv_iim423xx_set_accel_mode(&icm_driver, IIM423XX_ACCEL_CONFIG0_ACCEL_MODE_LN);
    rc |= inv_iim423xx_set_accel_frequency(&icm_driver, IIM423XX_ACCEL_CONFIG0_ODR_1_KHZ);
    
    // 3. 等待传感器启动（10ms）
    HAL_Delay(10);
    
    g_wom_manager.is_lp_mode = false;
    return rc;
}

int WOM_SwitchToLPMode(void) {
    // 重新调用WOM_Configure()
    return WOM_Configure(g_wom_manager.config.wom_threshold_x,
                        g_wom_manager.config.wom_threshold_y,
                        g_wom_manager.config.wom_threshold_z);
}
```

**测试验证**:
- [ ] WOM配置成功，寄存器值正确
- [ ] LP模式功耗<0.1mA
- [ ] WOM中断能正常触发
- [ ] 模式切换时间<20ms

---

#### 任务1.2：修改中断处理

**文件**: `Core/Src/stm32f4xx_it.c`

**修改内容**:

1. **添加WOM中断标志**:
```c
// 在文件顶部添加
extern volatile bool g_wom_triggered;  // WOM触发标志
```

2. **修改EXTI中断处理**:
```c
void EXTI9_5_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_7);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == GPIO_PIN_7) {  // INT1引脚（PC7）
        // 设置WOM触发标志
        g_wom_triggered = true;
        
        // 记录触发时间
        extern wom_manager_t g_wom_manager;
        g_wom_manager.last_trigger_time = HAL_GetTick();
        g_wom_manager.wom_trigger_count++;
        
        // 清除中断标志（硬件自动清除）
    }
}
```

**测试验证**:
- [ ] 中断能正常触发
- [ ] 标志位正确设置
- [ ] 中断响应时间<1ms

---

#### 任务1.3：修改GPIO初始化

**文件**: `Core/Src/main.c` - `MX_GPIO_Init()`

**修改内容**:

确保PC7配置为EXTI中断模式（当前已配置，需验证）:

```c
// PC7配置为EXTI中断，上升沿触发
GPIO_InitStruct.Pin = GPIO_PIN_7;
GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;  // 上升沿触发
GPIO_InitStruct.Pull = GPIO_NOPULL;
HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

// 使能EXTI中断
HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
```

**测试验证**:
- [ ] GPIO配置正确
- [ ] 中断优先级合理
- [ ] 中断能正常响应

---

### 阶段2：快速预检算法开发（第2周）

#### 任务2.1：创建快速预检模块

**文件**: `Core/Inc/fast_precheck.h` + `Core/Src/fast_precheck.c`

**功能需求**:
- 采集200样本
- 计算快速RMS
- 判断真实振动或误触发
- 统计信息记录

**数据结构设计**:

```c
#define FAST_PRECHECK_SAMPLES  200      // 快速预检样本数
#define FAST_PRECHECK_THRESHOLD 0.05f   // 快速预检阈值 (0.05g)

typedef struct {
    float32_t samples[FAST_PRECHECK_SAMPLES];  // 样本缓冲区
    uint16_t sample_count;                     // 当前样本数
    float32_t rms_value;                       // 计算的RMS值
    bool is_complete;                          // 是否完成
    bool is_real_vibration;                    // 是否真实振动
    uint32_t start_time;                       // 开始时间
    uint32_t end_time;                         // 结束时间
} fast_precheck_t;
```

**核心函数接口**:

```c
// 初始化快速预检
int FastPrecheck_Init(void);

// 重置快速预检
void FastPrecheck_Reset(void);

// 添加样本
int FastPrecheck_AddSample(float32_t accel_z_g);

// 检查是否完成
bool FastPrecheck_IsComplete(void);

// 获取检测结果
bool FastPrecheck_IsRealVibration(void);

// 获取RMS值
float32_t FastPrecheck_GetRMS(void);

// 获取耗时
uint32_t FastPrecheck_GetDuration(void);
```

**实现要点**:

```c
int FastPrecheck_AddSample(float32_t accel_z_g) {
    if (g_fast_precheck.sample_count >= FAST_PRECHECK_SAMPLES) {
        return -1;  // 已满
    }
    
    // 添加样本
    g_fast_precheck.samples[g_fast_precheck.sample_count++] = accel_z_g;
    
    // 检查是否完成
    if (g_fast_precheck.sample_count == FAST_PRECHECK_SAMPLES) {
        // 计算RMS
        float32_t sum_squares = 0.0f;
        for (uint16_t i = 0; i < FAST_PRECHECK_SAMPLES; i++) {
            sum_squares += g_fast_precheck.samples[i] * g_fast_precheck.samples[i];
        }
        g_fast_precheck.rms_value = sqrtf(sum_squares / FAST_PRECHECK_SAMPLES);
        
        // 判断阈值
        g_fast_precheck.is_real_vibration = (g_fast_precheck.rms_value >= FAST_PRECHECK_THRESHOLD);
        g_fast_precheck.is_complete = true;
        g_fast_precheck.end_time = HAL_GetTick();
        
        // 打印调试信息
        printf("FAST_PRECHECK: RMS=%.4f, Threshold=%.4f, Result=%s, Duration=%lums\r\n",
               g_fast_precheck.rms_value,
               FAST_PRECHECK_THRESHOLD,
               g_fast_precheck.is_real_vibration ? "REAL" : "FALSE",
               g_fast_precheck.end_time - g_fast_precheck.start_time);
    }
    
    return 0;
}
```

**测试验证**:
- [ ] 200样本采集正确
- [ ] RMS计算准确
- [ ] 阈值判断正确
- [ ] 耗时约0.2秒

---

#### 任务2.2：集成快速预检到数据处理流程

**文件**: `Core/Src/example-raw-data.c` - `HandleInvDeviceFifoPacket()`

**修改内容**:

在数据处理流程中添加快速预检逻辑:

```c
void HandleInvDeviceFifoPacket(inv_iim423xx_sensor_event_t * event) {
    // ... 现有代码 ...
    
    float32_t accel_z_g = (float32_t)accel[2] / 8192.0f;
    
    // 检查是否在快速预检阶段
    extern system_state_t g_system_state;
    if (g_system_state == STATE_FAST_PRECHECK) {
        // 添加样本到快速预检
        FastPrecheck_AddSample(accel_z_g);
        
        // 检查是否完成
        if (FastPrecheck_IsComplete()) {
            // 快速预检完成，更新状态
            if (FastPrecheck_IsRealVibration()) {
                // 真实振动，继续完整检测
                g_system_state = STATE_MONITORING;
            } else {
                // 误触发，快速退出
                g_system_state = STATE_FALSE_ALARM;
            }
        }
        return;  // 快速预检阶段不执行后续处理
    }
    
    // ... 现有的阶段1-5处理逻辑 ...
}
```

**测试验证**:
- [ ] 快速预检正确集成
- [ ] 不影响现有检测流程
- [ ] 状态转换正确

---

### 阶段3：系统集成（第3周）

#### 任务3.1：扩展系统状态机

**文件**: `Core/Inc/example-raw-data.h` + `Core/Src/example-raw-data.c`

**修改内容**:

1. **扩展状态枚举**:
```c
typedef enum {
    STATE_SYSTEM_INIT = 0,          // 系统初始化
    STATE_WOM_CONFIG,               // WOM配置状态 [新增]
    STATE_IDLE_SLEEP,               // 深度睡眠（WOM等待）
    STATE_WOM_TRIGGERED,            // WOM触发状态 [新增]
    STATE_FAST_PRECHECK,            // 快速预检状态 [新增]
    STATE_FALSE_ALARM,              // 误触发处理 [新增]
    STATE_MONITORING,               // 监测模式
    STATE_COARSE_TRIGGERED,         // 粗检测触发
    STATE_FINE_ANALYSIS,            // 细检测分析
    STATE_MINING_DETECTED,          // 挖掘振动检测
    STATE_ALARM_SENDING,            // 报警发送中
    STATE_ALARM_COMPLETE,           // 报警完成
    STATE_ERROR_HANDLING,           // 错误处理
    STATE_SYSTEM_RESET              // 系统重启
} system_state_t;
```

2. **添加状态转换函数**:
```c
void System_State_Machine_Process(void) {
    switch (g_system_state) {
        case STATE_SYSTEM_INIT:
            // 初始化完成后进入WOM配置
            g_system_state = STATE_WOM_CONFIG;
            break;
            
        case STATE_WOM_CONFIG:
            // WOM配置完成后进入Sleep
            if (WOM_Enable() == 0) {
                g_system_state = STATE_IDLE_SLEEP;
            }
            break;
            
        case STATE_IDLE_SLEEP:
            // 等待WOM触发（在主循环中处理）
            break;
            
        case STATE_WOM_TRIGGERED:
            // WOM触发后切换到LN模式并开始快速预检
            WOM_SwitchToLNMode();
            FastPrecheck_Reset();
            g_system_state = STATE_FAST_PRECHECK;
            break;
            
        case STATE_FAST_PRECHECK:
            // 快速预检中（在HandleInvDeviceFifoPacket中处理）
            break;
            
        case STATE_FALSE_ALARM:
            // 误触发，重新配置WOM并返回Sleep
            WOM_SwitchToLPMode();
            g_system_state = STATE_IDLE_SLEEP;
            break;
            
        case STATE_MONITORING:
            // 真实振动，执行现有的阶段2-5逻辑
            // ... 现有代码不变 ...
            break;
            
        // ... 其他状态保持不变 ...
    }
}
```

**测试验证**:
- [ ] 状态转换逻辑正确
- [ ] 所有状态都能正常工作
- [ ] 状态机无死锁

---

#### 任务3.2：重构主循环

**文件**: `Core/Src/main.c`

**修改内容**:

创建新的WOM主循环函数:

```c
void WOM_MainLoop(void) {
    printf("WOM_MAIN: Starting WOM main loop...\r\n");
    
    // 初始化WOM管理器
    if (WOM_Init() != 0) {
        printf("WOM_MAIN: ERROR - Failed to initialize WOM manager\r\n");
        return;
    }
    
    // 配置WOM参数（50 LSB ≈ 0.195g）
    if (WOM_Configure(50, 50, 50) != 0) {
        printf("WOM_MAIN: ERROR - Failed to configure WOM\r\n");
        return;
    }
    
    // 主循环
    while (1) {
        // 检查WOM是否触发
        if (WOM_IsTriggered()) {
            printf("WOM_MAIN: WOM triggered!\r\n");
            
            // 更新状态机
            g_system_state = STATE_WOM_TRIGGERED;
            System_State_Machine_Process();
            
            // 执行检测流程
            do {
                // 检查传感器中断
                if (irq_from_device & TO_MASK(INV_GPIO_INT1)) {
                    GetDataFromInvDevice();
                }
                
                // 处理状态机
                System_State_Machine_Process();
                
                // 短暂延时
                HAL_Delay(1);
                
            } while (g_system_state != STATE_IDLE_SLEEP && 
                     g_system_state != STATE_FALSE_ALARM);
            
            // 清除WOM触发标志
            WOM_ClearTrigger();
            
            // 打印统计信息
            uint32_t trigger_count, false_alarm_count;
            WOM_GetStatistics(&trigger_count, &false_alarm_count);
            printf("WOM_MAIN: Statistics - Triggers:%lu, FalseAlarms:%lu\r\n",
                   trigger_count, false_alarm_count);
        }
        
        // 进入Sleep模式等待WOM中断
        printf("WOM_MAIN: Entering sleep mode...\r\n");
        HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
        printf("WOM_MAIN: Woke up from sleep\r\n");
    }
}
```

**测试验证**:
- [ ] 主循环逻辑正确
- [ ] Sleep/Wake正常工作
- [ ] 检测流程完整

---

### 阶段4：测试与优化（第4周）

#### 任务4.1：功能测试

**测试项目**:

1. **WOM触发测试**
   - [ ] 手动振动能触发WOM
   - [ ] 阈值设置合理
   - [ ] 中断响应及时

2. **快速预检测试**
   - [ ] 误触发能正确识别
   - [ ] 真实振动能正确识别
   - [ ] 耗时约0.2-0.3秒

3. **完整检测测试**
   - [ ] 阶段2-5逻辑不变
   - [ ] 检测精度保持
   - [ ] 报警功能正常

4. **模式切换测试**
   - [ ] LP→LN切换正常
   - [ ] LN→LP切换正常
   - [ ] 切换时间<20ms

**测试方法**:
- 使用振动台模拟不同强度振动
- 记录触发次数、误触发率
- 测量功耗和响应时间

---

#### 任务4.2：功耗测试

**测试项目**:

1. **待机功耗测试**
   - [ ] LP+WOM模式功耗<0.15mA
   - [ ] STM32 Sleep功耗<1mA
   - [ ] 总待机功耗<0.2mA

2. **工作功耗测试**
   - [ ] 误触发场景功耗约8mA
   - [ ] 正常振动场景功耗约10-12mA
   - [ ] 挖掘振动场景功耗约12-15mA

3. **平均功耗测试**
   - [ ] 长时间运行平均功耗<1.5mA
   - [ ] 电池寿命>100天（3000mAh）

**测试方法**:
- 使用万用表串联测量电流
- 记录不同场景的功耗
- 计算加权平均功耗

---

#### 任务4.3：WOM阈值优化

**优化目标**:
- 降低误触发率<5%
- 避免漏检真实振动
- 平衡灵敏度和功耗

**优化方法**:

1. **阈值扫描测试**:
```
测试阈值范围：30-100 LSB（0.117g - 0.39g）
步进：10 LSB
每个阈值测试：100次振动 + 100次静止
记录：触发率、误触发率、漏检率
```

2. **环境适应性测试**:
```
测试环境：
- 室内静止环境
- 室外风吹环境
- 车辆经过环境
- 施工现场环境
```

3. **最优阈值确定**:
```
综合评分 = 触发准确率 × 0.6 + (1-误触发率) × 0.4
选择评分最高的阈值
```

**测试验证**:
- [ ] 找到最优阈值
- [ ] 误触发率<5%
- [ ] 漏检率<1%

---

## 📝 四、开发检查清单

### 4.1 代码质量检查

- [ ] 所有函数都有详细注释
- [ ] 所有魔法数字都定义为宏
- [ ] 所有错误都有返回值检查
- [ ] 所有指针都有NULL检查
- [ ] 代码符合项目编码规范

### 4.2 功能完整性检查

- [ ] WOM配置功能完整
- [ ] 快速预检功能完整
- [ ] 模式切换功能完整
- [ ] 状态机扩展完整
- [ ] 主循环重构完整

### 4.3 测试覆盖检查

- [ ] 单元测试覆盖率>80%
- [ ] 集成测试覆盖所有场景
- [ ] 功耗测试数据完整
- [ ] 性能测试数据完整
- [ ] 可靠性测试通过

### 4.4 文档完整性检查

- [ ] 代码注释完整
- [ ] API文档完整
- [ ] 测试报告完整
- [ ] 用户手册更新
- [ ] 技术文档归档

---

## 📊 五、开发进度跟踪

### 5.1 里程碑

| 里程碑 | 完成标准 | 预计时间 |
|--------|---------|---------|
| M1: WOM基础功能 | WOM配置、中断处理完成 | 第1周末 |
| M2: 快速预检算法 | 快速预检模块完成并集成 | 第2周末 |
| M3: 系统集成 | 状态机扩展、主循环重构完成 | 第3周末 |
| M4: 测试优化 | 所有测试通过、阈值优化完成 | 第4周末 |

### 5.2 风险管理

| 风险 | 影响 | 概率 | 应对措施 |
|------|------|------|---------|
| WOM阈值难以调优 | 高 | 中 | 预留额外测试时间，实现自适应算法 |
| 误触发率过高 | 中 | 中 | 优化快速预检算法，调整阈值 |
| 模式切换不稳定 | 高 | 低 | 增加切换延时，添加状态验证 |
| 功耗未达预期 | 中 | 低 | 优化Sleep配置，检查外设功耗 |

---

## 📚 六、关键代码实现细节

### 6.1 传感器驱动API映射

基于现有代码分析，IIM-42352驱动提供以下关键API：

```c
// 已有API（Iim423xxDriver_HL_apex.c）
int inv_iim423xx_configure_smd_wom(struct inv_iim423xx * s,
                                   const uint8_t x_th,
                                   const uint8_t y_th,
                                   const uint8_t z_th,
                                   IIM423XX_SMD_CONFIG_WOM_INT_MODE_t wom_int,
                                   IIM423XX_SMD_CONFIG_WOM_MODE_t wom_mode);

int inv_iim423xx_enable_wom(struct inv_iim423xx * s);
int inv_iim423xx_disable_wom(struct inv_iim423xx * s);

// 需要使用的API（Iim423xxDriver_HL.c）
int inv_iim423xx_set_accel_frequency(struct inv_iim423xx * s,
                                     const IIM423XX_ACCEL_CONFIG0_ODR_t frequency);
int inv_iim423xx_set_accel_fsr(struct inv_iim423xx * s,
                               IIM423XX_ACCEL_CONFIG0_FS_SEL_t accel_fsr_g);
int inv_iim423xx_set_reg_bank(struct inv_iim423xx * s, uint8_t bank);
int inv_iim423xx_read_reg(struct inv_iim423xx * s, uint8_t reg,
                          uint32_t len, uint8_t * buf);
int inv_iim423xx_write_reg(struct inv_iim423xx * s, uint8_t reg,
                           uint32_t len, const uint8_t * buf);
```

**关键寄存器定义**（Iim423xxDefs.h）:

```c
// Bank 0寄存器
#define MPUREG_PWR_MGMT0          0x4E  // 电源管理
#define MPUREG_ACCEL_CONFIG0      0x50  // 加速度计配置
#define MPUREG_SMD_CONFIG         0x57  // WOM配置
#define MPUREG_INTF_CONFIG1       0x4D  // 接口配置

// Bank 4寄存器
#define MPUREG_ACCEL_WOM_X_THR_B4 0x4A  // X轴WOM阈值
#define MPUREG_ACCEL_WOM_Y_THR_B4 0x4B  // Y轴WOM阈值
#define MPUREG_ACCEL_WOM_Z_THR_B4 0x4C  // Z轴WOM阈值

// 电源模式定义
#define IIM423XX_PWR_MGMT0_ACCEL_MODE_OFF  0x00
#define IIM423XX_PWR_MGMT0_ACCEL_MODE_LP   0x02  // Low Power
#define IIM423XX_PWR_MGMT0_ACCEL_MODE_LN   0x03  // Low Noise

// ODR定义
#define IIM423XX_ACCEL_CONFIG0_ODR_50_HZ   0x09
#define IIM423XX_ACCEL_CONFIG0_ODR_1_KHZ   0x06
```

### 6.2 WOM配置完整实现

```c
int WOM_Enable(void) {
    int rc = 0;
    uint8_t data;

    printf("WOM: Configuring sensor to LP+WOM mode...\r\n");

    // 1. 切换到Bank 0
    rc |= inv_iim423xx_set_reg_bank(&icm_driver, 0);
    if (rc != 0) {
        printf("WOM: ERROR - Failed to switch to Bank 0\r\n");
        return rc;
    }

    // 2. 读取当前PWR_MGMT0寄存器
    rc |= inv_iim423xx_read_reg(&icm_driver, MPUREG_PWR_MGMT0, 1, &data);
    if (rc != 0) {
        printf("WOM: ERROR - Failed to read PWR_MGMT0\r\n");
        return rc;
    }

    // 3. 设置加速度计为LP模式
    data &= ~0x03;  // 清除ACCEL_MODE位
    data |= IIM423XX_PWR_MGMT0_ACCEL_MODE_LP;  // 设置为LP模式
    rc |= inv_iim423xx_write_reg(&icm_driver, MPUREG_PWR_MGMT0, 1, &data);
    if (rc != 0) {
        printf("WOM: ERROR - Failed to set LP mode\r\n");
        return rc;
    }
    printf("WOM: Set accelerometer to LP mode\r\n");

    // 4. 设置ODR为50Hz
    rc |= inv_iim423xx_read_reg(&icm_driver, MPUREG_ACCEL_CONFIG0, 1, &data);
    data &= ~0x0F;  // 清除ODR位
    data |= IIM423XX_ACCEL_CONFIG0_ODR_50_HZ;  // 设置为50Hz
    rc |= inv_iim423xx_write_reg(&icm_driver, MPUREG_ACCEL_CONFIG0, 1, &data);
    if (rc != 0) {
        printf("WOM: ERROR - Failed to set ODR to 50Hz\r\n");
        return rc;
    }
    printf("WOM: Set ODR to 50Hz\r\n");

    // 5. 设置LP模式时钟（ACCEL_LP_CLK_SEL = 0）
    rc |= inv_iim423xx_read_reg(&icm_driver, MPUREG_INTF_CONFIG1, 1, &data);
    data &= ~0x08;  // 清除ACCEL_LP_CLK_SEL位
    rc |= inv_iim423xx_write_reg(&icm_driver, MPUREG_INTF_CONFIG1, 1, &data);
    if (rc != 0) {
        printf("WOM: ERROR - Failed to set LP clock\r\n");
        return rc;
    }
    printf("WOM: Set LP clock source\r\n");

    // 6. 切换到Bank 4设置WOM阈值
    rc |= inv_iim423xx_set_reg_bank(&icm_driver, 4);
    if (rc != 0) {
        printf("WOM: ERROR - Failed to switch to Bank 4\r\n");
        return rc;
    }

    // 7. 设置WOM阈值（50 LSB ≈ 0.195g）
    uint8_t thresholds[3] = {
        g_wom_manager.config.wom_threshold_x,
        g_wom_manager.config.wom_threshold_y,
        g_wom_manager.config.wom_threshold_z
    };
    rc |= inv_iim423xx_write_reg(&icm_driver, MPUREG_ACCEL_WOM_X_THR_B4, 3, thresholds);
    if (rc != 0) {
        printf("WOM: ERROR - Failed to set WOM thresholds\r\n");
        return rc;
    }
    printf("WOM: Set WOM thresholds: X=%d, Y=%d, Z=%d\r\n",
           thresholds[0], thresholds[1], thresholds[2]);

    // 8. 切换回Bank 0
    rc |= inv_iim423xx_set_reg_bank(&icm_driver, 0);
    if (rc != 0) {
        printf("WOM: ERROR - Failed to switch back to Bank 0\r\n");
        return rc;
    }

    // 9. 配置WOM模式（使用驱动提供的API）
    rc |= inv_iim423xx_configure_smd_wom(&icm_driver,
                                         g_wom_manager.config.wom_threshold_x,
                                         g_wom_manager.config.wom_threshold_y,
                                         g_wom_manager.config.wom_threshold_z,
                                         IIM423XX_SMD_CONFIG_WOM_INT_MODE_ANDED,
                                         IIM423XX_SMD_CONFIG_WOM_MODE_CMP_PREV);
    if (rc != 0) {
        printf("WOM: ERROR - Failed to configure WOM\r\n");
        return rc;
    }

    // 10. 使能WOM中断
    rc |= inv_iim423xx_enable_wom(&icm_driver);
    if (rc != 0) {
        printf("WOM: ERROR - Failed to enable WOM\r\n");
        return rc;
    }
    printf("WOM: Enabled WOM interrupt\r\n");

    // 11. 等待50ms让传感器稳定
    HAL_Delay(50);

    g_wom_manager.config.wom_enabled = true;
    g_wom_manager.is_lp_mode = true;

    printf("WOM: LP+WOM mode configured successfully\r\n");
    return 0;
}
```

### 6.3 低功耗管理器修改

**文件**: `Core/Src/low_power_manager.c`

需要修改的函数：

```c
// 移除RTC依赖，改为WOM触发
bool LowPower_IsDetectionComplete(void) {
    // 检查是否在误触发状态
    if (g_system_state == STATE_FALSE_ALARM) {
        return true;  // 误触发，快速退出
    }

    // 检查是否在快速预检阶段
    if (g_system_state == STATE_FAST_PRECHECK) {
        return false;  // 快速预检未完成
    }

    // 检查粗检测窗口是否填满
    bool window_full = Coarse_Detector_IsWindowFull();
    if (!window_full) {
        return false;
    }

    // 检查粗检测状态
    coarse_detection_state_t coarse_state = Coarse_Detector_GetState();
    if (coarse_state == COARSE_STATE_IDLE) {
        // 场景1：无显著振动
        return true;
    }

    // 场景2/3：等待状态机完成
    system_state_t current_state = System_State_Machine_GetCurrentState();
    bool state_machine_idle = (current_state == STATE_MONITORING ||
                              current_state == STATE_ALARM_COMPLETE);

    return state_machine_idle;
}
```

### 6.4 主程序初始化修改

**文件**: `Core/Src/main.c` - `main()`函数

```c
int main(void) {
    int rc = 0;

    // 1. HAL库和时钟初始化
    HAL_Init();
    SystemClock_Config();

    // 2. 外设初始化
    MX_GPIO_Init();
    MX_SPI1_Init();
    MX_USART1_UART_Init();
    MX_UART5_Init();

    // 注意：不再初始化RTC
    // #if ENABLE_LOW_POWER_MODE
    // MX_RTC_Init();
    // #endif

    printf("\r\n========================================\r\n");
    printf("STM32 Intelligent Vibration Detection System\r\n");
    printf("Version: v4.0-WOM\r\n");
    printf("Build: %s %s\r\n", __DATE__, __TIME__);
    printf("========================================\r\n\r\n");

    // 3. 传感器初始化（先配置为LN模式用于初始化）
    rc = SetupInvDevice(&iim423xx_serif);
    if (rc != 0) {
        printf("ERROR: Failed to setup sensor: %d\r\n", rc);
        Error_Handler();
    }

    rc = ConfigureInvDevice(IS_LOW_NOISE_MODE,
                           IS_HIGH_RES_MODE,
                           IIM423XX_ACCEL_CONFIG0_FS_SEL_4g,
                           IIM423XX_ACCEL_CONFIG0_ODR_1_KHZ,
                           USE_CLK_IN);
    if (rc != 0) {
        printf("ERROR: Failed to configure sensor: %d\r\n", rc);
        Error_Handler();
    }

    // 4. 初始化检测算法模块
    rc = FFT_Init(true, true);
    if (rc != 0) {
        printf("ERROR: Failed to initialize FFT: %d\r\n", rc);
    }

    FFT_SetTriggerMode(true);

    Coarse_Detector_Init();
    Fine_Detector_Init();

    // 5. 初始化WOM管理器
    rc = WOM_Init();
    if (rc != 0) {
        printf("ERROR: Failed to initialize WOM manager: %d\r\n", rc);
        Error_Handler();
    }

    // 6. 初始化快速预检
    rc = FastPrecheck_Init();
    if (rc != 0) {
        printf("ERROR: Failed to initialize fast precheck: %d\r\n", rc);
        Error_Handler();
    }

    // 7. 初始化系统状态机
    g_system_state = STATE_SYSTEM_INIT;

    printf("\r\nAll modules initialized successfully!\r\n");
    printf("Starting WOM main loop...\r\n\r\n");

    // 8. 进入WOM主循环
    WOM_MainLoop();

    // 不应该到达这里
    while (1) {
        HAL_Delay(1000);
    }
}
```

---

## 📚 七、调试与验证指南

### 7.1 调试输出规范

为了便于调试，所有模块的printf输出应遵循以下格式：

```c
// 格式：[模块名]: [级别] - 消息内容
printf("WOM: INFO - WOM triggered, count=%lu\r\n", count);
printf("WOM: ERROR - Failed to configure, rc=%d\r\n", rc);
printf("FAST_PRECHECK: DEBUG - RMS=%.4f, threshold=%.4f\r\n", rms, threshold);
printf("STATE_MACHINE: INFO - State transition: %d -> %d\r\n", old_state, new_state);
```

**模块前缀**:
- `WOM`: WOM管理器
- `FAST_PRECHECK`: 快速预检
- `STATE_MACHINE`: 状态机
- `MAIN`: 主程序
- `COARSE`: 粗检测
- `FINE`: 细检测
- `FFT`: FFT处理器
- `LORA`: LoRa通信

### 7.2 关键调试点

**阶段1调试点**:
```c
// WOM配置后验证
printf("WOM: DEBUG - Verifying WOM configuration...\r\n");
uint8_t verify_data;
inv_iim423xx_read_reg(&icm_driver, MPUREG_PWR_MGMT0, 1, &verify_data);
printf("WOM: DEBUG - PWR_MGMT0 = 0x%02X (expected: 0x02)\r\n", verify_data);

inv_iim423xx_read_reg(&icm_driver, MPUREG_ACCEL_CONFIG0, 1, &verify_data);
printf("WOM: DEBUG - ACCEL_CONFIG0 = 0x%02X (expected: 0x09)\r\n", verify_data);

inv_iim423xx_set_reg_bank(&icm_driver, 4);
uint8_t thresholds[3];
inv_iim423xx_read_reg(&icm_driver, MPUREG_ACCEL_WOM_X_THR_B4, 3, thresholds);
printf("WOM: DEBUG - WOM thresholds: X=%d, Y=%d, Z=%d\r\n",
       thresholds[0], thresholds[1], thresholds[2]);
inv_iim423xx_set_reg_bank(&icm_driver, 0);
```

**阶段2调试点**:
```c
// 快速预检每10个样本输出一次
if (g_fast_precheck.sample_count % 10 == 0) {
    printf("FAST_PRECHECK: DEBUG - Collected %d/200 samples\r\n",
           g_fast_precheck.sample_count);
}

// 快速预检完成后输出详细信息
printf("FAST_PRECHECK: INFO - Complete! RMS=%.4f, Threshold=%.4f, Result=%s\r\n",
       g_fast_precheck.rms_value,
       FAST_PRECHECK_THRESHOLD,
       g_fast_precheck.is_real_vibration ? "REAL_VIBRATION" : "FALSE_ALARM");
```

**阶段3调试点**:
```c
// 状态转换时输出
void System_State_Machine_SetState(system_state_t new_state) {
    if (g_system_state != new_state) {
        printf("STATE_MACHINE: INFO - Transition: %s -> %s\r\n",
               state_names[g_system_state],
               state_names[new_state]);
        g_system_state = new_state;
    }
}
```

### 7.3 性能测量宏

```c
// 定义性能测量宏
#define PERF_START(name) \
    uint32_t perf_start_##name = HAL_GetTick(); \
    printf("PERF: START - %s\r\n", #name);

#define PERF_END(name) \
    uint32_t perf_end_##name = HAL_GetTick(); \
    printf("PERF: END - %s, duration=%lums\r\n", \
           #name, perf_end_##name - perf_start_##name);

// 使用示例
PERF_START(wom_config);
WOM_Configure(50, 50, 50);
PERF_END(wom_config);

PERF_START(fast_precheck);
// ... 快速预检代码 ...
PERF_END(fast_precheck);
```

---

## 📚 八、参考资料

1. **IIM-42352数据手册** - Section 8.7 Wake on Motion Programming
2. **STM32F407参考手册** - Sleep模式配置
3. **现有代码架构文档** - `STM32智能震动检测系统_代码架构分析文档.md`
4. **纯WOM方案架构设计** - `纯WOM方案_完整架构设计文档.md`
5. **现有驱动代码** - `Iim423xx/Iim423xxDriver_HL_apex.c`

---

## 📋 九、开发交付物清单

### 9.1 代码文件

- [ ] `Core/Inc/wom_manager.h` - WOM管理器头文件
- [ ] `Core/Src/wom_manager.c` - WOM管理器实现
- [ ] `Core/Inc/fast_precheck.h` - 快速预检头文件
- [ ] `Core/Src/fast_precheck.c` - 快速预检实现
- [ ] `Core/Src/main.c` - 主程序修改
- [ ] `Core/Src/example-raw-data.c` - 检测算法修改
- [ ] `Core/Src/low_power_manager.c` - 低功耗管理修改
- [ ] `Core/Src/stm32f4xx_it.c` - 中断处理修改

### 9.2 文档文件

- [ ] 开发日志（记录每日开发进度）
- [ ] 测试报告（功能测试、性能测试、功耗测试）
- [ ] API文档（新增模块的API说明）
- [ ] 用户手册更新（WOM模式使用说明）
- [ ] 技术总结（开发经验、问题解决）

### 9.3 测试数据

- [ ] WOM触发测试数据（触发次数、响应时间）
- [ ] 快速预检测试数据（准确率、耗时）
- [ ] 功耗测试数据（各场景功耗、平均功耗）
- [ ] 阈值优化数据（不同阈值的性能对比）
- [ ] 长时间运行数据（7天连续运行统计）

---

**文档版本**: v1.0
**最后更新**: 2025-10-11
**作者**: AI Assistant
**审核状态**: 待审核

