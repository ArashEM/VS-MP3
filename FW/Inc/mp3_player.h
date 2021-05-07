/*****************************************************************************
  * @file    mp3_player.h
  * @author  Arash Golgol
  * @brief   MP3 player main tasks and functions
*****************************************************************************/
#ifndef MP3_PLAYER_H
#define MP3_PLAYER_H

/* FreeRTOS headers */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* General headers */
#include <stdint.h>

void vsmp3_init(void *vparameters);
void vtask_vs1063(void* vparameters);
void vtask_sdcard(void* vparameters);
void vtask_controller(void* vparameters);
void vtask_blink(void* vparameters);

struct mp3p_cmd {
	uint32_t    cmd;   /* command */
	uintptr_t 	arg;   /* argument (can be void pointer too) */
};

struct sd_msg {
	const char*     path;  /* file path to be streamed */ 
	QueueHandle_t		queue; /* FreeRTOS queue object for holding streamed data */
};

/* internal commands (used for cmd) */
enum {
	CMD_NOPE                 = 0x00,
	CMD_LED_BLINK_SET        = 0x01,
};

/* tasks priorities */
enum {
	TASK_BLINK_PRIORITY      = tskIDLE_PRIORITY,
	TASK_SDCARD_PRIORITY     = tskIDLE_PRIORITY + 2,
	TASK_VS1063_PRIORITY     = tskIDLE_PRIORITY + 3,
	TASK_LCD_PRIORITY        = tskIDLE_PRIORITY + 1,
	TASK_CONTROLLER_PRIORITY = tskIDLE_PRIORITY + 1,
};

/* tasks stack size */
enum {
	TASK_BLINK_STACK_SIZE      = configMINIMAL_STACK_SIZE,
  TASK_SDCARD_STACK_SIZE     = configMINIMAL_STACK_SIZE,
  TASK_VS1063_STACK_SIZE     = configMINIMAL_STACK_SIZE,
  TASK_LCD_STACK_SIZE        = configMINIMAL_STACK_SIZE,
  TASK_CONTROLLER_STACK_SIZE = configMINIMAL_STACK_SIZE,
};

#endif /* MP3_PLAYER_H */
