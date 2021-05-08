/*****************************************************************************
  * @file    mp3_player.c
  * @author  Arash Golgol
  * @brief   MP3 player main tasks and functions
*****************************************************************************/
/* FreeRTOS headers */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* HAL headers */
#include "main.h"

/* General headers */
#include <stdint.h>
#include "mp3_player.h"
#include "GUI.h"

/**
 * \brief initilize mp3-player and creat related tasks  	
 */
void vsmp3_init(void *vparameters)
{
	struct controller_qlist *qlist;	
	
	qlist = 
	(struct controller_qlist *)pvPortMalloc(sizeof(struct controller_qlist));
	
	/* create task command queue */
	qlist->blink = xQueueCreate(1, sizeof(struct mp3p_cmd));
	/* vs10xx and sdcard tasks need more depth */
	qlist->vs10xx = xQueueCreate(3, sizeof(struct mp3p_cmd));
	qlist->sdcard = xQueueCreate(3, sizeof(struct mp3p_cmd));
	
	/* create blink tasks */
	xTaskCreate(vtask_blink, 
	            "blink", 
	            TASK_BLINK_STACK_SIZE, 
	            qlist->blink, 					/* command queue to blink task */
	            TASK_BLINK_PRIORITY, 
	            NULL);
	
	/* create vs10xx task */
	xTaskCreate(vtask_vs10xx,
							"vs10xx",
							TASK_VS10XX_STACK_SIZE,
							qlist->vs10xx,
							TASK_VS10XX_PRIORITY,
							NULL);
							
	/* create sdcard streaming buffer task */
	xTaskCreate(vtask_sdcard,
							"sdcard",
							TASK_SDCARD_STACK_SIZE,
							qlist->sdcard,
							TASK_SDCARD_PRIORITY,
							NULL);
	
	/* create controller task */
	xTaskCreate(vtask_controller,
	            "controller",
	            TASK_CONTROLLER_STACK_SIZE,
							qlist,								/* list of command queues to each tasks */				
							TASK_CONTROLLER_PRIORITY, 
							NULL);
	
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
	struct mp3p_cmd						led_cmd = { 
															.cmd = CMD_LED_BLINK_SET,
															.arg = 100, 
	};	
	
	/**
	 * sending command to vtask_blink()
	 */
	for(;;) {
		/* check if there is a command and queue pointer */
		if(qlist == NULL) {	
			/* TODO: if there is no command queue, what should we do? */
			continue;
		}
		/* set blink delay */
		xQueueSend(qlist->blink, &led_cmd, 0);  
		
		led_cmd.arg = led_cmd.arg + 100;
		vTaskDelay(pdMS_TO_TICKS(5000));
	}
}


/**
 * \brief blink led 	
 */
void vtask_blink(void* vparameters)
{
	QueueHandle_t 	xqueue_blink = vparameters;  /* command queue */
	struct mp3p_cmd led_cmd;
	BaseType_t 			delay = 500;    /*if no command is send ever */
	BaseType_t 			xstatus;
	
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
					GUI_DispDecAt(delay, 10,220,4);
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
	QueueHandle_t 			xqueue_vs10xx = vparameters;  /* command queue */
	struct mp3p_cmd 		vs_cmd;
	struct stream_buff*	sbuff;
	BaseType_t 					xstatus;
	
	/**
	 *	if(vs_cmd.cmd = CMD_VS10XX_PLAY)
	 *		sbuff = (struct stream_buff*)vs_cmd.arg
	 *    xQueueReceive(sbuff->queue, ...)
	 */
}

/**
 * \breif SD-card data streamer
 */
void vtask_sdcard(void* vparameters)
{
	QueueHandle_t 			xqueue_sdcard = vparameters;  /* command queue */
	struct mp3p_cmd 		sd_cmd;
	struct stream_buff*	sbuff;
	BaseType_t 					xstatus;
	
	/**
	 * 	if(vs_cmd.cmd = CMD_VS10XX_PLAY)
	 *  sbuff = (struct stream_buff*)vs_cmd.arg
	 *  f_read(sbuff->file,...)
	 *  xQueueSend(sbuff->queue, sbuff->file, ...)
	 */
	
	for(;;) {
		if (xqueue_sdcard != NULL) {
			xstatus = xQueueReceive(xqueue_sdcard, &sd_cmd, 0);
			if (xstatus == pdPASS) {
			}
		}
	}
	
}
