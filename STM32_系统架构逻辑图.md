# STM32 智能挖掘检测系统架构逻辑图

## 🏗️ 系统总体架构图

```mermaid
graph TB
    subgraph "硬件层"
        IIM[IIM-42352传感器<br/>±4g, 1000Hz]
        STM32[STM32F407VGT6<br/>84MHz主频]
        LORA[LoRa模块<br/>无线通信]
        PC[上位机<br/>USB串口]
    end
    
    subgraph "STM32软件架构"
        SENSOR[传感器驱动层]
        FFT[FFT处理层]
        PROTOCOL[协议封装层]
        COMM[通信管理层]
        ALARM[报警控制层]
    end
    
    IIM -.SPI.-> STM32
    STM32 -.UART1.-> PC
    STM32 -.UART5.-> LORA
    
    SENSOR --> FFT
    FFT --> PROTOCOL
    PROTOCOL --> COMM
    COMM --> ALARM
```

## 📊 数据流架构图

```mermaid
flowchart LR
    subgraph "数据采集"
        A1[IIM-42352<br/>1000Hz采样]
        A2[SPI接口<br/>数据读取]
        A3[FIFO中断<br/>触发处理]
    end
    
    subgraph "信号处理"
        B1[原始数据<br/>16位ADC]
        B2[单位转换<br/>ADC→g值]
        B3[FFT处理<br/>512点变换]
        B4[频谱计算<br/>257点输出]
    end
    
    subgraph "协议封装"
        C1[帧头<br/>AA55]
        C2[命令码<br/>01/02/04]
        C3[载荷数据<br/>时间戳+数据]
        C4[校验和<br/>XOR校验]
        C5[帧尾<br/>0D]
    end
    
    subgraph "数据传输"
        D1[UART1<br/>调试串口]
        D2[UART5<br/>LoRa通信]
        D3[上位机<br/>数据接收]
        D4[LoRa网关<br/>云端上报]
    end
    
    A1 --> A2 --> A3
    A3 --> B1 --> B2 --> B3 --> B4
    B2 --> C1
    B4 --> C1
    C1 --> C2 --> C3 --> C4 --> C5
    C5 --> D1 --> D3
    C5 --> D2 --> D4
```

## 🔄 FFT处理模块架构图

```mermaid
graph TD
    subgraph "FFT处理器架构"
        A[传感器数据输入<br/>Z轴加速度]
        B[循环缓冲区<br/>512点时域数据]
        C{缓冲区满?}
        D[数据重排<br/>时序校正]
        E[汉宁窗函数<br/>频谱泄漏抑制]
        F[CMSIS DSP FFT<br/>512点复数变换]
        G[幅度谱计算<br/>复数→幅度]
        H[归一化处理<br/>双边谱→单边谱]
        I[物理量缩放<br/>输出真实g值]
        J[协议封装<br/>257点数据帧]
        K[UART传输<br/>2Hz更新频率]
    end
    
    A --> B
    B --> C
    C -->|否| B
    C -->|是| D
    D --> E
    E --> F
    F --> G
    G --> H
    H --> I
    I --> J
    J --> K
    
    style A fill:#e1f5fe
    style K fill:#c8e6c9
    style F fill:#fff3e0
```

## 🚨 LoRa报警状态机图

```mermaid
stateDiagram-v2
    [*] --> IDLE : 系统启动
    
    IDLE --> SET_1 : 收到0x10命令
    
    SET_1 --> WAIT_RESPONSE_1 : 发送Modbus命令<br/>寄存器=1
    
    WAIT_RESPONSE_1 --> HOLD : LoRa响应成功
    WAIT_RESPONSE_1 --> IDLE : 超时(5秒)
    
    HOLD --> SET_0 : 保持1秒完成
    
    SET_0 --> WAIT_RESPONSE_0 : 发送Modbus命令<br/>寄存器=0
    
    WAIT_RESPONSE_0 --> COMPLETE : LoRa响应成功
    WAIT_RESPONSE_0 --> IDLE : 超时(5秒)
    
    COMPLETE --> IDLE : 报警周期完成
    
    note right of SET_1
        Modbus命令格式:
        01 46 00 00 00 01 02 00 01 [CRC]
    end note
    
    note right of SET_0
        Modbus命令格式:
        01 46 00 00 00 01 02 00 00 [CRC]
    end note
```

## 📡 通信协议架构图

```mermaid
graph TB
    subgraph "协议栈架构"
        subgraph "应用层"
            APP1[频域数据<br/>257点FFT]
            APP2[原始数据<br/>三轴加速度]
            APP3[命令响应<br/>状态查询]
        end
        
        subgraph "协议层"
            PROTO[自定义协议<br/>AA55帧格式]
        end
        
        subgraph "传输层"
            UART1[UART1调试<br/>115200bps]
            UART5[UART5 LoRa<br/>115200bps]
        end
        
        subgraph "物理层"
            PHY1[USB转串口<br/>PC连接]
            PHY2[LoRa模块<br/>无线传输]
        end
    end
    
    APP1 --> PROTO
    APP2 --> PROTO
    APP3 --> PROTO
    
    PROTO --> UART1
    PROTO --> UART5
    
    UART1 --> PHY1
    UART5 --> PHY2
    
    subgraph "协议帧结构"
        FRAME[帧头<br/>AA55] --> CMD[命令<br/>01/02/04/10/11]
        CMD --> LEN[长度<br/>2字节小端]
        LEN --> DATA[载荷<br/>时间戳+数据]
        DATA --> CHK[校验<br/>XOR校验]
        CHK --> TAIL[帧尾<br/>0D]
    end
```

## ⚡ 中断处理架构图

```mermaid
graph TD
    subgraph "中断系统架构"
        A[传感器FIFO中断<br/>PC7引脚]
        B[UART1接收中断<br/>上位机命令]
        C[UART5接收中断<br/>LoRa响应]
        D[系统滴答中断<br/>时间基准]
        
        E[中断服务程序<br/>ISR]
        F[主循环处理<br/>Main Loop]
        
        subgraph "中断优先级"
            P1[优先级0<br/>传感器中断]
            P2[优先级1<br/>UART中断]
            P3[优先级2<br/>系统中断]
        end
    end
    
    A --> E
    B --> E
    C --> E
    D --> E
    
    E --> F
    
    A -.-> P1
    B -.-> P2
    C -.-> P2
    D -.-> P3
    
    style A fill:#ffcdd2
    style E fill:#fff3e0
    style F fill:#e8f5e8
```

## 🔧 系统初始化流程图

```mermaid
flowchart TD
    START([系统启动]) --> CLOCK[时钟配置<br/>84MHz PLL]
    CLOCK --> GPIO[GPIO初始化<br/>SPI/UART引脚]
    GPIO --> SPI[SPI1初始化<br/>传感器通信]
    SPI --> UART1[UART1初始化<br/>调试串口]
    UART1 --> UART5[UART5初始化<br/>LoRa通信]
    UART5 --> SENSOR[传感器初始化<br/>IIM-42352配置]
    SENSOR --> FFT[FFT处理器初始化<br/>缓冲区分配]
    FFT --> INT[中断使能<br/>FIFO/UART中断]
    INT --> LOOP[进入主循环<br/>事件驱动处理]
    
    subgraph "传感器配置详情"
        SC1[量程设置: ±4g]
        SC2[采样率: 1000Hz]
        SC3[低噪声模式]
        SC4[振动优化AAF]
        SC5[FIFO使能]
    end
    
    SENSOR -.-> SC1
    SC1 --> SC2 --> SC3 --> SC4 --> SC5
    
    style START fill:#c8e6c9
    style LOOP fill:#c8e6c9
    style SENSOR fill:#fff3e0
```

## 📈 性能监控架构图

```mermaid
graph LR
    subgraph "性能指标监控"
        subgraph "处理性能"
            A1[FFT处理时间<br/>~36μs]
            A2[数据采集频率<br/>1000Hz]
            A3[频域更新率<br/>2Hz]
            A4[原始数据率<br/>10Hz]
        end
        
        subgraph "通信性能"
            B1[UART1吞吐量<br/>115200bps]
            B2[UART5吞吐量<br/>115200bps]
            B3[协议效率<br/>>90%]
            B4[数据完整性<br/>XOR+CRC校验]
        end
        
        subgraph "系统性能"
            C1[CPU利用率<br/><30%]
            C2[内存使用<br/><50%]
            C3[功耗优化<br/>84MHz配置]
            C4[响应延迟<br/><100ms]
        end
    end
    
    A1 --> A2 --> A3 --> A4
    B1 --> B2 --> B3 --> B4
    C1 --> C2 --> C3 --> C4
    
    style A1 fill:#e3f2fd
    style B1 fill:#f3e5f5
    style C1 fill:#e8f5e8
```

## 🛡️ 错误处理架构图

```mermaid
graph TD
    subgraph "错误处理机制"
        A[错误检测]
        B{错误类型}
        
        C[传感器错误<br/>SPI通信失败]
        D[FFT处理错误<br/>数据异常]
        E[通信错误<br/>UART超时]
        F[LoRa错误<br/>响应超时]
        
        G[错误恢复<br/>重新初始化]
        H[错误记录<br/>日志输出]
        I[状态报告<br/>上位机通知]
        J[继续运行<br/>降级服务]
    end
    
    A --> B
    B --> C
    B --> D
    B --> E
    B --> F
    
    C --> G
    D --> G
    E --> H
    F --> H
    
    G --> I
    H --> I
    I --> J
    
    style A fill:#ffcdd2
    style G fill:#fff3e0
    style J fill:#c8e6c9
```

## 📊 内存管理架构图

```mermaid
graph TB
    subgraph "STM32F407内存布局"
        subgraph "Flash存储 (1MB)"
            F1[程序代码<br/>~200KB]
            F2[常量数据<br/>~50KB]
            F3[FFT系数<br/>~10KB]
            F4[预留空间<br/>~740KB]
        end
        
        subgraph "SRAM (192KB)"
            S1[系统栈<br/>~8KB]
            S2[全局变量<br/>~10KB]
            S3[FFT缓冲区<br/>~8KB]
            S4[通信缓冲区<br/>~4KB]
            S5[可用内存<br/>~162KB]
        end
        
        subgraph "外设寄存器"
            P1[GPIO寄存器]
            P2[UART寄存器]
            P3[SPI寄存器]
            P4[DMA寄存器]
        end
    end
    
    F1 --> S1
    F2 --> S2
    F3 --> S3
    
    style F1 fill:#e3f2fd
    style S3 fill:#fff3e0
    style P1 fill:#f3e5f5
```

---

**STM32智能挖掘检测系统架构逻辑图 v3.3 - 完整的系统设计可视化** 🚀
