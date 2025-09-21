/*
 * ________________________________________________________________________________________________________
 * FFT Test Module
 * 
 * Simple test functions to verify FFT functionality
 * ________________________________________________________________________________________________________
 */

#include "fft_test.h"
#include "fft_processor.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

/* Test signal parameters */
#define TEST_FREQUENCY_1    50.0f   // Hz
#define TEST_FREQUENCY_2    120.0f  // Hz
#define TEST_AMPLITUDE_1    1.0f
#define TEST_AMPLITUDE_2    0.5f
#define TEST_SAMPLES        512

void FFT_RunBasicTest(void)
{
    printf("\r\n[FFT Test] Starting basic FFT functionality test...\r\n");
    
    // Initialize FFT processor
    int rc = FFT_Init(false, true); // Manual processing, windowing enabled
    if (rc != 0) {
        printf("[FFT Test] ERROR: Failed to initialize FFT processor\r\n");
        return;
    }
    
    printf("[FFT Test] FFT processor initialized successfully\r\n");
    
    // Generate test signal: combination of two sine waves
    printf("[FFT Test] Generating test signal (%.1f Hz + %.1f Hz)...\r\n", 
           TEST_FREQUENCY_1, TEST_FREQUENCY_2);
    
    for (uint32_t i = 0; i < TEST_SAMPLES; i++) {
        float32_t t = (float32_t)i / 1000.0f; // Time in seconds (1kHz sampling)
        
        // Generate composite signal
        float32_t signal = TEST_AMPLITUDE_1 * sinf(2.0f * 3.14159f * TEST_FREQUENCY_1 * t) +
                          TEST_AMPLITUDE_2 * sinf(2.0f * 3.14159f * TEST_FREQUENCY_2 * t);
        
        // Add some simple noise (using simple pseudo-random)
        static uint32_t noise_seed = 12345;
        noise_seed = noise_seed * 1103515245 + 12345; // Simple LCG
        float32_t noise = 0.1f * ((float32_t)(noise_seed % 1000) / 1000.0f - 0.5f);
        signal += noise;
        
        FFT_AddSample(signal);
        
        // Print progress
        if ((i + 1) % 100 == 0) {
            printf("[FFT Test] Added %lu samples, buffer fill: %d%%\r\n", 
                   i + 1, FFT_GetBufferFillPercentage());
        }
    }
    
    // Check if FFT is ready
    if (FFT_IsReady()) {
        printf("[FFT Test] FFT buffer ready, processing...\r\n");
        
        rc = FFT_Process();
        if (rc == 0) {
            printf("[FFT Test] FFT processing completed successfully\r\n");
            
            // Get and display results
            const fft_result_t* results = FFT_GetResults();
            if (results != NULL) {
                printf("\r\n[FFT Test Results]\r\n");
                printf("Dominant Frequency: %.2f Hz (Expected: %.1f Hz or %.1f Hz)\r\n", 
                       results->dominant_frequency, TEST_FREQUENCY_1, TEST_FREQUENCY_2);
                printf("Dominant Magnitude: %.2f\r\n", results->dominant_magnitude);
                printf("Total Energy: %.2f\r\n", results->total_energy);
                
                // Check if detected frequency is close to expected
                float32_t freq_error_1 = fabsf(results->dominant_frequency - TEST_FREQUENCY_1);
                float32_t freq_error_2 = fabsf(results->dominant_frequency - TEST_FREQUENCY_2);
                float32_t min_error = (freq_error_1 < freq_error_2) ? freq_error_1 : freq_error_2;
                
                if (min_error < 5.0f) { // Allow 5Hz tolerance
                    printf("[FFT Test] PASS: Detected frequency is within expected range\r\n");
                } else {
                    printf("[FFT Test] FAIL: Detected frequency error too large (%.2f Hz)\r\n", min_error);
                }
                
                // Print detailed spectrum for analysis
                FFT_PrintResults(true);
            } else {
                printf("[FFT Test] ERROR: Failed to get FFT results\r\n");
            }
        } else {
            printf("[FFT Test] ERROR: FFT processing failed\r\n");
        }
    } else {
        printf("[FFT Test] ERROR: FFT not ready after adding samples\r\n");
    }
    
    printf("[FFT Test] Test completed\r\n\r\n");
}

void FFT_RunPerformanceTest(void)
{
    printf("\r\n[FFT Performance Test] Starting...\r\n");
    
    // Initialize FFT processor
    int rc = FFT_Init(false, false); // Manual processing, no windowing for speed
    if (rc != 0) {
        printf("[FFT Performance Test] ERROR: Failed to initialize FFT processor\r\n");
        return;
    }
    
    // Fill buffer with test data
    for (uint32_t i = 0; i < TEST_SAMPLES; i++) {
        float32_t signal = sinf(2.0f * 3.14159f * 100.0f * i / 1000.0f); // 100Hz sine wave
        FFT_AddSample(signal);
    }
    
    // Measure processing time
    uint32_t start_time = HAL_GetTick();
    
    rc = FFT_Process();
    
    uint32_t end_time = HAL_GetTick();
    uint32_t processing_time = end_time - start_time;
    
    if (rc == 0) {
        printf("[FFT Performance Test] Processing time: %lu ms\r\n", processing_time);
        printf("[FFT Performance Test] PASS: FFT processing completed\r\n");
    } else {
        printf("[FFT Performance Test] FAIL: FFT processing failed\r\n");
    }
    
    printf("[FFT Performance Test] Test completed\r\n\r\n");
}

void FFT_RunAllTests(void)
{
    printf("\r\n========================================\r\n");
    printf("         FFT MODULE TESTS\r\n");
    printf("========================================\r\n");
    
    FFT_RunBasicTest();
    FFT_RunPerformanceTest();
    
    printf("========================================\r\n");
    printf("         ALL TESTS COMPLETED\r\n");
    printf("========================================\r\n\r\n");
}
