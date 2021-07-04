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


/* Global external variable */
extern SemaphoreHandle_t			dreq_sem, 
															spi_tx_dma_sem, 
															sdio_rx_dma_sem;

extern TimerHandle_t					bl_tim;						/* backlight timer handle */

#if (configUSE_TRACE_FACILITY == 1)
extern traceHandle 						dreq_handle, 
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
	portBASE_TYPE taskWoken = pdFALSE;
	/* DREQ falling edge */
#if (configUSE_TRACE_FACILITY == 1)
	vTraceStoreISRBegin(dreq_handle);
#endif
	switch(GPIO_Pin) {
		/* vs10xx request data */
		case DREQ_Pin:
			xSemaphoreGiveFromISR(dreq_sem, &taskWoken);
			portEND_SWITCHING_ISR(taskWoken);
		break;
		
		/* KEY pressed */
		case KEY1_Pin:
		case KEY2_Pin:
		case KEY3_Pin:
			/* power of Backlight */
			HAL_GPIO_WritePin(BL_PWM_GPIO_Port,BL_PWM_Pin, GPIO_PIN_SET);
			/* reset backlight timer */
			xTimerResetFromISR(bl_tim,&taskWoken);
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
