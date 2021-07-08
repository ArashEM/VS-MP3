/*****************************************************************************
  * @file    helper.h
  * @author  Arash Golgol
  * @brief   Helpers for MP3 player
*****************************************************************************/
#ifndef _HELPER_H_
#define _HELPER_H_

/* General headers */
#include <stdint.h>
#include "lwrb/lwrb.h"
#include "mp3_player.h"

/* FreeRTOS headers */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


/* private functions */
static void sd_buff_evt_fn(lwrb_t* buff, lwrb_evt_type_t type, size_t len);

/* prototypes */
struct stream_buff*	sbuff_alloc(QueueHandle_t qw);
void start_playing(	struct stream_buff* sbuff, 
										struct controller_qlist* qlist,
										const char* file);
void stop_playing(	struct stream_buff* sbuff, 
										struct controller_qlist* qlist);
void set_volume(struct controller_qlist* qlist, const uint8_t vol);

struct controller_qlist* vsmp3_create_queues(void);
void vsmp3_create_tasks(struct controller_qlist* qlist);

/* macro */
#define is_eof(sbuff) f_eof(&(sbuff)->file)

#endif /*_HELPER_H_*/
	
