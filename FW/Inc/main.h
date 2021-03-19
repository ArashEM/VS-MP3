/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define CHG_Pin GPIO_PIN_13
#define CHG_GPIO_Port GPIOC
#define ADC_BAT_Pin GPIO_PIN_0
#define ADC_BAT_GPIO_Port GPIOC
#define KEY1_Pin GPIO_PIN_1
#define KEY1_GPIO_Port GPIOC
#define KEY2_Pin GPIO_PIN_2
#define KEY2_GPIO_Port GPIOC
#define KEY3_Pin GPIO_PIN_3
#define KEY3_GPIO_Port GPIOC
#define KEY4_Pin GPIO_PIN_0
#define KEY4_GPIO_Port GPIOA
#define LED_Pin GPIO_PIN_1
#define LED_GPIO_Port GPIOA
#define VDAC_Pin GPIO_PIN_4
#define VDAC_GPIO_Port GPIOA
#define xCS_Pin GPIO_PIN_4
#define xCS_GPIO_Port GPIOC
#define xDCS_Pin GPIO_PIN_5
#define xDCS_GPIO_Port GPIOC
#define DREQ_Pin GPIO_PIN_0
#define DREQ_GPIO_Port GPIOB
#define DREQ_EXTI_IRQn EXTI0_IRQn
#define xRST_Pin GPIO_PIN_1
#define xRST_GPIO_Port GPIOB
#define LCD_DCX_Pin GPIO_PIN_10
#define LCD_DCX_GPIO_Port GPIOB
#define TP_CS_Pin GPIO_PIN_11
#define TP_CS_GPIO_Port GPIOB
#define LCD_CS_Pin GPIO_PIN_12
#define LCD_CS_GPIO_Port GPIOB
#define LCD_RSTN_Pin GPIO_PIN_6
#define LCD_RSTN_GPIO_Port GPIOC
#define HDR_IO2_Pin GPIO_PIN_7
#define HDR_IO2_GPIO_Port GPIOC
#define HDR_IO1_Pin GPIO_PIN_8
#define HDR_IO1_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define PWR_EXT_Pin GPIO_PIN_15
#define PWR_EXT_GPIO_Port GPIOA
#define SD_CD_Pin GPIO_PIN_3
#define SD_CD_GPIO_Port GPIOB
#define REG_PG_Pin GPIO_PIN_4
#define REG_PG_GPIO_Port GPIOB
#define REG_LBO_Pin GPIO_PIN_5
#define REG_LBO_GPIO_Port GPIOB
#define REG_SYNC_Pin GPIO_PIN_8
#define REG_SYNC_GPIO_Port GPIOB
#define VBUS_Pin GPIO_PIN_9
#define VBUS_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
