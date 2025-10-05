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
static struct inv_iim423xx icm_driver;

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

		// 阶段3：使用FFT触发控制
		bool should_trigger = (trigger_detected || Coarse_Detector_GetState() == COARSE_STATE_TRIGGERED);
		FFT_SetTriggerState(should_trigger);

		// FFT处理现在由触发状态自动控制
		int result = FFT_AddSample(filtered_z_g);
#else
		// 使用滤波后的数据进行FFT处理 (连续模式)
		int result = FFT_AddSample(filtered_z_g);
#endif

#else
		// 原始处理方式 (向后兼容)
		int result = FFT_AddSample(accel_z_g);
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
    if (!coarse_detector.is_initialized) {
        return 0;
    }

    // 添加样本到RMS滑动窗口
    coarse_detector.rms_window[coarse_detector.window_index] = filtered_sample * filtered_sample;  // 平方值
    coarse_detector.window_index = (coarse_detector.window_index + 1) % RMS_WINDOW_SIZE;

    if (!coarse_detector.window_full && coarse_detector.window_index == 0) {
        coarse_detector.window_full = true;
    }

    // 计算当前RMS (仅在窗口满后)
    if (coarse_detector.window_full) {
        float32_t sum_squares = 0.0f;
        for (int i = 0; i < RMS_WINDOW_SIZE; i++) {
            sum_squares += coarse_detector.rms_window[i];
        }
        coarse_detector.current_rms = sqrtf(sum_squares / RMS_WINDOW_SIZE);

        // 计算峰值因子
        coarse_detector.peak_factor = coarse_detector.current_rms / coarse_detector.baseline_rms;

        // 状态机处理
        uint32_t current_time = HAL_GetTick();

        switch (coarse_detector.state) {
            case COARSE_STATE_IDLE:
                if (coarse_detector.peak_factor > TRIGGER_MULTIPLIER) {
                    coarse_detector.state = COARSE_STATE_TRIGGERED;
                    coarse_detector.trigger_start_time = current_time;
                    coarse_detector.trigger_count++;
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
#endif

