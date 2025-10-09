/**
 * @file rtc_wakeup.h
 * @brief STM32 RTC定时唤醒控制头文件
 * @author Augment Agent
 * @date 2025-01-08
 * @version v1.0
 * 
 * 为STM32智能震动检测系统低功耗模式提供RTC定时唤醒功能
 * 支持2秒周期唤醒，可通过宏配置调整
 */

#ifndef RTC_WAKEUP_H
#define RTC_WAKEUP_H

#ifdef __cplusplus
extern "C" {
#endif

/* 包含头文件 */
#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>

/* RTC配置参数 */
#define RTC_WAKEUP_CLOCK_SOURCE         RTC_WAKEUPCLOCK_CK_SPRE_16BITS  // 时钟源：1Hz时钟（最可靠）
#define RTC_LSI_FREQUENCY               32000                           // LSI频率：约32kHz
#define RTC_WAKEUP_CLOCK_FREQ           1                               // 唤醒时钟频率：1Hz

/* RTC状态枚举 */
typedef enum {
    RTC_STATE_UNINITIALIZED = 0,    // 未初始化
    RTC_STATE_INITIALIZED,          // 已初始化
    RTC_STATE_WAKEUP_ENABLED,       // 唤醒已使能
    RTC_STATE_WAKEUP_DISABLED,      // 唤醒已禁用
    RTC_STATE_ERROR                 // 错误状态
} rtc_state_t;

/* RTC统计结构体 */
typedef struct {
    uint32_t wakeup_count;          // 唤醒次数
    uint32_t wakeup_period_sec;     // 唤醒周期(秒)
    uint32_t last_wakeup_time;      // 上次唤醒时间
    uint32_t total_wakeup_time;     // 总唤醒时间
    rtc_state_t state;              // RTC状态
    bool is_wakeup_pending;         // 唤醒待处理标志
} rtc_wakeup_stats_t;

/* 全局变量声明 */
extern RTC_HandleTypeDef hrtc;
extern volatile bool rtc_wakeup_flag;

/* 核心API接口声明 */

/**
 * @brief RTC初始化
 * @return 0: 成功, <0: 失败
 */
int RTC_Wakeup_Init(void);

/**
 * @brief 配置RTC唤醒定时器
 * @param period_sec 唤醒周期(秒)
 * @return 0: 成功, <0: 失败
 */
int RTC_Wakeup_Configure(uint32_t period_sec);

/**
 * @brief 启动RTC唤醒定时器
 * @return 0: 成功, <0: 失败
 */
int RTC_Wakeup_Start(void);

/**
 * @brief 停止RTC唤醒定时器
 * @return 0: 成功, <0: 失败
 */
int RTC_Wakeup_Stop(void);

/**
 * @brief 处理RTC唤醒中断
 * @return 0: 成功, <0: 失败
 */
int RTC_Wakeup_HandleInterrupt(void);

/**
 * @brief 检查是否有RTC唤醒事件
 * @return true: 有唤醒事件, false: 无唤醒事件
 */
bool RTC_Wakeup_IsPending(void);

/**
 * @brief 清除RTC唤醒标志
 */
void RTC_Wakeup_ClearFlag(void);

/**
 * @brief 获取RTC唤醒统计信息
 * @return RTC统计结构体指针
 */
rtc_wakeup_stats_t* RTC_Wakeup_GetStats(void);

/**
 * @brief 打印RTC唤醒统计信息
 */
void RTC_Wakeup_PrintStats(void);

/**
 * @brief RTC唤醒中断回调函数
 * @param hrtc RTC句柄
 */
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc);

/* 内部函数声明 */
static int RTC_Wakeup_ConfigureClock(void);
static int RTC_Wakeup_ConfigureTimer(uint32_t period_sec);
static void RTC_Wakeup_UpdateStats(void);

#ifdef __cplusplus
}
#endif

#endif /* RTC_WAKEUP_H */
