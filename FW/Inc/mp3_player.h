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
#include "fatfs.h"
#include "lwrb/lwrb.h"

void vsmp3_init(void *vparameters);
void vtask_vs10xx(void* vparameters);
void vtask_sdcard(void* vparameters);
void vtask_controller(void* vparameters);
void vtask_blink(void* vparameters);

static void sd_buff_evt_fn(lwrb_t* buff, lwrb_evt_type_t type, size_t len);

/**
 * \brief mp3 player command 
 */
struct mp3p_cmd {
	uint32_t    cmd;   /* command */
	uintptr_t 	arg;   /* argument (can be void pointer too) */
};

/**
 * \berif controller needs to access to command queue of other tasks
 */
struct controller_qlist {
	QueueHandle_t		blink;
	QueueHandle_t		vs10xx;
	QueueHandle_t		sdcard;
};

struct stream_buff {
	FIL			file;  /* FatFS file object to be streamed */ 
	lwrb_t	lwrb; /* Ring Buffer for holding streamed data */
};

/* internal commands (used for cmd) */
enum {
	/* general commands */
	CMD_NOPE,
	CMD_LED_BLINK_SET,      		/* arg is value for blink delay in mS */
	
	/* vs10xx related commands */
	CMD_VS10XX_PLAY,						/* arg is pointer to struct stream_buff */		
	CMD_VS10XX_STOP,				
	CMD_VS10XX_SET_VOL,					/* arg is value for volume */
	
	/* sd card related commands */
	CMD_SDCARD_START_READ,			/* arg is pointer to struct stream_buff */
	CMD_SDCARD_CONT_READ,
	CMD_SDCARD_STOP_READ,
};

/* tasks priorities */
enum {
	TASK_BLINK_PRIORITY      = tskIDLE_PRIORITY,
	TASK_SDCARD_PRIORITY     = tskIDLE_PRIORITY + 3,
	TASK_VS10XX_PRIORITY     = tskIDLE_PRIORITY + 2,
	TASK_LCD_PRIORITY        = tskIDLE_PRIORITY + 1,
	TASK_CONTROLLER_PRIORITY = tskIDLE_PRIORITY + 4,
};

/* tasks stack size */
enum {
	TASK_BLINK_STACK_SIZE      = configMINIMAL_STACK_SIZE,
  TASK_SDCARD_STACK_SIZE     = configMINIMAL_STACK_SIZE,
  TASK_VS10XX_STACK_SIZE     = configMINIMAL_STACK_SIZE,
  TASK_LCD_STACK_SIZE        = configMINIMAL_STACK_SIZE,
  TASK_CONTROLLER_STACK_SIZE = configMINIMAL_STACK_SIZE,
};

/* constants and macro */
#define STREAM_BUFF_SIZE			1000
#define STREAM_BUFF_HALF_SIZE	(STREAM_BUFF_SIZE)/2

#endif /* MP3_PLAYER_H */
