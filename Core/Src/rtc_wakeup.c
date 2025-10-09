/**
 * @file rtc_wakeup.c
 * @brief STM32 RTC定时唤醒控制实现
 * @author Augment Agent
 * @date 2025-01-08
 * @version v1.0
 * 
 * 为STM32智能震动检测系统低功耗模式提供RTC定时唤醒功能
 * 支持2秒周期唤醒，可通过宏配置调整
 */

#include "rtc_wakeup.h"
#include "low_power_manager.h"
#include <stdio.h>
#include <string.h>

/* 全局变量 */
RTC_HandleTypeDef hrtc;
volatile bool rtc_wakeup_flag = false;
static rtc_wakeup_stats_t g_rtc_stats;
static bool g_rtc_initialized = false;

/**
 * @brief RTC初始化
 */
int RTC_Wakeup_Init(void)
{
    printf("RTC_WAKEUP: Initializing RTC...\r\n");
    
    // 清零统计结构体
    memset(&g_rtc_stats, 0, sizeof(rtc_wakeup_stats_t));
    g_rtc_stats.state = RTC_STATE_UNINITIALIZED;
    
    // 配置RTC时钟源
    int ret = RTC_Wakeup_ConfigureClock();
    if (ret != 0) {
        printf("RTC_WAKEUP: ERROR - Failed to configure RTC clock: %d\r\n", ret);
        g_rtc_stats.state = RTC_STATE_ERROR;
        return -1;
    }
    
    // 初始化RTC
    hrtc.Instance = RTC;
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
    // LSI频率通常在17-47kHz范围内，标称32kHz
    // 使用标准的预分频器配置：AsynchPrediv=127, SynchPrediv=255
    // 这样可以更好地适应LSI频率变化
    hrtc.Init.AsynchPrediv = 127;    // LSI / (127+1) = LSI/128
    hrtc.Init.SynchPrediv = 255;     // (LSI/128) / (255+1) = LSI/32768 ≈ 1Hz (当LSI=32768Hz时)
    hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
    hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
    
    if (HAL_RTC_Init(&hrtc) != HAL_OK) {
        printf("RTC_WAKEUP: ERROR - Failed to initialize RTC\r\n");
        g_rtc_stats.state = RTC_STATE_ERROR;
        return -2;
    }
    
    // 设置初始时间和日期
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
    
    sTime.Hours = 0;
    sTime.Minutes = 0;
    sTime.Seconds = 0;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;
    
    if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) {
        printf("RTC_WAKEUP: WARNING - Failed to set RTC time\r\n");
    }
    
    sDate.WeekDay = RTC_WEEKDAY_MONDAY;
    sDate.Month = RTC_MONTH_JANUARY;
    sDate.Date = 1;
    sDate.Year = 24;  // 2024
    
    if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK) {
        printf("RTC_WAKEUP: WARNING - Failed to set RTC date\r\n");
    }
    
    // 使能RTC唤醒中断
    HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
    printf("RTC_WAKEUP: RTC wakeup interrupt enabled\r\n");

    g_rtc_stats.state = RTC_STATE_INITIALIZED;
    g_rtc_initialized = true;

    printf("RTC_WAKEUP: RTC initialized successfully\r\n");
    return 0;
}

/**
 * @brief 配置RTC唤醒定时器
 */
int RTC_Wakeup_Configure(uint32_t period_sec)
{
    if (!g_rtc_initialized) {
        printf("RTC_WAKEUP: ERROR - RTC not initialized\r\n");
        return -1;
    }
    
    printf("RTC_WAKEUP: Configuring wakeup timer for %lu seconds\r\n", period_sec);
    
    // 停止现有的唤醒定时器
    HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
    
    // 配置唤醒定时器
    int ret = RTC_Wakeup_ConfigureTimer(period_sec);
    if (ret != 0) {
        printf("RTC_WAKEUP: ERROR - Failed to configure wakeup timer: %d\r\n", ret);
        return -2;
    }
    
    g_rtc_stats.wakeup_period_sec = period_sec;
    g_rtc_stats.state = RTC_STATE_WAKEUP_DISABLED;
    
    printf("RTC_WAKEUP: Wakeup timer configured successfully\r\n");
    return 0;
}

/**
 * @brief 启动RTC唤醒定时器
 */
int RTC_Wakeup_Start(void)
{
    if (!g_rtc_initialized) {
        printf("RTC_WAKEUP: ERROR - RTC not initialized\r\n");
        return -1;
    }

    if (g_rtc_stats.state == RTC_STATE_WAKEUP_ENABLED) {
        printf("RTC_WAKEUP: Wakeup timer already started\r\n");
        return 0;  // 已经启动，直接返回成功
    }

    // 清除唤醒标志
    rtc_wakeup_flag = false;
    g_rtc_stats.is_wakeup_pending = false;

    // 定时器已经在Configure函数中设置，这里只需要更新状态
    g_rtc_stats.state = RTC_STATE_WAKEUP_ENABLED;

    if (g_rtc_stats.wakeup_period_sec <= 10) {  // 只在调试模式下打印
        printf("RTC_WAKEUP: Wakeup timer started (%lu sec period)\r\n", g_rtc_stats.wakeup_period_sec);
    }

    return 0;
}

/**
 * @brief 停止RTC唤醒定时器
 */
int RTC_Wakeup_Stop(void)
{
    if (!g_rtc_initialized) {
        return -1;
    }
    
    if (HAL_RTCEx_DeactivateWakeUpTimer(&hrtc) != HAL_OK) {
        printf("RTC_WAKEUP: ERROR - Failed to stop wakeup timer\r\n");
        return -2;
    }
    
    g_rtc_stats.state = RTC_STATE_WAKEUP_DISABLED;
    printf("RTC_WAKEUP: Wakeup timer stopped\r\n");
    
    return 0;
}

/**
 * @brief 处理RTC唤醒中断
 */
int RTC_Wakeup_HandleInterrupt(void)
{
    if (!g_rtc_initialized) {
        return -1;
    }

    // 更新统计信息
    RTC_Wakeup_UpdateStats();

    // 设置唤醒标志
    rtc_wakeup_flag = true;
    g_rtc_stats.is_wakeup_pending = true;

    // 调试信息：使用RTC计数器而不是HAL_GetTick()
    // 因为HAL_GetTick()在Sleep期间被暂停，不能准确反映RTC间隔
    static uint32_t wakeup_count = 0;
    wakeup_count++;

    if (wakeup_count == 1) {
        printf("RTC_WAKEUP: *** FIRST RTC INTERRUPT ***\r\n");
        printf("RTC_WAKEUP: RTC timer is running with %lu sec period\r\n", g_rtc_stats.wakeup_period_sec);
    } else {
        // RTC硬件定时器工作正常，每次中断间隔固定为配置的周期
        printf("RTC_WAKEUP: *** RTC INTERRUPT #%lu *** (period: %lu sec)\r\n",
               wakeup_count, g_rtc_stats.wakeup_period_sec);
    }

    return 0;
}

/**
 * @brief 检查是否有RTC唤醒事件
 */
bool RTC_Wakeup_IsPending(void)
{
    return rtc_wakeup_flag;
}

/**
 * @brief 清除RTC唤醒标志
 */
void RTC_Wakeup_ClearFlag(void)
{
    rtc_wakeup_flag = false;
    g_rtc_stats.is_wakeup_pending = false;
}

/**
 * @brief 获取RTC唤醒统计信息
 */
rtc_wakeup_stats_t* RTC_Wakeup_GetStats(void)
{
    return &g_rtc_stats;
}

/**
 * @brief 打印RTC唤醒统计信息
 */
void RTC_Wakeup_PrintStats(void)
{
    printf("=== RTC WAKEUP STATISTICS ===\r\n");
    printf("State: %d\r\n", g_rtc_stats.state);
    printf("Wakeup count: %lu\r\n", g_rtc_stats.wakeup_count);
    printf("Wakeup period: %lu sec\r\n", g_rtc_stats.wakeup_period_sec);
    printf("Last wakeup: %lu ms\r\n", g_rtc_stats.last_wakeup_time);
    printf("Pending: %s\r\n", g_rtc_stats.is_wakeup_pending ? "YES" : "NO");
    printf("=============================\r\n");
}

/**
 * @brief RTC唤醒中断回调函数
 */
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
    // 处理RTC唤醒中断
    RTC_Wakeup_HandleInterrupt();
}

/**
 * @brief 配置RTC时钟源
 */
static int RTC_Wakeup_ConfigureClock(void)
{
    // 启用PWR时钟
    __HAL_RCC_PWR_CLK_ENABLE();

    // 启用对备份域的访问
    HAL_PWR_EnableBkUpAccess();

    // 启用LSI时钟作为RTC时钟源（更简单，不需要外部晶振）
    __HAL_RCC_LSI_ENABLE();

    // 等待LSI就绪
    uint32_t timeout = 1000;
    while (__HAL_RCC_GET_FLAG(RCC_FLAG_LSIRDY) == RESET && timeout > 0) {
        timeout--;
        HAL_Delay(1);
    }

    if (timeout == 0) {
        printf("RTC_WAKEUP: ERROR - LSI not ready\r\n");
        return -1;
    }

    // 配置RTC时钟源为LSI
    __HAL_RCC_RTC_CONFIG(RCC_RTCCLKSOURCE_LSI);

    // 启用RTC时钟
    __HAL_RCC_RTC_ENABLE();

    printf("RTC_WAKEUP: LSI clock configured successfully\r\n");
    return 0;
}

/**
 * @brief 配置RTC唤醒定时器
 */
static int RTC_Wakeup_ConfigureTimer(uint32_t period_sec)
{
    // 使用RTC_WAKEUPCLOCK_CK_SPRE_16BITS时钟源（1Hz）
    // 计数器值直接等于期望的秒数
    uint32_t wakeup_counter = period_sec;

    // 检查计数值范围（16位计数器，最大65535秒）
    if (wakeup_counter > 0xFFFF) {
        printf("RTC_WAKEUP: ERROR - Wakeup period too long (max 65535 seconds)\r\n");
        return -1;
    }

    printf("RTC_WAKEUP: === TIMER CONFIGURATION ===\r\n");
    printf("RTC_WAKEUP: Wakeup counter value: %lu (period: %lu sec)\r\n", wakeup_counter, period_sec);
    printf("RTC_WAKEUP: Clock source: 0x%X, Clock freq: %lu Hz\r\n", RTC_WAKEUP_CLOCK_SOURCE, RTC_WAKEUP_CLOCK_FREQ);
    printf("RTC_WAKEUP: Expected interval: %lu ms\r\n", period_sec * 1000);
    printf("RTC_WAKEUP: AsynchPrediv: %lu, SynchPrediv: %lu\r\n", (uint32_t)hrtc.Init.AsynchPrediv, (uint32_t)hrtc.Init.SynchPrediv);

    // 配置HAL库的RTC唤醒定时器
    HAL_StatusTypeDef hal_status = HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, wakeup_counter, RTC_WAKEUP_CLOCK_SOURCE);
    if (hal_status != HAL_OK) {
        printf("RTC_WAKEUP: ERROR - Failed to set wakeup timer, HAL status: %d\r\n", hal_status);
        return -2;
    }

    printf("RTC_WAKEUP: Wakeup timer configured successfully\r\n");
    printf("RTC_WAKEUP: Timer should trigger in %lu seconds\r\n", period_sec);
    return 0;
}

/**
 * @brief 更新RTC统计信息
 */
static void RTC_Wakeup_UpdateStats(void)
{
    g_rtc_stats.wakeup_count++;
    g_rtc_stats.last_wakeup_time = HAL_GetTick();
    g_rtc_stats.total_wakeup_time += g_rtc_stats.wakeup_period_sec;
}
