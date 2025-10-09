/**
 * @file low_power_manager.h
 * @brief STM32智能震动检测系统低功耗模式管理头文件
 * @author Augment Agent
 * @date 2025-01-08
 * @version v1.0
 * 
 * 基于STM32智能震动检测系统v4.0低功耗模式开发需求文档
 * 完全保持现有v4.0检测算法逻辑不变，只改变运行方式从连续运行改为定时唤醒运行
 */

#ifndef LOW_POWER_MANAGER_H
#define LOW_POWER_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

/* 包含头文件 */
#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>

/* Define ARM_MATH_CM4 for STM32F4 series before including arm_math.h */
#ifndef ARM_MATH_CM4
#define ARM_MATH_CM4
#endif
#include "arm_math.h"

/* 低功耗模式配置宏定义 */
#define ENABLE_LOW_POWER_MODE           1       // 使能低功耗模式
#define RTC_WAKEUP_PERIOD_SEC           2       // RTC唤醒周期(秒)
#define LOW_POWER_DEBUG_ENABLED         1       // 低功耗调试使能

/* 功耗管理参数 */
#define SLEEP_ENTRY_DELAY_MS            100     // 进入Sleep前延时
#define WAKEUP_STABILIZE_DELAY_MS       50      // 唤醒后稳定延时
#define SENSOR_STARTUP_DELAY_MS         10      // 传感器启动延时

/* 功耗模式枚举 */
typedef enum {
    POWER_MODE_CONTINUOUS,      // 连续模式(现有v4.0模式)
    POWER_MODE_LOW_POWER,       // 低功耗模式(新增)
    POWER_MODE_ULTRA_LOW,       // 超低功耗模式(预留)
} power_mode_t;

/* 唤醒源枚举 */
typedef enum {
    WAKEUP_SOURCE_NONE = 0,     // 无唤醒源
    WAKEUP_SOURCE_RTC,          // RTC唤醒
    WAKEUP_SOURCE_EXTERNAL,     // 外部中断唤醒
    WAKEUP_SOURCE_UART,         // UART唤醒
} wakeup_source_t;

/* 低功耗状态枚举 */
typedef enum {
    LOW_POWER_STATE_ACTIVE = 0, // 活跃状态
    LOW_POWER_STATE_SLEEP,      // Sleep状态
} low_power_state_t;

/* 低功耗管理结构体 */
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
    low_power_state_t current_state;    // 当前低功耗状态
    wakeup_source_t wakeup_source;      // 唤醒源标识
    uint32_t last_wakeup_time;          // 上次唤醒时间
    uint32_t next_sleep_time;           // 下次Sleep时间
    uint32_t detection_start_time;      // 检测开始时间
    uint32_t detection_end_time;        // 检测结束时间
} low_power_manager_t;

/* 核心API接口声明 */

/**
 * @brief 低功耗管理初始化
 * @return 0: 成功, <0: 失败
 */
int LowPower_Init(void);

/**
 * @brief 进入Sleep模式
 * @return 0: 成功, <0: 失败
 */
int LowPower_EnterSleep(void);

/**
 * @brief 处理RTC唤醒
 * @return 0: 成功, <0: 失败
 */
int LowPower_HandleWakeup(void);

/**
 * @brief 启动现有检测流程（完全复用v4.0逻辑）
 * @return 0: 成功, <0: 失败
 */
int LowPower_StartDetectionProcess(void);

/**
 * @brief 检测完成判断
 * @return true: 检测完成, false: 检测进行中
 */
bool LowPower_IsDetectionComplete(void);

/**
 * @brief 功耗统计更新
 */
void LowPower_UpdatePowerStats(void);

/**
 * @brief 获取功耗统计
 * @return 功耗管理结构体指针
 */
low_power_manager_t* LowPower_GetStats(void);

/**
 * @brief 显示低功耗统计信息
 */
void LowPower_PrintStats(void);

/**
 * @brief 显示三种低功耗场景说明
 */
void LowPower_PrintScenarios(void);

/**
 * @brief 设置功耗模式
 * @param mode 功耗模式
 * @return 0: 成功, <0: 失败
 */
int LowPower_SetMode(power_mode_t mode);

/**
 * @brief 获取当前功耗模式
 * @return 当前功耗模式
 */
power_mode_t LowPower_GetMode(void);

/**
 * @brief 低功耗模式主循环处理
 * @return 0: 成功, <0: 失败
 */
int LowPower_ProcessMainLoop(void);

/**
 * @brief 打印功耗统计信息
 */
void LowPower_PrintStats(void);

/* 内部函数声明（供调试使用） */
void LowPower_SaveSystemState(void);
void LowPower_RestoreSystemState(void);
void LowPower_DisablePeripherals(void);
void LowPower_EnablePeripherals(void);

#ifdef __cplusplus
}
#endif

#endif /* LOW_POWER_MANAGER_H */
