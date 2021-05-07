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
	QueueHandle_t 	xqueue_blink;	
	
	/* create blink task command queue  with only one command */
	xqueue_blink = xQueueCreate(1, sizeof(struct mp3p_cmd));
	
	/* create blink tasks */
	xTaskCreate(vtask_blink, 
	            "blink", 
	            TASK_BLINK_STACK_SIZE, 
	            xqueue_blink, 
	            TASK_BLINK_PRIORITY, 
	            NULL);
	
	/* create controller task */
	xTaskCreate(vtask_controller,
	            "controller",
	            TASK_CONTROLLER_STACK_SIZE,
							xqueue_blink,
							TASK_CONTROLLER_PRIORITY, 
							NULL);
	
	/* Start the scheduler. */
	vTaskStartScheduler();

	/* Will only get here if there was not enough heap space to create the
	   idle task. */
}

void vtask_controller(void* vparameters)
{
	QueueHandle_t 	xqueue_blink = vparameters;	/* command queue */
	struct mp3p_cmd	led_cmd = { 
		.cmd = CMD_LED_BLINK_SET,
		.arg = 100,
	};	
	
	/**
	 * sending command to vtask_blink()
	 */
	for(;;) {
		/* check if there is a command and queue pointer */
		if(xqueue_blink == NULL) {		
			continue;
		}
		/* set blink delay */
		xQueueSend(xqueue_blink, &led_cmd, 0);  
		
		led_cmd.arg = led_cmd.arg + 100;
		vTaskDelay(pdMS_TO_TICKS(5000));
	}
}

void vtask_blink(void* vparameters)
{
	QueueHandle_t xqueue_blink = vparameters;  /* command queue */
	struct mp3p_cmd led_cmd;
	BaseType_t delay = 500;    /*if no command is send ever */
	BaseType_t xstatus;
	
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
