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
struct controller_qlist* qlist;				/* queue list for all tasks */
FATFS 										fs;  				/* FS object for SD card logical drive */

/**
 * \brief initilize mp3-player and creat related tasks  	
 */
void vsmp3_init(void *vparameters)
{
	BaseType_t				xstatus;
	
	qlist = 
	(struct controller_qlist* )pvPortMalloc(sizeof(struct controller_qlist));
	configASSERT(qlist);
	
	/* create task command queue */
	qlist->blink  = xQueueCreate(3, sizeof(struct mp3p_cmd));
	qlist->vs10xx = xQueueCreate(3, sizeof(struct mp3p_cmd));
	qlist->sdcard = xQueueCreate(3, sizeof(struct mp3p_cmd));
	
	
	/* create blink tasks */
	xstatus = 	xTaskCreate(vtask_blink, 
													"blink", 
													TASK_BLINK_STACK_SIZE, 
													qlist, 					/* command queue to blink task */
													TASK_BLINK_PRIORITY, 
													NULL);
	configASSERT(xstatus == pdPASS);
	
	/* create controller task */
	xstatus = 	xTaskCreate(vtask_controller,
													"controller",
													TASK_CONTROLLER_STACK_SIZE,
													qlist,								/* list of command queues to each tasks */				
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
	
	/* mount SD card */
	if (f_mount(&fs,"", 0	) != FR_OK) {
		debug_print("f_mount failed\r\n");
	}
	
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
	uint8_t*									buff;
	FRESULT										result;
	
	/* initlizie file and stream buffer */
	sbuff = (struct stream_buff *)pvPortMalloc(sizeof(struct stream_buff));
	configASSERT(sbuff);
	result = f_open(&sbuff->file, "Timber.mp3", FA_READ);
	configASSERT(result == FR_OK);
	
	/* initlizie lwrb */
	buff  = (uint8_t *)pvPortMalloc(STREAM_BUFF_SIZE);
	configASSERT(buff);
	lwrb_init(&sbuff->lwrb, buff, STREAM_BUFF_SIZE);
	
	/* main loop */
	for(;;) {
		/* create SD card data stream buffer */
		qcmd.cmd = CMD_SDCARD_START_READ;
		qcmd.arg = (uintptr_t) sbuff;
		xQueueSend(qlist->sdcard, &qcmd, 0);
		
		/* start playing mp3 file */
		qcmd.cmd = CMD_VS10XX_PLAY;
		qcmd.arg = (uintptr_t) sbuff;
		xQueueSend(qlist->vs10xx, &qcmd, 0);
		
		/* start playing mp3 file */
		qcmd.cmd = CMD_LED_BLINK_SET;
		qcmd.arg = 500;
		xQueueSend(pqlist->blink, &qcmd, 0);
		
		vTaskDelay(pdMS_TO_TICKS(5110));
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
	
	/* main loop */
	for(;;) {
		xQueueReceive(qlist->blink, &blink_cmd, portMAX_DELAY);
		debug_print("cmd: %02x, arg: %02x\r\n", blink_cmd.cmd, blink_cmd.arg);
	} /* for(;;) */
}

/**
 * \brief VS1063A controller	
 */
void vtask_vs10xx(void* vparameters)
{
	struct controller_qlist* 	pqlist 	= vparameters;	/* command queue */
	struct mp3p_cmd						vs10xx_cmd;
	
	/* main loop */
	for(;;) {
		xQueueReceive(qlist->vs10xx, &vs10xx_cmd, portMAX_DELAY);
		debug_print("cmd: %02x, arg: %p\r\n", vs10xx_cmd.cmd, (void *) vs10xx_cmd.arg);
	} /* for(;;) */
}

/**
 * \breif SD-card data streamer
 */
void vtask_sdcard(void* vparameters)
{	
	struct controller_qlist* 	pqlist 	= vparameters;	/* command queue */
	struct mp3p_cmd 					sd_cmd;
	
	/* main loop */
	for(;;) {
		xQueueReceive(qlist->sdcard, &sd_cmd, portMAX_DELAY);
		debug_print("cmd: %02x, arg: %p\r\n", sd_cmd.cmd, (void *) sd_cmd.arg);
	} /* for(;;) */
}
/* end of file */
