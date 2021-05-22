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

/* Global variable */
//osSemaphoreId vs10xx_dreq_semHandle;
//osStaticSemaphoreDef_t vs10xx_dreq_sem_cb;
struct controller_qlist* qlist;	

/**
 * \brief initilize mp3-player and creat related tasks  	
 */
void vsmp3_init(void *vparameters)
{
	qlist = 
	(struct controller_qlist* )pvPortMalloc(sizeof(struct controller_qlist));
	
	/* create task command queue */
	qlist->blink = xQueueCreate(1, sizeof(struct mp3p_cmd));
	/* vs10xx and sdcard tasks need more depth */
	qlist->vs10xx = xQueueCreate(3, sizeof(struct mp3p_cmd));
	qlist->sdcard = xQueueCreate(3, sizeof(struct mp3p_cmd));
	
	/* setup vs1063 */
	vs_setup(&hspi1);
	vs_set_volume(&hspi1, 0x30, 0x30);
	/* increasing SPI clock rate */
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
	if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
	vs_ear_speaker(&hspi1, 0x01);
	vs_deselect_control();
	vs_select_data();
	
	/* create blink tasks */
	xTaskCreate(vtask_blink, 
	            "blink", 
	            TASK_BLINK_STACK_SIZE, 
	            qlist, 					/* command queue to blink task */
	            TASK_BLINK_PRIORITY, 
	            NULL);
	
	/* create vs10xx task */
	//  xTaskCreate(vtask_vs10xx,
	//  						"vs10xx",
	//  						TASK_VS10XX_STACK_SIZE,
	//  						qlist,
	//  						TASK_VS10XX_PRIORITY,
	//  						NULL);
							
	/* create sdcard streaming buffer task */
	//xTaskCreate(vtask_sdcard,
	//						"sdcard",
	//						TASK_SDCARD_STACK_SIZE,
	//						qlist,
	//						TASK_SDCARD_PRIORITY,
	//						NULL);
	
	/* create controller task */
	xTaskCreate(vtask_controller,
	            "controller",
	            TASK_CONTROLLER_STACK_SIZE,
							qlist,								/* list of command queues to each tasks */				
							TASK_CONTROLLER_PRIORITY, 
							NULL);
	
	/* definition and creation of vs10xx_dreq_sem */
//  osSemaphoreStaticDef(vs10xx_dreq_sem, &vs10xx_dreq_sem_cb);
//  vs10xx_dreq_semHandle = osSemaphoreCreate(osSemaphore(vs10xx_dreq_sem), 1);
	
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
	struct controller_qlist* 	qlist 	= vparameters;	/* command queue */
	struct mp3p_cmd						qcmd;
	struct stream_buff*				sbuff;
	uint8_t*									buff;
	FRESULT										result;
	FATFS* 										fs;  		/* File system object for SD card logical drive */
	
	/**
	 * sending command to vtask_blink()
	 */
	
	/* initlizie file and stream buffer */
	fs = (FATFS *)pvPortMalloc(sizeof(FATFS));
	sbuff = (struct stream_buff *)pvPortMalloc(sizeof(struct stream_buff));
	
	f_mount(fs,"" , 0);
	result = f_open(&sbuff->file, "Timber.mp3", FA_READ);
	
	/* initlizie lwrb */
	//buff  = (uint8_t *)pvPortMalloc(STREAM_BUFF_SIZE);
	//lwrb_init(&sbuff->lwrb, buff, STREAM_BUFF_SIZE);
	
	/* create SD card data stream buffer */
	//qcmd.cmd = CMD_SDCARD_START_READ;
	//qcmd.arg = (uintptr_t) sbuff;
	//xQueueSend(qlist->sdcard, &qcmd, 0);
	
	/* start playing mp3 file */
	//qcmd.cmd = CMD_VS10XX_PLAY;
	//qcmd.arg = (uintptr_t) sbuff;
	//xQueueSend(qlist->vs10xx, &qcmd, 0);
	
	/* start playing mp3 file */
	qcmd.cmd = CMD_LED_BLINK_SET;
	qcmd.arg = 200;
	
	for(;;) {
		xQueueSend(qlist->blink, &qcmd, 0);
		qcmd.arg += 200; 
		vTaskDelay(pdMS_TO_TICKS(5000));
	}
}


/**
 * \brief blink led 	
 */
void vtask_blink(void* vparameters)
{
	struct controller_qlist* 	qlist 	= vparameters;	/* command queue */
	QueueHandle_t 						xqueue_blink = qlist->blink;
	struct mp3p_cmd 					led_cmd;
	BaseType_t 								delay = 500;    /*if no command is send ever */
	BaseType_t 								xstatus;
	
	/**
	blink LED infinite loop
	it's delay is set via command in message queue 
	*/
	for(;;) {
		/* Check the pointer is not NULL. */
		if (xqueue_blink != NULL) {
			/* don't wait for new command  */
			xstatus = xQueueReceive(xqueue_blink, &led_cmd, 0);  
			if (xstatus == pdPASS) {
				if ( (led_cmd.cmd == CMD_LED_BLINK_SET) && (led_cmd.arg != 0 ) ) {
					delay = led_cmd.arg; 
					GUI_DispDecAt(delay, 0,220,4);
				}
			} 
		}
		
		vTaskDelay(pdMS_TO_TICKS(delay));
		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
	}
}

/**
 * \brief VS1063A controller	
 */
void vtask_vs10xx(void* vparameters)
{
	struct controller_qlist* 	qlist 	= vparameters;	/* command queue */
	QueueHandle_t 						xqueue_vs10xx = qlist->vs10xx;
	struct mp3p_cmd 					vs_cmd, sd_cmd;
	lwrb_t*										lwrb;
	struct stream_buff*				sbuff;
	BaseType_t 								xstatus;
	BaseType_t								vs_status;
	void*											buff;
	
	/* main loop */
	for(;;) {
		if (xqueue_vs10xx != NULL) {
			xstatus = xQueueReceive(xqueue_vs10xx, &vs_cmd, 0);
			if (xstatus == pdPASS) {
				sbuff = (struct stream_buff*)vs_cmd.arg;
				/* new command */
				switch (vs_cmd.cmd) {
					case CMD_VS10XX_PLAY:
						lwrb = &sbuff->lwrb;
						vs_status = 0x01;
						break;
					case CMD_VS10XX_STOP:
						vs_status = 0x00;
						break;
					default:
						break;
				} /* swithc (vs_cmd.cmd) */
			} else {
				/* nothing */
			}			
			
		/* playing */
		if (vs_status == 0x01) {
			if (HAL_GPIO_ReadPin(VS_DREQ_PORT, VS_DREQ) != GPIO_PIN_SET) {
				//osSemaphoreWait(vs10xx_dreq_semHandle, osWaitForever);
			}
			/* on DREQ rising edge, it can accept at last 32 byte of data */
			if (lwrb_get_full(lwrb) >= STREAM_BUFF_HALF_SIZE ) {
				/* buffer is half full or more */
				buff = lwrb_get_linear_block_read_address(lwrb);
				HAL_SPI_Transmit(&hspi1, (uint8_t* )buff, 32 , 0xFFFF);
				lwrb_skip(lwrb, 32);
			} else {
				/* we need more data in stream buffer */
				sd_cmd.cmd = CMD_SDCARD_CONT_READ;
				sd_cmd.arg = (uintptr_t) sbuff;
				xQueueSend(qlist->sdcard, &sd_cmd, 0);
				} /* if (lwrb_get_full(lwrb) >= STREAM_BUFF_HALF_SIZE ) */
			} /* if (vs_status == 0x01) */			
		} /* if (xqueue_vs10xx != NULL) */
	} /* for(;;) */
}

/**
 * \breif SD-card data streamer
 */
void vtask_sdcard(void* vparameters)
{
	struct controller_qlist* 	qlist = vparameters;	/* command queue */
	QueueHandle_t 						xqueue_sdcard = qlist->sdcard;
	struct mp3p_cmd 					sd_cmd;
	struct stream_buff*				sbuff;
	FIL*											file;
	lwrb_t*										lwrb;
	BaseType_t 								xstatus;
	FRESULT										result;
	uint32_t 									read_len;
	void*											buff;
	size_t 										len;						
	
	for(;;) {
		if (xqueue_sdcard != NULL) {
			/* wait for command queue */
			xstatus = xQueueReceive(xqueue_sdcard, &sd_cmd, portMAX_DELAY);
			if (xstatus == pdPASS) {
				/* new command in queue */
				sbuff = (struct stream_buff*)sd_cmd.arg;
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
				}
			} 
			
			/* writing data to stream buffer (keep it full) */
			len = lwrb_get_linear_block_write_length (lwrb);
			buff = lwrb_get_linear_block_write_address(lwrb);
			result = f_read(file, buff, len, &read_len);
			if (result == FR_OK) {
				lwrb_advance(lwrb, read_len);
			} 
		}
	}
}
/* end of file */
