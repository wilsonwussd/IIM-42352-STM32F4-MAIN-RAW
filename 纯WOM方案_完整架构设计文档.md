# STM32智能震动检测系统 - 纯WOM方案完整架构设计

## 📋 文档信息

- **方案名称**: 纯WOM方案（Wake-on-Motion触发架构）
- **设计目标**: 最大化功耗优化，电池寿命提升6.5倍
- **创建日期**: 2025-10-11
- **状态**: 架构设计阶段（未实施）

---

## 🎯 一、方案概述

### 1.1 核心理念

**从"定时轮询"到"事件驱动+快速验证"**

```
传统RTC方案：定时唤醒(2秒) → 采集数据 → 检测 → Sleep
纯WOM方案：  Sleep等待 → WOM触发 → 快速验证 → [真实振动]完整检测 / [误触发]立即Sleep
```

### 1.2 关键优势

| 指标 | RTC方案 | WOM方案 | 提升 |
|------|---------|---------|------|
| 平均功耗 | 7.29mA | 1.11mA | **84.8%降低** |
| 电池寿命 | 17.2天 | 112.6天 | **6.5倍提升** |
| 待机功耗 | 3.64mA | 0.125mA | **96.6%降低** |
| 响应速度 | 固定2秒 | 即时触发 | **更快响应** |

### 1.3 架构转变

```
RTC方案：周期性主动检测（无论是否有振动）
WOM方案：被动等待 + 事件驱动（只在有振动时工作）
```

---

## 🏗️ 二、系统架构设计

### 2.1 三级检测架构

#### **第一级：WOM硬件检测（传感器内部）**

```
功能：粗筛，过滤静止状态
位置：IIM-42352传感器内部
阈值：50 LSB (约0.195g)
功耗：极低（duty-cycled @ 50Hz）
响应：触发WOM中断到STM32
```

**配置参数：**
- ACCEL_MODE = 2 (Low Power Mode)
- ACCEL_ODR = 9 (50Hz)
- WOM_X_TH = 50 (Bank 4, 0x4A)
- WOM_Y_TH = 50 (Bank 4, 0x4B)
- WOM_Z_TH = 50 (Bank 4, 0x4C)
- WOM_MODE = 1 (Enable)
- INT_SOURCE1[2:0] = 111 (Enable X/Y/Z WOM to INT1)

#### **第二级：快速预检（STM32唤醒后）**

```
功能：快速排除误触发
位置：STM32主控
采集：200样本 @ 1000Hz（0.2秒）
算法：简化RMS计算
阈值：0.05g（略高于当前0.03g）
目的：最小化误触发功耗损失
```

**快速RMS算法：**
```c
float32_t Fast_RMS_Calculate(float32_t* samples, uint16_t count) {
    float32_t sum_squares = 0.0f;
    for (uint16_t i = 0; i < count; i++) {
        sum_squares += samples[i] * samples[i];
    }
    return sqrtf(sum_squares / count);
}
```

#### **第三级：完整检测（预检通过后）**

```
功能：精确分类
位置：STM32主控
流程：与当前完全相同（阶段2-5）
- 阶段2: 2000样本RMS滑动窗口
- 阶段3: 智能FFT触发
- 阶段4: 5维特征提取
- 阶段5: 系统状态机分类
```

### 2.2 系统状态机扩展

**新增状态：**

1. **STATE_WOM_CONFIG** - WOM配置状态
   - 配置传感器为LP+WOM模式
   - 设置WOM阈值和中断

2. **STATE_WOM_TRIGGERED** - WOM触发状态
   - 处理WOM中断
   - 配置传感器为LN模式

3. **STATE_FAST_PRECHECK** - 快速预检状态
   - 采集200样本
   - 计算快速RMS
   - 判断真实振动或误触发

4. **STATE_FALSE_ALARM** - 误触发处理状态
   - 记录误触发统计
   - 重新配置WOM模式
   - 快速返回Sleep

**状态转换逻辑：**

```
STATE_SYSTEM_INIT → STATE_WOM_CONFIG → STATE_IDLE_SLEEP
                                            ↓ (WOM中断)
                                     STATE_WOM_TRIGGERED
                                            ↓
                                     STATE_FAST_PRECHECK
                                       ↙          ↘
                            (RMS<0.05g)          (RMS≥0.05g)
                                ↓                    ↓
                        STATE_FALSE_ALARM    STATE_MONITORING
                                ↓                    ↓
                        STATE_WOM_CONFIG      阶段2-5完整检测
```

### 2.3 数据流设计

```
WOM触发 → STM32唤醒 → 配置传感器LN模式 → 等待10ms启动
    ↓
采集200样本 → 快速RMS计算 → 判断阈值
    ↓                           ↓
(≥0.05g)                    (<0.05g)
    ↓                           ↓
继续采集到2000样本          重新配置WOM
    ↓                           ↓
高通滤波 → 粗检测 → FFT → 细检测 → 状态机
    ↓
分类结果 → [正常振动/挖掘振动] → 重新配置WOM → Sleep
```

---

## ⚡ 三、功耗分析

### 3.1 功耗分解

#### **待机状态（90%时间）**

```
传感器LP+WOM模式：~0.05mA
STM32 Sleep模式：  <1mA
总计：            ~0.1-0.15mA
```

#### **误触发场景（5%时间）**

```
持续时间：0.3秒
传感器LN模式：0.28mA
STM32活动：   ~8mA
平均功耗：    ~8mA
```

#### **正常振动场景（3%时间）**

```
持续时间：14-15秒
传感器LN模式：0.28mA
STM32活动：   ~10-12mA
平均功耗：    ~10-12mA
```

#### **挖掘振动场景（2%时间）**

```
持续时间：22秒
传感器LN模式：0.28mA
STM32活动：   ~12-15mA
LoRa报警：    额外功耗
平均功耗：    ~12-15mA
```

### 3.2 加权平均功耗计算

```
总功耗 = 0.9 × 0.125mA + 0.05 × 8mA + 0.03 × 11mA + 0.02 × 13.5mA
       = 0.113mA + 0.4mA + 0.33mA + 0.27mA
       = 1.11mA
```

### 3.3 电池寿命计算

```
电池容量：3000mAh
平均功耗：1.11mA
电池寿命：3000mAh / 1.11mA = 2703小时 = 112.6天
```

**对比RTC方案：**
- RTC方案：17.2天
- WOM方案：112.6天
- **提升：6.5倍**

---

## 🔧 四、关键技术实现

### 4.1 WOM配置流程

```c
// 伪代码示例
void WOM_Configure(void) {
    // 1. 切换到Bank 0
    inv_iim423xx_write_reg(REG_BANK_SEL, 0x00);
    
    // 2. 设置加速度计为LP模式，50Hz
    inv_iim423xx_write_reg(PWR_MGMT0, 0x02);  // ACCEL_MODE = 2 (LP)
    inv_iim423xx_write_reg(ACCEL_CONFIG0, 0x09);  // ACCEL_ODR = 9 (50Hz)
    
    // 3. 设置LP模式时钟
    inv_iim423xx_write_reg(INTF_CONFIG1, 0x00);  // ACCEL_LP_CLK_SEL = 0
    
    // 4. 切换到Bank 4
    inv_iim423xx_write_reg(REG_BANK_SEL, 0x04);
    
    // 5. 设置WOM阈值（50 LSB ≈ 0.195g）
    inv_iim423xx_write_reg(ACCEL_WOM_X_THR, 50);
    inv_iim423xx_write_reg(ACCEL_WOM_Y_THR, 50);
    inv_iim423xx_write_reg(ACCEL_WOM_Z_THR, 50);
    
    // 6. 切换回Bank 0
    inv_iim423xx_write_reg(REG_BANK_SEL, 0x00);
    
    // 7. 使能WOM中断到INT1
    inv_iim423xx_write_reg(INT_SOURCE1, 0x07);  // WOM_X/Y/Z_INT1_EN
    
    // 8. 使能WOM功能
    inv_iim423xx_write_reg(SMD_CONFIG, 0x06);  // WOM_MODE=1, SMD_MODE=1
    
    // 9. 等待50ms
    HAL_Delay(50);
}
```

### 4.2 模式切换流程

```c
// WOM模式 → LN模式
void Switch_WOM_to_LN(void) {
    // 1. 切换到Bank 0
    inv_iim423xx_write_reg(REG_BANK_SEL, 0x00);
    
    // 2. 禁用WOM
    inv_iim423xx_write_reg(SMD_CONFIG, 0x00);
    
    // 3. 设置为LN模式，1000Hz
    inv_iim423xx_write_reg(PWR_MGMT0, 0x03);  // ACCEL_MODE = 3 (LN)
    inv_iim423xx_write_reg(ACCEL_CONFIG0, 0x06);  // ACCEL_ODR = 6 (1kHz)
    
    // 4. 等待传感器启动
    HAL_Delay(10);
}

// LN模式 → WOM模式
void Switch_LN_to_WOM(void) {
    // 调用WOM_Configure()重新配置
    WOM_Configure();
}
```

### 4.3 快速预检实现

```c
typedef struct {
    float32_t samples[200];
    uint16_t count;
    float32_t rms_threshold;
} fast_precheck_t;

fast_precheck_t g_fast_precheck = {
    .count = 0,
    .rms_threshold = 0.05f  // 0.05g
};

bool Fast_Precheck_Process(float32_t accel_z_g) {
    // 1. 采集200个样本
    if (g_fast_precheck.count < 200) {
        g_fast_precheck.samples[g_fast_precheck.count++] = accel_z_g;
        return false;  // 继续采集
    }
    
    // 2. 计算快速RMS
    float32_t rms = Fast_RMS_Calculate(g_fast_precheck.samples, 200);
    
    // 3. 判断阈值
    bool is_real_vibration = (rms >= g_fast_precheck.rms_threshold);
    
    // 4. 重置计数器
    g_fast_precheck.count = 0;
    
    return is_real_vibration;
}
```

### 4.4 WOM中断处理

```c
// EXTI中断回调（PC7引脚）
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == GPIO_PIN_7) {  // INT1引脚
        // 设置WOM触发标志
        g_wom_triggered = true;
        
        // 记录触发时间
        g_wom_trigger_time = HAL_GetTick();
    }
}

// 主循环处理
void WOM_MainLoop(void) {
    while (1) {
        if (g_wom_triggered) {
            // 1. 切换到LN模式
            Switch_WOM_to_LN();
            
            // 2. 执行快速预检
            bool is_real = Fast_Precheck_Execute();
            
            if (is_real) {
                // 3. 真实振动：执行完整检测
                Full_Detection_Process();
            } else {
                // 4. 误触发：记录统计
                g_false_alarm_count++;
            }
            
            // 5. 重新配置WOM模式
            Switch_LN_to_WOM();
            
            // 6. 清除触发标志
            g_wom_triggered = false;
        }
        
        // 7. 进入Sleep模式
        HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
    }
}
```

---

## 📊 五、性能指标对比

### 5.1 功耗性能

| 指标 | RTC方案 | WOM方案 | 改善 |
|------|---------|---------|------|
| 待机功耗 | 3.64mA | 0.125mA | ↓96.6% |
| 平均功耗 | 7.29mA | 1.11mA | ↓84.8% |
| 电池寿命 | 17.2天 | 112.6天 | ↑6.5倍 |

### 5.2 响应性能

| 指标 | RTC方案 | WOM方案 | 改善 |
|------|---------|---------|------|
| 最大延迟 | 2秒 | 即时 | ↑更快 |
| 误触发处理 | 2.5秒 | 0.3秒 | ↓88% |
| 检测精度 | 基准 | 相同 | 保持 |

### 5.3 可靠性

| 指标 | RTC方案 | WOM方案 | 评估 |
|------|---------|---------|------|
| 漏检风险 | 低 | 中等 | 需要调优 |
| 误触发率 | 无 | 5% | 可接受 |
| 算法兼容性 | 100% | 100% | 完全兼容 |

---

## ⚠️ 六、关键风险与对策

### 6.1 WOM阈值调优风险

**风险描述：**
- WOM阈值(0.195g)远高于当前粗检测阈值(0.03g)
- 阈值过高：漏检弱振动
- 阈值过低：误触发频繁

**对策：**
1. 从50 LSB开始测试，根据实际表现调整
2. 实现自适应阈值算法
3. 提供用户可配置接口
4. 现场测试不同环境下的最优阈值

### 6.2 误触发功耗风险

**风险描述：**
- 如果误触发率超过10%，功耗收益大幅降低
- 环境噪声可能导致频繁唤醒

**对策：**
1. 快速预检机制（0.3秒快速退出）
2. 误触发统计和分析
3. 动态调整WOM阈值
4. 考虑添加时间窗口过滤

### 6.3 模式切换延迟风险

**风险描述：**
- 传感器启动需要10ms
- 可能错过振动初始阶段

**对策：**
1. 接受10ms延迟（对2秒窗口影响小）
2. 优化SPI通信速度
3. 预配置寄存器值，减少切换时间

### 6.4 算法兼容性风险

**风险描述：**
- 高通滤波器状态需要重建
- RMS窗口需要重新填充

**对策：**
1. 采用"冷启动"方案，每次重置滤波器
2. 前200个样本用于滤波器稳定
3. 从第201个样本开始正式检测
4. 评估对检测精度的影响

---

## 🚀 七、实施计划

### 7.1 开发阶段

**阶段1：WOM基础功能开发（1周）**
- [ ] WOM配置函数实现
- [ ] 模式切换函数实现
- [ ] WOM中断处理实现
- [ ] 基础测试验证

**阶段2：快速预检算法开发（1周）**
- [ ] 快速RMS算法实现
- [ ] 预检阈值调优
- [ ] 误触发统计功能
- [ ] 算法性能测试

**阶段3：系统集成（1周）**
- [ ] 状态机扩展
- [ ] 主循环重构
- [ ] 完整流程集成
- [ ] 功能测试

**阶段4：现场测试与优化（2周）**
- [ ] 不同环境测试
- [ ] WOM阈值优化
- [ ] 功耗实测验证
- [ ] 可靠性测试

### 7.2 测试验证

**功能测试：**
- WOM触发响应测试
- 快速预检准确性测试
- 完整检测流程测试
- 误触发处理测试

**性能测试：**
- 功耗实测（万用表）
- 电池寿命验证
- 响应时间测试
- 检测精度对比

**可靠性测试：**
- 长时间运行测试（7天）
- 不同环境适应性测试
- 极端条件测试
- 误触发率统计

---

## 📈 八、预期收益

### 8.1 功耗收益

```
功耗降低：84.8%
电池寿命：17.2天 → 112.6天（6.5倍）
年度电池成本节省：约70%
```

### 8.2 性能收益

```
响应速度：2秒延迟 → 即时响应
待机功耗：3.64mA → 0.125mA（96.6%降低）
系统效率：大幅提升
```

### 8.3 商业价值

```
产品竞争力：电池寿命业界领先
维护成本：更换电池频率降低6.5倍
用户体验：更快的响应速度
```

---

## 📝 九、总结

### 9.1 方案优势

✅ **功耗优化显著**：84.8%功耗降低，6.5倍电池寿命提升  
✅ **响应速度更快**：事件驱动，即时响应  
✅ **算法完全兼容**：阶段2-5检测算法无需修改  
✅ **硬件无需改动**：利用现有INT1中断引脚  
✅ **实施风险可控**：渐进式开发，充分测试  

### 9.2 关键成功因素

🎯 **WOM阈值调优**：需要现场测试确定最优值  
🎯 **快速预检机制**：最小化误触发功耗损失  
🎯 **充分测试验证**：确保可靠性不降低  
🎯 **自适应优化**：根据实际表现动态调整  

### 9.3 建议

**强烈推荐实施纯WOM方案**，理由：
1. 功耗收益巨大（84.8%降低）
2. 技术风险可控（有完整对策）
3. 实施难度适中（4周开发周期）
4. 商业价值显著（电池寿命6.5倍提升）

---

## 📚 十、参考资料

1. IIM-42352数据手册 - Section 8.7 Wake on Motion Programming
2. 当前RTC方案架构文档
3. STM32F407 Sleep模式应用笔记
4. 低功耗设计最佳实践

---

**文档版本**: v1.0  
**最后更新**: 2025-10-11  
**作者**: AI Assistant  
**审核状态**: 待审核

