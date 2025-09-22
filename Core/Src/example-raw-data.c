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

		// Add Z-axis data to FFT processor
		int result = FFT_AddSample(accel_z_g);

		// Send raw accelerometer data every 100 samples (10Hz rate for raw data)
		static uint32_t raw_data_counter = 0;
		raw_data_counter++;
		if (raw_data_counter >= 100) {  // 1000Hz / 100 = 10Hz for raw data
			raw_data_counter = 0;
			Send_Raw_Accel_Data(accel_x_g, accel_y_g, accel_z_g);
		}
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

/**
 * @brief 发送原始加速度数据
 * @param accel_x X轴加速度 (g)
 * @param accel_y Y轴加速度 (g)
 * @param accel_z Z轴加速度 (g)
 */
void Send_Raw_Accel_Data(float32_t accel_x, float32_t accel_y, float32_t accel_z)
{
    // 构建协议帧数据
    uint8_t frame[23];  // 帧头(2) + 命令(1) + 长度(2) + 载荷(16) + 校验(1) + 帧尾(1) = 23字节
    uint16_t index = 0;

    // 帧头: AA 55
    frame[index++] = 0xAA;
    frame[index++] = 0x55;

    // 命令码: 02 (原始加速度数据)
    frame[index++] = 0x02;

    // 载荷长度: 16字节 (4字节时间戳 + 3*4字节float32)
    uint16_t payload_len = 16;
    frame[index++] = (uint8_t)(payload_len & 0xFF);        // 长度低字节
    frame[index++] = (uint8_t)((payload_len >> 8) & 0xFF); // 长度高字节

    // 时间戳: 当前时间 (小端序)
    uint32_t timestamp = HAL_GetTick();
    frame[index++] = (uint8_t)(timestamp & 0xFF);
    frame[index++] = (uint8_t)((timestamp >> 8) & 0xFF);
    frame[index++] = (uint8_t)((timestamp >> 16) & 0xFF);
    frame[index++] = (uint8_t)((timestamp >> 24) & 0xFF);

    // X轴加速度数据 (小端序)
    union {
        float32_t f;
        uint8_t bytes[4];
    } float_converter;

    float_converter.f = accel_x;
    frame[index++] = float_converter.bytes[0];
    frame[index++] = float_converter.bytes[1];
    frame[index++] = float_converter.bytes[2];
    frame[index++] = float_converter.bytes[3];

    // Y轴加速度数据 (小端序)
    float_converter.f = accel_y;
    frame[index++] = float_converter.bytes[0];
    frame[index++] = float_converter.bytes[1];
    frame[index++] = float_converter.bytes[2];
    frame[index++] = float_converter.bytes[3];

    // Z轴加速度数据 (小端序)
    float_converter.f = accel_z;
    frame[index++] = float_converter.bytes[0];
    frame[index++] = float_converter.bytes[1];
    frame[index++] = float_converter.bytes[2];
    frame[index++] = float_converter.bytes[3];

    // 计算校验和 (命令码 + 长度 + 载荷)
    uint8_t checksum = 0;
    for (uint16_t i = 2; i < index; i++) {  // 从命令码开始到载荷结束
        checksum ^= frame[i];
    }
    frame[index++] = checksum;

    // 帧尾: 0D
    frame[index++] = 0x0D;

    // 发送协议帧
    for (uint16_t i = 0; i < index; i++) {
        putchar(frame[i]);
    }
}

