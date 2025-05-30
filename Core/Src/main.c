/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
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
#include "crc.h"
#include "dma.h"
#include "dma2d.h"
#include "i2c.h"
#include "ltdc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_host.h"
#include "gpio.h"
#include "fmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
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

Gyro_Int_Data gyro_raw_data_s = { 0 };
Gyro_Int_Data gyro_scaled_data_s = { 0 };
Gyro_Int_Data gyro_offset_s = { 0 };

int16_t speed_x = 0;
int16_t speed_y = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int screen_id=0;

void click_led() {
	static uint8_t clicked;
	static uint32_t time;
	if (HAL_GPIO_ReadPin(B2_GPIO_Port, B2_Pin) == GPIO_PIN_SET) {
		if ((HAL_GetTick() - time) > 50) {
			time = HAL_GetTick();
			if (clicked == 0) {
				clicked = 1;
				if(screen_id==1){
					screen_id++;
					HAL_TIM_Base_Start_IT(&htim7);
				}
				HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
				time = HAL_GetTick();
			}
		}
	} else {
		clicked = 0;
	}
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
	if (hspi == &hspi5) {
		go_for_next_chunk();
	}
}
void lcd_print_all_chars(void) {
    const char znaczki[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const int znak_szer = 16;
    const int znak_wys = 16;

    int i = 0;

    for (int y = 0; y < LCD_HEIGHT && i < 36; y += znak_wys) {
        for (int x = znak_szer; x + znak_szer <= LCD_WIDTH && i < 36; x += znak_szer) {
            lcd_set_char(i, x, y, znaczki[i], GREEN);
            i++;
        }
    }
}
void set_new_figs(void) {
	lcd_set_rectangle(0, 100, 0, 50, 100, RED);
	lcd_set_rectangle(1, 150, 190, 60, 15, YELLOW);
	lcd_set_rectangle(2, 0, 250, 200, 30, RED);
	lcd_set_circle(LCD_WIDTH/2, LCD_HEIGHT/2, 10, GREEN);

	lcd_print_all_chars();
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
  MX_CRC_Init();
  MX_DMA2D_Init();
  MX_FMC_Init();
  MX_I2C3_Init();
  MX_LTDC_Init();
  MX_SPI5_Init();
  MX_TIM1_Init();
  MX_USART1_UART_Init();
  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */
	lcd_init();
	for (int y = 0; y < LCD_HEIGHT; y++) {
		for (int x = 0; x < LCD_WIDTH; x++) {
			lcd_put_pixel(x, y, BLUE);
		}
	}
	set_new_figs();
	lcd_update();

	while(!spi5_acquire()){}
	spi5_release();


	HAL_Delay(500); //żeby LCD skończył swoje przesyłanie
	gyro_init();
	HAL_Delay(2000);
	gyro_calculate_offset(&gyro_offset_s);
	HAL_Delay(1000);
	screen_id = 1;
//	HAL_TIM_Base_Start_IT(&htim7);

  /* USER CODE END 2 */

  /* Call init function for freertos objects (in cmsis_os2.c) */
  //MX_FREERTOS_Init();

  /* Start scheduler */
  //osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

	while (1) {

//		gyro_get_filtered_data(&gyro_raw_data_s);
//		gyro_compensate_and_scale(&gyro_raw_data_s, &gyro_offset_s, &gyro_scaled_data_s);
//		ball_set_speed();

//		lcd_update();
		click_led();
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 3;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */
  else if(htim->Instance == TIM7) {
	 gyro_get_filtered_data(&gyro_raw_data_s);
	 gyro_compensate_and_scale(&gyro_raw_data_s, &gyro_offset_s, &gyro_scaled_data_s);

	 ball_set_speed(&speed_x,&speed_y);
	 ball_move(&speed_x,&speed_y);
  }
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
	while (1) {
	}
  /* USER CODE END Error_Handler_Debug */
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
