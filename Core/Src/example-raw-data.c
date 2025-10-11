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

/* Standard libraries first */
#include <string.h>  // For memset, memcpy
#include <stdio.h>   // For putchar

/* HAL and main includes */
#include "main.h"    // For HAL_GetTick

/* Project specific includes */
#include "example-raw-data.h"

/* Clock calibration module */
#include "helperClockCalib.h"
#include "Message.h"
// #include "Iim423xx/RingBuffer.h"

/* FFT Processing */
#include "fft_processor.h"


/* --------------------------------------------------------------------------------------
 *  Static and extern variables
 * -------------------------------------------------------------------------------------- */

/* Just a handy variable to handle the iim423xx object */
/* Changed from static to allow access from low_power_manager.c for WOM configuration */
struct inv_iim423xx icm_driver;

/* structure allowing to handle clock calibration */
static clk_calib_t clk_calib;

/* Buffer to keep track of the timestamp when iim423xx data ready interrupt fires. */
// extern  RINGBUFFER(timestamp_buffer, 64, uint64_t);

#if ENABLE_DATA_PREPROCESSING
/* 高通滤波器实例 */
static highpass_filter_t z_axis_filter;
#endif

#if ENABLE_COARSE_DETECTION
/* 粗检测算法实例 */
static coarse_detector_t coarse_detector;
#endif

#if ENABLE_FINE_DETECTION
/* 细检测算法实例 */
static bool fine_detector_initialized = false;
#endif

#if ENABLE_SYSTEM_STATE_MACHINE
/* 阶段5：系统状态机全局变量 */
static system_state_machine_t g_state_machine;
static uint8_t state_machine_initialized = 0;

/* 状态名称字符串 (用于调试输出) */
static const char* state_names[STATE_COUNT] = {
    "SYSTEM_INIT",
    "IDLE_SLEEP",
    "MONITORING",
    "COARSE_TRIGGERED",
    "FINE_ANALYSIS",
    "MINING_DETECTED",
    "ALARM_SENDING",
    "ALARM_COMPLETE",
    "ERROR_HANDLING",
    "SYSTEM_RESET"
};

#endif

#if ENABLE_DATA_PREPROCESSING
/* 4阶Butterworth高通滤波器系数 (5Hz截止频率, 1000Hz采样频率) */
/* 使用scipy.signal.butter计算，转换为CMSIS DSP格式 */
static const float32_t highpass_coeffs[10] = {
    /* 第一个biquad段 (b0, b1, b2, a1, a2) */
    0.9597822f, -1.9195645f, 0.9597822f, 1.9426382f, -0.9435973f,
    /* 第二个biquad段 (b0, b1, b2, a1, a2) */
    1.0000000f, -2.0000000f, 1.0000000f, 1.9752696f, -0.9762448f
};
#endif

/*
 * ICM mounting matrix
 * Coefficients are coded as Q30 integer
 */
#if defined(ICM_FAMILY_CPLUS)
//static int32_t icm_mounting_matrix[9] = {  0,        -(1<<30),      0,
                                          //(1<<30),     0,           0,
                                           //0,          0,          (1<<30) };
static int32_t icm_mounting_matrix[9] = {  (1<<30),      0,      0,
										   0,       -(1<<30),           0,
										   0,          0,          (1<<30) };
#else
static int32_t icm_mounting_matrix[9] = { (1<<30),     0,           0,
                                           0,         (1<<30),      0,
                                           0,          0,          (1<<30) };
#endif

/* --------------------------------------------------------------------------------------
 *  static function declaration
 * -------------------------------------------------------------------------------------- */
static void apply_mounting_matrix(const int32_t matrix[9], int32_t raw[3]);

/* --------------------------------------------------------------------------------------
 *  Functions definition
 * -------------------------------------------------------------------------------------- */

int SetupInvDevice(struct inv_iim423xx_serif * icm_serif)
{
	int rc = 0;
	uint8_t who_am_i;
	
	/* Initialize device */
	INV_MSG(INV_MSG_LEVEL_INFO, "Initialize Iim423xx");
	
	rc = inv_iim423xx_init(&icm_driver, icm_serif, HandleInvDeviceFifoPacket);
	if(rc != INV_ERROR_SUCCESS) {
		INV_MSG(INV_MSG_LEVEL_ERROR, "!!! ERROR : failed to initialize Iim423xx.");
		return rc;
	}	
	
	/* Check WHOAMI */
	INV_MSG(INV_MSG_LEVEL_INFO, "Check Iim423xx whoami value");
	
	rc = inv_iim423xx_get_who_am_i(&icm_driver, &who_am_i);
	if(rc != INV_ERROR_SUCCESS) {
		INV_MSG(INV_MSG_LEVEL_ERROR, "!!! ERROR : failed to read Iim423xx whoami value.");
		return rc;
	}
	
	if(who_am_i != ICM_WHOAMI) {
		INV_MSG(INV_MSG_LEVEL_ERROR, "!!! ERROR :  bad WHOAMI value. Got 0x%02x (expected: 0x%02x)", who_am_i, ICM_WHOAMI);
		return INV_ERROR;
	}

#if ENABLE_DATA_PREPROCESSING
	/* 初始化高通滤波器 */
	printf("=== HIGHPASS FILTER INITIALIZATION ===\r\n");
	rc = Highpass_Filter_Init();
	if(rc != 0) {
		INV_MSG(INV_MSG_LEVEL_ERROR, "!!! ERROR : failed to initialize highpass filter.");
		printf("CRITICAL: Highpass filter initialization FAILED!\r\n");
		return rc;
	}
	INV_MSG(INV_MSG_LEVEL_INFO, "Highpass filter initialized successfully");

	// 验证滤波器工作 - 测试几个已知输入
	printf("=== FILTER VERIFICATION TEST ===\r\n");
	float32_t test_inputs[] = {0.0f, 1.0f, -1.0f, 0.5f, -0.5f};
	for(int i = 0; i < 5; i++) {
		float32_t test_output = Highpass_Filter_Process(test_inputs[i]);
		printf("TEST: Input=%.3f -> Output=%.6f\r\n", test_inputs[i], test_output);
	}
	printf("=== FILTER READY FOR OPERATION ===\r\n");
#else
	printf("INFO: Data preprocessing disabled, using raw data\r\n");
#endif

#if ENABLE_COARSE_DETECTION
	/* 初始化粗检测算法 */
	printf("=== COARSE DETECTION INITIALIZATION ===\r\n");
	printf("RMS Window Size: %d samples (%.1f seconds @ 1000Hz)\r\n", RMS_WINDOW_SIZE, RMS_WINDOW_SIZE / 1000.0f);
	printf("Baseline RMS: %.6f g\r\n", BASELINE_RMS_THRESHOLD);
	printf("Trigger Multiplier: %.1fx\r\n", TRIGGER_MULTIPLIER);
	rc = Coarse_Detector_Init();
	if(rc != 0) {
		INV_MSG(INV_MSG_LEVEL_ERROR, "!!! ERROR : failed to initialize coarse detector.");
		return rc;
	}
	INV_MSG(INV_MSG_LEVEL_INFO, "Coarse detector initialized successfully");
	printf("=== COARSE DETECTION READY ===\r\n");
#endif

	// RINGBUFFER_CLEAR(&timestamp_buffer);
	return rc;
}



int ConfigureInvDevice(uint8_t is_low_noise_mode,
		uint8_t is_high_res_mode,
		IIM423XX_ACCEL_CONFIG0_FS_SEL_t acc_fsr_g,
		IIM423XX_ACCEL_CONFIG0_ODR_t acc_freq,
		uint8_t is_rtc_mode)
{
	int rc = 0;
	
	if (!is_rtc_mode) {
		/*
		 * Compute the time drift between the MCU and ICM clock
		 */
		rc |= clock_calibration_init(&icm_driver, &clk_calib);
	} else {
		clock_calibration_reset(&icm_driver, &clk_calib);
		//clk_calib.coef[INV_IIM423XX_PLL] = 1.0f;
		clk_calib.coef[INV_IIM423XX_RC_OSC] = 1.0f;
		clk_calib.coef[INV_IIM423XX_WU_OSC] = 1.0f;

	}

	/* 
	 * Force or prevent CLKIN usage depending on example configuration
	 * Note that CLKIN can't be forced if part is not trimmed accordingly
	 * It can be always disabled however, whatever the part used
	 */
	rc |= inv_iim423xx_enable_clkin_rtc(&icm_driver, is_rtc_mode);

	if(is_high_res_mode)
		rc |= inv_iim423xx_enable_high_resolution_fifo(&icm_driver);
	else {
		rc |= inv_iim423xx_set_accel_fsr(&icm_driver, acc_fsr_g);

	}
	
	rc |= inv_iim423xx_set_accel_frequency(&icm_driver, acc_freq);
	

	{
	uint8_t data;

	//For  vibration sensing applications
	//set the IIM-423xx Accel filter values: ACCEL_AAF_DIS=0; ACCEL_AAF_DELT:63; ACCEL_AAF_DELTSQR: 3968; ACCEL_AAF_BITSHIFT=3
	rc |= inv_iim423xx_set_reg_bank(&icm_driver, 2);
	data = 0x7E;
	rc |= inv_iim423xx_write_reg(&icm_driver, MPUREG_ACCEL_CONFIG_STATIC2_B2, 1, &data);

	data = 0x80;
	rc |= inv_iim423xx_write_reg(&icm_driver, MPUREG_ACCEL_CONFIG_STATIC3_B2, 1, &data);

	data = 0x3F;
	rc |= inv_iim423xx_write_reg(&icm_driver, MPUREG_ACCEL_CONFIG_STATIC4_B2, 1, &data);

	rc |= inv_iim423xx_set_reg_bank(&icm_driver, 0); /* Set memory bank 0 */
	}


	
	if (is_low_noise_mode)
		rc |= inv_iim423xx_enable_accel_low_noise_mode(&icm_driver);
	else
		rc |= inv_iim423xx_enable_accel_low_power_mode(&icm_driver);
	

	return rc;
}


int GetDataFromInvDevice(void)
{
	/*
	 * Extract packets from FIFO. Callback defined at init time (i.e. 
	 * HandleInvDeviceFifoPacket) will be called for each valid packet extracted from 
	 * FIFO.
	 */
	return inv_iim423xx_get_data_from_fifo(&icm_driver);
}


void HandleInvDeviceFifoPacket(inv_iim423xx_sensor_event_t * event)
{
	uint64_t irq_timestamp = 0, extended_timestamp;
	int32_t accel[3];
	
	/*
	 * Extract the timestamp that was buffered when current packet IRQ fired. See 
	 * ext_interrupt_cb() in main.c for more details.
	 * As timestamp buffer is filled in interrupt handler, we should pop it with
	 * interrupts disabled to avoid any concurrent access.
	 */
	// inv_disable_irq();
	// if (!RINGBUFFER_EMPTY(&timestamp_buffer))
	// 	RINGBUFFER_POP(&timestamp_buffer, &irq_timestamp);
	// inv_enable_irq();
	
	/*
	 * Extend the 16-bit timestamp from the Iim423xx FIFO to a 64 bits timestamp.
	 */
	inv_helper_extend_timestamp_from_fifo(&icm_driver, &clk_calib, event->timestamp_fsync, irq_timestamp, event->sensor_mask, &extended_timestamp);	
	
	/*
	 * Compute raw data according to the format
	 */
	if(icm_driver.fifo_highres_enabled) {
		accel[0] = (((int32_t)event->accel[0] << 4)) | event->accel_high_res[0];
		accel[1] = (((int32_t)event->accel[1] << 4)) | event->accel_high_res[1];
		accel[2] = (((int32_t)event->accel[2] << 4)) | event->accel_high_res[2];
		
		
	} else {
		accel[0] = event->accel[0];
		accel[1] = event->accel[1];
		accel[2] = event->accel[2];
		

	}
	
	apply_mounting_matrix(icm_mounting_matrix, accel);

	/*
	 * Add accelerometer data to FFT processor and send raw data
	 * Using Z-axis (accel[2]) for vibration analysis
	 */
	if (event->sensor_mask & (1 << INV_IIM423XX_SENSOR_ACCEL)) {
		// Convert to float and normalize (assuming 4g range, 16-bit data)
		float32_t accel_x_g = (float32_t)accel[0] / 8192.0f; // Convert to g units
		float32_t accel_y_g = (float32_t)accel[1] / 8192.0f;
		float32_t accel_z_g = (float32_t)accel[2] / 8192.0f;

#if ENABLE_DATA_PREPROCESSING
		// 应用高通滤波器到Z轴数据 (用于震动分析)
		float32_t filtered_z_g = Highpass_Filter_Process(accel_z_g);

#if ENABLE_COARSE_DETECTION
		// 粗检测算法处理
		int trigger_detected = Coarse_Detector_Process(filtered_z_g);

		// 新架构：粗检测触发时，直接使用粗检测缓冲区进行FFT
		if (trigger_detected) {
			printf("=== NEW ARCHITECTURE: Coarse detection triggered! ===\r\n");

			// 获取粗检测缓冲区
			uint32_t buffer_size = 0;
			const float32_t* coarse_buffer = Coarse_Detector_GetBuffer(&buffer_size);

			if (coarse_buffer != NULL && buffer_size == FFT_SIZE) {
				printf("NEW_ARCH: Got coarse buffer with %lu samples, processing FFT...\r\n", buffer_size);

				// 验证粗检测缓冲区的RMS与粗检测计算的RMS是否一致
				float32_t verify_sum_squares = 0.0f;
				for (uint32_t i = 0; i < buffer_size; i++) {
					verify_sum_squares += coarse_buffer[i] * coarse_buffer[i];
				}
				float32_t verify_rms = sqrtf(verify_sum_squares / buffer_size);
				printf("NEW_ARCH_VERIFY: Coarse RMS=%.6f, Buffer RMS=%.6f, Match=%s\r\n",
				       coarse_detector.current_rms, verify_rms,
				       (fabsf(coarse_detector.current_rms - verify_rms) < 0.001f) ? "YES" : "NO");

				// 直接使用粗检测缓冲区进行FFT处理
				int fft_result = FFT_ProcessBuffer(coarse_buffer, buffer_size);

				if (fft_result == 0) {
					printf("NEW_ARCH: FFT processing successful!\r\n");

					// 获取FFT结果
					const fft_result_t* result_ptr = FFT_GetResults();
					if (result_ptr != NULL) {
						printf("NEW_ARCH: FFT Results - freq=%.2f Hz, mag=%.6f, energy=%.6f\r\n",
						       result_ptr->dominant_frequency,
						       result_ptr->dominant_magnitude,
						       result_ptr->total_energy);

						// 调用精检测
						fine_detection_features_t features;
						int fine_result = Fine_Detector_Process(
							result_ptr->magnitude_spectrum,
							FFT_OUTPUT_POINTS,
							result_ptr->dominant_frequency,
							&features
						);

						if (fine_result == 0) {
							printf("NEW_ARCH: Fine detection complete!\r\n");
						} else {
							printf("NEW_ARCH: Fine detection failed with error %d\r\n", fine_result);
						}
					} else {
						printf("NEW_ARCH: ERROR - Failed to get FFT results\r\n");
					}
				} else {
					printf("NEW_ARCH: ERROR - FFT processing failed with error %d\r\n", fft_result);
				}
			} else {
				printf("NEW_ARCH: ERROR - Failed to get coarse buffer (buffer=%p, size=%lu)\r\n",
				       coarse_buffer, buffer_size);
			}
		}
#else
		// 连续模式：不使用粗检测，直接使用FFT（保留用于向后兼容）
		// 注意：新架构下不推荐使用此模式
		printf("WARNING: Continuous mode without coarse detection is not recommended in new architecture\r\n");
#endif

#else
		// 原始处理方式 (向后兼容)
		// 注意：新架构下不推荐使用此模式
		printf("WARNING: Raw data mode without preprocessing is not recommended in new architecture\r\n");
#endif

		// 数据处理完成 - 阶段1高通滤波器工作正常，阶段2粗检测集成
	}

	/*
	 * 移除原始数据输出，只保留FFT处理
	 */
	// 注释掉原始数据输出，专注于频域数据
	// if (event->sensor_mask & (1 << INV_IIM423XX_SENSOR_ACCEL))
	//     INV_MSG(INV_MSG_LEVEL_INFO, "%u: %d, %d, %d, %d", (uint32_t)irq_timestamp,
	//             accel[0], accel[1], accel[2], event->temperature);

}

/* --------------------------------------------------------------------------------------
 *  Static functions definition
 * -------------------------------------------------------------------------------------- */

static void apply_mounting_matrix(const int32_t matrix[9], int32_t raw[3])
{
	unsigned i;
	int64_t data_q30[3];

	for(i = 0; i < 3; i++) {
		data_q30[i] =  ((int64_t)matrix[3*i+0] * raw[0]);
		data_q30[i] += ((int64_t)matrix[3*i+1] * raw[1]);
		data_q30[i] += ((int64_t)matrix[3*i+2] * raw[2]);
	}
	raw[0] = (int32_t)(data_q30[0]>>30);
	raw[1] = (int32_t)(data_q30[1]>>30);
	raw[2] = (int32_t)(data_q30[2]>>30);
}

/* 原始加速度数据发送函数已删除 - 调试串口现在专用于调试信息输出 */

#if ENABLE_DATA_PREPROCESSING
/* --------------------------------------------------------------------------------------
 *  高通滤波器实现 - 用于震动数据预处理
 * -------------------------------------------------------------------------------------- */

int Highpass_Filter_Init(void)
{
    printf("DEBUG: Starting highpass filter initialization...\r\n");

    // 清零滤波器状态
    memset(&z_axis_filter, 0, sizeof(highpass_filter_t));
    printf("DEBUG: Filter structure cleared\r\n");

    // 手动复制滤波器系数 (避免memcpy问题)
    for(int i = 0; i < 10; i++) {
        z_axis_filter.filter_coeffs[i] = highpass_coeffs[i];
    }
    printf("DEBUG: Filter coefficients manually copied\r\n");

    // 验证复制结果
    printf("DEBUG: Source coefficients:\r\n");
    for(int i = 0; i < 10; i++) {
        printf("  src[%d] = %.6f\r\n", i, highpass_coeffs[i]);
    }
    printf("DEBUG: Destination coefficients:\r\n");
    for(int i = 0; i < 10; i++) {
        printf("  dst[%d] = %.6f\r\n", i, z_axis_filter.filter_coeffs[i]);
    }

    printf("DEBUG: Filter coefficients loaded:\r\n");
    printf("  Stage1: b0=%.6f, b1=%.6f, b2=%.6f, a1=%.6f, a2=%.6f\r\n",
           z_axis_filter.filter_coeffs[0], z_axis_filter.filter_coeffs[1],
           z_axis_filter.filter_coeffs[2], z_axis_filter.filter_coeffs[3], z_axis_filter.filter_coeffs[4]);
    printf("  Stage2: b0=%.6f, b1=%.6f, b2=%.6f, a1=%.6f, a2=%.6f\r\n",
           z_axis_filter.filter_coeffs[5], z_axis_filter.filter_coeffs[6],
           z_axis_filter.filter_coeffs[7], z_axis_filter.filter_coeffs[8], z_axis_filter.filter_coeffs[9]);

    // 手动初始化CMSIS DSP滤波器实例 (避开有问题的初始化函数)
    printf("DEBUG: Manual CMSIS DSP initialization...\r\n");
    z_axis_filter.filter_instance.numStages = 2;  // 2个biquad段
    z_axis_filter.filter_instance.pCoeffs = z_axis_filter.filter_coeffs;
    z_axis_filter.filter_instance.pState = z_axis_filter.filter_state;

    // 清零状态数组 (2个biquad段，每段2个状态变量)
    for(int i = 0; i < 4; i++) {
        z_axis_filter.filter_state[i] = 0.0f;
    }
    printf("DEBUG: Filter state array cleared (4 elements)\r\n");

    printf("DEBUG: Manual initialization completed\r\n");

    // 验证手动初始化后系数是否完整
    printf("DEBUG: Post-manual-init coefficients check:\r\n");
    for(int i = 0; i < 10; i++) {
        printf("  manual[%d] = %.6f\r\n", i, z_axis_filter.filter_coeffs[i]);
    }

    z_axis_filter.is_initialized = true;

    // 高通滤波器初始化完成

    return 0;
}

float32_t Highpass_Filter_Process(float32_t input)
{
    static uint32_t process_count = 0;
    static uint32_t debug_count = 0;

    // 直接形式IIR滤波器状态变量 (避开CMSIS DSP)
    static float32_t x1 = 0.0f, x2 = 0.0f;  // 输入延迟
    static float32_t y1 = 0.0f, y2 = 0.0f;  // 输出延迟
    static float32_t x1_2 = 0.0f, x2_2 = 0.0f;  // 第二段输入延迟
    static float32_t y1_2 = 0.0f, y2_2 = 0.0f;  // 第二段输出延迟

    process_count++;
    debug_count++;

    if (!z_axis_filter.is_initialized) {
        if (process_count % 1000 == 1) {
            printf("ERROR: Highpass filter not initialized! Returning raw input.\r\n");
        }
        return input;
    }

    // 第一个biquad段: b0=0.959782, b1=-1.919564, b2=0.959782, a1=1.942638, a2=-0.943597
    float32_t stage1_out = 0.959782f * input + (-1.919564f) * x1 + 0.959782f * x2
                          + 1.942638f * y1 + (-0.943597f) * y2;

    // 更新第一段状态
    x2 = x1; x1 = input;
    y2 = y1; y1 = stage1_out;

    // 第二个biquad段: b0=1.000000, b1=-2.000000, b2=1.000000, a1=1.975270, a2=-0.976245
    float32_t stage2_out = 1.000000f * stage1_out + (-2.000000f) * x1_2 + 1.000000f * x2_2
                          + 1.975270f * y1_2 + (-0.976245f) * y2_2;

    // 更新第二段状态
    x2_2 = x1_2; x1_2 = stage1_out;
    y2_2 = y1_2; y1_2 = stage2_out;

    // 滤波器工作正常，无需调试输出

    return stage2_out;
}

void Highpass_Filter_Reset(void)
{
    if (z_axis_filter.is_initialized) {
        // 清零滤波器状态，保持系数不变
        memset(z_axis_filter.filter_state, 0, sizeof(z_axis_filter.filter_state));
    }
}
#endif

#if ENABLE_COARSE_DETECTION
/* --------------------------------------------------------------------------------------
 *  粗检测算法实现 - 基于RMS滑动窗口和峰值因子
 * -------------------------------------------------------------------------------------- */

int Coarse_Detector_Init(void)
{
    // 清零粗检测器结构
    memset(&coarse_detector, 0, sizeof(coarse_detector_t));

    // 初始化参数
    coarse_detector.baseline_rms = BASELINE_RMS_THRESHOLD;  // 初始基线RMS
    coarse_detector.state = COARSE_STATE_IDLE;
    coarse_detector.window_index = 0;
    coarse_detector.window_full = false;
    coarse_detector.is_initialized = true;

    // 粗检测算法初始化完成

    return 0;
}

int Coarse_Detector_Process(float32_t filtered_sample)
{
    static uint32_t debug_counter = 0;
    debug_counter++;

    if (!coarse_detector.is_initialized) {
        return 0;
    }

    // 新架构：存储原始样本值（不是平方值），用于FFT处理
    // RMS计算时再平方
    coarse_detector.rms_window[coarse_detector.window_index] = filtered_sample;  // 原始值
    coarse_detector.window_index = (coarse_detector.window_index + 1) % RMS_WINDOW_SIZE;

    if (!coarse_detector.window_full && coarse_detector.window_index == 0) {
        coarse_detector.window_full = true;
        printf("COARSE_DEBUG: RMS window is now full, detection active\r\n");
    }

    // 计算当前RMS (仅在窗口满后)
    if (coarse_detector.window_full) {
        float32_t sum_squares = 0.0f;
        float32_t max_val = -1000.0f;
        float32_t min_val = 1000.0f;

        for (int i = 0; i < RMS_WINDOW_SIZE; i++) {
            // 计算RMS时再平方
            float32_t sample = coarse_detector.rms_window[i];
            sum_squares += sample * sample;

            // 统计最大最小值
            if (sample > max_val) max_val = sample;
            if (sample < min_val) min_val = sample;
        }
        coarse_detector.current_rms = sqrtf(sum_squares / RMS_WINDOW_SIZE);

        // 计算峰值因子
        coarse_detector.peak_factor = coarse_detector.current_rms / coarse_detector.baseline_rms;

        // 详细调试输出（每512个样本输出一次）
        if (debug_counter % 512 == 0) {
            printf("COARSE_STATS: RMS=%.6f, Max=%.6f, Min=%.6f, Range=%.6f, Peak_Factor=%.2f\r\n",
                   coarse_detector.current_rms, max_val, min_val, max_val - min_val,
                   coarse_detector.peak_factor);
        }

        // 每512个样本输出一次调试信息（窗口满一次）
        if (debug_counter % 512 == 0) {
            printf("COARSE_DEBUG: RMS=%.6f baseline=%.6f peak_factor=%.2f state=%d\r\n",
                   coarse_detector.current_rms, coarse_detector.baseline_rms,
                   coarse_detector.peak_factor, coarse_detector.state);
        }

        // 状态机处理
        uint32_t current_time = HAL_GetTick();

        switch (coarse_detector.state) {
            case COARSE_STATE_IDLE:
                if (coarse_detector.peak_factor > TRIGGER_MULTIPLIER) {
                    coarse_detector.state = COARSE_STATE_TRIGGERED;
                    coarse_detector.trigger_start_time = current_time;
                    coarse_detector.trigger_count++;
                    printf("COARSE_TRIGGER: RMS=%.6f peak_factor=%.2f TRIGGERED!\r\n",
                           coarse_detector.current_rms, coarse_detector.peak_factor);

                    // 通知状态机粗检测触发
                    #if ENABLE_SYSTEM_STATE_MACHINE
                    System_State_Machine_SetCoarseTrigger(1);
                    #endif

                    return 1;  // 触发检测到
                }
                break;

            case COARSE_STATE_TRIGGERED:
                if (current_time - coarse_detector.trigger_start_time > TRIGGER_DURATION_MS) {
                    coarse_detector.state = COARSE_STATE_COOLDOWN;
                    coarse_detector.cooldown_start_time = current_time;
                }
                break;

            case COARSE_STATE_COOLDOWN:
                if (current_time - coarse_detector.cooldown_start_time > COOLDOWN_TIME_MS) {
                    coarse_detector.state = COARSE_STATE_IDLE;
                    // 更新基线RMS (简单的指数移动平均)
                    coarse_detector.baseline_rms = 0.95f * coarse_detector.baseline_rms + 0.05f * coarse_detector.current_rms;
                }
                break;
        }
    }

    return 0;  // 无触发
}

coarse_detection_state_t Coarse_Detector_GetState(void)
{
    return coarse_detector.state;
}

void Coarse_Detector_Reset(void)
{
    if (coarse_detector.is_initialized) {
        coarse_detector.state = COARSE_STATE_IDLE;
        coarse_detector.window_index = 0;
        coarse_detector.window_full = false;
        memset(coarse_detector.rms_window, 0, sizeof(coarse_detector.rms_window));
    }
}

bool Coarse_Detector_IsWindowFull(void)
{
    if (!coarse_detector.is_initialized) {
        return false;
    }
    return coarse_detector.window_full;
}

const float32_t* Coarse_Detector_GetBuffer(uint32_t* buffer_size)
{
    if (!coarse_detector.is_initialized || !coarse_detector.window_full) {
        printf("COARSE_BUFFER: Cannot get buffer - not initialized or window not full\r\n");
        return NULL;
    }

    if (buffer_size != NULL) {
        *buffer_size = RMS_WINDOW_SIZE;
    }

    // 重新排列循环缓冲区数据为时间顺序
    // 当前window_index指向最新数据的下一个位置（即最老数据的位置）
    // 需要重新排列：[window_index, window_index+1, ..., 511, 0, 1, ..., window_index-1]
    // 这样ordered_buffer[0]是最老的数据，ordered_buffer[511]是最新的数据

    uint32_t src_index = coarse_detector.window_index;  // 最老数据的位置

    for (uint32_t i = 0; i < RMS_WINDOW_SIZE; i++) {
        coarse_detector.ordered_buffer[i] = coarse_detector.rms_window[src_index];
        src_index = (src_index + 1) % RMS_WINDOW_SIZE;
    }

    printf("COARSE_BUFFER: Reordered buffer with %d samples (oldest at index %lu)\r\n",
           RMS_WINDOW_SIZE, coarse_detector.window_index);

    // 调试：输出前5个和后5个样本
    printf("COARSE_BUFFER: First 5 samples: %.6f, %.6f, %.6f, %.6f, %.6f\r\n",
           coarse_detector.ordered_buffer[0],
           coarse_detector.ordered_buffer[1],
           coarse_detector.ordered_buffer[2],
           coarse_detector.ordered_buffer[3],
           coarse_detector.ordered_buffer[4]);
    printf("COARSE_BUFFER: Last 5 samples: %.6f, %.6f, %.6f, %.6f, %.6f\r\n",
           coarse_detector.ordered_buffer[RMS_WINDOW_SIZE-5],
           coarse_detector.ordered_buffer[RMS_WINDOW_SIZE-4],
           coarse_detector.ordered_buffer[RMS_WINDOW_SIZE-3],
           coarse_detector.ordered_buffer[RMS_WINDOW_SIZE-2],
           coarse_detector.ordered_buffer[RMS_WINDOW_SIZE-1]);

    return coarse_detector.ordered_buffer;
}
#endif

#if ENABLE_FINE_DETECTION
/* --------------------------------------------------------------------------------------
 *  细检测算法实现 - 基于5维频域特征提取和规则分类器
 * -------------------------------------------------------------------------------------- */

int Fine_Detector_Init(void)
{
    fine_detector_initialized = true;
    printf("=== FINE DETECTION INITIALIZED ===\r\n");
    return 0;
}

static float32_t calculate_frequency_bin_energy(const float32_t* magnitude_spectrum,
                                               uint32_t spectrum_length,
                                               float32_t freq_min,
                                               float32_t freq_max)
{
    // FFT配置：采样频率1000Hz，FFT大小512，频率分辨率1.953125Hz
    const float32_t freq_resolution = 1000.0f / 512.0f;  // 1.953125 Hz

    // 计算频段对应的bin索引（向上取整，确保不包含低于freq_min的频率）
    uint32_t bin_min = (uint32_t)((freq_min / freq_resolution) + 0.5f);  // 向上取整
    uint32_t bin_max = (uint32_t)((freq_max / freq_resolution) + 0.5f);  // 向上取整

    // 限制在有效范围内
    if (bin_min >= spectrum_length) bin_min = spectrum_length - 1;
    if (bin_max >= spectrum_length) bin_max = spectrum_length - 1;
    if (bin_max < bin_min) bin_max = bin_min;

    // 计算频段能量 (幅度平方和)
    float32_t energy = 0.0f;
    for (uint32_t i = bin_min; i <= bin_max; i++) {
        energy += magnitude_spectrum[i] * magnitude_spectrum[i];
    }

    printf("FREQ_BIN_ENERGY_DEBUG: freq_range=[%.2f-%.2f]Hz, bin_range=[%lu-%lu], energy=%.6f\r\n",
           freq_min, freq_max, bin_min, bin_max, energy);

    return energy;
}

static float32_t calculate_total_energy(const float32_t* magnitude_spectrum,
                                       uint32_t spectrum_length)
{
    float32_t total_energy = 0.0f;

    // 排除低频噪音：只计算5Hz以上的能量
    // FFT频率分辨率：1.953125 Hz
    // 5Hz对应的bin: 5 / 1.953125 ≈ 2.56 → 向上取整到 bin 3 (5.86 Hz)
    const float32_t freq_resolution = 1000.0f / 512.0f;  // 1.953125 Hz
    const uint32_t min_bin = (uint32_t)((5.0f / freq_resolution) + 0.5f);  // 向上取整到 bin 3

    // 从5Hz以上开始计算，排除0-5Hz的噪音（包括2Hz噪音）
    for (uint32_t i = min_bin; i < spectrum_length; i++) {
        total_energy += magnitude_spectrum[i] * magnitude_spectrum[i];
    }

    printf("TOTAL_ENERGY_DEBUG: Calculated from bin %lu (%.2f Hz) to bin %lu, energy=%.6f\r\n",
           min_bin, min_bin * freq_resolution, spectrum_length - 1, total_energy);

    return total_energy;
}

static float32_t calculate_spectral_centroid(const float32_t* magnitude_spectrum,
                                            uint32_t spectrum_length)
{
    const float32_t freq_resolution = 1000.0f / 512.0f;  // 1.953125 Hz

    float32_t weighted_sum = 0.0f;
    float32_t magnitude_sum = 0.0f;

    // 排除低频噪音：从5Hz以上开始计算
    const uint32_t min_bin = (uint32_t)((5.0f / freq_resolution) + 0.5f);  // 向上取整到 bin 3 (5.86 Hz)

    for (uint32_t i = min_bin; i < spectrum_length; i++) {
        float32_t frequency = i * freq_resolution;
        float32_t magnitude_squared = magnitude_spectrum[i] * magnitude_spectrum[i];

        weighted_sum += frequency * magnitude_squared;
        magnitude_sum += magnitude_squared;
    }

    // 避免除零
    if (magnitude_sum < 1e-10f) {
        return 0.0f;
    }

    return weighted_sum / magnitude_sum;
}

static float32_t calculate_confidence_score(const fine_detection_features_t* features)
{
    // 规则权重（调整以适应真实震动特征）
    const float32_t w_low = 0.5f;    // 低频能量权重（提高，因为挖掘震动主要在低频）
    const float32_t w_mid = 0.2f;    // 中频能量权重（保持）
    const float32_t w_dom = 0.2f;    // 主频权重（保持）
    const float32_t w_cent = 0.1f;   // 频谱重心权重（降低）

    // 计算各项得分 (0-1范围)
    float32_t low_freq_score = (features->low_freq_energy > FINE_DETECTION_LOW_FREQ_THRESHOLD) ?
                               (features->low_freq_energy / FINE_DETECTION_LOW_FREQ_THRESHOLD) : 0.0f;
    if (low_freq_score > 1.0f) low_freq_score = 1.0f;

    // 中频能量得分：改为渐进式得分，而不是二元判定
    // 这样可以更好地反映中频能量的贡献
    float32_t mid_freq_score = (features->mid_freq_energy / FINE_DETECTION_MID_FREQ_THRESHOLD);
    if (mid_freq_score > 1.0f) mid_freq_score = 1.0f;

    float32_t dominant_freq_score = (features->dominant_frequency < FINE_DETECTION_DOMINANT_FREQ_MAX) ?
                                   (FINE_DETECTION_DOMINANT_FREQ_MAX - features->dominant_frequency) / FINE_DETECTION_DOMINANT_FREQ_MAX : 0.0f;

    float32_t centroid_score = (features->spectral_centroid < FINE_DETECTION_CENTROID_MAX) ?
                              (FINE_DETECTION_CENTROID_MAX - features->spectral_centroid) / FINE_DETECTION_CENTROID_MAX : 0.0f;

    // 加权计算总置信度
    float32_t confidence = w_low * low_freq_score +
                          w_mid * mid_freq_score +
                          w_dom * dominant_freq_score +
                          w_cent * centroid_score;

    // 限制在0-1范围内
    if (confidence < 0.0f) confidence = 0.0f;
    if (confidence > 1.0f) confidence = 1.0f;

    return confidence;
}

int Fine_Detector_Process(const float32_t* magnitude_spectrum,
                         uint32_t spectrum_length,
                         float32_t dominant_freq,
                         fine_detection_features_t* features)
{
    if (!fine_detector_initialized || !magnitude_spectrum || !features || spectrum_length != 257) {
        return -1;
    }

    uint32_t start_time = HAL_GetTick();

    // 清零输出结构
    memset(features, 0, sizeof(fine_detection_features_t));

    // 计算总能量
    float32_t total_energy = calculate_total_energy(magnitude_spectrum, spectrum_length);

    // 避免除零
    if (total_energy < 1e-10f) {
        features->is_valid = false;
        return 0;
    }

    // 1. 计算低频能量占比 (5-15Hz)
    float32_t low_freq_energy = calculate_frequency_bin_energy(magnitude_spectrum, spectrum_length,
                                                              FINE_DETECTION_LOW_FREQ_MIN,
                                                              FINE_DETECTION_LOW_FREQ_MAX);
    features->low_freq_energy = low_freq_energy / total_energy;

    // 2. 计算中频能量占比 (15-30Hz)
    float32_t mid_freq_energy = calculate_frequency_bin_energy(magnitude_spectrum, spectrum_length,
                                                              FINE_DETECTION_MID_FREQ_MIN,
                                                              FINE_DETECTION_MID_FREQ_MAX);
    features->mid_freq_energy = mid_freq_energy / total_energy;

    // 3. 计算高频能量占比 (30-100Hz)
    float32_t high_freq_energy = calculate_frequency_bin_energy(magnitude_spectrum, spectrum_length,
                                                               FINE_DETECTION_HIGH_FREQ_MIN,
                                                               FINE_DETECTION_HIGH_FREQ_MAX);
    features->high_freq_energy = high_freq_energy / total_energy;

    // 4. 主频 (直接使用FFT结果)
    features->dominant_frequency = dominant_freq;

    // 5. 计算频谱重心
    features->spectral_centroid = calculate_spectral_centroid(magnitude_spectrum, spectrum_length);

    // 计算置信度
    features->confidence_score = calculate_confidence_score(features);

    // 详细调试输出
    printf("FINE_DETECTION_DEBUG: ===== Feature Analysis =====\r\n");
    printf("  Total Energy (5-500Hz): %.6f\r\n", total_energy);
    printf("  Low Freq Energy (5-15Hz):  %.6f (%.2f%%, threshold: %.2f)\r\n",
           features->low_freq_energy, features->low_freq_energy * 100.0f,
           FINE_DETECTION_LOW_FREQ_THRESHOLD);
    printf("  Mid Freq Energy (15-30Hz): %.6f (%.2f%%, threshold: %.2f)\r\n",
           features->mid_freq_energy, features->mid_freq_energy * 100.0f,
           FINE_DETECTION_MID_FREQ_THRESHOLD);
    printf("  High Freq Energy (30-100Hz): %.6f (%.2f%%)\r\n",
           features->high_freq_energy, features->high_freq_energy * 100.0f);
    printf("  Dominant Frequency: %.2f Hz (max: %.2f Hz)\r\n",
           features->dominant_frequency, FINE_DETECTION_DOMINANT_FREQ_MAX);
    printf("  Spectral Centroid: %.2f Hz (max: %.2f Hz)\r\n",
           features->spectral_centroid, FINE_DETECTION_CENTROID_MAX);
    printf("  Confidence Score: %.4f (threshold: %.2f)\r\n",
           features->confidence_score, FINE_DETECTION_CONFIDENCE_THRESHOLD);

    // 能量分布验证
    float32_t energy_sum = features->low_freq_energy + features->mid_freq_energy + features->high_freq_energy;
    printf("  Energy Distribution Check: Low+Mid+High = %.6f (should be <= 1.0)\r\n", energy_sum);

    // 分类决策：添加主频过滤，排除低频噪音
    // 如果主频 < 5Hz，很可能是2Hz噪音，直接判定为正常振动
    if (features->dominant_frequency < 5.0f) {
        features->classification = FINE_DETECTION_NORMAL;
        printf("  >>> NOISE FILTER: Dominant freq < 5Hz, classified as NORMAL (likely 2Hz noise)\r\n");
    } else {
        features->classification = (features->confidence_score >= FINE_DETECTION_CONFIDENCE_THRESHOLD) ?
                                  FINE_DETECTION_MINING : FINE_DETECTION_NORMAL;
    }

    printf("  Classification: %s\r\n",
           (features->classification == FINE_DETECTION_MINING) ? "MINING" : "NORMAL");
    printf("FINE_DETECTION_DEBUG: =============================\r\n");

    // 性能统计
    features->analysis_timestamp = HAL_GetTick();
    features->computation_time_us = (features->analysis_timestamp - start_time) * 1000;  // 转换为微秒
    features->is_valid = true;

    // 通知状态机细检测结果
    #if ENABLE_SYSTEM_STATE_MACHINE
    uint8_t result = (features->classification == FINE_DETECTION_MINING) ? 2 : 1;
    System_State_Machine_SetFineResult(result);
    #endif

    return 0;
}

void Fine_Detector_PrintResults(const fine_detection_features_t* features)
{
    if (!features || !features->is_valid) {
        return;
    }

    // 输出细检测结果
    printf("FINE_DETECTION: class=%s conf=%.2f low=%.2f mid=%.2f high=%.2f centroid=%.1fHz\r\n",
           (features->classification == FINE_DETECTION_MINING) ? "MINING" : "NORMAL",
           features->confidence_score,
           features->low_freq_energy,
           features->mid_freq_energy,
           features->high_freq_energy,
           features->spectral_centroid);
}
#endif

#if ENABLE_SYSTEM_STATE_MACHINE
/* 阶段5：系统状态机实现 */

/**
 * @brief 状态转换函数
 * @param new_state: 新状态
 */
static void transition_to_state(system_state_t new_state)
{
    if (new_state >= STATE_COUNT) {
        printf("STATE_ERROR: Invalid state %d\r\n", new_state);
        return;
    }

    // 记录状态转换
    g_state_machine.previous_state = g_state_machine.current_state;
    g_state_machine.current_state = new_state;
    g_state_machine.state_enter_time = HAL_GetTick();
    g_state_machine.transition_count++;
    g_state_machine.state_count[new_state]++;

    // 调试输出
    printf("STATE_TRANSITION: %s -> %s (transition #%lu)\r\n",
           state_names[g_state_machine.previous_state],
           state_names[new_state],
           g_state_machine.transition_count);
}

/**
 * @brief 系统初始化状态处理
 */
static void handle_system_init(void)
{
    // 系统初始化完成，进入监测模式
    transition_to_state(STATE_MONITORING);
}

/**
 * @brief 深度休眠状态处理
 */
static void handle_idle_sleep(void)
{
    // 深度休眠逻辑 (阶段6实现)
    // 当前直接进入监测模式
    transition_to_state(STATE_MONITORING);
}

/**
 * @brief 监测模式状态处理
 */
static void handle_monitoring(void)
{
    // 检查粗检测触发
    if (g_state_machine.coarse_trigger_flag) {
        g_state_machine.coarse_trigger_flag = 0;  // 清除标志
        transition_to_state(STATE_COARSE_TRIGGERED);
        return;
    }

    // 监测超时检查 (可选)
    uint32_t current_time = HAL_GetTick();
    if (current_time - g_state_machine.state_enter_time > STATE_MONITORING_TIMEOUT_MS) {
        // 长时间无活动，可以考虑进入休眠 (阶段6实现)
        // 当前重置状态进入时间
        g_state_machine.state_enter_time = current_time;
    }
}

/**
 * @brief 粗检测触发状态处理
 */
static void handle_coarse_triggered(void)
{
    // 粗检测触发后，等待细检测结果
    transition_to_state(STATE_FINE_ANALYSIS);
}

/**
 * @brief 细检测分析状态处理
 */
static void handle_fine_analysis(void)
{
    // 检查细检测结果
    if (g_state_machine.fine_analysis_result != 0) {
        printf("STATE_DEBUG: Fine analysis result = %d\r\n", g_state_machine.fine_analysis_result);
        if (g_state_machine.fine_analysis_result == 2) {
            // 检测到挖掘震动
            g_state_machine.total_detections++;
            g_state_machine.mining_detections++;
            printf("STATE_INFO: Transitioning to MINING_DETECTED state\r\n");
            transition_to_state(STATE_MINING_DETECTED);
        } else {
            // 正常震动，返回监测模式
            g_state_machine.total_detections++;
            printf("STATE_INFO: Normal vibration, returning to MONITORING\r\n");
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

/**
 * @brief 挖掘检测状态处理
 */
static void handle_mining_detected(void)
{
    printf("STATE_INFO: Mining vibration detected! Triggering alarm...\r\n");

    // 触发报警 (集成现有的报警状态机)
    extern void Trigger_Alarm_Cycle(void);
    Trigger_Alarm_Cycle();

    transition_to_state(STATE_ALARM_SENDING);
}

/**
 * @brief 报警发送状态处理
 */
static void handle_alarm_sending(void)
{
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

/**
 * @brief 报警完成状态处理
 */
static void handle_alarm_complete(void)
{
    printf("STATE_INFO: Alarm cycle completed successfully\r\n");

    // 报警完成，返回监测模式
    transition_to_state(STATE_MONITORING);
}

/**
 * @brief 错误处理状态处理
 */
static void handle_error_handling(void)
{
    printf("STATE_ERROR: Error code %d, recovering...\r\n", g_state_machine.error_code);

    // 错误恢复延迟
    uint32_t current_time = HAL_GetTick();
    if (current_time - g_state_machine.state_enter_time > STATE_ERROR_RECOVERY_DELAY_MS) {
        // 清除错误，返回监测模式
        g_state_machine.error_code = 0;
        transition_to_state(STATE_MONITORING);
    }
}

/**
 * @brief 系统重置状态处理
 */
static void handle_system_reset(void)
{
    printf("STATE_INFO: System reset requested\r\n");

    // 重置状态机
    memset(&g_state_machine, 0, sizeof(g_state_machine));
    g_state_machine.current_state = STATE_SYSTEM_INIT;
    g_state_machine.state_enter_time = HAL_GetTick();
}

/**
 * @brief 初始化系统状态机
 * @return 0: 成功, -1: 失败
 */
int System_State_Machine_Init(void)
{
    if (state_machine_initialized) {
        return 0;  // 已经初始化
    }

    // 清零状态机结构体
    memset(&g_state_machine, 0, sizeof(g_state_machine));

    // 设置初始状态
    g_state_machine.current_state = STATE_SYSTEM_INIT;
    g_state_machine.previous_state = STATE_SYSTEM_INIT;
    g_state_machine.state_enter_time = HAL_GetTick();

    state_machine_initialized = 1;

    printf("=== SYSTEM STATE MACHINE INITIALIZED ===\r\n");
    return 0;
}

/**
 * @brief 主状态机处理函数
 */
void System_State_Machine_Process(void)
{
    if (!state_machine_initialized) {
        return;
    }

    // 更新状态持续时间
    g_state_machine.state_duration = HAL_GetTick() - g_state_machine.state_enter_time;

    // 根据当前状态调用相应处理函数
    switch (g_state_machine.current_state) {
        case STATE_SYSTEM_INIT:
            handle_system_init();
            break;

        case STATE_IDLE_SLEEP:
            handle_idle_sleep();
            break;

        case STATE_MONITORING:
            handle_monitoring();
            break;

        case STATE_COARSE_TRIGGERED:
            handle_coarse_triggered();
            break;

        case STATE_FINE_ANALYSIS:
            handle_fine_analysis();
            break;

        case STATE_MINING_DETECTED:
            handle_mining_detected();
            break;

        case STATE_ALARM_SENDING:
            handle_alarm_sending();
            break;

        case STATE_ALARM_COMPLETE:
            handle_alarm_complete();
            break;

        case STATE_ERROR_HANDLING:
            handle_error_handling();
            break;

        case STATE_SYSTEM_RESET:
            handle_system_reset();
            break;

        default:
            printf("STATE_ERROR: Unknown state %d\r\n", g_state_machine.current_state);
            g_state_machine.error_code = 1;
            transition_to_state(STATE_ERROR_HANDLING);
            break;
    }
}

/**
 * @brief 设置粗检测触发标志
 * @param triggered: 1=触发, 0=未触发
 */
void System_State_Machine_SetCoarseTrigger(uint8_t triggered)
{
    if (triggered) {
        g_state_machine.coarse_trigger_flag = 1;
        printf("STATE_EVENT: Coarse detection triggered\r\n");
    }
}

/**
 * @brief 设置细检测结果
 * @param result: 0=无效, 1=正常, 2=挖掘
 */
void System_State_Machine_SetFineResult(uint8_t result)
{
    printf("STATE_DEBUG: SetFineResult called with result=%d, current_state=%s\r\n",
           result, state_names[g_state_machine.current_state]);
    g_state_machine.fine_analysis_result = result;
    if (result == 2) {
        printf("STATE_EVENT: Mining vibration detected by fine analysis\r\n");
    } else if (result == 1) {
        printf("STATE_EVENT: Normal vibration detected by fine analysis\r\n");
    }
}

/**
 * @brief 设置报警发送状态
 * @param status: 0=进行中, 1=成功, 2=失败
 */
void System_State_Machine_SetAlarmStatus(uint8_t status)
{
    g_state_machine.alarm_send_status = status;
    if (status == 1) {
        printf("STATE_EVENT: Alarm sent successfully\r\n");
    } else if (status == 2) {
        printf("STATE_EVENT: Alarm sending failed\r\n");
    }
}

/**
 * @brief 设置错误代码
 * @param error_code: 错误代码
 */
void System_State_Machine_SetError(uint8_t error_code)
{
    g_state_machine.error_code = error_code;
    printf("STATE_EVENT: Error occurred, code=%d\r\n", error_code);
}

/**
 * @brief 获取当前状态
 * @return 当前状态
 */
system_state_t System_State_Machine_GetCurrentState(void)
{
    return g_state_machine.current_state;
}

/**
 * @brief 打印状态机状态信息
 */
void System_State_Machine_PrintStatus(void)
{
    if (!state_machine_initialized) {
        printf("STATE_INFO: State machine not initialized\r\n");
        return;
    }

    printf("=== SYSTEM STATE MACHINE STATUS ===\r\n");
    printf("Current State: %s (duration: %lums)\r\n",
           state_names[g_state_machine.current_state],
           g_state_machine.state_duration);
    printf("Previous State: %s\r\n", state_names[g_state_machine.previous_state]);
    printf("Total Transitions: %lu\r\n", g_state_machine.transition_count);
    printf("Total Detections: %lu\r\n", g_state_machine.total_detections);
    printf("Mining Detections: %lu\r\n", g_state_machine.mining_detections);
    printf("False Alarms: %lu\r\n", g_state_machine.false_alarms);

    // 状态计数统计
    printf("State Counts:\r\n");
    for (int i = 0; i < STATE_COUNT; i++) {
        if (g_state_machine.state_count[i] > 0) {
            printf("  %s: %lu\r\n", state_names[i], g_state_machine.state_count[i]);
        }
    }
    printf("=====================================\r\n");
}

#endif

