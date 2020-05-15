/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

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
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId mp3p_taskHandle;
uint32_t mp3p_taskBuffer[ 128 ];
osStaticThreadDef_t mp3p_taskControlBlock;
osThreadId blink_taskHandle;
uint32_t blink_TaskBuffer[ 128 ];
osStaticThreadDef_t blink_TaskControlBlock;
osSemaphoreId vs10xx_dreq_semHandle;
osStaticSemaphoreDef_t vs10xx_dreq_sem_cb;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
  
/* USER CODE END FunctionPrototypes */

void mp3p_task_fn(void const * argument);
void blink_task_fn(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];
  
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}                   
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
       
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of vs10xx_dreq_sem */
  osSemaphoreStaticDef(vs10xx_dreq_sem, &vs10xx_dreq_sem_cb);
  vs10xx_dreq_semHandle = osSemaphoreCreate(osSemaphore(vs10xx_dreq_sem), 1);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of mp3p_task */
  osThreadStaticDef(mp3p_task, mp3p_task_fn, osPriorityAboveNormal, 0, 128, mp3p_taskBuffer, &mp3p_taskControlBlock);
  mp3p_taskHandle = osThreadCreate(osThread(mp3p_task), NULL);

  /* definition and creation of blink_task */
  osThreadStaticDef(blink_task, blink_task_fn, osPriorityBelowNormal, 0, 128, blink_TaskBuffer, &blink_TaskControlBlock);
  blink_taskHandle = osThreadCreate(osThread(blink_task), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_mp3p_task_fn */
/**
  * @brief  Function implementing the mp3p_task thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_mp3p_task_fn */
void mp3p_task_fn(void const * argument)
{
  /* USER CODE BEGIN mp3p_task_fn */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END mp3p_task_fn */
}

/* USER CODE BEGIN Header_blink_task_fn */
/**
* @brief Function implementing the blink_task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_blink_task_fn */
void blink_task_fn(void const * argument)
{
  /* USER CODE BEGIN blink_task_fn */
  /* Infinite loop */
  for(;;)
  {
    HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
		osDelay(500);
  }
  /* USER CODE END blink_task_fn */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
    
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
