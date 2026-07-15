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
#include "adc.h"
#include "fdcan.h"
#include "i2c.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "can_protocol.h"
#include "can_service.h"
#include "central_sim.h"
#include "distance_sensor_service.h"
#include "front_zonal_app.h"
#include "input_service_stm32.h"
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

CanService can_service;
CanServiceStatus can_status;

DistanceSensorStatus distance_sensor_status;
InputService input_service;
InputServiceStatus input_status;

CentralSimulator central_simulator;
FrontZonalApp front_zonal_app;

volatile uint8_t distance_sensor_fault_injection = 0U;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_FDCAN1_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  if (InputServiceStm32_Init(
          &input_service,
          HAL_GetTick()) == 0U)
  {
    Error_Handler();
  }
  InputService_GetSnapshot(
      &input_service,
      &input_status);

  DistanceSensorService_Init();
  DistanceSensorService_GetSnapshot(
      &distance_sensor_status);

  /* Initialize CAN FD transport and RX notification. */
  if (CanService_Init(
          &can_service,
          &hfdcan1,
          HAL_GetTick()) == 0U)
  {
      Error_Handler();
  }
  CanService_GetSnapshot(
      &can_service,
      &can_status);
  CentralSimulator_Init(
      &central_simulator,
      HAL_GetTick());
  FrontZonalApp_Init(&front_zonal_app);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      uint32_t current_time = HAL_GetTick();

      DistanceSensorService_Process(current_time);
      DistanceSensorService_GetSnapshot(
          &distance_sensor_status);

      /* Nucleo B1 사용자 버튼 원시값 읽기 */
      InputServiceStm32_Process(
          &input_service,
          current_time);
      InputService_GetSnapshot(
          &input_service,
          &input_status);
      CanService_GetSnapshot(
          &can_service,
          &can_status);

      /* Update vehicle state and requested outputs. */
      FrontZonalAppInput app_input = {0};

      app_input.command_received =
          (uint8_t)(can_status.central_command_rx_count > 0U);
      app_input.last_command_rx_time_ms =
          can_status.last_central_command_rx_time_ms;
      app_input.command_led_request =
          can_status.last_central_command.led_request;
      app_input.command_warning_led_request =
          can_status.last_central_command.warning_led_request;
      app_input.distance_sensor_ready =
          distance_sensor_status.api_ready;
      app_input.distance_valid =
          distance_sensor_status.measurement_valid;
      app_input.distance_range_status =
          distance_sensor_status.range_status;
      app_input.distance_sensor_fault_injection =
          distance_sensor_fault_injection;

      FrontZonalApp_Process(
          &front_zonal_app,
          &app_input,
          current_time);

      HAL_GPIO_WritePin(
          LD2_GPIO_Port,
          LD2_Pin,
          (front_zonal_app.applied_led_output != 0U) ?
              GPIO_PIN_SET : GPIO_PIN_RESET);

      HAL_GPIO_WritePin(
          WARNING_LED_GPIO_Port,
          WARNING_LED_Pin,
          (front_zonal_app.applied_warning_led_output != 0U) ?
              GPIO_PIN_SET : GPIO_PIN_RESET);

      /* 마지막 송신으로부터 100ms가 지났는지 확인 */
      if (CanService_IsFrontStatusTxDue(
              &can_service,
              current_time) != 0U)
      {
          FrontZonalStatus status_message = {0};
          FrontZonalStatusInput status_input = {0};

          status_input.alive_counter =
              CanService_GetNextFrontStatusAliveCounter(
                  &can_service);
          status_input.uptime_ms = current_time;
          status_input.adc_raw =
              (uint16_t)input_status.adc_raw;
          status_input.adc_valid =
              input_status.adc_valid;
          status_input.zone_switch =
              input_status.zone_switch_debounced;
          status_input.distance_mm =
              distance_sensor_status.distance_mm;
          status_input.distance_range_status =
              distance_sensor_status.range_status;
          status_input.distance_valid =
              distance_sensor_status.measurement_valid;
          status_input.distance_sensor_ready =
              distance_sensor_status.api_ready;
          status_input.distance_sensor_fault_injection =
              distance_sensor_fault_injection;

          FrontZonalApp_BuildStatus(
              &front_zonal_app,
              &status_input,
              &status_message);

          (void)CanService_SendFrontStatus(
              &can_service,
              &status_message);
      }
      /* Pi가 연결되기 전까지만 사용하는 Central Computer 시뮬레이터 */
      CanService_GetSnapshot(
          &can_service,
          &can_status);

      CentralSimulatorInput central_input = {0};
      CentralControlCommand command_message = {0};

      central_input.distance_mm =
          can_status.last_front_status.distance_mm;
      central_input.range_status =
          can_status.last_front_status.range_status;
      central_input.distance_valid =
          can_status.last_front_status.distance_valid;
      central_input.distance_sensor_ready =
          can_status.last_front_status.vl53l0x_ready;
      central_input.last_status_rx_time_ms =
          can_status.last_front_status_rx_time_ms;

      if (CentralSimulator_BuildCommandIfDue(
              &central_simulator,
              &central_input,
              current_time,
              &command_message) != 0U)
      {
          if (CanService_SendCentralCommand(
                  &can_service,
                  &command_message) != 0U)
          {
              CentralSimulator_ConfirmCommandTransmitted(
                  &central_simulator);
          }
      }

      /*
       * 한 번 이상 송신했는데 300ms 동안 수신이 없으면
       * 통신이 끊긴 것으로 판단
       */
      CanService_Process(
          &can_service,
          current_time);
      CanService_GetSnapshot(
          &can_service,
          &can_status);

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
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV6;
  RCC_OscInitStruct.PLL.PLLN = 85;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void HAL_FDCAN_RxFifo0Callback(
    FDCAN_HandleTypeDef *hfdcan,
    uint32_t RxFifo0ITs)
{
    CanService_OnRxFifo0Interrupt(
        &can_service,
        hfdcan,
        RxFifo0ITs,
        HAL_GetTick());
}

/* USER CODE END 4 */

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
