/*****************************************************************************
  * @file    helper.c
  * @author  Arash Golgol
  * @brief   Helpers for MP3 player
*****************************************************************************/
/* FreeRTOS headers */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* HAL headers */

/* General headers */
#include <stdint.h>
#include "helper.h"
#include "mp3_player.h"
#include "vs10xx.h"
#include "debug.h"

/* Global variables */
static uint8_t								buff[STREAM_BUFF_SIZE];
#if (configUSE_TRACE_FACILITY == 1)
extern traceString 						sd_chn;	
#endif


/**
 * \brief           buffer event function
 */
static void sd_buff_evt_fn(lwrb_t* buff, lwrb_evt_type_t type, size_t len) {	
	struct stream_buff*		sbuff;
	struct mp3p_cmd 			sd_cmd;
	
	sbuff = container_of(buff, struct stream_buff, lwrb);
	
	switch (type) {
		case LWRB_EVT_RESET:
			break;
		
		case LWRB_EVT_READ:
#if (configUSE_TRACE_FACILITY == 1)
			vTracePrintF(sd_chn, "Buffer read event: %d byte(s)!\r\n", (int)len);
#endif
			if (lwrb_get_full(buff) < STREAM_BUFF_HALF_SIZE ) {
				/* we need more data in stream buffer */
				sd_cmd.cmd = CMD_SDCARD_CONT_READ;
				sd_cmd.arg = (uintptr_t) sbuff;
				xQueueSend(sbuff->qwrite, &sd_cmd, 0);
			}	 /* if (lwrb_get_full(buff) < STREAM_BUFF_HALF_SIZE ) */
		break;
    
		case LWRB_EVT_WRITE:
#if (configUSE_TRACE_FACILITY == 1)
			vTracePrintF(sd_chn, "Buffer write event: %d byte(s)!\r\n", (int)len);
#endif
		break;
    
		default: break;
	}
}

/**
 *	\brief	allocate stream buffer for playing 
 */
struct stream_buff*	sbuff_alloc(QueueHandle_t qw)
{
	struct stream_buff*				sbuff;
	
	/* initlizie file and stream buffer */
	sbuff = (struct stream_buff *)pvPortMalloc(sizeof(struct stream_buff));
	configASSERT(sbuff);
	sbuff->qwrite = qw;
	
	/* initlizie lwrb */
	//buff  = (uint8_t *)pvPortMalloc(STREAM_BUFF_SIZE);
	configASSERT(buff);
	lwrb_init(&sbuff->lwrb, buff, STREAM_BUFF_SIZE);
	lwrb_set_evt_fn(&sbuff->lwrb, sd_buff_evt_fn);
	
	return sbuff;
}

/**
 * \brief send required commands to start playing 
 */
void start_playing(	struct stream_buff* sbuff, 
										struct controller_qlist* qlist, 
										const char* file)
{
	struct mp3p_cmd						qcmd;
	FRESULT										result;
	
	/* controll input */
	configASSERT(sbuff);
	configASSERT(qlist);
	configASSERT(file);
	
	result = f_open(&sbuff->file, file, FA_READ);
	debug_print("f_open: %d\r\n",result);
	configASSERT(result == FR_OK);
	
	/* create SD card data stream buffer */
	qcmd.cmd = CMD_SDCARD_START_READ;
	qcmd.arg = (uintptr_t) sbuff;
	xQueueSend(qlist->sdcard, &qcmd, 0);
	
	/* start playing mp3 file */
	qcmd.cmd = CMD_VS10XX_PLAY;
	qcmd.arg = (uintptr_t) sbuff;
	xQueueSend(qlist->vs10xx, &qcmd, 0);
	
	/* blink while playing */
	qcmd.cmd = CMD_LED_BLINK_SET;
	qcmd.arg = 1000;
	xQueueSend(qlist->blink, &qcmd, 0);
	
	debug_print("start playing %s\r\n",file);
}

/**
 * \brief send required commands to stop palying
 */
void stop_playing(struct stream_buff* sbuff, struct controller_qlist* qlist)
{
	struct mp3p_cmd						qcmd;
	
	/* controll input */
	configASSERT(sbuff);
	configASSERT(qlist);
	
	/* create SD card data stream buffer */
	qcmd.cmd = CMD_SDCARD_STOP_READ;
	qcmd.arg = (uintptr_t) sbuff;
	xQueueSend(qlist->sdcard, &qcmd, 0);
	
	/* start playing mp3 file */
	qcmd.cmd = CMD_VS10XX_STOP;
	qcmd.arg = (uintptr_t) sbuff;
	xQueueSend(qlist->vs10xx, &qcmd, 0);
	
	/* blink while playing */
	qcmd.cmd = CMD_LED_BLINK_SET;
	qcmd.arg = portMAX_DELAY;
	xQueueSend(qlist->blink, &qcmd, 0);
	
	debug_print("stop playing\r\n");
}
		
/* EOF */
