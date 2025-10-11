/*
 * ________________________________________________________________________________________________________
 * FFT Processor Module Implementation
 * 
 * This module implements FFT processing for IIM423 accelerometer data using CMSIS DSP library.
 * ________________________________________________________________________________________________________
 */

#include "fft_processor.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

/* Include header with macro definitions */
#include "example-raw-data.h"  // For ENABLE_FINE_DETECTION macro and fine detection functions

/* External function declaration to avoid header conflicts */
extern uint32_t HAL_GetTick(void);

/* Mathematical constants */
#ifndef PI
#define PI 3.14159265358979323846f
#endif

/* Private Variables */
static fft_processor_t fft_processor;
static bool is_initialized = false;

/* Hanning Window Coefficients (precomputed for efficiency) */
static float32_t hanning_window[FFT_SIZE];
static bool window_computed = false;

/* Private Function Declarations */
static void compute_hanning_window(void);
static void apply_window(float32_t* data, uint32_t length);
static void find_dominant_frequency(const float32_t* magnitude_spectrum, uint32_t length, 
                                   float32_t* freq, float32_t* magnitude);
static float32_t calculate_total_energy(const float32_t* magnitude_spectrum, uint32_t length);

/* Public Function Implementations */

int FFT_Init(bool auto_process, bool window_enabled)
{
    // Clear the processor structure
    memset(&fft_processor, 0, sizeof(fft_processor_t));

    // Set configuration
    fft_processor.auto_process = auto_process;
    fft_processor.window_enabled = window_enabled;
    fft_processor.trigger_mode = false;  // Stage 3: Default to continuous mode
    fft_processor.is_triggered = false;  // Stage 3: Default to not triggered
    fft_processor.state = FFT_STATE_IDLE;
    
    // Precompute Hanning window if enabled
    if (window_enabled && !window_computed) {
        compute_hanning_window();
        window_computed = true;
    }
    
    // Initialize frequency bins in result structure (257 points: 0 to Nyquist)
    for (uint32_t i = 0; i < FFT_OUTPUT_POINTS; i++) {
        fft_processor.last_result.frequency_bins[i] = FFT_BinToFrequency(i);
    }
    
    is_initialized = true;
    
    // 移除初始化调试信息
    
    return 0;
}

int FFT_AddSample(float32_t sample)
{
    static uint32_t add_sample_call_count = 0;
    add_sample_call_count++;

    if (!is_initialized) {
        if (add_sample_call_count % 1000 == 0) {
            printf("FFT_ADD_SAMPLE_DEBUG: Not initialized! call_count=%lu\r\n", add_sample_call_count);
        }
        return -1; // Not initialized
    }

    // Skip adding samples if currently processing or completed (wait for reset)
    if (fft_processor.state == FFT_STATE_PROCESSING) {
        if (add_sample_call_count % 1000 == 0) {
            printf("FFT_ADD_SAMPLE_DEBUG: State is PROCESSING, skipping. call_count=%lu\r\n", add_sample_call_count);
        }
        return 0; // Ignore samples during processing
    }

    // Debug: Print state every 1000 calls
    if (add_sample_call_count % 1000 == 0) {
        printf("FFT_ADD_SAMPLE_DEBUG: call_count=%lu, sample_count=%lu, state=%d, trigger_mode=%d, is_triggered=%d\r\n",
               add_sample_call_count, fft_processor.sample_count, fft_processor.state,
               fft_processor.trigger_mode, fft_processor.is_triggered);
    }

    // 修改触发模式逻辑：
    // 在触发模式下，应该一直采集样本到循环缓冲区
    // 只是在未触发时不自动处理FFT
    // 这样当触发时，缓冲区已经有最近的数据了
    // Stage 3: Check if we should process samples based on trigger mode
    // if (fft_processor.trigger_mode && !fft_processor.is_triggered) {
    //     // In trigger mode but not triggered: skip sample collection to save power
    //     return 0;
    // }

    // Add sample to circular buffer
    fft_processor.time_buffer[fft_processor.buffer_index] = sample;
    fft_processor.buffer_index = (fft_processor.buffer_index + 1) % FFT_BUFFER_SIZE;

    // Only increment sample count if we haven't reached the buffer size yet
    if (fft_processor.sample_count < FFT_BUFFER_SIZE) {
        fft_processor.sample_count++;
    }

    // Update state based on buffer fill
    if (fft_processor.sample_count >= FFT_BUFFER_SIZE) {
        // 如果状态不是READY，则设置为READY
        if (fft_processor.state == FFT_STATE_IDLE || fft_processor.state == FFT_STATE_COLLECTING) {
            fft_processor.state = FFT_STATE_READY;
        }

        // 如果状态是READY，检查是否应该自动处理
        if (fft_processor.state == FFT_STATE_READY) {
            // Auto process if enabled and (not in trigger mode OR triggered)
            bool should_auto_process = fft_processor.auto_process && (!fft_processor.trigger_mode || fft_processor.is_triggered);

            // Debug: Print auto-process decision
            static uint32_t auto_process_check_count = 0;
            auto_process_check_count++;
            if (auto_process_check_count % 100 == 0) {
                printf("FFT_AUTO_PROCESS_CHECK: auto_process=%d, trigger_mode=%d, is_triggered=%d, should_process=%d, state=%d\r\n",
                       fft_processor.auto_process, fft_processor.trigger_mode, fft_processor.is_triggered, should_auto_process, fft_processor.state);
            }

            if (should_auto_process) {
                printf("FFT_AUTO_PROCESS: Calling FFT_Process()...\r\n");
                return FFT_Process();
            }
        }
    } else {
        fft_processor.state = FFT_STATE_COLLECTING;
    }

    return 0;
}

int FFT_ProcessBuffer(const float32_t* buffer, uint32_t buffer_size)
{
    printf("FFT_PROCESS_BUFFER: Called with buffer_size=%lu\r\n", buffer_size);

    if (!is_initialized) {
        printf("FFT_PROCESS_BUFFER: ERROR - FFT not initialized\r\n");
        return -1;
    }

    if (buffer == NULL) {
        printf("FFT_PROCESS_BUFFER: ERROR - NULL buffer pointer\r\n");
        return -2;
    }

    if (buffer_size != FFT_SIZE) {
        printf("FFT_PROCESS_BUFFER: ERROR - buffer_size=%lu, expected FFT_SIZE=%d\r\n",
               buffer_size, FFT_SIZE);
        return -3;
    }

    printf("FFT_PROCESS_BUFFER: Starting FFT processing from external buffer...\r\n");
    fft_processor.state = FFT_STATE_PROCESSING;

    // Debug: Calculate RMS of input data
    float32_t sum_squares = 0.0f;
    float32_t max_val = 0.0f;
    float32_t min_val = 0.0f;
    uint32_t zero_count = 0;
    uint32_t large_count = 0;  // 大于1g的样本数

    // Copy buffer to FFT input and calculate statistics
    for (uint32_t i = 0; i < FFT_SIZE; i++) {
        float32_t sample = buffer[i];
        fft_processor.fft_input[2*i] = sample; // Real part
        fft_processor.fft_input[2*i + 1] = 0.0f; // Imaginary part

        // Calculate statistics
        sum_squares += sample * sample;
        if (i == 0 || sample > max_val) max_val = sample;
        if (i == 0 || sample < min_val) min_val = sample;

        // 统计零值和大值
        if (sample == 0.0f) zero_count++;
        if (fabsf(sample) > 1.0f) large_count++;
    }

    float32_t input_rms = sqrtf(sum_squares / FFT_SIZE);
    printf("FFT_INPUT_STATS: RMS=%.6f, Max=%.6f, Min=%.6f, Range=%.6f\r\n",
           input_rms, max_val, min_val, max_val - min_val);
    printf("FFT_INPUT_QUALITY: Zero_samples=%lu, Large_samples=%lu (>1g)\r\n",
           zero_count, large_count);

    // 输出前10个和后10个样本用于验证数据连续性
    printf("FFT_INPUT_FIRST10: ");
    for (uint32_t i = 0; i < 10; i++) {
        printf("%.4f ", buffer[i]);
    }
    printf("\r\n");

    printf("FFT_INPUT_LAST10: ");
    for (uint32_t i = FFT_SIZE - 10; i < FFT_SIZE; i++) {
        printf("%.4f ", buffer[i]);
    }
    printf("\r\n");

    // Apply windowing if enabled
    if (fft_processor.window_enabled) {
        for (uint32_t i = 0; i < FFT_SIZE; i++) {
            fft_processor.fft_input[2*i] *= hanning_window[i];
        }
        printf("FFT_PROCESS_BUFFER: Applied Hanning window\r\n");
    }

    // Perform FFT using CMSIS DSP
    arm_cfft_f32(&arm_cfft_sR_f32_len512, fft_processor.fft_input, 0, 1);

    // Calculate magnitude spectrum
    arm_cmplx_mag_f32(fft_processor.fft_input, fft_processor.fft_output, FFT_SIZE);

    // Copy magnitude spectrum to result (257 points: 0 to Nyquist frequency)
    for (uint32_t i = 0; i < FFT_OUTPUT_POINTS; i++) {
        // 1. FFT归一化 (除以FFT_SIZE)
        float32_t normalized_magnitude = fft_processor.fft_output[i] / (float32_t)FFT_SIZE;

        // 2. 双边谱转单边谱 (除DC和Nyquist外乘以2)
        if (i > 0 && i < FFT_SIZE/2) {
            normalized_magnitude *= 2.0f;
        }

        // 3. 存储结果（不再进行额外缩放，保持原始幅度）
        fft_processor.last_result.magnitude_spectrum[i] = normalized_magnitude;
    }

    // Analyze results (pass full spectrum, function will skip DC internally)
    find_dominant_frequency(fft_processor.last_result.magnitude_spectrum, FFT_OUTPUT_POINTS,
                           &fft_processor.last_result.dominant_frequency,
                           &fft_processor.last_result.dominant_magnitude);

    // Calculate total energy
    fft_processor.last_result.total_energy = 0.0f;
    for (uint32_t i = 0; i < FFT_OUTPUT_POINTS; i++) {
        fft_processor.last_result.total_energy +=
            fft_processor.last_result.magnitude_spectrum[i] * fft_processor.last_result.magnitude_spectrum[i];
    }

    // Fill frequency bins
    for (uint32_t i = 0; i < FFT_OUTPUT_POINTS; i++) {
        fft_processor.last_result.frequency_bins[i] = FFT_BinToFrequency(i);
    }

    // Store metadata
    fft_processor.last_result.sample_count = FFT_SIZE;
    fft_processor.last_result.timestamp = HAL_GetTick();

    // Update state
    fft_processor.state = FFT_STATE_COMPLETE;

    printf("FFT_PROCESS_BUFFER: Complete! freq=%.2fHz mag=%.6f energy=%.6f\r\n",
           fft_processor.last_result.dominant_frequency,
           fft_processor.last_result.dominant_magnitude,
           fft_processor.last_result.total_energy);

    // 输出前20个频率bin的幅值（0-39 Hz，每bin 1.953Hz）
    printf("FFT_SPECTRUM_0-39Hz: ");
    for (uint32_t i = 0; i < 20; i++) {
        printf("%.6f ", fft_processor.last_result.magnitude_spectrum[i]);
    }
    printf("\r\n");

    // 输出频率bin对应的频率值
    printf("FFT_FREQ_BINS_0-39Hz: ");
    for (uint32_t i = 0; i < 20; i++) {
        printf("%.2f ", fft_processor.last_result.frequency_bins[i]);
    }
    printf("\r\n");

    return 0;
}

bool FFT_IsReady(void)
{
    return (fft_processor.state == FFT_STATE_READY || 
            fft_processor.state == FFT_STATE_COMPLETE);
}

int FFT_Process(void)
{
    printf("FFT_PROCESS_DEBUG: Called! sample_count=%lu, state=%d\r\n",
           fft_processor.sample_count, fft_processor.state);

    if (!is_initialized || fft_processor.sample_count < FFT_BUFFER_SIZE) {
        printf("FFT_PROCESS_DEBUG: Not ready! is_initialized=%d, sample_count=%lu, FFT_BUFFER_SIZE=%d\r\n",
               is_initialized, fft_processor.sample_count, FFT_BUFFER_SIZE);
        return -1; // Not ready
    }

    printf("FFT_PROCESS_DEBUG: Starting FFT processing...\r\n");
    fft_processor.state = FFT_STATE_PROCESSING;
    
    // Prepare FFT input buffer (copy from circular buffer in correct order)
    uint32_t start_index = fft_processor.buffer_index; // Oldest sample

    // Debug: Calculate RMS of FFT input data
    float32_t sum_squares = 0.0f;
    float32_t max_val = 0.0f;
    float32_t min_val = 0.0f;

    for (uint32_t i = 0; i < FFT_SIZE; i++) {
        uint32_t src_index = (start_index + i) % FFT_BUFFER_SIZE;
        float32_t sample = fft_processor.time_buffer[src_index];
        fft_processor.fft_input[2*i] = sample; // Real part
        fft_processor.fft_input[2*i + 1] = 0.0f; // Imaginary part (zero for real input)

        // Calculate statistics
        sum_squares += sample * sample;
        if (i == 0 || sample > max_val) max_val = sample;
        if (i == 0 || sample < min_val) min_val = sample;
    }

    float32_t input_rms = sqrtf(sum_squares / FFT_SIZE);
    printf("FFT_INPUT_DEBUG: RMS=%.6f, Max=%.6f, Min=%.6f, Range=%.6f\r\n",
           input_rms, max_val, min_val, max_val - min_val);
    
    // Apply windowing if enabled
    if (fft_processor.window_enabled) {
        for (uint32_t i = 0; i < FFT_SIZE; i++) {
            fft_processor.fft_input[2*i] *= hanning_window[i];
        }
    }
    
    // Perform FFT using CMSIS DSP
    arm_cfft_f32(&arm_cfft_sR_f32_len512, fft_processor.fft_input, 0, 1);
    
    // Calculate magnitude spectrum
    arm_cmplx_mag_f32(fft_processor.fft_input, fft_processor.fft_output, FFT_SIZE);

    // Copy magnitude spectrum to result (257 points: 0 to Nyquist frequency)
    // Apply normalization and scaling for proper vibration amplitude
    for (uint32_t i = 0; i < FFT_OUTPUT_POINTS; i++) {
        // 1. FFT归一化 (除以FFT_SIZE)
        float32_t normalized_magnitude = fft_processor.fft_output[i] / (float32_t)FFT_SIZE;

        // 2. 双边谱转单边谱 (除DC和Nyquist外乘以2)
        if (i > 0 && i < FFT_SIZE/2) {
            normalized_magnitude *= 2.0f;
        }

        // 3. 传感器特性缩放 - 根据IIM-42352特性调整
        // IIM-42352的ADC输出需要转换为实际的g值
        // 保持真实的物理量，让上位机处理显示缩放
        normalized_magnitude *= 0.001f;  // 真实物理缩放，输出真实的g值

        // 4. 存储结果 (此时单位为g)
        fft_processor.last_result.magnitude_spectrum[i] = normalized_magnitude;
    }

    // Analyze results (pass full spectrum, function will skip DC internally)
    find_dominant_frequency(fft_processor.last_result.magnitude_spectrum, FFT_OUTPUT_POINTS,
                           &fft_processor.last_result.dominant_frequency,
                           &fft_processor.last_result.dominant_magnitude);

    fft_processor.last_result.total_energy =
        calculate_total_energy(fft_processor.last_result.magnitude_spectrum, FFT_OUTPUT_POINTS);
    
    fft_processor.last_result.sample_count = FFT_SIZE; // Should always be FFT_SIZE for completed FFT
    fft_processor.last_result.timestamp = HAL_GetTick();

    fft_processor.state = FFT_STATE_COMPLETE;

    // FFT数据发送已删除 - 调试串口现在专用于调试信息输出
    // 输出FFT处理完成的调试信息
    printf("FFT_RESULT: freq=%.2fHz mag=%.6f energy=%.6f samples=%lu\r\n",
           fft_processor.last_result.dominant_frequency,
           fft_processor.last_result.dominant_magnitude,
           fft_processor.last_result.total_energy,
           fft_processor.last_result.sample_count);

#if ENABLE_FINE_DETECTION
    // 阶段4：细检测算法处理
    fine_detection_features_t fine_features;
    int fine_result = Fine_Detector_Process(fft_processor.last_result.magnitude_spectrum,
                                           FFT_OUTPUT_POINTS,
                                           fft_processor.last_result.dominant_frequency,
                                           &fine_features);

    if (fine_result == 0 && fine_features.is_valid) {
        Fine_Detector_PrintResults(&fine_features);
    } else {
        printf("FINE_DEBUG: result=%d valid=%d\r\n", fine_result, fine_features.is_valid);
    }
#endif

    // 输出前几个频率点的幅值，用于验证滤波效果
    printf("FFT_SPECTRUM: 0Hz=%.6f 5Hz=%.6f 10Hz=%.6f 25Hz=%.6f 50Hz=%.6f\r\n",
           fft_processor.last_result.magnitude_spectrum[0],   // 0Hz (DC)
           fft_processor.last_result.magnitude_spectrum[3],   // ~5Hz
           fft_processor.last_result.magnitude_spectrum[5],   // ~10Hz
           fft_processor.last_result.magnitude_spectrum[13],  // ~25Hz
           fft_processor.last_result.magnitude_spectrum[26]); // ~50Hz

    // Reset for next FFT cycle if auto processing is enabled
    if (fft_processor.auto_process) {
        fft_processor.sample_count = 0; // Reset sample count for next cycle
        fft_processor.buffer_index = 0; // Reset buffer index
        fft_processor.state = FFT_STATE_IDLE; // Ready for next data collection
    }

    return 0;
}

const fft_result_t* FFT_GetResults(void)
{
    if (fft_processor.state == FFT_STATE_COMPLETE) {
        return &fft_processor.last_result;
    }
    return NULL;
}

fft_state_t FFT_GetState(void)
{
    return fft_processor.state;
}

void FFT_Reset(void)
{
    fft_processor.buffer_index = 0;
    fft_processor.sample_count = 0;
    fft_processor.state = FFT_STATE_IDLE;
    memset(fft_processor.time_buffer, 0, sizeof(fft_processor.time_buffer));
}

uint8_t FFT_GetBufferFillPercentage(void)
{
    if (fft_processor.sample_count >= FFT_BUFFER_SIZE) {
        return 100;
    }
    return (uint8_t)((fft_processor.sample_count * 100) / FFT_BUFFER_SIZE);
}

float32_t FFT_BinToFrequency(uint32_t bin_index)
{
    return (float32_t)bin_index * FREQUENCY_RESOLUTION;
}

uint32_t FFT_FrequencyToBin(float32_t frequency)
{
    return (uint32_t)(frequency / FREQUENCY_RESOLUTION + 0.5f);
}

/* Private Function Implementations */

static void compute_hanning_window(void)
{
    for (uint32_t i = 0; i < FFT_SIZE; i++) {
        hanning_window[i] = 0.5f * (1.0f - cosf(2.0f * PI * i / (FFT_SIZE - 1)));
    }
}

static void apply_window(float32_t* data, uint32_t length)
{
    for (uint32_t i = 0; i < length; i++) {
        data[i] *= hanning_window[i];
    }
}

static void find_dominant_frequency(const float32_t* magnitude_spectrum, uint32_t length,
                                   float32_t* freq, float32_t* magnitude)
{
    uint32_t max_index = 0;
    float32_t max_value = 0.0f;

    // Skip DC component (index 0)
    for (uint32_t i = 1; i < length; i++) {
        if (magnitude_spectrum[i] > max_value) {
            max_value = magnitude_spectrum[i];
            max_index = i;
        }
    }

    *freq = FFT_BinToFrequency(max_index);
    *magnitude = max_value;

    printf("DOMINANT_FREQ_DEBUG: max_index=%lu (bin %lu), freq=%.2f Hz, mag=%.6f\r\n",
           max_index, max_index, *freq, *magnitude);
}

// Find multiple frequency peaks (similar to the reference chart)
static void find_frequency_peaks(const float32_t* magnitude_spectrum, uint32_t length)
{
    printf("\r\n=== Frequency Peaks Analysis ===\r\n");

    // Find peaks above threshold
    float32_t threshold = 0.0f;

    // Calculate average magnitude to set threshold
    for (uint32_t i = 1; i < length; i++) {
        threshold += magnitude_spectrum[i];
    }
    threshold = (threshold / (length - 1)) * 2.0f; // 2x average as threshold

    printf("Peak threshold: %.2f\r\n", threshold);
    printf("Significant peaks:\r\n");
    printf("Freq(Hz) : Magnitude\r\n");
    printf("-------------------\r\n");

    // Find local peaks
    for (uint32_t i = 2; i < length - 2; i++) {
        float32_t current = magnitude_spectrum[i];

        // Check if it's a local maximum and above threshold
        if (current > threshold &&
            current > magnitude_spectrum[i-1] &&
            current > magnitude_spectrum[i+1] &&
            current > magnitude_spectrum[i-2] &&
            current > magnitude_spectrum[i+2]) {

            float32_t freq = FFT_BinToFrequency(i);
            printf("%7.1f : %8.2f\r\n", freq, current);
        }
    }
    printf("===============================\r\n");
}

static float32_t calculate_total_energy(const float32_t* magnitude_spectrum, uint32_t length)
{
    float32_t total_energy = 0.0f;

    for (uint32_t i = 0; i < length; i++) {
        total_energy += magnitude_spectrum[i] * magnitude_spectrum[i];
    }

    return sqrtf(total_energy);
}

void FFT_PrintResults(bool detailed)
{
    if (fft_processor.state != FFT_STATE_COMPLETE) {
        printf("[FFT] No results available\r\n");
        return;
    }

    const fft_result_t* result = &fft_processor.last_result;

    printf("\r\n=== FFT Analysis Results ===\r\n");
    printf("Timestamp: %lu ms\r\n", result->timestamp);
    printf("Dominant Frequency: %.2f Hz\r\n", result->dominant_frequency);
    printf("Dominant Magnitude: %.2f\r\n", result->dominant_magnitude);
    printf("Total Energy: %.2f\r\n", result->total_energy);
    printf("FFT Points: %lu\r\n", result->sample_count);

    // Add frequency band analysis
    float32_t low_freq_energy = 0.0f;   // 0-50 Hz
    float32_t mid_freq_energy = 0.0f;   // 50-200 Hz
    float32_t high_freq_energy = 0.0f;  // 200-500 Hz

    for (uint32_t i = 1; i < FFT_OUTPUT_POINTS; i++) {
        float32_t freq = FFT_BinToFrequency(i);
        float32_t magnitude = result->magnitude_spectrum[i];

        if (freq < 50.0f) {
            low_freq_energy += magnitude * magnitude;
        } else if (freq < 200.0f) {
            mid_freq_energy += magnitude * magnitude;
        } else {
            high_freq_energy += magnitude * magnitude;
        }
    }

    printf("Low Freq (0-50Hz): %.2f\r\n", sqrtf(low_freq_energy));
    printf("Mid Freq (50-200Hz): %.2f\r\n", sqrtf(mid_freq_energy));
    printf("High Freq (200-500Hz): %.2f\r\n", sqrtf(high_freq_energy));

    // Vibration level assessment
    const char* vibration_level;
    if (result->total_energy < 100.0f) {
        vibration_level = "LOW";
    } else if (result->total_energy < 300.0f) {
        vibration_level = "MEDIUM";
    } else if (result->total_energy < 500.0f) {
        vibration_level = "HIGH";
    } else {
        vibration_level = "VERY HIGH";
    }

    printf("Vibration Level: %s\r\n", vibration_level);
    printf("============================\r\n");

    // Always show frequency peaks analysis (like the reference chart)
    find_frequency_peaks(result->magnitude_spectrum, FFT_OUTPUT_POINTS);

    if (detailed) {
        printf("\r\n=== Detailed Frequency Spectrum ===\r\n");
        printf("Freq(Hz) : Magnitude\r\n");
        printf("------------------------\r\n");

        // Print spectrum in ranges similar to the reference chart
        // 0-50Hz range (every 5Hz)
        for (uint32_t i = 0; i < FFT_OUTPUT_POINTS; i++) {
            float32_t freq = FFT_BinToFrequency(i);
            if (freq <= 50.0f && (i % 3 == 0)) { // Every ~6Hz in low range
                printf("%6.1f : %8.2f\r\n", freq, result->magnitude_spectrum[i]);
            }
        }

        printf("--- Mid Range (50-200Hz) ---\r\n");
        // 50-200Hz range (every 10Hz)
        for (uint32_t i = 0; i < FFT_OUTPUT_POINTS; i++) {
            float32_t freq = FFT_BinToFrequency(i);
            if (freq > 50.0f && freq <= 200.0f && (i % 5 == 0)) { // Every ~10Hz
                printf("%6.1f : %8.2f\r\n", freq, result->magnitude_spectrum[i]);
            }
        }

        printf("--- High Range (200-500Hz) ---\r\n");
        // 200-500Hz range (every 25Hz)
        for (uint32_t i = 0; i < FFT_OUTPUT_POINTS; i++) {
            float32_t freq = FFT_BinToFrequency(i);
            if (freq > 200.0f && (i % 13 == 0)) { // Every ~25Hz
                printf("%6.1f : %8.2f\r\n", freq, result->magnitude_spectrum[i]);
            }
        }
        printf("============================\r\n");
    }
}

void FFT_PrintSpectrumCSV(void)
{
    if (fft_processor.state != FFT_STATE_COMPLETE) {
        return;  // 静默返回，无调试信息
    }

    const fft_result_t* result = &fft_processor.last_result;

    // 对标图片格式：X轴每25Hz一个点，Y轴单位mg
    printf("SPECTRUM_START\r\n");

    // 输出对标图片格式的频域数据 (0-500Hz, 每25Hz一个点)
    for (uint32_t freq_hz = 0; freq_hz <= 500; freq_hz += 25) {
        // 找到最接近目标频率的FFT bin
        uint32_t bin_index = (uint32_t)((float32_t)freq_hz * FFT_SIZE / SAMPLING_FREQUENCY);
        if (bin_index >= FFT_OUTPUT_POINTS) {
            bin_index = FFT_OUTPUT_POINTS - 1;
        }

        // FFT输出是g单位，转换为mg单位显示
        float32_t magnitude_mg = result->magnitude_spectrum[bin_index] * 1000.0f;

        printf("%lu,%.1f\r\n", freq_hz, magnitude_mg);
    }

    printf("SPECTRUM_END\r\n");
}

/* --------------------------------------------------------------------------------------
 *  Stage 3: Intelligent FFT Control Functions
 * -------------------------------------------------------------------------------------- */

int FFT_SetTriggerMode(bool enable)
{
    if (!is_initialized) {
        return -1;
    }

    fft_processor.trigger_mode = enable;

    if (enable) {
        // Entering trigger mode: reset FFT state and clear buffer
        fft_processor.is_triggered = false;
        FFT_Reset();
    }

    return 0;
}

int FFT_SetTriggerState(bool triggered)
{
    if (!is_initialized) {
        return -1;
    }

    bool previous_state = fft_processor.is_triggered;
    fft_processor.is_triggered = triggered;

    // 注释掉重置逻辑：粗检测触发时不应该清空FFT缓冲区
    // 因为FFT缓冲区应该一直在采集数据，触发时应该使用已有的数据
    // If transitioning from not triggered to triggered, reset FFT for fresh data
    // if (!previous_state && triggered) {
    //     FFT_Reset();
    // }

    // 新逻辑：触发时打印调试信息
    if (!previous_state && triggered) {
        printf("FFT_TRIGGER: Triggered! sample_count=%lu, state=%d\r\n",
               fft_processor.sample_count, fft_processor.state);
    }

    return 0;
}

bool FFT_GetTriggerState(void)
{
    return fft_processor.is_triggered;
}

bool FFT_ShouldProcess(void)
{
    if (!is_initialized) {
        return false;
    }

    // If not in trigger mode, always process
    if (!fft_processor.trigger_mode) {
        return true;
    }

    // In trigger mode: only process if triggered
    return fft_processor.is_triggered;
}

/* FFT数据发送函数已删除 - 调试串口现在专用于调试信息输出 */

/* FFT完整频谱数据发送函数已删除 - 调试串口现在专用于调试信息输出 */

// Print spectrum in format similar to reference chart
void FFT_PrintSpectrumChart(void)
{
    if (fft_processor.state != FFT_STATE_COMPLETE) {
        return;
    }

    const fft_result_t* result = &fft_processor.last_result;

    printf("\r\n=== Frequency Spectrum Chart ===\r\n");
    printf("Reference: 0-500Hz, similar to your chart\r\n");
    printf("Freq(Hz) | Magnitude | Bar Chart\r\n");
    printf("---------|-----------|----------\r\n");

    // Print key frequency points similar to your reference chart
    uint32_t key_frequencies[] = {25, 50, 75, 100, 125, 150, 175, 200, 225, 250, 275, 300, 350, 400, 450, 500};
    uint32_t num_key_freq = sizeof(key_frequencies) / sizeof(key_frequencies[0]);

    for (uint32_t i = 0; i < num_key_freq; i++) {
        uint32_t target_freq = key_frequencies[i];
        uint32_t bin_index = FFT_FrequencyToBin((float32_t)target_freq);

        if (bin_index < FFT_OUTPUT_POINTS) {
            float32_t magnitude = result->magnitude_spectrum[bin_index];

            // Create simple bar chart (scale magnitude to 0-50 chars)
            uint32_t bar_length = (uint32_t)(magnitude * 50.0f / 200.0f); // Scale for display
            if (bar_length > 50) bar_length = 50;

            printf("%7d | %8.2f | ", target_freq, magnitude);
            for (uint32_t j = 0; j < bar_length; j++) {
                printf("#");  // Use # instead of special character
            }
            printf("\r\n");
        }
    }
    printf("================================\r\n");
}
