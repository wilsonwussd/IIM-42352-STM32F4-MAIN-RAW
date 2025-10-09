/**
 * @file clock_config_84mhz.h
 * @brief STM32F4 84MHz时钟配置说明和验证
 * @author Augment Agent
 * @date 2025-09-22
 */

#ifndef CLOCK_CONFIG_84MHZ_H
#define CLOCK_CONFIG_84MHZ_H

#ifdef __cplusplus
extern "C" {
#endif

/* 包含头文件 */
#include "stm32f4xx_hal.h"

/* 时钟配置参数定义 */
#define TARGET_SYSCLK_FREQ      84000000U   /* 目标系统时钟频率 84MHz */
#define HSE_FREQ                25000000U   /* 外部晶振频率 25MHz */
#define PLL_M_VALUE             25          /* PLL M分频系数 */
#define PLL_N_VALUE             336         /* PLL N倍频系数 */
#define PLL_P_VALUE             4           /* PLL P分频系数 (DIV4) */
#define PLL_Q_VALUE             7           /* PLL Q分频系数 (保持48MHz USB时钟) */

/* 计算验证 */
#define PLL_VCO_FREQ            ((HSE_FREQ / PLL_M_VALUE) * PLL_N_VALUE)  /* 336MHz */
#define CALCULATED_SYSCLK       (PLL_VCO_FREQ / PLL_P_VALUE)              /* 84MHz */
#define USB_CLK_FREQ            (PLL_VCO_FREQ / PLL_Q_VALUE)               /* 48MHz */

/* 总线时钟频率 */
#define AHB_CLK_FREQ            84000000U   /* AHB时钟 = SYSCLK */
#define APB1_CLK_FREQ           42000000U   /* APB1时钟 = AHB/2 */
#define APB2_CLK_FREQ           84000000U   /* APB2时钟 = AHB/1 */

/* Flash等待周期 */
#define FLASH_LATENCY_84MHZ     FLASH_LATENCY_2  /* 84MHz需要2个等待周期 */

/* 功耗对比 */
#define POWER_REDUCTION_PERCENT 50          /* 相对168MHz的功耗降低百分比 */

/* 性能参数 */
#define FFT_TIME_84MHZ_US       36          /* 512点FFT预估时间(微秒) */
#define FFT_TIME_168MHZ_US      18          /* 168MHz时的FFT时间(微秒) */

/* 时钟配置验证宏 */
#define VERIFY_SYSCLK_CONFIG() \
    do { \
        if (CALCULATED_SYSCLK != TARGET_SYSCLK_FREQ) { \
            Error_Handler(); \
        } \
    } while(0)

#define VERIFY_USB_CLK_CONFIG() \
    do { \
        if (USB_CLK_FREQ < 47000000U || USB_CLK_FREQ > 49000000U) { \
            Error_Handler(); \
        } \
    } while(0)

/* 函数声明 */
void Clock_Config_Verify(void);
void Clock_Performance_Test(void);
uint32_t Get_Current_SYSCLK_Freq(void);
void Print_Clock_Info(void);
void Full_84MHz_Test(void);

/* 时钟配置结构体 */
typedef struct {
    uint32_t SYSCLK_Freq;
    uint32_t HCLK_Freq;
    uint32_t PCLK1_Freq;
    uint32_t PCLK2_Freq;
    uint32_t Flash_Latency;
    float Power_Reduction;
} Clock_Config_Info_t;

/* 性能测试结果结构体 */
typedef struct {
    uint32_t FFT_Time_us;
    uint32_t UART_Test_Time_us;
    uint32_t SPI_Test_Time_us;
    uint8_t Performance_OK;
} Performance_Test_Result_t;

#ifdef __cplusplus
}
#endif

#endif /* CLOCK_CONFIG_84MHZ_H */

/**
 * @brief 84MHz时钟配置说明
 * 
 * 配置详情:
 * - HSE: 25MHz (外部晶振)
 * - PLL配置: (25MHz / 25) * 336 / 4 = 84MHz
 * - HCLK: 84MHz (AHB总线)
 * - PCLK1: 42MHz (APB1总线, ≤45MHz)
 * - PCLK2: 84MHz (APB2总线, ≤90MHz)
 * - Flash等待周期: 2 (适用于84MHz)
 * - USB时钟: 48MHz (336MHz / 7)
 * 
 * 功耗优势:
 * - 相对168MHz降低约50%功耗
 * - 仍保持充足的性能裕量
 * - 外设时钟在规范范围内
 * 
 * 性能影响:
 * - FFT计算时间从18μs增加到36μs
 * - 对于512ms执行一次的FFT，影响微乎其微
 * - UART、SPI等外设性能不受影响
 * 
 * 验证方法:
 * 1. 编译烧录新配置
 * 2. 调用Clock_Config_Verify()验证时钟
 * 3. 调用Clock_Performance_Test()测试性能
 * 4. 长时间运行验证稳定性
 */
