/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "can.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include"DM_Motor.h"
#include "drv_can.h"
#include "vofa.h"
#include "RC_remoto_control.h"
#include "Windmil_2006.h"
//达妙4310 电机上电默认位置不是0点
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

/* USER CODE BEGIN PV */
RC_Ctl_t RC_CtrlData;
uint8_t RxData[18] = {0};//接收遥控器数据的数,单缓冲区
uint8_t flag_RC = 0;//远程遥控相关的标志位
float DM_4310_position = 0.0f;//大风车达妙电机的位置
uint8_t Windmill_flag = 0;//大风车的状态机变量
uint32_t t0 = 0;//计时的变量
float Windmill_Target = 0.0f;//4310 位置目标的变量
//uint8_t flag_FIF0 = 0;//收到新帧后再处理角度,暂时设置一个变量
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void RemoteDataProcess(uint8_t *pData)//解析数据
{
  if(pData == NULL)
  {
    return;
  }

  RC_CtrlData.rc.ch0 = ((int16_t)pData[0] | ((int16_t)pData[1] << 8)) & 0x07FF;
  RC_CtrlData.rc.ch1 = (((int16_t)pData[1] >> 3) | ((int16_t)pData[2] << 5))
& 0x07FF;
  RC_CtrlData.rc.ch2 = (((int16_t)pData[2] >> 6) | ((int16_t)pData[3] << 2) |
                       ((int16_t)pData[4] << 10)) & 0x07FF;
  RC_CtrlData.rc.ch3 = (((int16_t)pData[4] >> 1) | ((int16_t)pData[5]<<7)) &
0x07FF;

  RC_CtrlData.rc.s1 = ((pData[5] >> 4) & 0x000C) >> 2;
  RC_CtrlData.rc.s2 = ((pData[5] >> 4) & 0x0003);

  RC_CtrlData.mouse.x = ((int16_t)pData[6]) | ((int16_t)pData[7] << 8);
  RC_CtrlData.mouse.y = ((int16_t)pData[8]) | ((int16_t)pData[9] << 8);
  RC_CtrlData.mouse.z = ((int16_t)pData[10]) | ((int16_t)pData[11] << 8);

  RC_CtrlData.mouse.press_l = pData[12];
  RC_CtrlData.mouse.press_r = pData[13];

  RC_CtrlData.key.v = ((int16_t)pData[14]);// | ((int16_t)pData[15] << 8); ;//上拨最小，下拨最大
  RC_CtrlData.rc.dial = pData[16] | (pData[17] << 8);//上拨最小，下拨最大
  //your control code ….
  if (RC_CtrlData.rc.ch3>1024)//向上拨摇杆，
  {
    flag_RC=1;
  }
  else if (RC_CtrlData.rc.ch3<1024)//向下拨摇杆
  {
    flag_RC=0;
  }
  else
  {
    //do nothing
  }
}
//重写串口空闲中断
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
  if(huart==&huart1)
  {
    uint8_t*pData=RxData;
    if (Size==18)//判断是否接收的是完整的数据据
    {
      RemoteDataProcess(pData);
    }
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, RxData, 18);//持续接收数据
    __HAL_DMA_DISABLE_IT(&hdma_usart1_rx,DMA_IT_HT);
  }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

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

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_CAN1_Init();
  MX_CAN2_Init();
  MX_TIM5_Init();
  MX_USART1_UART_Init();
  MX_UART7_Init();
  /* USER CODE BEGIN 2 */

  //CAN2初始化代码
  CAN_Init(&hcan2,User_CANCallback);//初始化CN2 准备使用机械臂的达妙电机
  CAN_Filter_Mask_Config(&hcan2,CAN_FILTER(16)|CAN_FIFO_0|CAN_STDID|CAN_DATA_TYPE,0x41,0x00);//配置滤波器

  //遥控器初始化相关函数
  HAL_UARTEx_ReceiveToIdle_DMA(&huart1, RxData, 18);//开启遥控器数据接收的串口
  __HAL_DMA_DISABLE_IT(&hdma_usart1_rx,DMA_IT_HT);//关掉有关DMA的接收一半数据就触发的中断

  //舵机初始化代码
  HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_1);//大风车舵机1初始化
  HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_2);//大风车舵机2初始化
  HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_3);//大风车舵机3初始化
  //初始化大风车舵机上电实际角度 具体角度看实际来调
  __HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_1,200.0f);//初始未上膛//200.0放飞镖 320夹飞镖
  __HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_2,600.0f);//初始未上膛//750放飞镖 600夹紧
  __HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_3,630.0f);//初始未上膛//630放飞镖 400夹紧  //

  //初始化4310电机,变成位置模式
  DM_Motor_Init(&hcan2,0x51,POSITION);
  DM_Motor_Setposition(&hcan2,0x51,POSITION,0.0f,10.0f);//先让达妙电机上电回到0点
  HAL_Delay(1500);

  //初始化大疆电机2006
  PID_Init(&DJI_2006_speed);
  PID_Init(&DJI_2006_position);


  // DM_Motor_Init(&hcan2,0x51,SPEED);//初始化达妙电机,设置为位置模式 里面有HAL_Delay
  // DM_Motor_Setspeed(&hcan2,0x51,SPEED,10.0f);//右,正方向/大风车的方向到时候具体看
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();  /* Call init function for freertos objects (in cmsis_os2.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    /* USER CODE END WHILE */

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
  RCC_OscInitStruct.PLL.PLLM = 6;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM14 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM14)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
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
#ifdef USE_FULL_ASSERT
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
