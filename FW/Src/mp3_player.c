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
#include "timers.h"

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
SemaphoreHandle_t					dreq_sem,					/* vs10xx dreq IRQ */
													spi_tx_dma_sem;		/* spi1_tx DMA compelete */
TimerHandle_t							bl_tim;						/* backlight timer handle */
struct controller_qlist*	qlist;						/* queue list for all tasks */
QueueHandle_t							hw_queue;					/* event and commands from hw */


#if (configUSE_TRACE_FACILITY == 1)
traceString 							sd_chn, vs_chn;	
traceHandle 							exti0_handle, spi_dma_handle;
#endif

/**
 * \brief initilize mp3-player and creat related tasks  	
 */
void vsmp3_init(void *vparameters)
{
	/* create queues */
	qlist = vsmp3_create_queues();
	
	/* create tasks */
	vsmp3_create_tasks(qlist);
	
	/* create queue for HW events */
	hw_queue = xQueueCreate(5, sizeof(int));
	configASSERT(hw_queue);
	
	/* create backlight timer */
	bl_tim = xTimerCreate("backlight", 
												pdMS_TO_TICKS(10000), 
												pdFALSE, 
												0, 
												vtimer_backlight);
	configASSERT(bl_tim);
	xTimerStart(bl_tim, portMAX_DELAY);
	
	/* debug */
	debug_print("task creation done\r\n");
	
	/* create binary semaphore for VS10xx DREQ interrup */
	dreq_sem = xSemaphoreCreateBinary();
	configASSERT(dreq_sem);

	/* create binary semaphore for SPI-TX DAM transfer complete */
	spi_tx_dma_sem = xSemaphoreCreateBinary();
	configASSERT(spi_tx_dma_sem);
	
	/* init vs1063 */
	vs_setup(&hspi1);
	
#if (configUSE_TRACE_FACILITY == 1)
	/* set queue name */
	vTraceSetQueueName(qlist->blink, "q-blink");
	vTraceSetQueueName(qlist->vs10xx, "q-vs10xx");
	vTraceSetQueueName(qlist->sdcard, "q-sdcard");
	vTraceSetQueueName(qlist->hmi, "q-hmi");
	vTraceSetQueueName(hw_queue, "q-hw");
	
	/* set semaphore name */
	vTraceSetSemaphoreName(dreq_sem, "dreq-sem");
	vTraceSetSemaphoreName(spi_tx_dma_sem, "spi-tx-dma-sem");
	
	/* set channel name */
	sd_chn = xTraceRegisterString("sdcard");
	vs_chn = xTraceRegisterString("vs10xx");
	
	/*set ISR name */
	exti0_handle = xTraceSetISRProperties("EXTI0-isr",
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
	struct stream_buff*				sbuff;
	FATFS* 										fs;  				/* FS object for SD card logical drive */
	FRESULT										result;
	DIR 											dir;
	static FILINFO 						fno;				/* valid pointer to fno.fname */
	BaseType_t 								xstatus;	
	uint32_t									hw_msg;
	
	/* check for sd-card presence */
	if(HAL_GPIO_ReadPin(SD_CD_GPIO_Port, SD_CD_Pin) != GPIO_PIN_RESET) {
		maintenance_mode("No SD Card",pqlist);
	}
	
	/* check for KEY4 pressed state */
	if(HAL_GPIO_ReadPin(KEY4_GPIO_Port, KEY4_Pin)  == GPIO_PIN_RESET) {
		/* init code for USB_DEVICE */
		MX_USB_DEVICE_Init();
		maintenance_mode("USB MSD",pqlist);
	}
	
	/* mount SD card */
	fs = (FATFS *)pvPortMalloc(sizeof(FATFS));
	configASSERT(fs);
	if (f_mount(fs,"", 0) != FR_OK) {
		debug_print("f_mount failed\r\n");
	}
	
	/* initlizie file and stream buffer */
	sbuff = sbuff_alloc(pqlist->sdcard);
	configASSERT(sbuff);
	
	/* check remained heap */
	debug_print("free heap: %zu\r\n",xPortGetFreeHeapSize());
	
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
			xstatus = xQueueReceive(hw_queue, &hw_msg, pdMS_TO_TICKS(500));  
			if (xstatus == pdPASS) {
				if(hw_msg == HW_KEY3_PRESSED) {
					break;		/* get out of while() loop */
				}
			}
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
	TickType_t								waits = portMAX_DELAY;
	static BaseType_t					vs_status;
	void*											buff;
	size_t										len;
	uint8_t										vol = INIT_VOLUME;
	static uint32_t						vol_is_set;
	
	/* main loop */
	for(;;) {
		xstatus = xQueueReceive(pqlist->vs10xx, &vs10xx_cmd, waits);
		if (xstatus == pdPASS) {
		//debug_print("cmd: %02x, arg: %p\r\n", vs10xx_cmd.cmd 
		//																		,	(void *) vs10xx_cmd.arg);
			sbuff = (struct stream_buff *)vs10xx_cmd.arg;
			switch (vs10xx_cmd.cmd) {
				case CMD_VS10XX_PLAY:
					lwrb = &sbuff->lwrb;
					waits = 0;			/* don't wait for command after playing is started*/
					vs_status = 0x01;
				break;
				
				case CMD_VS10XX_SET_VOL:
					vol = (uint8_t)vs10xx_cmd.arg;
					vol_is_set = 0x00;
				break;
				
				case CMD_VS10XX_STOP:
					vs_status = 0x00;
					waits = portMAX_DELAY;
				//ToDo: cancel/end current play (to avoid glitch)
				break;
				
				default:
				break; 
			} /* switch (vs10xx_cmd.cmd) */
		} /* if (xstatus == pdPASS) */
		
		/* playing */
		if (vs_status == 0x01) {
			/* on DREQ rising edge VS1063a can take 
			 * at least 32 bytes of SDI data or one SCI command */
			if(HAL_GPIO_ReadPin(VS_DREQ_PORT, VS_DREQ) == GPIO_PIN_SET) {
				/* SCI commands */
				if(!vol_is_set) {
					vs_set_volume(&hspi1, vol, vol);
					vol_is_set = 0x01;
					continue;
				}
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
 * \breif HMI controller 
 */
void vtask_hmi(void* vparameters)
{
	struct controller_qlist* 	pqlist 	= vparameters;	/* command queue */
	char 											banner[40];
	char*											message;
	uint32_t									index = 0;
	BaseType_t 								xstatus;
	struct mp3p_cmd 					hmi_cmd;
	extern GUI_CONST_STORAGE 	GUI_BITMAP bmowl;

	
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
	sprintf(banner,"Built on %s: %s",__DATE__, __TIME__);
	GUI_DispStringAt(banner, 120, 0);
	
	
	/* main loop*/
	for(;;) {
		/* wait 1000mS for new command */
		xstatus = xQueueReceive(pqlist->hmi, &hmi_cmd, pdMS_TO_TICKS(1000));
		if (xstatus == pdPASS) {
			switch(hmi_cmd.cmd) {
				case CMD_HMI_SET_MSG:
					message = (char *)hmi_cmd.arg;
					GUI_DispStringAtCEOL(message, 160, 220);
					index=0;
					break;
				
				default:
					break; 
			} /* switch(hmi_cmd.cmd) */
		} /* if (xstatus == pdPASS) */
		GUI_DispDecAt( index++, 10,220,4);
	} /* for(;;) */
}



/**
 * \brief Backlight callback
 */
void vtimer_backlight(TimerHandle_t xTimer)
{
	/* when expire, turn off backlight */
	HAL_GPIO_WritePin(BL_PWM_GPIO_Port,BL_PWM_Pin, GPIO_PIN_RESET);
}
/* end of file */
