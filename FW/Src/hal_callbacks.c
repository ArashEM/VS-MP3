/*****************************************************************************
  * @file    hal_callbacks.c
  * @author  Arash Golgol
  * @brief   Callbacks for HAL APIs
*****************************************************************************/
/* FreeRTOS headers */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"

/* HAL headers */
#include "main.h"
#include "spi.h"

/* General headers */
#include "debug.h"
#include "helper.h"
#include "mp3_player.h"


/* Global external variable */
extern SemaphoreHandle_t				dreq_sem, 
																spi_tx_dma_sem, 
																sdio_rx_dma_sem;
extern struct controller_qlist*	qlist;						/* queue list for all tasks */

extern TimerHandle_t						bl_tim,						/* backlight timer handle */
																gpio_tim;					/* gpio debouncer */
extern QueueHandle_t						hw_queue;					/* event and commands from hw */

#if (configUSE_TRACE_FACILITY == 1)
extern traceHandle 						exti0_handle, 
															spi_dma_handle,
															sdio_dma_handle;
#endif

/**
  * @brief  EXTI line detection callbacks.
  * @param  GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	portBASE_TYPE 	taskWoken = pdFALSE;
	static uint8_t	vol = INIT_VOLUME;
	static uint32_t				event;
	
#if (configUSE_TRACE_FACILITY == 1)
	vTraceStoreISRBegin(exti0_handle);
#endif
	
	switch(GPIO_Pin) {
		/* vs10xx request data */
		case DREQ_Pin:
			xSemaphoreGiveFromISR(dreq_sem, &taskWoken);
			portEND_SWITCHING_ISR(taskWoken);
		break;
		
		/* KEY1 pressed, VOL+ */
		case KEY1_Pin:
			vol += 0x08;
			set_volume(qlist, vol);
			/* power of Backlight */
			HAL_GPIO_WritePin(BL_PWM_GPIO_Port,BL_PWM_Pin, GPIO_PIN_SET);
			/* reset backlight timer */
			xTimerResetFromISR(bl_tim,&taskWoken);
			portEND_SWITCHING_ISR(taskWoken);
		break;
		
		/* KEY2 pressed, VOL- */
		case KEY2_Pin:
			vol -= 0x08;
			set_volume(qlist, vol);
			/* power of Backlight */
			HAL_GPIO_WritePin(BL_PWM_GPIO_Port,BL_PWM_Pin, GPIO_PIN_SET);
			/* reset backlight timer */
			xTimerResetFromISR(bl_tim,&taskWoken);
			portEND_SWITCHING_ISR(taskWoken);
			break; 
		
		case KEY3_Pin:
			event = HW_KEY3_PRESSED;
			/* power of Backlight */
			HAL_GPIO_WritePin(BL_PWM_GPIO_Port,BL_PWM_Pin, GPIO_PIN_SET);
			/* reset backlight timer */
			xTimerResetFromISR(bl_tim, &taskWoken);
		  xQueueSendFromISR(hw_queue, &event, &taskWoken);
			portEND_SWITCHING_ISR(taskWoken);
		break;
		
		default:
		break;
	} /* switch(GPIO_Pin) */
	
#if (configUSE_TRACE_FACILITY == 1)
	vTraceStoreISREnd(0);
#endif
	
}

/**
  * @brief  Tx Transfer completed callback.
  * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	portBASE_TYPE taskWoken = pdFALSE;
#if (configUSE_TRACE_FACILITY == 1)
	vTraceStoreISRBegin(spi_dma_handle); 
#endif
  if(hspi == &hspi1) {
		xSemaphoreGiveFromISR(spi_tx_dma_sem, &taskWoken);	
		portEND_SWITCHING_ISR(taskWoken);
	}
#if (configUSE_TRACE_FACILITY == 1)
	vTraceStoreISREnd(0);
#endif
}

/**
  * @brief BSP Rx Transfer completed callback
  * @retval None
  * @note empty (up to the user to fill it in or to remove it if useless)
  */
void BSP_SD_ReadCpltCallback(void)
{
#if (configUSE_TRACE_FACILITY == 1)
	vTraceStoreISRBegin(sdio_dma_handle);
#endif
	portBASE_TYPE taskWoken = pdFALSE;
	xSemaphoreGiveFromISR(sdio_rx_dma_sem, &taskWoken); 
	portEND_SWITCHING_ISR(taskWoken);
#if (configUSE_TRACE_FACILITY == 1)
	vTraceStoreISREnd(0);
#endif
}

/* End Of File */
