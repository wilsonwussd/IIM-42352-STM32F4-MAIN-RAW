/*
 * ________________________________________________________________________________________________________
   Copyright (C) [2022] by InvenSense, Inc.
   Permission to use, copy, modify, and/or distribute this software for any purpose
   with or without fee is hereby granted.
    
   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
   REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
   FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
   INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
   OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
   TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
   THIS SOFTWARE.
 * ________________________________________________________________________________________________________
 */
#ifndef _EXAMPLE_RAW_AG_H_
#define _EXAMPLE_RAW_AG_H_

#include <stdint.h>
#include <stdbool.h>  // For bool type
#include "Iim423xxTransport.h"
#include "Iim423xxDefs.h"
#include "Iim423xxDriver_HL.h"
#include "arm_math.h"  // For float32_t

/* 智能检测功能控制开关 */
#define ENABLE_INTELLIGENT_DETECTION  1
#define ENABLE_DATA_PREPROCESSING     1
#define ENABLE_COARSE_DETECTION       1
#define ENABLE_FINE_DETECTION         1
#define ENABLE_SYSTEM_STATE_MACHINE   1

#if ENABLE_DATA_PREPROCESSING
/* 高通滤波器配置 */
#define HIGHPASS_FILTER_ORDER    4      // 4阶Butterworth滤波器
#define HIGHPASS_CUTOFF_FREQ     5.0f   // 5Hz截止频率
#define SAMPLING_FREQ            1000.0f // 1000Hz采样频率

/* IIR滤波器状态结构 */
typedef struct {
    arm_biquad_casd_df1_inst_f32 filter_instance;
    float32_t filter_state[2 * (HIGHPASS_FILTER_ORDER/2)]; // 每个biquad段2个状态变量
    float32_t filter_coeffs[5 * (HIGHPASS_FILTER_ORDER/2)]; // 每个biquad段5个系数
    bool is_initialized;
} highpass_filter_t;
#endif

#if ENABLE_COARSE_DETECTION
/* 粗检测算法配置 */
#define RMS_WINDOW_SIZE          2000    // RMS滑动窗口大小 (2000ms @ 1000Hz) - 增加到2秒窗口
#define BASELINE_RMS_THRESHOLD   0.001f  // 基线RMS阈值 (降低到1mg用于调试)
#define TRIGGER_MULTIPLIER       1.5f    // 触发倍数 (降低到1.5x用于调试)
#define TRIGGER_DURATION_MS      2000    // 触发持续时间 (2000ms)
#define COOLDOWN_TIME_MS         10000   // 冷却时间 (10000ms)

/* 粗检测状态枚举 */
typedef enum {
    COARSE_STATE_IDLE = 0,      // 空闲状态
    COARSE_STATE_TRIGGERED,     // 触发状态
    COARSE_STATE_COOLDOWN       // 冷却状态
} coarse_detection_state_t;

/* 粗检测算法状态结构 */
typedef struct {
    float32_t rms_window[RMS_WINDOW_SIZE];  // RMS滑动窗口缓冲区
    uint32_t window_index;                  // 窗口索引
    bool window_full;                       // 窗口是否已满

    float32_t current_rms;                  // 当前RMS值
    float32_t baseline_rms;                 // 基线RMS值
    float32_t peak_factor;                  // 峰值因子

    coarse_detection_state_t state;         // 检测状态
    uint32_t trigger_start_time;            // 触发开始时间
    uint32_t cooldown_start_time;           // 冷却开始时间

    uint32_t trigger_count;                 // 触发计数
    bool is_initialized;                    // 初始化标志
} coarse_detector_t;
#endif


/**
 * \brief This function is in charge of reseting and initializing Iim423xx device. It should
 * be succesfully executed before any access to Iim423xx device.
 *
 * \return 0 on success, negative value on error.
 */
int SetupInvDevice(struct inv_iim423xx_serif * icm_serif);

/**
 * \brief This function configures the device in order to output accelerometer.
 *
 * It initialyses clock calibration module (this will allow to extend the 16 bits 
 * timestamp produced by Iim423xx to a 64 bits timestamp).
 * Then function sets full scale range and frequency for both accel and it 
 * starts them in the requested power mode. 
 *
 * \param[in] is_low_noise_mode : if true sensors are started in low-noise mode else in 
 *                                low-power mode.
 * \param[in] is_high_res_mode : if true raw accelerometers  are used in 20 bits format.
 *                                The fifo will be in 20 byte mode.
 * \param[in] acc_fsr_g :   full scale range for accelerometer. See IIM423XX_ACCEL_CONFIG0_FS_SEL_t in Iim423xxDefs.h
 *                         for possible values.
 * \param[in] acc_freq :    accelerometer frequency. See IIM423XX_ACCEL_CONFIG0_ODR_t in Iim423xxDefs.h
 *                         for possible values.
 * \param[in] is_rtc_mode :    requested status for RTC/CLKIN feature
 * \return 0 on success, negative value on error.
 */
int ConfigureInvDevice(uint8_t is_low_noise_mode,
                       uint8_t is_high_res_mode,
                       IIM423XX_ACCEL_CONFIG0_FS_SEL_t acc_fsr_g,
					   IIM423XX_ACCEL_CONFIG0_ODR_t acc_freq,
					   uint8_t is_rtc_mode);

/**
 * \brief This function extracts data from the Iim423xx FIFO.
 *
 * The function just calls Iim423xx driver function inv_iim423xx_get_data_from_fifo.
 * But note that for each packet extracted from FIFO, a user defined function is called to 
 * allow custom handling of each packet. In this example custom packet handling function
 * is HandleInvDeviceFifoPacket.
 *
 * \return 0 on success, negative value on error.
 */
int GetDataFromInvDevice(void);

/**
 * \brief This function is the custom handling packet function.
 *
 * It is passed in parameter at driver init time and it is called by
 * inv_iim423xx_get_data_from_fifo function each time a new valid packet is extracted
 * from FIFO.
 * In this implementation, function extends packet timestamp from 16 to 64 bits and then
 * process data from packet and print them on UART.
 *
 * \param[in] event structure containing sensor data from one packet
 */
void HandleInvDeviceFifoPacket(inv_iim423xx_sensor_event_t * event);

/* 原始加速度数据发送函数已删除 - 调试串口现在专用于调试信息输出 */

#if ENABLE_DATA_PREPROCESSING
/**
 * \brief Initialize high-pass filter for vibration data preprocessing
 *
 * This function initializes a 4th-order Butterworth high-pass filter with 5Hz cutoff frequency
 * to remove low-frequency noise and DC components from accelerometer data.
 *
 * \return 0 on success, negative value on error
 */
int Highpass_Filter_Init(void);

/**
 * \brief Apply high-pass filter to accelerometer data
 *
 * This function applies the high-pass filter to remove low-frequency components
 * from the accelerometer data, preparing it for vibration analysis.
 *
 * \param[in] input Raw accelerometer data in g units
 * \return Filtered accelerometer data in g units
 */
float32_t Highpass_Filter_Process(float32_t input);

/**
 * \brief Reset high-pass filter state
 *
 * This function resets the internal state of the high-pass filter,
 * useful for restarting the filtering process.
 */
void Highpass_Filter_Reset(void);
#endif

#if ENABLE_COARSE_DETECTION
/**
 * \brief Initialize coarse detection algorithm
 *
 * This function initializes the coarse detection algorithm including RMS sliding window,
 * baseline tracking, and trigger detection logic.
 *
 * \return 0 on success, negative value on error
 */
int Coarse_Detector_Init(void);

/**
 * \brief Process one sample through coarse detection algorithm
 *
 * This function processes one filtered accelerometer sample through the coarse detection
 * algorithm, updating RMS window, calculating peak factor, and checking trigger conditions.
 *
 * \param filtered_sample Filtered accelerometer sample (in g units)
 * \return 1 if trigger detected, 0 if no trigger
 */
int Coarse_Detector_Process(float32_t filtered_sample);

/**
 * \brief Get current coarse detection state
 *
 * \return Current detection state (IDLE/TRIGGERED/COOLDOWN)
 */
coarse_detection_state_t Coarse_Detector_GetState(void);

/**
 * \brief Reset coarse detection algorithm
 *
 * This function resets the coarse detection algorithm to initial state.
 */
void Coarse_Detector_Reset(void);

/**
 * \brief Check if coarse detection window is full
 *
 * This function checks if the RMS sliding window has collected enough samples
 * to start detection. Useful for low-power mode to ensure sufficient data collection.
 *
 * \return true if window is full (2000 samples collected), false otherwise
 */
bool Coarse_Detector_IsWindowFull(void);
#endif

#if ENABLE_FINE_DETECTION
/* 细检测算法配置 */
#define FINE_DETECTION_LOW_FREQ_MIN    5.0f     // 低频段下限 (Hz)
#define FINE_DETECTION_LOW_FREQ_MAX    15.0f    // 低频段上限 (Hz)
#define FINE_DETECTION_MID_FREQ_MIN    15.0f    // 中频段下限 (Hz)
#define FINE_DETECTION_MID_FREQ_MAX    30.0f    // 中频段上限 (Hz)
#define FINE_DETECTION_HIGH_FREQ_MIN   30.0f    // 高频段下限 (Hz)
#define FINE_DETECTION_HIGH_FREQ_MAX   100.0f   // 高频段上限 (Hz)

/* 分类阈值配置 */
#define FINE_DETECTION_LOW_FREQ_THRESHOLD     0.4f    // 低频能量阈值
#define FINE_DETECTION_MID_FREQ_THRESHOLD     0.2f    // 中频能量阈值
#define FINE_DETECTION_DOMINANT_FREQ_MAX      50.0f   // 主频上限 (Hz)
#define FINE_DETECTION_CENTROID_MAX           80.0f   // 频谱重心上限 (Hz)
#define FINE_DETECTION_CONFIDENCE_THRESHOLD   0.7f    // 置信度阈值

/* 分类结果定义 */
typedef enum {
    FINE_DETECTION_NORMAL = 0,    // 正常/环境干扰
    FINE_DETECTION_MINING = 1     // 挖掘震动
} fine_detection_result_t;

/* 细检测特征结构 */
typedef struct {
    // 5维频域特征
    float32_t low_freq_energy;       // 低频能量占比 (5-15Hz)
    float32_t mid_freq_energy;       // 中频能量占比 (15-30Hz)
    float32_t high_freq_energy;      // 高频能量占比 (30-100Hz)
    float32_t dominant_frequency;    // 主频 (Hz)
    float32_t spectral_centroid;     // 频谱重心 (Hz)

    // 分类结果
    fine_detection_result_t classification; // 分类结果
    float32_t confidence_score;      // 置信度分数 (0-1)

    // 性能统计
    uint32_t analysis_timestamp;     // 分析时间戳
    uint32_t computation_time_us;    // 计算时间 (微秒)

    // 状态标志
    bool is_valid;                   // 结果有效性
} fine_detection_features_t;

/**
 * \brief Initialize fine detection algorithm
 *
 * \return 0 on success, negative value on error
 */
int Fine_Detector_Init(void);

/**
 * \brief Process FFT spectrum data for fine detection
 *
 * \param magnitude_spectrum: FFT magnitude spectrum array
 * \param spectrum_length: Length of spectrum array (should be 257)
 * \param dominant_freq: Dominant frequency from FFT analysis
 * \param features: Output structure for extracted features and classification
 * \return 0 on success, negative value on error
 */
int Fine_Detector_Process(const float32_t* magnitude_spectrum,
                         uint32_t spectrum_length,
                         float32_t dominant_freq,
                         fine_detection_features_t* features);

/**
 * \brief Print fine detection results
 *
 * \param features: Fine detection features and results
 */
void Fine_Detector_PrintResults(const fine_detection_features_t* features);
#endif

#if ENABLE_SYSTEM_STATE_MACHINE
/* 阶段5：系统状态机定义 */

/* 系统状态枚举 */
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

    // 低功耗模式新增状态
    STATE_LOW_POWER_SLEEP_PREPARE,   // 准备进入低功耗Sleep
    STATE_LOW_POWER_SLEEP_MODE,      // 低功耗Sleep模式
    STATE_LOW_POWER_WAKEUP,          // 低功耗唤醒
    STATE_LOW_POWER_DETECTION_ACTIVE,// 低功耗检测活跃状态

    STATE_COUNT                      // 状态总数
} system_state_t;

/* 状态机结构体 */
typedef struct {
    system_state_t current_state;    // 当前状态
    system_state_t previous_state;   // 前一状态
    uint32_t state_enter_time;       // 状态进入时间
    uint32_t state_duration;         // 状态持续时间

    // 状态转换条件
    uint8_t coarse_trigger_flag;     // 粗检测触发标志
    uint8_t fine_analysis_result;    // 细检测结果 (0=无效, 1=正常, 2=挖掘)
    uint8_t alarm_send_status;       // 报警发送状态
    uint8_t error_code;              // 错误代码

    // 状态统计
    uint32_t state_count[STATE_COUNT];  // 各状态计数
    uint32_t transition_count;       // 状态转换计数

    // 检测统计
    uint32_t total_detections;       // 总检测次数
    uint32_t mining_detections;      // 挖掘检测次数
    uint32_t false_alarms;           // 误报次数
} system_state_machine_t;

/* 状态机配置参数 */
#define STATE_MONITORING_TIMEOUT_MS     30000   // 监测状态超时 (30秒)
#define STATE_FINE_ANALYSIS_TIMEOUT_MS  5000    // 细检测分析超时 (5秒)
#define STATE_ALARM_SENDING_TIMEOUT_MS  10000   // 报警发送超时 (10秒)
#define STATE_ERROR_RECOVERY_DELAY_MS   1000    // 错误恢复延迟 (1秒)

/* 函数声明 */
int System_State_Machine_Init(void);
void System_State_Machine_Process(void);
void System_State_Machine_SetCoarseTrigger(uint8_t triggered);
void System_State_Machine_SetFineResult(uint8_t result);
void System_State_Machine_SetAlarmStatus(uint8_t status);
void System_State_Machine_SetError(uint8_t error_code);
system_state_t System_State_Machine_GetCurrentState(void);
void System_State_Machine_PrintStatus(void);

#endif

#endif /* !_EXAMPLE_RAW_AG_H_ */
