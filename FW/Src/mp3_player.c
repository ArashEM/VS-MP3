/*****************************************************************************
  * @file    mp3_player.c
  * @author  Arash Golgol
  * @brief   MP3 player main tasks and functions
*****************************************************************************/
/* FreeRTOS headers */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "cmsis_os.h"

/* HAL headers */
#include "main.h"
#include "spi.h"

/* General headers */
#include <stdint.h>
#include "GUI.h"
#include "WM.h"
#include "mp3_player.h"
#include "vs10xx.h"
#include "debug.h"
#include "helper.h"

/* Global variable */
FATFS 										fs;  				/* FS object for SD card logical drive */
SemaphoreHandle_t					dreq_sem;		/* vs10xx dreq IRQ */
SemaphoreHandle_t					spi_tx_dma_sem;	
static FILINFO 						fno;				// ToDo: don't use global!
extern GUI_CONST_STORAGE GUI_BITMAP bmowl;

#if (configUSE_TRACE_FACILITY == 1)
traceString 							sd_chn, vs_chn;	
traceHandle 							dreq_handle, spi_dma_handle;
#endif

/**
 * \brief initilize mp3-player and creat related tasks  	
 */
void vsmp3_init(void *vparameters)
{
	BaseType_t									xstatus;
	struct controller_qlist*		qlist;			/* queue list for all tasks */
	
	qlist = 
	(struct controller_qlist* )pvPortMalloc(sizeof(struct controller_qlist));
	configASSERT(qlist);
	
	/* create task command queue */
	qlist->blink  = xQueueCreate(2, sizeof(struct mp3p_cmd));
	qlist->vs10xx = xQueueCreate(2, sizeof(struct mp3p_cmd));
	qlist->sdcard = xQueueCreate(2, sizeof(struct mp3p_cmd));
	
	/* create blink tasks */
	xstatus = 	xTaskCreate(vtask_blink, 
													"blink", 
													TASK_BLINK_STACK_SIZE, 
													qlist, 			/* command queue to blink task */
													TASK_BLINK_PRIORITY, 
													NULL);
	configASSERT(xstatus == pdPASS);
	
	/* create controller task */
	xstatus = 	xTaskCreate(vtask_controller,
													"controller",
													TASK_CONTROLLER_STACK_SIZE,
													qlist,		/* list of command queues to each tasks */				
													TASK_CONTROLLER_PRIORITY, 
													NULL);
	configASSERT(xstatus == pdPASS);
	
	/* create vs10xx task */
	xstatus = 	xTaskCreate(vtask_vs10xx,
													"vs10xx",
													TASK_VS10XX_STACK_SIZE,
													qlist,
													TASK_VS10XX_PRIORITY,
													NULL);
	configASSERT(xstatus == pdPASS);
	
	/* create sdcard streaming buffer task */
	xstatus = 	xTaskCreate(vtask_sdcard,
													"sdcard",
													TASK_SDCARD_STACK_SIZE,
													qlist,
													TASK_SDCARD_PRIORITY,
													NULL);
	configASSERT(xstatus == pdPASS);
	
	/* create HDMI controller task */
	xstatus = 	xTaskCreate(vtask_hmi,
													"hmi",
													TASK_HMI_STACK_SIZE,
													qlist,
													TASK_HMI_PRIORITY,
													NULL);
	configASSERT(xstatus == pdPASS);
	
	
	/* debug */
	debug_print("task creation done\r\n");
	
	/* create binary semaphore for VS10xx DREQ interrup */
	dreq_sem = xSemaphoreCreateBinary();
	configASSERT(dreq_sem);

	/* create binary semaphore for SPI-TX DAM transfer complete */
	spi_tx_dma_sem = xSemaphoreCreateBinary();
	configASSERT(spi_tx_dma_sem);
	
	/* mount SD card */
	if (f_mount(&fs,"", 0	) != FR_OK) {
		debug_print("f_mount failed\r\n");
	}
	
	/* init vs1063 */
	vs_setup(&hspi1);
	
#if (configUSE_TRACE_FACILITY == 1)
	/* set queue name */
	vTraceSetQueueName(qlist->blink, "q-blink");
	vTraceSetQueueName(qlist->vs10xx, "q-vs10xx");
	vTraceSetQueueName(qlist->sdcard, "q-sdcard");
	
	/* set semaphore name */
	vTraceSetSemaphoreName(dreq_sem, "dreq-sem");
	vTraceSetSemaphoreName(spi_tx_dma_sem, "spi-tx-dma-sem");
	
	/* set channel name */
	sd_chn = xTraceRegisterString("sdcard");
	vs_chn = xTraceRegisterString("vs10xx");
	
	/*set ISR name */
	dreq_handle = xTraceSetISRProperties("dreq-isr",
																				NVIC_GetPriority(DREQ_EXTI_IRQn));
	spi_dma_handle = xTraceSetISRProperties("spi-dma-isr", 
																		NVIC_GetPriority(DMA1_Channel3_IRQn));
#endif
	
	/* check remained heap */
	debug_print("free heap: %zu\r\n",xPortGetFreeHeapSize());
	
	/* Start the scheduler. */
	vTaskStartScheduler();

	/* Will only get here if there was not enough heap space to create the
	   idle task. */
}

/**
 * \brief main system controller   	
 */
void vtask_controller(void* vparameters)
{
	struct controller_qlist* 	pqlist 	= vparameters;	/* command queue */
	struct stream_buff*				sbuff;
	FRESULT										result;
	DIR 											dir;
	
	/* initlizie file and stream buffer */
	sbuff = sbuff_alloc(pqlist->sdcard);
	configASSERT(sbuff);
	
	result = f_opendir(&dir, (char *)"/");
	configASSERT(result == FR_OK);
	
	/* main loop */
	for(;;) {
		/* Read a directory item */
		result = f_readdir(&dir, &fno);			
		if (result != FR_OK) { 
			/* Break on error or end of dir */
			debug_print("f_readdir():%d\r\n",result);
			continue;  	
		} else if (fno.fname[0] == 0) {	
			/* end of DIR files */
			f_closedir(&dir);
			configASSERT(f_opendir(&dir, (char *)"/") == FR_OK);
			continue;
		}
		/* don't go inside dirs */
		if (fno.fattrib & AM_DIR) {
			continue;
		}
		
		/* play */
		// ToDo: Check file name ends with '.mp3'
		start_playing(sbuff, pqlist, fno.fname);
		while(!is_eof(sbuff)) {
			vTaskDelay(pdMS_TO_TICKS(500));
		}
		stop_playing(sbuff, pqlist);
	} /* for(;;) */
}


/**
 * \brief blink led 	
 */
void vtask_blink(void* vparameters)
{
	struct controller_qlist* 	pqlist 	= vparameters;	/* command queue */
	struct mp3p_cmd						blink_cmd;
	BaseType_t 								xstatus;
	TickType_t								queue_wait = portMAX_DELAY;
	BaseType_t 								blink_wait = 500;    /*if no command is send ever */
	
	/* main loop */
	for(;;) {
		/* don't wait for command */
		xstatus = xQueueReceive(pqlist->blink, &blink_cmd, queue_wait);	
		/* new command is available */
		if (xstatus == pdPASS) {		
			debug_print("cmd: %02x, arg: %02x\r\n", blink_cmd.cmd, blink_cmd.arg);
			switch(blink_cmd.cmd) {
				case CMD_LED_BLINK_SET:
					if(blink_cmd.arg != 0) {
						blink_wait = blink_cmd.arg;
						queue_wait = 0;
					}
				break;
				case CMD_LED_BLINK_OFF:
					HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
					queue_wait = portMAX_DELAY;
				continue;
			} /*switch(blink_cmd.cmd)*/
		} /* if (xstatus == pdPASS) */
		
		vTaskDelay(pdMS_TO_TICKS(blink_wait));
		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
	} /* for(;;) */
}

/**
 * \brief VS1063A controller	
 */
void vtask_vs10xx(void* vparameters)
{
	struct controller_qlist* 	pqlist 	= vparameters;	/* command queue */
	struct mp3p_cmd 					vs10xx_cmd;
	lwrb_t*										lwrb;
	struct stream_buff*				sbuff;
	BaseType_t 								xstatus;
	static BaseType_t					vs_status;
	void*											buff;
	size_t										len;
	
	/* main loop */
	for(;;) {
		/* don't wait for command */
		xstatus = xQueueReceive(pqlist->vs10xx, &vs10xx_cmd, 0);
		if (xstatus == pdPASS) {
		//debug_print("cmd: %02x, arg: %p\r\n", vs10xx_cmd.cmd 
		//																		,	(void *) vs10xx_cmd.arg);
			sbuff = (struct stream_buff *)vs10xx_cmd.arg;
			switch (vs10xx_cmd.cmd) {
				case CMD_VS10XX_PLAY:
					lwrb = &sbuff->lwrb;
					vs_status = 0x01;
				break;
				
				case CMD_VS10XX_STOP:
					vs_status = 0x00;
				//ToDo: cancel/end current play (to avoid glitch)
				break;
				
				default:
				break; 
			} /* switch (vs10xx_cmd.cmd) */
		} /* if (xstatus == pdPASS) */
		
		/* playing */
		if (vs_status == 0x01) {
			/* on DREQ rising edge, it can accept at last 32 byte of data */
			if(HAL_GPIO_ReadPin(VS_DREQ_PORT, VS_DREQ) == GPIO_PIN_SET) {
				len  = lwrb_get_linear_block_read_length(lwrb);
				buff = lwrb_get_linear_block_read_address(lwrb);
				/* only 32 byte can be transfered */
				if(len >= 32) { 
					len = 32;
				}
				// ToDo: Check return value
				HAL_SPI_Transmit(&hspi1, (uint8_t* )buff, len, HAL_MAX_DELAY);
				//if ( HAL_SPI_Transmit_DMA(&hspi1, (uint8_t* )buff, len) != HAL_OK) {
				//	continue;
				//}
				///* wait for transfer completion */
				//xSemaphoreTake(spi_tx_dma_sem, portMAX_DELAY);
				lwrb_skip(lwrb, len);
			} else {
				/* wait for DREQ */
				xSemaphoreTake(dreq_sem, portMAX_DELAY);
			} /* if(dreq == 1) */
		} /* if (vs_status == 0x01) */			
	} /* for(;;) */
}

/**
 * \breif SD-card data streamer
 */
void vtask_sdcard(void* vparameters)
{	
	struct controller_qlist* 	pqlist 	= vparameters;	/* command queue */
	struct mp3p_cmd 					sd_cmd;
	struct stream_buff*				sbuff;
	FIL*											file;
	lwrb_t*										lwrb;
	uint32_t 									read_len;
	void*											buff;		
	size_t										len;
	
	/* main loop */
	for(;;) {
		/* wait for command queue */
		xQueueReceive(pqlist->sdcard, &sd_cmd, portMAX_DELAY);
		//debug_print("cmd: %02x, arg: %p\r\n", sd_cmd.cmd, (void *) sd_cmd.arg);
		
		sbuff = (struct stream_buff *)sd_cmd.arg;
		switch(sd_cmd.cmd) {
			case CMD_SDCARD_START_READ:		/* file and lwrb are valid */
				file = &sbuff->file;
				lwrb = &sbuff->lwrb;
			break;
					
			case CMD_SDCARD_CONT_READ:		/* only lwrb is valid */
				/* if end of file is reached we can't read more data */
				if (f_eof(file)) {		
					continue;
				} else {
					lwrb = &sbuff->lwrb;
					break;
				}
								
			case CMD_SDCARD_STOP_READ:		/* file and lwrb are valid */
			/* end of streaming */
				f_close(&sbuff->file);
				lwrb_reset(&sbuff->lwrb);
			continue;
		} /* switch(sd_cmd.cmd) */
		
		/* writing data to stream buffer (keep it full) */
		len = lwrb_get_linear_block_write_length(lwrb);
		buff = lwrb_get_linear_block_write_address(lwrb);	
		if (f_read(file, buff, len, &read_len) == FR_OK) {
			lwrb_advance(lwrb, read_len);
		} 
	} /* for(;;) */
}

/**
 * HMI controller 
 */
void vtask_hmi(void* vparameters)
{
	struct controller_qlist* 	pqlist 	= vparameters;	/* command queue */
	char 											message[30];
	uint32_t									index;
	
	/* Init the STemWin GUI Library */
  GUI_Init();
	/* Activate the use of memory device feature */
  WM_SetCreateFlags(WM_CF_MEMDEV);
	/* show banner */
	GUI_SetBkColor(GUI_WHITE);
  GUI_Clear();
  GUI_DrawBitmap(&bmowl, 70, 30);
	GUI_SetColor(GUI_BLACK);
  GUI_SetFont(&GUI_Font16_ASCII);
	GUI_DispStringAt("VS-MP3 player ...", 0, 0);
	sprintf(message,"Built on %s: %s",__DATE__, __TIME__);
	GUI_DispStringAt(message, 120, 0);
	
	
	/* main loop*/
	for(;;) {
		GUI_DispDecAt( index++, 10,220,4);
		GUI_DispStringAtCEOL(fno.fname, 160, 220);
		if (index > 9999) {
			index = 0;
		}
		vTaskDelay(pdMS_TO_TICKS(1000));
	} /* for(;;) */
}

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
	if(GPIO_Pin == DREQ_Pin) {	
		xSemaphoreGiveFromISR(dreq_sem, &taskWoken); 	
		portEND_SWITCHING_ISR(taskWoken);
	}
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
/* end of file */
