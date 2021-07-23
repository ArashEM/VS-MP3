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
#include "timers.h"

/* General headers */
#include <stdint.h>
#include <stdio.h>
#include "fatfs.h"
#include "lwrb/lwrb.h"

void vsmp3_init(void *vparameters);
void vtask_vs10xx(void* vparameters);
void vtask_sdcard(void* vparameters);
void vtask_controller(void* vparameters);
void vtask_blink(void* vparameters);
void vtask_hmi(void* vparameters);

void vtimer_backlight(TimerHandle_t xTimer);

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
	QueueHandle_t		hmi;
};

struct stream_buff {
	FIL							file;  		/* FatFS file object to be streamed */ 
	lwrb_t					lwrb; 		/* Ring Buffer for holding streamed data */
	QueueHandle_t		qwrite;		/* queue handle for write tasks */
};

/* internal commands (used for cmd) */
enum {
	/* general commands */
	CMD_NOPE,
	CMD_LED_BLINK_SET,      		/* arg is value for blink delay in mS */
	CMD_LED_BLINK_OFF,					/* are is not used */
	
	/* vs10xx related commands */
	CMD_VS10XX_PLAY,						/* arg is pointer to struct stream_buff */		
	CMD_VS10XX_STOP,				
	CMD_VS10XX_SET_VOL,					/* arg is value for volume */
	
	/* sd card related commands */
	CMD_SDCARD_START_READ,			/* arg is pointer to struct stream_buff */
	CMD_SDCARD_CONT_READ,
	CMD_SDCARD_STOP_READ,
	
	/* hmi related commands */
	CMD_HMI_SET_MSG,						/* arg is char * which point to file name */
};

/* HW queue values */
enum {
	/* Keys */
	HW_KEY1_PRESSED,
	HW_KEY2_PRESSED,
	HW_KEY3_PRESSED,
	HW_KEY4_PRESSED,
};

/* tasks priorities */
enum {
	TASK_BLINK_PRIORITY      = tskIDLE_PRIORITY,
	TASK_SDCARD_PRIORITY     = tskIDLE_PRIORITY + 3,
	TASK_VS10XX_PRIORITY     = tskIDLE_PRIORITY + 2,
	TASK_HMI_PRIORITY        = tskIDLE_PRIORITY + 1,
	TASK_CONTROLLER_PRIORITY = tskIDLE_PRIORITY + 4,
};

/* tasks stack size */
enum {
	TASK_BLINK_STACK_SIZE      = configMINIMAL_STACK_SIZE,
  TASK_SDCARD_STACK_SIZE     = configMINIMAL_STACK_SIZE,
  TASK_VS10XX_STACK_SIZE     = configMINIMAL_STACK_SIZE,
  TASK_HMI_STACK_SIZE        = configMINIMAL_STACK_SIZE * 2,
  TASK_CONTROLLER_STACK_SIZE = configMINIMAL_STACK_SIZE * 3,
};

/* constants and macro */
#define STREAM_BUFF_SIZE			4096
#define STREAM_BUFF_HALF_SIZE	( STREAM_BUFF_SIZE ) / 2
#define INIT_VOLUME						0x30

/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:	the pointer to the member.
 * @type:	the type of the container struct this is embedded in.
 * @member:	the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member)	\
	((type *)((char *)(ptr) - offsetof(type, member)));



#endif /* MP3_PLAYER_H */
