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
    if (!is_initialized) {
        return -1; // Not initialized
    }

    // Skip adding samples if currently processing or completed (wait for reset)
    if (fft_processor.state == FFT_STATE_PROCESSING) {
        return 0; // Ignore samples during processing
    }

    // Add sample to circular buffer
    fft_processor.time_buffer[fft_processor.buffer_index] = sample;
    fft_processor.buffer_index = (fft_processor.buffer_index + 1) % FFT_BUFFER_SIZE;

    // Only increment sample count if we haven't reached the buffer size yet
    if (fft_processor.sample_count < FFT_BUFFER_SIZE) {
        fft_processor.sample_count++;
    }

    // Update state based on buffer fill
    if (fft_processor.sample_count >= FFT_BUFFER_SIZE) {
        if (fft_processor.state == FFT_STATE_IDLE || fft_processor.state == FFT_STATE_COLLECTING) {
            fft_processor.state = FFT_STATE_READY;

            // Auto process if enabled
            if (fft_processor.auto_process) {
                return FFT_Process();
            }
        }
    } else {
        fft_processor.state = FFT_STATE_COLLECTING;
    }

    return 0;
}

bool FFT_IsReady(void)
{
    return (fft_processor.state == FFT_STATE_READY || 
            fft_processor.state == FFT_STATE_COMPLETE);
}

int FFT_Process(void)
{
    if (!is_initialized || fft_processor.sample_count < FFT_BUFFER_SIZE) {
        return -1; // Not ready
    }
    
    fft_processor.state = FFT_STATE_PROCESSING;
    
    // Prepare FFT input buffer (copy from circular buffer in correct order)
    uint32_t start_index = fft_processor.buffer_index; // Oldest sample
    for (uint32_t i = 0; i < FFT_SIZE; i++) {
        uint32_t src_index = (start_index + i) % FFT_BUFFER_SIZE;
        fft_processor.fft_input[2*i] = fft_processor.time_buffer[src_index]; // Real part
        fft_processor.fft_input[2*i + 1] = 0.0f; // Imaginary part (zero for real input)
    }
    
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

    // Analyze results (exclude DC component for peak detection)
    find_dominant_frequency(&fft_processor.last_result.magnitude_spectrum[1], FFT_OUTPUT_POINTS-1,
                           &fft_processor.last_result.dominant_frequency,
                           &fft_processor.last_result.dominant_magnitude);

    // Adjust dominant frequency to account for skipped DC component
    fft_processor.last_result.dominant_frequency += FREQUENCY_RESOLUTION;

    fft_processor.last_result.total_energy =
        calculate_total_energy(fft_processor.last_result.magnitude_spectrum, FFT_OUTPUT_POINTS);
    
    fft_processor.last_result.sample_count = FFT_SIZE; // Should always be FFT_SIZE for completed FFT
    fft_processor.last_result.timestamp = HAL_GetTick();

    fft_processor.state = FFT_STATE_COMPLETE;

    // 发送高分辨率频域数据 (257点)
    FFT_SendFullSpectrumViaProtocol();

    // 可选：同时发送兼容的21点数据
    // FFT_SendSpectrumViaProtocol();

    // 同时输出CSV格式（可选）
    // FFT_PrintSpectrumCSV();

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

void FFT_SendSpectrumViaProtocol(void)
{
    if (fft_processor.state != FFT_STATE_COMPLETE) {
        return;  // 静默返回，无调试信息
    }

    const fft_result_t* result = &fft_processor.last_result;

    // 构建协议帧数据
    uint8_t frame[95];  // 完整帧长度
    uint16_t index = 0;

    // 帧头: AA 55
    frame[index++] = 0xAA;
    frame[index++] = 0x55;

    // 命令码: 01 (频域数据)
    frame[index++] = 0x01;

    // 长度: 58 00 (88字节载荷，小端序)
    frame[index++] = 0x58;
    frame[index++] = 0x00;

    // 时间戳: 当前时间 (小端序)
    uint32_t timestamp = HAL_GetTick();
    frame[index++] = (uint8_t)(timestamp & 0xFF);
    frame[index++] = (uint8_t)((timestamp >> 8) & 0xFF);
    frame[index++] = (uint8_t)((timestamp >> 16) & 0xFF);
    frame[index++] = (uint8_t)((timestamp >> 24) & 0xFF);

    // 21个频点的真实频域数据
    for (uint32_t i = 0; i < 21; i++) {
        uint32_t freq_hz = i * 25;  // 0, 25, 50, ..., 500Hz

        // 找到最接近目标频率的FFT bin
        uint32_t bin_index = (uint32_t)((float32_t)freq_hz * FFT_SIZE / SAMPLING_FREQUENCY);
        if (bin_index >= FFT_OUTPUT_POINTS) {
            bin_index = FFT_OUTPUT_POINTS - 1;
        }

        // FFT输出是g单位，转换为mg单位显示
        float32_t magnitude_mg = result->magnitude_spectrum[bin_index] * 1000.0f;

        // 将float32转换为字节 (小端序)
        union {
            float32_t f;
            uint8_t bytes[4];
        } float_converter;

        float_converter.f = magnitude_mg;

        frame[index++] = float_converter.bytes[0];
        frame[index++] = float_converter.bytes[1];
        frame[index++] = float_converter.bytes[2];
        frame[index++] = float_converter.bytes[3];
    }

    // 计算校验和 (命令码 + 长度 + 载荷)
    uint8_t checksum = 0;
    for (int i = 2; i < index; i++) {  // 从命令码开始到载荷结束
        checksum ^= frame[i];
    }
    frame[index++] = checksum;

    // 帧尾: 0D
    frame[index++] = 0x0D;

    // 发送协议帧 (不发送文本标识，纯二进制)
    for (int i = 0; i < index; i++) {
        putchar(frame[i]);
    }
}

void FFT_SendFullSpectrumViaProtocol(void)
{
    if (fft_processor.state != FFT_STATE_COMPLETE) {
        return;  // 静默返回，无调试信息
    }

    const fft_result_t* result = &fft_processor.last_result;

    // 计算帧长度: 帧头(2) + 命令(1) + 长度(2) + 时间戳(4) + 257个float32(1028) + 校验(1) + 帧尾(1) = 1039字节
    static uint8_t frame[1039];  // 使用静态缓冲区避免栈溢出
    uint16_t index = 0;

    // 帧头: AA 55
    frame[index++] = 0xAA;
    frame[index++] = 0x55;

    // 命令码: 04 (高分辨率频域数据)
    frame[index++] = 0x04;

    // 载荷长度: 1032字节 (4字节时间戳 + 257*4字节float32)
    uint16_t payload_len = 4 + FFT_OUTPUT_POINTS * 4;
    frame[index++] = (uint8_t)(payload_len & 0xFF);        // 长度低字节
    frame[index++] = (uint8_t)((payload_len >> 8) & 0xFF); // 长度高字节

    // 时间戳: 当前时间 (小端序)
    uint32_t timestamp = HAL_GetTick();
    frame[index++] = (uint8_t)(timestamp & 0xFF);
    frame[index++] = (uint8_t)((timestamp >> 8) & 0xFF);
    frame[index++] = (uint8_t)((timestamp >> 16) & 0xFF);
    frame[index++] = (uint8_t)((timestamp >> 24) & 0xFF);

    // 257个频点的完整频域数据 (0到500Hz)
    for (uint32_t i = 0; i < FFT_OUTPUT_POINTS; i++) {
        // FFT输出是g单位，转换为mg单位
        float32_t magnitude_mg = result->magnitude_spectrum[i] * 1000.0f;

        // 将float32转换为字节 (小端序)
        union {
            float32_t f;
            uint8_t bytes[4];
        } float_converter;

        float_converter.f = magnitude_mg;

        frame[index++] = float_converter.bytes[0];
        frame[index++] = float_converter.bytes[1];
        frame[index++] = float_converter.bytes[2];
        frame[index++] = float_converter.bytes[3];
    }

    // 计算校验和 (命令码 + 长度 + 载荷)
    uint8_t checksum = 0;
    for (uint16_t i = 2; i < index; i++) {  // 从命令码开始到载荷结束
        checksum ^= frame[i];
    }
    frame[index++] = checksum;

    // 帧尾: 0D
    frame[index++] = 0x0D;

    // 分批发送协议帧，避免UART缓冲区溢出
    const uint16_t chunk_size = 64;  // 每次发送64字节
    for (uint16_t i = 0; i < index; i += chunk_size) {
        uint16_t remaining = index - i;
        uint16_t send_size = (remaining > chunk_size) ? chunk_size : remaining;

        for (uint16_t j = 0; j < send_size; j++) {
            putchar(frame[i + j]);
        }

        // 短暂延迟，确保UART传输完成
        for (volatile int delay = 0; delay < 1000; delay++);
    }
}

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
