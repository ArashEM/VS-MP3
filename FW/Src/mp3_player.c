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
#include "mp3_player.h"
#include "GUI.h"
#include "vs10xx.h"
#include "debug.h"

/* Global variable */
FATFS 										fs;  				/* FS object for SD card logical drive */
SemaphoreHandle_t					dreq_sem;		/* vs10xx dreq IRQ */
SemaphoreHandle_t					spi_tx_dma_sem;	
uint8_t										buff[STREAM_BUFF_SIZE];

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
	qlist->blink  = xQueueCreate(1, sizeof(struct mp3p_cmd));
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
	struct mp3p_cmd						qcmd;
	struct stream_buff*				sbuff;
	FRESULT										result;
	//uint8_t*									buff;
	
	/* initlizie file and stream buffer */
	sbuff = (struct stream_buff *)pvPortMalloc(sizeof(struct stream_buff));
	configASSERT(sbuff);
	sbuff->qwrite = pqlist->sdcard;
	result = f_open(&sbuff->file, "ALAN.flac", FA_READ);
	configASSERT(result == FR_OK);
	
	/* initlizie lwrb */
	//buff  = (uint8_t *)pvPortMalloc(STREAM_BUFF_SIZE);
	configASSERT(buff);
	lwrb_init(&sbuff->lwrb, buff, STREAM_BUFF_SIZE);
	lwrb_set_evt_fn(&sbuff->lwrb, sd_buff_evt_fn);
	
	debug_print("free heap: %zu\r\n",xPortGetFreeHeapSize());
	
	/* create SD card data stream buffer */
	qcmd.cmd = CMD_SDCARD_START_READ;
	qcmd.arg = (uintptr_t) sbuff;
	xQueueSend(pqlist->sdcard, &qcmd, 0);
	
	/* start playing mp3 file */
	qcmd.cmd = CMD_VS10XX_PLAY;
	qcmd.arg = (uintptr_t) sbuff;
	xQueueSend(pqlist->vs10xx, &qcmd, 0);
	
	/* start playing mp3 file */
	qcmd.cmd = CMD_LED_BLINK_SET;
	qcmd.arg = 1000;
	xQueueSend(pqlist->blink, &qcmd, 0);
	
	/* main loop */
	for(;;) {
		vTaskDelay(pdMS_TO_TICKS(10000));
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
	BaseType_t 								delay_ms = 500;    /*if no command is send ever */
	
	/* main loop */
	for(;;) {
		/* don't wait for command */
		xstatus = xQueueReceive(pqlist->blink, &blink_cmd, 0);	
		/* new command is available */
		if (xstatus == pdPASS) {		
			debug_print("cmd: %02x, arg: %02x\r\n", blink_cmd.cmd, blink_cmd.arg);
			if (blink_cmd.cmd == CMD_LED_BLINK_SET && blink_cmd.arg != 0) {
				delay_ms = blink_cmd.arg;
			}
		} /* if (xstatus == pdPASS) */
		
		vTaskDelay(pdMS_TO_TICKS(delay_ms));
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
 * \brief           Buffer event function
 */
static void sd_buff_evt_fn(lwrb_t* buff, lwrb_evt_type_t type, size_t len) {	
	struct stream_buff*		sbuff;
	struct mp3p_cmd 			sd_cmd;
	
	sbuff = container_of(buff, struct stream_buff, lwrb);
	
	switch (type) {
		case LWRB_EVT_RESET:
			break;
		
		case LWRB_EVT_READ:
			vTracePrintF(sd_chn, "Buffer read event: %d byte(s)!\r\n", (int)len);
			if (lwrb_get_full(buff) < STREAM_BUFF_HALF_SIZE ) {
				/* we need more data in stream buffer */
				sd_cmd.cmd = CMD_SDCARD_CONT_READ;
				sd_cmd.arg = (uintptr_t) sbuff;
				xQueueSend(sbuff->qwrite, &sd_cmd, 0);
			}	 /* if (lwrb_get_full(buff) < STREAM_BUFF_HALF_SIZE ) */
		break;
    
		case LWRB_EVT_WRITE:
			vTracePrintF(sd_chn, "Buffer write event: %d byte(s)!\r\n", (int)len);
		break;
    
		default: break;
	}
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
