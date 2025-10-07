# STM32智能震动检测系统状态机逻辑梳理

## 🔍 状态机总体架构

### **10状态定义**
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
    STATE_SYSTEM_RESET,              // 系统重置
    STATE_COUNT                      // 状态总数
} system_state_t;
```

### **状态机数据结构**
```c
typedef struct {
    system_state_t current_state;    // 当前状态
    system_state_t previous_state;   // 前一状态
    uint32_t state_enter_time;       // 状态进入时间
    uint32_t state_duration;         // 状态持续时间

    // 状态转换条件
    uint8_t coarse_trigger_flag;     // 粗检测触发标志
    uint8_t fine_analysis_result;    // 细检测结果 (0=无效, 1=正常, 2=挖掘)
    uint8_t alarm_send_status;       // 报警发送状态 (0=进行中, 1=成功, 2=失败)
    uint8_t error_code;              // 错误代码

    // 状态统计
    uint32_t state_count[STATE_COUNT];  // 各状态计数
    uint32_t transition_count;       // 状态转换计数
    uint32_t total_detections;       // 总检测次数
    uint32_t mining_detections;      // 挖掘检测次数
    uint32_t false_alarms;           // 误报次数
} system_state_machine_t;
```

## 🔄 状态转换逻辑详解

### **1. STATE_SYSTEM_INIT (系统初始化)**
**功能**: 系统启动时的初始化状态
**处理逻辑**:
```c
static void handle_system_init(void) {
    // 系统初始化完成，直接进入监测模式
    transition_to_state(STATE_MONITORING);
}
```
**转换条件**: 
- ✅ **自动转换** → `STATE_MONITORING`

**特点**: 
- 系统启动后的第一个状态
- 完成初始化后立即转换到监测模式
- 无超时机制，快速转换

---

### **2. STATE_IDLE_SLEEP (深度休眠)**
**功能**: 长时间无活动时的低功耗状态
**处理逻辑**:
```c
static void handle_idle_sleep(void) {
    // 深度休眠逻辑 (阶段6实现)
    // 当前直接进入监测模式
    transition_to_state(STATE_MONITORING);
}
```
**转换条件**:
- ✅ **自动转换** → `STATE_MONITORING` (当前实现)
- 🔄 **唤醒信号** → `STATE_MONITORING` (阶段6实现)

**特点**:
- 为阶段6功耗管理预留的状态
- 当前版本直接转换到监测模式
- 未来将实现真正的低功耗休眠

---

### **3. STATE_MONITORING (监测模式)**
**功能**: 系统的主要工作状态，连续监测震动
**处理逻辑**:
```c
static void handle_monitoring(void) {
    // 检查粗检测触发
    if (g_state_machine.coarse_trigger_flag) {
        g_state_machine.coarse_trigger_flag = 0;  // 清除标志
        transition_to_state(STATE_COARSE_TRIGGERED);
        return;
    }

    // 监测超时检查 (可选)
    uint32_t current_time = HAL_GetTick();
    if (current_time - g_state_machine.state_enter_time > STATE_MONITORING_TIMEOUT_MS) {
        // 长时间无活动，重置状态进入时间
        g_state_machine.state_enter_time = current_time;
    }
}
```
**转换条件**:
- ✅ **粗检测触发** → `STATE_COARSE_TRIGGERED`
- 🔄 **长时间无活动** → `STATE_IDLE_SLEEP` (阶段6实现)

**特点**:
- 系统的核心状态，大部分时间处于此状态
- 等待粗检测算法的触发信号
- 具有超时保护机制

---

### **4. STATE_COARSE_TRIGGERED (粗检测触发)**
**功能**: 粗检测算法检测到震动后的过渡状态
**处理逻辑**:
```c
static void handle_coarse_triggered(void) {
    // 粗检测触发后，等待细检测结果
    transition_to_state(STATE_FINE_ANALYSIS);
}
```
**转换条件**:
- ✅ **自动转换** → `STATE_FINE_ANALYSIS`

**特点**:
- 快速过渡状态，立即启动细检测
- 无条件转换到细检测分析状态
- 为细检测算法做准备

---

### **5. STATE_FINE_ANALYSIS (细检测分析)**
**功能**: 执行5维特征提取和智能分类
**处理逻辑**:
```c
static void handle_fine_analysis(void) {
    // 检查细检测结果
    if (g_state_machine.fine_analysis_result != 0) {
        if (g_state_machine.fine_analysis_result == 2) {
            // 检测到挖掘震动
            g_state_machine.total_detections++;
            g_state_machine.mining_detections++;
            transition_to_state(STATE_MINING_DETECTED);
        } else {
            // 正常震动，返回监测模式
            g_state_machine.total_detections++;
            transition_to_state(STATE_MONITORING);
        }
        g_state_machine.fine_analysis_result = 0;  // 清除结果
        return;
    }

    // 超时检查
    uint32_t current_time = HAL_GetTick();
    if (current_time - g_state_machine.state_enter_time > STATE_FINE_ANALYSIS_TIMEOUT_MS) {
        printf("STATE_WARNING: Fine analysis timeout, returning to monitoring\r\n");
        transition_to_state(STATE_MONITORING);
    }
}
```
**转换条件**:
- ✅ **挖掘震动检测** (`fine_analysis_result == 2`) → `STATE_MINING_DETECTED`
- ✅ **正常震动** (`fine_analysis_result == 1`) → `STATE_MONITORING`
- ✅ **分析超时** (5秒) → `STATE_MONITORING`

**特点**:
- 等待细检测算法的分析结果
- 具有5秒超时保护机制
- 统计检测次数和挖掘检测次数

---

### **6. STATE_MINING_DETECTED (挖掘检测)**
**功能**: 确认检测到挖掘震动，准备触发报警
**处理逻辑**:
```c
static void handle_mining_detected(void) {
    printf("STATE_INFO: Mining vibration detected! Triggering alarm...\r\n");

    // 触发报警 (集成现有的报警状态机)
    extern void Trigger_Alarm_Cycle(void);
    Trigger_Alarm_Cycle();

    transition_to_state(STATE_ALARM_SENDING);
}
```
**转换条件**:
- ✅ **自动转换** → `STATE_ALARM_SENDING`

**特点**:
- 确认挖掘震动检测结果
- 调用现有的报警系统
- 立即转换到报警发送状态

---

### **7. STATE_ALARM_SENDING (报警发送)**
**功能**: 通过LoRa发送报警信息
**处理逻辑**:
```c
static void handle_alarm_sending(void) {
    // 检查报警发送状态
    if (g_state_machine.alarm_send_status == 1) {
        // 报警发送成功
        transition_to_state(STATE_ALARM_COMPLETE);
        g_state_machine.alarm_send_status = 0;  // 清除状态
        return;
    } else if (g_state_machine.alarm_send_status == 2) {
        // 报警发送失败
        printf("STATE_WARNING: Alarm sending failed\r\n");
        g_state_machine.false_alarms++;
        transition_to_state(STATE_ERROR_HANDLING);
        g_state_machine.alarm_send_status = 0;  // 清除状态
        return;
    }

    // 超时检查
    uint32_t current_time = HAL_GetTick();
    if (current_time - g_state_machine.state_enter_time > STATE_ALARM_SENDING_TIMEOUT_MS) {
        printf("STATE_WARNING: Alarm sending timeout\r\n");
        g_state_machine.false_alarms++;
        transition_to_state(STATE_ERROR_HANDLING);
    }
}
```
**转换条件**:
- ✅ **报警发送成功** (`alarm_send_status == 1`) → `STATE_ALARM_COMPLETE`
- ✅ **报警发送失败** (`alarm_send_status == 2`) → `STATE_ERROR_HANDLING`
- ✅ **发送超时** (10秒) → `STATE_ERROR_HANDLING`

**特点**:
- 等待LoRa报警系统的反馈
- 具有10秒超时保护机制
- 统计误报次数

---

### **8. STATE_ALARM_COMPLETE (报警完成)**
**功能**: 报警发送成功后的确认状态
**处理逻辑**:
```c
static void handle_alarm_complete(void) {
    printf("STATE_INFO: Alarm cycle completed successfully\r\n");

    // 报警完成，返回监测模式
    transition_to_state(STATE_MONITORING);
}
```
**转换条件**:
- ✅ **自动转换** → `STATE_MONITORING`

**特点**:
- 确认报警发送成功
- 立即返回监测模式
- 完成一个完整的检测-报警周期

---

### **9. STATE_ERROR_HANDLING (错误处理)**
**功能**: 处理系统错误和异常情况
**处理逻辑**:
```c
static void handle_error_handling(void) {
    printf("STATE_ERROR: Error code %d, recovering...\r\n", g_state_machine.error_code);

    // 错误恢复延迟
    uint32_t current_time = HAL_GetTick();
    if (current_time - g_state_machine.state_enter_time > STATE_ERROR_RECOVERY_DELAY_MS) {
        // 清除错误，返回监测模式
        g_state_machine.error_code = 0;
        transition_to_state(STATE_MONITORING);
    }
}
```
**转换条件**:
- ✅ **错误恢复** (1秒延迟后) → `STATE_MONITORING`
- 🔄 **严重错误** → `STATE_SYSTEM_RESET` (未实现)

**特点**:
- 处理各种错误情况
- 1秒恢复延迟，避免频繁错误
- 自动清除错误代码

---

### **10. STATE_SYSTEM_RESET (系统重置)**
**功能**: 系统重置和重新初始化
**处理逻辑**:
```c
static void handle_system_reset(void) {
    printf("STATE_INFO: System reset requested\r\n");

    // 重置状态机
    memset(&g_state_machine, 0, sizeof(g_state_machine));
    g_state_machine.current_state = STATE_SYSTEM_INIT;
    g_state_machine.state_enter_time = HAL_GetTick();
}
```
**转换条件**:
- ✅ **自动转换** → `STATE_SYSTEM_INIT`

**特点**:
- 完全重置状态机
- 清除所有统计数据
- 重新开始初始化流程

## 🔧 状态机配置参数

```c
#define STATE_MONITORING_TIMEOUT_MS     30000   // 监测状态超时 (30秒)
#define STATE_FINE_ANALYSIS_TIMEOUT_MS  5000    // 细检测分析超时 (5秒)
#define STATE_ALARM_SENDING_TIMEOUT_MS  10000   // 报警发送超时 (10秒)
#define STATE_ERROR_RECOVERY_DELAY_MS   1000    // 错误恢复延迟 (1秒)
```

## 📊 状态机接口函数

### **外部接口**
```c
int System_State_Machine_Init(void);                    // 初始化状态机
void System_State_Machine_Process(void);                // 主处理函数
void System_State_Machine_SetCoarseTrigger(uint8_t triggered);  // 设置粗检测触发
void System_State_Machine_SetFineResult(uint8_t result);        // 设置细检测结果
void System_State_Machine_SetAlarmStatus(uint8_t status);       // 设置报警状态
void System_State_Machine_SetError(uint8_t error_code);         // 设置错误代码
system_state_t System_State_Machine_GetCurrentState(void);      // 获取当前状态
void System_State_Machine_PrintStatus(void);                    // 打印状态信息
```

### **内部函数**
```c
static void transition_to_state(system_state_t new_state);      // 状态转换
static void handle_system_init(void);                           // 各状态处理函数
static void handle_idle_sleep(void);
static void handle_monitoring(void);
// ... 其他状态处理函数
```

---

**STM32智能震动检测系统状态机 - 完整的逻辑梳理和实现细节** 🔄🚀
