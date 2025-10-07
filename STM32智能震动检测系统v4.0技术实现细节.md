# STM32智能震动检测系统v4.0技术实现细节

## 🔬 **核心算法实现**

### **阶段1: 高通滤波器实现**

#### **滤波器设计参数**
```c
// 4阶Butterworth高通滤波器系数 (5Hz截止频率, 1000Hz采样率)
static const float32_t highpass_coeffs[10] = {
    // 第一个二阶节 (biquad 1)
    0.9691733f, -1.9383466f, 0.9691733f, 1.9371648f, -0.9395291f,
    // 第二个二阶节 (biquad 2)  
    1.0000000f, -2.0000000f, 1.0000000f, 1.9752696f, -0.9762448f
};
```

#### **Direct Form II实现**
```c
float32_t Highpass_Filter_Process(float32_t input) {
    // 第一个二阶节处理
    float32_t w1 = input - highpass_coeffs[3] * delay_z1[0] - highpass_coeffs[4] * delay_z1[1];
    float32_t y1 = highpass_coeffs[0] * w1 + highpass_coeffs[1] * delay_z1[0] + highpass_coeffs[2] * delay_z1[1];
    
    // 更新第一个二阶节的延迟线
    delay_z1[1] = delay_z1[0];
    delay_z1[0] = w1;
    
    // 第二个二阶节处理
    float32_t w2 = y1 - highpass_coeffs[8] * delay_z2[0] - highpass_coeffs[9] * delay_z2[1];
    float32_t output = highpass_coeffs[5] * w2 + highpass_coeffs[6] * delay_z2[0] + highpass_coeffs[7] * delay_z2[1];
    
    // 更新第二个二阶节的延迟线
    delay_z2[1] = delay_z2[0];
    delay_z2[0] = w2;
    
    return output;
}
```

### **阶段2: 粗检测算法实现**

#### **RMS滑动窗口计算**
```c
int Coarse_Detector_Process(float32_t sample) {
    // 添加新样本到滑动窗口
    coarse_detector.window_buffer[coarse_detector.window_index] = sample;
    coarse_detector.window_index = (coarse_detector.window_index + 1) % COARSE_WINDOW_SIZE;
    
    if (!coarse_detector.window_full && coarse_detector.window_index == 0) {
        coarse_detector.window_full = true;
    }
    
    if (coarse_detector.window_full) {
        // 计算RMS值
        float32_t sum_squares = 0.0f;
        for (int i = 0; i < COARSE_WINDOW_SIZE; i++) {
            float32_t val = coarse_detector.window_buffer[i];
            sum_squares += val * val;
        }
        float32_t rms = sqrtf(sum_squares / COARSE_WINDOW_SIZE);
        
        // 计算峰值因子
        float32_t peak_factor = rms / coarse_detector.baseline_rms;
        
        // 触发判断逻辑
        if (peak_factor > TRIGGER_MULTIPLIER) {
            if (coarse_detector.state == COARSE_STATE_IDLE) {
                coarse_detector.state = COARSE_STATE_TRIGGERED;
                coarse_detector.trigger_count++;
                return 1; // 触发检测
            }
        } else {
            if (coarse_detector.state == COARSE_STATE_TRIGGERED) {
                coarse_detector.state = COARSE_STATE_COOLDOWN;
            } else if (coarse_detector.state == COARSE_STATE_COOLDOWN) {
                coarse_detector.state = COARSE_STATE_IDLE;
            }
        }
    }
    
    return 0; // 未触发
}
```

### **阶段3: 智能FFT控制实现**

#### **触发模式控制**
```c
int FFT_AddSample(float32_t sample) {
    // 检查触发模式
    if (fft_processor.trigger_mode && !fft_processor.is_triggered) {
        // 触发模式下但未触发，跳过样本收集
        return 0;
    }
    
    // 添加样本到循环缓冲区
    fft_processor.time_buffer[fft_processor.buffer_index] = sample;
    fft_processor.buffer_index = (fft_processor.buffer_index + 1) % FFT_SIZE;
    
    if (!fft_processor.buffer_full && fft_processor.buffer_index == 0) {
        fft_processor.buffer_full = true;
    }
    
    // 检查是否可以处理FFT
    if (fft_processor.buffer_full && fft_processor.auto_process) {
        return FFT_Process();
    }
    
    return 0;
}

void FFT_SetTriggerState(bool triggered) {
    if (fft_processor.trigger_mode) {
        fft_processor.is_triggered = triggered;
        if (!triggered) {
            // 触发结束，重置缓冲区
            FFT_Reset();
        }
    }
}
```

### **阶段4: 细检测算法实现**

#### **5维特征提取**
```c
void Fine_Detection_Extract_Features(float32_t* fft_magnitude, fine_detection_features_t* features) {
    float32_t total_energy = 0.0f;
    float32_t low_energy = 0.0f;    // 5-15Hz
    float32_t mid_energy = 0.0f;    // 15-30Hz  
    float32_t high_energy = 0.0f;   // 30-100Hz
    float32_t weighted_freq_sum = 0.0f;
    float32_t max_magnitude = 0.0f;
    int max_index = 0;
    
    // 遍历频域数据计算各频段能量
    for (int i = 1; i < FFT_OUTPUT_SIZE; i++) {
        float32_t freq = (float32_t)i * FREQ_RESOLUTION;
        float32_t magnitude = fft_magnitude[i];
        float32_t energy = magnitude * magnitude;
        
        total_energy += energy;
        weighted_freq_sum += freq * energy;
        
        // 寻找主频
        if (magnitude > max_magnitude) {
            max_magnitude = magnitude;
            max_index = i;
        }
        
        // 分频段统计能量
        if (freq >= 5.0f && freq <= 15.0f) {
            low_energy += energy;
        } else if (freq >= 15.0f && freq <= 30.0f) {
            mid_energy += energy;
        } else if (freq >= 30.0f && freq <= 100.0f) {
            high_energy += energy;
        }
    }
    
    // 计算特征值
    if (total_energy > 0.0f) {
        features->low_freq_energy = low_energy / total_energy;
        features->mid_freq_energy = mid_energy / total_energy;
        features->high_freq_energy = high_energy / total_energy;
        features->spectral_centroid = weighted_freq_sum / total_energy;
    }
    
    features->dominant_frequency = (float32_t)max_index * FREQ_RESOLUTION;
}
```

#### **规则分类器**
```c
void Fine_Detection_Classify(fine_detection_features_t* features) {
    // 规则1: 低频能量占比 (挖掘震动特征)
    float32_t rule1_score = (features->low_freq_energy > 0.4f) ? 0.4f : 0.0f;
    
    // 规则2: 主频范围 (挖掘震动通常<50Hz)
    float32_t rule2_score = (features->dominant_frequency < 50.0f) ? 0.3f : 0.0f;
    
    // 规则3: 频谱重心 (挖掘震动能量集中在低频)
    float32_t rule3_score = (features->spectral_centroid < 80.0f) ? 0.2f : 0.0f;
    
    // 规则4: 中频能量适中 (机械传输特征)
    float32_t rule4_score = (features->mid_freq_energy > 0.1f && features->mid_freq_energy < 0.5f) ? 0.1f : 0.0f;
    
    // 计算总置信度
    features->confidence_score = rule1_score + rule2_score + rule3_score + rule4_score;
    
    // 分类决策
    if (features->confidence_score > 0.7f) {
        features->classification = FINE_DETECTION_MINING;
    } else {
        features->classification = FINE_DETECTION_NORMAL;
    }
    
    features->is_valid = true;
}
```

### **阶段5: 系统状态机实现**

#### **状态处理函数**
```c
static void handle_monitoring_state(void) {
    // 检查粗检测触发
    if (g_state_machine.coarse_trigger_flag) {
        transition_to_state(STATE_COARSE_TRIGGERED);
        g_state_machine.coarse_trigger_flag = 0;
        return;
    }
    
    // 检查长时间无活动
    uint32_t idle_time = HAL_GetTick() - g_state_machine.state_enter_time;
    if (idle_time > IDLE_TIMEOUT_MS) {
        transition_to_state(STATE_IDLE_SLEEP);
        return;
    }
}

static void handle_fine_analysis_state(void) {
    // 检查细检测结果
    if (g_state_machine.fine_analysis_result != 0) {
        if (g_state_machine.fine_analysis_result == 2) {
            // 挖掘震动检测
            transition_to_state(STATE_MINING_DETECTED);
            g_state_machine.mining_detections++;
        } else {
            // 正常震动
            transition_to_state(STATE_MONITORING);
        }
        g_state_machine.fine_analysis_result = 0;
        return;
    }
    
    // 检查超时
    uint32_t analysis_time = HAL_GetTick() - g_state_machine.state_enter_time;
    if (analysis_time > FINE_ANALYSIS_TIMEOUT_MS) {
        transition_to_state(STATE_MONITORING);
        return;
    }
}
```

#### **状态转换控制**
```c
static void transition_to_state(system_state_t new_state) {
    g_state_machine.previous_state = g_state_machine.current_state;
    g_state_machine.current_state = new_state;
    g_state_machine.state_enter_time = HAL_GetTick();
    g_state_machine.transition_count++;
    g_state_machine.state_count[new_state]++;
    
    // 状态转换日志
    printf("STATE_TRANSITION: %s -> %s (transition #%lu)\r\n",
           state_names[g_state_machine.previous_state],
           state_names[new_state],
           g_state_machine.transition_count);
}
```

## 🔧 **关键技术优化**

### **内存优化策略**
- **静态分配**: 避免动态内存分配，防止内存碎片
- **循环缓冲区**: 高效的数据缓存管理
- **内存对齐**: 32位边界对齐，提升访问效率
- **局部变量优化**: 减少栈使用，避免栈溢出

### **性能优化策略**
- **CMSIS DSP**: 使用ARM优化的DSP库函数
- **查表法**: 预计算常用数学函数
- **位运算**: 使用位运算替代除法和取模
- **编译器优化**: -O2优化级别，平衡性能和代码大小

### **功耗优化策略**
- **按需处理**: FFT仅在触发时激活
- **时钟管理**: 84MHz低功耗配置
- **休眠模式**: 长时间无活动时进入休眠
- **中断驱动**: 避免轮询，减少CPU占用

---

**STM32智能震动检测系统v4.0 - 核心算法与优化技术详解** 🔬
