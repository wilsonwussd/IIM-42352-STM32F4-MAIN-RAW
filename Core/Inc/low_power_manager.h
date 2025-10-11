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
#define ENABLE_WOM_MODE                 1       // 使能WOM模式（1=WOM触发，0=RTC定时）
#define RTC_WAKEUP_PERIOD_SEC           2       // RTC唤醒周期(秒) - WOM模式下不使用
#define LOW_POWER_DEBUG_ENABLED         1       // 低功耗调试使能
#define LOW_POWER_DEBUG_VERBOSE         0       // 详细调试输出（0=简洁，1=详细）

/* 功耗管理参数 */
#define SLEEP_ENTRY_DELAY_MS            100     // 进入Sleep前延时
#define WAKEUP_STABILIZE_DELAY_MS       50      // 唤醒后稳定延时
#define SENSOR_STARTUP_DELAY_MS         10      // 传感器启动延时
#define DETECTION_TIMEOUT_MS            5000    // 检测超时时间（5秒）

/* WOM配置参数（参考IIM-42352数据手册Section 8.7） */
#define WOM_THRESHOLD_X                 8       // X轴WOM阈值 (LSB, 8 * 3.9mg ≈ 31mg)
#define WOM_THRESHOLD_Y                 8       // Y轴WOM阈值 (LSB, 8 * 3.9mg ≈ 31mg)
#define WOM_THRESHOLD_Z                 8       // Z轴WOM阈值 (LSB, 8 * 3.9mg ≈ 31mg)
#define WOM_MODE                        1       // WOM模式: 0=与初始值比较, 1=与前一个值比较
#define WOM_INT_MODE                    0       // WOM中断模式: 0=OR(任一轴), 1=AND(所有轴)
#define GUARD_ACCEL_ODR                 50      // LP模式加速度计ODR (Hz)
#define GUARD_ACCEL_FSR                 4       // LP模式加速度计量程 (±g)

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
    WAKEUP_SOURCE_WOM,          // WOM中断唤醒
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
    uint32_t wakeup_period_sec;         // 唤醒周期（RTC模式）
    uint8_t debug_enabled;              // 调试使能
    uint8_t wom_enabled;                // WOM模式使能

    // WOM配置参数
    uint8_t wom_threshold_x;            // X轴WOM阈值
    uint8_t wom_threshold_y;            // Y轴WOM阈值
    uint8_t wom_threshold_z;            // Z轴WOM阈值
    uint8_t wom_mode;                   // WOM比较模式
    uint8_t wom_int_mode;               // WOM中断模式

    // 运行状态
    uint8_t low_power_enabled;          // 低功耗模式使能
    uint8_t is_lp_mode;                 // 当前是否LP模式（WOM模式）
    uint32_t sleep_count;               // Sleep次数统计
    uint32_t wakeup_count;              // 唤醒次数统计
    uint32_t wom_trigger_count;         // WOM触发次数
    uint32_t wom_false_alarm_count;     // WOM误触发次数
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
    uint32_t last_wom_trigger_time;     // 上次WOM触发时间
    uint32_t next_sleep_time;           // 下次Sleep时间
    uint32_t detection_start_time;      // 检测开始时间
    uint32_t detection_end_time;        // 检测结束时间
    uint8_t fast_exit_enabled;          // 快速退出标志（场景1优化）
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
 * @brief 检查检测是否超时
 * @return true: 超时, false: 未超时
 */
bool LowPower_IsDetectionTimeout(void);

/**
 * @brief 检查是否应该退出检测循环（完成或超时）
 * @return true: 应该退出, false: 继续检测
 */
bool LowPower_ShouldExitDetection(void);

/**
 * @brief 检查是否应该快速退出（场景1优化）
 * @return true: 应该快速退出, false: 继续正常处理
 */
bool LowPower_ShouldFastExit(void);

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

/* WOM管理API接口声明 */

/**
 * @brief 配置WOM阈值
 * @param threshold_x X轴阈值 (LSB)
 * @param threshold_y Y轴阈值 (LSB)
 * @param threshold_z Z轴阈值 (LSB)
 * @return 0: 成功, <0: 失败
 */
int LowPower_WOM_Configure(uint8_t threshold_x, uint8_t threshold_y, uint8_t threshold_z);

/**
 * @brief 使能WOM模式（配置传感器为LP+WOM）
 * @return 0: 成功, <0: 失败
 */
int LowPower_WOM_Enable(void);

/**
 * @brief 禁用WOM模式
 * @return 0: 成功, <0: 失败
 */
int LowPower_WOM_Disable(void);

/**
 * @brief 切换到LN模式（1000Hz高性能模式）
 * @return 0: 成功, <0: 失败
 */
int LowPower_WOM_SwitchToLNMode(void);

/**
 * @brief 切换到LP+WOM模式（50Hz低功耗模式）
 * @return 0: 成功, <0: 失败
 */
int LowPower_WOM_SwitchToLPMode(void);

/**
 * @brief 检查WOM是否触发
 * @return true: WOM已触发, false: 未触发
 */
bool LowPower_WOM_IsTriggered(void);

/**
 * @brief 清除WOM触发标志
 */
void LowPower_WOM_ClearTrigger(void);

/**
 * @brief WOM中断处理函数（在EXTI中断中调用）
 */
void LowPower_WOM_IRQHandler(void);

/**
 * @brief 获取WOM统计信息
 * @param trigger_count WOM触发次数
 * @param false_alarm_count 误触发次数
 */
void LowPower_WOM_GetStatistics(uint32_t* trigger_count, uint32_t* false_alarm_count);

/**
 * @brief 读取并打印所有WOM相关寄存器（用于调试）
 */
void LowPower_WOM_DumpRegisters(void);

/* 内部函数声明（供调试使用） */
void LowPower_SaveSystemState(void);
void LowPower_RestoreSystemState(void);
void LowPower_DisablePeripherals(void);
void LowPower_EnablePeripherals(void);

#ifdef __cplusplus
}
#endif

#endif /* LOW_POWER_MANAGER_H */
