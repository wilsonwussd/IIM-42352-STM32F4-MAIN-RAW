/*
 * ________________________________________________________________________________________________________
 * FFT Test Module Header
 * 
 * Test functions to verify FFT functionality
 * ________________________________________________________________________________________________________
 */

#ifndef _FFT_TEST_H_
#define _FFT_TEST_H_

#include <stdint.h>
#include "arm_math.h"

/**
 * @brief Run basic FFT functionality test
 * Tests FFT with known sine wave signals and verifies frequency detection
 */
void FFT_RunBasicTest(void);

/**
 * @brief Run FFT performance test
 * Measures FFT processing time
 */
void FFT_RunPerformanceTest(void);

/**
 * @brief Run all FFT tests
 * Executes all available FFT tests
 */
void FFT_RunAllTests(void);

#endif /* _FFT_TEST_H_ */
