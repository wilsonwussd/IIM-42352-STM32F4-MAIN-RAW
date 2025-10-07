/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "clock_config_84mhz.h"  // 84MHz时钟配置验证
#include <stdio.h>
#include <string.h>  // For strlen
#include "example-raw-data.h"
/* InvenSense utils */
#include "Message.h"
#include "ErrorHelper.h"
/* FFT Processing */
#include "fft_processor.h"
#include "fft_test.h"
// #include "simple_protocol_test.h"  // 直接在main.c中实现

#include "arm_math.h"

/*
 * Select UART port on which INV_MSG() will be printed.
 */
#define LOG_UART_ID INV_UART_SENSOR_CTRL

/* 
 * Set of timers used throughout standalone applications 
 */
#define TIMEBASE_TIMER INV_TIMER1
#define DELAY_TIMER    INV_TIMER2


/* 
 * Select communication link between SmartMotion and ICM4235x 
 */
#define SERIF_TYPE IIM423XX_UI_SPI4
//#define SERIF_TYPE IIM423XX_UI_I2C

/* 
 * Define msg level 
 */
#define MSG_LEVEL INV_MSG_LEVEL_DEBUG

/*
 * Set power mode flag
 * Set this flag to run example in low-noise mode.
 * Reset this flag to run example in low-power mode.
 * Note : low-noise mode is not available with sensor data frequencies less than 12.5Hz.
 */
#define IS_LOW_NOISE_MODE 1

/* 
 * Set this to 0 if you want to test timestamping mechanism without CLKIN 32k capability.
 * Please set a hardware bridge between PA17 (from MCU) and CLKIN pins (to ICM).
 */
#define USE_CLK_IN 0

/*
 * Select Fifo resolution Mode (default is low resolution mode)
 * Low resolution mode : 16 bits data format
 * High resolution mode : 20 bits data format
 * Warning: Enabling High Res mode will force FSR to 16g and 2000dps
 */
#define IS_HIGH_RES_MODE 0

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart5;

/* USER CODE BEGIN PV */
/* LoRa Communication Variables */
uint8_t lora_rx_buffer[64];
uint8_t lora_tx_buffer[64];
volatile uint8_t lora_rx_complete = 0;
volatile uint8_t lora_tx_complete = 1;
uint8_t lora_rx_index = 0;

/* Upper Computer Communication Variables */
uint8_t uart1_rx_buffer[64];
volatile uint8_t uart1_rx_complete = 0;
uint8_t uart1_rx_index = 0;

/* Alarm Signal State */
typedef enum {
    ALARM_STATE_IDLE = 0,
    ALARM_STATE_SET_1,
    ALARM_STATE_WAIT_RESPONSE_1,
    ALARM_STATE_HOLD,
    ALARM_STATE_SET_0,
    ALARM_STATE_WAIT_RESPONSE_0,
    ALARM_STATE_COMPLETE
} alarm_state_t;

volatile alarm_state_t alarm_state = ALARM_STATE_IDLE;
uint32_t alarm_hold_start_time = 0;
uint32_t lora_timeout_start_time = 0;

#define ALARM_HOLD_TIME_MS      1000    // 1 second hold time
#define LORA_TIMEOUT_MS         5000    // 5 second timeout for LoRa response
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_UART5_Init(void);
static void check_rc(int rc, const char * msg_context);
void msg_printer(int level, const char * str, va_list ap);

static void SetupMCUHardware(struct inv_iim423xx_serif * icm_serif);

/* USER CODE BEGIN PFP */
void inv_iim423xx_sleep_ms(uint32_t ms);
void inv_iim423xx_sleep_us(uint32_t us);
void Simple_Protocol_Test(void);  // 协议测试函数声明

/* LoRa Communication Functions */
uint16_t Calculate_CRC16_Modbus(uint8_t *data, uint16_t length);
void Build_Modbus_Command(uint8_t value, uint8_t *command_buffer);
void LoRa_Send_Command(uint8_t *command, uint8_t length);
void Process_Alarm_State_Machine(void);
void Trigger_Alarm_Cycle(void);

/* Upper Computer Communication Functions */
void Process_UART1_Command(void);
void Send_Response_To_PC(const char *message);
void Start_UART1_Reception(void);
void Start_LoRa_Reception(void);
/* USER CODE END PFP */
//low-levevl IO access function
int inv_io_hal_read_reg(struct inv_iim423xx_serif * serif, uint8_t reg, uint8_t * rbuffer, uint32_t rlen);
int inv_io_hal_write_reg(struct inv_iim423xx_serif * serif, uint8_t reg, const uint8_t * wbuffer, uint32_t wlen);

/* Flag set from iim423xx device irq handler */
static volatile int irq_from_device;


#define TO_MASK(a) (1U << (unsigned)(a))
enum gpio_inv_pin_num {
	INV_GPIO_INT1 = 0,         /* Connected to the INT1 pin of the Invensense chip. */
	INV_GPIO_INT2,             /* Connected to the INT2 pin of the Invensense chip. */
	INV_GPIO_FSYNC,            /* Connected to the FSYNC pin of the Invensense chip. */
	INV_GPIO_3RD_PARTY_INT1,   /* Connected to the interrupt pin of the 3rd party chip. */
	INV_GPIO_SW0_BUTTON,       /* Connected to the SW0 button. */
	INV_GPIO_CLKIN,            /* Output SLCK on PA17 pin. Set a hardawre bridge to connect to CLKIN pin of the Invensense chip. */
	INV_GPIO_AD0,              /* Connected to the AD0 pin of the Invensense chip. */
	INV_GPIO_MAX
};

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// 简化的printf重定向 - 支持Keil和GCC
int fputc(int ch, FILE *f)
{
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 1000);
  return ch;
}

int __io_putchar(int ch)
{
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 1000);
  return ch;
}

// 系统调用重定向
int _write(int file, char *ptr, int len)
{
  HAL_UART_Transmit(&huart1, (uint8_t *)ptr, len, 1000);
  return len;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	int rc = 0;
	struct inv_iim423xx_serif iim423xx_serif;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

//  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_UART5_Init();
//  /* USER CODE BEGIN 2 */

  // 强制延迟确保UART完全初始化
  HAL_Delay(100);

  // 多重UART测试
  const char test_msg1[] = "\r\n\r\n=== SYSTEM BOOT START ===\r\n";
  HAL_UART_Transmit(&huart1, (uint8_t*)test_msg1, strlen(test_msg1), 1000);
  HAL_Delay(50);

  const char test_msg2[] = "UART1 Direct HAL transmission OK\r\n";
  HAL_UART_Transmit(&huart1, (uint8_t*)test_msg2, strlen(test_msg2), 1000);
  HAL_Delay(50);

  // 测试printf
  printf("Printf redirection test OK\r\n");
  HAL_Delay(50);

  printf("=== IIM42352 Intelligent Detection System ===\r\n");
  printf("Version: v4.0-dev (Stage 5 - System State Machine)\r\n");
  HAL_Delay(50);
  printf("Version: v4.0-dev (Stage 1 Verification)\n");
  printf("Bolgen Studio\n");


#if ENABLE_INTELLIGENT_DETECTION
  printf("INTELLIGENT DETECTION: ENABLED\n");
#if ENABLE_DATA_PREPROCESSING
  printf("DATA PREPROCESSING: ENABLED (5Hz Highpass Filter)\n");
#else
  printf("DATA PREPROCESSING: DISABLED\n");
#endif
#else
  printf("INTELLIGENT DETECTION: DISABLED (Legacy Mode)\n");
#endif

  /* 84MHz时钟配置验证和性能测试 */
  Full_84MHz_Test();

	   HAL_Delay(1000);

  /* Initialize UART reception */
  Start_UART1_Reception();
  Start_LoRa_Reception();

  printf("LoRa Communication System Initialized\n");
  printf("Waiting for commands from upper computer...\n");

		/* Initialize MCU hardware */
	SetupMCUHardware(&iim423xx_serif);

  /* USER CODE END 2 */
	/* Initialize Iim423xx */
	rc = SetupInvDevice(&iim423xx_serif);
	check_rc(rc, "error while setting up INV device");

		/* Configure Iim423xx */

	rc = ConfigureInvDevice((uint8_t )IS_LOW_NOISE_MODE,
				(uint8_t )IS_HIGH_RES_MODE,
				IIM423XX_ACCEL_CONFIG0_FS_SEL_4g,
				IIM423XX_ACCEL_CONFIG0_ODR_1_KHZ,
				(uint8_t )USE_CLK_IN);
	check_rc(rc, "error while configuring INV device");

	/* Initialize FFT Processor */
	rc = FFT_Init(true, true); // Auto process enabled, windowing enabled
	if (rc != 0) {
		while(1);  // 初始化失败，停止运行
	}

#if ENABLE_COARSE_DETECTION
	/* 启用FFT触发模式 (阶段3) */
	rc = FFT_SetTriggerMode(true);
	if(rc != 0) {
		printf("!!! ERROR : failed to enable FFT trigger mode.\r\n");
		while(1);  // 触发模式启用失败，停止运行
	}
	printf("=== FFT TRIGGER MODE ENABLED ===\r\n");
#endif

#if ENABLE_FINE_DETECTION
	/* 初始化细检测算法 (阶段4) */
	rc = Fine_Detector_Init();
	if(rc != 0) {
		printf("!!! ERROR : failed to initialize fine detector.\r\n");
		while(1);  // 细检测初始化失败，停止运行
	}
	printf("=== FINE DETECTION ENABLED ===\r\n");
#endif

#if ENABLE_SYSTEM_STATE_MACHINE
	/* 初始化系统状态机 (阶段5) */
	rc = System_State_Machine_Init();
	if(rc != 0) {
		printf("!!! ERROR : failed to initialize system state machine.\r\n");
		while(1);  // 状态机初始化失败，停止运行
	}
	printf("=== SYSTEM STATE MACHINE ENABLED ===\r\n");
#endif

	/* Skip FFT Tests for production */
	// FFT_RunAllTests();

  /* Infinite loop - 开始Z轴频域数据采集 */

	do {
		/* Poll device for data */
		if (irq_from_device & TO_MASK(INV_GPIO_INT1)) {
			rc = GetDataFromInvDevice();
			check_rc(rc, "error while processing FIFO");

			// inv_disable_irq();
			irq_from_device &= ~TO_MASK(INV_GPIO_INT1);
			// inv_enable_irq();
		}

		/* FFT自动处理模式，无需手动检查 */
		// FFT处理和数据输出现在是自动的

		/* Process upper computer commands */
		if (uart1_rx_complete) {
			Process_UART1_Command();
			uart1_rx_complete = 0;
			Start_UART1_Reception();  // Restart reception
		}

		// 直接读取UART1数据寄存器 - 处理二进制命令
		if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE)) {
			// 直接从数据寄存器读取
			uint8_t received_char = (uint8_t)(huart1.Instance->DR & 0xFF);

			// 处理二进制命令（单字节命令）
			if (uart1_rx_index == 0) {
				// 第一个字节，检查是否是有效命令
				if (received_char == 0x10 || received_char == 0x11 ||
				    received_char == 0x02 || received_char == 0x04) {
					uart1_rx_buffer[uart1_rx_index] = received_char;
					uart1_rx_index++;
					uart1_rx_complete = 1;  // 单字节命令立即完成
				}
			} else {
				// 缓冲区溢出保护，重置
				uart1_rx_index = 0;
			}
		}

		/* Process alarm state machine */
		Process_Alarm_State_Machine();

#if ENABLE_SYSTEM_STATE_MACHINE
		/* Process system state machine (阶段5) */
		System_State_Machine_Process();
#endif

		/* 协议测试 - 每10秒发送一次测试数据 (可选) */
		static uint32_t last_test_time = 0;
		if (HAL_GetTick() - last_test_time > 10000) {
			last_test_time = HAL_GetTick();
			// Simple_Protocol_Test();  // 暂时禁用测试数据
		}

		// ?????????????CPU
		HAL_Delay(1);

	} while(1);
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
   HAL_Delay(1000);
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;  // 改为DIV4，84MHz = (25MHz/25)*336/4
  RCC_OscInitStruct.PLL.PLLQ = 7;              // 调整PLLQ保持48MHz USB时钟
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;     // HCLK = 84MHz
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;      // APB1 = 42MHz (≤45MHz)
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;      // APB2 = 84MHz (≤90MHz)

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)  // 84MHz需要2个等待周期
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;

  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief UART5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART5_Init(void)
{

  /* USER CODE BEGIN UART5_Init 0 */

  /* USER CODE END UART5_Init 0 */

  /* USER CODE BEGIN UART5_Init 1 */

  /* USER CODE END UART5_Init 1 */
  huart5.Instance = UART5;
  huart5.Init.BaudRate = 115200;
  huart5.Init.WordLength = UART_WORDLENGTH_8B;
  huart5.Init.StopBits = UART_STOPBITS_1;
  huart5.Init.Parity = UART_PARITY_NONE;
  huart5.Init.Mode = UART_MODE_TX_RX;
  huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart5.Init.OverSampling = UART_OVERSAMPLING_16;

  if (HAL_UART_Init(&huart5) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE END UART5_Init 2 */

}


/*
 * Helper function to check RC value and block programm exectution
 */
static void check_rc(int rc, const char * msg_context)
{
	if(rc < 0) {
		INV_MSG(INV_MSG_LEVEL_ERROR, "%s: error %d (%s)\r\n", msg_context, rc, inv_error_str(rc));
		printf("WARNING: Continuing despite error in %s\r\n", msg_context);
		// while(1);  // 注释掉无限循环，允许程序继续执行
	}
}


/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();  /* Enable GPIOE clock for PE14 */

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CSB_GPIO_Port, CSB_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_14, GPIO_PIN_SET);  /* Set PE14 to HIGH */

  /*Configure GPIO pin : CSB_Pin */
  GPIO_InitStruct.Pin = CSB_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(CSB_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PC7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PE14 */
  GPIO_InitStruct.Pin = GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  /* Push-pull output */
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;        /* Pull-down resistor */
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; /* Low speed */
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

/* USER CODE BEGIN 4 */

/**
 * @brief Test PE14 GPIO pin functionality
 * @param None
 * @retval None
 */
void Test_PE14_GPIO(void)
{


    // Final state (HIGH)
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_14, GPIO_PIN_SET);

}

/*
 * This function initializes MCU on which this software is running.
 * It configures:
 *   - a UART link used to print some messages
 *   - interrupt priority group and GPIO so that MCU can receive interrupts from ICM4235x
 *   - a microsecond timer requested by Iim423xx driver to compute some delay
 *   - a microsecond timer used to get some timestamps
 *   - a serial link to communicate from MCU to Iim423xx
 */
static void SetupMCUHardware(struct inv_iim423xx_serif * icm_serif)
{
	// inv_io_hal_board_init();
	
	/* configure UART */
//	 config_uart(LOG_UART_ID);

	/* Setup message facility to see internal traces from FW */
	INV_MSG_SETUP(MSG_LEVEL, msg_printer);

	// 移除启动标题，保持输出简洁
	// INV_MSG(INV_MSG_LEVEL_INFO, "########################");
	// INV_MSG(INV_MSG_LEVEL_INFO, "#   Example Raw Data   #");
	// INV_MSG(INV_MSG_LEVEL_INFO, "########################");

	/*
	 * Configure input capture mode GPIO connected to pin EXT3-9 (pin PB03).
	 * This pin is connected to Icm406xx INT1 output and thus will receive interrupts 
	 * enabled on INT1 from the device.
	 * A callback function is also passed that will be executed each time an interrupt
	 * fires.
	*/
	// inv_gpio_sensor_irq_init(INV_GPIO_INT1, ext_interrupt_cb, 0);

	/* Init timer peripheral for delay */
	// inv_delay_init(DELAY_TIMER);
	
	/*
	 * Configure the timer for the timebase
	 */
	// inv_timer_configure_timebase(1000000);
	// inv_timer_enable(TIMEBASE_TIMER);

// #if USE_CLK_IN
// 	rtc_timer_init(NULL);
// 	/* Output 32kHz SLCK to PA17, it is up to user to connect it or not at board level to have CLKIN capability */
// 	inv_gpio_output_clk_on_pin(INV_GPIO_CLKIN);
// #endif

	/* Initialize serial inteface between MCU and Iim423xx */
	icm_serif->context   = 0;        /* no need */
	icm_serif->read_reg  = inv_io_hal_read_reg;
	icm_serif->write_reg = inv_io_hal_write_reg;
	icm_serif->max_read  = 1024*32;  /* maximum number of bytes allowed per serial read */
	icm_serif->max_write = 1024*32;  /* maximum number of bytes allowed per serial write */
	icm_serif->serif_type = SERIF_TYPE;
	// inv_io_hal_init(icm_serif); /* I2C/SPI IO bus init */
}

/*
 * Iim423xx interrupt handler.
 * Function is executed when an Iim423xx interrupt rises on MCU.
 * This function get a timestamp and store it in the timestamp buffer.
 * Note that this function is executed in an interrupt handler and thus no protection
 * are implemented for shared variable timestamp_buffer.
 */
static void ext_interrupt_cb(void * context, unsigned int int_num)
{
	(void)context;

#if USE_CLK_IN
	/* 
	 * Read timestamp from the RTC derived from SLCK, clocking CLKIN
	 */
	// uint64_t timestamp = rtc_timer_get_time_us();
#else
	/* 
	 * Read timestamp from the timer dedicated to timestamping 
	 */
	// uint64_t timestamp = inv_timer_get_counter(TIMEBASE_TIMER);
#endif

	// if(int_num == INV_GPIO_INT1) {
	// 	if (!RINGBUFFER_FULL(&timestamp_buffer))
	// 		RINGBUFFER_PUSH(&timestamp_buffer, &timestamp);
	// }

	irq_from_device |= TO_MASK(int_num);
}

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
	ext_interrupt_cb(0, INV_GPIO_INT1);
}	
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_7) { // PC7 - ???????
        irq_from_device |= TO_MASK(INV_GPIO_INT1);
    }
}


/* USER CODE END 4 */
/*
 * Printer function for message facility
 */
void msg_printer(int level, const char * str, va_list ap)
{
	static char out_str[256]; /* static to limit stack usage */
	unsigned idx = 0;
	const char * s[INV_MSG_LEVEL_MAX] = {
	    "",    // INV_MSG_LEVEL_OFF
	    "[E] ", // INV_MSG_LEVEL_ERROR
	    "[W] ", // INV_MSG_LEVEL_WARNING
	    "[I] ", // INV_MSG_LEVEL_INFO
	    "[V] ", // INV_MSG_LEVEL_VERBOSE
	    "[D] ", // INV_MSG_LEVEL_DEBUG
	};
	idx += snprintf(&out_str[idx], sizeof(out_str) - idx, "%s", s[level]);
	if(idx >= (sizeof(out_str)))
		return;
	idx += vsnprintf(&out_str[idx], sizeof(out_str) - idx, str, ap);
	if(idx >= (sizeof(out_str)))
		return;
	idx += snprintf(&out_str[idx], sizeof(out_str) - idx, "\r\n");
	if(idx >= (sizeof(out_str)))
		return;

	// inv_uart_mngr_puts(LOG_UART_ID, out_str, idx); /*output message to UART port on the board */
	  
	HAL_UART_Transmit(&huart1, (uint8_t *)out_str, idx, 0xFFFF);
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

void inv_helper_disable_irq(void)
{
	// inv_disable_irq();
}


/* --------------------------------------------------------------------------------------
 *  Extern functions definition
 * -------------------------------------------------------------------------------------- */

/*
 * Iim423xx driver needs to get time in us. Let's give its implementation here.
 */
uint64_t inv_iim423xx_get_time_us(void)
{
// #if USE_CLK_IN
	// return rtc_timer_get_time_us();
// #else
	// return inv_timer_get_counter(TIMEBASE_TIMER);
// #endif
  static uint64_t OldTimeVal;
  uint64_t NewTimeVal;

  NewTimeVal = (HAL_GetTick() * 1000) + (SysTick->VAL * 1000 / SysTick->LOAD);

  if (NewTimeVal < OldTimeVal) NewTimeVal += 1000;
  OldTimeVal = NewTimeVal;

  return NewTimeVal;
}

void inv_iim423xx_sleep_ms(uint32_t ms)
{
  while (ms--)
  {
    inv_iim423xx_sleep_us(1000);
  }
}

/*!
 * SPI read function
 */
unsigned char iim423xx_spi_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
	uint8_t device_addr = *(uint8_t*)intf_ptr;

  (void)intf_ptr;

  reg_addr |= 0x80;
  // 移除SPI调试信息
  // printf("SPI Read: reg=0x%02X, len=%lu\r\n", reg_addr, len);

  // ??CS????
  HAL_GPIO_WritePin(CSB_GPIO_Port, CSB_Pin, GPIO_PIN_SET);
  for(volatile int i = 0; i < 100; i++); // ???

  HAL_GPIO_WritePin(CSB_GPIO_Port, CSB_Pin, GPIO_PIN_RESET);
  for(volatile int i = 0; i < 100; i++); // ???

  HAL_SPI_Transmit(&hspi1, &reg_addr, 1, 100);

  // ??????
  while (HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_BUSY);

  // ?????????
  for(volatile int i = 0; i < 500; i++);

  HAL_SPI_Receive(&hspi1, reg_data, (uint16_t)len, 100);

  // ??????
  while (HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_BUSY);

  for(volatile int i = 0; i < 100; i++); // ???
  HAL_GPIO_WritePin(CSB_GPIO_Port, CSB_Pin, GPIO_PIN_SET);
  for(volatile int i = 0; i < 100; i++); // ???

  // 移除接收数据调试信息
  // if(len <= 4) {
  //   printf("Received: ");
  //   for(uint32_t i = 0; i < len; i++) {
  //     printf("0x%02X ", reg_data[i]);
  //   }
  //   printf("\r\n");
  // }

  return 0;
}

unsigned char iim423xx_spi_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
  uint8_t device_addr = *(uint8_t*)intf_ptr;

  (void)intf_ptr;

  reg_addr &= 0x7f;

  // 移除SPI调试信息
  // printf("SPI Write: reg=0x%02X, len=%lu\r\n", reg_addr, len);

  // ??CS????
  HAL_GPIO_WritePin(CSB_GPIO_Port, CSB_Pin, GPIO_PIN_SET);
  for(volatile int i = 0; i < 100; i++); // ???

  HAL_GPIO_WritePin(CSB_GPIO_Port, CSB_Pin, GPIO_PIN_RESET);
  for(volatile int i = 0; i < 100; i++); // ???

  HAL_SPI_Transmit(&hspi1, &reg_addr, 1, 100);

  while (HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_BUSY);

  for(volatile int i = 0; i < 100; i++); // ???

  HAL_SPI_Transmit(&hspi1, (uint8_t *)reg_data, (uint16_t)len, 100);

  while (HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_BUSY);

  for(volatile int i = 0; i < 100; i++); // ???
  HAL_GPIO_WritePin(CSB_GPIO_Port, CSB_Pin, GPIO_PIN_SET);
  for(volatile int i = 0; i < 100; i++); // ???

  return 0;
}


/*
 * Iim423xx driver needs a sleep feature from external device. Thus inv_iim423xx_sleep_us
 * is defined as extern symbol in driver. Let's give its implementation here.
 */
void inv_iim423xx_sleep_us(uint32_t us)
{
	// inv_delay_us(us);
	uint32_t count;
	
	count = us;
	count = (SystemCoreClock/1000000) * count;
  count = count > 16777216 ? 16777216 : count;
  SysTick->LOAD = count - 1;
  SysTick->VAL = 0;

  while (!((SysTick->CTRL >> 16) & 0x1));
}


//low-levevl IO access function
int inv_io_hal_read_reg(struct inv_iim423xx_serif * serif, uint8_t reg, uint8_t * rbuffer, uint32_t rlen)
{
	switch (serif->serif_type) {
		case IIM423XX_AUX1_SPI3:
		case IIM423XX_AUX1_SPI4:
		case IIM423XX_AUX2_SPI3:
		case IIM423XX_UI_SPI4:
			return iim423xx_spi_read(reg, rbuffer, rlen, 0);
//			return inv_spi_master_read_register(INV_SPI_AP, reg, rlen, rbuffer);
//		case IIM423XX_UI_I2C:
//			while(inv_i2c_master_read_register(ICM_I2C_ADDR, reg, rlen, rbuffer)) {
//			inv_delay_us(32000); // Loop in case of I2C timeout
//		}
//		return 0;
	default:
		return -1;
}
}
//low-levevl IO access function
int inv_io_hal_write_reg(struct inv_iim423xx_serif * serif, uint8_t reg, const uint8_t * wbuffer, uint32_t wlen)
{
int rc;

switch (serif->serif_type) {
	case IIM423XX_AUX1_SPI3:
	case IIM423XX_AUX1_SPI4:
	case IIM423XX_AUX2_SPI3:
	case IIM423XX_UI_SPI4:
		return iim423xx_spi_write(reg, wbuffer, wlen, 0);
//		for(uint32_t i=0; i<wlen; i++) {
//			rc = inv_spi_master_write_register(INV_SPI_AP, reg+i, 1, &wbuffer[i]);
//			if(rc)
//				return rc;
//		}
//		return 0;
//	case IIM423XX_UI_I2C:
//		while(inv_i2c_master_write_register(ICM_I2C_ADDR, reg, wlen, wbuffer)) {
//			inv_delay_us(32000); // Loop in case of I2C timeout
//		}
//		return 0;
	default:
		return -1;
}
}
/*
 * Clock calibration module needs to enable IRQ. Thus inv_helper_enable_irq is
 * defined as extern symbol in clock calibration module. Let's give its implementation
 * here.
 */
void inv_helper_enable_irq(void)
{
	// inv_enable_irq();
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/* USER CODE BEGIN 4 */
/**
 * @brief 简单协议测试函数
 */
void Simple_Protocol_Test(void)
{
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

    // 21个float32测试数据
    // 0Hz: 130000.0mg = 0x47FE2000
    frame[index++] = 0x00; frame[index++] = 0x20; frame[index++] = 0xFE; frame[index++] = 0x47;

    // 25Hz: 450.0mg = 0x43E10000
    frame[index++] = 0x00; frame[index++] = 0x00; frame[index++] = 0xE1; frame[index++] = 0x43;

    // 50Hz: 420.0mg = 0x43D20000
    frame[index++] = 0x00; frame[index++] = 0x00; frame[index++] = 0xD2; frame[index++] = 0x43;

    // 其余18个点用简化数据 (100.0f = 0x42C80000)
    for (int i = 0; i < 18; i++) {
        frame[index++] = 0x00; frame[index++] = 0x00; frame[index++] = 0xC8; frame[index++] = 0x42;
    }

    // 计算校验和 (命令码 + 长度 + 载荷)
    uint8_t checksum = 0;
    for (int i = 2; i < index; i++) {  // 从命令码开始到载荷结束
        checksum ^= frame[i];
    }
    frame[index++] = checksum;

    // 帧尾: 0D
    frame[index++] = 0x0D;

    // 协议帧发送已删除 - 仅输出调试信息
    printf("DEBUG: Protocol frame prepared (%d bytes) - not sent\r\n", index);
}

/**
 * @brief Calculate CRC16 for Modbus protocol
 * @param data: Data buffer
 * @param length: Data length
 * @retval CRC16 value
 */
uint16_t Calculate_CRC16_Modbus(uint8_t *data, uint16_t length)
{
    uint16_t crc = 0xFFFF;
    uint16_t i, j;

    for (i = 0; i < length; i++) {
        crc ^= data[i];
        for (j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc = crc >> 1;
            }
        }
    }

    return crc;
}

/**
 * @brief Build Modbus command for alarm register
 * @param value: Register value (0 or 1)
 * @param command_buffer: Output buffer for command
 */
void Build_Modbus_Command(uint8_t value, uint8_t *command_buffer)
{
    // Modbus command structure: 01 46 00 00 00 01 02 00 [value] [CRC_L] [CRC_H]
    command_buffer[0] = 0x01;  // Device address
    command_buffer[1] = 0x46;  // Function code (Write Multiple Registers)
    command_buffer[2] = 0x00;  // Register address high byte
    command_buffer[3] = 0x00;  // Register address low byte
    command_buffer[4] = 0x00;  // Number of registers high byte
    command_buffer[5] = 0x01;  // Number of registers low byte (1 register)
    command_buffer[6] = 0x02;  // Data length (2 bytes)
    command_buffer[7] = 0x00;  // Data high byte
    command_buffer[8] = value; // Data low byte (0 or 1)

    // Calculate CRC16
    uint16_t crc = Calculate_CRC16_Modbus(command_buffer, 9);
    command_buffer[9] = (uint8_t)(crc & 0xFF);        // CRC low byte
    command_buffer[10] = (uint8_t)((crc >> 8) & 0xFF); // CRC high byte
}

/**
 * @brief Send command to LoRa module
 * @param command: Command buffer
 * @param length: Command length
 */
void LoRa_Send_Command(uint8_t *command, uint8_t length)
{
    lora_tx_complete = 0;
    lora_timeout_start_time = HAL_GetTick();

    // Send command via UART5
    HAL_UART_Transmit(&huart5, command, length, 1000);
    lora_tx_complete = 1;

    printf("LoRa Command Sent: ");
    for (int i = 0; i < length; i++) {
        printf("%02X ", command[i]);
    }
    printf("\n");
}

// LoRa_Wait_Response函数已删除，改用状态机中的直接检查方式

/**
 * @brief Process alarm state machine
 */
void Process_Alarm_State_Machine(void)
{
    static uint8_t modbus_command[11];
    static uint8_t response[10];

    switch (alarm_state) {
        case ALARM_STATE_SET_1:
            printf("Setting alarm register to 1...\n");
            Build_Modbus_Command(1, modbus_command);
            LoRa_Send_Command(modbus_command, 11);
            alarm_state = ALARM_STATE_WAIT_RESPONSE_1;
            break;

        case ALARM_STATE_WAIT_RESPONSE_1:
            // 检查是否收到LoRa响应
            if (lora_rx_complete) {
                // 复制响应数据
                memcpy(response, lora_rx_buffer, lora_rx_index);

                printf("Alarm set to 1 confirmed\n");
                printf("LoRa Response Received: ");
                for (int i = 0; i < lora_rx_index; i++) {
                    printf("%02X ", lora_rx_buffer[i]);
                }
                printf("\n");

                // 重置接收状态
                lora_rx_complete = 0;
                lora_rx_index = 0;
                Start_LoRa_Reception();

                // 进入保持状态
                alarm_hold_start_time = HAL_GetTick();
                alarm_state = ALARM_STATE_HOLD;
                Send_Response_To_PC("ALARM_SET_SUCCESS");

                // 通知系统状态机报警发送进行中
                #if ENABLE_SYSTEM_STATE_MACHINE
                System_State_Machine_SetAlarmStatus(0);  // 进行中
                #endif
            } else if ((HAL_GetTick() - lora_timeout_start_time) > LORA_TIMEOUT_MS) {
                printf("Timeout waiting for response to set 1\n");
                alarm_state = ALARM_STATE_IDLE;
                Send_Response_To_PC("ALARM_SET_TIMEOUT");

                // 通知系统状态机报警发送失败
                #if ENABLE_SYSTEM_STATE_MACHINE
                System_State_Machine_SetAlarmStatus(2);  // 失败
                #endif
            }
            break;

        case ALARM_STATE_HOLD:
            if ((HAL_GetTick() - alarm_hold_start_time) >= ALARM_HOLD_TIME_MS) {
                printf("Hold time completed, setting alarm register to 0...\n");
                Build_Modbus_Command(0, modbus_command);
                LoRa_Send_Command(modbus_command, 11);
                alarm_state = ALARM_STATE_WAIT_RESPONSE_0;
            }
            break;

        case ALARM_STATE_WAIT_RESPONSE_0:
            // 检查是否收到LoRa响应
            if (lora_rx_complete) {
                // 复制响应数据
                memcpy(response, lora_rx_buffer, lora_rx_index);

                printf("Alarm set to 0 confirmed\n");
                printf("LoRa Response Received: ");
                for (int i = 0; i < lora_rx_index; i++) {
                    printf("%02X ", lora_rx_buffer[i]);
                }
                printf("\n");

                // 重置接收状态
                lora_rx_complete = 0;
                lora_rx_index = 0;
                Start_LoRa_Reception();

                // 进入完成状态
                alarm_state = ALARM_STATE_COMPLETE;
                Send_Response_To_PC("ALARM_RESET_SUCCESS");

                // 通知系统状态机报警发送成功
                #if ENABLE_SYSTEM_STATE_MACHINE
                System_State_Machine_SetAlarmStatus(1);  // 成功
                #endif
            } else if ((HAL_GetTick() - lora_timeout_start_time) > LORA_TIMEOUT_MS) {
                printf("Timeout waiting for response to set 0\n");
                alarm_state = ALARM_STATE_IDLE;
                Send_Response_To_PC("ALARM_RESET_TIMEOUT");

                // 通知系统状态机报警发送失败
                #if ENABLE_SYSTEM_STATE_MACHINE
                System_State_Machine_SetAlarmStatus(2);  // 失败
                #endif
            }
            break;

        case ALARM_STATE_COMPLETE:
            printf("Alarm cycle completed successfully\n");
            alarm_state = ALARM_STATE_IDLE;
            Send_Response_To_PC("ALARM_CYCLE_COMPLETE");
            break;

        default:
            break;
    }
}

/**
 * @brief Trigger alarm cycle
 */
void Trigger_Alarm_Cycle(void)
{
    if (alarm_state == ALARM_STATE_IDLE) {
        alarm_state = ALARM_STATE_SET_1;
        printf("Alarm cycle triggered\n");
    } else {
        printf("Alarm cycle already in progress\n");
    }
}

/**
 * @brief Process UART1 command from upper computer
 */
void Process_UART1_Command(void)
{
    // 处理二进制协议命令
    if (uart1_rx_index >= 1) {
        uint8_t command = uart1_rx_buffer[0];

        switch (command) {
            case 0x10:  // 触发报警命令
                // 触发LoRa报警周期
                Trigger_Alarm_Cycle();
                Send_Response_To_PC("ALARM_TRIGGERED");
                break;

            case 0x11:  // 查询状态命令
                // 发送状态响应
                Send_Response_To_PC("STATUS_OK");
                break;

            case 0x02:  // 原始数据命令（保持兼容）
                // 继续发送原始数据（已经在主循环中处理）
                break;

            case 0x04:  // FFT数据命令（保持兼容）
                // 继续发送FFT数据（已经在主循环中处理）
                break;

            default:
                // 未知命令
                break;
        }
    }

    // Clear buffer
    memset(uart1_rx_buffer, 0, sizeof(uart1_rx_buffer));
    uart1_rx_index = 0;
}

/**
 * @brief Send response to PC via UART1 (现在仅输出调试信息)
 * @param message: Message to send
 */
void Send_Response_To_PC(const char *message)
{
    // 不再发送协议数据，仅输出调试信息
    printf("DEBUG: Response message: %s\r\n", message);
}

/**
 * @brief Start UART1 reception
 */
void Start_UART1_Reception(void)
{
    uart1_rx_index = 0;
    uart1_rx_complete = 0;
    HAL_UART_Receive_IT(&huart1, &uart1_rx_buffer[uart1_rx_index], 1);
}

/**
 * @brief Start LoRa reception
 */
void Start_LoRa_Reception(void)
{
    lora_rx_index = 0;
    lora_rx_complete = 0;
    HAL_UART_Receive_IT(&huart5, &lora_rx_buffer[lora_rx_index], 1);
}

/**
 * @brief UART receive complete callback
 * @param huart: UART handle
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) {
        // UART1 - Upper computer communication - 简化版本，避免printf导致重启
        if (uart1_rx_buffer[uart1_rx_index] == '\n' || uart1_rx_buffer[uart1_rx_index] == '\r') {
            uart1_rx_buffer[uart1_rx_index] = '\0';
            uart1_rx_complete = 1;
        } else {
            uart1_rx_index++;
            if (uart1_rx_index < sizeof(uart1_rx_buffer) - 1) {
                HAL_UART_Receive_IT(&huart1, &uart1_rx_buffer[uart1_rx_index], 1);
            } else {
                // Buffer overflow, reset
                uart1_rx_index = 0;
                HAL_UART_Receive_IT(&huart1, &uart1_rx_buffer[uart1_rx_index], 1);
            }
        }
    } else if (huart->Instance == UART5) {
        // UART5 - LoRa communication - 简化版本
        lora_rx_index++;
        if (lora_rx_index >= 7) {  // Expected response length for Modbus
            lora_rx_complete = 1;
        } else if (lora_rx_index < sizeof(lora_rx_buffer)) {
            HAL_UART_Receive_IT(&huart5, &lora_rx_buffer[lora_rx_index], 1);
        } else {
            // Buffer overflow, reset
            lora_rx_index = 0;
            HAL_UART_Receive_IT(&huart5, &lora_rx_buffer[lora_rx_index], 1);
        }
    }
}

/* USER CODE END 4 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
