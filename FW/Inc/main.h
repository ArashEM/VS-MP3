/**
  ******************************************************************************
  * File Name          : main.h
  * Description        : This file contains the common defines of the application
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H
  /* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

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
#define xRST_Pin GPIO_PIN_1
#define xRST_GPIO_Port GPIOB
#define HDR_IO4_Pin GPIO_PIN_10
#define HDR_IO4_GPIO_Port GPIOB
#define SPI2_NCS1_Pin GPIO_PIN_11
#define SPI2_NCS1_GPIO_Port GPIOB
#define SPI2_NCS0_Pin GPIO_PIN_12
#define SPI2_NCS0_GPIO_Port GPIOB
#define HDR_IO3_Pin GPIO_PIN_6
#define HDR_IO3_GPIO_Port GPIOC
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

void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)

/**
  * @}
  */ 

/**
  * @}
*/ 

#endif /* __MAIN_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
