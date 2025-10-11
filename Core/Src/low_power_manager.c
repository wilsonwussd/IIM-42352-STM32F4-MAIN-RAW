/**
 * @file low_power_manager.c
 * @brief STM32智能震动检测系统低功耗模式管理实现
 * @author Augment Agent
 * @date 2025-01-08
 * @version v1.0
 * 
 * 基于STM32智能震动检测系统v4.0低功耗模式开发需求文档
 * 完全保持现有v4.0检测算法逻辑不变，只改变运行方式从连续运行改为定时唤醒运行
 */

#include "low_power_manager.h"
#include "rtc_wakeup.h"
#include "main.h"
#include "example-raw-data.h"
#include "Iim423xxDefs.h"
#include "Iim423xxDriver_HL.h"
#include <stdio.h>
#include <string.h>

/* 全局变量 */
static low_power_manager_t g_low_power_manager;
static power_mode_t g_current_power_mode = POWER_MODE_CONTINUOUS;
static bool g_low_power_initialized = false;

/* WOM触发标志（由中断设置，由应用清除） */
/* 注意：不使用static，以便main.c可以访问来区分WOM中断和DATA_RDY中断 */
volatile bool g_wom_triggered = false;
static volatile uint32_t g_wom_isr_count = 0;  // ISR重入计数器

/* 外部变量声明 */
extern volatile uint32_t irq_from_device;
extern struct inv_iim423xx icm_driver;

/* 外部函数声明 */
extern void SystemClock_Config(void);  // 系统时钟配置函数（STOP模式唤醒后需要重新配置）
extern UART_HandleTypeDef huart1;      // 调试串口句柄

/**
 * @brief 低功耗管理初始化
 */
int LowPower_Init(void)
{
    printf("LOW_POWER: Initializing low power manager...\r\n");

    // 清零管理结构体
    memset(&g_low_power_manager, 0, sizeof(low_power_manager_t));

    // 初始化配置参数
    g_low_power_manager.wakeup_period_sec = RTC_WAKEUP_PERIOD_SEC;
    g_low_power_manager.debug_enabled = LOW_POWER_DEBUG_ENABLED;
    g_low_power_manager.low_power_enabled = ENABLE_LOW_POWER_MODE;
    g_low_power_manager.wom_enabled = ENABLE_WOM_MODE;

    // 初始化WOM配置参数
    g_low_power_manager.wom_threshold_x = WOM_THRESHOLD_X;
    g_low_power_manager.wom_threshold_y = WOM_THRESHOLD_Y;
    g_low_power_manager.wom_threshold_z = WOM_THRESHOLD_Z;
    g_low_power_manager.wom_mode = WOM_MODE;
    g_low_power_manager.wom_int_mode = WOM_INT_MODE;

    // 初始化状态
    g_low_power_manager.is_sleeping = false;
    g_low_power_manager.is_lp_mode = false;
    g_low_power_manager.current_state = LOW_POWER_STATE_ACTIVE;
    g_low_power_manager.wakeup_source = WAKEUP_SOURCE_NONE;
    g_low_power_manager.last_wakeup_time = HAL_GetTick();
    g_wom_triggered = false;

#if ENABLE_WOM_MODE
    printf("LOW_POWER: WOM mode enabled\r\n");
    printf("LOW_POWER: WOM thresholds: X=%d (%.1f mg), Y=%d (%.1f mg), Z=%d (%.1f mg)\r\n",
           g_low_power_manager.wom_threshold_x, g_low_power_manager.wom_threshold_x * 3.9f,
           g_low_power_manager.wom_threshold_y, g_low_power_manager.wom_threshold_y * 3.9f,
           g_low_power_manager.wom_threshold_z, g_low_power_manager.wom_threshold_z * 3.9f);
    printf("LOW_POWER: WOM mode: %s\r\n", g_low_power_manager.wom_mode ? "Compare with previous" : "Compare with initial");
    printf("LOW_POWER: WOM INT mode: %s\r\n", g_low_power_manager.wom_int_mode ? "AND" : "OR");

    // 禁用INT1中断防止初始化期间的误触发
    printf("LOW_POWER: Disabling INT1 interrupt during initialization...\r\n");
    HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
    HAL_NVIC_ClearPendingIRQ(EXTI9_5_IRQn);

    // 禁用传感器中的所有中断源（关键！）
    printf("LOW_POWER: Disabling ALL interrupt sources in sensor...\r\n");
    int ret = inv_iim423xx_set_reg_bank(&icm_driver, 0);

    // 禁用INT_SOURCE0（DATA_RDY等）
    uint8_t int_source0 = 0x00;
    ret |= inv_iim423xx_write_reg(&icm_driver, MPUREG_INT_SOURCE0, 1, &int_source0);

    // 禁用INT_SOURCE1（WOM等）
    uint8_t int_source1 = 0x00;
    ret |= inv_iim423xx_write_reg(&icm_driver, MPUREG_INT_SOURCE1, 1, &int_source1);

    // 禁用WOM模式
    uint8_t smd_config = 0;
    ret |= inv_iim423xx_read_reg(&icm_driver, MPUREG_SMD_CONFIG, 1, &smd_config);
    smd_config &= ~BIT_SMD_CONFIG_SMD_MODE_MASK;  // 设置为disabled (0)
    ret |= inv_iim423xx_write_reg(&icm_driver, MPUREG_SMD_CONFIG, 1, &smd_config);

    if (ret != 0) {
        printf("LOW_POWER: ERROR - Failed to disable interrupts: %d\r\n", ret);
        return -1;
    }
    printf("LOW_POWER: All interrupt sources disabled\r\n");

    // 清除任何pending的中断状态
    uint8_t int_status;
    inv_iim423xx_read_reg(&icm_driver, MPUREG_INT_STATUS, 1, &int_status);
    inv_iim423xx_read_reg(&icm_driver, MPUREG_INT_STATUS2, 1, &int_status);
    inv_iim423xx_read_reg(&icm_driver, MPUREG_INT_STATUS3, 1, &int_status);
    HAL_NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
    printf("LOW_POWER: Cleared all interrupt status registers\r\n");
#else
    // 初始化RTC唤醒功能（仅在非WOM模式下）
    int ret = RTC_Wakeup_Init();
    if (ret != 0) {
        printf("LOW_POWER: ERROR - Failed to initialize RTC wakeup: %d\r\n", ret);
        return -1;
    }

    // 配置RTC唤醒周期
    ret = RTC_Wakeup_Configure(g_low_power_manager.wakeup_period_sec);
    if (ret != 0) {
        printf("LOW_POWER: ERROR - Failed to configure RTC wakeup: %d\r\n", ret);
        return -2;
    }
    printf("LOW_POWER: RTC wakeup mode enabled\r\n");
    printf("LOW_POWER: Wakeup period: %lu seconds\r\n", g_low_power_manager.wakeup_period_sec);
#endif

    g_low_power_initialized = true;
    printf("LOW_POWER: Low power manager initialized successfully\r\n");
    printf("LOW_POWER: Debug enabled: %s\r\n", g_low_power_manager.debug_enabled ? "YES" : "NO");

    return 0;
}

/**
 * @brief 进入Sleep模式
 */
int LowPower_EnterSleep(void)
{
    if (!g_low_power_initialized) {
        printf("LOW_POWER: ERROR - Low power manager not initialized\r\n");
        return -1;
    }
    
    if (g_low_power_manager.debug_enabled) {
        printf("LOW_POWER: Entering sleep mode...\r\n");
    }
    
    // 更新统计信息
    g_low_power_manager.sleep_count++;
    g_low_power_manager.is_sleeping = true;
    g_low_power_manager.current_state = LOW_POWER_STATE_SLEEP;
    uint32_t sleep_start_time = HAL_GetTick();
    
    // 保存系统状态
    LowPower_SaveSystemState();
    
    // 关闭不必要的外设
    LowPower_DisablePeripherals();

    // RTC唤醒定时器应该已经在主循环中启动了，这里不需要重复启动
    
    // 延时确保系统稳定
    HAL_Delay(SLEEP_ENTRY_DELAY_MS);
    
    if (g_low_power_manager.debug_enabled) {
        printf("LOW_POWER: System entering sleep mode now...\r\n");
        // 确保调试信息发送完成
        HAL_Delay(10);
    }

#if ENABLE_WOM_MODE
    // WOM模式：清除所有pending的传感器中断状态
    uint8_t int_status, int_status2, int_status3;
    inv_iim423xx_read_reg(&icm_driver, MPUREG_INT_STATUS, 1, &int_status);
    inv_iim423xx_read_reg(&icm_driver, MPUREG_INT_STATUS2, 1, &int_status2);
    inv_iim423xx_read_reg(&icm_driver, MPUREG_INT_STATUS3, 1, &int_status3);

    // 关键：如果INT_STATUS2有WOM标志，说明在进入STOP前就已经触发了
    // 这种情况下应该立即处理，不进入STOP
    if ((int_status2 & 0x07) != 0) {
        printf("LOW_POWER: WOM triggered before entering STOP! (ST2=0x%02X)\r\n", int_status2);
        printf("LOW_POWER: Will process WOM event instead of entering STOP\r\n");
        g_wom_triggered = true;
        g_low_power_manager.wom_trigger_count++;
        g_low_power_manager.is_sleeping = false;
        return 0;  // 不进入STOP，直接返回让main loop处理
    }

    // 清除NVIC pending的中断
    HAL_NVIC_ClearPendingIRQ(EXTI9_5_IRQn);

    // 清除可能的误触发标志
    g_wom_triggered = false;

    if (g_low_power_manager.debug_enabled) {
        printf("LOW_POWER: Cleared pending interrupts before sleep (ST=0x%02X, ST2=0x%02X, ST3=0x%02X)\r\n",
               int_status, int_status2, int_status3);
    }

    // 关键：先打印所有调试信息，再禁用SysTick
    printf("LOW_POWER: Enabling WOM interrupt for wakeup...\r\n");
    printf("LOW_POWER: System will enter STOP mode now...\r\n");

    // 等待UART发送完成（在禁用SysTick之前）
    HAL_Delay(100);  // 给足够时间让UART发送完成
#endif

    // 使能EXTI9_5中断，以便WOM触发时唤醒系统
    HAL_NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

    // 禁用SysTick定时器（STOP模式必须禁用）
    // 注意：必须在所有HAL_Delay()之后禁用
    CLEAR_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);

#if !ENABLE_WOM_MODE
    // 非WOM模式：禁用传感器GPIO中断（PC7）
    HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
#endif

    // 进入STOP模式（低功耗稳压器，WFI指令）
    // 相比SLEEP模式，STOP模式功耗更低，但唤醒后需要重新配置时钟
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

    // ===== 从STOP模式唤醒后，代码从这里继续执行 =====

    // 关键：重新配置系统时钟（STOP模式下HSE和PLL会关闭，唤醒后需要重新配置）
    SystemClock_Config();

    // 关键：重新初始化UART（STOP模式下UART时钟停止，可能导致状态异常）
    // 必须在SystemClock_Config()之后，因为UART需要时钟
    HAL_StatusTypeDef uart_status;
    uart_status = HAL_UART_DeInit(&huart1);
    uart_status = HAL_UART_Init(&huart1);

    // 添加延时，确保UART完全初始化
    HAL_Delay(100);

    // 测试：直接发送字符，不使用printf
    const char test_msg[] = "\r\n=== WAKEUP TEST ===\r\n";
    HAL_UART_Transmit(&huart1, (uint8_t*)test_msg, sizeof(test_msg)-1, 1000);

#if !ENABLE_WOM_MODE
    // 非WOM模式：恢复传感器GPIO中断
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
#endif

    // 重新使能SysTick定时器
    SET_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);

    // 唤醒后执行
    printf("\r\n========================================\r\n");
    printf("*** WOKE UP FROM STOP MODE ***\r\n");
    printf("========================================\r\n");

    // 验证并打印WOM触发信息（在ISR中无法打印和验证）
    if (g_wom_triggered) {
        // 现在时钟已恢复，读取传感器中断状态进行验证
        uint8_t int_status, int_status2, int_status3;
        inv_iim423xx_read_reg(&icm_driver, MPUREG_INT_STATUS, 1, &int_status);
        inv_iim423xx_read_reg(&icm_driver, MPUREG_INT_STATUS2, 1, &int_status2);
        inv_iim423xx_read_reg(&icm_driver, MPUREG_INT_STATUS3, 1, &int_status3);

        printf("Sensor interrupt status: ST=0x%02X, ST2=0x%02X, ST3=0x%02X\r\n",
               int_status, int_status2, int_status3);

        // 验证是否真的是WOM触发（检查INT_STATUS2的WOM标志）
        if ((int_status2 & 0x07) == 0) {
            // 不是WOM触发，是误触发！
            printf("*** FALSE WOM TRIGGER DETECTED! ***\r\n");
            printf("INT_STATUS2 WOM flags are all 0, this is not a real WOM event\r\n");
            g_low_power_manager.wom_false_alarm_count++;
            g_wom_triggered = false;  // 清除标志
            printf("False alarm count: %lu\r\n", g_low_power_manager.wom_false_alarm_count);
        } else {
            // 真正的WOM触发
            printf("*** REAL WOM TRIGGER CONFIRMED! ***\r\n");
            printf("WOM_X=%d, WOM_Y=%d, WOM_Z=%d\r\n",
                   int_status2 & 0x01,
                   (int_status2 >> 1) & 0x01,
                   (int_status2 >> 2) & 0x01);
            printf("WOM Trigger Count: %lu\r\n", g_low_power_manager.wom_trigger_count);
            printf("WOM ISR Reentry Count: %lu\r\n", g_wom_isr_count);
            printf("Last Trigger Time: %lu ms\r\n", g_low_power_manager.last_wom_trigger_time);
        }
    }
    printf("========================================\r\n\r\n");

    g_low_power_manager.is_sleeping = false;
    uint32_t sleep_end_time = HAL_GetTick();
    uint32_t sleep_duration = sleep_end_time - sleep_start_time;

    // 注意：由于HAL_SuspendTick()，sleep_duration只包含进入/退出Sleep的开销
    // 实际Sleep时间由RTC周期决定（例如2秒），但HAL_GetTick()在Sleep期间不更新
    // 所以这里的sleep_duration不代表真实的Sleep时间
    g_low_power_manager.total_sleep_time_sec += sleep_duration / 1000;

    if (g_low_power_manager.debug_enabled) {
        // 检查唤醒原因
        if (RTC_Wakeup_IsPending()) {
            printf("LOW_POWER: Woke up from RTC interrupt\r\n");
            printf("LOW_POWER: Note: Actual sleep time is ~%lu sec (RTC period), HAL_GetTick() overhead: %lu ms\r\n",
                   g_low_power_manager.wakeup_period_sec, sleep_duration);
        } else {
            printf("LOW_POWER: Woke up from OTHER interrupt (overhead: %lu ms)\r\n", sleep_duration);
        }
    }
    
    // 恢复系统状态
    LowPower_RestoreSystemState();
    
    // 重新启用外设
    LowPower_EnablePeripherals();
    
    // 稳定延时
    HAL_Delay(WAKEUP_STABILIZE_DELAY_MS);
    
    return 0;
}

/**
 * @brief 处理RTC唤醒
 */
int LowPower_HandleWakeup(void)
{
    if (!g_low_power_initialized) {
        return -1;
    }
    
    // 更新统计信息
    g_low_power_manager.wakeup_count++;
    g_low_power_manager.last_wakeup_time = HAL_GetTick();
    g_low_power_manager.wakeup_source = WAKEUP_SOURCE_RTC;
    g_low_power_manager.is_sleeping = false;
    g_low_power_manager.current_state = LOW_POWER_STATE_ACTIVE;
    
    if (g_low_power_manager.debug_enabled) {
        printf("LOW_POWER: RTC wakeup handled (count: %lu)\r\n", g_low_power_manager.wakeup_count);
    }
    
    // 清除RTC唤醒标志
    RTC_Wakeup_ClearFlag();
    
    return 0;
}

/**
 * @brief 启动现有检测流程（完全复用v4.0逻辑）
 */
int LowPower_StartDetectionProcess(void)
{
    if (!g_low_power_initialized) {
        return -1;
    }
    
    g_low_power_manager.detection_count++;
    g_low_power_manager.detection_start_time = HAL_GetTick();
    
    if (g_low_power_manager.debug_enabled) {
        printf("LOW_POWER: Starting detection process (count: %lu)\r\n", g_low_power_manager.detection_count);
        printf("LOW_POWER: === SCENARIO ANALYSIS START ===\r\n");
        printf("LOW_POWER: Will collect 2000 samples (2 seconds) for coarse detection\r\n");
        printf("LOW_POWER: Sensor startup delay: %d ms\r\n", SENSOR_STARTUP_DELAY_MS);

#if ENABLE_SYSTEM_STATE_MACHINE
        // 显示当前系统状态
        system_state_t current_state = System_State_Machine_GetCurrentState();
        printf("LOW_POWER: Current system state: %d\r\n", current_state);
#endif
    }
    
    // 传感器启动延时
    HAL_Delay(SENSOR_STARTUP_DELAY_MS);

    // 重置快速退出标志
    g_low_power_manager.fast_exit_enabled = 0;

#if ENABLE_COARSE_DETECTION
    // 重置粗检测窗口，确保每次唤醒都重新收集2000个样本
    // 这样可以确保每次检测都基于最新的2秒数据
    extern void Coarse_Detector_Reset(void);
    Coarse_Detector_Reset();
    if (g_low_power_manager.debug_enabled) {
        printf("LOW_POWER: Coarse detector window reset for fresh data collection\r\n");
    }
#endif

    // 注意：这里不直接调用现有的检测函数，而是让主循环继续运行现有的检测逻辑
    // 现有的检测流程会在主循环中自动执行：
    // 1. 传感器中断触发数据采集
    // 2. 5Hz高通滤波处理
    // 3. 粗检测算法（RMS滑动窗口）
    // 4. 智能FFT触发控制
    // 5. 细检测算法（5维特征提取）
    // 6. LoRa报警（如果检测到挖掘振动）

    return 0;
}

/**
 * @brief 检测完成判断
 */
bool LowPower_IsDetectionComplete(void)
{
    if (!g_low_power_initialized) {
        return true;  // 如果未初始化，认为检测完成
    }

    // 优化的检测完成判断逻辑：
    // 1. 粗检测RMS窗口必须已满（确保收集了足够的样本）
    // 2. 如果粗检测未触发 -> 立即完成（场景1：快速完成）
    // 3. 如果粗检测触发 -> 等待状态机完成（场景2/3）

#if ENABLE_COARSE_DETECTION
    // 首先检查粗检测窗口是否已满
    // 这确保至少收集了2000个样本（2秒@1000Hz）
    extern bool Coarse_Detector_IsWindowFull(void);
    extern coarse_detection_state_t Coarse_Detector_GetState(void);

    bool window_full = Coarse_Detector_IsWindowFull();
    if (!window_full) {
        // 窗口未满，继续收集数据
        return false;
    }

    // 声明detection_complete变量（在所有分支之前）
    bool detection_complete = false;

    // 窗口已满，检查粗检测状态
    coarse_detection_state_t coarse_state = Coarse_Detector_GetState();
    if (coarse_state == COARSE_STATE_IDLE) {
        // 粗检测未触发，立即完成（场景1：无振动）
        // 设置快速退出标志，通知主循环停止处理传感器数据
        g_low_power_manager.fast_exit_enabled = 1;
        detection_complete = true;
    } else {
        // 粗检测已触发，需要等待完整的检测流程完成
        g_low_power_manager.fast_exit_enabled = 0;

#if ENABLE_SYSTEM_STATE_MACHINE
        // 获取系统状态机状态
        system_state_t current_state = System_State_Machine_GetCurrentState();
        // 只有在MONITORING或ALARM_COMPLETE状态时才认为检测完成
        // 如果在COARSE_TRIGGERED、FINE_ANALYSIS、MINING_DETECTED、ALARM_SENDING等状态，需要继续处理
        bool state_machine_idle = (current_state == STATE_MONITORING ||
                                  current_state == STATE_IDLE_SLEEP ||
                                  current_state == STATE_ALARM_COMPLETE);

        detection_complete = state_machine_idle;
#else
        // 如果状态机未启用，窗口满且粗检测触发后也认为完成
        detection_complete = true;
#endif
    }
#else
    // 如果粗检测未启用，直接认为完成
    bool detection_complete = true;
#endif
    
    if (detection_complete && g_low_power_manager.detection_start_time > 0) {
        // 更新检测结束时间和统计
        g_low_power_manager.detection_end_time = HAL_GetTick();
        uint32_t detection_duration = g_low_power_manager.detection_end_time - g_low_power_manager.detection_start_time;
        g_low_power_manager.total_active_time_ms += detection_duration;

        if (g_low_power_manager.debug_enabled) {
            printf("LOW_POWER: Detection completed (duration: %lu ms)\r\n", detection_duration);
            printf("LOW_POWER: === SCENARIO ANALYSIS END ===\r\n");

            // 优化的场景判断：基于实际检测状态而不是时间
#if ENABLE_COARSE_DETECTION && ENABLE_SYSTEM_STATE_MACHINE
            extern coarse_detection_state_t Coarse_Detector_GetState(void);
            coarse_detection_state_t coarse_state = Coarse_Detector_GetState();
            system_state_t current_state = System_State_Machine_GetCurrentState();

            if (coarse_state == COARSE_STATE_IDLE) {
                // 粗检测未触发 -> 场景1
                printf("LOW_POWER: >>> SCENARIO 1: No vibration detected (quick completion)\r\n");
                printf("LOW_POWER: >>> Expected: 2000 samples (2s) -> Coarse [NOT TRIGGERED] -> Sleep\r\n");
                printf("LOW_POWER: >>> Actual: Window full, no trigger, fast completion\r\n");
            } else if (current_state == STATE_MONITORING &&
                      (coarse_state == COARSE_STATE_TRIGGERED || coarse_state == COARSE_STATE_COOLDOWN)) {
                // 粗检测触发但未进入报警 -> 场景2
                printf("LOW_POWER: >>> SCENARIO 2: Normal vibration detected (moderate duration)\r\n");
                printf("LOW_POWER: >>> Expected: 2000 samples -> Coarse [TRIGGERED] -> FFT -> Fine [NORMAL] -> Sleep\r\n");
                printf("LOW_POWER: >>> Actual: Coarse triggered, fine analysis completed, no alarm\r\n");
            } else if (current_state == STATE_ALARM_COMPLETE) {
                // 完成报警流程 -> 场景3
                printf("LOW_POWER: >>> SCENARIO 3: Mining vibration detected with alarm\r\n");
                printf("LOW_POWER: >>> Expected: Full detection chain + LoRa alarm (~5-7 seconds)\r\n");
                printf("LOW_POWER: >>> Actual: Mining detected, alarm sent successfully\r\n");
            } else {
                // 其他情况（例如错误恢复）
                printf("LOW_POWER: >>> SCENARIO: Other (state=%d, coarse_state=%d)\r\n", current_state, coarse_state);
            }
#else
            // 如果没有启用检测算法，使用时间判断（兼容模式）
            if (detection_duration < 2200) {
                printf("LOW_POWER: >>> SCENARIO 1: Quick completion (duration < 2.2s)\r\n");
            } else if (detection_duration < 4000) {
                printf("LOW_POWER: >>> SCENARIO 2: Moderate duration (2.2s - 4s)\r\n");
            } else {
                printf("LOW_POWER: >>> SCENARIO 3: Long duration (> 4s)\r\n");
            }
#endif
            printf("LOW_POWER: >>> Power saving achieved: Sleep mode between detections\r\n");
        }

        // 重置检测开始时间
        g_low_power_manager.detection_start_time = 0;
    }
    
    return detection_complete;
}

/**
 * @brief 检查检测是否超时
 */
bool LowPower_IsDetectionTimeout(void)
{
    if (!g_low_power_initialized) {
        return false;
    }

    // 如果检测未开始，不算超时
    if (g_low_power_manager.detection_start_time == 0) {
        return false;
    }

    // 计算已经过的时间
    uint32_t elapsed_ms = HAL_GetTick() - g_low_power_manager.detection_start_time;

    // 检查是否超时
    if (elapsed_ms > DETECTION_TIMEOUT_MS) {
        if (g_low_power_manager.debug_enabled) {
            printf("LOW_POWER: ⚠️ Detection timeout! Elapsed: %lu ms (limit: %d ms)\r\n",
                   elapsed_ms, DETECTION_TIMEOUT_MS);
            printf("LOW_POWER: Forcing detection completion and returning to sleep mode\r\n");
        }

        // 更新统计
        g_low_power_manager.detection_end_time = HAL_GetTick();
        g_low_power_manager.total_active_time_ms += elapsed_ms;
        g_low_power_manager.detection_start_time = 0;  // 重置检测开始时间

        return true;
    }

    return false;
}

/**
 * @brief 检查是否应该退出检测循环（完成或超时）
 */
bool LowPower_ShouldExitDetection(void)
{
    if (!g_low_power_initialized) {
        return true;
    }

    // 检查超时（优先级最高）
    if (LowPower_IsDetectionTimeout()) {
        if (g_low_power_manager.debug_enabled) {
            printf("LOW_POWER: Exiting detection due to TIMEOUT\r\n");
        }
        return true;
    }

    // 检查检测完成
    if (LowPower_IsDetectionComplete()) {
        if (g_low_power_manager.debug_enabled) {
            printf("LOW_POWER: Exiting detection due to COMPLETION\r\n");
        }
        return true;
    }

    // 继续检测
    return false;
}

/**
 * @brief 检查是否应该快速退出（场景1优化）
 *
 * 当粗检测未触发时，不需要继续处理传感器FIFO中的剩余数据
 * 可以立即退出主循环，节省时间
 *
 * @return true: 应该快速退出, false: 继续正常处理
 */
bool LowPower_ShouldFastExit(void)
{
    if (!g_low_power_initialized) {
        return false;
    }

    return g_low_power_manager.fast_exit_enabled;
}

/**
 * @brief 功耗统计更新
 */
void LowPower_UpdatePowerStats(void)
{
    if (!g_low_power_initialized) {
        return;
    }
    
    // 计算平均功耗（简化估算）
    uint32_t total_time_sec = g_low_power_manager.total_sleep_time_sec + 
                             (g_low_power_manager.total_active_time_ms / 1000);
    
    if (total_time_sec > 0) {
        // 估算平均功耗：Sleep时0.1mA，Active时10mA
        float sleep_power = g_low_power_manager.total_sleep_time_sec * 0.1f;
        float active_power = (g_low_power_manager.total_active_time_ms / 1000.0f) * 10.0f;
        g_low_power_manager.average_power_ma = (sleep_power + active_power) / total_time_sec;
    }
}

/**
 * @brief 获取功耗统计
 */
low_power_manager_t* LowPower_GetStats(void)
{
    LowPower_UpdatePowerStats();
    return &g_low_power_manager;
}

/**
 * @brief 显示低功耗统计信息
 */
void LowPower_PrintStats(void)
{
    if (!g_low_power_initialized) {
        return;
    }

    LowPower_UpdatePowerStats();

    printf("=== LOW POWER STATISTICS ===\r\n");
    printf("Sleep count: %lu\r\n", g_low_power_manager.sleep_count);
    printf("Wakeup count: %lu\r\n", g_low_power_manager.wakeup_count);
    printf("Detection count: %lu\r\n", g_low_power_manager.detection_count);
    printf("Total sleep time: %lu sec\r\n", g_low_power_manager.total_sleep_time_sec);
    printf("Total active time: %lu ms\r\n", g_low_power_manager.total_active_time_ms);
    printf("Average power: %.2f mA\r\n", g_low_power_manager.average_power_ma);
    printf("Current state: %s\r\n",
           (g_low_power_manager.current_state == LOW_POWER_STATE_SLEEP) ? "SLEEP" : "ACTIVE");

    // 计算功耗优化效果
    if (g_low_power_manager.total_active_time_ms > 0) {
        uint32_t total_time_ms = g_low_power_manager.total_sleep_time_sec * 1000 + g_low_power_manager.total_active_time_ms;
        float sleep_ratio = (float)(g_low_power_manager.total_sleep_time_sec * 1000) / total_time_ms * 100.0f;
        printf("Sleep ratio: %.1f%% (Power saving achieved!)\r\n", sleep_ratio);
    }

    printf("============================\r\n");
}

/**
 * @brief 显示三种低功耗场景说明
 */
void LowPower_PrintScenarios(void)
{
    printf("=== LOW POWER MODE SCENARIOS (2000-sample window) ===\r\n");
    printf("SCENARIO 1: No Vibration\r\n");
    printf("  Sleep -> RTC Wakeup -> 2000 samples (2s) -> Coarse [NOT TRIGGERED] -> Sleep\r\n");
    printf("  Duration: ~2000-2200ms, Power saving: Maximum\r\n");
    printf("\r\n");
    printf("SCENARIO 2: Normal Vibration\r\n");
    printf("  Sleep -> RTC Wakeup -> 2000 samples (2s) -> Coarse [TRIGGERED] -> 512 samples\r\n");
    printf("  -> FFT -> Fine detection [NORMAL] -> Sleep\r\n");
    printf("  Duration: 2200-4000ms, Power saving: Good\r\n");
    printf("\r\n");
    printf("SCENARIO 3: Mining Vibration\r\n");
    printf("  Sleep -> RTC Wakeup -> 2000 samples (2s) -> Coarse [TRIGGERED] -> 512 samples\r\n");
    printf("  -> FFT -> Fine detection [MINING] -> LoRa Alarm -> Sleep\r\n");
    printf("  Duration: >4000ms (~5-7s), Power saving: Moderate (due to alarm)\r\n");
    printf("================================\r\n");
}

/**
 * @brief 设置功耗模式
 */
int LowPower_SetMode(power_mode_t mode)
{
    g_current_power_mode = mode;
    printf("LOW_POWER: Power mode set to %d\r\n", mode);
    return 0;
}

/**
 * @brief 获取当前功耗模式
 */
power_mode_t LowPower_GetMode(void)
{
    return g_current_power_mode;
}



/**
 * @brief 保存系统状态
 */
void LowPower_SaveSystemState(void)
{
    // 这里可以保存需要在Sleep期间保持的状态
    // 当前实现中，大部分状态由硬件和HAL库自动保持
}

/**
 * @brief 恢复系统状态
 */
void LowPower_RestoreSystemState(void)
{
    // 这里可以恢复Sleep前保存的状态
    // 当前实现中，大部分状态由硬件和HAL库自动恢复
}

/**
 * @brief 关闭不必要的外设
 */
void LowPower_DisablePeripherals(void)
{
    // 在Sleep模式下，大部分外设会自动进入低功耗状态
    // 这里可以显式关闭一些不需要的外设
    // 注意：保持RTC和UART1（调试用）运行
}

/**
 * @brief 重新启用外设
 */
void LowPower_EnablePeripherals(void)
{
    // 唤醒后重新启用外设
    // 大部分外设会自动恢复，这里处理需要手动恢复的外设
}

/**
 * @brief 低功耗模式主循环处理
 */
int LowPower_ProcessMainLoop(void)
{
    if (!g_low_power_initialized) {
        printf("LOW_POWER: ERROR - Not initialized\r\n");
        return -1;
    }

    // 检查当前功耗模式
    if (g_current_power_mode != POWER_MODE_LOW_POWER) {
        // 非低功耗模式，直接返回
        return 0;
    }

#if ENABLE_WOM_MODE
    // WOM模式：检查WOM触发事件
    if (LowPower_WOM_IsTriggered()) {
        if (g_low_power_manager.debug_enabled) {
            printf("LOW_POWER: Processing WOM wakeup event\r\n");
        }

        // 更新唤醒统计
        g_low_power_manager.wakeup_count++;
        g_low_power_manager.last_wakeup_time = HAL_GetTick();
        g_low_power_manager.wakeup_source = WAKEUP_SOURCE_WOM;

        // 清除WOM触发标志
        LowPower_WOM_ClearTrigger();

        // 切换到LN模式进行检测
        int ret = LowPower_WOM_SwitchToLNMode();
        if (ret != 0) {
            printf("LOW_POWER: ERROR - Failed to switch to LN mode: %d\r\n", ret);
            return -2;
        }

        // 启动检测流程
        ret = LowPower_StartDetectionProcess();
        if (ret != 0) {
            printf("LOW_POWER: ERROR - Failed to start detection: %d\r\n", ret);
            return -3;
        }

        return 1;  // 表示需要运行检测流程
    }
#else
    // RTC模式：检查是否有RTC唤醒事件
    if (RTC_Wakeup_IsPending()) {
        if (g_low_power_manager.debug_enabled) {
            printf("LOW_POWER: Processing RTC wakeup event\r\n");
        }

        // 处理RTC唤醒
        int ret = LowPower_HandleWakeup();
        if (ret != 0) {
            printf("LOW_POWER: ERROR - Failed to handle wakeup: %d\r\n", ret);
            return -2;
        }

        // 启动检测流程
        ret = LowPower_StartDetectionProcess();
        if (ret != 0) {
            printf("LOW_POWER: ERROR - Failed to start detection: %d\r\n", ret);
            return -3;
        }

        return 1;  // 表示需要运行检测流程
    }
#endif

    return 0;  // 无事件，继续等待
}

/* ============================================================================
 * WOM管理函数实现
 * ============================================================================ */

/**
 * @brief 配置WOM阈值
 */
int LowPower_WOM_Configure(uint8_t threshold_x, uint8_t threshold_y, uint8_t threshold_z)
{
    int rc = 0;
    uint8_t readback[3] = {0};

    if (g_low_power_manager.debug_enabled) {
        printf("WOM: Configuring WOM thresholds...\r\n");
    }

    // 切换到Bank 4
    rc = inv_iim423xx_set_reg_bank(&icm_driver, 4);
    if (rc != 0) {
        printf("WOM: ERROR - Failed to switch to Bank 4: %d\r\n", rc);
        return -1;
    }

    // 写入WOM阈值（逐个寄存器写入）
    rc = inv_iim423xx_write_reg(&icm_driver, MPUREG_ACCEL_WOM_X_THR_B4, 1, &threshold_x);
    if (rc != 0) {
        printf("WOM: ERROR - Failed to write WOM X threshold: %d\r\n", rc);
        inv_iim423xx_set_reg_bank(&icm_driver, 0);
        return -2;
    }

    rc = inv_iim423xx_write_reg(&icm_driver, MPUREG_ACCEL_WOM_Y_THR_B4, 1, &threshold_y);
    if (rc != 0) {
        printf("WOM: ERROR - Failed to write WOM Y threshold: %d\r\n", rc);
        inv_iim423xx_set_reg_bank(&icm_driver, 0);
        return -2;
    }

    rc = inv_iim423xx_write_reg(&icm_driver, MPUREG_ACCEL_WOM_Z_THR_B4, 1, &threshold_z);
    if (rc != 0) {
        printf("WOM: ERROR - Failed to write WOM Z threshold: %d\r\n", rc);
        inv_iim423xx_set_reg_bank(&icm_driver, 0);
        return -2;
    }

    // 读回验证
    rc = inv_iim423xx_read_reg(&icm_driver, MPUREG_ACCEL_WOM_X_THR_B4, 1, &readback[0]);
    rc |= inv_iim423xx_read_reg(&icm_driver, MPUREG_ACCEL_WOM_Y_THR_B4, 1, &readback[1]);
    rc |= inv_iim423xx_read_reg(&icm_driver, MPUREG_ACCEL_WOM_Z_THR_B4, 1, &readback[2]);
    if (rc != 0) {
        printf("WOM: ERROR - Failed to read back WOM thresholds: %d\r\n", rc);
        inv_iim423xx_set_reg_bank(&icm_driver, 0);
        return -3;
    }

    // 验证
    if (readback[0] != threshold_x || readback[1] != threshold_y || readback[2] != threshold_z) {
        printf("WOM: ERROR - WOM threshold verification failed!\r\n");
        printf("WOM: Expected: X=%d Y=%d Z=%d\r\n", threshold_x, threshold_y, threshold_z);
        printf("WOM: Got:      X=%d Y=%d Z=%d\r\n", readback[0], readback[1], readback[2]);
        inv_iim423xx_set_reg_bank(&icm_driver, 0);
        return -4;
    }

    if (g_low_power_manager.debug_enabled) {
        printf("WOM: Thresholds verified: X=%d (%.1f mg), Y=%d (%.1f mg), Z=%d (%.1f mg)\r\n",
               threshold_x, threshold_x * 3.9f,
               threshold_y, threshold_y * 3.9f,
               threshold_z, threshold_z * 3.9f);
    }

    // 切换回Bank 0
    rc = inv_iim423xx_set_reg_bank(&icm_driver, 0);
    if (rc != 0) {
        printf("WOM: ERROR - Failed to switch to Bank 0: %d\r\n", rc);
        return -5;
    }

    // 更新配置
    g_low_power_manager.wom_threshold_x = threshold_x;
    g_low_power_manager.wom_threshold_y = threshold_y;
    g_low_power_manager.wom_threshold_z = threshold_z;

    printf("WOM: WOM thresholds configured successfully\r\n");
    return 0;
}

/**
 * @brief 使能WOM模式（配置传感器为LP+WOM）
 * 参考IIM-42352数据手册Section 8.7和参考工程guard_sensor.c
 */
int LowPower_WOM_Enable(void)
{
    int rc = 0;
    uint8_t reg_value = 0;
    uint8_t pwr_mgmt0_val = 0;
    uint8_t accel_config0_val = 0;
    uint8_t int_status = 0;
    uint8_t int_status2 = 0;
    uint8_t int_status3 = 0;

    printf("WOM: Enabling WOM mode (following datasheet section 8.7)...\r\n");

    // ===== 禁用NVIC中断防止配置过程中的误触发 =====
    printf("WOM: Disabling NVIC interrupt during configuration...\r\n");
    HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
    HAL_NVIC_ClearPendingIRQ(EXTI9_5_IRQn);

    // 确保在Bank 0
    rc = inv_iim423xx_set_reg_bank(&icm_driver, 0);
    if (rc != 0) {
        printf("WOM: ERROR - Failed to switch to Bank 0: %d\r\n", rc);
        return -1;
    }

    // ===== STEP 0: 配置加速度计为LP模式50Hz（WOM前提条件） =====
    printf("WOM: STEP 0: Configuring accelerometer to LP mode 50Hz...\r\n");

    // 先禁用加速度计
    printf("WOM:   Disabling accelerometer...\r\n");
    rc = inv_iim423xx_disable_accel(&icm_driver);
    if (rc != 0) {
        printf("WOM: ERROR - Failed to disable accel: %d\r\n", rc);
        return -1;
    }
    HAL_Delay(50);  // 等待传感器关闭

    // 使能LP模式（使用驱动函数）
    printf("WOM:   Enabling Low Power mode...\r\n");
    rc = inv_iim423xx_enable_accel_low_power_mode(&icm_driver);
    if (rc != 0) {
        printf("WOM: ERROR - Failed to enable LP mode: %d\r\n", rc);
        return -1;
    }

    // 设置ODR为50Hz
    rc = inv_iim423xx_read_reg(&icm_driver, MPUREG_ACCEL_CONFIG0, 1, &accel_config0_val);
    printf("WOM:   ACCEL_CONFIG0 current: 0x%02X\r\n", accel_config0_val);

    accel_config0_val &= ~0x0F;  // 清除ODR位
    accel_config0_val |= 0x09;   // 设置为50Hz (ODR=9)
    rc = inv_iim423xx_write_reg(&icm_driver, MPUREG_ACCEL_CONFIG0, 1, &accel_config0_val);
    if (rc != 0) {
        printf("WOM: ERROR - Failed to set ODR to 50Hz: %d\r\n", rc);
        return -1;
    }
    printf("WOM:   ACCEL_CONFIG0 new: 0x%02X (50Hz)\r\n", accel_config0_val);

    // 等待模式切换稳定
    HAL_Delay(50);

    // ===== STEP 1: 禁用所有INT1中断源（关键！避免误触发） =====
    printf("WOM: STEP 1: Disabling ALL INT1 interrupt sources...\r\n");

    // 禁用INT_SOURCE0中的所有中断
    uint8_t int_source0 = 0x00;  // 全部禁用
    rc = inv_iim423xx_write_reg(&icm_driver, MPUREG_INT_SOURCE0, 1, &int_source0);
    if (rc != 0) {
        printf("WOM: ERROR - Failed to write INT_SOURCE0: %d\r\n", rc);
        return -2;
    }
    printf("WOM:   INT_SOURCE0 = 0x00 (all disabled)\r\n");

    // 禁用INT_SOURCE1中的所有中断（稍后会重新使能WOM）
    reg_value = 0x00;
    rc = inv_iim423xx_write_reg(&icm_driver, MPUREG_INT_SOURCE1, 1, &reg_value);
    if (rc != 0) {
        printf("WOM: ERROR - Failed to write INT_SOURCE1: %d\r\n", rc);
        return -3;
    }
    printf("WOM:   INT_SOURCE1 = 0x00 (all disabled)\r\n");

    // 等待中断源禁用生效
    HAL_Delay(10);

    // 验证INT_SOURCE0确实被禁用
    rc = inv_iim423xx_read_reg(&icm_driver, MPUREG_INT_SOURCE0, 1, &int_source0);
    if (rc == 0) {
        printf("WOM:   Verified INT_SOURCE0 = 0x%02X\r\n", int_source0);
        if (int_source0 != 0x00) {
            printf("WOM: WARNING - INT_SOURCE0 not 0x00! Re-writing...\r\n");
            int_source0 = 0x00;
            inv_iim423xx_write_reg(&icm_driver, MPUREG_INT_SOURCE0, 1, &int_source0);
        }
    }

    // ===== STEP 2: 路由WOM中断到INT1 =====
    printf("WOM: STEP 2: Routing WOM interrupts to INT1...\r\n");
    rc = inv_iim423xx_read_reg(&icm_driver, MPUREG_INT_SOURCE1, 1, &reg_value);
    if (rc != 0) {
        printf("WOM: ERROR - Failed to read INT_SOURCE1: %d\r\n", rc);
        return -4;
    }
    printf("WOM:   INT_SOURCE1 current: 0x%02X\r\n", reg_value);

    // 使能X和Y轴WOM中断（Z轴受重力影响，禁用）
    reg_value |= BIT_INT_SOURCE1_WOM_X_INT1_EN;
    reg_value |= BIT_INT_SOURCE1_WOM_Y_INT1_EN;
    // reg_value |= BIT_INT_SOURCE1_WOM_Z_INT1_EN;  // Z轴禁用

    rc = inv_iim423xx_write_reg(&icm_driver, MPUREG_INT_SOURCE1, 1, &reg_value);
    if (rc != 0) {
        printf("WOM: ERROR - Failed to write INT_SOURCE1: %d\r\n", rc);
        return -5;
    }
    printf("WOM:   INT_SOURCE1 new: 0x%02X (WOM X/Y enabled)\r\n", reg_value);

    // ===== STEP 3: 等待50ms（数据手册要求） =====
    printf("WOM: STEP 3: Waiting 50ms as per datasheet...\r\n");
    HAL_Delay(50);

    // ===== STEP 4: 使能WOM模式 =====
    printf("WOM: STEP 4: Enabling WOM mode in SMD_CONFIG...\r\n");
    rc = inv_iim423xx_read_reg(&icm_driver, MPUREG_SMD_CONFIG, 1, &reg_value);
    if (rc != 0) {
        printf("WOM: ERROR - Failed to read SMD_CONFIG: %d\r\n", rc);
        return -6;
    }
    printf("WOM:   SMD_CONFIG current: 0x%02X\r\n", reg_value);

    // 配置WOM模式
    // 注意：头文件中的位定义是错误的！正确的位定义是：
    // Bit 4:3 = SMD_MODE (00=DISABLED, 01=WOM, 10=SMD_SHORT, 11=SMD_LONG)
    // Bit 2   = WOM_INT_MODE (0=OR, 1=AND)
    // Bit 1:0 = WOM_MODE (00=CMP_INIT, 01=CMP_PREV)

    // 清除所有WOM相关位
    reg_value &= 0xE0;  // 保留Bit 7:5，清除Bit 4:0

    // 设置SMD_MODE = 1 (WOM) at Bit 4:3
    reg_value |= (1 << 3);  // 0x08

    // 设置WOM_INT_MODE = 0 (OR) at Bit 2
    if (g_low_power_manager.wom_int_mode == 1) {
        reg_value |= (1 << 2);  // 0x04
    }

    // 设置WOM_MODE = 1 (CMP_PREV) at Bit 1:0
    if (g_low_power_manager.wom_mode == 1) {
        reg_value |= 0x01;
    }

    rc = inv_iim423xx_write_reg(&icm_driver, MPUREG_SMD_CONFIG, 1, &reg_value);
    if (rc != 0) {
        printf("WOM: ERROR - Failed to write SMD_CONFIG: %d\r\n", rc);
        return -7;
    }
    printf("WOM:   SMD_CONFIG new: 0x%02X (WOM enabled)\r\n", reg_value);

    // ===== STEP 5: 等待WOM稳定 =====
    printf("WOM: STEP 5: Waiting 500ms for WOM to stabilize...\r\n");
    HAL_Delay(500);

    // ===== STEP 6: 清除所有中断状态并使能NVIC中断 =====
    printf("WOM: STEP 6: Clearing all interrupt status and enabling NVIC...\r\n");

    // 清除所有中断状态寄存器（读取即清除）
    inv_iim423xx_read_reg(&icm_driver, MPUREG_INT_STATUS, 1, &int_status);
    if (int_status != 0) {
        printf("WOM:   Cleared INT_STATUS: 0x%02X\r\n", int_status);
    }

    inv_iim423xx_read_reg(&icm_driver, MPUREG_INT_STATUS2, 1, &int_status2);
    if (int_status2 != 0) {
        printf("WOM:   Cleared INT_STATUS2: 0x%02X\r\n", int_status2);
    }

    inv_iim423xx_read_reg(&icm_driver, MPUREG_INT_STATUS3, 1, &int_status3);
    if (int_status3 != 0) {
        printf("WOM:   Cleared INT_STATUS3: 0x%02X\r\n", int_status3);
    }

    // 清除GPIO pending中断
    HAL_NVIC_ClearPendingIRQ(EXTI9_5_IRQn);

    // 注意：不在这里使能NVIC，而是在进入STOP前才使能
    // 这样可以避免配置期间的误触发
    printf("WOM:   NVIC will be enabled before entering STOP mode\r\n");

    // ===== STEP 7: 验证最终配置 =====
    printf("WOM: STEP 7: Verifying final configuration...\r\n");

    // 读取PWR_MGMT_0验证加速度计模式（重用之前的变量）
    rc = inv_iim423xx_read_reg(&icm_driver, MPUREG_PWR_MGMT_0, 1, &pwr_mgmt0_val);
    printf("WOM:   PWR_MGMT_0 = 0x%02X (ACCEL_MODE = %d)\r\n", pwr_mgmt0_val, pwr_mgmt0_val & 0x03);

    // 读取ACCEL_CONFIG0验证ODR（重用之前的变量）
    rc = inv_iim423xx_read_reg(&icm_driver, MPUREG_ACCEL_CONFIG0, 1, &accel_config0_val);
    printf("WOM:   ACCEL_CONFIG0 = 0x%02X (ODR = %d)\r\n", accel_config0_val, accel_config0_val & 0x0F);

    // 读取INT_SOURCE1验证WOM中断路由
    rc = inv_iim423xx_read_reg(&icm_driver, MPUREG_INT_SOURCE1, 1, &reg_value);
    printf("WOM:   INT_SOURCE1 = 0x%02X (WOM routing)\r\n", reg_value);

    // 读取SMD_CONFIG验证WOM使能
    rc = inv_iim423xx_read_reg(&icm_driver, MPUREG_SMD_CONFIG, 1, &reg_value);
    printf("WOM:   SMD_CONFIG = 0x%02X (WOM mode = %d)\r\n", reg_value, (reg_value >> 3) & 0x03);

    // 读取INT_STATUS2检查当前中断状态（重用之前的变量）
    rc = inv_iim423xx_read_reg(&icm_driver, MPUREG_INT_STATUS2, 1, &int_status2);
    printf("WOM:   INT_STATUS2 = 0x%02X (should be 0x00 after clearing)\r\n", int_status2);

    g_low_power_manager.is_lp_mode = true;

    // 关键：清除可能在配置过程中产生的误触发标志
    // 确保系统能正常进入STOP模式
    g_wom_triggered = false;
    g_wom_isr_count = 0;

    printf("WOM: WOM mode enabled successfully\r\n");
    printf("WOM: System ready to detect motion. Shake the sensor to trigger WOM!\r\n");

    // 最终验证：读取几个加速度样本，确认传感器在LP模式下采样
    printf("\r\nWOM: Final verification - Reading accel samples in LP mode...\r\n");
    for (int i = 0; i < 5; i++) {
        HAL_Delay(100);  // 等待100ms (LP模式50Hz，应该有5个新样本)

        // 读取加速度数据
        uint8_t accel_data[6];
        rc = inv_iim423xx_read_reg(&icm_driver, MPUREG_ACCEL_DATA_X0_UI, 6, accel_data);
        if (rc == 0) {
            int16_t ax = (int16_t)((accel_data[0] << 8) | accel_data[1]);
            int16_t ay = (int16_t)((accel_data[2] << 8) | accel_data[3]);
            int16_t az = (int16_t)((accel_data[4] << 8) | accel_data[5]);

            // 转换为mg (±4g range, 16-bit: 1 LSB = 4000/32768 = 0.122 mg)
            float ax_mg = ax * 0.122f;
            float ay_mg = ay * 0.122f;
            float az_mg = az * 0.122f;

            printf("  Sample %d: X=%.0f Y=%.0f Z=%.0f mg\r\n", i+1, ax_mg, ay_mg, az_mg);
        }
    }
    printf("WOM: If samples are changing, sensor is sampling in LP mode.\r\n");
    printf("WOM: If samples are frozen, sensor might not be in LP mode!\r\n\r\n");

    return 0;
}

/**
 * @brief 禁用WOM模式
 */
int LowPower_WOM_Disable(void)
{
    int rc = 0;
    uint8_t reg_value = 0;

    printf("WOM: Disabling WOM mode...\r\n");

    // 禁用NVIC中断
    HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);

    // 切换到Bank 0
    rc = inv_iim423xx_set_reg_bank(&icm_driver, 0);
    if (rc != 0) {
        printf("WOM: ERROR - Failed to switch to Bank 0: %d\r\n", rc);
        return -1;
    }

    // 禁用WOM
    rc = inv_iim423xx_read_reg(&icm_driver, MPUREG_SMD_CONFIG, 1, &reg_value);
    reg_value &= ~BIT_SMD_CONFIG_SMD_MODE_MASK;  // 设置为disabled (0)
    rc |= inv_iim423xx_write_reg(&icm_driver, MPUREG_SMD_CONFIG, 1, &reg_value);
    if (rc != 0) {
        printf("WOM: ERROR - Failed to disable WOM: %d\r\n", rc);
        return -2;
    }

    // 清除中断状态
    uint8_t int_status;
    inv_iim423xx_read_reg(&icm_driver, MPUREG_INT_STATUS2, 1, &int_status);

    g_low_power_manager.is_lp_mode = false;
    printf("WOM: WOM mode disabled\r\n");

    return 0;
}

/**
 * @brief 切换到LN模式（1000Hz高性能模式）
 */
int LowPower_WOM_SwitchToLNMode(void)
{
    int rc = 0;

    if (g_low_power_manager.debug_enabled) {
        printf("WOM: Switching to LN mode (1000Hz)...\r\n");
    }

    // 禁用WOM
    rc = LowPower_WOM_Disable();
    if (rc != 0) {
        printf("WOM: ERROR - Failed to disable WOM: %d\r\n", rc);
        return -1;
    }

    // 关键：使能DATA_RDY中断，以便在LN模式下接收数据
    // 在WOM模式下，INT_SOURCE0被设置为0x00（禁用DATA_RDY）
    // 现在需要重新使能DATA_RDY中断
    uint8_t int_source0 = BIT_INT_SOURCE0_UI_DRDY_INT1_EN;
    rc = inv_iim423xx_write_reg(&icm_driver, MPUREG_INT_SOURCE0, 1, &int_source0);
    if (rc != 0) {
        printf("WOM: ERROR - Failed to enable DATA_RDY interrupt: %d\r\n", rc);
        return -2;
    }
    printf("WOM: Enabled DATA_RDY interrupt for LN mode\r\n");

    // 设置为LN模式，1000Hz
    rc = inv_iim423xx_set_accel_frequency(&icm_driver, IIM423XX_ACCEL_CONFIG0_ODR_1_KHZ);
    if (rc != 0) {
        printf("WOM: ERROR - Failed to set ODR to 1000Hz: %d\r\n", rc);
        return -3;
    }

    // 等待传感器启动（10ms）
    HAL_Delay(10);

    g_low_power_manager.is_lp_mode = false;

    if (g_low_power_manager.debug_enabled) {
        printf("WOM: Switched to LN mode successfully\r\n");
    }

    return 0;
}

/**
 * @brief 切换到LP+WOM模式（50Hz低功耗模式）
 */
int LowPower_WOM_SwitchToLPMode(void)
{
    if (g_low_power_manager.debug_enabled) {
        printf("WOM: Switching to LP+WOM mode (50Hz)...\r\n");
    }

    // 重新配置WOM
    int rc = LowPower_WOM_Configure(g_low_power_manager.wom_threshold_x,
                                    g_low_power_manager.wom_threshold_y,
                                    g_low_power_manager.wom_threshold_z);
    if (rc != 0) {
        printf("WOM: ERROR - Failed to configure WOM: %d\r\n", rc);
        return -1;
    }

    // 使能WOM
    rc = LowPower_WOM_Enable();
    if (rc != 0) {
        printf("WOM: ERROR - Failed to enable WOM: %d\r\n", rc);
        return -2;
    }

    if (g_low_power_manager.debug_enabled) {
        printf("WOM: Switched to LP+WOM mode successfully\r\n");
    }

    return 0;
}

/**
 * @brief 检查WOM是否触发
 */
bool LowPower_WOM_IsTriggered(void)
{
    return g_wom_triggered;
}

/**
 * @brief 清除WOM触发标志
 */
void LowPower_WOM_ClearTrigger(void)
{
    g_wom_triggered = false;
}

/**
 * @brief WOM中断处理函数（在EXTI中断中调用）
 * 参考参考工程guard_sensor.c的实现，添加INT_STATUS2验证
 */
void LowPower_WOM_IRQHandler(void)
{
    // 防止重复处理：如果已经触发过且未清除，直接返回
    if (g_wom_triggered) {
        g_wom_isr_count++;
        return;  // 静默返回
    }

    // 关键：STOP模式下不能进行任何外设通信！
    // - 不能使用printf（UART时钟停止）
    // - 不能使用SPI通信（SPI时钟停止）
    // - 只能设置标志，唤醒后再处理

    // 设置WOM触发标志
    g_wom_triggered = true;
    g_low_power_manager.wom_trigger_count++;

    // 注意：HAL_GetTick()可能不准确，因为SysTick已禁用
    // 但是可以用来记录相对时间
    g_low_power_manager.last_wom_trigger_time = HAL_GetTick();

    // ISR返回后，CPU会从HAL_PWR_EnterSTOPMode()返回
    // 然后执行SystemClock_Config()恢复时钟
    // 最后在LowPower_ExitSleep()中打印调试信息
}

/**
 * @brief 获取WOM统计信息
 */
void LowPower_WOM_GetStatistics(uint32_t* trigger_count, uint32_t* false_alarm_count)
{
    if (trigger_count != NULL) {
        *trigger_count = g_low_power_manager.wom_trigger_count;
    }
    if (false_alarm_count != NULL) {
        *false_alarm_count = g_low_power_manager.wom_false_alarm_count;
    }
}

/**
 * @brief 读取并打印所有WOM相关寄存器（用于调试）
 */
void LowPower_WOM_DumpRegisters(void)
{
    uint8_t reg_value = 0;

    printf("\r\n========================================\r\n");
    printf("  WOM REGISTER DUMP\r\n");
    printf("========================================\r\n");

    // Bank 0寄存器
    inv_iim423xx_set_reg_bank(&icm_driver, 0);

    // PWR_MGMT_0 (0x4E)
    inv_iim423xx_read_reg(&icm_driver, MPUREG_PWR_MGMT_0, 1, &reg_value);
    printf("PWR_MGMT_0 (0x4E):     0x%02X\r\n", reg_value);
    printf("  ACCEL_MODE:          %d ", reg_value & 0x03);
    switch(reg_value & 0x03) {
        case 0: printf("(OFF)\r\n"); break;
        case 2: printf("(LP)\r\n"); break;
        case 3: printf("(LN)\r\n"); break;
        default: printf("(UNKNOWN)\r\n"); break;
    }

    // ACCEL_CONFIG0 (0x50)
    inv_iim423xx_read_reg(&icm_driver, MPUREG_ACCEL_CONFIG0, 1, &reg_value);
    printf("ACCEL_CONFIG0 (0x50):  0x%02X\r\n", reg_value);
    printf("  ODR:                 %d ", reg_value & 0x0F);
    switch(reg_value & 0x0F) {
        case 6: printf("(1.5625Hz)\r\n"); break;
        case 7: printf("(3.125Hz)\r\n"); break;
        case 8: printf("(6.25Hz)\r\n"); break;
        case 9: printf("(12.5Hz/50Hz)\r\n"); break;
        case 10: printf("(25Hz/100Hz)\r\n"); break;
        case 11: printf("(50Hz/200Hz)\r\n"); break;
        case 12: printf("(100Hz/500Hz)\r\n"); break;
        case 13: printf("(200Hz/1kHz)\r\n"); break;
        case 14: printf("(500Hz/2kHz)\r\n"); break;
        default: printf("(UNKNOWN)\r\n"); break;
    }
    printf("  FSR:                 %d ", (reg_value >> 5) & 0x03);
    switch((reg_value >> 5) & 0x03) {
        case 0: printf("(±16g)\r\n"); break;
        case 1: printf("(±8g)\r\n"); break;
        case 2: printf("(±4g)\r\n"); break;
        case 3: printf("(±2g)\r\n"); break;
    }

    // INT_CONFIG (0x14)
    inv_iim423xx_read_reg(&icm_driver, MPUREG_INT_CONFIG, 1, &reg_value);
    printf("INT_CONFIG (0x14):     0x%02X\r\n", reg_value);
    printf("  INT1_POLARITY:       %d (%s)\r\n", reg_value & 0x01, (reg_value & 0x01) ? "HIGH" : "LOW");
    printf("  INT1_DRIVE:          %d (%s)\r\n", (reg_value >> 1) & 0x01, ((reg_value >> 1) & 0x01) ? "PP" : "OD");

    // INT_CONFIG0 (0x63)
    inv_iim423xx_read_reg(&icm_driver, MPUREG_INT_CONFIG0, 1, &reg_value);
    printf("INT_CONFIG0 (0x63):    0x%02X\r\n", reg_value);

    // INT_CONFIG1 (0x64)
    inv_iim423xx_read_reg(&icm_driver, MPUREG_INT_CONFIG1, 1, &reg_value);
    printf("INT_CONFIG1 (0x64):    0x%02X\r\n", reg_value);

    // INT_SOURCE0 (0x65)
    inv_iim423xx_read_reg(&icm_driver, MPUREG_INT_SOURCE0, 1, &reg_value);
    printf("INT_SOURCE0 (0x65):    0x%02X\r\n", reg_value);
    printf("  UI_DRDY_INT1_EN:     %d\r\n", (reg_value >> 3) & 0x01);

    // INT_SOURCE1 (0x66)
    inv_iim423xx_read_reg(&icm_driver, MPUREG_INT_SOURCE1, 1, &reg_value);
    printf("INT_SOURCE1 (0x66):    0x%02X\r\n", reg_value);
    printf("  WOM_X_INT1_EN:       %d\r\n", reg_value & 0x01);
    printf("  WOM_Y_INT1_EN:       %d\r\n", (reg_value >> 1) & 0x01);
    printf("  WOM_Z_INT1_EN:       %d\r\n", (reg_value >> 2) & 0x01);

    // SMD_CONFIG (0x6B)
    inv_iim423xx_read_reg(&icm_driver, MPUREG_SMD_CONFIG, 1, &reg_value);
    printf("SMD_CONFIG (0x6B):     0x%02X\r\n", reg_value);
    printf("  SMD_MODE:            %d ", (reg_value >> 3) & 0x03);
    switch((reg_value >> 3) & 0x03) {
        case 0: printf("(DISABLED)\r\n"); break;
        case 1: printf("(WOM)\r\n"); break;
        case 2: printf("(SMD_SHORT)\r\n"); break;
        case 3: printf("(SMD_LONG)\r\n"); break;
    }
    printf("  WOM_INT_MODE:        %d (%s)\r\n", (reg_value >> 2) & 0x01, ((reg_value >> 2) & 0x01) ? "AND" : "OR");
    printf("  WOM_MODE:            %d (%s)\r\n", reg_value & 0x03, (reg_value & 0x03) ? "CMP_PREV" : "CMP_INIT");

    // INT_STATUS (0x2D)
    inv_iim423xx_read_reg(&icm_driver, MPUREG_INT_STATUS, 1, &reg_value);
    printf("INT_STATUS (0x2D):     0x%02X\r\n", reg_value);

    // INT_STATUS2 (0x37)
    inv_iim423xx_read_reg(&icm_driver, MPUREG_INT_STATUS2, 1, &reg_value);
    printf("INT_STATUS2 (0x37):    0x%02X\r\n", reg_value);
    printf("  WOM_X:               %d\r\n", reg_value & 0x01);
    printf("  WOM_Y:               %d\r\n", (reg_value >> 1) & 0x01);
    printf("  WOM_Z:               %d\r\n", (reg_value >> 2) & 0x01);

    // Bank 4寄存器 - WOM阈值
    inv_iim423xx_set_reg_bank(&icm_driver, 4);

    uint8_t wom_x, wom_y, wom_z;
    inv_iim423xx_read_reg(&icm_driver, MPUREG_ACCEL_WOM_X_THR_B4, 1, &wom_x);
    inv_iim423xx_read_reg(&icm_driver, MPUREG_ACCEL_WOM_Y_THR_B4, 1, &wom_y);
    inv_iim423xx_read_reg(&icm_driver, MPUREG_ACCEL_WOM_Z_THR_B4, 1, &wom_z);

    printf("\r\n--- Bank 4 Registers ---\r\n");
    printf("WOM_X_THR (0x4A):      %d (%.1f mg)\r\n", wom_x, wom_x * 3.9f);
    printf("WOM_Y_THR (0x4B):      %d (%.1f mg)\r\n", wom_y, wom_y * 3.9f);
    printf("WOM_Z_THR (0x4C):      %d (%.1f mg)\r\n", wom_z, wom_z * 3.9f);

    // 切换回Bank 0
    inv_iim423xx_set_reg_bank(&icm_driver, 0);

    printf("========================================\r\n\r\n");
}
