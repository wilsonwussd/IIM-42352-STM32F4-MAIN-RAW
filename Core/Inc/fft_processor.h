/*
 * ________________________________________________________________________________________________________
 * FFT Processor Module for IIM423 Sensor Data
 * 
 * This module provides FFT processing capabilities for accelerometer data from IIM423 sensor.
 * It includes circular buffer for time-domain data collection and FFT computation using CMSIS DSP library.
 * ________________________________________________________________________________________________________
 */

#ifndef _FFT_PROCESSOR_H_
#define _FFT_PROCESSOR_H_

#include <stdint.h>
#include <stdbool.h>

/* Define ARM_MATH_CM4 for STM32F4 series */
#ifndef ARM_MATH_CM4
#define ARM_MATH_CM4
#endif

#include "arm_math.h"
#include "arm_const_structs.h"

/* FFT Configuration Parameters */
#define FFT_SIZE                512     // FFT length (must be power of 2)
#define FFT_BUFFER_SIZE         FFT_SIZE
#define SAMPLING_FREQUENCY      1000.0f // Hz - IIM423 sampling rate
#define FREQUENCY_RESOLUTION    (SAMPLING_FREQUENCY / FFT_SIZE) // ~1.95 Hz
#define MAX_FREQUENCY           (SAMPLING_FREQUENCY / 2.0f)     // 500 Hz (Nyquist)
#define FFT_OUTPUT_POINTS       (FFT_SIZE/2 + 1)  // 257 points (0 to Nyquist)

/* FFT Processing States */
typedef enum {
    FFT_STATE_IDLE = 0,
    FFT_STATE_COLLECTING,
    FFT_STATE_READY,
    FFT_STATE_PROCESSING,
    FFT_STATE_COMPLETE
} fft_state_t;

/* FFT Result Structure */
typedef struct {
    float32_t dominant_frequency;    // Hz
    float32_t dominant_magnitude;    // Magnitude of dominant frequency
    float32_t total_energy;          // Total signal energy
    float32_t frequency_bins[FFT_OUTPUT_POINTS]; // Frequency bins (Hz) - 257 points
    float32_t magnitude_spectrum[FFT_OUTPUT_POINTS]; // Magnitude spectrum - 257 points
    uint32_t sample_count;           // Number of samples processed
    uint32_t timestamp;              // Processing timestamp
} fft_result_t;

/* FFT Processor Structure */
typedef struct {
    float32_t time_buffer[FFT_BUFFER_SIZE];     // Time domain circular buffer
    float32_t fft_input[FFT_SIZE * 2];          // FFT input buffer (complex: real, imag, real, imag...)
    float32_t fft_output[FFT_SIZE];             // FFT magnitude output

    uint32_t buffer_index;                      // Current buffer write index
    uint32_t sample_count;                      // Total samples collected
    fft_state_t state;                          // Current processing state

    bool auto_process;                          // Auto process when buffer full
    bool window_enabled;                        // Apply windowing function
    bool trigger_mode;                          // Trigger-based processing mode (Stage 3)
    bool is_triggered;                          // Current trigger state

    fft_result_t last_result;                   // Last FFT computation result
} fft_processor_t;

/* Public Function Declarations */

/**
 * @brief Initialize the FFT processor
 * @param auto_process: Enable automatic FFT processing when buffer is full
 * @param window_enabled: Enable Hanning window application
 * @return 0 on success, negative on error
 */
int FFT_Init(bool auto_process, bool window_enabled);

/**
 * @brief Add a new sample to the FFT buffer
 * @param sample: New accelerometer sample (float32_t)
 * @return 0 on success, negative on error
 */
int FFT_AddSample(float32_t sample);

/**
 * @brief Check if FFT is ready for processing
 * @return true if ready, false otherwise
 */
bool FFT_IsReady(void);

/**
 * @brief Process FFT computation
 * @return 0 on success, negative on error
 */
int FFT_Process(void);

/**
 * @brief Process FFT computation from external buffer (新架构)
 *
 * This function processes FFT directly from an external buffer (e.g., coarse detector buffer)
 * without using the internal circular buffer. This ensures data synchronization between
 * coarse detection and FFT processing.
 *
 * @param buffer: Pointer to external buffer containing time-domain samples
 * @param buffer_size: Size of the buffer (must be FFT_SIZE = 512)
 * @return 0 on success, negative on error
 */
int FFT_ProcessBuffer(const float32_t* buffer, uint32_t buffer_size);

/**
 * @brief Get the last FFT computation results
 * @return Pointer to fft_result_t structure
 */
const fft_result_t* FFT_GetResults(void);

/**
 * @brief Get current FFT processor state
 * @return Current fft_state_t
 */
fft_state_t FFT_GetState(void);

/**
 * @brief Reset the FFT processor
 */
void FFT_Reset(void);

/**
 * @brief Get buffer fill percentage
 * @return Fill percentage (0-100)
 */
uint8_t FFT_GetBufferFillPercentage(void);

/**
 * @brief Print FFT results to UART
 * @param detailed: Print detailed frequency spectrum if true
 */
void FFT_PrintResults(bool detailed);

/**
 * @brief Print frequency spectrum in CSV format for plotting
 */
void FFT_PrintSpectrumCSV(void);

/* FFT数据发送函数已删除 - 调试串口现在专用于调试信息输出 */

/**
 * @brief Print frequency spectrum chart similar to reference image
 */
void FFT_PrintSpectrumChart(void);

/**
 * @brief Convert frequency bin index to frequency in Hz
 * @param bin_index: Frequency bin index (0 to FFT_SIZE/2-1)
 * @return Frequency in Hz
 */
float32_t FFT_BinToFrequency(uint32_t bin_index);

/**
 * @brief Find frequency bin index for given frequency
 * @param frequency: Target frequency in Hz
 * @return Bin index
 */
uint32_t FFT_FrequencyToBin(float32_t frequency);

/* Stage 3: Intelligent FFT Control Functions */

/**
 * @brief Enable trigger-based FFT processing mode
 * @param enable: true to enable trigger mode, false for continuous mode
 * @return 0 on success, negative on error
 */
int FFT_SetTriggerMode(bool enable);

/**
 * @brief Set FFT trigger state (called by coarse detector)
 * @param triggered: true when vibration detected, false when idle
 * @return 0 on success, negative on error
 */
int FFT_SetTriggerState(bool triggered);

/**
 * @brief Get current FFT trigger state
 * @return true if triggered, false if idle
 */
bool FFT_GetTriggerState(void);

/**
 * @brief Check if FFT should process samples based on trigger state
 * @return true if should process, false if should skip
 */
bool FFT_ShouldProcess(void);

#endif /* _FFT_PROCESSOR_H_ */
